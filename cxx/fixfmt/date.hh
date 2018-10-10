#pragma once

#include <cmath>
#include <string>

#include "math.hh"

//------------------------------------------------------------------------------

// FIXME: This is a kind of hacky placeholder implementation until we have a
// proper date implementation to work with.

namespace fixfmt {

using std::string;

class TickDate
{
public:

  constexpr static int PRECISION_NONE = -1;

  TickDate()
  {
  }

  size_t    get_width()     const { return 10; }

  string operator()(long val) const;

private:

};


//------------------------------------------------------------------------------

}  // namespace fixfmt

