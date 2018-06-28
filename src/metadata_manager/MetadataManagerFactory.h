/*******************************************************************************
* File MetadataManagerFactory.h
*
* Goal:
*
* Created: December 10th, 2016  - Anthony Kougkas
 * Updated:
* Illinois Institute of Technology - SCS Lab
* (C) 2016
******************************************************************************/
#ifndef IRIS_METADATAMANAGERFACTORY_H
#define IRIS_METADATAMANAGERFACTORY_H
/******************************************************************************
*include files
******************************************************************************/
#include <memory>
#include <unordered_map>
#include "IrisMetadataManager.h"
/******************************************************************************
*Class
******************************************************************************/
class MetadataManagerFactory {
private:
/******************************************************************************
*Variables and members
******************************************************************************/
  static std::shared_ptr<MetadataManagerFactory> instance;
  std::unordered_map<std::string, std::shared_ptr<IrisMetadataManager>>
      metadataManagerMap;
/******************************************************************************
*Constructor
******************************************************************************/
  MetadataManagerFactory():metadataManagerMap(){}
public:
/******************************************************************************
*Getters and setters
******************************************************************************/
  inline static std::shared_ptr<MetadataManagerFactory> getInstance(){
    return instance == nullptr ? instance = std::shared_ptr<MetadataManagerFactory>
                                         (new MetadataManagerFactory())
                               : instance;
  }
  std::shared_ptr<IrisMetadataManager> getMetadataManager(const std::string name);
/******************************************************************************
*Destructor
******************************************************************************/
  virtual ~MetadataManagerFactory(){}
};


#endif //IRIS_METADATAMANAGERFACTORY_H
