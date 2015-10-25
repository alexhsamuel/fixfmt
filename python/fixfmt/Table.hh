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

  // Holds references to the buffers referenced by the table's columns.
  std::vector<py::BufferRef> buffers_;

};


