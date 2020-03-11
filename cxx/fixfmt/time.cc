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
  if (val == NAT_VALUE)
    return nat_;

  // FIXME: Validate range.

  // Find the whole number of seconds, and the fractional seconds scaled up
  // by pow10(precision).
  time_t whole;
  long frac;
  if (round_scale_) {
    // Round at required precision.
    // FIXME: Do bankers' rounding; this rounds half away from zero.
    long const rounded = (val + round_scale_ / 2) / round_scale_;
    // Separate whole and fractional seconds.
    whole = (time_t) (rounded / prec_scale_);
    frac = rounded % prec_scale_;
  }
  else {
    // More precision than available.
    whole = (time_t) (val / scale_);
    frac = (val % scale_) * (prec_scale_ / scale_);
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
  if (prec_ > 0) {
    // Decimal point.
    result[pos++] = '.';
    // Render digits of the fractional seconds.
    for (int i = 0; i < prec_; ++i) {
      result[pos + prec_ - 1 - i] = '0' + frac % 10;
      frac /= 10;
    }
    assert(frac == 0);
    pos += prec_;
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

