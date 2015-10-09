#include <cassert>
#include <string>

#include "fixfmt/text.hh"
#include "py.hh"

using namespace py;
using std::string;

//------------------------------------------------------------------------------

// FIXME: Make these unicode-aware.

namespace {

ref<Object> pad(Module* self, Tuple* args, Dict* kw_args)
{
  static char const* arg_names[] = {
    "string", "length", "pad", "left", nullptr};
  char const* str;
  int length;
  int pad = ' ';
  int left = false;
  Arg::ParseTupleAndKeywords(
      args, kw_args, "sI|Cp", arg_names, &str, &length, &pad, &left);

  string r = fixfmt::pad(string(str), length, (char) pad, (bool) left);
  return Unicode::from(r);
}


}

//------------------------------------------------------------------------------

Methods<Module>& add_functions(Methods<Module>& methods)
{
  methods
    .add<pad>("pad")
    ;
  return methods;
}


