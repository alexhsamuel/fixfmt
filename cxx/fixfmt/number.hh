#pragma once

#include <cassert>
#include <cstring>
#include <ostream>
#include <sstream>
#include <string>

#include "fixfmt/base.hh"
#include "fixfmt/math.hh"
#include "fixfmt/text.hh"

//------------------------------------------------------------------------------

namespace fixfmt {

using std::string;

class Number
{
public:

  constexpr static char PAD_SPACE      = ' ';
  constexpr static char PAD_ZERO       = '0';

  constexpr static char SIGN_NONE      = ' ';
  constexpr static char SIGN_NEGATIVE  = '-';
  constexpr static char SIGN_ALWAYS    = '+';

  constexpr static int  PRECISION_NONE = -1;

  /*
   * Arguments to a number formatter.
   */
  struct Args
  {
    int     size            = 8;
    int     precision       = PRECISION_NONE;
    char    sign            = SIGN_NEGATIVE;
    char    pad             = ' ';
    char    point           = '.';
    char    bad             = '#';
    string  nan             = "NaN";
    string  inf             = "inf";
  };
  
  Number()                              = default;
  Number(Number const&)                 = default;
  Number(Number&&) noexcept             = default;
  Number& operator=(Number const&)      = default;
  Number& operator=(Number&&) noexcept  = default;
  ~Number() noexcept                    = default;

  Number(Args const& args) 
    : args_(args) { check(args_); set_up(); }
  Number(Args&& args) 
    : args_(std::move(args)) { check(args_); set_up(); }

  /*
   * Convenience ctor for the most common options.
   */
  explicit 
  Number(
      int   const size,
      int   const precision =PRECISION_NONE,
      char  const sign      =SIGN_NEGATIVE)
  : Number(Args{size, .precision=precision, .sign=sign})
  {
  }

  Args const& get_args() const noexcept
    { return args_; }
  void set_args(Args const& args) 
    { check(args); args_ = args; set_up(); }
  void set_args(Args&& args) 
    { check(args); args_ = std::move(args); set_up(); }

  size_t        get_width() const noexcept { return width_; }
  string        operator()(long val) const;
  string        operator()(double val) const;

  // Make sure we use the integer implementation for integral types.
  string operator()(int            val) const { return operator()((long) val); }
  string operator()(short          val) const { return operator()((long) val); }
  string operator()(char           val) const { return operator()((long) val); }
  string operator()(unsigned long  val) const { return operator()((long) val); }
  string operator()(unsigned int   val) const { return operator()((long) val); }
  string operator()(unsigned short val) const { return operator()((long) val); }
  string operator()(unsigned char  val) const { return operator()((long) val); }

private:

  static void check(Args const&);
  char get_sign_char(bool nonneg) const;
  string format_inf_nan(string const& str, int sign) const;
  void set_up();

  Args      args_ = {};

  size_t    width_;
  string    nan_;
  string    pos_inf_;
  string    neg_inf_;
  string    bad_;

};


//------------------------------------------------------------------------------

inline void
Number::check(
  Args const& args)
{
  // FIXME: Use Expect() or similar.
  assert(args.size >= 0);
  assert(args.precision == PRECISION_NONE || args.precision >= 0);
  assert(args.size > 0 || args.precision > 0);
  assert(args.pad == PAD_SPACE || args.pad == PAD_ZERO);
  assert(
         args.sign == SIGN_NONE 
      || args.sign == SIGN_NEGATIVE 
      || args.sign == SIGN_ALWAYS);
}


inline char 
Number::get_sign_char(
  bool const nonneg) 
  const
{
  assert(args_.sign != SIGN_NONE);
  return nonneg ? (args_.sign == SIGN_ALWAYS ? '+' : ' ') : '-';
}


inline string 
Number::format_inf_nan(
  string const& str, 
  int const sign) 
  const
{
  // Tack on the sign.
  bool const has_sign = args_.sign != SIGN_NONE;
  string result = 
    ! has_sign || sign == 0 ? str
    : get_sign_char(sign > 0) + str;
  int const len = string_length(result);
  int const size = args_.size + has_sign;

  // Try to put it in the integer part.
  if (len <= (int) width_)
    return pad(
      pad(result, size, " ", PAD_POSITION_RIGHT_JUSTIFY), 
      width_, " ", PAD_POSITION_LEFT_JUSTIFY);
  else {
    // Doesn't fit at all.
    string_truncate(result, width_);
    return result;
  }
}


inline void
Number::set_up()
{
  width_ =
        args_.size
      + (args_.precision == PRECISION_NONE ? 0 : 1 + args_.precision)
      + (args_.sign == SIGN_NEGATIVE || args_.sign == SIGN_ALWAYS ? 1 : 0);
  nan_ = format_inf_nan(args_.nan, 0);
  pos_inf_ = format_inf_nan(args_.inf,  1);
  neg_inf_ = format_inf_nan(args_.inf, -1);
  bad_ = std::string(width_, args_.bad);
}


}  // namespace fixfmt

