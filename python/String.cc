#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include <Python.h>

#include "string.hh"
#include "py.hh"
#include "text.hh"
#include "base.hh"

using namespace py;

//------------------------------------------------------------------------------

namespace {

class String
  : public ExtensionType
{
  std::unique_ptr<fixfmt::String> fmt_;

  static int tp_init(String* self, PyObject* args, PyObject* kw_args)
  {
    static char const* arg_names[] 
        = {"size", "ellipsis", "pad", "position", "pad_left", nullptr};

    int         size;
    char const* ellipsis = "...";
    char        pad = ' ';
    double      position = 1.0;
    bool        pad_left = false;
    if (PyArg_ParseTupleAndKeywords(
        args, kw_args, "i|sCdb", (char**) arg_names,
        &size, &ellipsis, &pad, &position, &pad_left)) {
      // FIXME: Validate args.
      self->fmt_ = std::unique_ptr<fixfmt::String>(
          new fixfmt::String(size, ellipsis, pad, position, pad_left));
      return 0;
    }
    else
      return -1;
  }

  static PyObject* tp_call(String* self, PyObject* args, PyObject* kw_args)
  {
    static char const* arg_names[] = {"str", nullptr};
    char* val;
    if (PyArg_ParseTupleAndKeywords(
        args, kw_args, "s", (char**) arg_names, &val)) 
      return Unicode::from((*self->fmt_)(val)).release();
    else
      return nullptr;
  }

  static PyMethodDef const tp_methods[];

public:

  static PyTypeObject type;

};

PyMethodDef const String::tp_methods[] = {
  METHODDEF_END
};


PyTypeObject String::type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  "fixfmt.String",             // tp_name
  sizeof(String),              // tp_basicsize
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


PyTypeObject* get_String()
{
  int result = PyType_Ready(&String::type);
  assert(result == 0); unused(result);
  return &String::type;
}


