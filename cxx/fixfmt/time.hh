#pragma once

#include <cmath>
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
  : width_(25 + (precision == PRECISION_NONE ? 0 : 1 + precision)),
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


//------------------------------------------------------------------------------

}  // namespace fixfmt

