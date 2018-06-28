/*******************************************************************************
* File API.h
*
* Goal:
*
* Created: December 10th, 2016  - Anthony Kougkas
 * Updated:
* Illinois Institute of Technology - SCS Lab
* (C) 2016
******************************************************************************/
#ifndef IRIS_API_H
#define IRIS_API_H
/******************************************************************************
*include files
******************************************************************************/
#include <memory>
#include "utils/prefetching/AbstractPrefetcher.h"
#include "utils/prefetching/PrefetcherFactory.h"
#include "config/constants.h"
#include "utils/caching/CacheManager.h"
#include "utils/prefetching/ObjectStorePrefetcher.h"
#include "utils/prefetching/FileSystemPrefetcher.h"
#include "metadata_manager/POSIXMetadataManager.h"
#include "metadata_manager/MetadataManagerFactory.h"
#include "metadata_manager/IrisMetadataManager.h"
#include "mapper/posix/POSIXMapper.h"
#include "mapper/MapperFactory.h"
#include "mapper/s3/S3Mapper.h"
#include "mapper/HDF5Mapper.h"
#include "metadata_manager/HDF5MetadataManager.h"
#include "metadata_manager/S3MetadataManager.h"
#include "object_stores/ObjectStoreFactory.h"
#include "file_systems/FileSystemFactory.h"
#include "utils/aggregation/AggregatorFactory.h"
#include "dlfcn.h"
//#define _GNU_SOURCE
/******************************************************************************
*PRELOAD STUFF
******************************************************************************/
#define IRIS_FORWARD_DECL(name, ret, args) typedef ret(*__real_t_##name)args;
#ifdef PRELOAD_POSIX
#define IRIS_DECL(__name) __name
#else
namespace iris{}
#define IRIS_DECL(__name) iris::__name
#endif
#define MAP_OR_FAIL(func)                                                      \
    __real_t_##func __real_##func;                                             \
    __real_##func = (__real_t_##func)dlsym(RTLD_NEXT,#func);

static bool is_excluded(const char* path) {
  static const char* excluded[] = {
      "/etc/", "/dev/", "/usr/", "/bin/", "/boot/",
      "/lib/", "/opt/", "/sbin/", "/sys/", "/proc/",
      "/home/anthony/Dropbox/Projects/iris",
      NULL
  };
  int tmp_index = 0;
  const char* exclude;
  while((exclude = excluded[tmp_index])) {
    if(!(strncmp(exclude, path, strlen(exclude)))) break;
    tmp_index++;
  }
  return exclude != 0;
}

/******************************************************************************
*Class
******************************************************************************/
class API {
private:
/******************************************************************************
*Variables and members
******************************************************************************/
  static std::shared_ptr<API> instance;
  std::shared_ptr<CacheManager> cacheManager;
  std::shared_ptr<PrefetcherFactory> prefetcherFactory;
  std::shared_ptr<MetadataManagerFactory> metadataManagerFactory;
  std::shared_ptr<MapperFactory> mapperFactory;
  std::shared_ptr<ObjectStoreFactory> objectStoreFactory;
  std::shared_ptr<FileSystemFactory> fileSystemFactory;
/******************************************************************************
*Constructors
******************************************************************************/
  API();
public:
/******************************************************************************
*Destructor
******************************************************************************/
  virtual ~API();
/******************************************************************************
*Getters and setters
******************************************************************************/
  inline static std::shared_ptr<API> getInstance(){
    return instance== nullptr ? instance=std::shared_ptr<API>(new API())
                              : instance;
  }

  inline std::shared_ptr<CacheManager> getCacheManager(){
    return cacheManager;
  }

  inline std::shared_ptr<PrefetcherFactory> getPrefetcherFactory(){
    return prefetcherFactory;
  }

  inline std::shared_ptr<MetadataManagerFactory> getMetadataManagerFactory() {
    return metadataManagerFactory;
  }

  inline std::shared_ptr<MapperFactory> getMapperFactory(){
    return mapperFactory;
  }

  inline std::shared_ptr<ObjectStoreFactory> getObjectStoreFactory(){
    return objectStoreFactory;
  }

  inline std::shared_ptr<FileSystemFactory> getFileSystemFactory() {
    return fileSystemFactory;
  }
};

#endif //IRIS_API_H
