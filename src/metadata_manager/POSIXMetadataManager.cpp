/******************************************************************************
*include files
******************************************************************************/
#include <chrono>
#include <iostream>
#include "POSIXMetadataManager.h"
#include "../config/constants.h"
#include "../config/ConfigurationManager.h"

/******************************************************************************
*Interface
******************************************************************************/
bool POSIXMetadataManager::checkIfFileExists(const char * filename) {
#ifdef TIMER
  Timer timer = Timer(); timer.startTime();
#endif
  //Error checks
#ifdef RELEASE
  /*Check if the filename is longer than allowed (<256)*/
  if(std::strlen(filename) > MAX_FILENAME_LENGTH){
    fprintf(stderr, "ERROR! File name too long!\n fn: checkIfFileIsOpen");
    exit(-1);
  }
#endif /*RELEASE*/
  /* Check if the filename exists in the map of the created files*/
  auto iterator = created_files.find(filename);
#ifdef TIMER
  timer.endTime(__FUNCTION__);
#endif
  return !(iterator == created_files.end());
}

bool POSIXMetadataManager::checkIfFileIsOpen(const char *filename) {
#ifdef TIMER
  Timer timer = Timer(); timer.startTime();
#endif
  //Error checks
#ifdef RELEASE
  /*Check if the filename is longer than allowed (<256)*/
  if(std::strlen(filename) > MAX_FILENAME_LENGTH){
    fprintf(stderr, "ERROR! File name too long!\n fn: checkIfFileIsOpen");
    exit(-1);
  }
#endif /*RELEASE*/
  if(checkIfFileExists(filename)) {
    /* Check if the file has been previously been opened*/
    auto iterator = created_files.find(filename);
#ifdef TIMER
    timer.endTime(__FUNCTION__);
#endif
    return iterator == created_files.end() ?
           false : created_files[filename].opened;
  }
  else{
#ifdef DEBUG
    fprintf(stderr, "INFO! File not opened as it doesnt exist! fn: "
        "checkIfFileIsOpen\n");
#endif /* DEBUG*/
    return false;
  }
}

const char *POSIXMetadataManager::getFilename(FILE *fh) {
#ifdef TIMER
  Timer timer = Timer(); timer.startTime();
#endif
  //Error checks
#ifdef RELEASE
  if(fh == nullptr) return nullptr;
#endif /*RELEASE*/
  auto iterator = fh2filename.find(fh);
#ifdef TIMER
  timer.endTime(__FUNCTION__);
#endif
  if(iterator == fh2filename.end()) return nullptr;
  else return iterator->second; //holds the filename
}

int POSIXMetadataManager::createMetadata(FILE * fh, const char * filename,
                                         const char* mode) {
#ifdef TIMER
  Timer timer = Timer(); timer.startTime();
#endif
  //Error checks
#ifdef RELEASE
  if(fh == nullptr || std::strlen(filename) > MAX_FILENAME_LENGTH)
    return METADATA_CREATION_FAILED;
#endif /*RELEASE*/
  Stat file_metadata;
  if(ConfigurationManager::getInstance()->POSIX_MODE ==STRICT)
    file_metadata = {true,
                     mode,
                     getuid(),
                     getgid(),
                     0,
                     time(NULL),
                     time(NULL),
                     time(NULL)};
  else file_metadata = {true,
                        mode,
                        0,
                        0,
                        0,
                        0,
                        0,
                        0};
  created_files.insert(std::make_pair(filename,file_metadata));
  fh2filename.insert(std::make_pair(fh, filename));
  pointer.insert(std::make_pair(fh,0));
#ifdef TIMER
  timer.endTime(__FUNCTION__);
#endif
  return OPERATION_SUCCESSFUL;
}

int POSIXMetadataManager::updateMetadataOnOpen(FILE * fh, const char * filename,
                                               const char* mode) {
#ifdef TIMER
  Timer timer = Timer(); timer.startTime();
#endif
  //Error checks
#ifdef RELEASE
  if(fh == nullptr || std::strlen(filename) > MAX_FILENAME_LENGTH)
    return METADATA_UPDATE_FAILED__OPEN;
#endif /*RELEASE*/
  if(ConfigurationManager::getInstance()->POSIX_MODE ==STRICT)
    created_files[filename] = {true,
                               mode,
                               created_files[filename].st_uid,
                               created_files[filename].st_gid,
                               created_files[filename].st_size,
                               time(NULL),
                               created_files[filename].mtime,
                               created_files[filename].ctime};
  else created_files[filename] = {true,
                                  mode,
                                  0,
                                  0,
                                  created_files[filename].st_size,
                                  0,
                                  0,
                                  0};
  fh2filename.insert(std::make_pair(fh, filename));
  pointer.insert(std::make_pair(fh,0));
#ifdef TIMER
  timer.endTime(__FUNCTION__);
#endif
  return OPERATION_SUCCESSFUL;
}

int POSIXMetadataManager::updateMetadataOnClose(FILE * fh,
                                                const char * filename) {
#ifdef TIMER
  Timer timer = Timer(); timer.startTime();
#endif
  //Error checks
#ifdef RELEASE
  if(fh == nullptr || std::strlen(filename) > MAX_FILENAME_LENGTH)
    return METADATA_UPDATE_FAILED__CLOSE;
#endif /*RELEASE*/
  if(ConfigurationManager::getInstance()->POSIX_MODE ==STRICT){
    created_files[filename].opened = false;
    created_files[filename].atime = time(NULL);
  }
  else created_files[filename].opened = false;
  fh2filename.erase(fh);
  pointer.erase(fh);
#ifdef TIMER
  timer.endTime(__FUNCTION__);
#endif
  return OPERATION_SUCCESSFUL;
}

int POSIXMetadataManager::updateMetadataOnRead(FILE *fh,
                                               std::size_t operationSize) {
#ifdef TIMER
  Timer timer = Timer(); timer.startTime();
#endif
  //Error checks
#ifdef RELEASE
  if(fh == nullptr || operationSize == 0) return METADATA_UPDATE_FAILED__READ;
#endif /*RELEASE*/
  const char * filename = getFilename(fh);
#ifdef RELEASE
  if(filename == nullptr) return FILENAME_DOES_NOT_EXIST;
#endif /*RELEASE*/
  if(ConfigurationManager::getInstance()->POSIX_MODE ==STRICT)
    created_files[filename].atime = time(NULL);
  updateFpPosition(fh, operationSize, SEEK_CUR, filename);
#ifdef TIMER
  timer.endTime(__FUNCTION__);
#endif
  return OPERATION_SUCCESSFUL;
}

int POSIXMetadataManager::updateMetadataOnWrite(FILE *fh,
                                                std::size_t operationSize) {
#ifdef TIMER
  Timer timer = Timer(); timer.startTime();
#endif
#ifdef RELEASE
  if(fh == nullptr || operationSize == 0) return METADATA_UPDATE_FAILED__WRITE;
#endif /*RELEASE*/
  const char * filename = getFilename(fh);
#ifdef RELEASE
  if(filename == nullptr) return FILENAME_DOES_NOT_EXIST;
#endif /*RELEASE*/
  if(ConfigurationManager::getInstance()->POSIX_MODE ==STRICT){
    created_files[filename].atime = time(NULL);
    created_files[filename].mtime = time(NULL);
  }
  long int fileOffset = getFpPosition(fh);
#ifdef RELEASE
  if(fileOffset < 0) return FP_DOES_NOT_EXIST;
#endif /*RELEASE*/
  created_files[filename].st_size =
      fileOffset + operationSize > created_files[filename].st_size
      ? fileOffset + operationSize : created_files[filename].st_size;
  updateFpPosition(fh, operationSize, SEEK_CUR, filename);
#ifdef TIMER
  timer.endTime(__FUNCTION__);
#endif
  return OPERATION_SUCCESSFUL;
}

int POSIXMetadataManager::updateFpPosition(FILE *fh, long int offset,
                                           int origin, const char * filename) {
#ifdef TIMER
  Timer timer = Timer(); timer.startTime();
#endif
#ifdef RELEASE
  if(fh == nullptr) return UPDATE_FILE_POINTER_FAILED;
#endif /*RELEASE*/

  auto index = pointer.find(fh);
  if(index == pointer.end()) return UPDATE_FILE_POINTER_FAILED;
#ifdef RELEASE
  const char * filename = getFilename(fh);
  if(filename == nullptr) return FILENAME_DOES_NOT_EXIST;
#endif /*RELEASE*/

  switch(origin){
    case SEEK_SET:
      pointer[index->first] = 0+offset;
#ifdef RELEASE
    if(pointer[index->first] <0 || pointer[index->first] > getFilesize(filename))
        return UPDATE_FILE_POINTER_FAILED;
#endif /*RELEASE*/
      break;
    case SEEK_CUR:
      pointer[index->first] += offset;
#ifdef RELEASE
    if(pointer[index->first] <0 || pointer[index->first] > getFilesize(filename))
        return UPDATE_FILE_POINTER_FAILED;
#endif /*RELEASE*/
      break;
    case SEEK_END:
      pointer[index->first] = getFilesize(filename) + offset;
#ifdef RELEASE
    if(pointer[index->first] <0 || pointer[index->first] > getFilesize(filename))
        return UPDATE_FILE_POINTER_FAILED;
#endif /*RELEASE*/
      break;
    default:
      fprintf(stderr, "Seek origin fault!\n");
      return UPDATE_FILE_POINTER_FAILED;
  }
#ifdef TIMER
  timer.endTime(__FUNCTION__);
#endif
  return OPERATION_SUCCESSFUL;
}

long int POSIXMetadataManager::getFpPosition(FILE *fh) {
#ifdef TIMER
  Timer timer = Timer(); timer.startTime();
#endif
#ifdef RELEASE
  if(fh == nullptr) return -1;
#endif /*RELEASE*/
  auto index = pointer.find(fh);
#ifdef TIMER
  timer.endTime(__FUNCTION__);
#endif
  if(index == pointer.end()) return -1;
  else return index->second; //holds the offset
}

/******************************************************************************
*Functions
******************************************************************************/
long int POSIXMetadataManager::getFilesize(const char *filename) {
#ifdef TIMER
  Timer timer = Timer(); timer.startTime();
#endif
#ifdef RELEASE
  if(!checkIfFileExists(filename) || std::strlen(filename) > MAX_FILENAME_LENGTH)
    return -1;
#endif /*RELEASE*/
  auto iterator = created_files.find(filename);
  if(iterator == created_files.end()) return -1;
#ifdef TIMER
  timer.endTime(__FUNCTION__);
#endif
  return iterator->second.st_size;
}








