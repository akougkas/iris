/*******************************************************************************
* File MapperFactory.h
*
* Goal:
*
* Created: December 10th, 2016  - Anthony Kougkas
 * Updated:
* Illinois Institute of Technology - SCS Lab
* (C) 2016
******************************************************************************/
#ifndef IRIS_MAPPERFACTORY_H
#define IRIS_MAPPERFACTORY_H
/******************************************************************************
*include files
******************************************************************************/
#include <memory>
#include <unordered_map>
#include "AbstractMapper.h"
/******************************************************************************
*Class
******************************************************************************/
class MapperFactory {
private:
/******************************************************************************
*Constructor
******************************************************************************/
  MapperFactory(): mapperMap(){}
/******************************************************************************
*Private members
******************************************************************************/
  static std::shared_ptr<MapperFactory> instance;
  std::unordered_map<std::string, std::shared_ptr<AbstractMapper>>
      mapperMap;
public:
/******************************************************************************
*Gettters and setters
******************************************************************************/
  inline static std::shared_ptr<MapperFactory> getInstance(){
    return instance== nullptr ? instance = std::shared_ptr<MapperFactory>
        (new MapperFactory()) : instance;
  }
  std::shared_ptr<AbstractMapper> getMapper(const std::string name);
/******************************************************************************
*Destructor
******************************************************************************/
  virtual ~MapperFactory(){
    //mapperMap.clear();
  }
};


#endif //IRIS_MAPPERFACTORY_H
