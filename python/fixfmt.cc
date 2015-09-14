#include <Python.h>

#include "Bool.hh"
#include "Number.hh"
#include "py.hh"
#include "String.hh"
#include "Table.hh"

using namespace py;

//------------------------------------------------------------------------------

namespace {

auto methods = Methods<Module>()
;


PyModuleDef testmod_module = {
  PyModuleDef_HEAD_INIT,
  "fixfmt",
  nullptr,
  -1,
  methods
};


}  // anonymous namespace

//------------------------------------------------------------------------------

PyMODINIT_FUNC
PyInit_fixfmt(void)
{
  auto module = Module::Create(&testmod_module);

  Bool::type_.Ready();
  module->add(&Bool::type_);

  Number::type_.Ready();
  module->add(&Number::type_);

  String::type_.Ready();
  module->add(&String::type_);

  Table::type_.Ready();
  module->add(&Table::type_);

  return module.release();
}


