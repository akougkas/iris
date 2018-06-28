/*******************************************************************************
* File AbstractObjectStore.h
*
* Goal:
*
* Created: December 10th, 2016  - Anthony Kougkas
 * Updated:
* Illinois Institute of Technology - SCS Lab
* (C) 2016
******************************************************************************/
#ifndef IRIS_ABSTRACTOBJECTSTORE_H
#define IRIS_ABSTRACTOBJECTSTORE_H
/******************************************************************************
*include files
******************************************************************************/
#include <memory>
#include "../config/Components.h"
/******************************************************************************
*Class
******************************************************************************/
class AbstractObjectStore {
public:
/******************************************************************************
*Interface
******************************************************************************/
  virtual int get(std::shared_ptr<Key> &key) = 0;
  virtual int put(std::shared_ptr<Key> &key) = 0;
  virtual int remove(std::shared_ptr<Key> &key) = 0;
};

#endif //IRIS_ABSTRACTOBJECTSTORE_H
