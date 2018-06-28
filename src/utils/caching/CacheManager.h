/*******************************************************************************
* File CacheManager.h
*
* Goal:
*
* Created: December 10th, 2016  - Anthony Kougkas
 * Updated:
* Illinois Institute of Technology - SCS Lab
* (C) 2016
******************************************************************************/
#ifndef IRIS_CACHEMANAGER_H
#define IRIS_CACHEMANAGER_H
/******************************************************************************
*include files
******************************************************************************/
#include <memory>
#include <vector>
#include <unordered_map>
#include <list>
#include "../../config/constants.h"
#include "../tools/Buffer.h"
#include "AbstractReplacementPolicy.h"
/******************************************************************************
*Class
******************************************************************************/
class CacheManager {
private:
/******************************************************************************
*Variables and members
******************************************************************************/
  static std::shared_ptr<CacheManager> instance;
  std::shared_ptr<AbstractReplacementPolicy> policy;
/******************************************************************************
*Constructor
******************************************************************************/
  CacheManager();
public:
/******************************************************************************
*Getters and setters
******************************************************************************/
  inline static std::shared_ptr<CacheManager> getInstance(){
    return instance== nullptr ? instance=std::shared_ptr<CacheManager>
        (new CacheManager()): instance;
  }
/******************************************************************************
*Interface
******************************************************************************/
  int isCached(std::shared_ptr<Key> &key);
  int addToCache(std::shared_ptr<Key> &key);
/******************************************************************************
*Destructor
******************************************************************************/
  virtual ~CacheManager();
};


#endif //IRIS_CACHEMANAGER_H
