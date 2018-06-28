#include "API.h"
/******************************************************************************
*Initialization of static
******************************************************************************/
std::shared_ptr<API> API::instance = nullptr;
/******************************************************************************
*Constructor
******************************************************************************/
API::API() {
  cacheManager = CacheManager::getInstance();
  prefetcherFactory = PrefetcherFactory::getInstance();
  metadataManagerFactory = MetadataManagerFactory::getInstance();
  mapperFactory = MapperFactory::getInstance();
  objectStoreFactory=ObjectStoreFactory::getInstance();
  fileSystemFactory=FileSystemFactory::getInstance();
}
/******************************************************************************
*Destructor
******************************************************************************/
API::~API() {
  cacheManager.reset();
  prefetcherFactory.reset();
  metadataManagerFactory.reset();
  mapperFactory.reset();
  objectStoreFactory.reset();
  fileSystemFactory.reset();
}












