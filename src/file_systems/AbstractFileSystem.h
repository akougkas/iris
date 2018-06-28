/*******************************************************************************
* File AbstractFileSystem.h
*
* Goal:
*
* Created: December 10th, 2016  - Anthony Kougkas
 * Updated:
* Illinois Institute of Technology - SCS Lab
* (C) 2016
******************************************************************************/
#ifndef IRIS_ABSTRACTFILESYSTEM_H
#define IRIS_ABSTRACTFILESYSTEM_H
/******************************************************************************
*include files
******************************************************************************/
#include "../config/Components.h"
/******************************************************************************
*Class
******************************************************************************/
class AbstractFileSystem {
public:
/******************************************************************************
*Interface
******************************************************************************/
  virtual int read(std::string containerName, VirtualObject &object) = 0;
  virtual int write(std::string containerName, const VirtualObject &object) = 0;
  virtual void flush(){}
};

#endif //IRIS_ABSTRACTFILESYSTEM_H



