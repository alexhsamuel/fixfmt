#include <memory>
#include <utility>

#include <Python.h>

#include "String.hh"

using namespace py;
using std::unique_ptr;

//------------------------------------------------------------------------------

namespace {

int tp_init(String* self, PyObject* args, PyObject* kw_args)
{
  static char const* arg_names[] 
      = {"size", "ellipsis", "pad", "position", "pad_left", nullptr};

  int         size;
  char const* ellipsis = "...";
  char        pad = ' ';
  double      position = 1.0;
  bool        pad_left = false;
  if (!PyArg_ParseTupleAndKeywords(
      args, kw_args, "i|sCdb", (char**) arg_names,
      &size, &ellipsis, &pad, &position, &pad_left)) 
    return -1;

  // FIXME: Validate args.
  self->fmt_ = unique_ptr<fixfmt::String>(
      new fixfmt::String(size, ellipsis, pad, position, pad_left));
  return 0;
}


PyObject* tp_call(String* self, PyObject* args, PyObject* kw_args)
{
  static char const* arg_names[] = {"str", nullptr};
  char* val;
  if (!PyArg_ParseTupleAndKeywords(
      args, kw_args, "s", (char**) arg_names, &val)) 
    return nullptr;

  return Unicode::from((*self->fmt_)(val)).release();
}


auto methods = Methods<String>()
;


Object* get_width(String* const self, void* /* closure */)
{
  return Long::FromLong(self->fmt_->get_width()).release();
}


PyGetSetDef const tp_getset[] = {
  {
    (char*)     "width",                                    // name
    (getter)    get_width,                                  // get
    (setter)    nullptr,                                    // set
    (char*)     nullptr,                                    // doc
    (void*)     nullptr,                                    // closure
  },
  GETSETDEF_END
};


}  // anonymous namespace


Type String::type_ = PyTypeObject{
  PyVarObject_HEAD_INIT(nullptr, 0)
  (char const*)         "fixfmt.String",                    // tp_name
  (Py_ssize_t)          sizeof(String),                     // tp_basicsize
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
  (PyMethodDef*)        methods,                            // tp_methods
  (PyMemberDef*)        nullptr,                            // tp_members
  (PyGetSetDef*)        tp_getset,                          // tp_getset
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


