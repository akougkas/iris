/*******************************************************************************
* File HyperdexClientGetOptimised.h
*
* Goal:
*
* Created: December 10th, 2016  - Anthony Kougkas
 * Updated:
* Illinois Institute of Technology - SCS Lab
* (C) 2016
******************************************************************************/
#ifndef IRIS_HYPERDEXCLIENTGETOPTIMISED_H
#define IRIS_HYPERDEXCLIENTGETOPTIMISED_H
/******************************************************************************
*include files
******************************************************************************/
#include "HyperdexClient.h"
/******************************************************************************
*Class
******************************************************************************/
class HyperdexClientGetOptimised : public HyperdexClient{
private:
/******************************************************************************
*Variables and members
******************************************************************************/
  static std::shared_ptr<HyperdexClientGetOptimised> instance;
/******************************************************************************
*Constructor
******************************************************************************/
  HyperdexClientGetOptimised():HyperdexClient(){}
public:
/******************************************************************************
*Getters and setters
******************************************************************************/
  static std::shared_ptr<HyperdexClientGetOptimised> getInstance(){
      return instance == nullptr
             ? instance = std::shared_ptr<HyperdexClientGetOptimised>
              (new HyperdexClientGetOptimised()) : instance;
  }
/******************************************************************************
*Destructor
******************************************************************************/
  virtual ~HyperdexClientGetOptimised(){}
};


#endif //IRIS_HYPERDEXCLIENTGETOPTIMISED_H
