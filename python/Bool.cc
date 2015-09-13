#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include <Python.h>

#include "Bool.hh"
#include "fixfmt.hh"
#include "py.hh"

using namespace py;
using std::string;
using std::unique_ptr;

//------------------------------------------------------------------------------

namespace {

int tp_init(Bool* self, PyObject* args, PyObject* kw_args)
{
  static char const* arg_names[] = {"true", "false", "size", "pad_left", nullptr};
  char const*    true_str    = "true";
  char const*    false_str   = "false";
  int            size        = -1;
  bool           pad_left    = false;
  if (!PyArg_ParseTupleAndKeywords(
      args, kw_args, "|ssib", (char**) arg_names,
      &true_str, &false_str, &size, &pad_left)) 
    return -1;

  if (size < 0)
    size = std::max(strlen(true_str), strlen(false_str));
  self->fmt_ = unique_ptr<fixfmt::Bool>(
      new fixfmt::Bool(string(true_str), string(false_str), size, pad_left));
  return 0;
}


PyObject* tp_call(Bool* self, PyObject* args, PyObject* kw_args)
{
  static char const* arg_names[] = {"value", nullptr};
  bool val;
  if (!PyArg_ParseTupleAndKeywords(
      args, kw_args, "b", (char**) arg_names, &val)) 
    return nullptr;

  fixfmt::Bool& fmt = *self->fmt_;
  // FIXME
  string const& str = fmt(val);
  return Unicode::FromStringAndSize((char*) str.c_str(), str.length()).release();
}


PyMethodDef const tp_methods[] = {
  METHODDEF_END
};


}  // anonymous namespace


Type Bool::type_ = PyTypeObject{
  PyVarObject_HEAD_INIT(nullptr, 0)
  (char const*)         "fixfmt.Bool",                      // tp_name
  (Py_ssize_t)          sizeof(Bool),                       // tp_basicsize
  (Py_ssize_t)          0,                                  // tp_itemsize
  (destructor)          nullptr,                            // tp_dealloc
  (printfunc)           nullptr,                            // tp_print
  (getattrfunc)         nullptr,                            // tp_getattr
  (setattrfunc)         nullptr,                            // tp_setattr
  (void*)               nullptr,                            // tp_reserved
  (reprfunc)            nullptr,                            // tp_repr
  (PyNumberMethods*)    nullptr,                            // tp_as_number
  (PySequenceMethods*)  nullptr,                            // tp_as_sequence
  (PyMappingMethods*)   nullptr,                            // tp_as_mapping
  (hashfunc)            nullptr,                            // tp_hash
  (ternaryfunc)         tp_call,                            // tp_call
  (reprfunc)            nullptr,                            // tp_str
  (getattrofunc)        nullptr,                            // tp_getattro
  (setattrofunc)        nullptr,                            // tp_setattro
  (PyBufferProcs*)      nullptr,                            // tp_as_buffer
  (unsigned long)       Py_TPFLAGS_DEFAULT
                        | Py_TPFLAGS_BASETYPE,              // tp_flags
  (char const*)         nullptr,                            // tp_doc
  (traverseproc)        nullptr,                            // tp_traverse
  (inquiry)             nullptr,                            // tp_clear
  (richcmpfunc)         nullptr,                            // tp_richcompare
  (Py_ssize_t)          0,                                  // tp_weaklistoffset
  (getiterfunc)         nullptr,                            // tp_iter
  (iternextfunc)        nullptr,                            // tp_iternext
  (PyMethodDef*)        tp_methods,                         // tp_methods
  (PyMemberDef*)        nullptr,                            // tp_members
  (PyGetSetDef*)        nullptr,                            // tp_getset
  (_typeobject*)        nullptr,                            // tp_base
  (PyObject*)           nullptr,                            // tp_dict
  (descrgetfunc)        nullptr,                            // tp_descr_get
  (descrsetfunc)        nullptr,                            // tp_descr_set
  (Py_ssize_t)          0,                                  // tp_dictoffset
  (initproc)            tp_init,                            // tp_init
  (allocfunc)           nullptr,                            // tp_alloc
  (newfunc)             PyType_GenericNew,                  // tp_new
  (freefunc)            nullptr,                            // tp_free
  (inquiry)             nullptr,                            // tp_is_gc
  (PyObject*)           nullptr,                            // tp_bases
  (PyObject*)           nullptr,                            // tp_mro
  (PyObject*)           nullptr,                            // tp_cache
  (PyObject*)           nullptr,                            // tp_subclasses,
  (PyObject*)           nullptr,                            // tp_weaklist
  (destructor)          nullptr,                            // tp_del
  (unsigned int)        0,                                  // tp_version_tag
  (destructor)          nullptr,                            // tp_finalize
};


