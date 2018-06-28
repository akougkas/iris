/*******************************************************************************
* File HyperdexClientPutOptimised.h
*
* Goal:
*
* Created: December 10th, 2016  - Anthony Kougkas
 * Updated:
* Illinois Institute of Technology - SCS Lab
* (C) 2016
******************************************************************************/
#ifndef IRIS_HYPERDEXCLIENTPUTOPTIMISED_H
#define IRIS_HYPERDEXCLIENTPUTOPTIMISED_H
/******************************************************************************
*include files
******************************************************************************/
#include "HyperdexClient.h"
/******************************************************************************
*Class
******************************************************************************/
class HyperdexClientPutOptimised : public HyperdexClient{
private:
/******************************************************************************
*Variables and members
******************************************************************************/
  static std::shared_ptr<HyperdexClientPutOptimised> instance;
/******************************************************************************
*Constructor
******************************************************************************/
  HyperdexClientPutOptimised():HyperdexClient(){}
public:
/******************************************************************************
*Getters and setters
******************************************************************************/
  static std::shared_ptr<HyperdexClientPutOptimised> getInstance(){
    return instance == nullptr ?
        instance = std::shared_ptr<HyperdexClientPutOptimised>
            (new HyperdexClientPutOptimised()) : instance;
  }
/******************************************************************************
*Interface
******************************************************************************/
  int put(std::shared_ptr<Key> &key) override;
/******************************************************************************
*Destructor
******************************************************************************/
  virtual ~HyperdexClientPutOptimised(){}
};

#endif //IRIS_HYPERDEXCLIENTPUTOPTIMISED_H
