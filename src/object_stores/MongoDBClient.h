/*******************************************************************************
* File MongoDBClient.h
*
* Goal:
*
* Created: January 25th, 2017  - Anthony Kougkas
 * Updated:
* Illinois Institute of Technology - SCS Lab
* (C) 2016
******************************************************************************/
#ifndef IRIS_MONGODBCLIENT_H
#define IRIS_MONGODBCLIENT_H
/******************************************************************************
*include files
******************************************************************************/
#include <memory>
#include <vector>
#include "AbstractObjectStore.h"
#include "../config/Components.h"
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/client.hpp>

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::finalize;
/******************************************************************************
*Class
******************************************************************************/
class MongoDBClient : public AbstractObjectStore {
protected:
  mongocxx::client client;
/******************************************************************************
*Constructor
******************************************************************************/
  MongoDBClient();
private:
  static std::shared_ptr<MongoDBClient> instance;
/******************************************************************************
*Variables and members
******************************************************************************/
  std::string filename;
  std::unordered_map<std::string,std::string> objectID;
/******************************************************************************
*Init function
******************************************************************************/
  int init();
public:
/******************************************************************************
*Getters and setters
******************************************************************************/
  static std::shared_ptr<MongoDBClient> getInstance(){
    return instance == nullptr ? instance = std::shared_ptr<MongoDBClient>
        (new MongoDBClient()) : instance;
  }
/******************************************************************************
*Interface
******************************************************************************/
  int get(std::shared_ptr<Key> &key) override;
  int put(std::shared_ptr<Key> &key) override;
  int remove(std::shared_ptr<Key> &key) override;
/******************************************************************************
*Destructor
******************************************************************************/
  virtual ~MongoDBClient();
};

#endif //IRIS_MONGODBCLIENT_H








