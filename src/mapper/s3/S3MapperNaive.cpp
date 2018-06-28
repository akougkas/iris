/******************************************************************************
*include files
******************************************************************************/
#include "../../utils/tools/city.h"
#include "S3MapperNaive.h"

/******************************************************************************
*Interface
******************************************************************************/
std::pair<std::string, VirtualObject>
S3MapperNaive::mapObject(std::string objectName, std::size_t objectSize,
    operation operationType) {
#ifdef TIMER_M
  Timer timer = Timer(); timer.startTime();
#endif
  std::string filename = "container_" + objectName + ".dat";
  VirtualObject object = VirtualObject();
  object.name = objectName;
  object.size = objectSize;
  object.containerOffset = 0;
#ifdef TIMER_M
  timer.endTime(__FUNCTION__);
#endif
  return std::make_pair(filename,object);
}

