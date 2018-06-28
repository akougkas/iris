/*******************************************************************************
* File ObjectStorePrefetcher.h
*
* Goal:
*
* Created: December 10th, 2016  - Anthony Kougkas
 * Updated:
* Illinois Institute of Technology - SCS Lab
* (C) 2016
******************************************************************************/
#ifndef IRIS_OBJECTSTOREPREFETCHER_H
#define IRIS_OBJECTSTOREPREFETCHER_H
/******************************************************************************
*include files
******************************************************************************/
#include "AbstractPrefetcher.h"
#include "../../config/constants.h"
#include "../../object_stores/HyperdexClient.h"
#include "../../object_stores/ObjectStoreFactory.h"
#include "../../mapper/MapperFactory.h"
#include "../caching/CacheManager.h"
#include <vector>
/******************************************************************************
*Class
******************************************************************************/
class ObjectStorePrefetcher: public AbstractPrefetcher{
private:
/******************************************************************************
*Variables and members
******************************************************************************/
  std::shared_ptr<ObjectStoreFactory> objectStoreFactory;
  std::shared_ptr<CacheManager> cacheManager;
  std::shared_ptr<MapperFactory> mapperFactory;
/******************************************************************************
*Functions
******************************************************************************/
  int engine(const char * fileName, long int &fileOffset, size_t &operationSize,
             long int fileSize, int prefetchingMode);
public:
/******************************************************************************
*Constructor
******************************************************************************/
  ObjectStorePrefetcher(){
    objectStoreFactory=ObjectStoreFactory::getInstance();
    cacheManager=CacheManager::getInstance();
    mapperFactory=MapperFactory::getInstance();
 }
/******************************************************************************
*Destructor
******************************************************************************/
  virtual ~ObjectStorePrefetcher(){}
/******************************************************************************
*Interface
******************************************************************************/
  int fetch(const char *fileName, long int fileOffset, size_t operationSize,
            long int fileSize);
  int fetchKey(std::shared_ptr<Key> key);
  int fetchKeys(std::vector<std::shared_ptr<Key>> keys);
};


#endif //IRIS_OBJECTSTOREPREFETCHER_H
