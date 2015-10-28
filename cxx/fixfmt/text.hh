#pragma once

#include <cassert>
#include <iostream>
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
next_utf8(string::const_iterator& i)
{
  unsigned char c = *i++;
  if ((c & 0xc0) == 0xc0) {
    // It's multibyte.  The number of bytes is the number of MSB's before 
    // the first zero.
    // FIXME: Improve this.
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
  // FIXME: Problem if the last code point is malformed.
  for (auto i = str.begin(); i != str.end(); next_utf8(i))
    ++length;
  return length;
}


constexpr char ANSI_ESCAPE = '\x1b';

inline bool
within(unsigned char min, unsigned char val, unsigned char max)
{
  return min <= val && val <= max;
}


/**
 * Advances an iterator past an ANSI escape sequence, if it is positioned at
 * one.
 */
inline bool
skip_ansi_escape(string::const_iterator& i, string::const_iterator const& end)
{
  assert(i != end);
  if (*i == ANSI_ESCAPE) {
    ++i;
    if (i != end && *i++ == '[') 
      // Got CSI.  Read until we pass a final byte.
      while (i != end && !within(64, *i++, 126))
        ;
    else
      // Assume single-character escape.
      ;
    return true;
  }
  else
    return false;
}


/**
 * Returns the number of code points in a UTF-8-encoded string, skipping
 * escape sequences.
 */
inline size_t
string_length(string const& str)
{
  size_t length = 0;
  auto const& end = str.end();
  // FIXME: Problem if the last code point is malformed.

  auto i = str.begin();
  // Count characters.
  while (i != end) {
    if (skip_ansi_escape(i, end))
      ;
    else {
      ++length;
      next_utf8(i);
    }
  }
  return length;
}


/**
 * Concatenates copies of `str` up to `length`.  If `length` is not divisible
 * by the length of `str`, the last copy is partial.
 */
inline string
fill(
  string const& str,
  size_t const length)
{
  size_t const str_len = string_length(str);
  assert(str_len > 0);
  if (str.length() == 1)
    return string(length, str[0]);
  else {
    string result;
    result.reserve(length);
    // Concatenate whole copies.
    size_t l = length;
    while (l >= str_len) {
      result += str;
      l -= str_len;
    }
    if (l > 0) {
      // Concatenate a partial copy.
      auto i = str.begin();
      for (; l > 0; --l)
        next_utf8(i);
      result.append(str.begin(), i);
    }
    assert(string_length(result) == length);
    return result;
  }
}


inline string
pad(
  string const& str,
  size_t const length,
  string const& pad=" ",
  bool const left=false)
{
  size_t const str_len = string_length(str);
  if (str_len < length) {
    string const padding = fill(pad, length - str_len);
    string const result = left ? padding + str : str + padding;
    assert(string_length(result) == length);
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
  size_t const ellipsis_len = string_length(ellipsis);
  assert(max_length >= ellipsis_len);
  assert(0 <= position);
  assert(position <= 1);

  size_t const length = string_length(str);
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
    assert(string_length(elided) == max_length);
    return elided;
  }
}


inline string
palide(
  string const& str,
  size_t const length,
  string const& ellipsis=ELLIPSIS,
  string const& pad=" ",
  float const position=1.0,
  bool const left=false)
{
  return fixfmt::pad(elide(str, length, ellipsis, position), length, pad, left);
}


}  // namespace fixfmt
