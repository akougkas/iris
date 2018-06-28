/*******************************************************************************
* File S3Mapper.h
*
* Goal: This is the S3/Swift mapper
*
* Created: December 10th, 2016  - Anthony Kougkas
 * Updated:
* Illinois Institute of Technology - SCS Lab
* (C) 2016
******************************************************************************/
#ifndef IRIS_S3MAPPER_H
#define IRIS_S3MAPPER_H
/******************************************************************************
*include files
******************************************************************************/
#include <vector>
#include <unordered_map>
#include <memory>
#include "../AbstractMapper.h"
#include "../../config/Components.h"
/******************************************************************************
*Class
******************************************************************************/
class S3Mapper: public AbstractMapper {
protected:
/******************************************************************************
*Variables and members
******************************************************************************/
  static std::shared_ptr<S3Mapper> instance;
  std::unordered_map<std::string,Container*> containerList;
  std::unordered_map<std::string, std::size_t> existingHashValues;
  std::size_t currentContainerSize;
  std::size_t currentHashValue;
/******************************************************************************
*Functions
******************************************************************************/
  virtual size_t hashKey(std::string objectName, size_t objectSize);

  virtual std::pair<std::string, VirtualObject>
  mapObjectForGet(std::string objectName, std::size_t objectSize);

  virtual std::pair<std::string, VirtualObject>
  mapObjectForPut(std::string objectName, std::size_t objectSize);

/******************************************************************************
*Constructor
******************************************************************************/
  S3Mapper() :currentContainerSize(0), currentHashValue(0),
              existingHashValues(),
              containerList(){
  };
public:
/******************************************************************************
*Getters and setters
******************************************************************************/
  static std::shared_ptr<S3Mapper> getInstance() {
    return instance == nullptr
           ? instance = std::shared_ptr<S3Mapper>(new S3Mapper())
           : instance;
  }
/******************************************************************************
*Interface
******************************************************************************/
  virtual std::pair<std::string, VirtualObject>
  mapObject(std::string objectName, std::size_t objectSize,
            operation operationType);
/******************************************************************************
*Destructor
******************************************************************************/
  virtual ~S3Mapper(){}
};

#endif //IRIS_S3MAPPER_H
