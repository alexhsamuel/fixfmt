#pragma once

#include <cassert>
#include <iostream>
#include <string>

#include "fixfmt/math.hh"

//------------------------------------------------------------------------------

namespace {

inline bool
within(unsigned char min, unsigned char val, unsigned char max)
{
  return min <= val && val <= max;
}


}  // anonymous namespace

//------------------------------------------------------------------------------

namespace fixfmt {

using std::string;

constexpr char const* ELLIPSIS = "\u2026";

constexpr char ANSI_ESCAPE = '\x1b';

/**
 * Advances an iterator on a UTF-8 string by one code point.
 */
// FIXME: Take an end parameter.
inline bool
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
  return true;
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
  // Count characters.
  for (auto i = str.begin(); i != end; ) 
    if (skip_ansi_escape(i, end))
      ;
    else {
      ++length;
      next_utf8(i);
    }
  return length;
}


/**
 * Truncates a string to `length` code points, skipping escape sequences.
 */
inline void
string_truncate(string& str, size_t length)
{
  auto const& begin = str.cbegin();
  auto const& end = str.cend();
  for (auto i = begin; i != end; ! skip_ansi_escape(i, end) && next_utf8(i))
    if (i - begin == (int) length) {
      str.resize(i - begin);
      break;
    }
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


// FIXME: Do we need both pad() and center()?

/**
 * Left- or right-justifies a string to fixed length by padding.
 */
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


/**
 * Centers a string in to fixed length by padding both sides.
 */
inline string
center(
  string const& str,
  size_t const length,
  string const& pad=" ",
  float position=0.5)
{
  assert(string_length(pad) > 0);
  assert(0 <= position);
  assert(position <= 1);
  size_t const str_len = string_length(str);
  if (str_len < length) {
    size_t const pad_len = length - str_len;
    size_t const left_len = (size_t) round(position * pad_len);
    string const result = 
      fill(pad, left_len) + str + fill(pad, pad_len - left_len);
    assert(string_length(result) == length);
    return result;
  }
  else
    return str;
}


/**
 * Trims a string to a fixed length by eliding characters and replacing them
 * with an ellipsis.
 */
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


/**
 * Either pads or elides a string to achieve a fixed length.
 */
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
