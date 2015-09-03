#pragma once

#include <cassert>
#include <string>

#include "text.hh"

using std::string;

//------------------------------------------------------------------------------

namespace fixfmt {

class Bool
{
private:

  size_t const size_;
  string const true_;
  string const false_;

public:

  Bool(
      string const& true_str,
      string const& false_str,
      size_t size,
      bool pad_left)
  : size_(size),
    true_(palide(true_str, size, "", ' ', 1.0, pad_left)),
    false_(palide(false_str, size, "", ' ', 1.0, pad_left))
  {
    assert(true_.size() == size_);
    assert(false_.size() == size_);
  }

  Bool(
    string const& true_str="True",
    string const& false_str="False")
    : Bool(
        true_str, false_str,
        std::max(true_str.size(), false_str.size()),
        false)
  {
  }

  size_t get_width() const { return size_; }

  size_t get_size() const { return size_; }

  string operator()(bool val)
  {
    return val ? true_ : false_;
  }

};


}  // namespace fixfmt

