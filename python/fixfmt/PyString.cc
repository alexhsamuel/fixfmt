#include <memory>
#include <utility>

#include <Python.h>

#include "PyString.hh"

using namespace py;
using std::unique_ptr;

//------------------------------------------------------------------------------

namespace {

int tp_init(PyString* self, PyObject* args, PyObject* kw_args)
{
  static char const* arg_names[] 
      = {"size", "ellipsis", "pad", "elide_pos", "pad_pos", nullptr};

  int   size;
  char* ellipsis = nullptr;
  char* pad = nullptr;
  float elide_pos = 1;
  float pad_pos = fixfmt::PAD_POS_LEFT_JUSTIFY;
#if PY3K
  if (!PyArg_ParseTupleAndKeywords(
      args, kw_args, "i|ssff", (char**) arg_names,
      &size, &ellipsis, &pad, &elide_pos, &pad_pos)) 
    return -1;
#else
  if (!PyArg_ParseTupleAndKeywords(
      args, kw_args, "i|etetff", (char**) arg_names,
      &size, "utf-8", &ellipsis, "utf-8", &pad, &elide_pos, &pad_pos))
    return -1;
  PyMemGuard ellipsis_guard(ellipsis);
  PyMemGuard pad_guard(pad);
#endif

  if (ellipsis == nullptr)
    ellipsis = (char*) "\u2026";
  if (pad == nullptr)
    pad = (char*) " ";
  if (strlen(pad) == 0) {
    PyErr_SetString(PyExc_ValueError, "empty pad");
    return 1;
  }

  new(self) PyString;
  self->fmt_ = std::make_unique<fixfmt::String>(
    fixfmt::String::Args{
      size, ellipsis, pad, (float) elide_pos, (float) pad_pos});
  return 0;
}


PyObject* tp_call(PyString* self, PyObject* args, PyObject* kw_args)
{
  static char const* arg_names[] = {"str", nullptr};
  char* val;
#if PY3K
  if (!PyArg_ParseTupleAndKeywords(
      args, kw_args, "s", (char**) arg_names, &val))
    return nullptr;
#else
  if (!PyArg_ParseTupleAndKeywords(
      args, kw_args, "et", (char**) arg_names, "utf-8", &val))
    return nullptr;
  PyMemGuard str_guard(val);
#endif

  return Unicode::from((*self->fmt_)(val)).release();
}


auto methods = Methods<PyString>()
;


ref<Object> get_ellipsis(PyString* const self, void* /* closure */)
{
  return Unicode::from(self->fmt_->get_args().ellipsis);
}


ref<Object> get_pad(PyString* const self, void* /* closure */)
{
  return Unicode::from(self->fmt_->get_args().pad);
}


ref<Object> get_pad_pos(PyString* const self, void* /* closure */)
{
  return Float::FromDouble(self->fmt_->get_args().pad_pos);
}


ref<Object> get_elide_pos(PyString* const self, void* /* closure */)
{
  return Float::FromDouble(self->fmt_->get_args().elide_pos);
}


ref<Object> get_size(PyString* const self, void* /* closure */)
{
  return Long::FromLong(self->fmt_->get_args().size);
}


ref<Object> get_width(PyString* const self, void* /* closure */)
{
  return Long::FromLong(self->fmt_->get_width());
}


auto getsets = GetSets<PyString>()
  .add_get<get_ellipsis>        ("ellipsis")
  .add_get<get_pad>             ("pad")
  .add_get<get_pad_pos>         ("pad_pos")
  .add_get<get_elide_pos>       ("elide_pos")
  .add_get<get_size>            ("size")
  .add_get<get_width>           ("width")
  ;


}  // anonymous namespace


Type PyString::type_ = PyTypeObject{
  PyVarObject_HEAD_INIT(nullptr, 0)
  (char const*)         "fixfmt._ext.String",               // tp_name
  (Py_ssize_t)          sizeof(PyString),                   // tp_basicsize
  (Py_ssize_t)          0,                                  // tp_itemsize
  (destructor)          nullptr,                            // tp_dealloc
  (printfunc)           nullptr,                            // tp_print
  (getattrfunc)         nullptr,                            // tp_getattr
  (setattrfunc)         nullptr,                            // tp_setattr
#if PY3K
  (PyAsyncMethods*)     nullptr,                            // tp_as_async
#else
  (cmpfunc)             nullptr,                            // tp_compare
#endif
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
  (PyGetSetDef*)        getsets,                            // tp_getset
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
  (PyObject*)           nullptr,                            // tp_subclasses
  (PyObject*)           nullptr,                            // tp_weaklist
  (destructor)          nullptr,                            // tp_del
  (unsigned int)        0,                                  // tp_version_tag
#if PY3K
  (destructor)          nullptr,                            // tp_finalize
#endif
};


