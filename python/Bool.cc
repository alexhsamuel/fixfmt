#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include <Python.h>

#include "base.hh"
#include "bool.hh"
#include "py.hh"

using namespace py;

//------------------------------------------------------------------------------

namespace {

class Bool
  : public ExtensionType
{
  std::unique_ptr<fixfmt::Bool> fmt_;

  static int tp_init(Bool* self, PyObject* args, PyObject* kw_args)
  {
    static char const* arg_names[] = {"true", "false", "size", "pad_left", nullptr};

    char const*    true_str    = "true";
    char const*    false_str   = "false";
    int            size        = -1;
    bool           pad_left    = false;
    if (PyArg_ParseTupleAndKeywords(
        args, kw_args, "|ssib", (char**) arg_names,
        &true_str, &false_str, &size, &pad_left)) {
      if (size < 0)
        size = std::max(strlen(true_str), strlen(false_str));
      self->fmt_ = std::unique_ptr<fixfmt::Bool>(
          new fixfmt::Bool(
              std::string(true_str), std::string(false_str), size, pad_left));
      return 0;
    }
    else
      return -1;
  }

  static PyObject* tp_call(Bool* self, PyObject* args, PyObject* kw_args)
  {
    static char const* arg_names[] = {"value", nullptr};
    bool val;
    if (PyArg_ParseTupleAndKeywords(
        args, kw_args, "b", (char**) arg_names, &val)) {
      fixfmt::Bool& fmt = *self->fmt_;
      // FIXME
      std::string const& str = fmt(val);
      return Unicode::FromStringAndSize((char*) str.c_str(), str.length()).release();
    }
    else
      return nullptr;
  }

  static PyMethodDef const tp_methods[];

public:

  static PyTypeObject type;

};

PyMethodDef const Bool::tp_methods[] = {
  METHODDEF_END
};


PyTypeObject Bool::type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  "fixfmt.Bool",               // tp_name
  sizeof(Bool),                // tp_basicsize
  0,                           // tp_itemsize
  nullptr,                     // tp_dealloc
  nullptr,                     // tp_print
  nullptr,                     // tp_getattr
  nullptr,                     // tp_setattr
  nullptr,                     // tp_reserved
  nullptr,                     // tp_repr
  nullptr,                     // tp_as_number
  nullptr,                     // tp_as_sequence
  nullptr,                     // tp_as_mapping
  nullptr,                     // tp_hash
  (ternaryfunc) tp_call,       // tp_call
  nullptr,                     // tp_str
  nullptr,                     // tp_getattro
  nullptr,                     // tp_setattro
  nullptr,                     // tp_as_buffer
  Py_TPFLAGS_DEFAULT
  | Py_TPFLAGS_BASETYPE,       // tp_flags
  nullptr,                     // tp_doc
  nullptr,                     // tp_traverse
  nullptr,                     // tp_clear
  nullptr,                     // tp_richcompare
  0,                           // tp_weaklistoffset
  nullptr,                     // tp_iter
  nullptr,                     // tp_iternext
  (PyMethodDef*) tp_methods,   // tp_methods
  nullptr,                     // tp_members
  nullptr,                     // tp_getset
  nullptr,                     // tp_base
  nullptr,                     // tp_dict
  nullptr,                     // tp_descr_get
  nullptr,                     // tp_descr_set
  0,                           // tp_dictoffset
  (initproc) tp_init,          // tp_init
  nullptr,                     // tp_alloc
  PyType_GenericNew,           // tp_new
};


}  // anonymous namespace


PyTypeObject* get_Bool()
{
  int result = PyType_Ready(&Bool::type);
  assert(result == 0); unused(result);
  return &Bool::type;
}


