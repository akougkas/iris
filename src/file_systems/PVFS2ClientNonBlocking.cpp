/******************************************************************************
*include files
******************************************************************************/
#include <iostream>
#include <unordered_set>
#include "PVFS2ClientNonBlocking.h"
#include "../utils/tools/Buffer.h"

/******************************************************************************
*Initialization of static members
******************************************************************************/
std::shared_ptr<PVFS2ClientNonBlocking> PVFS2ClientNonBlocking::instance = nullptr;

/******************************************************************************
*Functions
******************************************************************************/
int PVFS2ClientNonBlocking::flushToDisk(const MemTable& _memTable) {
  for(auto iterator = _memTable.memTable.begin();
       iterator != _memTable.memTable.end(); ++iterator) {
    auto file = open(iterator->second);
    auto object = iterator->first;
    fseek(file.fh, object.containerOffset, SEEK_SET);
    fwrite(object.data, sizeof(char), object.size, file.fh);
    for (auto linkedObject:object.linkedObjects) {
      fseek(file.fh, linkedObject.containerOffset, SEEK_SET);
      fwrite(linkedObject.data, sizeof(char), linkedObject.size, file.fh);
    }
    close(iterator->second);
  }
  backupMemTable.reset();
  return OPERATION_SUCCESSFUL;
}

int PVFS2ClientNonBlocking::readFromDisk(std::string containerName,
                                         VirtualObject &object) {
  auto file = open(containerName);
  fseek(file.fh, object.containerOffset, SEEK_SET);
  Buffer buffer = Buffer(object.size);
  auto readBytes = fread(buffer.data(), sizeof(char), object.size, file.fh);
  if (readBytes != object.size) {
    return FILE_READ_FAILED;
  }
  for (auto linkedObject:object.linkedObjects) {
    Buffer linkedBuffer = Buffer(linkedObject.size);
    fseek(file.fh, linkedObject.containerOffset, SEEK_SET);
    readBytes = fread(linkedBuffer.data(), sizeof(char), object.size, file.fh);
    if (readBytes != linkedObject.size) {
      return FILE_READ_FAILED;
    }
    buffer.append(linkedBuffer);
  }
  object.data = buffer.data();
  close(containerName);
  return OPERATION_SUCCESSFUL;
}

/******************************************************************************
*Interface
******************************************************************************/
int
PVFS2ClientNonBlocking::read(std::string containerName, VirtualObject &object) {
#ifdef TIMER_I
  Timer timer = Timer(); timer.startTime();
#endif
  auto memtableIterator = activeMemTable.memTable.find(object);
  if (memtableIterator == activeMemTable.memTable.end()) {
    if (asyncFlushing.valid()) asyncFlushing.wait();//wait for flushing to end
    return readFromDisk(containerName, object);
  } else {
    object.data = memtableIterator->first.data;
  }
#ifdef TIMER_I
  timer.endTime(__FUNCTION__);
#endif
  return OPERATION_SUCCESSFUL;
}

int PVFS2ClientNonBlocking::write(std::string containerName,
                                  const VirtualObject &object) {
#ifdef TIMER_I
  Timer timer = Timer(); timer.startTime();
#endif
  if(activeMemTable.size + object.size >
     ConfigurationManager::getInstance()->MEMTABLE_CAPACITY) {
    if (asyncFlushing.valid()) asyncFlushing.wait();//wait for flushing to end
    auto findObjects = activeMemTable.memTable.find(object);
    if(findObjects==activeMemTable.memTable.end()) {
      activeMemTable.erase(object);
    }
    activeMemTable.insert(object, containerName);
    MemTable temp = activeMemTable;
    activeMemTable = backupMemTable;
    backupMemTable = temp;
    auto classInstance = instance;
    asyncFlushing = std::async(std::launch::async,
                               &PVFS2ClientNonBlocking::flushToDisk,
                               classInstance, backupMemTable);
  } else {
    auto findObjects = activeMemTable.memTable.find(object);
    if(findObjects==activeMemTable.memTable.end()) {
      activeMemTable.erase(object);
    }
    activeMemTable.insert(object, containerName);
  }
#ifdef TIMER_I
  timer.endTime(__FUNCTION__);
#endif
  return OPERATION_SUCCESSFUL;
}

void PVFS2ClientNonBlocking::flush() {
  if (asyncFlushing.valid()) asyncFlushing.wait();
  if(flushToDisk(activeMemTable)==OPERATION_SUCCESSFUL) activeMemTable.reset();
}





