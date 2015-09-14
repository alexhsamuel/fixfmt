#include <iostream>
#include <string>
#include <utility>

#include <Python.h>

#include "py.hh"

using namespace py;

//------------------------------------------------------------------------------

namespace {

using MethodPtr = ref<Long> (*)(PyObject*, PyObject*);  // FIXME

template<MethodPtr PTR>
class FunctionDef
{
public:

  static constexpr PyMethodDef END = {nullptr, nullptr, 0, nullptr};

  FunctionDef(std::string name) : name_(std::move(name)) {}

  operator PyMethodDef()
    { return {name_.c_str(), method, METH_VARARGS, nullptr}; }

private:

  static PyObject* method(PyObject* self, PyObject* args)
    { return PTR(self, args).release(); }

  std::string name_;

};


ref<Object> testfunc(Module* self, Tuple* args, Dict* kw_args)
{
  Object* arg;
  static char const* arg_names[] = {"obj", nullptr};
  Arg::ParseTupleAndKeywords(args, kw_args, "O", (char**) arg_names, &arg);
  auto name = arg->Str();
  std::cout << "Hello, " << name << "!\n";
  return Long::FromLong(name->Length());
}


auto methods = Methods<Module>()
  .add<testfunc>("testfunc");


static struct PyModuleDef testmod_module = {
  PyModuleDef_HEAD_INIT,
  "fixfmt.testmod",
  nullptr,
  -1,
  methods
};


}  // anonymous namespace

PyMODINIT_FUNC
PyInit_testmod(void)
{
  PyObject* module;
  module = PyModule_Create(&testmod_module);
  if (module == nullptr)
    return nullptr;

  // Other initialization here.
  return module;
}


