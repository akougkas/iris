/*******************************************************************************
* File ObjectStoreFactory.h
*
* Goal:
*
* Created: December 10th, 2016  - Anthony Kougkas
 * Updated:
* Illinois Institute of Technology - SCS Lab
* (C) 2016
******************************************************************************/
#ifndef IRIS_OBJECTSTOREFACTORY_H
#define IRIS_OBJECTSTOREFACTORY_H
/******************************************************************************
*include files
******************************************************************************/
#include <memory>
#include "AbstractObjectStore.h"
/******************************************************************************
*Class
******************************************************************************/
class ObjectStoreFactory {
private:
/******************************************************************************
*Variables and members
******************************************************************************/
  static std::shared_ptr<ObjectStoreFactory> instance;
  std::unordered_map<std::string, std::shared_ptr<AbstractObjectStore>>
      objectStoreMap;
/******************************************************************************
*Constructors
******************************************************************************/
  ObjectStoreFactory(){}
public:
/******************************************************************************
*Getters and setters
******************************************************************************/
  inline static std::shared_ptr<ObjectStoreFactory> getInstance(){
    return instance == nullptr ? instance = std::shared_ptr<ObjectStoreFactory>
        (new ObjectStoreFactory()) : instance;
  }
  std::shared_ptr<AbstractObjectStore> getObjectStore(const std::string name);
/******************************************************************************
*Destructor
******************************************************************************/
  virtual ~ObjectStoreFactory(){}
};


#endif //IRIS_OBJECTSTOREFACTORY_H
