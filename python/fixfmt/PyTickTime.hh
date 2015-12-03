#pragma once

#include <Python.h>

#include "fixfmt.hh"
#include "py.hh"

//------------------------------------------------------------------------------

class PyTickTime
  : public py::ExtensionType
{
public:

  /**
   * The wrapped formatter type.
   */
  using Formatter = fixfmt::TickTime;

  static py::Type type_;

  PyTickTime(std::unique_ptr<Formatter> fmt)
    : fmt_(std::move(fmt))
  {
  }

  std::unique_ptr<Formatter> const fmt_;

};

