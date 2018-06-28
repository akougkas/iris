/******************************************************************************
*include files
******************************************************************************/
#include "config/constants.h"
#include <cstring>
#include <algorithm>
#include <iostream>
#include "POSIX.h"
#include "object_stores/MongoDBClient.h"
/******************************************************************************
*Interface operations
******************************************************************************/
/******************************************************************************
*fopen
******************************************************************************/
FILE* IRIS_DECL(fopen)(const char *filename, const char *mode) {
#ifdef TIMER
  Timer timer = Timer(); timer.startTime();
#endif
#ifdef DEBUG
  std::cout << "####################  FOPEN ####################" << std::endl;
#endif/*DEBUG*/
  FILE* fh;
#ifdef PRELOAD_POSIX
  if(is_excluded(filename)){
    MAP_OR_FAIL(fopen);
    fh=__real_fopen(filename,mode);
    addOrUpdateFileDescriptorPool(fh);
    return fh;
  }
#endif
  auto apiInstance = API::getInstance();
  auto posixMetadataManager = std::static_pointer_cast<POSIXMetadataManager>
      (apiInstance->getMetadataManagerFactory()->
          getMetadataManager(POSIX_METADATA_MANAGER));
#ifdef RELEASE
  /*Check if the filename is longer than allowed (<256)*/
  if(std::strlen(filename) > MAX_FILENAME_LENGTH){
    fprintf(stderr, "ERROR! File name too long!\n");
    return NULL;
  }
#endif /*RELEASE*/

  /*First check if the file already exists and if it is opened*/

  if(posixMetadataManager->checkIfFileExists(filename)){
    if(posixMetadataManager->checkIfFileIsOpen(filename)){
      //TODO: find the fh from the map and return it
    }
    fh = fmemopen(NULL, FILE_BUFFER_CAPACITY, mode);
    if (fh == NULL){
      fprintf(stderr, "open() error: %s\n", strerror(errno));
      return NULL;
    }
    posixMetadataManager->updateMetadataOnOpen(fh, filename, mode);
  }
  else{
    fh = fmemopen(NULL, FILE_BUFFER_CAPACITY, mode);
    if (fh == NULL){
      fprintf(stderr, "fmemopen() error: %s\n", strerror(errno));
      return NULL;
    }
    posixMetadataManager->createMetadata(fh, filename, mode);
  }
#ifdef TIMER1
  timer.endTime(__FUNCTION__);
#endif
  return fh;
}

/******************************************************************************
*fclose
******************************************************************************/
int IRIS_DECL(fclose)(FILE *stream) {
#ifdef TIMER1
  Timer timer = Timer(); timer.startTime();
#endif
#ifdef DEBUG
  std::cout << "####################  FCLOSE ###################" << std::endl;
#endif/*DEBUG*/
#ifdef PRELOAD_POSIX
  if(isFileDescriptorExcluded(stream)){
    MAP_OR_FAIL(fclose);
    deleteFileDescriptorFromPool(stream);
    return __real_fclose(stream);
  }
#endif
  if(asyncFetch.valid()) asyncFetch.get();
  auto apiInstance = API::getInstance();
  auto posixMetadataManager = std::static_pointer_cast<POSIXMetadataManager>
      (apiInstance->getMetadataManagerFactory()->
          getMetadataManager(POSIX_METADATA_MANAGER));


  const char * filename = posixMetadataManager->getFilename(stream);
  if (!posixMetadataManager->checkIfFileIsOpen(filename)) return -1;
  else {
#ifdef PRELOAD_POSIX
    MAP_OR_FAIL(fclose);
    __real_fclose(stream);
#else
    std::fclose(stream);
#endif
    posixMetadataManager->updateMetadataOnClose(stream, filename);
  }
#ifdef TIMER1
  timer.endTime(__FUNCTION__);
#endif
  return 0;
}

/******************************************************************************
*fseek
******************************************************************************/
int IRIS_DECL(fseek)(FILE *stream, long int offset, int origin) {
#ifdef TIMER1
  Timer timer = Timer(); timer.startTime();
#endif
#ifdef DEBUG
  std::cout << "####################  FSEEK ####################" << std::endl;
#endif/*DEBUG*/
#ifdef PRELOAD_POSIX
  if(isFileDescriptorExcluded(stream)){
    MAP_OR_FAIL(fseek);
    return __real_fseek(stream,offset,origin);
  }
#endif
  auto apiInstance = API::getInstance();
  auto posixMetadataManager = std::static_pointer_cast<POSIXMetadataManager>
      (apiInstance->getMetadataManagerFactory()->
          getMetadataManager(POSIX_METADATA_MANAGER));

  const char * filename = posixMetadataManager->getFilename(stream);
  posixMetadataManager->updateFpPosition(stream, offset, origin, filename);
#ifdef TIMER1
  timer.endTime(__FUNCTION__);
#endif
  return 0;
}

/******************************************************************************
*fread
******************************************************************************/
size_t IRIS_DECL(fread)(void *ptr, std::size_t size, std::size_t count, FILE
*stream) {
#ifdef DEBUG
  std::cout << "####################  FREAD ####################" << std::endl;
#endif/*DEBUG*/
#ifdef PRELOAD_POSIX
  if(isFileDescriptorExcluded(stream)){
    MAP_OR_FAIL(fread);
    return __real_fread(ptr,size,count,stream);
  }
#endif
  if(asyncFetch.valid()) asyncFetch.get();
#ifdef TIMER1
  Timer timer = Timer(); timer.startTime();
#endif
  auto operationSize = size*count;
#ifdef RELEASE
  if(operationSize == 0) return 0;
#endif /*RELEASE*/
  auto apiInstance = API::getInstance();
  auto posixMetadataManager = std::static_pointer_cast<POSIXMetadataManager>
      (apiInstance->getMetadataManagerFactory()->
          getMetadataManager(POSIX_METADATA_MANAGER));

  auto posixMapper = std::static_pointer_cast<POSIXMapper>
      (apiInstance->getMapperFactory()->getMapper
          (ConfigurationManager::getInstance()->getPOSIX_MAPPER()));
  auto cacheManager = apiInstance->getCacheManager();
  auto objectStorePrefetcher = std::static_pointer_cast<ObjectStorePrefetcher>
      (apiInstance->getPrefetcherFactory()->getPrefetcher(OBJECTSTORE_PREFETCHER));
  auto objectStoreClient = std::static_pointer_cast<MongoDBClient>
      (apiInstance->getObjectStoreFactory()->getObjectStore(MONGO_CLIENT));

  const char * filename = posixMetadataManager->getFilename(stream);
  long int fileOffset = posixMetadataManager->getFpPosition(stream);
  auto filesize = posixMetadataManager->getFilesize(filename);

  auto keys = posixMapper->generateKeys(filename, fileOffset, operationSize,
                                        GET_OPERATION);

  Buffer buffer = Buffer(ptr);
  size_t bufferIndex = 0;
  for (auto&& key : keys) {
    auto originalKeySize = key->size;
    int status = 0;
    if(cacheManager->isCached(key) == NO_CACHED_DATA_FOUND) {
      status = objectStoreClient->get(key);
      if (status == OPERATION_SUCCESSFUL) {
        buffer.update(key->data, bufferIndex,
                      originalKeySize > strlen((char *) key->data) ? strlen(
                          (char *) key->data) : originalKeySize);
      } else {
        //TODO:throw error for get
      }
    }else{
      buffer.update(key->data, bufferIndex,
                    originalKeySize > strlen((char *) key->data) ? strlen(
                        (char *) key->data) : originalKeySize);
    }
    bufferIndex+=originalKeySize;
    key.reset();
  }
  asyncFetch= std::async (std::launch::async,
                              &ObjectStorePrefetcher::fetch,
                  objectStorePrefetcher, filename, fileOffset,operationSize, filesize);
  //objectStorePrefetcher->fetch(filename, fileOffset, operationSize, filesize);
  posixMetadataManager->updateMetadataOnRead(stream, operationSize);
#ifdef TIMER1
  timer.endTime(__FUNCTION__);
#endif
  keys.clear();
  return operationSize;
}
/******************************************************************************
*fwrite
******************************************************************************/
size_t IRIS_DECL(fwrite)(const void *ptr, size_t size, size_t count, FILE
*stream) {

#ifdef DEBUG
  std::cout << "####################  FWRITE ###################" << std::endl;
#endif/*DEBUG*/
#ifdef TIMER1
  Timer timer = Timer(); timer.startTime();
#endif
#ifdef PRELOAD_POSIX
  if(isFileDescriptorExcluded(stream)){
    MAP_OR_FAIL(fwrite);
    return __real_fwrite(ptr,size,count,stream);
  }
#endif
  if(asyncFetch.valid()) asyncFetch.get();
  auto operationSize = size * count;
  if(operationSize == 0) return 0;

  auto apiInstance = API::getInstance();
  auto posixMetadataManager = std::static_pointer_cast<POSIXMetadataManager>
      (apiInstance->getMetadataManagerFactory()->
          getMetadataManager(POSIX_METADATA_MANAGER));

  auto posixMapper = std::static_pointer_cast<POSIXMapper>
      (apiInstance->getMapperFactory()->getMapper
  (ConfigurationManager::getInstance()->getPOSIX_MAPPER()));
  auto cacheManager = apiInstance->getCacheManager();
  auto objectStoreClient = std::static_pointer_cast<MongoDBClient>
      (apiInstance->getObjectStoreFactory()->getObjectStore(MONGO_CLIENT));

  const char *filename = posixMetadataManager->getFilename(stream);
  long int fileOffset = posixMetadataManager->getFpPosition(stream);
  if(!posixMetadataManager->checkIfFileIsOpen(filename)) return 0;

  auto keys = posixMapper->generateKeys(filename, fileOffset, operationSize,PUT_OPERATION);
  std::size_t bufferIndex = 0;
  for (auto &&key : keys) {
    key->data=malloc(key->size);
    memcpy(key->data,(char*)ptr + bufferIndex,key->size);
    bufferIndex += key->size;
    objectStoreClient->put(key);
    if(ConfigurationManager::getInstance()->CACHING_MODE == ON)
      cacheManager->addToCache(key);
    key.reset();
  }
  posixMetadataManager->updateMetadataOnWrite(stream, operationSize);
#ifdef TIMER1
  timer.endTime(__FUNCTION__);
#endif
  keys.clear();
 // delete keys;
  return operationSize;
}

