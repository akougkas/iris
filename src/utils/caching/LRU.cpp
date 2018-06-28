/******************************************************************************
*include files
******************************************************************************/
#include "LRU.h"

/******************************************************************************
*Interface
******************************************************************************/
int LRU::isCached(std::shared_ptr<Key> &key) {
#ifdef TIMER
  Timer timer = Timer(); timer.startTime();
#endif
  auto iterator = cacheMap.find(key->name);
  if(iterator == cacheMap.end()) return NO_CACHED_DATA_FOUND;
  refresh(iterator);
  key->data = iterator->second.first;
#ifdef TIMER
  timer.endTime(__FUNCTION__);
#endif
  return OPERATION_SUCCESSFUL;
}
int LRU::addToCache(std::shared_ptr<Key> &key) {
#ifdef TIMER
  Timer timer = Timer(); timer.startTime();
#endif
  auto cacheline = cacheMap.find(key->name);
  if (cacheline != cacheMap.end()){
    refresh(cacheline);
  }
  else {
    if (cacheMap.size()*ConfigurationManager::getInstance()->MAX_OBJ_SIZE >=
        ConfigurationManager::getInstance()->CACHE_CAPACITY) {
      cacheMap.erase(usedList.back());
      usedList.pop_back();
    }
    usedList.push_front(key->name);
  }
  cacheMap[key->name] = { key->data, usedList.begin() };
#ifdef TIMER
  timer.endTime(__FUNCTION__);
#endif
  return OPERATION_SUCCESSFUL;
}
/******************************************************************************
*Functions
******************************************************************************/
void LRU::refresh(HIPII::iterator cacheline) {
#ifdef TIMER
  Timer timer = Timer(); timer.startTime();
#endif
    std::string key = cacheline->first;
    usedList.erase(cacheline->second.second);
    usedList.push_front(key);
    cacheline->second.second = usedList.begin();
#ifdef TIMER
  timer.endTime(__FUNCTION__);
#endif
}




