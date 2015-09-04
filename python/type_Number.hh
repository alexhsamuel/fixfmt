#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include <Python.h>

#include "base.hh"
#include "number.hh"
#include "py.hh"

using namespace py;
using std::unique_ptr;

//------------------------------------------------------------------------------

class Number
  : public ExtensionType
{
public:

  static PyTypeObject type;

  unique_ptr<fixfmt::Number> fmt_;

};

