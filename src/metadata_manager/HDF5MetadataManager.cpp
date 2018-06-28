
//
// Created by anthony on 12/10/16.
//

#include "HDF5MetadataManager.h"
#include "../config/return_codes.h"

int HDF5MetadataManager::updateFileName(hid_t fileId, std::string fileName) {
  auto fileIterator=fileToFileNameMap.find(fileId);
  if(fileIterator != fileToFileNameMap.end()) fileToFileNameMap.erase(fileIterator);
  fileToFileNameMap.insert(std::make_pair(fileId,fileName));
  return OPERATION_SUCCESSFUL;
}

int HDF5MetadataManager::updateDataset(hid_t datasetId, hid_t fileId) {
  auto datasetIterator=datasetToFileMap.find(datasetId);
  if(datasetIterator != datasetToFileMap.end()) datasetToFileMap.erase(datasetIterator);
  datasetToFileMap.insert(std::make_pair(datasetId,fileId));
  return OPERATION_SUCCESSFUL;
}

std::string HDF5MetadataManager::getFileNameFromDataset(hid_t datasetId) {
  auto datasetIterator = datasetToFileMap.find(datasetId);
  if(datasetIterator == datasetToFileMap.end()) return nullptr;
  else {
    auto fileIterator=fileToFileNameMap.find(datasetIterator->second);
    if(fileIterator == fileToFileNameMap.end()) return nullptr;
    else return fileIterator->second;
  }
}

hid_t HDF5MetadataManager::getFileIdFromDataset(hid_t datasetId) {
  auto datasetIterator = datasetToFileMap.find(datasetId);
  if(datasetIterator == datasetToFileMap.end()) return -1;
  else return datasetIterator->second;
}

std::string HDF5MetadataManager::getFileNameFromFile(hid_t fileId) {
  auto fileIterator=fileToFileNameMap.find(fileId);
  if(fileIterator == fileToFileNameMap.end()) return nullptr;
  else return fileIterator->second;
}

int HDF5MetadataManager::deleteFileName(hid_t fileId) {
  auto fileIterator=fileToFileNameMap.find(fileId);
  if(fileIterator != fileToFileNameMap.end()) fileToFileNameMap.erase(fileIterator);
  return OPERATION_SUCCESSFUL;
}

int HDF5MetadataManager::deleteDataset(hid_t datasetId) {
  auto datasetIterator=datasetToFileMap.find(datasetId);
  if(datasetIterator != datasetToFileMap.end()) datasetToFileMap.erase(datasetIterator);
  return OPERATION_SUCCESSFUL;
}
