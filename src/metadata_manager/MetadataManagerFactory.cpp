/******************************************************************************
*include files
******************************************************************************/
#include "MetadataManagerFactory.h"
#include "../config/constants.h"
#include "POSIXMetadataManager.h"
#include "HDF5MetadataManager.h"
#include "S3MetadataManager.h"
/******************************************************************************
*Initialization of static
******************************************************************************/
std::shared_ptr<MetadataManagerFactory> MetadataManagerFactory::instance = nullptr;
/******************************************************************************
*Getters and setters
******************************************************************************/
std::shared_ptr<IrisMetadataManager>
MetadataManagerFactory::getMetadataManager(const std::string name) {
  auto iter = metadataManagerMap.find(name);
  if(iter != metadataManagerMap.end()) return iter->second;
  else {
    std::shared_ptr<IrisMetadataManager> metadataManagerInstance;
    if (name == IRIS_METADATA_MANAGER) {
      metadataManagerInstance =
          std::shared_ptr<IrisMetadataManager>(new IrisMetadataManager());
    } else if (name == POSIX_METADATA_MANAGER) {
      metadataManagerInstance =
          std::shared_ptr<IrisMetadataManager>(new POSIXMetadataManager());
    } else if (name == HDF5_METADATA_MANAGER) {
      metadataManagerInstance =
          std::shared_ptr<IrisMetadataManager>(new HDF5MetadataManager());
    }else if (name == S3_METADATA_MANAGER) {
      metadataManagerInstance =
          std::shared_ptr<IrisMetadataManager>(new S3MetadataManager());
    } else return nullptr;
    metadataManagerMap.emplace(name,metadataManagerInstance);
    return metadataManagerInstance;
  }
}





