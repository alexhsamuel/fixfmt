#pragma once

#include <memory>

#include <Python.h>

#include "fixfmt.hh"
#include "py.hh"

//------------------------------------------------------------------------------

class Table
  : public py::ExtensionType
{
public:

  static py::Type type_;

  std::unique_ptr<fixfmt::Table> table_;

};

