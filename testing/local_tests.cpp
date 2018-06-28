//
// Created by anthony on 12/24/16.
//
#include <iostream>
#include <chrono>
#include <iomanip>
#include "../src/iris.h"
#include <mpi.h>
#include "local_tests.h"
#include "../src/utils/tools/Timer.h"
#include "../src/utils/tools/clearcache.h"

#ifdef IRIS

#include "../src/mapper/posix/POSIXMapperGetOptimised.h"
#include "../src/mapper/posix/POSIXMapperPutOptimised.h"

#endif
std::unordered_map<std::string,std::string> local_tests::keyToID =
    std::unordered_map<std::string,std::string>();
char *local_tests::randstring(long length) {
  long n;
  static char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.-#'?!";
  char *randomString = NULL;
  if (length) {
    randomString = (char *) malloc(sizeof(char) * (length + 1));
    if (randomString) {
      for (n = 0; n < length; n++) {
        int key = rand() % (int) (sizeof(charset) - 1);
        randomString[n] = charset[key];
      }
      randomString[length] = '\0';
    }
  }
  return randomString;
}

FILE* local_tests::open(const char* name,const char *mode){
  return fopen(name,mode);
}
int local_tests::close(FILE* fh){
  fclose(fh);
  return 0;
}
size_t local_tests::read(void* ptr, FILE* fh, size_t amount) {
  return fread(ptr, sizeof(char), amount, fh);
}

size_t local_tests::write(void* ptr, FILE* fh, size_t amount) {
  return fwrite(ptr, sizeof(char), amount, fh);
}

int local_tests::seek(FILE* fh,size_t amount) {
  fseek(fh, (amount), SEEK_SET);
  return 0;
}

int local_tests::replay_trace(std::string path, std::string traceName,
                              char * filename, int repetitions, int rank){
  /*Initialization of some stuff*/
  std::string traceFile=path+traceName;
  std::FILE* trace;
  std::FILE* file = nullptr;
  char* line = NULL;
  int comm_size;

  size_t len=0;
  ssize_t readsize;
  std::string operation;
  long offset = 0;
  long request_size = 0;
  char* word;
  line = (char*) malloc(128);
  std::vector<double> timings;
  double average=0;
  int rep =  repetitions;

  /* Do the I/O and comparison*/
  while(rep) {
    /*Opening the trace file*/
    trace = std::fopen(traceFile.c_str(), "r");
    if (trace==NULL) {
      return 0;
    }
    /*system("/home/anthony/Dropbox/ReSearch/Projects/iris/scripts/clearcache"
               ".sh");*/
    /*While loop to read each line from the trace and create I/O*/
    Timer globalTimer = Timer();
    globalTimer.startTime();
    time_t now = time(0);
    char *dt = ctime(&now);
    std::cout << traceName << "," << dt;
    int lineNumber=0;
    while ((readsize = getline(&line, &len, trace)) != -1) {
      if (readsize < 4) {
        break;
      }
      word = strtok(line, ",");
      operation = word;
      word = strtok(NULL, ",");
      offset = atol(word);
      word = strtok(NULL, ",");
      request_size = atol(word);


      Timer operationTimer = Timer();
      operationTimer.startTime();
      if (operation == "FOPEN") {
        file = open(filename, "w+");
      } else if (operation == "FCLOSE") {
        close(file);
      } else if (operation == "WRITE") {
        char* writebuf = randstring(request_size);
        std::cout <<writebuf<<"\n";
        seek(file, (size_t) offset);
        write(writebuf, file, (size_t) request_size);
        if(writebuf) free(writebuf);
      } else if (operation == "READ") {
        char* readbuf = (char*)malloc((size_t) request_size);
        seek(file, (size_t) offset);
        read(readbuf, file, (size_t) request_size);
        if(readbuf) free(readbuf);
      } else if (operation == "LSEEK") {
        seek(file, (size_t) offset);
      }
      operationTimer.endTimeWithoutPrint(operation + "," + std::to_string(offset) + ","
                                         + std::to_string(request_size) + ",");

      lineNumber++;
    }
#ifdef IRIS
    std::cout << "Iris,";
#else
    std::cout << "Other,";
#endif
    timings.emplace_back(globalTimer.endTimeWithoutPrint(""));
    rep--;

    std::fclose(trace);
  }
  for(auto timing:timings){
    average +=timing;
  }
  average=average/repetitions;
  double global_time;
#ifdef MPI_ENABLE
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
  MPI_Allreduce(&average, &global_time, 1, MPI_DOUBLE, MPI_SUM,
                MPI_COMM_WORLD);
#else
  global_time=average;
  comm_size=1;
#endif
  double mean = global_time / comm_size;

  if(rank == 0) {
    printf("Time : %lf\n",mean);
    std::cout <<
              #ifdef IRIS
              "IRIS,"
              #else
              "other,"
              #endif
              << "average,"
              << std::setprecision(6)
              #ifdef IRIS
              << average/repetitions
              #else
              << mean
              #endif
              << "\n";
  }
  if (line) free(line);

#ifdef IRIS

#else
  /*if( remove( "/mnt/orangefs/temp/file.dat" ) != 0 )
    perror( "Error deleting file" );*/
#endif

  return 0;
}

int local_tests::prepare_data(std::string path, std::string traceName) {
  std::string traceFile=path+traceName;
  std::FILE* trace;
  std::FILE* file = nullptr;
  std::FILE* obj = nullptr;
  char* line = NULL;
  size_t len=0;
  ssize_t readsize;
  std::string operation;
  long offset = 0;
  long request_size = 0;
  char* word;
  line = (char*) malloc(128);
  int lineNumber=0;

  /* putting down the data, file for PFS and objects for Hyperdex*/
#ifdef PRELOAD_POSIX
  MAP_OR_FAIL(fopen);
  trace = __real_fopen(traceFile.c_str(), "r");
#else
  fopen(traceFile.c_str(), "r");
#endif
  struct hyperdex_client_attribute attribute;
  enum hyperdex_client_returncode op_status, loop_status;
  while ((readsize = getline(&line, &len, trace)) != -1){
    lineNumber++;
    word = strtok(line, ",");
    operation = word;
    word = strtok(NULL, ",");
    offset = atol(word);
    word = strtok(NULL, ",");
    request_size = atol(word);
    char* writebuf = randstring(request_size);

    if (operation == "FOPEN") {
      //  file = std::open("/mnt/orangefs/temp/file.dat", "w+");
      obj = fopen("/mnt/orangefs/temp/file.dat", "w+");
    } else if (operation == "FCLOSE") {
      //std::close(file);
      fclose(obj);
    } else if (operation == "WRITE") {

    } else if (operation == "READ") {
      //std::write(writebuf, sizeof(char), request_size, file);
      fwrite(writebuf, sizeof(char), request_size, obj);
    } else if (operation == "LSEEK") {

    }
    if (writebuf) free(writebuf);
    lineNumber++;
  }
#ifdef PRELOAD_POSIX
  MAP_OR_FAIL(fclose);
  __real_fclose(trace);
#else
  fclose(trace);
#endif
  return 0;
}

double local_tests::copy_data_PFS2KVS(std::string traceFile, std::string
filePath,int rank, int commSize) {
  FILE* trace;
  FILE* file = nullptr;
  char* line = NULL;
  size_t len=0;
  ssize_t readsize;
  std::string operation;
  long offset = 0;
  long request_size = 0;
  std::size_t total_size = 0;
  std::size_t bytes_read = 0;
  char* word = nullptr;
  line = (char*) malloc(128);

  /* putting down random data with size equal to the trace*/
  trace = std::fopen(traceFile.c_str(), "r");
  while ((readsize = getline(&line, &len, trace)) != -1){
    word = strtok(line, ",");
    operation = word;
    word = strtok(NULL, ",");
    offset = atol(word);
    word = strtok(NULL, ",");
    request_size = atol(word);
    if(operation=="WRITE"||operation=="READ") total_size += (size_t)request_size;
  }
  std::fclose(trace);
  if(rank==0) {
    int sizeToBuild= (int) (total_size / 1024 + 1024);
    std::string script_cmd = "/home/anthony/Dropbox/Projects/iris/scripts/create_random_data.sh "
                             + filePath
                             + " " + std::to_string(commSize)
                             + " " + std::to_string(sizeToBuild);
    system(script_cmd.c_str());
  }
  MPI_Barrier(MPI_COMM_WORLD);
  /* get ready for the copy*/
  char* readbuf = (char*)malloc(total_size);
  /* read the data to a read buf and write them to Hyperdex*/
  Timer timer = Timer();
  timer.startTime();
  file = std::fopen((filePath+"_"+std::to_string(rank)).c_str(), "r");
  bytes_read = std::fread(readbuf, sizeof(char), total_size, file);
  std::fclose(file);
  iris::put("data_"+std::to_string(rank),readbuf,total_size);
  iris::flushData();
  if (readbuf) free(readbuf);
  if (line) free(line);

  auto timing = timer.endTimeWithoutPrint("");

  double serial;
  double parallel;

  MPI_Allreduce(&timing, &parallel, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
  MPI_Allreduce(&timing, &serial, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

  MPI_Barrier(MPI_COMM_WORLD);
  if(rank == 0) {
    std::cout << traceFile << ",sizeMB," << (total_size*commSize)/1048576
              << ",copyPFS2KVS-serial,"
              << std::setprecision(6)
              <<"\033[2;36m"+std::to_string(serial)+"\033[0m"
              << ",copyPFS2KVS-parallel,"
              << std::setprecision(6)
              <<"\033[2;36m"+std::to_string(parallel)+"\033[0m\n"
              << "\n";
  }
  return 0;
}

double
local_tests::copy_data_KVS2PFS(std::string traceFile, std::string filePath,
                               int rank, int commSize) {
  FILE* trace;
  FILE* file = nullptr;
  char* line = NULL;
  size_t len=0;
  ssize_t readsize;
  std::string operation;
  long offset = 0;
  long request_size = 0;
  std::size_t total_size = 0;
  std::size_t bytes_written = 0;
  char* word = nullptr;
  line = (char*) malloc(128);

  /* putting down random data with size equal to the trace*/
  trace = std::fopen(traceFile.c_str(), "r");
  while ((readsize = getline(&line, &len, trace)) != -1){
    word = strtok(line, ",");
    operation = word;
    word = strtok(NULL, ",");
    offset = atol(word);
    word = strtok(NULL, ",");
    request_size = atol(word);
    if(operation=="WRITE"||operation=="READ") total_size += (size_t)request_size;
  }
  std::fclose(trace);

  MPI_Barrier(MPI_COMM_WORLD);
  /* get ready for the copy*/
  char* writeBuf = randstring(total_size);
  iris::put("data_"+std::to_string(rank),writeBuf,total_size);
  Timer timer = Timer();
  timer.startTime();
  auto data = iris::get("data_" + std::to_string(rank));

/* read the data to a read buf and write them to Hyperdex*/
  file = std::fopen((filePath+"_"+std::to_string(rank)).c_str(), "w+");
  bytes_written = std::fwrite(writeBuf, sizeof(char), total_size, file);
  std::fclose(file);
  if (writeBuf) free(writeBuf);

  if (line) free(line);
  auto timing = timer.endTimeWithoutPrint("");

  double serial;
  double parallel;

  MPI_Allreduce(&timing, &parallel, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
  MPI_Allreduce(&timing, &serial, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

  MPI_Barrier(MPI_COMM_WORLD);
  if(rank == 0) {
    std::cout << traceFile << ",sizeMB," << (total_size*commSize)/1048576
              << ",copyKVS2PFS-serial,"
              << std::setprecision(6)
              <<"\033[2;36m"+std::to_string(serial)+"\033[0m"
              << ",copyKVS2PFS-parallel,"
              << std::setprecision(6)
              <<"\033[2;36m"+std::to_string(parallel)+"\033[0m\n"
              << "\n";
  }
  return 0;
}
int local_tests::metadata(int num_operations, std::string path_to_file){
  FILE* file = nullptr;
  std::string filename[num_operations];
  std::string temp = "file";
  Timer timer = Timer();
  timer.startTime();
  for(int i=0; i<num_operations;++i){
    filename[i] = path_to_file.c_str() + temp + std::to_string(i);
    file = fopen(filename[i].c_str(), "w+");
    if(file == nullptr) fprintf(stderr, "Error opening file %s", filename[i].c_str());
    fclose(file);
  }
  char* op_name = (char*)malloc(64*sizeof(char));
  sprintf(op_name,"Metadata of %d operations,", num_operations);
  timer.endTime(op_name);
#ifdef IRIS
  printf("Test completed.\n");
#else
  printf("Test completed. Cleaning up...\n");
        for(int i=0; i<num_operations;++i){
        if( remove( filename[i].c_str() ) != 0 )
        perror( "Error deleting file" );
        }
#endif
  return 0;
}

int local_tests::metadata_mpi(int num_operations, std::string path_to_file, int rank){
  FILE* file = nullptr;
  double timing=0;

  std::string filename[num_operations];
  std::string temp = "file";
  Timer timer = Timer();
  timer.startTime();
  for(int i=0; i<num_operations;++i){
    filename[i] = path_to_file.c_str() +std::to_string(rank)+ temp + std::to_string(i);
    file = fopen(filename[i].c_str(), "w+");
    if(file == nullptr) fprintf(stderr, "Error opening file %s", filename[i].c_str());
    fclose(file);
  }
  std::string rank_num = std::to_string(rank) + ",";
  timing = timer.endTime(rank_num);
  double global_time;
  double max_time;

  MPI_Allreduce(&timing, &max_time, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
  MPI_Allreduce(&timing, &global_time, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
  int comm_size=0;
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
  double mean = global_time / comm_size;

  if(rank == 0) {
    std::cout <<
              #ifdef IRIS
              "IRIS,"
              #else
              "POSIX,"
              #endif
              << "average,"
              << std::setprecision(10)
              << mean
              << ",max,"
              << std::setprecision(10)
              << max_time
              << "\n";
  }
#ifdef IRIS

#else
  for(int i=0; i<num_operations;++i){
        if( remove( filename[i].c_str() ) != 0 )
        perror( "Error deleting file" );
        }
#endif
  return 0;
}


int local_tests::replay_trace_pfs(std::string traceFile,
                                  std::string filename, int repetitions, int
                                  rank) {
  /*Initialization of some stuff*/
  std::FILE* trace;
  std::FILE* file = nullptr;
  char* line = NULL;
  int comm_size;
  size_t len=0;
  ssize_t readsize;
  std::string operation;
  long offset = 0;
  long request_size = 0;
  char* word;
  line = (char*) malloc(128);
  std::vector<double> timings;
  double average=0;
  int rep =  repetitions;
  filename=filename+"_"+std::to_string(rank);
  /* Do the I/O and comparison*/
  while(rep) {
    /*Opening the trace file*/
    trace = std::fopen(traceFile.c_str(), "r");
    if (trace==NULL) {
      return 0;
    }
    /*While loop to read each line from the trace and create I/O*/
    Timer globalTimer = Timer();
    globalTimer.startTime();
    time_t now = time(0);
    char *dt = ctime(&now);
    int lineNumber=0;
    while ((readsize = getline(&line, &len, trace)) != -1) {
      if (readsize < 4) {
        break;
      }
      word = strtok(line, ",");
      operation = word;
      word = strtok(NULL, ",");
      offset = atol(word);
      word = strtok(NULL, ",");
      request_size = atol(word);
      if (operation == "FOPEN") {
        file=std::fopen(filename.c_str(),"r+");
      } else if (operation == "FCLOSE") {
        std::fclose(file);
      } else if (operation == "WRITE") {
        char* writebuf = randstring(request_size);
        std::fseek(file, offset, SEEK_SET);
        std::fwrite(writebuf, sizeof(char), request_size, file);
        if(writebuf) free(writebuf);
      } else if (operation == "READ") {
        char* readbuf = (char*)malloc((size_t) request_size);
        std::fseek(file, (offset), SEEK_SET);
        std::fread(readbuf, sizeof(char), request_size, file);
        if(readbuf) free(readbuf);
      } else if (operation == "LSEEK") {
        //std::fseek(file, (offset), SEEK_SET);
      }
      lineNumber++;
    }
    //std::cout << "PFS,";
    timings.emplace_back(globalTimer.endTimeWithoutPrint(""));
    rep--;
    std::fclose(trace);
  }
  for(auto timing:timings){
    average +=timing;
  }
  average=average/repetitions;
  double global_time;
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
  MPI_Allreduce(&average, &global_time, 1, MPI_DOUBLE, MPI_SUM,
                MPI_COMM_WORLD);
  double mean = global_time / comm_size;

  if(rank == 0) {
    //printf("Time : %lf\n",mean);
    std::cout << traceFile
              << ",pfs,"
              << std::setprecision(6)
              <<"\033[2;36m"+std::to_string(mean)+"\033[0m\n"
              << "\n";
  }
  if (line) free(line);
  return 0;
}

int local_tests::prepare_data_pfs(std::string filename, int no_of_ranks) {
  std::string command="/home/anthony/Dropbox/Projects/iris/scripts/preparepfsData.sh "+
                      filename +" "+std::to_string(no_of_ranks);
  system(command.c_str());
  return 0;
}

int local_tests::replay_trace_mongo(std::string traceFile, int rank) {
  /*Initialization of some stuff*/
  std::FILE* trace;
  char* line = NULL;
  int comm_size;
  size_t len=0;
  ssize_t readsize;
  std::string operation;
  long offset = 0;
  long request_size = 0;
  char* word;
  line = (char*) malloc(128);
  std::vector<double> timings;
  double average=0;

  /*Opening the trace file*/
  trace = std::fopen(traceFile.c_str(), "r");
  if (trace==NULL) {
    return 0;
  }
  /*While loop to read each line from the trace and create I/O*/
  Timer globalTimer = Timer();
  globalTimer.startTime();
  time_t now = time(0);
  char *dt = ctime(&now);
  int lineNumber=0;
  using bsoncxx::builder::basic::kvp;
  auto uri = mongocxx::uri{"mongodb://localhost:27017"};
  auto client = mongocxx::client{uri};
  std::string database = "iris";
  std::string filename = "file";
  mongocxx::collection file = client[database].collection(filename);

  while ((readsize = getline(&line, &len, trace)) != -1) {
    if (readsize < 4) {
      break;
    }
    word = strtok(line, ",");
    operation = word;
    word = strtok(NULL, ",");
    offset = atol(word);
    word = strtok(NULL, ",");
    request_size = atol(word);
    if (operation == "FOPEN") {
    } else if (operation == "FCLOSE") {
    } else if (operation == "WRITE") {
      if(request_size == 0) continue;
      char* writebuf = randstring(request_size);
      std::string key= std::to_string(offset)+"_"+std::to_string
          (request_size)+"_"+std::to_string(rank);
      auto document = bsoncxx::builder::basic::document{};
      std::string data=writebuf;
      document.append(kvp("key",key), kvp("value",data));
      //adding the created key-value pair to the collection
      bsoncxx::document::view putView = document.view();//get the view
      auto add = file.insert_one(document.view());//insert it
      // to collection
      if (!add) {
        std::cout << "Unacknowledged write. No id available." << "\n";
        return EXIT_SUCCESS;
      }
      if (add->inserted_id().type() == bsoncxx::type::k_oid) {
        bsoncxx::oid id = add->inserted_id().get_oid().value;
        keyToID.emplace(key,id.to_string());
      } else {
        std::cout << "Inserted id was not an OID type" << "\n";
      }
      if(writebuf) free(writebuf);
    } else if (operation == "READ") {
      if(request_size == 0) continue;
      char* readbuf = (char*)malloc((size_t) request_size);
      //retrieving a value from the collection
      std::string key= std::to_string(offset)+"_"+std::to_string
          (request_size)+"_"+std::to_string(rank);
      bsoncxx::oid insertedID=
          bsoncxx::oid{mongocxx::stdx::string_view{keyToID[key]}};
      using bsoncxx::builder::stream::finalize;
      // Create the query filter
      auto filter = bsoncxx::builder::stream::document{}
          << "_id"
          << insertedID
          << finalize;
      // Execute find
      auto getDoc = file.find_one(filter.view());
      if (!getDoc){
        std::cout << "Document not found!" << "\n";
      }
      if(readbuf) free(readbuf);
    } else if (operation == "LSEEK") {
    }
    lineNumber++;
  }
  //std::cout << "PFS,";
  timings.emplace_back(globalTimer.endTimeWithoutPrint(""));
  std::fclose(trace);
  for(auto timing:timings){
    average +=timing;
  }
  double global_time;
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
  MPI_Allreduce(&average, &global_time, 1, MPI_DOUBLE, MPI_SUM,
                MPI_COMM_WORLD);
  double mean = global_time / comm_size;

  if(rank == 0) {
    //printf("Time : %lf\n",mean);
    std::cout << traceFile
              << ",mongo,"
              << std::setprecision(6)
              <<"\033[2;36m"+std::to_string(mean)+"\033[0m\n"
              << "\n";
  }
  if (line) free(line);
  return 0;
}

int local_tests::prepare_data_mongo(std::string traceFile, int rank) {
  /*Initialization of some stuff*/
  std::FILE* trace;
  char* line = NULL;
  size_t len=0;
  ssize_t readsize;
  std::string operation;
  long offset = 0;
  long request_size = 0;
  char* word;
  line = (char*) malloc(128);

  /*Opening the trace file*/
  trace = std::fopen(traceFile.c_str(), "r");
  if (trace==NULL) {
    return 0;
  }
  /*While loop to read each line from the trace and create I/O*/
  int lineNumber=0;
  using bsoncxx::builder::basic::kvp;
  auto uri = mongocxx::uri{"mongodb://localhost:27017"};
  auto client = mongocxx::client{uri};
  std::string database = "iris";
  std::string filename = "file";
  mongocxx::collection file = client[database].collection(filename);

  while ((readsize = getline(&line, &len, trace)) != -1) {
    if (readsize < 4) {
      break;
    }
    word = strtok(line, ",");
    operation = word;
    word = strtok(NULL, ",");
    offset = atol(word);
    word = strtok(NULL, ",");
    request_size = atol(word);
    if (operation == "FOPEN") {
    } else if (operation == "FCLOSE") {
    } else if (operation == "WRITE") {
    } else if (operation == "READ") {
      if(request_size == 0) continue;
      char* writebuf = randstring(request_size);
      std::string key= std::to_string(offset)+"_"+std::to_string
          (request_size)+"_"+std::to_string(rank);
      auto document = bsoncxx::builder::basic::document{};
      document.append(kvp("key",key), kvp("value",writebuf));
      //adding the created key-value pair to the collection
      bsoncxx::document::view putView = document.view();//get the view
      auto add = file.insert_one(putView);//insert it to collection
      if (!add) {
        std::cout << "Unacknowledged write. No id available." << "\n";
        return EXIT_SUCCESS;
      }
      if (add->inserted_id().type() == bsoncxx::type::k_oid) {
        bsoncxx::oid id = add->inserted_id().get_oid().value;
        keyToID.emplace(key,id.to_string());
      } else {
        std::cout << "Inserted id was not an OID type" << "\n";
      }
      if(writebuf) free(writebuf);
    } else if (operation == "LSEEK") {
    }
    lineNumber++;
  }
  if (line) free(line);
  return 0;
}

int local_tests::mongo_init(int rank) {
  auto uri = mongocxx::uri{"mongodb://localhost:27017"};
  auto client = mongocxx::client{uri};
  std::string database = "iris";
  std::string filename = "file";
  if(rank==0) {
    mongocxx::instance instance{};
    std::string arg = "/home/anthony/Dropbox/Projects/iris/scripts/dropDB.sh "
                          "" + database +" > /tmp/dropDB.log";
    system(arg.c_str());
    mongocxx::database db = client[database];
    mongocxx::collection file = client.database(database).has_collection(filename) ?
                                db.collection(filename) :
                                db.create_collection(filename);
    arg = "/home/anthony/Dropbox/Projects/iris/scripts"
                          "/shardCollection.sh " + database + " " + filename;
    system(arg.c_str());
  }
  mongocxx::write_concern wc = mongocxx::write_concern();
  wc.journal(true);
  MPI_Barrier(MPI_COMM_WORLD);
  keyToID=std::unordered_map<std::string,std::string>();
  return 0;
}

int
local_tests::replay_trace_iris_f2o(std::string traceFile, std::string filename,
                                   int repetitions, int rank) {
  /*Initialization of some stuff*/
  std::FILE* trace;
  std::FILE* file = nullptr;
  char* line = NULL;
  int comm_size;
  size_t len=0;
  ssize_t readsize;
  std::string operation;
  long offset = 0;
  long request_size = 0;
  char* word;
  line = (char*) malloc(128);
  std::vector<double> timings;
  double average=0;
  int rep =  repetitions;
  filename=filename+"_"+std::to_string(rank);
  /* Do the I/O and comparison*/
  while(rep) {
    /*Opening the trace file*/
    trace = std::fopen(traceFile.c_str(), "r");
    if (trace==NULL) {
      return 0;
    }
    /*While loop to read each line from the trace and create I/O*/
    Timer globalTimer = Timer();
    globalTimer.startTime();
    time_t now = time(0);
    char *dt = ctime(&now);
    int lineNumber=0;
    while ((readsize = getline(&line, &len, trace)) != -1) {
      if (readsize < 4) {
        break;
      }
      word = strtok(line, ",");
      operation = word;
      word = strtok(NULL, ",");
      offset = atol(word);
      word = strtok(NULL, ",");
      request_size = atol(word);
      if (operation == "FOPEN") {
        file=iris::fopen(filename.c_str(),"r+");
      } else if (operation == "FCLOSE") {
        iris::fclose(file);
      } else if (operation == "WRITE") {
        if(request_size == 0) continue;
        char* writebuf = (char *) calloc(request_size, sizeof(char));
        iris::fwrite(writebuf,sizeof(char), (size_t) request_size,file);
        if(writebuf) free(writebuf);
      } else if (operation == "READ") {
        if(request_size == 0) continue;
        char* readBuf = (char *) malloc(request_size);
        iris::fread(readBuf,sizeof(char), (size_t) request_size,file);
        if(readBuf) free(readBuf);
        usleep(3000);
      } else if (operation == "LSEEK") {
        std::fseek(file, (offset), SEEK_SET);
      }
      lineNumber++;
    }
    //std::cout << "PFS,";
    timings.emplace_back(globalTimer.endTimeWithoutPrint(""));
    rep--;
    std::fclose(trace);
  }
  for(auto timing:timings){
    average +=timing;
  }
  average=average/repetitions;
  double global_time;
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
  MPI_Allreduce(&average, &global_time, 1, MPI_DOUBLE, MPI_SUM,
                MPI_COMM_WORLD);
  double mean = global_time / comm_size;

  if(rank == 0) {
    //printf("Time : %lf\n",mean);
    std::cout << traceFile
              << ",iris_f2o,"
              << std::setprecision(6)
              <<"\033[2;36m"+std::to_string(mean)+"\033[0m\n"
              << "\n";
  }
  if (line) free(line);
  return 0;
}

int local_tests::prepare_data_iris_f2o(std::string traceFile,
                                       std::string filename, int repetitions, int rank) {

  /*Initialization of some stuff*/
  std::FILE* trace;
  std::FILE* file = nullptr;
  char* line = NULL;
  int comm_size;
  size_t len=0;
  ssize_t readsize;
  std::string operation;
  long offset = 0;
  long request_size = 0;
  char* word;
  line = (char*) malloc(128);
  filename=filename+"_"+std::to_string(rank);
  /* Do the I/O and comparison*/
  /*Opening the trace file*/
  trace = std::fopen(traceFile.c_str(), "r");
  if (trace==NULL) {
    return 0;
  }
  /*While loop to read each line from the trace and create I/O*/
  int lineNumber=0;
  while ((readsize = getline(&line, &len, trace)) != -1) {
    if (readsize < 4) {
      break;
    }
    word = strtok(line, ",");
    operation = word;
    word = strtok(NULL, ",");
    offset = atol(word);
    word = strtok(NULL, ",");
    request_size = atol(word);
    if (operation == "FOPEN") {
      file=iris::fopen(filename.c_str(),"r+");
    } else if (operation == "FCLOSE") {
      iris::fclose(file);
    } else if (operation == "WRITE") {
    } else if (operation == "READ") {
      if(request_size == 0) continue;
      char* writebuf = (char *) std::calloc(request_size, sizeof(char));
      iris::fwrite(writebuf,sizeof(char), (size_t) request_size,file);
      if(writebuf) free(writebuf);
    } else if (operation == "LSEEK") {
      std::fseek(file, (offset), SEEK_SET);
    }
    lineNumber++;
  }
  std::fclose(trace);
  if (line) free(line);
  return 0;
}

int local_tests::replay_trace_iris_o2f(std::string traceFile, int rank) {
  /*Initialization of some stuff*/
  std::FILE* trace;
  char* line = NULL;
  int comm_size;
  size_t len=0;
  ssize_t readsize;
  std::string operation;
  long offset = 0;
  long request_size = 0;
  char* word;
  line = (char*) malloc(128);
  std::vector<double> timings;
  double average=0;

  /*Opening the trace file*/
  trace = std::fopen(traceFile.c_str(), "r");
  if (trace==NULL) {
    return 0;
  }
  /*While loop to read each line from the trace and create I/O*/
  Timer globalTimer = Timer();
  globalTimer.startTime();
  time_t now = time(0);
  char *dt = ctime(&now);
  int lineNumber=0;
  while ((readsize = getline(&line, &len, trace)) != -1) {
    if (readsize < 4) {
      break;
    }
    word = strtok(line, ",");
    operation = word;
    word = strtok(NULL, ",");
    offset = atol(word);
    word = strtok(NULL, ",");
    request_size = atol(word);
    if (operation == "FOPEN") {
    } else if (operation == "FCLOSE") {
    } else if (operation == "WRITE") {
      if(request_size == 0) continue;
      char* writebuf = randstring(request_size);
      std::string key=std::to_string(offset)+"_" + std::to_string(request_size)
                      +"_"+std::to_string(rank);
      std::string data(writebuf);
      iris::put(key, writebuf, (size_t) request_size);
      if(writebuf) free(writebuf);
    } else if (operation == "READ") {
      if(request_size == 0) continue;
      std::string key=std::to_string(offset)+"_" + std::to_string(request_size)
                      +"_"+std::to_string(rank);
      iris::get(key);
    } else if (operation == "LSEEK") {
    }
    lineNumber++;
  }
  iris::flushData();
  //std::cout << "PFS,";
  average=globalTimer.endTimeWithoutPrint("");
  std::fclose(trace);
  double global_time;
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
  MPI_Allreduce(&average, &global_time, 1, MPI_DOUBLE, MPI_SUM,
                MPI_COMM_WORLD);
  double mean = global_time / comm_size;

  if(rank == 0) {
    //printf("Time : %lf\n",mean);
    std::cout << traceFile
              << ",iris-o2f,"
              << std::setprecision(6)
              <<"\033[2;36m"+std::to_string(mean)+"\033[0m\n"
              << "\n";
  }
  if (line) free(line);
  return 0;

}

int local_tests::prepare_data_iris_o2f(std::string traceFile, int rank) {
  /*Initialization of some stuff*/
  std::FILE* trace;
  char* line = NULL;
  int comm_size;
  size_t len=0;
  ssize_t readsize;
  std::string operation;
  long offset = 0;
  long request_size = 0;
  char* word;
  line = (char*) malloc(128);
  std::vector<double> timings;
  double average=0;

  /*Opening the trace file*/
  trace = std::fopen(traceFile.c_str(), "r");
  if (trace==NULL) {
    return 0;
  }
  int lineNumber=0;
  while ((readsize = getline(&line, &len, trace)) != -1) {
    if (readsize < 4) {
      break;
    }
    word = strtok(line, ",");
    operation = word;
    word = strtok(NULL, ",");
    offset = atol(word);
    word = strtok(NULL, ",");
    request_size = atol(word);
    if (operation == "FOPEN") {
    } else if (operation == "FCLOSE") {
    } else if (operation == "WRITE") {
    } else if (operation == "READ") {
      if(request_size == 0) continue;
      char* writebuf = randstring(request_size);
      std::string key=std::to_string(offset)+"_" + std::to_string(request_size)
                      +"_"+std::to_string(rank);
      iris::put(key, writebuf, (size_t) request_size);
      if(writebuf) free(writebuf);
    } else if (operation == "LSEEK") {
    }
    lineNumber++;
  }
  if (line) free(line);
  return 0;
}

int local_tests::mapping_cost(int numOperations) {
  Timer time = Timer();
  time.startTime();
  for(int i=0;i<numOperations;++i){
    /*if(i%2==0) POSIXMapper().generateKeys("name",0,64*1024,GET_OPERATION);
    else POSIXMapper().generateKeys("name",0,64*1024,PUT_OPERATION);*/
    POSIXMapper().generateKeys("name",0,64*1024,GET_OPERATION);
  }
  auto balanceModeTime=time.endTimeWithoutPrint("")/numOperations;
  time.startTime();
  for(int i=0;i<numOperations;++i){
    /*if(i%2==0) POSIXMapperGetOptimised::getInstance()->generateKeys("name",0,
                                                                   64*1024,
                                                       GET_OPERATION);
    else  POSIXMapperGetOptimised::getInstance()->generateKeys("name",0,64*1024,PUT_OPERATION);*/
    POSIXMapperGetOptimised::getInstance()->generateKeys("name",0,64*1024,
                                                         GET_OPERATION);
  }
  auto readOptModeTime=time.endTimeWithoutPrint("")/numOperations;
  time.startTime();
  for(int i=0;i<numOperations;++i){
    /*if(i%2==0) POSIXMapperPutOptimised::getInstance()->generateKeys("name",0,
                                                                    64*1024,
                                                                    GET_OPERATION);
    else  POSIXMapperPutOptimised::getInstance()->generateKeys("name",0,64*1024,PUT_OPERATION);*/
    POSIXMapperPutOptimised::getInstance()->generateKeys("name",0,64*1024,
                                                         GET_OPERATION);
  }
  auto writeOptModeTime=time.endTimeWithoutPrint("")/numOperations;
  std::cout << "balanced,"
            << std::setprecision(6)
            <<"\033[2;36m"+std::to_string(balanceModeTime)+"\033[0m"
            << ",readOpt,"
            << std::setprecision(6)
            <<"\033[2;36m"+std::to_string(readOptModeTime)+"\033[0m"
            << ",writeOpt,"
            << std::setprecision(6)
            <<"\033[2;36m"+std::to_string(writeOptModeTime)+"\033[0m"
            << "\n";
  return 0;
}

int local_tests::prepare_data_iris_hybrid(std::string traceFile,
                                          std::string filename, int repetitions,
                                          int rank) {
  /*Initialization of some stuff*/
  std::FILE* trace;
  std::FILE* file = nullptr;
  char* line = NULL;
  int comm_size;
  size_t len=0;
  ssize_t readsize;
  std::string operation;
  long offset = 0;
  long request_size = 0;
  char* word;
  line = (char*) malloc(128);
  filename=filename+"_"+std::to_string(rank);
  /* Do the I/O and comparison*/
  /*Opening the trace file*/
  trace = std::fopen(traceFile.c_str(), "r");
  if (trace==NULL) {
    return 0;
  }

  /*While loop to read each line from the trace and create I/O*/
  int lineNumber=0;
  while ((readsize = getline(&line, &len, trace)) != -1) {
    if (readsize < 4) {
      break;
    }
    word = strtok(line, ",");
    operation = word;
    word = strtok(NULL, ",");
    offset = atol(word);
    word = strtok(NULL, ",");
    request_size = atol(word);
    if (operation == "FOPEN") {
      file=iris::fopen(filename.c_str(),"w+");
    } else if (operation == "FCLOSE") {
      iris::fclose(file);
    } else if (operation == "WRITE") {
    } else if (operation == "READ") {
      if(request_size == 0) continue;
      char* writebuf = (char *) std::calloc(request_size, sizeof(char));
      if(request_size > HYBRID_THRESHHOLD){
        std::string key=std::to_string(offset)+"_" + std::to_string(request_size)
                        +"_"+std::to_string(rank);
        iris::put(key,writebuf,request_size);
      }else{
        iris::fwrite(writebuf,sizeof(char), (size_t) request_size,file);
      }
      if(writebuf) free(writebuf);
    } else if (operation == "LSEEK") {
      std::fseek(file, (offset), SEEK_SET);
    }
    lineNumber++;
  }
  std::fclose(trace);
  if (line) free(line);
  return 0;
}

int local_tests::replay_trace_iris_hybrid(std::string traceFile,
                                          std::string filename, int repetitions,
                                          int rank) {
  /*Initialization of some stuff*/
  std::FILE* trace;
  std::FILE* file = nullptr;
  char* line = NULL;
  int comm_size;
  size_t len=0;
  ssize_t readsize;
  std::string operation;
  long offset = 0;
  long request_size = 0;
  char* word;
  line = (char*) malloc(128);
  std::vector<double> timings;
  double average=0;
  int rep =  repetitions;
  filename=filename+"_"+std::to_string(rank);
  /* Do the I/O and comparison*/
  while(rep) {
    /*Opening the trace file*/
    trace = std::fopen(traceFile.c_str(), "r");
    if (trace==NULL) {
      return 0;
    }
    /*While loop to read each line from the trace and create I/O*/
    Timer globalTimer = Timer();
    globalTimer.startTime();
    time_t now = time(0);
    char *dt = ctime(&now);
    int lineNumber=0;
    while ((readsize = getline(&line, &len, trace)) != -1) {
      if (readsize < 4) {
        break;
      }
      word = strtok(line, ",");
      operation = word;
      word = strtok(NULL, ",");
      offset = atol(word);
      word = strtok(NULL, ",");
      request_size = atol(word);
      if (operation == "FOPEN") {
        file=iris::fopen(filename.c_str(),"w+");
      } else if (operation == "FCLOSE") {
        iris::fclose(file);
      } else if (operation == "WRITE") {
        if(request_size == 0) continue;
        char* writebuf = (char *) std::calloc(request_size, sizeof(char));
        if(request_size > HYBRID_THRESHHOLD){
          std::string key=filename+"_" + std::to_string(offset)+"_" + std::to_string(request_size)
                          +"_"+std::to_string(rank);
          iris::put(key,writebuf,request_size);
        }else{
          iris::fwrite(writebuf,sizeof(char), (size_t) request_size,file);
        }
        if(writebuf) free(writebuf);
      } else if (operation == "READ") {
        if(request_size == 0) continue;
        char* readBuf;
        if(request_size > HYBRID_THRESHHOLD){
          std::string key=std::to_string(offset)+"_" + std::to_string(request_size)
                          +"_"+std::to_string(rank);
          clearcache(filename);
          readBuf= (char *) iris::get(key);
        }else{
          readBuf=(char *) malloc(request_size);
          iris::fread(readBuf,sizeof(char), (size_t) request_size,file);
        }
        if(readBuf) free(readBuf);
      } else if (operation == "LSEEK") {
        std::fseek(file, (offset), SEEK_SET);
      }
      lineNumber++;
    }
    //std::cout << "PFS,";
    timings.emplace_back(globalTimer.endTimeWithoutPrint(""));
    rep--;
    std::fclose(trace);
  }
  for(auto timing:timings){
    average +=timing;
  }
  average=average/repetitions;
  double global_time;
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
  MPI_Allreduce(&average, &global_time, 1, MPI_DOUBLE, MPI_SUM,
                MPI_COMM_WORLD);
  double mean = global_time / comm_size;

  if(rank == 0) {
    //printf("Time : %lf\n",mean);
    std::cout << traceFile
              << ",iris_hybrid,"
              << std::setprecision(6)
              <<"\033[2;36m"+std::to_string(mean)+"\033[0m\n"
              << "\n";
  }
  if (line) free(line);
  return 0;
}



