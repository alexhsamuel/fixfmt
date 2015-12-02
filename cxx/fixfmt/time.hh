#pragma once

#include <cassert>
#include <cmath>
#include <ctime>
#include <string>

#include "math.hh"

//------------------------------------------------------------------------------

// FIXME: This is a kind of hacky placeholder implementation until we have a
// proper time implementation to work with.

namespace fixfmt {

using std::string;

class TickTime
{
public:

  constexpr static long SCALE_SEC   =          1l;
  constexpr static long SCALE_MSEC  =       1000l;
  constexpr static long SCALE_USEC  =    1000000l;
  constexpr static long SCALE_NSEC  = 1000000000l;

  constexpr static int PRECISION_NONE = -1;

  TickTime(
    long    const scale     =SCALE_SEC,
    int     const precision =PRECISION_NONE)
    : width_(20 + (precision == PRECISION_NONE ? 0 : 1 + precision)),
      bad_result_(width_, '#'),  // FIXME
      scale_(scale),
      precision_(precision)
  {
  }

  size_t    get_width()     const { return width_; }

  long      get_scale()     const { return scale_; }
  int       get_precision() const { return precision_; }
  
  string operator()(long val) const;

private:

  size_t    const width_;
  string    const bad_result_;

  long      const scale_;
  int       const precision_;

};


inline string TickTime::operator()(long val) const 
{
  // FIXME: Validate range.

  int const precision = precision_ == PRECISION_NONE ? 0 : precision_;
  long const prec_scale = pow10(precision);

  // Find the whole number of seconds, and the fractional seconds scaled up
  // by pow10(precision).
  time_t whole;
  long frac;
  if (scale_ > prec_scale) {
    // Round at required precision.
    long const round_scale = scale_ / prec_scale;
    // FIXME: Do bankers' rounding; this rounds half up.
    long const rounded = (val + round_scale / 2) / round_scale;
    // Separate whole and fractional seconds.
    whole = (time_t) (rounded / prec_scale);
    frac = rounded % prec_scale;
  }
  else {
    // More precision than available.
    whole = (time_t) (val / scale_);
    frac = (val % scale_) * (prec_scale / scale_);
  }

  // Break down the whole number of seconds into time components.
  struct tm time;
  if (gmtime_r(&whole, &time) == NULL) 
    return bad_result_;

  // Render the time in whole seconds.
  string result(width_, '?');
  size_t pos = strftime(&result[0], width_, "%Y-%m-%dT%H:%M:%S", &time);
  if (pos != 19)
    return bad_result_;

  // Tack on subsecond precision, if indicated.
  if (precision_ != PRECISION_NONE) {
    // Decimal point.
    result[pos++] = '.';
    // Render digits of the fractional seconds.
    for (int i = 0; i < precision; ++i) {
      result[pos + precision - 1 - i] = '0' + frac % 10;
      frac /= 10;
    }
    assert(frac == 0);
    pos += precision;
  }

  // UTC ('Zulu') time.
  result[pos++] = 'Z';

  assert(pos == width_);
  return result;
}


}  // namespace fixfmt

