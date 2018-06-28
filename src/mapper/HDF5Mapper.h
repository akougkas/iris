//
// Created by anthony on 12/10/16.
//

#ifndef IRIS_HDF5MAPPER_H
#define IRIS_HDF5MAPPER_H


#include <vector>
#include "AbstractMapper.h"
#include <hdf5.h>
#include "../config/constants.h"
#include "../config/Components.h"

class HDF5Mapper: public AbstractMapper {
public:
  std::vector<std::shared_ptr<Key>>
  generateKeys(std::string fileName,hid_t dataset_id, hid_t mem_type_id, hid_t mem_space_id, hid_t file_space_id, hid_t xfer_plist_id);
};


#endif //IRIS_HDF5MAPPER_H
