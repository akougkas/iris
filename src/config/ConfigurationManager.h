/*******************************************************************************
* File ConfigurationManager.h
*
* Goal: Use this manager to set/define the internal IRIS components
*
* Created: December 10th, 2016  - Anthony Kougkas
 * Updated:
* Illinois Institute of Technology - SCS Lab
* (C) 2016
******************************************************************************/
#ifndef IRIS_CONFIGURATIONMANAGER_H
#define IRIS_CONFIGURATIONMANAGER_H
/******************************************************************************
*include files
******************************************************************************/
#include "constants.h"
/******************************************************************************
*Class
******************************************************************************/
class ConfigurationManager {
private:
  static std::shared_ptr<ConfigurationManager> instance;
/******************************************************************************
*Mapper
******************************************************************************/
  std::string POSIX_MAPPER;
  std::string HDF5_MAPPER;
  std::string S3_MAPPER;
  std::string PVFS2_CLIENT;
/******************************************************************************
*Constructor
******************************************************************************/
  ConfigurationManager(): MAX_FILENAME_LENGTH(256),
                          POSIX_MODE(RELAXED),
                          CACHING_MODE(ON),
                          PVFS2CLIENT_MODE(BLOCKING),
                          FILE_MAPPING_MODE(BALANCED),
                          OBJECT_MAPPING_MODE(OPTIMIZED),
                          MAX_OBJ_SIZE(2*1024*1024),
                          MEMTABLE_CAPACITY(8*MAX_OBJ_SIZE),
                          CONTAINER_MAX_SIZE(16*MEMTABLE_CAPACITY),
                          PREFETCH_MODE(SEQUENTIAL),
                          CACHE_CAPACITY(2ul*1024ul*1024ul*1024ul),
                          CURRENT_POLICY(LRU_POLICY),
                          COORDINATOR((char *) "127.0.0.1"),
                          COORDINATOR_PORT(1982)
  {
    POSIX_MAPPER =
        FILE_MAPPING_MODE==BALANCED
        ?"POSIX_MAPPER":
        FILE_MAPPING_MODE==GET_BASED
        ?POSIX_MAPPER_GET_OPTIMISED: POSIX_MAPPER_PUT_OPTIMISED;
    HDF5_MAPPER =
        FILE_MAPPING_MODE==BASIC
        ?HDF5_MAPPER_BASIC:"HDF5_MAPPER";
    S3_MAPPER=OBJECT_MAPPING_MODE==NAIVE
              ?S3_MAPPER_NAIVE:
              OBJECT_MAPPING_MODE==NAIVE_OPTIMIZED
              ?S3_MAPPER_NAIVE_OPTIMIZED :
              OBJECT_MAPPING_MODE==OPTIMIZED
              ?S3_MAPPER_OPTIMIZED:"S3_MAPPER";
    HYPERDEX_CLIENT=
        FILE_MAPPING_MODE==GET_BASED?HYPERDEX_CLIENT_GET_OPTIMISED:
        FILE_MAPPING_MODE==PUT_BASED?HYPERDEX_CLIENT_PUT_OPTIMISED:"HYPERDEX_CLIENT";
    PVFS2_CLIENT =
        PVFS2CLIENT_MODE==BLOCKING?"PVFS2_CLIENT":PVFS2_CLIENT_NON_BLOCKING;
    MONGO_URI = "mongodb://localhost:27017";
    WORKING_DIRECTORY = "/mnt/orangefs/hybrid/";
  }
public:
/******************************************************************************
*Iris library parameters
******************************************************************************/
  u_int16_t MAX_FILENAME_LENGTH;
  u_int16_t POSIX_MODE;
  u_int16_t CACHING_MODE;
  u_int16_t PVFS2CLIENT_MODE;
  u_int16_t FILE_MAPPING_MODE;
  u_int16_t OBJECT_MAPPING_MODE=NAIVE;
  u_int64_t MAX_OBJ_SIZE;
  u_int64_t MEMTABLE_CAPACITY;
  u_int64_t CONTAINER_MAX_SIZE;
/******************************************************************************
*Prefetcher
******************************************************************************/
  u_int16_t PREFETCH_MODE;
/******************************************************************************
*Cache Manager
******************************************************************************/
  unsigned long CACHE_CAPACITY;
  replacementPolicy CURRENT_POLICY;
/******************************************************************************
*Object Stores
******************************************************************************/
  //HYPERDEX
  char * COORDINATOR;
  uint16_t COORDINATOR_PORT;
  std::string HYPERDEX_CLIENT;
  //MONGODB
  std::string MONGO_URI;
/******************************************************************************
*File Systems
******************************************************************************/
  std::string WORKING_DIRECTORY;
/******************************************************************************
*Gettters and setters
******************************************************************************/
  inline static std::shared_ptr<ConfigurationManager> getInstance(){
    return instance== nullptr ? instance = std::shared_ptr<ConfigurationManager>
        (new ConfigurationManager()) : instance;
  }
  std::string getPOSIX_MAPPER() {
    return FILE_MAPPING_MODE==BALANCED
           ?"POSIX_MAPPER":
           FILE_MAPPING_MODE==GET_BASED
           ?POSIX_MAPPER_GET_OPTIMISED: POSIX_MAPPER_PUT_OPTIMISED;
  }
  std::string getHDF5_MAPPER() {
    return FILE_MAPPING_MODE==BASIC
           ?HDF5_MAPPER_BASIC:"HDF5_MAPPER";
  }
  std::string getS3_MAPPER()  {
    return OBJECT_MAPPING_MODE==NAIVE
           ?S3_MAPPER_NAIVE:
           OBJECT_MAPPING_MODE==NAIVE_OPTIMIZED
           ?S3_MAPPER_NAIVE_OPTIMIZED :
           OBJECT_MAPPING_MODE==OPTIMIZED
           ?S3_MAPPER_OPTIMIZED:"S3_MAPPER";
  }
  std::string getPVFS2_CLIENT() {
    return PVFS2CLIENT_MODE==BLOCKING?"PVFS2_CLIENT":PVFS2_CLIENT_NON_BLOCKING;
  }
/******************************************************************************
*Destructor
******************************************************************************/
  virtual ~ConfigurationManager();
};
#endif //IRIS_CONFIGURATIONMANAGER_H
