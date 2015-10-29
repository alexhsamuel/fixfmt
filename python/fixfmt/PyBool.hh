#include <memory>

#include <Python.h>

#include "fixfmt.hh"
#include "py.hh"

//------------------------------------------------------------------------------

class PyBool
  : public py::ExtensionType
{
public:

  /**
   * The wrapped formatter type.
   */
  using Formatter = fixfmt::Bool;

  static py::Type type_;

  std::unique_ptr<Formatter> fmt_;

};

