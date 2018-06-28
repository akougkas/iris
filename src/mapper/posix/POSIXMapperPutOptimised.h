    /*******************************************************************************
* File POSIXMapperPutOptimised.h
*
* Goal:
*
* Created: December 10th, 2016  - Anthony Kougkas
 * Updated:
* Illinois Institute of Technology - SCS Lab
* (C) 2016
******************************************************************************/
#ifndef IRIS_POSIXMAPPERPUTOPTIMISED_H
#define IRIS_POSIXMAPPERPUTOPTIMISED_H
/******************************************************************************
*include files
******************************************************************************/
#include <set>
#include <unordered_set>
#include "../../utils/google_btree/btree.h"
#include "../../utils/google_btree/btree_set.h"
#include "POSIXMapper.h"
/******************************************************************************
*Class
******************************************************************************/
class POSIXMapperPutOptimised : public POSIXMapper{
private:
/******************************************************************************
*Variables and members
******************************************************************************/
  static std::shared_ptr<POSIXMapperPutOptimised> instance;
  btree::btree_set<Range> keySet;
/******************************************************************************
*Functions
******************************************************************************/
  std::vector<std::shared_ptr<Key>> generateKeysForPut(const char *name,
                                                       long offset, size_t size);
  std::vector<std::shared_ptr<Key>> generateKeysForGet(const char *name,
                                                       long offset, size_t size);
/******************************************************************************
*Constructor
******************************************************************************/
  POSIXMapperPutOptimised(){
    keySet=btree::btree_set<Range>();
  }
public:
/******************************************************************************
*Interface
******************************************************************************/
  std::vector<std::shared_ptr<Key>>
  generateKeys(const char *name, long int offset, size_t size,
               operation operationType) override;
/******************************************************************************
*Getters and setters
******************************************************************************/
  inline static std::shared_ptr<POSIXMapperPutOptimised> getInstance(){
    return instance== nullptr ? instance = std::shared_ptr<POSIXMapperPutOptimised>
        (new POSIXMapperPutOptimised()) : instance;
  }
/******************************************************************************
*Destructor
******************************************************************************/
  virtual ~POSIXMapperPutOptimised(){}
};


#endif //IRIS_POSIXMAPPERPUTOPTIMISED_H