#include <cassert>
#include <cmath>
#include <iomanip>
#include <limits>
#include <string>

#include "fixfmt/double-conversion/double-conversion.h"
#include "fixfmt/double-conversion/fast-dtoa.h"
#include "fixfmt/text.hh"
#include "py.hh"

using namespace py;
using std::string;
using DTSC = fixfmt::double_conversion::DoubleToStringConverter;

//------------------------------------------------------------------------------

// FIXME: Add docstrings.

namespace {

template<typename TYPE> constexpr auto MODE = DTSC::SHORTEST;
template<> constexpr auto MODE<float> = DTSC::SHORTEST_SINGLE;

template<typename TYPE>
ref<Object> analyze_float(Module* module, Tuple* args, Dict* kw_args)
{
  static char const* arg_names[] = {"buf", "max_precision", nullptr};
  PyObject* array_obj;
  int max_precision;
  Arg::ParseTupleAndKeywords(
    args, kw_args, "Oi", arg_names, &array_obj, &max_precision);

  BufferRef buffer(array_obj, PyBUF_ND);
  if (buffer->ndim != 1)
    throw TypeError("not a one-dimensional array");
  if (buffer->itemsize != sizeof(TYPE))
    throw TypeError("wrong itemsize");
  TYPE const* const array = (TYPE const* const) buffer->buf;
  size_t const length = buffer->shape[0];

  bool has_nan = false;
  bool has_pos_inf = false;
  bool has_neg_inf = false;
  size_t num = 0;
  TYPE min = std::numeric_limits<TYPE>::max();
  TYPE max = std::numeric_limits<TYPE>::min();

  // Estimate precision truncating 'precision' digits to the right of the
  // decimal point, and comparing the result to 'tolerance'.  
  // FIXME: Might not be correct for long double.
  int precision = 0;
  TYPE precision_scale = 1;

  // Note: Weird.  LLVM 6.1.0 on OSX vectorizes this loop only if the precision
  // logic is _present_, with the result that it runs _faster_ than without.

  for (size_t i = 0; i < length; ++i) {
    TYPE const val = array[i];
    // Flag NaN.
    if (std::isnan(val)) {
      has_nan = true;
      continue;
    }
    // Flag positive and negative infinity.
    if (std::isinf(val)) {
      if (val > 0)
        has_pos_inf = true;
      else
        has_neg_inf = true;
      continue;
    }
    // Keep count of non-NaN/infinity values.
    ++num;
    // Compute min and max, excluding NaN and infinity.
    if (val < min)
      min = val;
    if (val > max)
      max = val;

    if (precision == max_precision)
      // At max precision; no need to check further.
      ;
    else {
      // We need to check if this value requires additional precision to format
      // accurately.  Start with a naive check.
      double const scaled = val * precision_scale;
      if (long(std::round(scaled)) == scaled)
        // Good to go.
        ;
      else {
        // Might need more precision.  We need to do full-blown formatting.
        char buf[384];  // Enough room for DBL_MAX.
        bool sign;
        int length;
        int decimal_pos;
        DTSC::DoubleToAscii(
          std::abs(val), MODE<TYPE>, 0,
          buf, sizeof(buf), &sign, &length, &decimal_pos);

        auto const prec = std::min(length - decimal_pos, max_precision);
        if (prec > precision) {
          precision = prec;
          precision_scale = fixfmt::pow10(precision);
        }
      }
    }
  }

  // FIXME-PY3: Use a StructSequenceType.
  return (ref<Tuple>) (Tuple::builder
    << Bool::from(has_nan)
    << Bool::from(has_pos_inf)
    << Bool::from(has_neg_inf)
    << Long::FromLong(num)
    << Float::FromDouble(min)
    << Float::FromDouble(max)
    << Long::FromLong(precision)
  );
}


ref<Object> center(Module* module, Tuple* args, Dict* kw_args)
{
  static char const* arg_names[] = {
    "string", "length", "pad", nullptr};
  int length;
  char* str;
  char* pad = nullptr;
#if PY3
  Arg::ParseTupleAndKeywords(
      args, kw_args, 
      "sI|s", arg_names, &str, &length, &pad);
#else
  Arg::ParseTupleAndKeywords(
      args, kw_args, 
      "etI|et", arg_names, "utf-8", &str, &length, "utf-8", &pad);
  PyMemGuard str_guard(str);
  PyMemGuard pad_guard(pad);
#endif

  if (pad == nullptr)
    pad = (char*) " ";
  if (strlen(pad) == 0)
    throw ValueError("empty pad");

  return Unicode::from(fixfmt::center(string(str), length, pad));
}


ref<Object> pad(Module* module, Tuple* args, Dict* kw_args)
{
  static char const* arg_names[] = {"string", "length", "pad", "pos", nullptr};
  char* str;
  int length;
  char* pad = nullptr;
  float pos = fixfmt::PAD_POS_LEFT_JUSTIFY;
#if PY3
  Arg::ParseTupleAndKeywords(
      args, kw_args, 
      "sI|sf", arg_names, &str, &length, &pad,
      &pos, nullptr);
#else
  Arg::ParseTupleAndKeywords(
      args, kw_args, 
      "etI|etf", arg_names, "utf-8", &str, &length, "utf-8", &pad,
      &pos, nullptr);
  PyMemGuard str_guard(str);
  PyMemGuard pad_guard(pad);
#endif

  if (pad == nullptr)
    pad = (char*) " ";
  if (strlen(pad) == 0)
    throw ValueError("empty pad");
  if (pos < 0 or pos > 1)
    throw ValueError("pos out of range");

  return Unicode::from(fixfmt::pad(string(str), length, pad, (float) pos));
}


ref<Object> elide(Module* module, Tuple* args, Dict* kw_args)
{
  static char const* arg_names[] = {
    "string", "length", "ellipsis", "pos", nullptr};
  char* str;
  int length;
  char* ellipsis = nullptr;
  float pos = 1;
#if PY3
  Arg::ParseTupleAndKeywords(
    args, kw_args, 
    "sI|sf", arg_names, &str, &length, &ellipsis, &pos);
#else
  Arg::ParseTupleAndKeywords(
    args, kw_args, 
    "etI|etf", arg_names, "utf-8", &str, &length, "utf-8", &ellipsis, &pos);
  PyMemGuard str_guard(str);
  PyMemGuard ellipsis_guard(ellipsis);
#endif

  if (ellipsis == nullptr)
    ellipsis = (char*) fixfmt::ELLIPSIS;

  string r = fixfmt::elide(string(str), length, string(ellipsis), pos);
  return Unicode::from(r);
}


ref<Object> palide(Module* module, Tuple* args, Dict* kw_args)
{
  static char const* arg_names[] = {
    "string", "length", "ellipsis", "pad", "elide_pos", 
    "pad_pos", nullptr };
  char* str;
  int length;
  char* ellipsis = nullptr;
  char* pad = nullptr;
  float elide_pos = 1;
  float pad_pos = fixfmt::PAD_POS_LEFT_JUSTIFY;
  Arg::ParseTupleAndKeywords(
    args, kw_args, "sI|ssff", arg_names,
    &str, &length, &ellipsis, &pad, &elide_pos, &pad_pos);

  if (ellipsis == nullptr)
    ellipsis = (char*) fixfmt::ELLIPSIS;
  if (pad == nullptr)
    pad = (char*) " ";
  if (strlen(pad) == 0)
    throw ValueError("empty pad");

  return Unicode::from(fixfmt::palide(
    string(str), length, string(ellipsis), pad, elide_pos, pad_pos));
}


ref<Object> string_length(Module* module, Tuple* args, Dict* kw_args)
{
  static char const* arg_names[] = { "string", nullptr };
  char* str;

  Arg::ParseTupleAndKeywords(args, kw_args, "s", arg_names, &str);

  return Long::FromLong(fixfmt::string_length(str));
}


}  // anonymous namespace

//------------------------------------------------------------------------------

Methods<Module>& add_functions(Methods<Module>& methods)
{
  methods
    .add<analyze_float<double>> ("analyze_double")
    .add<analyze_float<float>>  ("analyze_float")
    .add<center>                ("center")
    .add<elide>                 ("elide")
    .add<pad>                   ("pad")
    .add<palide>                ("palide")
    .add<string_length>         ("string_length")
    ;
  return methods;
}


