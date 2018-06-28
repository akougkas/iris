/******************************************************************************
*include files
******************************************************************************/
#include "PVFS2Client.h"
#include "../utils/tools/Buffer.h"

/******************************************************************************
*Initialization of static members
******************************************************************************/
std::shared_ptr<PVFS2Client> PVFS2Client::instance = nullptr;
/******************************************************************************
*Functions
******************************************************************************/
PVFS2Client::ContainerStat PVFS2Client::open(std::string containerName) {
  auto fileHandlerIterator = createdFiles.find(containerName);
  if(fileHandlerIterator == createdFiles.end()){
    //we create the file and add to map
    FILE * fh = std::fopen((ConfigurationManager::getInstance()->
        WORKING_DIRECTORY + containerName).c_str(), "w+");
    seek(fh,0);
    ContainerStat stat = {true, fh};
    createdFiles.emplace(containerName, stat);
    return stat;
  }
  else if(!fileHandlerIterator->second.opened){
    FILE * fh = std::fopen((ConfigurationManager::getInstance()->
        WORKING_DIRECTORY + containerName).c_str(), "r+");
    seek(fh,0);
    fileHandlerIterator->second.opened = true;
    fileHandlerIterator->second.fh = fh;
  }
  return fileHandlerIterator->second;
}

int PVFS2Client::close(std::string containerName) {
  auto fileHandlerIterator= createdFiles.find(containerName);
  if(fileHandlerIterator==createdFiles.end()){
    return FILENAME_DOES_NOT_EXIST;
  }else{
    std::fclose(fileHandlerIterator->second.fh);
    fileHandlerIterator->second.opened=false;
    fileHandlerIterator->second.fh= nullptr;
  }
  return OPERATION_SUCCESSFUL;
}

int PVFS2Client::seek(FILE *fh, size_t offset) {
  if(std::fseek(fh,offset,SEEK_SET)) return FILE_SEEK_FAILED;
  return OPERATION_SUCCESSFUL;
}
/******************************************************************************
*Interface
******************************************************************************/
int PVFS2Client::read(std::string containerName, VirtualObject &object) {
#ifdef TIMER_I
  Timer timer = Timer(); timer.startTime();
#endif
  auto file= open(containerName);
  if(file.opened){
    seek(file.fh, object.containerOffset);
    if(object.size == 0){
      std::fseek(file.fh, 0, SEEK_END);
      long fileSize = ftell(file.fh);
      std::fseek(file.fh, 0, SEEK_SET);
      object.size= (size_t) fileSize;
    }
    Buffer buffer=Buffer(object.size);
    size_t readBytes=std::fread(buffer.data(),sizeof(char),object.size,file.fh);
    if(readBytes!=object.size) {
      return FILE_READ_FAILED;
    }
    for(auto linkedObject:object.linkedObjects){
      std::fseek(file.fh,linkedObject.containerOffset,SEEK_SET);
      readBytes = std::fread(linkedObject.data,sizeof(char),linkedObject
                                    .size,
                            file.fh);
      buffer.append(linkedObject.data,linkedObject.size);
    }
    object.data=buffer.data();
  }
  close(containerName);
#ifdef TIMER_I
  timer.endTime(__FUNCTION__);
#endif
  return OPERATION_SUCCESSFUL;
}

int PVFS2Client::write(std::string containerName, const VirtualObject &object) {
#ifdef TIMER_I
  Timer timer = Timer(); timer.startTime();
#endif
  auto file= open(containerName);
  if(file.opened){
    seek(file.fh, object.containerOffset);
    size_t writeBytes=std::fwrite(object.data,sizeof(char),object.size,file.fh);
    if(writeBytes!=object.size) {
      return FILE_WRITE_FAILED;
    }
    for(auto linkedObject:object.linkedObjects){
      std::fseek(file.fh,linkedObject.containerOffset,SEEK_SET);
      std::fwrite(linkedObject.data,sizeof(char),linkedObject.size,file.fh);
    }
  }
  close(containerName);
#ifdef TIMER_I
  timer.endTime(__FUNCTION__);
#endif
  return OPERATION_SUCCESSFUL;
}

