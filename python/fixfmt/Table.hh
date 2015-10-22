#pragma once

#include <memory>

#include <Python.h>

#include "fixfmt.hh"
#include "py.hh"

//------------------------------------------------------------------------------

class BufferRef
{
public:

  BufferRef(Py_buffer&& buffer) 
    : buffer_(buffer) 
  {
    std::cerr << "BufferRef " << buffer_.buf << "\n";
  }

  ~BufferRef() 
  { 
    std::cerr << "~BufferRef " << buffer_.buf << "\n";
    PyBuffer_Release(&buffer_); 
  }

private:

  Py_buffer buffer_;

};


class Table
  : public py::ExtensionType
{
public:

  static py::Type type_;

  std::unique_ptr<fixfmt::Table> table_;
  std::vector<BufferRef> buffers_;

};

