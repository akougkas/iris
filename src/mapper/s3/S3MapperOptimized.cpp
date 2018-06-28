/******************************************************************************
*include files
******************************************************************************/
#include <mpi.h>
#include "../../utils/tools/city.h"
#include "S3MapperOptimized.h"

/******************************************************************************
*Initialization of static members
******************************************************************************/
std::shared_ptr<S3MapperOptimized> S3MapperOptimized::instance = nullptr;
/******************************************************************************
*Functions
******************************************************************************/
std::pair<std::string, VirtualObject>
S3MapperOptimized::mapObjectForGet(std::string objectName,
                                   std::size_t objectSize) {
  std::pair<std::string, VirtualObject> location;
  std::size_t hashValue = hashKey(objectName,objectSize);
  std::size_t containerIndex=hashValue/CONTAINER_COUNT;
  std::size_t objectIndex=hashValue%NUM_KEYS_IN_CONTAINER;
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  std::string filename = "container_" + std::to_string(rank) + "_"
                         + std::to_string(containerIndex)
                         + ".dat";
  if(containerTable[containerIndex] == nullptr){
    return std::pair<std::string, VirtualObject>();
  }else{
    Container* container = containerTable[containerIndex];
    if(container->objectTable[objectIndex] == nullptr){
      return std::pair<std::string, VirtualObject>();
    }
    return std::make_pair(container->containerName,
                          *container->objectTable[objectIndex]);
  }
}

std::pair<std::string, VirtualObject>
S3MapperOptimized::mapObjectForPut(std::string objectName,
                                   std::size_t objectSize) {
  std::pair<std::string, VirtualObject> location;
  std::size_t hashValue = hashKey(objectName,objectSize);
  std::size_t containerIndex=hashValue/CONTAINER_COUNT;
  std::size_t objectIndex=hashValue%NUM_KEYS_IN_CONTAINER;
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  std::string filename = "container_" + std::to_string(rank) + "_"
                         + std::to_string(containerIndex)
                         + ".dat";
  VirtualObject* object = new VirtualObject();
  Container* container;
  if(containerTable[containerIndex] == nullptr){
    //container is not there
    container = new Container();
    container->containerName = filename;
    container->size = objectSize;
    container->filePointer = objectSize;
    object->name=objectName;
    object->size=objectSize;
    object->containerOffset=0;
    container->objectTable[objectIndex]=object;
    location = {filename, *object};
    containerTable[containerIndex]=container;
  }else{
    container = containerTable[containerIndex];
    object->name=objectName;
    object->size=objectSize;
    object->containerOffset = container->filePointer;
    if(container->objectTable[objectIndex] != nullptr){
      //object is there
      object = container->objectTable[objectIndex];
      if(object->size<objectSize){
        if(object->linkedObjects.size()==0){
          //that is there no linked Objects so just append in end
          VirtualObject linkedVirtualObject=VirtualObject();
          linkedVirtualObject.name=objectName;
          linkedVirtualObject.containerOffset=container->filePointer;
          linkedVirtualObject.size=objectSize-object->size;
          object->linkedObjects.emplace_back(linkedVirtualObject);
        }else{
          //there are linked objects so try to first fill them up
          std::size_t remainingSize=objectSize-object->size;
          for(auto linkedVirtualObject:object->linkedObjects){
            remainingSize-=linkedVirtualObject.size;
            if(remainingSize<=0){
              break;
            }
          }
          //if still size is remaining then add a new object to list
          if(remainingSize>0){
            VirtualObject linkedVirtualObject=VirtualObject();
            linkedVirtualObject.name=objectName;
            linkedVirtualObject.containerOffset=container->filePointer;
            linkedVirtualObject.size=objectSize-remainingSize;
            object->linkedObjects.emplace_back(linkedVirtualObject);
          }
        }
      }
    }
    container->objectTable[objectIndex]=object;
    location = {filename, *object};
  }
  return location;
}

size_t S3MapperOptimized::hashKey(std::string objectName, size_t objectSize) {
  return CityHash32(objectName.c_str(), strlen(objectName.c_str()));
}