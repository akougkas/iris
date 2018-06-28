//
// Created by anthony on 3/1/17.
//

#ifndef IRIS_TRACE_REPLAYER_H
#define IRIS_TRACE_REPLAYER_H

#include <string>
class trace_replayer {
public:
  enum operation_type{
    NONE        =0,
    READ_OPER   =1,
    WRITE_OPER  =2,
    OPEN_OPER   =3,
    CLOSE_OPER  =4,
    SEEK_OPER   =5
  };
  static int replay_trace(std::string traceFile,
                          std::string filename, int repetitions,
                          int rank, int mode);
  static int prepare_data(std::string traceFile,
                          std::string filename, int repetitions,
                          int rank, int mode);

private:
  static char *randstring(long length);
  static FILE * open(const char *name, const char *mode);
  static int close(FILE *fh);
  static long read(void* ptr, FILE *fh, size_t amount);
  static long write(void* ptr, FILE *fh, size_t amount);
  static int seek(FILE *fh, size_t amount);
};


#endif //IRIS_TRACE_REPLAYER_H
