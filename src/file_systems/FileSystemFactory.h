/*******************************************************************************
* File FileSystemFactory.h
*
* Goal:
*
* Created: December 10th, 2016  - Anthony Kougkas
 * Updated:
* Illinois Institute of Technology - SCS Lab
* (C) 2016
******************************************************************************/
#ifndef IRIS_FILESYSTEMFACTORY_H
#define IRIS_FILESYSTEMFACTORY_H
/******************************************************************************
*include files
******************************************************************************/
#include <memory>
#include "AbstractFileSystem.h"
/******************************************************************************
*Class
******************************************************************************/
class FileSystemFactory {
private:
/******************************************************************************
*Variables and members
******************************************************************************/
  static std::shared_ptr<FileSystemFactory> instance;
  std::unordered_map<std::string, std::shared_ptr<AbstractFileSystem>>
      fileSystemMap;
/******************************************************************************
*Constructors
******************************************************************************/
  FileSystemFactory(){}
public:
/******************************************************************************
*Getters and setters
******************************************************************************/
  static std::shared_ptr<FileSystemFactory> getInstance(){
    return FileSystemFactory::instance == nullptr
           ? instance = std::shared_ptr<FileSystemFactory>(new FileSystemFactory())
                               : instance;
  }
  std::shared_ptr<AbstractFileSystem> getFileSystem(const std::string name);
/******************************************************************************
*Destructor
******************************************************************************/
  virtual ~FileSystemFactory(){
    auto iter = fileSystemMap.find(PVFS2_CLIENT_NON_BLOCKING);
    if(iter != fileSystemMap.end()) iter->second.reset();
  }
};


#endif //IRIS_FILESYSTEMFACTORY_H


