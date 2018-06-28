/*******************************************************************************
* File S3.h
*
* Goal: This is the S3/Swift interface
*
* Created: December 10th, 2016  - Anthony Kougkas
 * Updated:
* Illinois Institute of Technology - SCS Lab
* (C) 2016
******************************************************************************/
#ifndef IRIS_S3_H
#define IRIS_S3_H
/******************************************************************************
*include files
******************************************************************************/
#include "API.h"
#include <bsoncxx/v_noabi/bsoncxx/json.hpp>
#include <mongocxx/v_noabi/mongocxx/client.hpp>
#include <mongocxx/v_noabi/mongocxx/stdx.hpp>
#include <mongocxx/result/insert_one.hpp>

namespace iris {
/******************************************************************************
*Interface operations
******************************************************************************/
  void* get(std::string objectName);
  void put(std::string objectName, void* data, std::size_t objectSize);
  void remove(std::string objectName);
  void flushData();

}
#endif //IRIS_S3_H

















