//
// Created by anthony on 2/14/17.
//

#ifndef IRIS_ASSERTUTIL_H
#define IRIS_ASSERTUTIL_H

#define CHECK(condition)                                                       \
  do {                                                                         \
    if (!(condition)) {                                                        \
      std::cerr << "Check `" #condition "` failed in " << __FILE__ << " line " \
                << __LINE__ << std::endl;                                      \
      std::exit(EXIT_FAILURE);                                                 \
    }                                                                          \
  } while (false)

#define ASSERT(condition, message)                                       \
  do {                                                                   \
    if (!(condition)) {                                                  \
      std::cerr << "Assertion `" #condition "` failed in " << __FILE__   \
                << " line " << __LINE__ << ": " << message << std::endl; \
      std::exit(EXIT_FAILURE);                                           \
    }                                                                    \
  } while (false)

#endif //IRIS_ASSERTUTIL_H
