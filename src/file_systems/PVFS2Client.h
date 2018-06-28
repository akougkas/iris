/*******************************************************************************
* File PVFS2CLient.h
*
* Goal: This is the PVFS2 client
*
* Created: December 10th, 2016  - Anthony Kougkas
 * Updated:
* Illinois Institute of Technology - SCS Lab
* (C) 2016
******************************************************************************/
#ifndef IRIS_PVFS2CLIENT_H
#define IRIS_PVFS2CLIENT_H
/******************************************************************************
*include files
******************************************************************************/
#include <unordered_map>
#include "AbstractFileSystem.h"
#include "../metadata_manager/MetadataManagerFactory.h"
/******************************************************************************
*Class
******************************************************************************/
class PVFS2Client: public AbstractFileSystem {
private:
  static std::shared_ptr<PVFS2Client> instance;
protected:
/******************************************************************************
*Variables and members
******************************************************************************/
  typedef struct ContainerStat {
    bool            opened;     /*flag if the file is opened*/
    FILE *          fh;         /* file handler */
  } containerStat;
  std::unordered_map<std::string, ContainerStat> createdFiles;
/******************************************************************************
*Functions
******************************************************************************/
  virtual ContainerStat open(std::string containerName);

  virtual int close(std::string containerName);

  virtual int seek(FILE *fh, size_t offset);
/******************************************************************************
*Constructor
******************************************************************************/
  PVFS2Client() :createdFiles() {}
public:
/******************************************************************************
*Getters and setters
******************************************************************************/
  static std::shared_ptr<PVFS2Client> getInstance() {
    return instance == nullptr
           ? instance = std::shared_ptr<PVFS2Client>(new PVFS2Client())
           : instance;
  }
/******************************************************************************
*Interface
******************************************************************************/
  virtual int read(std::string containerName, VirtualObject &object) override;

  virtual int write(std::string containerName, const VirtualObject &object) override;
/******************************************************************************
*Destructor
******************************************************************************/
  virtual ~PVFS2Client(){}

};

#endif //IRIS_PVFS2CLIENT_H

