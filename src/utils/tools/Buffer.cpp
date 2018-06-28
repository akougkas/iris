/******************************************************************************
*include files
******************************************************************************/
#include <iostream>
#include "Buffer.h"
/******************************************************************************
*Constructors
******************************************************************************/
Buffer::Buffer(void *data)
    : free_(false)
{
  if(data != nullptr) data_ = (char *) data;
}

Buffer::Buffer(size_type s)
    :free_(true)
{
  if(s != 0) data_ = (char*)malloc(s);//new char[s];
  else data_ = 0;
  size_ = capacity_ = s;
}

Buffer::Buffer(size_type s, size_type c)
    : free_(true)
{
  if(s > c)
    throw std::invalid_argument("size greater than capacity");

  if(s != 0) data_ = (char*)malloc(s);//new char[s];
  else data_ = 0;
  size_ = s;
  capacity_ = c;
}

Buffer::Buffer(const void* d, size_type s)
    : free_(true)
{
  if(s != 0)
  {
    data_ = (char*)malloc(s);//new char[s];
    std::memcpy (data_, d, s);
  }
  else data_ = 0;
  size_ = capacity_ = s;
}

Buffer::Buffer(const void* d, size_type s, size_type c)
    : free_(true)
{
  if(s > c)
    throw std::invalid_argument("size greater than capacity");

  if(c != 0)
  {
    data_ = (char*)malloc(s);//new char[c];

    if(s != 0)
      std::memcpy(data_, d, s);
  }
  else data_ = 0;
  size_ = s;
  capacity_ = c;
}

Buffer::Buffer(void* d, size_type s, size_type c, bool own)
    : data_(static_cast<char*> (d)), size_(s), capacity_(c), free_(own)
{
  if(s > c)
    throw std::invalid_argument("size greater than capacity");
}

Buffer::Buffer(const Buffer& x)
    : free_(true)
{
  if(x.capacity_ != 0)
  {
    data_ = (char*)malloc(x.capacity_);//new char[x.capacity_];

    if(x.size_ != 0)
      std::memcpy (data_, x.data_, x.size_);
  }
  else data_ = 0;
  size_ = x.size_;
  capacity_ = x.capacity_;
}
/******************************************************************************
*Destructor
******************************************************************************/
Buffer::~Buffer()
{
  if(free_) free(data_);//delete[] data_;
}
/******************************************************************************
*Interface
******************************************************************************/
void Buffer::update(const void *data, Buffer::size_type offset,
                    Buffer::size_type size) {
  if(offset+size <= size_) std::memcpy(data_+offset, data, size);
}


Buffer& Buffer::operator= (const Buffer& x)
{

  if(&x != this)
  {
    if(x.size_ > capacity_)
    {
      if(free_) free(data_);//delete[] data_;
      data_ = (char*)malloc(x.capacity_);//new char[x.capacity_];
      capacity_ = x.capacity_;
      free_ = true;
    }
    if(x.size_ != 0) std::memcpy (data_, x.data_, x.size_);
    size_ = x.size_;
  }
  return *this;
}

void Buffer::swap(Buffer& x)
{
  char* d(x.data_);
  size_type s(x.size_);
  size_type c(x.capacity_);
  bool f(x.free_);

  x.data_ = data_;
  x.size_ = size_;
  x.capacity_ = capacity_;
  x.free_ = free_;

  data_ = d;
  size_ = s;
  capacity_ = c;
  free_ = f;
}

char* Buffer::detach()
{
  char* r(data_);
  data_ = 0;
  size_ = 0;
  capacity_ = 0;

  return r;
}

void Buffer::assign(const void* d, size_type s)
{
  if(s > capacity_)
  {
    if(free_) free(data_);//delete[] data_;
    data_ = (char*)malloc(s);//new char[s];
    capacity_ = s;
    free_ = true;
  }
  if(s != 0) std::memcpy (data_, d, s);
  size_ = s;
}

void Buffer::assign(void* d, size_type s, size_type c, bool own)
{
  if(free_) delete[] data_;
  data_ = static_cast<char*>(d);
  size_ = s;
  capacity_ = c;
  free_ = own;
}

void Buffer::append(const Buffer& b)
{
  append(b.data(), b.size());
}

void Buffer::append(const void* d, size_type s)
{
  if(s != 0)
  {
    size_type newsize(size_ + s);

    if(capacity_ < newsize) capacity (newsize);
    //TODO: replacement policies instead of resizing
    std::memcpy(data_ + size_, d, s);
    size_ = newsize;
  }
}

void Buffer::fill(char v)
{
  if(size_ > 0) std::memset(data_, v, size_);
}

Buffer::size_type Buffer::size() const
{
  return size_;
}

bool Buffer::size(size_type s)
{
  bool r(false);

  if (capacity_ < s) r = capacity(s);
  size_ = s;
  return r;
}

Buffer::size_type Buffer::capacity() const
{
  return capacity_;
}

bool Buffer::capacity(size_type c)
{
  // Ignore capacity decrease requests.
  //
  if(capacity_ >= c) return false;

  char* d(new char[c]);

  if(size_ != 0) std::memcpy (d, data_, size_);

  if(free_) free(data_);//delete[] data_;
  data_ = d;
  capacity_ = c;
  free_ = true;

  return true;
}

bool Buffer::empty () const
{
  return size_ == 0;
}

void Buffer::clear ()
{
  size_ = 0;
}

char* Buffer::data ()
{
  return data_;
}

const char* Buffer::data () const
{
  return data_;
}

char& Buffer::operator[] (size_type i)
{
  return data_[i];
}

char Buffer::operator[] (size_type i) const
{
  return data_[i];
}

char& Buffer::at (size_type i)
{
  if (i >= size_)
    throw std::out_of_range ("index out of range");

  return data_[i];
}

char Buffer::at (size_type i) const
{
  if (i >= size_)
    throw std::out_of_range ("index out of range");

  return data_[i];
}

Buffer::size_type Buffer::find (char v, size_type pos) const
{
  if (size_ == 0 || pos >= size_)
    return npos;

  char* p (static_cast<char*> (std::memchr (data_ + pos, v, size_ - pos)));
  return p != 0 ? static_cast<size_type> (p - data_) : npos;
}

Buffer::size_type Buffer::rfind (char v, size_type pos) const
{
  // memrchr() is not standard.
  //
  if (size_ != 0)
  {
    size_type n (size_);

    if (--n > pos)
      n = pos;

    for (++n; n-- != 0; )
      if (data_[n] == v)
        return n;
  }

  return npos;
}



/******************************************************************************
*Operators
******************************************************************************/
bool operator== (const Buffer& a, const Buffer& b)
{
  return a.size () == b.size () &&
         std::memcmp (a.data (), b.data (), a.size ()) == 0;
}

bool operator!= (const Buffer& a, const Buffer& b)
{
  return !(a == b);
}
