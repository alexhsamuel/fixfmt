#pragma once

#include <cassert>
#include <string>

#include "fixfmt/math.hh"

//------------------------------------------------------------------------------

namespace fixfmt {

using std::string;

constexpr char const* ELLIPSIS = "\u2026";

inline string
pad(
  string const& str,
  size_t const length,
  char const pad=' ',
  bool const left=false)
{
  if (str.length() < length) {
    string const padding(length - str.length(), pad);
    string const result = left ? padding + str : str + padding;
    assert(result.length() == length);
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
  assert(max_length >= ellipsis.length());
  assert(0 <= position);
  assert(position <= 1);

  size_t const length = str.length();
  if (length <= max_length)
    return str;
  else {
    size_t const keep   = max_length - ellipsis.length();
    size_t const nleft  = (size_t) round(position * keep);
    size_t const nright = keep - nleft;
    string elided;
    if (nleft > 0)
      elided += str.substr(0, nleft);
    elided += ellipsis;
    if (nright > 0)
      elided += str.substr(length - nright);
    assert(elided.length() == max_length);
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
