#include <iostream>
#include <chrono>
#include "CacheManager.h"
#include "LRU.h"
#include "LFU.h"
/******************************************************************************
*Initialization of static
******************************************************************************/
std::shared_ptr<CacheManager> CacheManager::instance = nullptr;
/******************************************************************************
*Constructor
******************************************************************************/
CacheManager::CacheManager() {
  if(ConfigurationManager::getInstance()->CURRENT_POLICY == LRUPolicy){
    CacheManager::policy =
        std::static_pointer_cast<LRU>(std::shared_ptr<LRU>(new LRU()));
  }
  else if(ConfigurationManager::getInstance()->CURRENT_POLICY == LFUPolicy){
    CacheManager::policy =
        std::static_pointer_cast<LFU>(std::shared_ptr<LFU>(new LFU()));
  }
  else {
    fprintf(stderr, "Caching policy is empty. Select LRU or LFU\n");
  }
}
/******************************************************************************
*Destructor
******************************************************************************/
CacheManager::~CacheManager() {}
/******************************************************************************
*Functions
******************************************************************************/
int CacheManager::isCached(std::shared_ptr<Key> &key) {
  return policy->isCached(key);
}

int CacheManager::addToCache(std::shared_ptr<Key> &key) {
  return policy->addToCache(key);
}





