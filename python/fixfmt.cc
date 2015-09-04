#include <Python.h>

#include "Number.hh"
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
extern PyTypeObject* get_String();
extern PyTypeObject* get_Table();

PyMODINIT_FUNC
PyInit_fixfmt(void)
{
  auto module = Module::Create(&testmod_module);
  module->add(get_Bool());
  module->ready_and_add_type(&Number::type);
  module->add(get_String());
  module->add(get_Table());
  return module.release();
}


