#pragma once

#include <cassert>
#include <cstring>
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
      int      const size,
      int      const precision=PRECISION_NONE,
      char     const pad=' ',
      char     const sign=SIGN_NEGATIVE,
      string   const nan="NaN",
      string   const inf="inf",
      char     const point='.',
      char     const bad='#');

  size_t           get_width()     const { return width_; }

  int              get_size()      const { return size_; }
  int              get_precision() const { return precision_; }
  char             get_pad()       const { return pad_; }
  char             get_sign()      const { return sign_; }
  char             get_point()     const { return point_; }
  string const&    get_nan()       const { return nan_; }
  string const&    get_inf()       const { return inf_; }
  char             get_bad()       const { return bad_; }

  void format(long const val, char* const buf) const;

  // Make sure we use the integer implementation for integer types.
  void format(int            val, char* const buf) const
    { format((long) val, buf); }
  void format(short          val, char* const buf) const
    { format((long) val, buf); }
  void format(char           val, char* const buf) const
    { format((long) val, buf); }
  void format(unsigned long  val, char* const buf) const
    { format((long) val, buf); }
  void format(unsigned int   val, char* const buf) const
    { format((long) val, buf); }
  void format(unsigned short val, char* const buf) const
    { format((long) val, buf); }
  void format(unsigned char  val, char* const buf) const
    { format((long) val, buf); }

  template<typename T>
  string operator()(T val) const
  {
    char buf[width_];
    format(val, buf);
    return string(buf, width_);
  }

private:

  char get_sign_char(bool const nonneg) const
  {
    assert(sign_ != SIGN_NONE);
    return nonneg ? (sign_ == SIGN_ALWAYS ? '+' : ' ') : '-';
  }

  std::string format_nan(string const& nan) const
  {
    string result = pad(nan, width_, " ", true);
    // Truncate if necessary.
    return result.substr(0, width_);
  }

  std::string format_inf(string const& inf, bool const nonneg) const
  {
    int const num_pad = width_ - inf.length() - (sign_ != SIGN_NONE);
    string result(num_pad < 0 ? 0 : num_pad, ' ');
    if (sign_ != SIGN_NONE)
      result += get_sign_char(nonneg);
    result += inf;
    // Truncate if necessary.
    return result.substr(0, width_);
  }

  void format(double const val, char* const buf) const;

  /**
   * Formats 'width' digits of 'val' into 'buf', filling on the left with
   * 'fill'.  Returns the number of characters filled, or -1 if 'val' didn't
   * fit in 'width' digits.
   */
  static int format_long(char* buf, int width, unsigned long val, char fill);

  void format(
      bool  const nonneg, long  const whole, long  const frac,
      char* const buf) const;

  int      const size_;
  int      const precision_;
  char     const pad_;
  char     const sign_;
  char     const point_;
  size_t   const width_;
  string   const nan_;
  string   const inf_;
  char     const bad_;
  string   const nan_pad_;
  string   const pos_inf_;
  string   const neg_inf_;

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
    string   const nan,
    string   const inf,
    char     const point,
    char     const bad)
: size_{size},
  precision_{precision},
  pad_{pad},
  sign_{sign},
  point_{point},
  width_(
      size_
      + (precision_ == PRECISION_NONE ? 0 : 1 + precision)
      + (sign == SIGN_NEGATIVE || sign == SIGN_ALWAYS ? 1 : 0)),
  nan_{std::move(nan)},
  inf_{std::move(inf)},
  bad_{bad},
  nan_pad_{format_nan(nan)},
  pos_inf_{format_inf(inf, true)},
  neg_inf_{format_inf(inf, false)}
{
  assert(size_ >= 0);
  assert(precision_ == PRECISION_NONE || precision_ >= 0);
  assert(pad_ == PAD_SPACE || pad_ == PAD_ZERO);
  assert(
      sign_ == SIGN_NONE || sign_ == SIGN_NEGATIVE || sign_ == SIGN_ALWAYS);
}


inline void Number::format(long const val, char* const buf) const
{
  bool const nonneg = val >= 0;
  if (! nonneg && sign_ == SIGN_NONE)
    // With SIGN_NONE, we can't render negative numbers.
    set(buf, bad_, width_);
  else
    format(nonneg, labs(val), 0, buf);
}


inline void Number::format(double const val, char* const buf) const
{
  bool const nonneg = val >= 0;
  if (isnan(val))
    nan_pad_.copy(buf, width_);
  else if (val < 0 && sign_ == SIGN_NONE)
    // With SIGN_NONE, we can't render negative numbers.
    set(buf, bad_, width_);
  else if (isinf(val))
    // Copy the appropriate infinity.
    (val > 0 ? pos_inf_ : neg_inf_).copy(buf, width_);
  else {
    int    const precision = precision_ == PRECISION_NONE ? 0 : precision_;
    double const abs_val   = round(fabs(val), precision);
    long   const whole     = abs_val;
    long   const frac      = round((abs_val - whole) * pow10(precision));
    format(nonneg, whole, frac, buf);
  }
}


inline int Number::format_long(char* buf, int width, unsigned long val,
                               char fill)
{
  if (val == 0 && width > 0)
    // For exact zero, render a single zero.
    buf[--width] = '0';
  else {
    // Render digits.
    while (width > 0 && val > 0) {
      buf[--width] = '0' + (val % 10);
      val /= 10;
    }
    // We should have rendered all the value; otherwise we've overflowed.
    if (val != 0)
      return -1;
  }

  // Fill the rest.
  set(buf, fill, width);
  return width;
}


inline void Number::format(
    bool  const nonneg,
    long  const whole,
    long  const frac,
    char* const buf) const
{
  int const sign_len = sign_ == SIGN_NONE ? 0 : 1;

  // Format the whole number part.
  int const whole_fill{format_long(buf + sign_len, size_, whole, pad_)};
  if (whole_fill < 0)
    // Didn't fit.
    set(buf, bad_, width_);
  else {
    if (sign_ != SIGN_NONE) {
      // Add the sign.
      char const sign_char = get_sign_char(nonneg);
      if (pad_ == PAD_SPACE) {
        // Space padding comes outside the sign.
        buf[0] = pad_;
        buf[whole_fill] = sign_char;
      }
      else
        buf[0] = sign_char;
    }

    if (precision_ != PRECISION_NONE) {
      // Add the decimal point.
      char* const point = buf + sign_len + size_;
      *point = point_;
      if (precision_ > 0) {
        // Format the fractional part.
        int const fill = format_long(point + 1, precision_, frac, '0');
        assert(fill >= 0); unused(fill);
      }
    }
  }
}


}  // namespace fixfmt

