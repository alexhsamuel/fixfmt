#pragma once

#include <memory>

#include <Python.h>

#include "fixfmt.hh"
#include "py.hh"

//------------------------------------------------------------------------------

class PyNumber
  : public py::ExtensionType
{
public:

  /**
   * The wrapped formatter type.
   */
  using Formatter = fixfmt::Number;

  static py::Type type_;

  std::unique_ptr<Formatter> fmt_;

};



