/*******************************************************************************
* File POSIXMetadataManager.h
*
* Goal:
*
* Created: December 10th, 2016  - Anthony Kougkas
 * Updated:
* Illinois Institute of Technology - SCS Lab
* (C) 2016
******************************************************************************/
#ifndef IRIS_POSIXMETADATAMANAGER_H
#define IRIS_POSIXMETADATAMANAGER_H
/******************************************************************************
*include files
******************************************************************************/
#include <unistd.h>
#include <ctime>
#include <unordered_map>
#include "IrisMetadataManager.h"
#include "../utils/google_btree/btree_set.h"
#include <stdint.h>
/******************************************************************************
*Class
******************************************************************************/
class POSIXMetadataManager: public IrisMetadataManager {
private:
/******************************************************************************
*Variables and members
******************************************************************************/
  /*filename plus metadata information*/
  struct Stat {
    bool            opened;     /*flag if the file is opened*/
    const char*     mode;       /* mode */
    uid_t           st_uid;     /* user ID of owner */
    gid_t           st_gid;     /* group ID of owner */
    size_t          st_size;    /* total size, in bytes */
    time_t          atime;      /* time of last access */
    time_t          mtime;      /* time of last modification */
    time_t          ctime;      /* time of last status change */
  };
  std::unordered_map<const char *, Stat> created_files;
  /*file handler to filename mapping*/
  std::unordered_map<FILE *, const char *> fh2filename;
  /*file position for every file handler that is opened*/
  std::unordered_map<FILE *, long int> pointer;
public:
/******************************************************************************
*Constructor
******************************************************************************/
  POSIXMetadataManager() :created_files(), fh2filename(), pointer(){}
/******************************************************************************
*Interface
******************************************************************************/
  bool checkIfFileExists(const char *filename);
  bool checkIfFileIsOpen(const char *filename);
  const char * getFilename(FILE *fh);
  long int getFilesize(const char * filename);
  int createMetadata(FILE * fh, const char * filename, const char* mode);
  int updateMetadataOnOpen(FILE * fh, const char * filename, const char* mode);
  int updateMetadataOnClose(FILE * fh, const char * filename);
  int updateMetadataOnRead(FILE * fh, std::size_t operationSize);
  int updateMetadataOnWrite(FILE *fh, std::size_t operationSize);
  int updateFpPosition(FILE * fh, long int offset, int origin, const char*
  filename);
  long int getFpPosition(FILE * fh);

/******************************************************************************
*Destructor
******************************************************************************/
  virtual ~POSIXMetadataManager(){
    //TODO: serialize structures with flatbuffers and send down to ObjectStore
  }
};

#endif //IRIS_POSIXMETADATAMANAGER_H
