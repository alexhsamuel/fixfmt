#pragma once

#include <cassert>
#include <string>

#include "fixfmt/text.hh"

//------------------------------------------------------------------------------

namespace fixfmt {

using std::string;

class Bool
{
public:

  Bool(
      string const& true_str,
      string const& false_str,
      size_t size,
      bool pad_left)
  : size_(size),
    pad_left_(pad_left),
    true_(true_str),
    false_(false_str),
    true_result_(palide(true_str, size, "", " ", 1.0, pad_left)),
    false_result_(palide(false_str, size, "", " ", 1.0, pad_left))
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
  string        get_true()      const { return true_; }
  string        get_false()     const { return false_; }

  string operator()(bool val) const
  {
    return val ? true_result_ : false_result_;
  }

private:

  size_t const size_;
  bool const pad_left_;

  string const true_;
  string const false_;

  string const true_result_;
  string const false_result_;

};


}  // namespace fixfmt

