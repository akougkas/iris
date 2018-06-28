/******************************************************************************
*include files
******************************************************************************/
#include "ObjectStoreFactory.h"
#include "HyperdexClient.h"
#include "HyperdexClientGetOptimised.h"
#include "HyperdexClientPutOptimised.h"
#include "MongoDBClient.h"
/******************************************************************************
*Initialization of static
******************************************************************************/
std::shared_ptr<ObjectStoreFactory> ObjectStoreFactory::instance = nullptr;
/******************************************************************************
*Getters and setters
******************************************************************************/
std::shared_ptr<AbstractObjectStore>
ObjectStoreFactory::getObjectStore(const std::string name) {
  auto iter = objectStoreMap.find(name);
  if(iter != objectStoreMap.end()) return iter->second;
  else {
    std::shared_ptr<AbstractObjectStore> objectStoreClient;
    if (name == HYPERDEX_CLIENT_GET_OPTIMISED) {
      objectStoreClient = std::static_pointer_cast<HyperdexClientGetOptimised>
          (HyperdexClientGetOptimised::getInstance());
    } else if (name == HYPERDEX_CLIENT_PUT_OPTIMISED) {
      objectStoreClient = std::static_pointer_cast<HyperdexClientPutOptimised>
          (HyperdexClientPutOptimised::getInstance());
    } else if (name ==ConfigurationManager::getInstance()->HYPERDEX_CLIENT) {
      objectStoreClient = std::static_pointer_cast<HyperdexClient>
          (HyperdexClient::getInstance());
    } else if (name == MONGO_CLIENT) {
      objectStoreClient = std::static_pointer_cast<MongoDBClient>
          (MongoDBClient::getInstance());
    }
    objectStoreMap.emplace(name, objectStoreClient);
    return objectStoreClient;
  }
}
