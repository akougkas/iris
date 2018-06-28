//
// Created by anthony on 12/24/16.
//

#ifndef IRIS_LOCAL_TESTS_H
#define IRIS_LOCAL_TESTS_H

#include <string>
#include <stdio.h>
#include <vector>
#include <mongocxx/instance.hpp>
#include <mongocxx/client.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/stdx.hpp>
#include <unordered_map>
#include "../src/config/constants.h"

class local_tests {
public:
  static const size_t HYBRID_THRESHHOLD=20*1024;
  static FILE * open(const char *name, const char *mode);
  static int close(FILE *fh);
  static size_t read(void* ptr, FILE *fh, size_t amount);
  static size_t write(void* ptr, FILE *fh, size_t amount);
  static int seek(FILE *fh, size_t amount);

  static int prepare_data(std::string path, std::string traceName);
  static double copy_data_PFS2KVS(std::string traceFile, std::string
  filePath,int rank, int commSize);
  static double copy_data_KVS2PFS(std::string traceFile, std::string
  filePath,int rank, int commSize);
  static int replay_trace(std::string path, std::string traceName,
                          char * filename, int repetitions, int rank);
  static int metadata(int num_operations, std::string path_to_file);
  static int metadata_mpi(int num_operations, std::string path_to_file, int rank);
  static int mapping_cost(int numOperations);
  /*static double obj2filesPut(size_t numObjects, size_t objectSize);
  static double obj2filesGet(size_t numObjects, size_t objectSize);

  static int testMappingObj(u_int16_t  opType,
                            size_t opSize,
                            size_t totalSize,
                            u_int16_t mode,
                            u_int16_t async);*/
  static int prepare_data_pfs(std::string filename,int no_of_ranks);
  static int replay_trace_pfs(std::string traceFile,
                              std::string filename, int repetitions, int rank);
  static int mongo_init(int rank);
  static int prepare_data_mongo(std::string traceFile, int rank);
  static int replay_trace_mongo(std::string traceFile, int rank);
  static int prepare_data_iris_f2o(std::string traceFile,
                                   std::string filename, int repetitions, int rank);
  static int replay_trace_iris_f2o(std::string traceFile,
                              std::string filename, int repetitions, int rank);
  static int prepare_data_iris_o2f(std::string traceFile, int rank);
  static int replay_trace_iris_o2f(std::string traceFile, int rank);

  static int prepare_data_iris_hybrid(std::string traceFile,
                                   std::string filename, int repetitions, int rank);
  static int replay_trace_iris_hybrid(std::string traceFile,
                                   std::string filename, int repetitions, int rank);


private:
  static std::unordered_map<std::string,std::string> keyToID;
  static char *randstring(long length);
  //static void emptyDirectory();



};

#endif //IRIS_LOCAL_TESTS_H
