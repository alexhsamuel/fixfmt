#pragma once

#include <cmath>
#include <string>

#include "math.hh"
#include "fixfmt/text.hh"

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

  // NaT ("not a time") value used by numpy.datetime64.
  constexpr static long NAT_VALUE   = -9223372036854775807l - 1;

  constexpr static int PRECISION_NONE = -1;

  TickTime(
    long    const  scale    =SCALE_SEC,
    int     const  precision=PRECISION_NONE,
    string  const& nat      ="NaT")
  : width_(25 + (precision == PRECISION_NONE ? 0 : 1 + precision)),
    bad_result_(width_, '#'),  // FIXME
    scale_(scale),
    precision_(precision),
    nat_(palide(nat, width_, "", " ", 1, PAD_POS_LEFT_JUSTIFY)),
    prec_(precision_ == PRECISION_NONE ? 0 : precision_),
    prec_scale_(pow10(prec_)),
    round_scale_(scale_ > prec_scale_ ? scale_ / prec_scale_ : 0)
  {
  }

  size_t        get_width()     const { return width_; }

  long          get_scale()     const { return scale_; }
  int           get_precision() const { return precision_; }
  string const& get_nat()       const { return nat_; }
  
  string operator()(long val) const;

private:

  size_t    const width_;
  string    const bad_result_;

  long      const scale_;
  int       const precision_;
  string    const nat_;

  // Intermediate values used in formatting computation.
  int       const prec_;
  long      const prec_scale_;
  long      const round_scale_;

};


//------------------------------------------------------------------------------

}  // namespace fixfmt

