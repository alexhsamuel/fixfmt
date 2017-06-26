#pragma once

#include <cassert>
#include <string>

#include "fixfmt/text.hh"

//------------------------------------------------------------------------------

namespace fixfmt {

using std::string;

/*
 * Fixed-width formatter for strings.
 */
class String
{
public:

  /*
   * Arguments to a string formatter.
   */
  struct Args
  {
    int     size            = 8;
    string  ellipsis        = ELLIPSIS;
    string  pad             = " ";
    float   elide_pos       = 1;
    float   pad_pos         = 1;
  };

  String()                              = default;
  String(String const&)                 = default;
  String(String&&)                      = default;
  String& operator=(String const&)      = default;
  String& operator=(String&&)           = default;
  ~String() noexcept                    = default;

  String(Args const& args)          : args_(args) { check(args_); }
  String(Args&& args)               : args_(std::move(args)) { check(args_); }
  explicit String(int const size)   : String(Args{size}) {}

  Args const&   get_args() const noexcept { return args_; }
  void          set_args(Args const& args) { check(args); args_ = args; }
  void          set_args(Args&& args) { check(args); args_ = std::move(args); }

  size_t        get_width() const noexcept { return args_.size; }
  string        operator()(string const& str) const;

private:

  static void   check(Args const&);

  Args args_ = {};

};


inline void
String::check(
  Args const& args)
{
  assert(args.ellipsis.length() <= (size_t) args.size);
  assert(0 <= args.elide_pos && args.elide_pos <= 1);
  assert(0 <= args.pad_pos && args.pad_pos <= 1);
}


inline string 
String::operator()(
  string const& str) 
  const
{
  return palide(
    str, args_.size, args_.ellipsis, args_.pad, args_.elide_pos, args_.pad_pos);
}


}  // namespace fixfmt

