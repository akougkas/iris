/******************************************************************************
*include files
******************************************************************************/
#include <mpi.h>
#include "S3Mapper.h"

/******************************************************************************
*Initialization of static members
******************************************************************************/
std::shared_ptr<S3Mapper> S3Mapper::instance = nullptr;
/******************************************************************************
*Interface
******************************************************************************/
std::pair<std::string, VirtualObject>
S3Mapper::mapObject(std::string objectName, std::size_t objectSize,
                    operation operationType) {
#ifdef TIMER_M
  Timer timer = Timer(); timer.startTime();
#endif
  std::pair<std::string, VirtualObject> location;
  if (operationType == GET_OPERATION) {
    location = mapObjectForGet(objectName, objectSize);
  } else {
    location = mapObjectForPut(objectName, objectSize);
  }
#ifdef TIMER_M
  timer.endTime(__FUNCTION__);
#endif
  return location;
}
/******************************************************************************
*Functions
******************************************************************************/
std::size_t S3Mapper::hashKey(std::string objectName, std::size_t objectSize) {
  auto keyIterator = existingHashValues.find(objectName);
  std::size_t hash;
  if (keyIterator != existingHashValues.end()) {
    hash = keyIterator->second;
  }else if (currentContainerSize + objectSize <=
      ConfigurationManager::getInstance()->CONTAINER_MAX_SIZE
            && currentHashValue != 0) {
    existingHashValues.insert(std::make_pair(objectName,currentHashValue));
    return currentHashValue;
  }
  else {
    //hash = CityHash32(objectName.c_str(), strlen(objectName.c_str()));
    hash = ++currentHashValue;
    existingHashValues.insert(std::make_pair(objectName,hash));
  }
  return hash;
}

std::pair<std::string, VirtualObject>
S3Mapper::mapObjectForGet(std::string objectName, std::size_t objectSize) {
  std::size_t hashValue = hashKey(objectName,objectSize);
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  std::string filename = "container_" + std::to_string(rank) + "_"
                         + std::to_string(hashValue)
                         + ".dat";
  auto containerIterator = containerList.find(filename);
  if(containerIterator == containerList.end()){
    return std::pair<std::string, VirtualObject>();
  }
  else{
    auto objectIterator = containerIterator->second->objects.find(objectName);
    if(objectIterator == containerIterator->second->objects.end()){
      return std::pair<std::string, VirtualObject>();
    }
    return std::make_pair(containerIterator->second->containerName,
                          *objectIterator->second);
  }
}

std::pair<std::string, VirtualObject>
S3Mapper::mapObjectForPut(std::string objectName, std::size_t objectSize) {
  std::pair<std::string, VirtualObject> location;
  std::size_t hashValue = hashKey(objectName,objectSize);
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  std::string filename = "container_" + std::to_string(rank) + "_"
                         + std::to_string(hashValue)
                         + ".dat";
  VirtualObject* object = new VirtualObject();
  auto containerIterator = containerList.find(filename);
  Container* container;
  if(containerIterator == containerList.end()){
    container = new Container();
    container->containerName = filename;
    container->size = objectSize;
    container->filePointer = objectSize;
    object->name = objectName;
    object->size = objectSize;
    object->containerOffset = 0;
    container->objects.insert({object->name,object});
    containerList.insert({filename, container});
    location = {filename, *object};
    currentContainerSize=container->size;
  }
  else{
    object->name = objectName;
    object->size = objectSize;
    object->containerOffset = containerIterator->second->filePointer;
    auto objectIterator = containerIterator->second->objects.find(objectName);
    if(objectIterator != containerIterator->second->objects.end()){
      containerIterator->second->invalidObjects.emplace(objectName,objectIterator->second);
      containerIterator->second->size = containerIterator->second->size - objectIterator->second->size;
      containerIterator->second->objects.erase(objectIterator);
    }
    containerIterator->second->size+=object->size;
    containerIterator->second->objects.emplace(object->name,object);
    containerIterator->second->filePointer = containerIterator->second->filePointer + objectSize;
    containerList.erase(containerIterator);
    containerList.emplace(filename, containerIterator->second);
    location = {filename, *object};
    currentContainerSize=containerIterator->second->size;
  }
  currentHashValue=hashValue;
  return location;
}