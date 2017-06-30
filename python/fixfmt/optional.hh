/*------------------------------------------------------------------------------

  Stub for experimental std::optional.

------------------------------------------------------------------------------*/

#pragma once

#include <cassert>
#include <utility>

//------------------------------------------------------------------------------

template<class T>
class optional
{
public:

  optional() : has_value_(false) {}
  optional(T const& value) : has_value_(true), value_(value) {}
  optional(T&& value) : has_value_(true), value_(std::move(value)) {}
  
  optional(optional const&) = default;
  optional(optional&&) = default;
  ~optional() = default;
  optional& operator=(optional const&) = default;
  optional& operator=(optional&&) = default;

  operator bool() { return has_value_; }

  T const& operator*() const { assert(has_value_); return value_; }
  T const& operator->() const { assert(has_value_); return value_; }

private:

  bool has_value_;
  T value_;

};

