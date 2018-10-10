#include <cassert>
#include <ctime>
#include <iostream>

#include "date.hh"

//------------------------------------------------------------------------------

namespace fixfmt {

string 
TickDate::operator()(
  long val) 
  const 
{
  // FIXME: Validate range.

  time_t const whole = 86400 * val;
  struct tm time;
  if (gmtime_r(&whole, &time) == NULL) 
    return "####-##-##";

  // Render the time in whole seconds.
  char buf[] = "??????????";
  size_t const pos = strftime(buf, 11, "%Y-%m-%d", &time);
  if (pos != 10)
    return "####-##-##";

  return buf;
}


//------------------------------------------------------------------------------

}  // namespace fixfmt

