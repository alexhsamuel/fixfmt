#include <Python.h>

#include "PyBool.hh"
#include "PyNumber.hh"
#include "PyString.hh"
#include "PyTable.hh"
#include "PyTickTime.hh"
#include "py.hh"

using namespace py;

//------------------------------------------------------------------------------

extern Methods<Module>& add_functions(Methods<Module>&);

namespace {

Methods<Module> methods;

#if PY3K

PyModuleDef testmod_module = {
  PyModuleDef_HEAD_INIT,
  "fixfmt._ext",
  nullptr,
  -1,
  add_functions(methods)
};

#endif

}  // anonymous namespace

//------------------------------------------------------------------------------

PyMODINIT_FUNC
#if PY3K
PyInit__ext(void)
#else
init_ext(void)
#endif
{
#if PY3K
  auto module = Module::Create(&testmod_module);
#else
  auto module = Module::Init("fixfmt._ext", add_functions(methods));
#endif

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

#if PY3K
    return module.release();
#else
    module.release();
#endif
  }
  catch (Exception) {
#if PY3K
    return nullptr;
#endif
  }
}


