#include <Python.h>

#include "PyBool.hh"
#include "PyNumber.hh"
#include "PyString.hh"
#include "PyTable.hh"
#include "PyTickTime.hh"
#include "PyTickDate.hh"
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

  try {
    PyBool::type_.Ready();
    module->add(&PyBool::type_);

    PyNumber::type_.Ready();
    {
      // Add the 'SCALES' class attribute; this must be done from extension 
      // code.
      Dict* const dict = (Dict*) PyNumber::type_.tp_dict;
      assert(dict != nullptr);
      dict->SetItemString("SCALES", PyDict_New());
    }
    module->add(&PyNumber::type_);

    PyString::type_.Ready();
    module->add(&PyString::type_);

    PyTable::type_.Ready();
    module->add(&PyTable::type_);

    PyTickTime::type_.Ready();
    module->add(&PyTickTime::type_);

    PyTickDate::type_.Ready();
    module->add(&PyTickDate::type_);

    return module.release();
  }
  catch (Exception) {
    return nullptr;
  }
}


