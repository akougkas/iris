/******************************************************************************
*include files
******************************************************************************/
#include <iostream>
#include "POSIXMapperPutOptimised.h"

/******************************************************************************
*Initialization of static members
******************************************************************************/
std::shared_ptr<POSIXMapperPutOptimised> POSIXMapperPutOptimised::instance = nullptr;
/******************************************************************************
*Interface
******************************************************************************/
std::vector<std::shared_ptr<Key>>
POSIXMapperPutOptimised::generateKeys(const char *name, long int offset,
                                      size_t size, operation operationType) {
  std::vector<std::shared_ptr<Key>> keys;
  switch(operationType){
    case PUT:
      keys=generateKeysForPut(name,offset,size);
      break;
    case GET:
      keys=generateKeysForGet(name,offset,size);
      break;
  }
  return keys;
}
/******************************************************************************
*Functions
******************************************************************************/
std::vector<std::shared_ptr<Key>>
POSIXMapperPutOptimised::generateKeysForPut(const char *name, long offset,
                                                             size_t size) {
#ifdef TIMER_M
  Timer timer = Timer(); timer.startTime();
#endif
  std::vector<std::shared_ptr<Key>> keys;
  std::shared_ptr<Key> key = std::shared_ptr<Key>(new Key());
  std::size_t keySize = strlen(name)+
      sizeof(char) +
      sizeof(offset)+
      sizeof(char)+
      sizeof(size);
  key->name = new char[keySize];
  std::sprintf((char*)key->name, "%s_%ld_%ld", name, offset,offset+size);
  key->offset=0;
  key->size=size;
  Range range=Range();
  range.max=offset+size;
  range.min=(size_t)offset;
  range.keyName=key->name;
  keySet.insert(range);
#ifdef DEBUG
  std::cout << "keyName : " << key.name << "\tOffset : " << key.offset
     <<"\tData Size : " << key.size << std::endl;
#endif/*DEBUG*/
#ifdef TIMER_M
  timer.endTime(__FUNCTION__);
#endif
  keys.push_back(std::move(key));
  return keys;
}

std::vector<std::shared_ptr<Key>>
POSIXMapperPutOptimised::generateKeysForGet(const char *name, long offset,
                                                             size_t size) {
#ifdef TIMER_M
  Timer timer = Timer(); timer.startTime();
#endif
  std::vector<std::shared_ptr<Key>> keys;
  Range range=Range();
  range.max=offset+size;
  range.min=(size_t )offset;
  std::vector<Range> emptyBounds=std::vector<Range>();
  emptyBounds.emplace_back(range);
  size_t remaining=size;
  for(auto rangeInKeySet : keySet){
    if(remaining<=0){
      break;
    }
    for(auto j=emptyBounds.begin();j!=emptyBounds.end();++j){
      range=*j;
      if(rangeInKeySet.contains(range)){
        std::pair<size_t,size_t> bounds=range.getContainedPortion(rangeInKeySet);
        std::shared_ptr<Key> key = std::shared_ptr<Key>(new Key());
        key->name=rangeInKeySet.keyName.c_str();
        key->offset=bounds.first;
        key->size=bounds.second-rangeInKeySet.min-bounds.first;
        remaining-=key->size;

        emptyBounds.erase(j);
        if(bounds.first!=0) {
          Range prev;
          prev.min = rangeInKeySet.min;
          prev.max = rangeInKeySet.min + bounds.first;
          emptyBounds.emplace_back(prev);
        }
        if(bounds.second!=rangeInKeySet.max) {
          Range latter;
          latter.min = bounds.second;
          latter.max = rangeInKeySet.max;
          emptyBounds.emplace_back(latter);
        }
        keys.push_back(std::move(key));
#ifdef DEBUG
        std::cout << "keyName : " << key.name << "\tOffset : " << key.offset
     <<"\tData Size : " << key.size << std::endl;
#endif/*DEBUG*/
        break;
      }
    }
  }
#ifdef TIMER_M
  timer.endTime(__FUNCTION__);
#endif
  return keys;
}


