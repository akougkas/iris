/*******************************************************************************
* File S3MapperNaive.h
*
* Goal: This is the S3/Swift mapper
*
* Created: December 10th, 2016  - Anthony Kougkas
 * Updated:
* Illinois Institute of Technology - SCS Lab
* (C) 2016
******************************************************************************/
#ifndef IRIS_S3MAPPERNAIVE_H
#define IRIS_S3MAPPERNAIVE_H
/******************************************************************************
*include files
******************************************************************************/
#include "S3Mapper.h"
/******************************************************************************
*Class
******************************************************************************/
class S3MapperNaive : public S3Mapper {
public:
/******************************************************************************
*Interface
******************************************************************************/
  virtual std::pair<std::string, VirtualObject>
  mapObject(std::string objectName, std::size_t objectSize,
            operation operationType) override;
};
#endif //IRIS_S3MAPPERNAIVE_H




