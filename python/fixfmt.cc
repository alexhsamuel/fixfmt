#include <Python.h>

#include "py.hh"

using namespace py;

//------------------------------------------------------------------------------

namespace {

static PyMethodDef methods[] = {
  METHODDEF_END
};


PyModuleDef testmod_module = {
  PyModuleDef_HEAD_INIT,
  "fixfmt",
  nullptr,
  -1,
  methods
};


}  // anonymous namespace

//------------------------------------------------------------------------------

extern PyTypeObject* get_Bool();
extern PyTypeObject* get_Number();
extern PyTypeObject* get_String();

PyMODINIT_FUNC
PyInit_fixfmt(void)
{
  auto module = Module::Create(&testmod_module);
  module->add(get_Bool());
  module->add(get_Number());
  module->add(get_String());
  return module.release();
}


