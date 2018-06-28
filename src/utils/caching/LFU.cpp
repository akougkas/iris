/******************************************************************************
*include files
******************************************************************************/
#include <regex>
#include "LFU.h"

/******************************************************************************
*Interface
******************************************************************************/
int LFU::isCached(std::shared_ptr<Key> &key) {
#ifdef TIMER
  Timer timer = Timer(); timer.startTime();
#endif
  auto data = get(key->name);
  if(data == nullptr) return NO_CACHED_DATA_FOUND;
  key->data = data;
#ifdef TIMER
  timer.endTime(__FUNCTION__);
#endif
  return OPERATION_SUCCESSFUL;
}

int LFU::addToCache(std::shared_ptr<Key> &key) {
#ifdef TIMER
  Timer timer = Timer(); timer.startTime();
#endif
  set(key->name,key->data);
#ifdef TIMER
  timer.endTime(__FUNCTION__);
#endif
  return OPERATION_SUCCESSFUL;
}
/******************************************************************************
*Functions
******************************************************************************/
void* LFU::get(std::string key) {
  if(!mp.count(key)) return nullptr;
  unsigned long index = mp[key];
  void* val = pq[index]->val;
  pq[index]->frequency++;
  pq[index]->timeStamp = ++ts;
  sink(index);
  return val;
}

void LFU::set(std::string key, void* value) {
  if(capacity <= 0) return;
  if(mp.count(key)) {
    unsigned long index = mp[key];
    pq[index]->val = value;
    get(key);
  }
  else {
    if (pq.size() - 1 == capacity) {
      std::string oldKey = pq[1]->key;
      mp.erase(oldKey);
      Node *newnode = new Node(key, value, ++ts);
      pq[1] = newnode;
      mp[key] = 1;
      sink(1);
    } else {
      Node *newnode = new Node(key, value, ++ts);
      pq.push_back(newnode);
      mp[key] = (pq.size() - 1);
      swim(pq.size() - 1);

    }
  }
}
void LFU::sink(unsigned long index) {
  unsigned long left = 2 * index, right = 2 * index + 1, target = index;
  if(left < pq.size() && pq[left]->frequency <= pq[target]->frequency)
    target = left;
  if(right < pq.size()) {
    if(pq[right]->frequency < pq[target]->frequency || (pq[right]->frequency == pq[target]->frequency && pq[right]->timeStamp < pq[target]->timeStamp))
      target = right;
  }
  if(target != index) {
    myswap(target, index);
    sink(target);
  }
}

/*
 * Recursively swim a node in priority queue. A node will be swimmed, when its frequency is less than its
 * parent node. If the node has the same frequency with its parent, it is not needed to be swimmed, because
 * it is recently accessed.
 */
void LFU::swim(unsigned long index) {
  unsigned long par = index / 2;
  while(par > 0 && pq[par]->frequency > pq[index]->frequency) {
    myswap(par, index);
    index = par;
    par /= 2;
  }
}

void LFU::myswap(unsigned long id1, unsigned long id2) {
  std::swap(pq[id1], pq[id2]);
  mp[pq[id1]->key] = id1;
  mp[pq[id2]->key] = id2;
}

