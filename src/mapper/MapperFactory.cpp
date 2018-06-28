/******************************************************************************
*include files
******************************************************************************/
#include "MapperFactory.h"
#include "posix/POSIXMapper.h"
#include "HDF5Mapper.h"
#include "s3/S3Mapper.h"
#include "posix/POSIXMapperGetOptimised.h"
#include "posix/POSIXMapperPutOptimised.h"
#include "s3/S3MapperOptimized.h"
#include "s3/S3MapperNaiveOptimized.h"
/******************************************************************************
*Initialization of static members
******************************************************************************/
std::shared_ptr<MapperFactory> MapperFactory::instance = nullptr;
/******************************************************************************
*Gettters and setters
******************************************************************************/
std::shared_ptr<AbstractMapper> MapperFactory::getMapper(const std::string name){
  if(mapperMap.empty()){
    mapperMap=std::unordered_map<std::string,
        std::shared_ptr<AbstractMapper>>();
  }
  auto iter = mapperMap.find(name);
  if(iter != mapperMap.end()) return iter->second;
  else {
    std::shared_ptr<AbstractMapper> mapperInstance;
    if (name == POSIX_MAPPER_GET_OPTIMISED) {
      mapperInstance = POSIXMapperGetOptimised::getInstance();
    } else if (name == POSIX_MAPPER_PUT_OPTIMISED) {
      mapperInstance = POSIXMapperPutOptimised::getInstance();
    } else if (name == ConfigurationManager::getInstance()->getPOSIX_MAPPER()) {
      mapperInstance = std::shared_ptr<POSIXMapper>(new POSIXMapper());
    } else if (name == ConfigurationManager::getInstance()->getHDF5_MAPPER()) {
      mapperInstance = std::shared_ptr<HDF5Mapper>();
    } else if (name == S3_MAPPER_NAIVE) {
      mapperInstance = std::shared_ptr<S3MapperNaive>(new S3MapperNaive());
    } else if (name == S3_MAPPER_NAIVE_OPTIMIZED) {
      mapperInstance = S3MapperNaiveOptimized::getInstance();
    } else if (name == S3_MAPPER_OPTIMIZED) {
      mapperInstance = S3MapperOptimized::getInstance();
    } else if (name == ConfigurationManager::getInstance()->getS3_MAPPER()) {
      mapperInstance = S3Mapper::getInstance();
    } else return nullptr;
    mapperMap.emplace(name, mapperInstance);
    return mapperInstance;
  }
}

