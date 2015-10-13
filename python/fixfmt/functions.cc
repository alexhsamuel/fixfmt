#include <cassert>
#include <string>

#include "fixfmt/text.hh"
#include "py.hh"

using namespace py;
using std::string;

//------------------------------------------------------------------------------

// FIXME: Make these unicode-aware.

namespace {

ref<Object> pad(Module* module, Tuple* args, Dict* kw_args)
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


ref<Object> elide(Module* module, Tuple* args, Dict* kw_args)
{
  static char const* arg_names[] = {
    "string", "length", "ellipsis", "position", nullptr};
  char const* str;
  int length;
  char const* ellipsis = fixfmt::ELLIPSIS;
  float position = 1.0;
  Arg::ParseTupleAndKeywords(
    args, kw_args, "sI|sf", arg_names, &str, &length, &ellipsis, &position);

  string r = fixfmt::elide(string(str), length, string(ellipsis), position);
  return Unicode::from(r);
}


ref<Object> palide(Module* module, Tuple* args, Dict* kw_args)
{
  static char const* arg_names[] = {
    "string", "length", "ellipsis", "pad", "position", "left", nullptr };
  char const* str;
  int length;
  char const* ellipsis = fixfmt::ELLIPSIS;
  int pad = ' ';
  float position = 1.0;
  int left = false;
  Arg::ParseTupleAndKeywords(
    args, kw_args, "sI|sCfp", arg_names,
    &str, &length, &ellipsis, &pad, &position, &left);

  string r = fixfmt::palide(
    string(str), length, string(ellipsis), (char) pad, position, (bool) left);
  return Unicode::from(r);
}


}

//------------------------------------------------------------------------------

Methods<Module>& add_functions(Methods<Module>& methods)
{
  methods
    .add<elide>("elide")
    .add<pad>("pad")
    .add<palide>("palide")
    ;
  return methods;
}


