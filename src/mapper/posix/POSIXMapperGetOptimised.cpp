/******************************************************************************
*include files
******************************************************************************/
#include <iostream>
#include "POSIXMapperGetOptimised.h"

/******************************************************************************
*Initialization of static members
******************************************************************************/
std::shared_ptr<POSIXMapperGetOptimised> POSIXMapperGetOptimised::instance=nullptr;
/******************************************************************************
*Interface
******************************************************************************/
std::vector<std::shared_ptr<Key>>
POSIXMapperGetOptimised::generateKeys(const char *name, long int offset,
                                      size_t size,
                                      operation operationType) {
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
POSIXMapperGetOptimised::generateKeysForPut(const char *name, long offset,
                                                             size_t size) {
  std::vector<std::shared_ptr<Key>> keys;
  std::shared_ptr<Key> key = std::shared_ptr<Key>(new Key());
  Range range=Range();
  range.min=(size_t )offset;
  range.max=offset+size;
  std::size_t keySize = strlen(name)+
                        sizeof(char) +
                        sizeof(offset);
  key->name = new char[keySize];
  std::sprintf((char*)key->name, "%s_%d", name, 0);
  range.keyName=key->name;
  auto keyIterator=keyRange.find(range);
  if(keyIterator==keyRange.end()){
    key->offset=0;
    key->size=size;
  }else{
    auto bounds=range.getContainedPortion(*keyIterator);
    key->offset=bounds.first;
    key->size=size;
    range.min=keyIterator->min<range.min?keyIterator->min:range.min;
    range.max=keyIterator->max>range.max?keyIterator->max:range.max;
    keyRange.erase(keyIterator);
  }


  keyRange.insert(range);
  keys.push_back(std::move(key));
#ifdef DEBUG
  std::cout << "keyName : " << key->name << "\tOffset : " << key->offset
     <<"\tData Size : " << key->size << std::endl;
#endif/*DEBUG*/
  return keys;
}

std::vector<std::shared_ptr<Key>>
POSIXMapperGetOptimised::generateKeysForGet(const char *name, long offset,
                                                             size_t size) {
#ifdef TIMER_M
  Timer timer = Timer(); timer.startTime();
#endif
  std::vector<std::shared_ptr<Key>> keys;
  Range range=Range();
  range.min=(size_t )offset;
  range.max=offset+size;
  std::shared_ptr<Key> key = std::shared_ptr<Key>(new Key());
  std::size_t keySize = strlen(name)+
                        sizeof(char) +
                        sizeof(offset);
  key->name = new char[keySize];
  std::sprintf((char*)key->name, "%s_%d", name, 0);
  range.keyName=key->name;
  auto keyIterator=keyRange.find(range);
  if(keyIterator==keyRange.end()){
    //TODO:throw error
  }else{
    auto bounds=range.getContainedPortion(*keyIterator);

    key->name=keyIterator->keyName.c_str();
    key->offset=bounds.first;
    key->size=bounds.second-bounds.first;
    keys.push_back(std::move(key));
#ifdef DEBUG
    std::cout << "keyName : " << key->name << "\tOffset : " << key->offset
              <<"\tData Size : " << key->size << std::endl;
#endif/*DEBUG*/
  }
#ifdef TIMER_M
  timer.endTime(__FUNCTION__);
#endif
  return keys;
}