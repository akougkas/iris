/******************************************************************************
*include files
******************************************************************************/
#include "PrefetcherFactory.h"
#include "../../config/constants.h"
#include "FileSystemPrefetcher.h"
#include "ObjectStorePrefetcher.h"
/******************************************************************************
*Initialization of static
******************************************************************************/
std::shared_ptr<PrefetcherFactory> PrefetcherFactory::instance = nullptr;
/******************************************************************************
*Gettters and setters
******************************************************************************/
std::shared_ptr<AbstractPrefetcher>
PrefetcherFactory::getPrefetcher(const std::string name) {
  auto iter = prefetcherMap.find(name);
  if(iter != prefetcherMap.end()) return iter->second;
  else {
    std::shared_ptr<AbstractPrefetcher> prefetcherInstance;
    if (name == FILESYSTEM_PREFETCHER) {
      prefetcherInstance =
          std::shared_ptr<AbstractPrefetcher>(new FileSystemPrefetcher());
    } else if (name == OBJECTSTORE_PREFETCHER) {
      prefetcherInstance =
          std::shared_ptr<AbstractPrefetcher>(new ObjectStorePrefetcher());
    } else return nullptr;
    prefetcherMap.emplace(name, prefetcherInstance);
    return std::shared_ptr<AbstractPrefetcher>(prefetcherInstance);
  }
}


