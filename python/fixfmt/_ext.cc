#include <Python.h>

#include "PyBool.hh"
#include "PyNumber.hh"
#include "PyString.hh"
#include "PyTable.hh"
#include "py.hh"

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

  PyNumber::type_.Ready();
  module->add(&PyNumber::type_);

  PyString::type_.Ready();
  module->add(&PyString::type_);

  PyTable::type_.Ready();
  module->add(&PyTable::type_);

  return module.release();
}


