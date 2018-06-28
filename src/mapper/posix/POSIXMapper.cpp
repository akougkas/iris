/******************************************************************************
*include files
******************************************************************************/
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include "POSIXMapper.h"
#include "POSIXMapperGetOptimised.h"

/******************************************************************************
*Functions
******************************************************************************/
std::vector<std::shared_ptr<Key>>
POSIXMapper::generateKeys(const char *name, long int offset, size_t size,
                          operation operationType) {
#ifdef TIMER_M
  Timer timer = Timer(); timer.startTime();
#endif
  std::vector<std::shared_ptr<Key>> keys;
  std::size_t baseKey =
      (size_t) (offset / ConfigurationManager::getInstance()->MAX_OBJ_SIZE);
  std::size_t remainingOperationSize=size;
  int i=0;
  std::size_t keySize = strlen(name)+ sizeof(char) + sizeof(baseKey);

  while(remainingOperationSize!=0) {
    std::shared_ptr<Key> key = std::shared_ptr<Key>(new Key());
    key->name = new char[keySize];
    std::size_t baseOffset = ((baseKey + i) *
    ConfigurationManager::getInstance()->MAX_OBJ_SIZE);
    std::sprintf((char*)key->name, "%s_%ld", name, (baseKey + i));
    if (offset == baseOffset) {
      key->offset = 0;
      if (remainingOperationSize < ConfigurationManager::getInstance()->MAX_OBJ_SIZE) {
        key->size = remainingOperationSize;
        remainingOperationSize =0;
      } else {
        key->size = ConfigurationManager::getInstance()->MAX_OBJ_SIZE;
        remainingOperationSize -= ConfigurationManager::getInstance()->MAX_OBJ_SIZE;
      }
      offset+=key->size;
    }else {
      key->offset = offset - baseOffset;
      key->size = (baseOffset+ConfigurationManager::getInstance()->MAX_OBJ_SIZE)-offset<size
                  ?(baseOffset+ConfigurationManager::getInstance()->MAX_OBJ_SIZE)-offset
                  :size;
      remainingOperationSize -= key->size;
      offset += key->size;
    }
    ++i;
    keys.push_back(std::move(key));
#ifdef DEBUG2
    std::cout << "keyName : " << key.name << "\tOffset : " << key.offset
     <<"\tData Size : " << key.size << std::endl;
#endif
  }
#ifdef TIMER_M
  timer.endTime(__FUNCTION__);
#endif
  return keys;
}