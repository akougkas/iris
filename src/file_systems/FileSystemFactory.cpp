/******************************************************************************
*include files
******************************************************************************/
#include <cstring>
#include <memory>
#include <iostream>
#include "FileSystemFactory.h"
#include "PVFS2Client.h"
#include "PVFS2ClientNonBlocking.h"
/******************************************************************************
*Initialization of static members
******************************************************************************/
std::shared_ptr<FileSystemFactory> FileSystemFactory::instance = nullptr;
/******************************************************************************
*Getters and setters
******************************************************************************/
std::shared_ptr<AbstractFileSystem>
FileSystemFactory::getFileSystem(std::string name) {
  auto iter = fileSystemMap.find(name);
  if(iter != fileSystemMap.end()) return iter->second;
  else {
    std::shared_ptr<AbstractFileSystem> fileSystemClient;
    if (name == PVFS2_CLIENT_BLOCKING) {
      fileSystemClient = PVFS2Client::getInstance();
    } else if (name == PVFS2_CLIENT_NON_BLOCKING) {
      fileSystemClient = PVFS2ClientNonBlocking::getInstance();
    }
    fileSystemMap.emplace(name, fileSystemClient);
    return fileSystemClient;
  }
}
