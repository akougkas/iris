/*******************************************************************************
* File POSIXMapperGetOptimised.h
*
* Goal:
*
* Created: December 10th, 2016  - Anthony Kougkas
 * Updated:
* Illinois Institute of Technology - SCS Lab
* (C) 2016
******************************************************************************/
#ifndef IRIS_POXISMAPPERGETOPTIMISED_H
#define IRIS_POXISMAPPERGETOPTIMISED_H
/******************************************************************************
*include files
******************************************************************************/
#include <unordered_set>
#include "POSIXMapper.h"
/******************************************************************************
*Class
******************************************************************************/
class POSIXMapperGetOptimised : public POSIXMapper {
private:
/******************************************************************************
*Variables and members
******************************************************************************/
  static std::shared_ptr<POSIXMapperGetOptimised> instance;
  std::unordered_set<Range> keyRange;
/******************************************************************************
*Functions
******************************************************************************/
  std::vector<std::shared_ptr<Key>>
  generateKeysForPut(const char *name, long offset, size_t size);

  std::vector<std::shared_ptr<Key>>
  generateKeysForGet(const char *name, long offset, size_t size);
/******************************************************************************
*Constructor
******************************************************************************/
  POSIXMapperGetOptimised(){
    keyRange=std::unordered_set<Range>();
  }
public:
/******************************************************************************
*Interface
******************************************************************************/
  std::vector<std::shared_ptr<Key>>
  generateKeys(const char *name, long int offset,
               size_t size, operation operationType) override;
/******************************************************************************
*Getters and setters
******************************************************************************/
  inline static std::shared_ptr<POSIXMapperGetOptimised> getInstance(){
    return instance== nullptr ? instance = std::shared_ptr<POSIXMapperGetOptimised>
        (new POSIXMapperGetOptimised()) : instance;
  }
/******************************************************************************
*Destructor
******************************************************************************/
  virtual ~POSIXMapperGetOptimised(){}
};
#endif //IRIS_POXISMAPPERGETOPTIMISED_H
