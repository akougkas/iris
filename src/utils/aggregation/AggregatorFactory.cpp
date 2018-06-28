/******************************************************************************
*include files
******************************************************************************/
#include "AggregatorFactory.h"
#include "../../config/constants.h"
#include "FileSystemAggregator.h"
#include "ObjectStoreAggregator.h"
/******************************************************************************
*Initialization of static
******************************************************************************/
std::shared_ptr<AggregatorFactory> AggregatorFactory::instance = nullptr;
/******************************************************************************
*Gettters and setters
******************************************************************************/
std::shared_ptr<AbstractAggregator>
AggregatorFactory::getAggregator(const std::string name) {
  auto iter = aggregatorMap.find(name);
  if(iter != aggregatorMap.end()) return iter->second;
  else {
    std::shared_ptr<AbstractAggregator> aggregatorInstance;
    if (name == FILESYSTEM_AGGREGATOR) {
      aggregatorInstance =
          std::shared_ptr<AbstractAggregator>(new FileSystemAggregator());
    } else if (name == OBJECTSTORE_AGGREGATOR) {
      aggregatorInstance =
          std::shared_ptr<AbstractAggregator>(new ObjectStoreAggregator());
    } else return nullptr;
    aggregatorMap.emplace(name, aggregatorInstance);
    return aggregatorInstance;
  }
}




