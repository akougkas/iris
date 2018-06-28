/*******************************************************************************
* File constants.h
*
* Goal: Constants and system parameters
*
* Created: December 10th, 2016  - Anthony Kougkas
 * Updated:
* Illinois Institute of Technology - SCS Lab
* (C) 2016
******************************************************************************/
#ifndef IRIS_CONSTANTS_H
#define IRIS_CONSTANTS_H
/******************************************************************************
*include files
******************************************************************************/
#include <string>
#include <future>
#include "return_codes.h"
/******************************************************************************
*Iris library parameters
******************************************************************************/
static const u_int16_t FILE_BUFFER_CAPACITY = 1;
static const int rootRank=0;
const u_int64_t CONTAINER_COUNT = 512 * 131072;//64 files
//max uint32 hash value = 4294967295 (make it 4294967296)
const u_int64_t NUM_KEYS_IN_CONTAINER = 4294967296/CONTAINER_COUNT;
/*******************
*Various modes
*******************/
typedef enum posix_modes{
  STRICT   = 0,
  RELAXED  = 1
} posix_mode;
typedef enum caching_switch{
  ON   = 0,
  OFF  = 1
} caching;
typedef enum file_mapping_modes{
  BALANCED  = 0,
  GET_BASED = 1,
  PUT_BASED = 2,
  BASIC     = 3
} file_mapping_modes;
typedef enum object_mapping_modes{
  NAIVE           = 0,
  NAIVE_OPTIMIZED = 1,
  SIMPLE          = 2,
  OPTIMIZED       = 3
}object_mapping_modes ;

typedef enum pvfs_client_modes{
  BLOCKING          = 0,
  NON_BLOCKING      = 1
} pvfs_client_modes;
typedef enum operation{
  GET = 0,
  PUT = 1
}operation;
static operation GET_OPERATION = GET;
static operation PUT_OPERATION = PUT;
/******************************************************************************
*Asynchronous operations
******************************************************************************/
static std::future<int> asyncFetch;
/******************************************************************************
*Prefetcher
******************************************************************************/
static const std::string FILESYSTEM_PREFETCHER = "FILESYSTEM_PREFETCHER";
static const std::string OBJECTSTORE_PREFETCHER = "OBJECTSTORE_PREFETCHER";
typedef enum prefetching_modes{
  SEQUENTIAL    = 0,
  STRIDED       = 1,
  RANDOM        = 2,
  USER_DEFINED  = 3,
  NONE          = 4
} prefetch_mode;
/******************************************************************************
*Cache Manager
******************************************************************************/
typedef enum replacementPolicy{
  LRU_POLICY = 0,
  LFU_POLICY = 1
}replacementPolicy;
static replacementPolicy LRUPolicy = LRU_POLICY;
static replacementPolicy LFUPolicy = LFU_POLICY;
/******************************************************************************
*Aggregator
******************************************************************************/
static const std::string FILESYSTEM_AGGREGATOR = "FILESYSTEM_AGGREGATOR";
static const std::string OBJECTSTORE_AGGREGATOR = "OBJECTSTORE_AGGREGATOR";
/******************************************************************************
*Metadata Manager
******************************************************************************/
static const std::string IRIS_METADATA_MANAGER = "IRIS_METADATA_MANAGER";
static const std::string POSIX_METADATA_MANAGER = "POSIX_METADATA_MANAGER";
static const std::string HDF5_METADATA_MANAGER = "HDF5_METADATA_MANAGER";
static const std::string S3_METADATA_MANAGER = "S3_METADATA_MANAGER";
/******************************************************************************
*Mappers
******************************************************************************/
static const std::string POSIX_MAPPER_GET_OPTIMISED ="POSIX_MAPPER_GET_OPTIMISED";
static const std::string POSIX_MAPPER_PUT_OPTIMISED ="POSIX_MAPPER_PUT_OPTIMISED";
static const std::string HDF5_MAPPER_BASIC = "HDF5_MAPPER_BASIC";
static const std::string S3_MAPPER_NAIVE = "S3_MAPPER_NAIVE";
static const std::string S3_MAPPER_NAIVE_OPTIMIZED = "S3_MAPPER_NAIVE_OPTIMIZED";
static const std::string S3_MAPPER_OPTIMIZED = "S3_MAPPER_OPTIMIZED";
/******************************************************************************
*Object Stores
******************************************************************************/
static const std::string HYPERDEX_CLIENT_PUT_OPTIMISED ="HYPERDEX_CLIENT_PUT_OPTIMISED";
static const std::string HYPERDEX_CLIENT_GET_OPTIMISED ="HYPERDEX_CLIENT_GET_OPTIMISED";
static const char * SPACE = "IRIS_KEYSPACE";
static const char * ATTRIBUTE_NAME = "data";
static const char * DESCRIPTION = "space IRIS_KEYSPACE key filename_chunk "
    "attributes data tolerate 0 failures";

static const std::string MONGO_CLIENT = "MONGO_CLIENT";
static const std::string MONGO_DATABASE  = "iris"; //iris database in MongoDB
static const std::string MONGO_COLLECTION  = "file"; //FILE_RANK is the format
/******************************************************************************
*File Systems
******************************************************************************/
static const std::string PVFS2_CLIENT_BLOCKING="PVFS2_CLIENT";
static const std::string PVFS2_CLIENT_NON_BLOCKING="PVFS2_CLIENT_NON_BLOCKING";

#endif //IRIS_CONSTANTS_H

