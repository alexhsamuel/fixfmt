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
  char const* pad = " ";
  int left = false;
  Arg::ParseTupleAndKeywords(
      args, kw_args, "sI|sp", arg_names, &str, &length, &pad, &left);

  // FIXME: Validate args.
  if (strlen(pad) == 0)
    throw ValueError("empty pad");

  string r = fixfmt::pad(string(str), length, pad, (bool) left);
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
  char const* pad = " ";
  float position = 1.0;
  int left = false;
  Arg::ParseTupleAndKeywords(
    args, kw_args, "sI|ssfp", arg_names,
    &str, &length, &ellipsis, &pad, &position, &left);

  // FIXME: Validate args.
  if (strlen(pad) == 0)
    throw ValueError("empty pad");

  string r = fixfmt::palide(
    string(str), length, string(ellipsis), pad, position, (bool) left);
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


