#ifndef IRIS_CLEARCACHE_H
#define IRIS_CLEARCACHE_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string>

inline void clearcache(std::string filename)
{
        int fd = open(filename.c_str(), O_RDWR);
        posix_fadvise(fd, 0, 0, 4);
        close(fd);
}

#endif //IRIS_CLEARCACHE_H