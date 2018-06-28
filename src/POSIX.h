/*******************************************************************************
* File POSIX.h
*
* Goal: This is the POSIX interface
*
* Created: December 10th, 2016  - Anthony Kougkas
 * Updated:
* Illinois Institute of Technology - SCS Lab
* (C) 2016
******************************************************************************/
#ifndef IRIS_POSIX_H
#define IRIS_POSIX_H
/******************************************************************************
*include files
******************************************************************************/
#include "API.h"
/******************************************************************************
*PRELOAD STUFF
******************************************************************************/
#ifdef PRELOAD_POSIX
IRIS_FORWARD_DECL(fopen,FILE *,(const char *filename, const char *mode));
IRIS_FORWARD_DECL(fclose,int,(FILE *stream));
IRIS_FORWARD_DECL(fseek,int,(FILE *stream, long int offset, int origin));
IRIS_FORWARD_DECL(fread,size_t,(void *ptr, size_t size, size_t count, FILE *stream));
IRIS_FORWARD_DECL(fwrite,size_t,(const void *ptr, size_t size, size_t count, FILE *stream));

static std::unordered_map<FILE*,int> fileDescriptorPool;
static int addOrUpdateFileDescriptorPool(FILE* fh) {
  if(fileDescriptorPool.empty()){
    fileDescriptorPool=std::unordered_map<FILE*,int>();
  }
  auto poolIterator = fileDescriptorPool.find(fh);
  if (poolIterator != fileDescriptorPool.end())
    fileDescriptorPool.erase(poolIterator);
  fileDescriptorPool.insert({fh,fileno(fh)});
  return OPERATION_SUCCESSFUL;
}

static bool isFileDescriptorExcluded(FILE* fh) {
  auto poolIterator=fileDescriptorPool.find(fh);
  return poolIterator != fileDescriptorPool.end();
}

static int deleteFileDescriptorFromPool(FILE* fh) {
  auto poolIterator=fileDescriptorPool.find(fh);
  if(poolIterator!=fileDescriptorPool.end()) fileDescriptorPool.erase
        (poolIterator);
  return OPERATION_SUCCESSFUL;
}
#else
namespace iris {
#endif
/******************************************************************************
*Interface operations
******************************************************************************/
  FILE *fopen(const char *filename, const char *mode);

  int fclose(FILE *stream);

  int fseek(FILE *stream, long int offset, int origin);

  size_t fread(void *ptr, size_t size, size_t count, FILE *stream);

  size_t fwrite(const void *ptr, size_t size, size_t count, FILE *stream);
#ifndef PRELOAD_POSIX
}
#endif
#endif //IRIS_POSIX_H
