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

  Table() {}

  ~Table() {
    for (auto buf : buffers_) {
      std::cerr << "release " << buf.buf << "\n";
      PyBuffer_Release(&buf);
    }
  }

  void hold_buffer(Py_buffer&& buf) { 
    std::cerr << "hold " << buf.buf << "\n";
    buffers_.push_back(buf); 
  }

  std::unique_ptr<fixfmt::Table> table_;

private:

  std::vector<Py_buffer> buffers_;

};

