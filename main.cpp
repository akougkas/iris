
#include <mpi.h>
#include "testing/local_tests.h"
#include "src/config/ConfigurationManager.h"
#include "src/S3.h"

int main(int argc, char *args[]) {

  int rank,comm_size;
  MPI_Init(&argc, &args);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
  std::string tracefile = args[1];
  std::string mode = args[2];
  //int threshHoldInMB=atoi(args[4]);

  if(mode=="PFS") {
    std::string pathToFile = args[3];
    if(rank==0) local_tests::prepare_data_pfs(pathToFile,comm_size);
    MPI_Barrier(MPI_COMM_WORLD);
    local_tests::replay_trace_pfs(tracefile,pathToFile,1,rank);
  }else if(mode=="MONGO"){
    local_tests::mongo_init(rank);
    local_tests::prepare_data_mongo(tracefile,rank);
    local_tests::replay_trace_mongo(tracefile,rank);
    /*ConfigurationManager::getInstance()->OBJECT_MAPPING_MODE =
        object_mapping_modes::OPTIMIZED;
    ConfigurationManager::getInstance()->PVFS2CLIENT_MODE =
        pvfs_client_modes::NON_BLOCKING;
    ConfigurationManager::getInstance()->MEMTABLE_CAPACITY = 32*1024*1024;
    local_tests::prepare_data_iris_o2f(tracefile,rank);
    local_tests::replay_trace_iris_o2f(tracefile,rank);
    iris::flushData();*/
  }else if(mode=="IRIS_O2F"){
    local_tests::prepare_data_iris_o2f(tracefile,rank);
    local_tests::replay_trace_iris_o2f(tracefile,rank);
  }else if(mode=="IRIS_F2O"){
    std::string pathToFile = args[3];
    ConfigurationManager::getInstance()->FILE_MAPPING_MODE =
        file_mapping_modes ::BALANCED;
    ConfigurationManager::getInstance()->PVFS2CLIENT_MODE =
        pvfs_client_modes::BLOCKING;
    local_tests::prepare_data_iris_f2o(tracefile,pathToFile,1,rank);
    local_tests::replay_trace_iris_f2o(tracefile,pathToFile,1,rank);
  }else if(mode=="COPYPFS2KVS"){
    std::string pathToFile = args[3];
    local_tests::copy_data_PFS2KVS(tracefile,pathToFile,rank, comm_size);
  }else if(mode=="COPYKVS2PFS"){
    std::string pathToFile = args[3];
    local_tests::copy_data_KVS2PFS(tracefile,pathToFile,rank, comm_size);
  }else if(mode=="F2OCOST"){
    for(int i =0;i<5;++i){
      local_tests::mapping_cost(131072);
    }
  }
  else if(mode=="F2OPREFECTH") {
    local_tests::mongo_init(rank);
    std::string pathToFile = args[3];
    local_tests::prepare_data_iris_f2o(tracefile,pathToFile,1,rank);
    local_tests::replay_trace_iris_f2o(tracefile,pathToFile,1,rank);
  }
  else if(mode=="HYBRID") {
    local_tests::mongo_init(rank);
    std::string pathToFile = args[3];
    local_tests::prepare_data_iris_hybrid(tracefile,pathToFile,1,rank);
    local_tests::replay_trace_iris_hybrid(tracefile,pathToFile,1,rank);
    iris::flushData();
  }
  MPI_Finalize();

  return 0;
}





