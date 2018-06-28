/*******************************************************************************
* File AggregatorFactory.h
*
* Goal:
*
* Created: December 10th, 2016  - Anthony Kougkas
 * Updated:
* Illinois Institute of Technology - SCS Lab
* (C) 2016
******************************************************************************/
#ifndef IRIS_AGGREGATORFACTORY_H
#define IRIS_AGGREGATORFACTORY_H
/******************************************************************************
*include files
******************************************************************************/
#include "../prefetching/AbstractPrefetcher.h"
#include "AbstractAggregator.h"
#include <memory>
#include <unordered_map>
/******************************************************************************
*Class
******************************************************************************/
class AggregatorFactory {
private:
/******************************************************************************
*Constructor
******************************************************************************/
  AggregatorFactory(){}
/******************************************************************************
*Private members
******************************************************************************/
  static std::shared_ptr<AggregatorFactory> instance;
  std::unordered_map<std::string, std::shared_ptr<AbstractAggregator>>
      aggregatorMap;
public:
/******************************************************************************
*Gettters and setters
******************************************************************************/
  inline static std::shared_ptr<AggregatorFactory> getInstance(){
    return instance== nullptr ? instance=std::shared_ptr<AggregatorFactory>
        (new AggregatorFactory()) : instance;
  }
  std::shared_ptr<AbstractAggregator> getAggregator(const std::string name);
/******************************************************************************
*Destructor
******************************************************************************/
  virtual ~AggregatorFactory(){}
};


#endif //IRIS_AGGREGATORFACTORY_H


