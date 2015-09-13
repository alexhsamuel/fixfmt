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


static auto
testfunc(PyObject* self, PyObject* args)
{
  Object* arg;
  PyArg_ParseTuple(args, "O", &arg);
  // FIXME: Something like this:
  //   parser(args).add(arg).parse();
  auto name = arg->Str();
  std::cout << "Hello, " << name << "!\n";
  return Long::FromLong(name->Length());
}


static PyMethodDef methods[] = {
  FunctionDef<testfunc>("testfunc"),
  FunctionDef<nullptr>::END
};


static struct PyModuleDef testmod_module = {
  PyModuleDef_HEAD_INIT,
  "testmod",
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


