#pragma once

#include <cassert>
#include <string>

#include "fixfmt/text.hh"

//------------------------------------------------------------------------------

namespace fixfmt {

using std::string;

class Bool
{
private:

  size_t const size_;
  bool const pad_left_;

  string const true_;
  string const false_;

public:

  Bool(
      string const& true_str,
      string const& false_str,
      size_t size,
      bool pad_left)
  : size_(size),
    pad_left_(pad_left),
    true_(palide(true_str, size, "", " ", 1.0, pad_left)),
    false_(palide(false_str, size, "", " ", 1.0, pad_left))
  {
    assert(true_.size() == size_);
    assert(false_.size() == size_);
  }

  Bool(
    string const& true_str="True",
    string const& false_str="False")
    : Bool(
        true_str, false_str,
        std::max(string_length(true_str), string_length(false_str)),
        false)
  {
  }

  size_t        get_width()     const { return size_; }

  size_t        get_size()      const { return size_; }
  bool          get_pad_left()  const { return pad_left_; }

  string operator()(bool val) const
  {
    return val ? true_ : false_;
  }

};


}  // namespace fixfmt

