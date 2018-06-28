/*******************************************************************************
* File S3MapperOptimized.h
*
* Goal: This is the S3/Swift mapper
*
* Created: December 10th, 2016  - Anthony Kougkas
 * Updated:
* Illinois Institute of Technology - SCS Lab
* (C) 2016
******************************************************************************/
#ifndef IRIS_S3MAPPEROPTIMIZED_H
#define IRIS_S3MAPPEROPTIMIZED_H
/******************************************************************************
*include files
******************************************************************************/
#include <vector>
#include <unordered_map>
#include <memory>
#include "../../config/Components.h"
#include "S3Mapper.h"
#include "../../config/ConfigurationManager.h"
#include "../../config/constants.h"
/******************************************************************************
*Class
******************************************************************************/
class S3MapperOptimized: public S3Mapper {
protected:
/******************************************************************************
*Functions
******************************************************************************/
  std::pair<std::string, VirtualObject>
  mapObjectForGet(std::string objectName, std::size_t objectSize) override;

  std::pair<std::string, VirtualObject>
  mapObjectForPut(std::string objectName, std::size_t objectSize) override;

  size_t hashKey(std::string objectName, size_t objectSize) override;
private:
/******************************************************************************
*Variables and members
******************************************************************************/
  static std::shared_ptr<S3MapperOptimized> instance;
  std::array<Container*,CONTAINER_COUNT> containerTable;
/******************************************************************************
*Constructor
******************************************************************************/
  S3MapperOptimized() :containerTable() {}
public:
/******************************************************************************
*Getters and setters
******************************************************************************/
  static std::shared_ptr<S3MapperOptimized> getInstance() {
    return instance == nullptr ? instance = std::shared_ptr<S3MapperOptimized>
        (new S3MapperOptimized()) : instance;
  }
/******************************************************************************
*Destructor
******************************************************************************/
  virtual ~S3MapperOptimized(){

  }
};

#endif //IRIS_S3MAPPEROPTIMIZED_H

