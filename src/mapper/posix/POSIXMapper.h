/*******************************************************************************
* File POSIXMapper.h
*
* Goal:
*
* Created: December 10th, 2016  - Anthony Kougkas
 * Updated:
* Illinois Institute of Technology - SCS Lab
* (C) 2016
******************************************************************************/
#ifndef IRIS_POSIXMAPPER_H
#define IRIS_POSIXMAPPER_H
/******************************************************************************
*include files
******************************************************************************/
#include <vector>
#include "../AbstractMapper.h"
#include "../../config/Components.h"
/******************************************************************************
*Class
******************************************************************************/
class POSIXMapper: public AbstractMapper {
public:
/******************************************************************************
*Functions
******************************************************************************/
  virtual std::vector<std::shared_ptr<Key>>
  generateKeys(const char *name, long int offset,
               size_t size,operation operationType);
};
#endif //IRIS_POSIXMAPPER_H
