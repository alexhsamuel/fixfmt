#include "double-conversion/double-conversion.h"
#include "number.hh"

//------------------------------------------------------------------------------

namespace fixfmt {

using std::string;

string Number::operator()(long val) const
{
  if (val < 0 && sign_ == SIGN_NONE)
    return bad_result_;

  // Format directly into the string's buffer.
  string result(width_, pad_);
  char* buf = &result[0];

  int const sign_len = sign_ == SIGN_NONE ? 0 : 1;
  bool const nonneg = val >= 0;
  val = std::abs(val);

  int i = size_;
  if (val == 0 && size_ > 0)
    // For exact zero, render a single zero.
    buf[sign_len + --i] = '0';
  else {
    // Render digits.
    for (; i > 0 && val > 0; val /= 10)
      buf[sign_len + --i] = '0' + val % 10;
    // We should have rendered the entire value; otherwise we've overflowed.
    if (val != 0)
      return bad_result_;
  }

  // Render the sign.
  if (sign_ != SIGN_NONE)
    *(buf + (pad_ == PAD_ZERO ? 0 : i)) = get_sign_char(nonneg);
      
  if (precision_ != PRECISION_NONE) {
    // Add the decimal point.
    char* point = buf + sign_len + size_;
    *point++ = point_;
    if (precision_ > 0) 
      // Format the fractional part.
      set(point, '0', precision_);
  }

  assert(result.length() == width_);
  return std::move(result);
}


string Number::operator()(double const val) const
{
  if (isnan(val))
    return nan_result_;
  else if (val < 0 && sign_ == SIGN_NONE)
    // With SIGN_NONE, we can't render negative numbers.
    return bad_result_;
  else if (isinf(val))
    // Return the appropriate infinity.
    return val >= 0 ? pos_inf_result_ : neg_inf_result_;

  else {
    int const precision = precision_ == PRECISION_NONE ? 0 : precision_;

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
    // FIXME: Why are trailing zeros being suppressed?  Can we change this, as we
    // will just add them later?
    // assert(length - decimal_pos == precision);
    assert(length - decimal_pos <= precision);

    string result;

    if (decimal_pos > size_)
      // Integral part too large.
      return bad_result_;

    // The number of digits in the integral part.
    //
    // If size_ is positive but there are no integral digits at all, show a
    // zero regardless; this is conventional.
    int const int_digits = 
      decimal_pos > 0 ? decimal_pos 
      : size_ > 0 ? 1
      : 0;

    // Add pad and sign.  Space padding precedes sign, while zero padding
    // follows it.  
    if (pad_ == PAD_SPACE && size_ > int_digits)
      // Space padding. 
      result.append(size_ - int_digits, ' ');
    if (sign_ != SIGN_NONE)
      // The sign character.
      result.push_back(get_sign_char(val >= 0));
    if (pad_ == PAD_ZERO && size_ > int_digits)
      // Zero padding.
      result.append(size_ - int_digits, '0');

    // Add digits for the integral part.
    if (decimal_pos > 0)
      result.append(buf, decimal_pos);
    else if (size_ > 0)
      // Show at least one zero.
      result.push_back('0');

    if (precision_ != PRECISION_NONE) {
      // Add the decimal point.
      result.push_back('.');
      
      // Pad with zeros after the decimal point if needed.
      if (decimal_pos < 0)
        result.append(-decimal_pos, '0');
      // Add fractional digits.
      if (length - decimal_pos > 0)
        result.append(&buf[std::max(decimal_pos, 0)]);
      // Pad with zeros at the end, if necessary.
      if (length - decimal_pos < precision_)
        result.append(precision_ - (length - decimal_pos), '0');
    }
 
    assert(result.length() == width_);
    return result;
  }
}


}  // namespace fixfmt

