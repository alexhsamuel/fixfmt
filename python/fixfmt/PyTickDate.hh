#pragma once

#include <Python.h>

#include "fixfmt.hh"
#include "py.hh"

//------------------------------------------------------------------------------

class PyTickDate
  : public py::ExtensionType
{
public:

  /**
   * The wrapped formatter type.
   */
  using Formatter = fixfmt::TickDate;

  static py::Type type_;

  PyTickDate(std::unique_ptr<Formatter> fmt)
    : fmt_(std::move(fmt))
  {
  }

  std::unique_ptr<Formatter> const fmt_;

};

