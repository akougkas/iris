/*******************************************************************************
* File LFU.h
*
* Goal: Least Frequently Used cache replacement policy
*
* Created: December 10th, 2016  - Anthony Kougkas
 * Updated:
* Illinois Institute of Technology - SCS Lab
* (C) 2016
******************************************************************************/
#ifndef IRIS_LFU_H
#define IRIS_LFU_H
/******************************************************************************
*include files
******************************************************************************/
#include <vector>
#include <queue>
#include <map>
#include "AbstractReplacementPolicy.h"
#include "../google_btree/btree_set.h"
#include "../../config/ConfigurationManager.h"
/******************************************************************************
*Class
******************************************************************************/
class LFU: public AbstractReplacementPolicy {
private:
/******************************************************************************
*Variables and members
******************************************************************************/
  struct Node {
    std::string key; // key of the element.
    void* val; // value of the element.
    int frequency; // usage frequency
    int timeStamp; // the latest time stamp when this element is accessed.
    Node(): key(NULL), val(nullptr), timeStamp(-1), frequency(0) {}
    //TODO: check the initilization
    Node(std::string k, void* v, int ts): key(k), val(v), timeStamp(ts),
                                          frequency(0) {}
  };
  std::vector<Node*> pq;
  std::unordered_map<std::string, unsigned long> mp;
  std::size_t capacity=ConfigurationManager::getInstance()->CACHE_CAPACITY;
  int ts;
protected:
  std::unordered_map<Key,void*> cacheMap;
/******************************************************************************
*Functions
******************************************************************************/
private:
  void* get(std::string key);
  void set(std::string key, void* value);
  void sink(unsigned long index);
  void swim(unsigned long index);
  void myswap(unsigned long id1, unsigned long id2);
public:
/******************************************************************************
*Interface
******************************************************************************/
  virtual int isCached(std::shared_ptr<Key> &key) override;
  int addToCache(std::shared_ptr<Key> &key) override;
/******************************************************************************
*Constructor
******************************************************************************/
  LFU():AbstractReplacementPolicy(){}
/******************************************************************************
*Destructor
******************************************************************************/
  virtual ~LFU(){}
};
#endif //IRIS_LFU_H
