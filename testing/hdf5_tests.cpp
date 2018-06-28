//
// Created by admin2 on 2/4/17.
//

#include <hdf5.h>
#include <cstring>
#include <hyperdex/client.h>
#include <dlfcn.h>
#include "hdf5_tests.h"
#include "../src/API.h"
#include "../src/iris.h"
#include "../src/utils/tools/Timer.h"

int hdf5_tests::prepare_data(std::string path, std::string traceName) {
  std::string traceFile = path + traceName;
  hid_t file=-1, space=-1, dset=-1;          /* Handles */
  FILE *trace;
  char *line = NULL;
  size_t len = 0;
  ssize_t readsize;
  std::string operation;
  long offset = 0;
  long request_size = 0;
  char *word;
  line = (char *) malloc(128);
  int lineNumber = 0;

  /* putting down the data, file for PFS and objects for Hyperdex*/
  MAP_OR_FAIL(fopen);
  trace = __real_fopen(traceFile.c_str(), "r");
  struct hyperdex_client_attribute attribute;
  enum hyperdex_client_returncode op_status, loop_status;
  while ((readsize = getline(&line, &len, trace)) != -1) {
    word = strtok(line, ",");
    operation = word;
    word = strtok(NULL, ",");
    offset = atol(word);
    word = strtok(NULL, ",");
    request_size = atol(word);
    if (operation == "FOPEN") {
      file = H5Fcreate(FILE_NAME, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
      hsize_t DIM0=(64*1024*1024)/sizeof(int);
      hsize_t dims[1] = {DIM0};
      space = H5Screate_simple(1, dims, NULL);
      dset = H5Dcreate2(file, DATASET, H5T_STD_I32LE, space, H5P_DEFAULT,
                        H5P_DEFAULT, H5P_DEFAULT);
    } else if (operation == "FCLOSE") {
      herr_t status;
      if(dset) status = H5Dclose(dset);
      if(space) status = H5Sclose(space);
      if(file) status = H5Fclose(file);
    } else if (operation == "WRITE") {
    } else if (operation == "READ") {
      herr_t status;
      const hsize_t DIM0=request_size/sizeof(int);
      int wdata[DIM0];
      hsize_t i;
      hsize_t dims[1] = {DIM0};
      hsize_t     count[1];              /* size of subset in the file */
      hsize_t     off[1];             /* subset offset in the file */
      hsize_t     stride[1];
      hsize_t     block[1];
      off[0] = offset/sizeof(int);
      count[0]  = DIM0;
      stride[0] = 1;
      block[0] = 1;
      hid_t       dataspace_id, memspace_id;
      memspace_id = H5Screate_simple (1, dims, NULL);
      dataspace_id = H5Dget_space (dset);
      status = H5Sselect_hyperslab (dataspace_id, H5S_SELECT_SET, off,
                                    stride, count, block);
      status = H5Dwrite(dset, H5T_NATIVE_INT, memspace_id,
                        dataspace_id, H5P_DEFAULT, wdata);
    } else if (operation == "LSEEK") {
    }
    lineNumber++;
  }
  MAP_OR_FAIL(fclose);
  __real_fclose(trace);
  if (line) free(line);
  return 0;
}

int hdf5_tests::replay_trace(std::string path, std::string traceName, std::string filename, int repetitions, int rank) {
  std::string traceFile = path + traceName;
  hid_t file, space, dset;          /* Handles */
  herr_t status;
  FILE *trace;
  char *line = NULL;
  size_t len = 0;
  ssize_t readsize;
  std::string operation;
  long offset = 0;
  long request_size = 0;
  char *word;
  line = (char *) malloc(128);
  int lineNumber = 0;

  /* putting down the data, file for PFS and objects for Hyperdex*/
  MAP_OR_FAIL(fopen);
  trace = __real_fopen(traceFile.c_str(), "r");
  struct hyperdex_client_attribute attribute;
  enum hyperdex_client_returncode op_status, loop_status;
  double average=0;
  int rep=repetitions;
  while(rep--) {
    Timer globalTimer = Timer();
    globalTimer.startTime();
    while ((readsize = getline(&line, &len, trace)) != -1) {
      word = strtok(line, ",");
      operation = word;
      word = strtok(NULL, ",");
      offset = atol(word);
      word = strtok(NULL, ",");
      request_size = atol(word);
      if (operation == "FOPEN") {
        file = H5Fcreate(FILE_NAME, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
        hsize_t DIM0 = (64 * 1024 * 1024) / sizeof(int);
        hsize_t dims[1] = {DIM0};
        space = H5Screate_simple(1, dims, NULL);
        dset = H5Dcreate2(file, DATASET, H5T_STD_I32LE, space, H5P_DEFAULT,
                          H5P_DEFAULT, H5P_DEFAULT);
      } else if (operation == "FCLOSE") {
        if (dset) status = H5Dclose(dset);
        if (space) status = H5Sclose(space);
        if (file) status = H5Fclose(file);
      } else if (operation == "WRITE") {
        const hsize_t DIM0 = request_size / sizeof(int);
        int wdata[DIM0];
        hsize_t i, j;
        for (i = 0; i < DIM0; i++) wdata[i] = i;
        hsize_t dims[1] = {DIM0};
        hsize_t count[1];              /* size of subset in the file */
        hsize_t off[1];             /* subset offset in the file */
        hsize_t stride[1];
        hsize_t block[1];
        off[0] = offset / sizeof(int);
        count[0] = DIM0;
        stride[0] = 1;
        block[0] = 1;
        hid_t dataspace_id, memspace_id;
        memspace_id = H5Screate_simple(1, dims, NULL);
        dataspace_id = H5Dget_space(dset);
        status = H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, off,
                                     stride, count, block);
        status = H5Dwrite(dset, H5T_NATIVE_INT, memspace_id,
                          dataspace_id, H5P_DEFAULT, wdata);
      } else if (operation == "READ") {
        const hsize_t DIM0 = request_size / sizeof(int);
        hsize_t dims[1] = {DIM0};
        int rdata[DIM0];
        hsize_t i, j;
        hsize_t count[1];              /* size of subset in the file */
        hsize_t off[1];             /* subset offset in the file */
        hsize_t stride[1];
        hsize_t block[1];
        off[0] = offset / sizeof(int);
        count[0] = DIM0;
        stride[0] = 1;
        block[0] = 1;
        hid_t dataspace_id, memspace_id;
        memspace_id = H5Screate_simple(1, dims, NULL);
        dataspace_id = H5Dget_space(dset);
        status = H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, off,
                                     stride, count, block);
        status = H5Dread(dset, H5T_NATIVE_INT, memspace_id,
                         dataspace_id, H5P_DEFAULT, rdata);
      } else if (operation == "LSEEK") {
      }
      lineNumber++;
    }
    average+=globalTimer.endTimeWithoutPrint("");
  }
  MAP_OR_FAIL(fclose);
  __real_fclose(trace);
  printf("Time :%lf",average/repetitions);
  return 0;
}
