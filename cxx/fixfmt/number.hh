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

  Number(
      int      size,
      int      precision=PRECISION_NONE,
      char     pad=' ',
      char     sign=SIGN_NEGATIVE,
      string   nan="NaN",
      string   inf="inf",
      char     point='.',
      char     bad='#');

  size_t           get_width()     const { return width_; }

  int              get_size()      const { return size_; }
  int              get_precision() const { return precision_; }
  char             get_pad()       const { return pad_; }
  char             get_sign()      const { return sign_; }
  char             get_point()     const { return point_; }
  string const&    get_nan()       const { return nan_; }
  string const&    get_inf()       const { return inf_; }
  char             get_bad()       const { return bad_; }

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
    assert(sign_ != SIGN_NONE);
    return nonneg ? (sign_ == SIGN_ALWAYS ? '+' : ' ') : '-';
  }

  string format_inf_nan(string const& str, int const sign) const
  {
    // Tack on the sign.
    bool const has_sign = sign_ != SIGN_NONE;
    string result = 
      ! has_sign || sign == 0 ? str
      : get_sign_char(sign > 0) + str;
    int const len = string_length(result);
    int const size = size_ + has_sign;

    // Try to put it in the integer part.
    if (len <= (int) width_)
      return pad(pad(result, size, " ", true), width_, " ", false);
    else {
      // Doesn't fit at all.
      string_truncate(result, width_);
      return result;
    }
  }

  int      const size_;
  int      const precision_;
  char     const pad_;
  char     const sign_;
  char     const point_;
  size_t   const width_;
  string   const nan_;
  string   const inf_;
  char     const bad_;

  // Preformatted results.
  string   const nan_result_;
  string   const pos_inf_result_;
  string   const neg_inf_result_;
  string   const bad_result_;

};


//------------------------------------------------------------------------------

namespace {

// FIXME: Elsewhere.
inline void set(char* const buf, char const character, size_t const num)
{
  if (num > 0)
    memset(buf, character, num);
}


}  // anonymous namespace


inline Number::Number(
    int      const size,
    int      const precision,
    char     const pad,
    char     const sign,
    string         nan,
    string         inf,
    char     const point,
    char     const bad)
: size_(size),
  precision_(precision),
  pad_(pad),
  sign_(sign),
  point_(point),
  width_(
      size_
      + (precision_ == PRECISION_NONE ? 0 : 1 + precision)
      + (sign == SIGN_NEGATIVE || sign == SIGN_ALWAYS ? 1 : 0)),
  nan_(std::move(nan)),
  inf_(std::move(inf)),
  bad_(bad),
  nan_result_(format_inf_nan(nan_, 0)),
  pos_inf_result_(format_inf_nan(inf_,  1)),
  neg_inf_result_(format_inf_nan(inf_, -1)),
  bad_result_(width_, bad)
{
  assert(size_ >= 0);
  assert(precision_ == PRECISION_NONE || precision_ >= 0);
  assert(size_ > 0 || precision_ > 0);
  assert(pad_ == PAD_SPACE || pad_ == PAD_ZERO);
  assert(
      sign_ == SIGN_NONE || sign_ == SIGN_NEGATIVE || sign_ == SIGN_ALWAYS);
}


}  // namespace fixfmt

