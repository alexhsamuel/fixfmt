#include <memory>
#include <sstream>
#include <utility>

#include <Python.h>

#include "PyString.hh"

using namespace py;
using std::unique_ptr;

//------------------------------------------------------------------------------

namespace {

ref<Unicode> tp_repr(PyString* self)
{
  auto const& args = self->fmt_->get_args();
  std::stringstream ss;
  ss << "String(" << args.size << ", ellipsis='" << args.ellipsis
     << "', pad='" << args.pad << ", elide_pos=" << args.elide_pos
     << ", pad_pos=" << args.pad_pos << ")";
  return Unicode::from(ss.str());
}


int tp_init(PyString* self, PyObject* args, PyObject* kw_args)
{
  static char const* arg_names[] 
      = {"size", "ellipsis", "pad", "elide_pos", "pad_pos", nullptr};

  int   size;
  char* ellipsis = nullptr;
  char* pad = nullptr;
  float elide_pos = 1;
  float pad_pos = fixfmt::PAD_POS_LEFT_JUSTIFY;
  if (!PyArg_ParseTupleAndKeywords(
      args, kw_args, "i|ssff", (char**) arg_names,
      &size, &ellipsis, &pad, &elide_pos, &pad_pos)) 
    return -1;

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
  Object* val;
  if (!PyArg_ParseTupleAndKeywords(
      args, kw_args, "O", (char**) arg_names, &val))
    return nullptr;

  return Unicode::from((*self->fmt_)(val->Str()->as_utf8_string())).release();
}


auto methods = Methods<PyString>()
;


ref<Object> get_elide_pos(PyString* const self, void* /* closure */)
{
  return Float::FromDouble(self->fmt_->get_args().elide_pos);
}


void set_elide_pos(PyString* const self, Object* val, void* /* closure */)
{
  auto args = self->fmt_->get_args();
  args.elide_pos = val->double_value();
  self->fmt_->set_args(args);
}


ref<Object> get_ellipsis(PyString* const self, void* /* closure */)
{
  return Unicode::from(self->fmt_->get_args().ellipsis);
}


void set_ellipsis(PyString* const self, Object* val, void* /* closure */)
{
  auto args = self->fmt_->get_args();
  args.ellipsis = val->Str()->as_utf8_string();
  self->fmt_->set_args(args);
}


ref<Object> get_pad(PyString* const self, void* /* closure */)
{
  return Unicode::from(self->fmt_->get_args().pad);
}


void set_pad(PyString* const self, Object* val, void* /* closure */)
{
  auto args = self->fmt_->get_args();
  args.pad = val->Str()->as_utf8_string();
  self->fmt_->set_args(args);
}


ref<Object> get_pad_pos(PyString* const self, void* /* closure */)
{
  return Float::FromDouble(self->fmt_->get_args().pad_pos);
}


void set_pad_pos(PyString* const self, Object* val, void* /* closure */)
{
  auto const pos = val->double_value();
  if (pos < 0 || 1 < pos)
    throw ValueError("pos out of range");
  auto args = self->fmt_->get_args();
  args.pad_pos = pos;
  self->fmt_->set_args(args);
}


ref<Object> get_size(PyString* const self, void* /* closure */)
{
  return Long::FromLong(self->fmt_->get_args().size);
}


void set_size(PyString* const self, Object* val, void* /* closure */)
{
  auto const size = val->long_value();
  if (size < 0)
    throw ValueError("size out of range");
  auto args = self->fmt_->get_args();
  args.size = size;
  self->fmt_->set_args(args);
}


ref<Object> get_width(PyString* const self, void* /* closure */)
{
  return Long::FromLong(self->fmt_->get_width());
}


auto getsets = GetSets<PyString>()
  .add_getset<get_elide_pos , set_elide_pos >("elide_pos")
  .add_getset<get_ellipsis  , set_ellipsis  >("ellipsis")
  .add_getset<get_pad       , set_pad       >("pad")
  .add_getset<get_pad_pos   , set_pad_pos   >("pad_pos")
  .add_getset<get_size      , set_size      >("size")
  .add_get   <get_width                     >("width")
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
  (PyAsyncMethods*)     nullptr,                            // tp_as_async
  (reprfunc)            wrap<PyString, tp_repr>,            // tp_repr
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
  (destructor)          nullptr,                            // tp_finalize
};


