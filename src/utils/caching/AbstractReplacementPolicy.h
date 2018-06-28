/*******************************************************************************
* File AbstractReplacementPolicy.h
*
* Goal:
*
* Created: December 10th, 2016  - Anthony Kougkas
 * Updated:
* Illinois Institute of Technology - SCS Lab
* (C) 2016
******************************************************************************/
#ifndef IRIS_ABSTRACTREPLACEMENTPOLICY_H
#define IRIS_ABSTRACTREPLACEMENTPOLICY_H
/******************************************************************************
*include files
******************************************************************************/
#include <list>
#include <memory>
#include "../../config/Components.h"
/******************************************************************************
*Class
******************************************************************************/
class AbstractReplacementPolicy {
public:
/******************************************************************************
*Interface
******************************************************************************/
  virtual int isCached(std::shared_ptr<Key> &key)=0;
  virtual int addToCache(std::shared_ptr<Key> &key) = 0;
/******************************************************************************
*Constructor
******************************************************************************/
  AbstractReplacementPolicy(){}
/******************************************************************************
*Destructor
******************************************************************************/
  virtual ~AbstractReplacementPolicy() {}
};
#endif //IRIS_ABSTRACTREPLACEMENTPOLICY_H
