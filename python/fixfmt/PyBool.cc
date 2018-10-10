#include <cstring>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

#include <Python.h>

#include "PyBool.hh"
#include "fixfmt.hh"
#include "py.hh"

using namespace py;
using std::string;
using std::unique_ptr;

//------------------------------------------------------------------------------

namespace {

ref<Unicode> tp_repr(PyBool* self)
{
  auto const& args = self->fmt_->get_args();
  std::stringstream ss;
  ss << "Bool('" << args.true_str << "', '" << args.false_str
     << "', size=" << args.size << ", pos=" << args.pos << ")";
  return Unicode::from(ss.str());
}


int tp_init(PyBool* self, PyObject* args, PyObject* kw_args)
{
  static char const* arg_names[] = {
    "true", "false", "size", "pos", nullptr };
  char const*    true_str    = "true";
  char const*    false_str   = "false";
  int            size        = -1;
  float          pos         = fixfmt::PAD_POS_LEFT_JUSTIFY;
  if (!PyArg_ParseTupleAndKeywords(
      args, kw_args, "|ssif", (char**) arg_names,
      &true_str, &false_str, &size, &pos)) 
    return -1;

  if (size < 0)
    size = std::max(
      fixfmt::string_length(true_str), fixfmt::string_length(false_str));

  new(self) PyBool;
  self->fmt_ = std::make_unique<fixfmt::Bool>(
      fixfmt::Bool::Args{size, string(true_str), string(false_str), pos});
  return 0;
}


PyObject* tp_call(PyBool* self, PyObject* args, PyObject* kw_args)
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


auto methods = Methods<PyBool>()
;


ref<Object> get_false(PyBool* const self, void* /* closure */)
{
  return Unicode::from(self->fmt_->get_args().false_str);
}


void set_false(PyBool* const self, Object* val, void* /* closure */)
{
  auto args = self->fmt_->get_args();
  args.false_str = val->Str()->as_utf8_string();
  self->fmt_->set_args(args);
}


ref<Object> get_pos(PyBool* const self, void* /* closure */)
{
  return Float::FromDouble(self->fmt_->get_args().pos);
}


void set_pos(PyBool* const self, Object* val, void* /* closure */)
{
  auto const pos = val->double_value();
  if (pos < 0 || 1 < pos)
    throw ValueError("pos out of range");
  auto args = self->fmt_->get_args();
  args.pos = pos;
  self->fmt_->set_args(args);
}


ref<Object> get_size(PyBool* const self, void* /* closure */)
{
  return Long::FromLong(self->fmt_->get_args().size);
}


void set_size(PyBool* const self, Object* val, void* /* closure */)
{
  auto const size = val->long_value();
  if (size < 0)
    throw ValueError("size out of range");
  auto args = self->fmt_->get_args();
  args.size = size;
  self->fmt_->set_args(args);
}


ref<Object> get_true(PyBool* const self, void* /* closure */)
{
  return Unicode::from(self->fmt_->get_args().true_str);
}


void set_true(PyBool* const self, Object* val, void* /* closure */)
{
  auto args = self->fmt_->get_args();
  args.true_str = val->Str()->as_utf8_string();
  self->fmt_->set_args(args);
}


ref<Object> get_width(PyBool* const self, void* /* closure */)
{
  return Long::FromLong(self->fmt_->get_width());
}


auto getsets = GetSets<PyBool>()
  .add_getset<get_false, set_false> ("false")
  .add_getset<get_pos, set_pos>     ("pos")
  .add_getset<get_size, set_size>   ("size")
  .add_getset<get_true, set_true>   ("true")
  .add_get<get_width>               ("width")
  ;


}  // anonymous namespace


Type PyBool::type_ = PyTypeObject{
  PyVarObject_HEAD_INIT(nullptr, 0)
  (char const*)         "fixfmt._ext.Bool",                 // tp_name
  (Py_ssize_t)          sizeof(PyBool),                     // tp_basicsize
  (Py_ssize_t)          0,                                  // tp_itemsize
  (destructor)          nullptr,                            // tp_dealloc
  (printfunc)           nullptr,                            // tp_print
  (getattrfunc)         nullptr,                            // tp_getattr
  (setattrfunc)         nullptr,                            // tp_setattr
  (PyAsyncMethods*)     nullptr,                            // tp_as_async
  (reprfunc)            wrap<PyBool, tp_repr>,              // tp_repr
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


