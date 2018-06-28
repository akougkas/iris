/*******************************************************************************
* File PrefetcherFactory.h
*
* Goal:
*
* Created: December 10th, 2016  - Anthony Kougkas
 * Updated:
* Illinois Institute of Technology - SCS Lab
* (C) 2016
******************************************************************************/
#ifndef IRIS_PREFETCHERFACTORY_H
#define IRIS_PREFETCHERFACTORY_H
/******************************************************************************
*include files
******************************************************************************/
#include "AbstractPrefetcher.h"
#include <memory>
#include <unordered_map>
/******************************************************************************
*Class
******************************************************************************/
class PrefetcherFactory {
private:
/******************************************************************************
*Constructor
******************************************************************************/
  PrefetcherFactory(){}
/******************************************************************************
*Private members
******************************************************************************/
  static std::shared_ptr<PrefetcherFactory> instance;
  std::unordered_map<std::string, std::shared_ptr<AbstractPrefetcher>>
      prefetcherMap;
public:
/******************************************************************************
*Destructor
******************************************************************************/
  virtual ~PrefetcherFactory(){}
/******************************************************************************
*Gettters and setters
******************************************************************************/
  inline static std::shared_ptr<PrefetcherFactory> getInstance(){
    return instance== nullptr ? instance=std::shared_ptr<PrefetcherFactory>
        (new PrefetcherFactory()) : instance;
  }
  std::shared_ptr<AbstractPrefetcher> getPrefetcher(const std::string name);
};


#endif //IRIS_PREFETCHERFACTORY_H
