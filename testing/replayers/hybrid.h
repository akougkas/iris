//
// Created by anthony on 1/23/17.
//

#ifndef REPLAYERS_HYBRID_H
#define REPLAYERS_HYBRID_H


#include <string>

const long THRESHOLD = 20*1024;
class hybrid {
public:
  enum operation_type{
    NONE        =0,
    READ_OPER   =1,
    WRITE_OPER  =2,
    OPEN_OPER   =3,
    CLOSE_OPER  =4,
    SEEK_OPER   =5
  };
  static long replay(std::string trace, std::string pvfs_file, int rank);
  static long prepare(std::string trace, int rank);
  static int init();

private:
  static char *randstring(long length);
};


#endif //REPLAYERS_HYBRID_H
