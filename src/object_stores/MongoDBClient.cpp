/******************************************************************************
*include files
******************************************************************************/
#include "MongoDBClient.h"
#include "../utils/tools/Buffer.h"
#include <mpi.h>
#include <bsoncxx/builder/basic/document.hpp>
#include <mongocxx/logger.hpp>
#include <bsoncxx/stdx/make_unique.hpp>
/******************************************************************************
*Initialization of static
******************************************************************************/
std::shared_ptr<MongoDBClient> MongoDBClient::instance = nullptr;
/******************************************************************************
*Constructors
******************************************************************************/
MongoDBClient::MongoDBClient() :objectID(){
  if (init() != OPERATION_SUCCESSFUL) {
    fprintf(stderr, "MongoDB failed to initialize!\n");
    exit(-1);
  }
}
/******************************************************************************
*Destructor
******************************************************************************/
MongoDBClient::~MongoDBClient() {}
/******************************************************************************
*Init function
******************************************************************************/
int MongoDBClient::init() {
#ifdef TIMER
  Timer timer = Timer(); timer.startTime();
#endif
  int rank=0;
  MPI_Comm_rank ( MPI_COMM_WORLD, &rank);
  class noop_logger : public mongocxx::logger {
  public:
    virtual void operator()(mongocxx::log_level,
                            mongocxx::stdx::string_view,
                            mongocxx::stdx::string_view) noexcept {}
  };
  /* Setup the MongoDB client */
  mongocxx::instance instance{mongocxx::stdx::make_unique<noop_logger>()};
  auto uri = mongocxx::uri{ConfigurationManager::getInstance()->MONGO_URI};
  client = mongocxx::client{uri};
  /* make sure the client is OK */
  if (!client) {
    fprintf(stderr, "Cannot create MongoDB client.\n");
    return MONGO_CLIENT_CREATION_FAILED;
  }
  mongocxx::database db = client[MONGO_DATABASE];
  if (!db) {
    fprintf(stderr, "Cannot connect to IRIS MongoDB database.\n");
    return MONGO_DB_CONNECTION_FAILED;
  }
  mongocxx::collection file;
  filename = MONGO_COLLECTION;
  if(rank==0) {
    file = client.database(MONGO_DATABASE).has_collection(filename) ?
           db.collection(filename) :
           db.create_collection(filename);
    std::string enableShardingCmd = "/home/anthony/Dropbox/Projects/iris/scripts/shardCollection.sh "
                                    + MONGO_DATABASE + " " + filename;
    system(enableShardingCmd.c_str());
  }

  MPI_Barrier(MPI_COMM_WORLD);
  file = db.collection(filename);
  if (!file) {
    return MONGO_COLLECTION_CREATION_FAILED;
  }

#ifdef DEBUG
  printf("MongoDB client created.\n\n");
#endif /* DEBUG*/
#ifdef TIMER
  timer.endTime(__FUNCTION__);
#endif
  return OPERATION_SUCCESSFUL;
}
/******************************************************************************
*Interface
******************************************************************************/
int MongoDBClient::get(std::shared_ptr<Key> &key) {
#ifdef TIMER
  Timer timer = Timer(); timer.startTime();
#endif
  try{
  //std::cout<<"get key " << key->name<< " size "<< key->size<<"\n";
  /*retrieve the unique objectID from map*/
  bsoncxx::oid insertedID;
  mongocxx::collection file = client[MONGO_DATABASE].collection(filename);
  auto oidIterator = objectID.find(key->name);
  if(oidIterator == objectID.end()) return MONGO_GET_NOT_FOUND;
  else insertedID =
           bsoncxx::oid{mongocxx::stdx::string_view{oidIterator->second}};

  // Create the query filter
  using bsoncxx::builder::stream::finalize;
  bsoncxx::builder::stream::document filter;
  filter << "_id"
         << bsoncxx::oid{mongocxx::stdx::string_view{insertedID.to_string()}}
         << finalize;
  // Execute the query to find the document
  auto getDoc = file.find_one(filter.view());
  if(getDoc){
    bsoncxx::document::element value = getDoc->view()["value"];
    key->size = value.get_utf8().value.to_string().length();
    key->data = (void *) value.get_utf8().value.to_string().c_str();
  }
  else{
    std::cout << "Document not found!" << "\n";
  }
  }catch (const std::exception& xcp) {
    std::cout << "failed: " << xcp.what() << "\n";
    return MONGO_GET_OPERATION_FAILED;
  }
#ifdef TIMER
  timer.endTime(__FUNCTION__);
#endif
  return OPERATION_SUCCESSFUL;
}

int MongoDBClient::put(std::shared_ptr<Key> &key) {
#ifdef TIMER
  Timer timer = Timer(); timer.startTime();
#endif
  int status = 0;
  std::shared_ptr<Key> originalKey =  std::shared_ptr<Key>(new Key());
  originalKey->name = key->name;
  originalKey->offset = key->offset;
  originalKey->size = key->size;
  originalKey->data = key->data;
  Buffer dataBuffer;
  mongocxx::collection file = client[MONGO_DATABASE].collection(filename);
  try{
  if (key->offset == 0) {
    //std::cout<<"aligned key " << key->name<< " size "<< key->size<<"\n";
    if (key->size != ConfigurationManager::getInstance()->MAX_OBJ_SIZE) {
      status = get(key);
      if (status == OPERATION_SUCCESSFUL && key->size > originalKey->size) {
        dataBuffer = Buffer(key->data, key->size);
        dataBuffer.assign(originalKey->data, originalKey->size);
        key->data = dataBuffer.data();
      }
    }
  } else {
    //std::cout<<"merge key " << key->name<< " size "<< key->size<<"\n";
    status = get(key);
    if (status == OPERATION_SUCCESSFUL) {
      if (key->size > originalKey->offset + originalKey->size) {
        std::memcpy((char *) key->data + originalKey->offset, originalKey->data, originalKey->size);
      } else {
        if(key->size<originalKey->offset){
          dataBuffer = Buffer(key->data, key->size);
        }else{
          dataBuffer = Buffer(key->data, originalKey->offset);
        }
        dataBuffer.append(originalKey->data, originalKey->size);
        key->offset = 0;
        key->size = dataBuffer.size();
        key->data = dataBuffer.data();
      }
    }
  }
  //std::cout<<"put key " << key->name<< " size "<< key->size<<"\n";
  //Using the basic builder, create the kv pair or document
  auto document = bsoncxx::builder::basic::document{};
  using bsoncxx::builder::basic::kvp;
  std::string data((const char*)key->data,key->size);
  std::string keyName(key->name,std::strlen(key->name));
  //std::cout<<"KeyName :"<<keyName<<"\n";
  document.append(kvp("key",keyName),kvp("value",data));

  //adding the created key-value pair to the collection
  bsoncxx::document::view putView =  document.view();//get the view
  //retrieve the unique objectID from map
  auto oidIterator = objectID.find(keyName);
  if(oidIterator != objectID.end()){
    objectID.erase(oidIterator);
  }
  auto add = file.insert_one(putView);//insert it to collection
  if (!add) {
    std::cout << "Unacknowledged write. No id available." << "\n";
    return MONGO_PUT_OPERATION_FAILED;
  }
  if (add->inserted_id().type() == bsoncxx::type::k_oid) {
    bsoncxx::oid id = add->inserted_id().get_oid().value;
    objectID.insert({keyName, id.to_string()});
  } else std::cout << "Inserted id was not an OID type" << "\n";

  }catch (const std::exception& xcp) {
    std::cout << "Exception with failed: " << xcp.what() << "\n";
    return MONGO_PUT_OPERATION_FAILED;
  }
#ifdef TIMER
  timer.endTime(__FUNCTION__);
#endif
  key->offset=originalKey->offset;
  key->size=originalKey->size;
  return OPERATION_SUCCESSFUL;
}

int MongoDBClient::remove(std::shared_ptr<Key> &key) {
  return OPERATION_SUCCESSFUL;
}

