//
// Created by anthony on 12/10/16.
//


#include "hdf5_hl.h"
#include "HDF5.h"

hid_t H5Fopen(const char *name, unsigned flags, hid_t fapl_id) {
  void *file_ptr;
  int file_size;
  hid_t fileId = -1;
  auto apiInstance = API::getInstance();
  auto objectStoreClient = std::static_pointer_cast<HyperdexClient>
      (apiInstance->getObjectStoreFactory()->getObjectStore
          (ConfigurationManager::getInstance()->HYPERDEX_CLIENT));
  std::shared_ptr<Key> key = std::shared_ptr<Key>(new Key());
  key->name = name;
  if (fileId < 0) {
    int status = objectStoreClient->get(key);
    if (status == OPERATION_SUCCESSFUL) {
      /*fileId = H5LTopen_file_image(key->data, (size_t) key->size,
                                   H5LT_FILE_IMAGE_DONT_RELEASE);*/
      if (fileId > 0) {
        auto hdf5MetadataManager = std::static_pointer_cast<HDF5MetadataManager>
            (apiInstance->getMetadataManagerFactory()->
                getMetadataManager(HDF5_METADATA_MANAGER));
        hdf5MetadataManager->updateFileName(fileId, name);
      }
      key.reset();
      return fileId;
    } else {
      MAP_OR_FAIL(H5Fopen);
      return __real_H5Fopen(name, flags, fapl_id);
    }
  }
  return fileId;
}

hid_t H5Fcreate(const char *name, unsigned flags, hid_t fcpl_id, hid_t fapl_id) {
  hid_t fapl_id2 = H5Pcreate(H5P_FILE_ACCESS);
  herr_t error = H5Pset_fapl_core(fapl_id2, 1024, 0);
  MAP_OR_FAIL(H5Fcreate);
  hid_t fileId = __real_H5Fcreate(name, flags, fcpl_id, fapl_id2);
  auto apiInstance = API::getInstance();
  auto hdf5MetadataManager = std::static_pointer_cast<HDF5MetadataManager>
      (apiInstance->getMetadataManagerFactory()->
          getMetadataManager(HDF5_METADATA_MANAGER));
  hdf5MetadataManager->updateFileName(fileId, name);
  return fileId;
}

hid_t H5Screate_simple(int rank, const hsize_t *current_dims, const hsize_t *maximum_dims) {
  MAP_OR_FAIL(H5Screate_simple);
  hid_t dataspace_Id = __real_H5Screate_simple(rank, current_dims, maximum_dims);
  return dataspace_Id;
}

hid_t
H5Dcreate2(hid_t fileId, const char *name, hid_t dtype_id, hid_t space_id, hid_t lcpl_id, hid_t dcpl_id,
                      hid_t dapl_id) {
  MAP_OR_FAIL(H5Dcreate2);
  hid_t dataset_id = __real_H5Dcreate2(fileId, name, dtype_id, space_id, lcpl_id,
                                       dcpl_id, dapl_id);
  auto apiInstance = API::getInstance();
  auto hdf5MetadataManager = std::static_pointer_cast<HDF5MetadataManager>
      (apiInstance->getMetadataManagerFactory()->
          getMetadataManager(HDF5_METADATA_MANAGER));
  hdf5MetadataManager->updateDataset(dataset_id, fileId);
  H5Fflush(fileId, H5F_SCOPE_LOCAL);
  return dataset_id;
}

herr_t
H5Dwrite(hid_t dataset_id, hid_t mem_type_id, hid_t mem_space_id, hid_t file_space_id, hid_t xfer_plist_id,
                    void *buf) {
  auto apiInstance = API::getInstance();
  auto hdf5MetadataManager = std::static_pointer_cast<HDF5MetadataManager>
      (apiInstance->getMetadataManagerFactory()->
          getMetadataManager(HDF5_METADATA_MANAGER));
  auto hdf5Mapper = std::static_pointer_cast<HDF5Mapper>
      (apiInstance->getMapperFactory()->getMapper(ConfigurationManager::getInstance()->getHDF5_MAPPER()));
  auto objectStoreClient = std::static_pointer_cast<HyperdexClient>
      (apiInstance->getObjectStoreFactory()->getObjectStore(ConfigurationManager::getInstance()->HYPERDEX_CLIENT));
  auto cacheManager = apiInstance->getCacheManager();
  std::string filename = hdf5MetadataManager->getFileNameFromDataset(dataset_id);
  if (ConfigurationManager::getInstance()->FILE_MAPPING_MODE == BASIC) {
    MAP_OR_FAIL(H5Dwrite);
    herr_t error = __real_H5Dwrite(dataset_id, mem_type_id, mem_space_id, file_space_id, xfer_plist_id, buf);
    free(buf);
    std::shared_ptr<Key> key = std::shared_ptr<Key>(new Key());
    key->name = filename.c_str();
    ssize_t buf_size;
    char *buf_ptr;
    hid_t fileId=hdf5MetadataManager->getFileIdFromDataset(dataset_id);
    buf_size = H5Fget_file_image(fileId, NULL, 0);
    buf_ptr = (char *) malloc((size_t) buf_size);
    buf_size = H5Fget_file_image(fileId, buf_ptr, (size_t) buf_size);
    key->size = (size_t) buf_size;
    key->data = buf_ptr;
    key->offset = 0;
    int status = objectStoreClient->put(key);
    key.reset();
    if(buf_ptr) delete(buf_ptr);
    return error;
  } else {
    auto keys = hdf5Mapper->generateKeys(filename, dataset_id, mem_type_id, mem_space_id, file_space_id, xfer_plist_id);
    std::size_t bufferIndex = 0;
    for (auto &&key : keys) {
      key->data = malloc(key->size);
      memcpy(key->data, (char *) buf + bufferIndex, key->size);
      bufferIndex += key->size;
      objectStoreClient->put(key);
      if (ConfigurationManager::getInstance()->CACHING_MODE == ON) cacheManager->addToCache(key);
      key.reset();
    }
  }
  return H5Eclear1();
}

herr_t
H5Dread(hid_t dataset_id, hid_t mem_type_id, hid_t mem_space_id, hid_t file_space_id, hid_t xfer_plist_id,
                   void *buf) {
  auto apiInstance = API::getInstance();
  auto hdf5MetadataManager = std::static_pointer_cast<HDF5MetadataManager>
      (apiInstance->getMetadataManagerFactory()->
          getMetadataManager(HDF5_METADATA_MANAGER));
  auto hdf5Mapper = std::static_pointer_cast<HDF5Mapper>
      (apiInstance->getMapperFactory()->getMapper(ConfigurationManager::getInstance()->getHDF5_MAPPER()));
  auto objectStoreClient = std::static_pointer_cast<HyperdexClient>
      (apiInstance->getObjectStoreFactory()->getObjectStore(ConfigurationManager::getInstance()->HYPERDEX_CLIENT));
  auto cacheManager = apiInstance->getCacheManager();
  std::string filename = hdf5MetadataManager->getFileNameFromDataset(dataset_id);
  if (ConfigurationManager::getInstance()->FILE_MAPPING_MODE == BASIC) {
    MAP_OR_FAIL(H5Dread);
    herr_t error = __real_H5Dread(dataset_id, mem_type_id, mem_space_id, file_space_id, xfer_plist_id, buf);
    return error;
  } else {
    auto keys = hdf5Mapper->generateKeys(filename, dataset_id, mem_type_id, mem_space_id, file_space_id, xfer_plist_id);
    Buffer buffer = Buffer(buf);
    size_t bufferIndex = 0;
    for (auto &&key : keys) {
      auto originalKeySize = key->size;
      int status = 0;
      if (cacheManager->isCached(key) == NO_CACHED_DATA_FOUND)
        status = objectStoreClient->get(key);
      if (status == OPERATION_SUCCESSFUL) {
        buffer.update(key->data, bufferIndex,
                      originalKeySize > strlen((char *) key->data) ? strlen(
                          (char *) key->data) : originalKeySize);
      } else {
        //TODO:throw error for get
      }
      bufferIndex += originalKeySize;
      key.reset();
    }
  }
  return H5Eclear1();
}

herr_t H5Dclose(hid_t dataset_id) {
  auto apiInstance = API::getInstance();
  auto hdf5MetadataManager = std::static_pointer_cast<HDF5MetadataManager>
      (apiInstance->getMetadataManagerFactory()->
          getMetadataManager(HDF5_METADATA_MANAGER));
  hdf5MetadataManager->deleteDataset(dataset_id);
  MAP_OR_FAIL(H5Dclose);
  return __real_H5Dclose(dataset_id);
}

herr_t H5Sclose(hid_t space_id) {
  MAP_OR_FAIL(H5Sclose);
  return __real_H5Sclose(space_id);
}

herr_t H5Fclose(hid_t file_id) {
  auto apiInstance = API::getInstance();
  auto hdf5MetadataManager = std::static_pointer_cast<HDF5MetadataManager>
      (apiInstance->getMetadataManagerFactory()->
          getMetadataManager(HDF5_METADATA_MANAGER));
  std::string fileName = hdf5MetadataManager->getFileNameFromFile(file_id);
  auto objectStoreClient = std::static_pointer_cast<HyperdexClient>
      (apiInstance->getObjectStoreFactory()->getObjectStore(ConfigurationManager::getInstance()->HYPERDEX_CLIENT));
  std::shared_ptr<Key> key = std::shared_ptr<Key>(new Key());
  key->name = fileName.c_str();
  ssize_t buf_size;
  char *buf_ptr;
  buf_size = H5Fget_file_image(file_id, NULL, 0);
  buf_ptr = (char *) malloc((size_t) buf_size);
  buf_size = H5Fget_file_image(file_id, buf_ptr, (size_t) buf_size);
  key->size = (size_t) buf_size;
  key->data = buf_ptr;
  key->offset = 0;
  int status = objectStoreClient->put(key);
  if(buf_ptr) delete(buf_ptr);
  MAP_OR_FAIL(H5Fclose);
  return __real_H5Fclose(file_id);
}

hid_t H5Dopen2(hid_t loc_id, const char *name, hid_t dapl_id) {
  auto apiInstance = API::getInstance();
  MAP_OR_FAIL(H5Dopen2);
  hid_t datasetId = __real_H5Dopen2(loc_id, name, dapl_id);
  auto hdf5MetadataManager = std::static_pointer_cast<HDF5MetadataManager>
      (apiInstance->getMetadataManagerFactory()->
          getMetadataManager(HDF5_METADATA_MANAGER));
  hdf5MetadataManager->updateDataset(datasetId, loc_id);
  return datasetId;
}

