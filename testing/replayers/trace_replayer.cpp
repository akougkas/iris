//
// Created by anthony on 3/1/17.
//

#include <cstring>
#include <vector>
#include <iostream>
#include <mpi.h>
#include <iomanip>
#include "trace_replayer.h"
#include "../../src/utils/tools/Timer.h"
#include "../../src/S3.h"
#include "../../src/utils/tools/city.h"

char* trace_replayer::randstring(long length) {
  int n;
  static char charset[] =
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.-#'?!";
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

FILE* trace_replayer::open(const char* name,const char *mode){
  return fopen(name,mode);
}
int trace_replayer::close(FILE* fh){
  fclose(fh);
  return 0;
}
long trace_replayer::read(void* ptr, FILE* fh,size_t amount) {
  return fread(ptr, sizeof(char), amount, fh);
}
long trace_replayer::write(void* ptr, FILE* fh,size_t amount) {
  return fwrite(ptr, sizeof(char), amount, fh);
}
int trace_replayer::seek(FILE* fh,size_t amount) {
  fseek(fh, (amount), SEEK_SET);
  return 0;
}


int trace_replayer::replay_trace(std::string traceFile,
                              std::string filename, int repetitions,
                                 int rank, int mode){
  /*Initialization of some stuff*/
  FILE* trace;
  FILE* file = nullptr;
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
  int rep = repetitions;

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
    std::cout << traceFile << "," << dt;
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
        if(mode)
        file = open((filename+std::to_string(rank)).c_str(), "w+");
      } else if (operation == "FCLOSE") {
        if(mode) close(file);
      } else if (operation == "WRITE") {
        void* writebuf = randstring(request_size);
        if(mode){//if it is files-to-objects
          seek(file, (size_t) offset);
          write(writebuf, file, (size_t) request_size);
          if(writebuf) free(writebuf);
        }
        else{//if it is objects-to-files
          //this is wrong it should just some key0 with some size in trace
          uint32_t hash = CityHash32(std::to_string(offset+request_size).c_str(),
                                     std::to_string(offset+request_size)
                                         .length());
          iris::put(std::to_string(hash), writebuf, (size_t) request_size);
        }
      } else if (operation == "READ") {
        char* readbuf = (char*)malloc((size_t) request_size);
        if(mode){
          seek(file, (size_t) offset);
          read(readbuf, file, (size_t) request_size);
          if(readbuf) free(readbuf);
        }
        else{
          uint32_t hash = CityHash32(std::to_string(offset+request_size).c_str(),
                                     std::to_string(offset+request_size)
                                         .length());
          iris::get(std::to_string(hash));

        }
      } else if (operation == "LSEEK") {
        if(mode) seek(file, (size_t) offset);
      }
      operationTimer.endTimeWithoutPrint(operation + "," + std::to_string(offset) + ","
                                         + std::to_string(request_size) + ",");
      lineNumber++;
    }
    std::cout << "Iris,";
    timings.emplace_back(globalTimer.endTime(""));
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
    printf("Time : %lf\n",mean);
    std::cout << "IRIS,"
              << "average,"
              << std::setprecision(6)
              << average/repetitions
              << "\n";
  }
  if (line) free(line);

  /*if( remove( "/home/anthony/temp/" ) != 0 )
    perror( "Error deleting file" );*/
  return 0;
}

int trace_replayer::prepare_data(std::string traceFile, std::string filename,
                                 int repetitions, int rank, int mode) {
  FILE* trace;
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

  trace = fopen(traceFile.c_str(), "r");


  while ((readsize = getline(&line, &len, trace)) != -1){
    lineNumber++;
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
      char* writebuf = randstring(request_size);
      uint32_t hash = CityHash32(std::to_string(offset+request_size).c_str(),
                                 std::to_string(offset+request_size)
                                     .length());
      iris::put(std::to_string(hash), writebuf, (size_t) request_size);
      if (writebuf) free(writebuf);
    } else if (operation == "LSEEK") {

    }

    lineNumber++;
  }
  fclose(trace);
  return 0;
}




