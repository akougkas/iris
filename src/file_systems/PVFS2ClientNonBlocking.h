/*******************************************************************************
* File PVFS2CLientNonBlocking.h
*
* Goal: This is the PVFS2 client using asynchronous calls
*
* Created: December 10th, 2016  - Anthony Kougkas
 * Updated:
* Illinois Institute of Technology - SCS Lab
* (C) 2016
******************************************************************************/
#ifndef IRIS_PVFS2CLIENTNONBLOCKING_H
#define IRIS_PVFS2CLIENTNONBLOCKING_H
/******************************************************************************
*include files
******************************************************************************/
#include "PVFS2Client.h"
/******************************************************************************
*Class
******************************************************************************/
class PVFS2ClientNonBlocking: public PVFS2Client {
private:
/******************************************************************************
*Variables and members
******************************************************************************/
  static std::shared_ptr<PVFS2ClientNonBlocking> instance;
  MemTable activeMemTable, backupMemTable;
  bool isFlushing;
  std::future<int> asyncFlushing;
/******************************************************************************
*Functions
******************************************************************************/
  int readFromDisk(std::string containerName, VirtualObject &object);
  int flushToDisk(const MemTable &_memTable);
/******************************************************************************
*Constructor
******************************************************************************/
  PVFS2ClientNonBlocking()
      :PVFS2Client(), isFlushing(false), activeMemTable(), backupMemTable(){
  }
public:

/******************************************************************************
*Getters and setters
******************************************************************************/
  static std::shared_ptr<PVFS2ClientNonBlocking> getInstance() {
    return instance == nullptr
           ? instance = std::shared_ptr<PVFS2ClientNonBlocking>
            (new PVFS2ClientNonBlocking()) : instance;
  }
/******************************************************************************
*Interface
******************************************************************************/
  virtual int read(std::string containerName, VirtualObject &object) override;

  virtual int write(std::string containerName, const VirtualObject &object) override;

  virtual void flush() override;
/******************************************************************************
*Destructor
******************************************************************************/
  virtual ~PVFS2ClientNonBlocking(){
    activeMemTable.reset();
    backupMemTable.reset();
  }
};

#endif //IRIS_PVFS2CLIENTNONBLOCKING_H




