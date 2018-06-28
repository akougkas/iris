//
// Created by anthony on 12/28/16.
//

#ifndef IRIS_TIMER_H
#define IRIS_TIMER_H


#include <chrono>
#include <string>

class Timer {
public:
  void startTime();
  double endTime(std::string fnName);
  double endTimeWithoutPrint(std::string fnName);
private:
  std::chrono::high_resolution_clock::time_point t1;
};


#endif //IRIS_TIMER_H
