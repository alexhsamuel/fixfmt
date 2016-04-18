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
    int      size           = 8;
    int      precision      = PRECISION_NONE;
    char     sign           = SIGN_NEGATIVE;
    char     pad            = ' ';
    char     point          = '.';
    string   const nan      = "NaN";
    string   const inf      = "inf";
    char     const bad      = '#';
  };
  
  Number(Args args);

  /*
   * Convenience ctor for the most common options.
   */
  Number(
      int   const size,
      int   const precision =PRECISION_NONE,
      char  const sign      =SIGN_NEGATIVE)
  : Number(Args{size, precision, sign})
  {
  }

  size_t           get_width()     const { return width_; }

  int              get_size()      const { return args_.size; }
  int              get_precision() const { return args_.precision; }
  char             get_pad()       const { return args_.pad; }
  char             get_sign()      const { return args_.sign; }
  char             get_point()     const { return args_.point; }
  string const&    get_nan()       const { return args_.nan; }
  string const&    get_inf()       const { return args_.inf; }
  char             get_bad()       const { return args_.bad; }

  string operator()(long           val) const;
  string operator()(double         val) const;

  // Make sure we use the integer implementation for integral types.
  string operator()(int            val) const { return operator()((long) val); }
  string operator()(short          val) const { return operator()((long) val); }
  string operator()(char           val) const { return operator()((long) val); }
  string operator()(unsigned long  val) const { return operator()((long) val); }
  string operator()(unsigned int   val) const { return operator()((long) val); }
  string operator()(unsigned short val) const { return operator()((long) val); }
  string operator()(unsigned char  val) const { return operator()((long) val); }

private:

  char get_sign_char(bool const nonneg) const
  {
    assert(args_.sign != SIGN_NONE);
    return nonneg ? (args_.sign == SIGN_ALWAYS ? '+' : ' ') : '-';
  }

  string format_inf_nan(string const& str, int const sign) const
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
      return pad(pad(result, size, " ", true), width_, " ", false);
    else {
      // Doesn't fit at all.
      string_truncate(result, width_);
      return result;
    }
  }

  Args      const args_;
  size_t    const width_;

  // Preformatted outputs.
  string    const nan_result_;
  string    const pos_inf_result_;
  string    const neg_inf_result_;
  string    const bad_result_;

};


//------------------------------------------------------------------------------

inline 
Number::Number(
  Args args)
: args_(std::move(args)),
  width_(
        args_.size
      + (args_.precision == PRECISION_NONE ? 0 : 1 + args_.precision)
      + (args_.sign == SIGN_NEGATIVE || args_.sign == SIGN_ALWAYS ? 1 : 0)),
  nan_result_(format_inf_nan(args_.nan, 0)),
  pos_inf_result_(format_inf_nan(args_.inf,  1)),
  neg_inf_result_(format_inf_nan(args_.inf, -1)),
  bad_result_(width_, args_.bad)
{
  // FIXME: Use Expect() or similar.
  assert(args_.size >= 0);
  assert(args_.precision == PRECISION_NONE || args_.precision >= 0);
  assert(args_.size > 0 || args_.precision > 0);
  assert(args_.pad == PAD_SPACE || args_.pad == PAD_ZERO);
  assert(
         args_.sign == SIGN_NONE 
      || args_.sign == SIGN_NEGATIVE 
      || args_.sign == SIGN_ALWAYS);
}


}  // namespace fixfmt

