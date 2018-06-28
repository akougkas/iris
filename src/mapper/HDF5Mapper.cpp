//
// Created by anthony on 12/10/16.
//


#include "HDF5Mapper.h"

std::vector<std::shared_ptr<Key>>
HDF5Mapper::generateKeys(std::string fileName, hid_t dataset_id, hid_t mem_type_id, hid_t mem_space_id,
                         hid_t file_space_id, hid_t xfer_plist_id) {
#ifdef TIMER_M
  Timer timer = Timer(); timer.startTime();
#endif
  std::vector<std::shared_ptr<Key>> keys;
  ssize_t dataset_name_len = H5Iget_name(dataset_id, NULL, 0);
  char dataset_name[dataset_name_len];
  H5Iget_name(dataset_id, dataset_name, (size_t) (dataset_name_len + 1));
  hid_t dataspace_id = H5Dget_space(dataset_id);
  int rank = H5Sget_simple_extent_ndims(dataspace_id);
  hsize_t dims[rank];
  hsize_t max_dims[rank];
  H5Sget_simple_extent_dims(dataspace_id, dims, max_dims);
  size_t sizeOfElement = H5Tget_size(mem_type_id);
  hssize_t totalElements = H5Sget_simple_extent_npoints(dataspace_id);
  std::size_t keySize = strlen(fileName.c_str()) + strlen(dataset_name) + 2 * sizeof(char) + 2 * sizeof(size_t);
  hsize_t start[rank];
  hsize_t end[rank];
  herr_t status = H5Sget_select_bounds(dataspace_id, start, end);
  for (auto rankIndex = 0; rankIndex < rank; rankIndex++) {
    hsize_t elementsForRank = dims[rankIndex];
    hsize_t size = elementsForRank * sizeOfElement;
    std::size_t offset = 0;
    if (status > -1) {
      offset = start[rankIndex];
    }
    std::size_t baseKey = offset / ConfigurationManager::getInstance()->MAX_OBJ_SIZE;
    std::size_t remainingOperationSize = size;
    int i = 0;
    while (remainingOperationSize != 0) {
      std::shared_ptr<Key> key = std::shared_ptr<Key>(new Key());
      key->name = new char[keySize];
      std::size_t baseOffset = ((baseKey + i) * ConfigurationManager::getInstance()->MAX_OBJ_SIZE);
      snprintf((char *) key->name, keySize, "%s_%s_%d_%ld",fileName.c_str(), dataset_name, rankIndex, (baseKey + i));
      if (offset == baseOffset) {
        key->offset = 0;
        if (remainingOperationSize < ConfigurationManager::getInstance()->MAX_OBJ_SIZE) {
          key->size = remainingOperationSize;
          remainingOperationSize = 0;
        } else {
          key->size = ConfigurationManager::getInstance()->MAX_OBJ_SIZE;
          remainingOperationSize -= ConfigurationManager::getInstance()->MAX_OBJ_SIZE;
        }
        offset += key->size;
      } else {
        key->offset = offset - baseOffset;
        key->size = (baseOffset + ConfigurationManager::getInstance()->MAX_OBJ_SIZE) - offset < size ? (baseOffset + ConfigurationManager::getInstance()->MAX_OBJ_SIZE) - offset : size;
        remainingOperationSize -= key->size;
        offset += key->size;
      }
      ++i;
      keys.push_back(std::move(key));
    }
  }
#ifdef TIMER_M
  timer.endTime(__FUNCTION__);
#endif
  return keys;
}

