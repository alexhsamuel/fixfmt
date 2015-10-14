#pragma once

#include <cassert>
#include <string>

#include "fixfmt/math.hh"

//------------------------------------------------------------------------------

namespace fixfmt {

using std::string;

constexpr char const* ELLIPSIS = "\u2026";

/**
 * Advances an iterator on a UTF-8 string by one code point.
 */
inline void
next_utf8(string::const_iterator& i, string::const_iterator end)
{
  unsigned char c = *i++;
  if ((c & 0xc0) == 0xc0) {
    // It's multibyte.  The number of bytes is the number of MSB's before 
    // the first zero.
    ++i;
    if ((c & 0xe0) == 0xe0) {
      ++i;
      if ((c & 0xf0) == 0xf0) {
        ++i;
        if ((c & 0xf8) == 0xf8) {
          ++i;
          if ((c & 0xfc) == 0xfc)
            ++i;
        }
      }
    }
  }
}


/**
 * Returns the number of code points in a UTF-8-encoded string.
 */
inline size_t
utf8_length(string const& str)
{
  size_t length = 0;
  for (auto i = str.begin(); i != str.end(); next_utf8(i, str.end()))
    ++length;
  return length;
}


inline string
pad(
  string const& str,
  size_t const length,
  char const pad=' ',
  bool const left=false)
{
  size_t const str_len = utf8_length(str);
  if (str_len < length) {
    string const padding(length - str_len, pad);
    string const result = left ? padding + str : str + padding;
    assert(utf8_length(result) == length);
    return result;
  }
  else
    return str;
}


inline string
elide(
  string const& str,
  size_t const max_length,
  string const& ellipsis=ELLIPSIS,
  float const position=1.0)
{
  size_t const ellipsis_len = utf8_length(ellipsis);
  assert(max_length >= ellipsis_len);
  assert(0 <= position);
  assert(position <= 1);

  size_t const length = utf8_length(str);
  if (length <= max_length)
    return str;
  else {
    size_t const keep   = max_length - ellipsis_len;
    size_t const nleft  = (size_t) round(position * keep);
    size_t const nright = keep - nleft;
    string elided;
    if (nleft > 0)
      elided += str.substr(0, nleft);
    elided += ellipsis;
    if (nright > 0)
      elided += str.substr(length - nright);
    assert(utf8_length(elided) == max_length);
    return elided;
  }
}


inline string
palide(
  string const& str,
  size_t const length,
  string const& ellipsis=ELLIPSIS,
  char const pad=' ',
  float const position=1.0,
  bool const left=false)
{
  return fixfmt::pad(elide(str, length, ellipsis, position), length, pad, left);
}


}  // namespace fixfmt
