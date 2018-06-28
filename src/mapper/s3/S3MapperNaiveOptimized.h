/*******************************************************************************
* File S3MapperNaiveOptimized.h
*
* Goal: This is the S3/Swift mapper
*
* Created: December 10th, 2016  - Anthony Kougkas
 * Updated:
* Illinois Institute of Technology - SCS Lab
* (C) 2016
******************************************************************************/
#ifndef IRIS_S3MAPPERNAIVEOPTIMIZED_H
#define IRIS_S3MAPPERNAIVEOPTIMIZED_H
/******************************************************************************
*include files
******************************************************************************/
#include <mpi.h>
#include "S3MapperNaive.h"
/******************************************************************************
*Class
******************************************************************************/
class S3MapperNaiveOptimized : public S3MapperNaive  {
private:
/******************************************************************************
*Variables and members
******************************************************************************/
  static std::shared_ptr<S3MapperNaiveOptimized> instance;
  Container keySpace;
  static std::string CONTAINER_NAME;
/******************************************************************************
*Functions
******************************************************************************/
  std::pair<std::string, VirtualObject>
  mapObjectForGet(std::string objectName, std::size_t objectSize);

  std::pair<std::string, VirtualObject>
  mapObjectForPut(std::string objectName, std::size_t objectSize);
  std::mutex mtx;
protected:
/******************************************************************************
*Constructor
******************************************************************************/
  S3MapperNaiveOptimized() : keySpace() {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    keySpace.containerName = "container_" + std::to_string(rank) + ".dat";//CONTAINER_NAME;
  }
public:
/******************************************************************************
*Getters and setters
******************************************************************************/
  static std::shared_ptr<S3MapperNaiveOptimized> getInstance() {
    return instance == nullptr
    ? instance = std::shared_ptr<S3MapperNaiveOptimized>(new S3MapperNaiveOptimized())
           : instance;
  }
/******************************************************************************
*Interface
******************************************************************************/
  virtual std::pair<std::string, VirtualObject>
  mapObject(std::string objectName, std::size_t objectSize,
            operation operationType) override;
/******************************************************************************
*Destructor
******************************************************************************/
  virtual ~S3MapperNaiveOptimized(){}
};
#endif //IRIS_S3MAPPERNAIVEOPTIMIZED_H


