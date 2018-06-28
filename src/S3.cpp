/******************************************************************************
*include files
******************************************************************************/
#include <mpi.h>
#include "S3.h"
#include "file_systems/PVFS2Client.h"

/******************************************************************************
*Interface operations
******************************************************************************/
void* iris::get(std::string objectName) {
#ifdef TIMER
  Timer timer = Timer(); timer.startTime();
#endif
  auto apiInstance = API::getInstance();
  auto s3Mapper = std::static_pointer_cast<S3Mapper>
      (apiInstance->getMapperFactory()->
          getMapper(ConfigurationManager::getInstance()->getS3_MAPPER()));
  auto pvfs2Client = std::static_pointer_cast<PVFS2Client>
      (apiInstance->getFileSystemFactory()->
          getFileSystem(ConfigurationManager::getInstance()->getPVFS2_CLIENT()));

  auto virtualObjectPair=s3Mapper->mapObject(objectName,0,operation::GET);

  pvfs2Client->read(virtualObjectPair.first,virtualObjectPair.second);
#ifdef TIMER
  timer.endTime(__FUNCTION__);
#endif
  return virtualObjectPair.second.data;
}

void iris::put(std::string objectName, void *data, size_t objectSize) {
#ifdef TIMER
  Timer timer = Timer(); timer.startTime();
#endif
  auto apiInstance = API::getInstance();
  auto s3Mapper = std::static_pointer_cast<S3Mapper>
      (apiInstance->getMapperFactory()->
          getMapper(ConfigurationManager::getInstance()->getS3_MAPPER()));
  auto pvfs2Client = std::static_pointer_cast<PVFS2Client>
      (apiInstance->getFileSystemFactory()->
          getFileSystem(ConfigurationManager::getInstance()->getPVFS2_CLIENT()));
  std::pair<std::string, VirtualObject> virtualObjectPair;
  try{
    virtualObjectPair=s3Mapper->mapObject(objectName,
                                               objectSize,
                                               operation::PUT);

    if(virtualObjectPair.second.linkedObjects.empty()){
      virtualObjectPair.second.data=malloc(virtualObjectPair.second.size);
      std::memcpy(virtualObjectPair.second.data,data,virtualObjectPair.second
          .size);
    }else{
      virtualObjectPair.second.data=malloc(virtualObjectPair.second.size);
      std::memcpy(virtualObjectPair.second.data,data,virtualObjectPair.second
          .size);
      std::size_t ptr = virtualObjectPair.second.size;
      for(auto &linkedObject : virtualObjectPair.second.linkedObjects){
        linkedObject.data = malloc(linkedObject.size);
        std::memcpy(linkedObject.data,(void*)((char*)data+ptr),linkedObject.size);
        ptr += linkedObject.size;
      }
    }
    pvfs2Client->write(virtualObjectPair.first,virtualObjectPair.second);
  }catch (const std::exception& xcp) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::cout << "Exception with failed: " << xcp.what()
              << "\nRank:" << rank << "Size:" <<virtualObjectPair.second.size
        << "Data:" << (char*)data << "\n";
  }

#ifdef TIMER
  timer.endTime(__FUNCTION__);
#endif
}

void iris::remove(std::string objectName) {}

void iris::flushData(){
#ifdef TIMER
  Timer timer = Timer(); timer.startTime();
#endif
  auto apiInstance = API::getInstance();
  auto pvfs2Client = std::static_pointer_cast<PVFS2Client>
      (apiInstance->getFileSystemFactory()->
          getFileSystem(ConfigurationManager::getInstance()->getPVFS2_CLIENT()));
  pvfs2Client->flush();
#ifdef TIMER
  timer.endTime(__FUNCTION__);
#endif
}
