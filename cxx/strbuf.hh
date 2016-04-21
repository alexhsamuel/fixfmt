#pragma once

#include <cstring>
#include <string>

//------------------------------------------------------------------------------

class StringBuffer
{
public:

  // FIXME: Use span from GSL.

  StringBuffer(
    char* const ptr, 
    size_t const size, 
    size_t const pos=0) 
    noexcept
  : ptr_(ptr), 
    size_(size), 
    pos_(pos)
  {
  }

  StringBuffer(StringBuffer const&) noexcept            = default;
  StringBuffer(StringBuffer&&) noexcept                 = default;
  StringBuffer& operator=(StringBuffer const&) noexcept = default;
  StringBuffer& operator=(StringBuffer&&) noexcept      = default;
  ~StringBuffer() noexcept                              = default;

  StringBuffer& 
  append(
    char const c)
  {
    check_space(1);
    ptr_[pos_++] = c;
    return *this;
  }

  StringBuffer&
  append(
    char const* const str)
  {
    for (char const* p = str; *p != 0; ++p)
      append(*p);
  }

  StringBuffer&
  append(
    std::string const& str,
    size_t const subpos=0,
    size_t const sublen=std::string::npos)
  {
    size_t const len 
      = sublen == std::string::npos ? str.size() - subpos : sublen;
    check_space(len);
    memcpy(ptr_ + pos_, &str[subpos], len);
    pos_ += len;
    return *this;
  }

private:

  void check_space(size_t const len) { assert(pos_ + len <= size_); }

  zstring const ptr_;
  size_t const size_;
  size_t pos_;

};


