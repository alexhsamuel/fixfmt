#pragma once

#include <cassert>
#include <string>

#include "fixfmt/text.hh"

//------------------------------------------------------------------------------

namespace fixfmt {

using std::move;
using std::string;

class Bool
{
public:

  Bool(
      string true_str,
      string false_str,
      size_t size,
      bool pad_left)
  : size_(size),
    pad_left_(pad_left),
    true_(move(true_str)),
    false_(move(false_str)),
    true_result_(palide(true_, size, "", " ", 1.0, pad_left)),
    false_result_(palide(false_, size, "", " ", 1.0, pad_left))
  {
    assert(true_result_.size() == size_);
    assert(false_result_.size() == size_);
  }

  Bool(
    string true_str="True",
    string false_str="False")
    : Bool(
        move(true_str), move(false_str),
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

