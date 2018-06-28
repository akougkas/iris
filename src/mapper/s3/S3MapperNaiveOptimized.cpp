/******************************************************************************
*include files
******************************************************************************/
#include "S3MapperNaiveOptimized.h"

/******************************************************************************
*Initialization of static members
******************************************************************************/
std::shared_ptr<S3MapperNaiveOptimized> S3MapperNaiveOptimized::instance=nullptr;
std::string S3MapperNaiveOptimized::CONTAINER_NAME = "container_naive_opt.dat";
/******************************************************************************
*Interface
******************************************************************************/
std::pair<std::string, VirtualObject>
S3MapperNaiveOptimized::mapObject(std::string objectName,
                                  std::size_t objectSize,
                                  operation operationType) {
#ifdef TIMER_M
  Timer timer = Timer(); timer.startTime();
#endif
  std::pair<std::string, VirtualObject> location;
  if(operationType == GET_OPERATION){
    location = mapObjectForGet(objectName, objectSize);
  }
  else {
    location = mapObjectForPut(objectName, objectSize);
  }
#ifdef TIMER_M
  timer.endTime(__FUNCTION__);
#endif
  return location;
}


std::pair<std::string, VirtualObject>
S3MapperNaiveOptimized::mapObjectForGet(std::string objectName,
                                        std::size_t objectSize) {
  auto objectIterator = keySpace.objects.find(objectName);
  if(objectIterator == keySpace.objects.end()){
    return std::pair<std::string, VirtualObject>();
  }
  return std::make_pair(keySpace.containerName,
                        *objectIterator->second);
}

std::pair<std::string, VirtualObject>
S3MapperNaiveOptimized::mapObjectForPut(std::string objectName,
                                        std::size_t objectSize) {
  VirtualObject* object = new VirtualObject();
  object->name = objectName;
  object->size = objectSize;
  object->containerOffset = keySpace.filePointer;
  auto objectIterator = keySpace.objects.find(objectName);
  if(objectIterator != keySpace.objects.end()){
    keySpace.size = keySpace.size - objectIterator->second->size;
  }
  keySpace.objects.erase(object->name);
  keySpace.objects.emplace(object->name, object);
  keySpace.filePointer = keySpace.filePointer +objectSize;
  keySpace.size = keySpace.size + objectSize;
  return std::make_pair(keySpace.containerName,*object);
}
