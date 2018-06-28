//
// Created by admin2 on 2/4/17.
//

#ifndef IRIS_HDF5_TESTS_H
#define IRIS_HDF5_TESTS_H


#include <string>
#define FILE_NAME            "h5ex_d_rdwr.h5"
#define DATASET         "DS1"
class hdf5_tests{
public:
  static int prepare_data(std::string path, std::string traceName);
  static int replay_trace(std::string path, std::string traceName,
                          std::string filename, int repetitions, int rank);
};


#endif //IRIS_HDF5_TESTS_H
