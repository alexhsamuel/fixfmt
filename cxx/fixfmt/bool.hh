#pragma once

#include <cassert>
#include <string>

#include "fixfmt/text.hh"

//------------------------------------------------------------------------------

namespace fixfmt {

using std::move;
using std::string;

/*
 * Fixed-width formatter for boolean values.
 */
class Bool
{
public:

  /*
   * Arguments to a boolean formatter.
   */
  struct Args
  {
    int     size        = 5;
    string  true_str    = "True";
    string  false_str   = "False";
    float   pos         = PAD_POS_LEFT_JUSTIFY;
  };

  Bool()                            = default;
  Bool(Bool const&)                 = default;
  Bool(Bool&&)                      = default;
  Bool& operator=(Bool const&)      = default;
  Bool& operator=(Bool&&)           = default;
  ~Bool() noexcept                  = default;

  Bool(Args const& args) 
    : args_(args) { check(args_); set_up(); }
  Bool(Args&& args)              
    : args_(std::move(args)) { check(args_); set_up(); }

  Bool(
    string const& true_str, 
    string const& false_str)
  : Bool(Args{
      (int) std::max(string_length(true_str), string_length(false_str)),
      true_str, false_str})
  {
  }

  Args const& get_args() const noexcept
    { return args_; }
  void set_args(Args const& args)
    { check(args); args_ = args; set_up(); }
  void set_args(Args&& args)
    { check(args); args_ = std::move(args); set_up(); }

  size_t get_width() const noexcept { return args_.size; }
  string operator()(bool const val) const
    { return val ? true_ : false_; }

private:

  static void check(Args const&) {}
  void set_up();

  Args args_;

  string true_;
  string false_;

};


inline void
Bool::set_up()
{
  true_  = palide(args_.true_str,  args_.size, "", " ", 1, args_.pos);
  false_ = palide(args_.false_str, args_.size, "", " ", 1, args_.pos);
}


}  // namespace fixfmt

