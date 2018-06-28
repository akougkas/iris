//
// Created by anthony on 12/10/16.
//

#ifndef IRIS_HDF5METADATAMANAGER_H
#define IRIS_HDF5METADATAMANAGER_H


#include <hdf5.h>
#include <string>
#include <unordered_map>
#include "IrisMetadataManager.h"

class HDF5MetadataManager: public IrisMetadataManager {

  std::unordered_map<hid_t,std::string> fileToFileNameMap;
  std::unordered_map<hid_t,hid_t> datasetToFileMap;
public:
  HDF5MetadataManager() : fileToFileNameMap(), datasetToFileMap() {}
  int updateFileName(hid_t,std::string);
  int updateDataset(hid_t,hid_t);
  int deleteFileName(hid_t fileId);
  int deleteDataset(hid_t datasetId);
  std::string getFileNameFromDataset(hid_t datasetId);
  std::string getFileNameFromFile(hid_t fileId);

  hid_t getFileIdFromDataset(hid_t datasetId);
};


#endif //IRIS_HDF5METADATAMANAGER_H
