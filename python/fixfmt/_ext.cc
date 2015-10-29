#include <Python.h>

#include "PyBool.hh"
#include "Number.hh"
#include "py.hh"
#include "String.hh"
#include "Table.hh"

using namespace py;

//------------------------------------------------------------------------------

extern Methods<Module>& add_functions(Methods<Module>&);

namespace {

Methods<Module> methods;

PyModuleDef testmod_module = {
  PyModuleDef_HEAD_INIT,
  "fixfmt._ext",
  nullptr,
  -1,
  add_functions(methods)
};


}  // anonymous namespace

//------------------------------------------------------------------------------

PyMODINIT_FUNC
PyInit__ext(void)
{
  auto module = Module::Create(&testmod_module);

  PyBool::type_.Ready();
  module->add(&PyBool::type_);

  Number::type_.Ready();
  module->add(&Number::type_);

  String::type_.Ready();
  module->add(&String::type_);

  Table::type_.Ready();
  module->add(&Table::type_);

  return module.release();
}


