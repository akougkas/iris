//
// Created by anthony on 12/21/16.
//

#ifndef IRIS_BUFFER_H
#define IRIS_BUFFER_H
/******************************************************************************
*include files
******************************************************************************/
#include <cstddef>
#include <cstring>
#include <stdexcept>

/******************************************************************************
*Class Buffer
******************************************************************************/
class Buffer {
public:
  typedef std::size_t size_type;
  static const size_type npos = static_cast<size_type> (-1);

/******************************************************************************
*Constructors
******************************************************************************/
  explicit Buffer(size_type size = 0);

  Buffer(size_type size, size_type capacity);

  Buffer(const void *data, size_type size);

  Buffer(const void *data, size_type size, size_type capacity);

  Buffer(void *data, size_type size, size_type capacity,
         bool assume_ownership);

  Buffer(const Buffer &);

  Buffer(void * data);


/******************************************************************************
*Destructor
******************************************************************************/
  ~Buffer();

/******************************************************************************
*Interface
******************************************************************************/
  void update(const void * data, size_type offset, size_type size);

  Buffer &operator=(const Buffer &);

  void swap(Buffer &);

  char *detach();

  void assign(const void *data, size_type size);

  void assign(void *data, size_type size, size_type capacity,
              bool assume_ownership);

  void append(const Buffer &);

  void append(const void *data, size_type size);

  void fill(char value = 0);

  size_type size() const;

  bool size(size_type);

  size_type capacity() const;

  bool capacity(size_type);

  bool empty() const;

  void clear();

  char *data();

  const char *data() const;

  char &operator[](size_type);

  char operator[](size_type) const;

  char &at(size_type);

  char at(size_type) const;

  size_type find(char, size_type pos = 0) const;

  size_type rfind(char, size_type pos = npos) const;

private:
/******************************************************************************
*Variables
******************************************************************************/
  char* data_;
  size_type size_;
  size_type capacity_;
  bool free_;
};

/******************************************************************************
*Operators
******************************************************************************/
bool operator==(const Buffer &, const Buffer &);

bool operator!=(const Buffer &, const Buffer &);

#endif //IRIS_BUFFER_H
