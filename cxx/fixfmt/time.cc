#include <cassert>
#include <ctime>

#include "time.hh"

//------------------------------------------------------------------------------

namespace fixfmt {

string 
TickTime::operator()(
  long val) 
  const 
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

  // UTC offset.
  result[pos++] = '+';
  result[pos++] = '0';
  result[pos++] = '0';
  result[pos++] = ':';
  result[pos++] = '0';
  result[pos++] = '0';

  assert(pos == width_);
  return result;
}


//------------------------------------------------------------------------------

}  // namespace fixfmt

