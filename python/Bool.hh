#include <memory>

#include <Python.h>

#include "fixfmt.hh"
#include "py.hh"

//------------------------------------------------------------------------------

class Bool
  : public py::ExtensionType
{
public:

  static py::Type type_;

  std::unique_ptr<fixfmt::Bool> fmt_;

};

