/*******************************************************************************
* File Components.h
*
* Goal: This is all the internal IRIS components
*
* Created: December 10th, 2016  - Anthony Kougkas
 * Updated:
* Illinois Institute of Technology - SCS Lab
* (C) 2016
******************************************************************************/
#ifndef IRIS_COMPONENTS_H
#define IRIS_COMPONENTS_H
/******************************************************************************
*include files
******************************************************************************/
#include <cstdio>
#include <cstring>
#include <string>
#include <cstdlib>
#include <unordered_map>
#include <vector>
#include "return_codes.h"
#include "ConfigurationManager.h"
#include "../utils/google_btree/btree_set.h"

/******************************************************************************
*Key structure
******************************************************************************/
class Key{
public:
  const char* name;
  std::size_t offset;
  std::size_t size;
  void* data;
/*******************
*Constructor
*******************/
  Key() :name(), offset(0), size(0), data(nullptr) {}
  Key(const Key&) = default;               // Copy constructor
  Key(Key&&) = default;                    // Move constructor
/*******************
*Destructor
*******************/
  virtual ~Key() { }//if(data) free(data); }
/*******************
*Operators
*******************/
  Key& operator=(const Key&) & = default;  // Copy assignment operator
  Key& operator=(Key&&) & = default;       // Move assignment operator
  bool operator==(const Key &other) const{
    return strcmp(name,other.name)==0;
  }
};
/******************************************************************************
*Virtual Object
******************************************************************************/
class VirtualObject{
public:
  std::string name;
  std::size_t containerOffset;
  std::size_t size;
  void* data;
  std::vector<VirtualObject> linkedObjects;
/*******************
*Constructor
*******************/
  VirtualObject() :name(), containerOffset(0), size(0), data(nullptr),
                   linkedObjects() {}
  VirtualObject(const VirtualObject&) =default; // Copy constructor
  VirtualObject(VirtualObject&&) =default; // Move constructor
/*******************
*Destructor
*******************/
  virtual ~VirtualObject() {}
/*******************
*Operators
*******************/
  bool operator==(const VirtualObject &other) const{
    return name==other.name;
  }
  VirtualObject& operator=(const VirtualObject&)& =default;// Copy assignment operator
  VirtualObject& operator=(VirtualObject&&) & = default;// Move assignment operator
};
/*******************
*Hash function def
*******************/
namespace std {
  template<>
  struct hash<VirtualObject> {
    std::size_t operator()(const VirtualObject &obj) const{
      return ((hash<string>()(obj.name)) >> 1);
    }
  };
}
/******************************************************************************
*Container (of objects)
******************************************************************************/
class Container{
public:
  std::string containerName;
  std::size_t filePointer;
  std::size_t size;
  std::array<VirtualObject*, NUM_KEYS_IN_CONTAINER> objectTable;
  std::unordered_map<std::string,VirtualObject*> objects;
  std::unordered_map<std::string,VirtualObject*> invalidObjects;
/*******************
*Constructors
*******************/
  Container() : containerName(), filePointer(0), size(0), objects(),
                invalidObjects(), objectTable(){}
  Container(const Container&) =default; // Copy constructor
  Container(Container&&) =default; // Move constructor
/*******************
*Destructor
*******************/
  virtual ~Container() {}
/*******************
*Operators
*******************/
  Container& operator=(const Container&)& =default;// Copy assignment operator
  Container& operator=(Container&&) & = default;// Move assignment operator
};
/******************************************************************************
*MemTable
******************************************************************************/
class MemTable{
public:
  std::unordered_map<VirtualObject, std::string> memTable;
  std::size_t size;
/*******************
*Constructors
*******************/
  MemTable() : memTable(), size(0) {}
  MemTable(const MemTable&) =default; // Copy constructor
  MemTable(MemTable&&) =default; // Move constructor
/*******************
*Destructor
*******************/
  virtual ~MemTable() {}
/*******************
*Operators
*******************/
  MemTable& operator=(const MemTable&)& =default;// Copy assignment operator
  MemTable& operator=(MemTable&&) & = default;// Move assignment operator
/*******************
*Functions
*******************/
  inline int insert(const VirtualObject& object, std::string containerName){
    memTable.insert({object, containerName});
    size += object.size;
    return OPERATION_SUCCESSFUL;
  }
  inline int erase(const VirtualObject& object){
    auto status = memTable.erase(object);
    if(status > 0) size-= object.size;
    return OPERATION_SUCCESSFUL;
  }
  inline void reset(){
    memTable.clear();
    size=0;
  }
};
/******************************************************************************
*Class Range for searching within a range
******************************************************************************/
class Range{
public:
  Range(){
    timestamp=time(NULL);
  }
  size_t min;
  size_t max;
  std::string keyName;
  time_t timestamp;
  bool contains(Range other)  const{
    return other.min >= min || other.max <= max;
  }
  std::pair<size_t,size_t> getContainedPortion(Range other)  const{
    size_t minimum=other.min<this->min?this->min-other.min:0;
    size_t maximum=other.max>this->max?other.max:this->max;
    return std::make_pair(minimum,maximum);
  }
  bool operator<(const Range& other) const{
    return timestamp > other.timestamp;
  }
  bool operator==(const Range &other) const{
    return contains(other);
  }
};

//Hash function to compare Keys and Ranges
namespace std {

  template <>
  struct hash<Key>
  {
    std::size_t operator()(const Key& k) const
    {
      using std::size_t;
      using std::hash;
      using std::string;
      return ((hash<string>()(k.name)) >> 1);
    }
  };
  template <>
  struct hash<Range>
  {
    std::size_t operator()(const Range& r) const
    {
      using std::size_t;
      using std::hash;
      using std::string;
      return ((hash<string>()(r.keyName)) >> 1);
    }
  };
}
#endif //IRIS_COMPONENTS_H
