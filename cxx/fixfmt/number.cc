#include "double-conversion/double-conversion.h"
#include "number.hh"

namespace fixfmt {

using std::string;

//------------------------------------------------------------------------------

Number::Scale const Number::SCALE_NONE          = {};

Number::Scale const Number::SCALE_PERCENT       = {1E-2 , "%"};
Number::Scale const Number::SCALE_PER_MILLE     = {1E-3 , "\u2030"};
Number::Scale const Number::SCALE_BASIS_POINTS  = {1E-4 , " bps"};

Number::Scale const Number::SCALE_TERA          = {1E+12, "T"};
Number::Scale const Number::SCALE_GIGA          = {1E+9 , "G"};
Number::Scale const Number::SCALE_MEGA          = {1E+6 , "M"};
Number::Scale const Number::SCALE_KILO          = {1E+3 , "k"};
Number::Scale const Number::SCALE_DECI          = {1E-1 , "d"};
Number::Scale const Number::SCALE_CENTI         = {1E-2 , "c"};
Number::Scale const Number::SCALE_MILLI         = {1E-3 , "m"};
Number::Scale const Number::SCALE_MICRO         = {1E-6 , "\u00b5"};
Number::Scale const Number::SCALE_NANO          = {1E-9 , "n"};
Number::Scale const Number::SCALE_PICO          = {1E-12, "p"};
Number::Scale const Number::SCALE_FEMTO         = {1E-15, "f"};

Number::Scale const Number::SCALE_GIBI          = {1 << 30, "Gi"};
Number::Scale const Number::SCALE_MEBI          = {1 << 20, "Mi"};
Number::Scale const Number::SCALE_KIBI          = {1 << 10, "Ki"};

//------------------------------------------------------------------------------

string 
Number::operator()(
  long val) 
  const
{
  // Always use the FP code path if there's a scale.
  if (args_.scale.enabled())
    return (*this)((double) val);

  if (val < 0 && args_.sign == SIGN_NONE)
    return bad_;

  // Format directly into the string's buffer.
  string result(alloc_size_, args_.pad);
  char* const buf = &result[0];

  int const sign_len = args_.sign == SIGN_NONE ? 0 : 1;
  bool const nonneg = val >= 0;
  val = std::abs(val);

  int i = args_.size;
  if (val == 0 && args_.size > 0)
    // For exact zero, render a single zero.
    buf[sign_len + --i] = '0';
  else {
    // Render digits.
    for (; i > 0 && val > 0; val /= 10)
      buf[sign_len + --i] = '0' + val % 10;
    // We should have rendered the entire value; otherwise we've overflowed.
    if (val != 0)
      return bad_;
  }

  // Render the sign.
  if (args_.sign != SIGN_NONE)
    *(buf + (args_.pad == PAD_ZERO ? 0 : i)) = get_sign_char(nonneg);
      
  if (args_.precision != PRECISION_NONE) {
    // Add the decimal point.
    char* point = buf + sign_len + args_.size;
    *point++ = args_.point;
    if (args_.precision > 0) 
      // Format the fractional part.
      memset(point, '0', args_.precision);
  }

  assert(string_length(result) == width_);
  return result;
}


string 
Number::operator()(
  double const value) 
  const
{
  if (std::isnan(value))
    return nan_;
  else if (value < 0 && args_.sign == SIGN_NONE)
    // With SIGN_NONE, we can't render negative numbers.
    return bad_;

  // Apply the scale factor, if any.
  double const val = args_.scale.enabled() ? value / args_.scale.factor : value;

  if (std::isinf(val))
    // Return the appropriate infinity.
    return val >= 0 ? pos_inf_ : neg_inf_;

  else {
    int const precision 
      = args_.precision == PRECISION_NONE ? 0 : args_.precision;

    // FIXME: Assumes ASCII only.
    char buf[384];  // Enough room for DBL_MAX.
    bool sign;
    int length;
    int decimal_pos;
    double_conversion::DoubleToStringConverter::DoubleToAscii(
      std::abs(val), 
      double_conversion::DoubleToStringConverter::FIXED,
      precision,
      buf, sizeof(buf),
      &sign, &length, &decimal_pos);
    // FIXME: Why are trailing zeros being suppressed?  Can we change this, as
    // we will just add them later?
    // assert(length - decimal_pos == precision);
    assert(length - decimal_pos <= precision);

    string result;

    if (decimal_pos > args_.size)
      // Integral part too large.
      return bad_;

    // The number of digits in the integral part.
    //
    // If args_.size is positive but there are no integral digits at all, show a
    // zero regardless; this is conventional.
    int const int_digits = 
      decimal_pos > 0 ? decimal_pos 
      : args_.size > 0 ? 1
      : 0;

    // Add pad and sign.  Space padding precedes sign, while zero padding
    // follows it.  
    if (args_.pad == PAD_SPACE && args_.size > int_digits)
      // Space padding. 
      result.append(args_.size - int_digits, ' ');
    if (args_.sign != SIGN_NONE)
      // The sign character.
      result.push_back(get_sign_char(val >= 0));
    if (args_.pad == PAD_ZERO && args_.size > int_digits)
      // Zero padding.
      result.append(args_.size - int_digits, '0');

    // Add digits for the integral part.
    if (decimal_pos > length) {
      // The integral part needs to be zero-padded.
      result.append(buf, length);
      result.append(decimal_pos - length, '0');
      length = decimal_pos;
    }
    else if (decimal_pos > 0)
      result.append(buf, decimal_pos);
    else if (args_.size > 0)
      // Show at least one zero.
      result.push_back('0');

    if (args_.precision != PRECISION_NONE) {
      // Add the decimal point.
      result.push_back(args_.point);
      
      // Pad with zeros after the decimal point if needed.
      if (decimal_pos < 0)
        result.append(-decimal_pos, '0');
      // Add fractional digits.
      if (length - decimal_pos > 0)
        result.append(&buf[std::max(decimal_pos, 0)]);
      // Pad with zeros at the end, if necessary.
      if (length - decimal_pos < args_.precision)
        result.append(args_.precision - (length - decimal_pos), '0');
    }
 
    if (args_.scale.enabled()) 
      // Tack on the scale suffix.
      result.append(args_.scale.suffix);

    assert(string_length(result) == width_);
    return result;
  }
}


//------------------------------------------------------------------------------

}  // namespace fixfmt

