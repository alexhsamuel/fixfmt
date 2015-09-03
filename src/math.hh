#pragma once

#include <cmath>

//------------------------------------------------------------------------------

namespace fixfmt {

inline double pow10(int n)
{
  static constexpr double POW10[40] = {
    1e-20, 1e-19, 1e-18, 1e-17, 1e-16, 1e-15, 1e-14, 1e-13, 1e-12, 1e-11,
    1e-10, 1e-09, 1e-08, 1e-07, 1e-06, 1e-05, 1e-04, 1e-03, 1e-02, 1e-01,
    1    , 1e+01, 1e+02, 1e+03, 1e+04, 1e+05, 1e+06, 1e+07, 1e+08, 1e+09,
    1e+10, 1e+11, 1e+12, 1e+13, 1e+14, 1e+15, 1e+16, 1e+17, 1e+18, 1e+19,
  };

  if (-20 <= n && n < 20)
    return POW10[n + 20];
  else
    return pow(10, n);
}


/** Rounds 'val' to the nearest integer using banker's roudning.  */
inline long round(double val)
{
  long const i = (long) val;
  double const r = val - i;
  return i + (val > 0
    ? (r <  0.5 || (i % 2 == 0 && r ==  0.5) ? 0 :  1)
    : (r > -0.5 || (i % 2 == 0 && r == -0.5) ? 0 : -1));
}

/** Rounds 'val' to 'num_digits' of precision using banker's rounding.  */
inline double round(double val, int num_digits)
{
  double const mult = pow10(num_digits);
  return round(val * mult) / mult;
}


}  // namespace fixfmt
