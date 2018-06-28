/*******************************************************************************
* File HyperdexClient.h
*
* Goal:
*
* Created: December 10th, 2016  - Anthony Kougkas
 * Updated:
* Illinois Institute of Technology - SCS Lab
* (C) 2016
******************************************************************************/
#ifndef IRIS_HYPERDEXCLIENT_H
#define IRIS_HYPERDEXCLIENT_H
/******************************************************************************
*include files
******************************************************************************/
#include <memory>
#include <vector>
#include <hyperdex/client.h>
#include "AbstractObjectStore.h"
#include "../config/constants.h"
#include "../utils/caching/CacheManager.h"
#include "../utils/prefetching/PrefetcherFactory.h"
#include "../config/Components.h"
/******************************************************************************
*Class
******************************************************************************/
class HyperdexClient : public AbstractObjectStore {
protected:
  struct hyperdex_client* hyperdexClient;
/******************************************************************************
*Constructor
******************************************************************************/
  HyperdexClient();
private:
  static std::shared_ptr<HyperdexClient> instance;
/******************************************************************************
*Variables and members
******************************************************************************/
  std::shared_ptr<CacheManager> cacheManager;
  std::shared_ptr<PrefetcherFactory> prefetcherFactory;
  struct OperationData{
    int operationType;
    int completionStatus;
    Key* key;
    const hyperdex_client_attribute **attributes;
    size_t *attributes_sz;
  };
  std::unordered_map<int64_t,OperationData> operationToKeyMap;
  int getKey(int64_t operationId,std::shared_ptr<Key> &key);
  int logRequest(int64_t operationId,
                 operation OPERATION_TYPE,
                 std::shared_ptr<Key> &key,
                 const hyperdex_client_attribute **attributes,
                 size_t *attributes_sz);
/******************************************************************************
*Init function
******************************************************************************/
  int init();
public:
/******************************************************************************
*functions
******************************************************************************/
  int refresh_spaces();
  int getKeyFromMap(int64_t operationId, std::shared_ptr<Key> &key);
/******************************************************************************
*Getters and setters
******************************************************************************/
  static std::shared_ptr<HyperdexClient> getInstance(){
    return instance == nullptr ? instance = std::shared_ptr<HyperdexClient>
        (new HyperdexClient()) : instance;
  }
/******************************************************************************
*Interface
******************************************************************************/
  int get(std::shared_ptr<Key> &key) override;
  int put(std::shared_ptr<Key> &key) override;
  int remove(std::shared_ptr<Key> &key) override;
/* Hyperdex specific calls*/
  int getRange(std::vector<std::shared_ptr<Key>> &keys);
  int putRange(std::vector<std::shared_ptr<Key>> &keys);
/******************************************************************************
*Destructor
******************************************************************************/
  virtual ~HyperdexClient();

};

#endif //IRIS_HYPERDEXCLIENT_H
