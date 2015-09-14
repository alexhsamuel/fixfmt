#include "Number.hh"

using namespace py;
using std::unique_ptr;

//------------------------------------------------------------------------------

namespace {

static int tp_init(Number* self, PyObject* args, PyObject* kw_args)
{
  static char const* arg_names[] = {
      "size", "precision", "pad", "sign", "nan", "inf", "point", "bad",
      nullptr
  };

  int            size;
  int            precision   = fixfmt::Number::PRECISION_NONE;
  int            pad         = fixfmt::Number::PAD_SPACE;
  int            sign        = fixfmt::Number::SIGN_NEGATIVE;
  char const*    nan         = "NaN";
  char const*    inf         = "inf";
  int            point       = '.';
  int            bad         = '#';
  if (!PyArg_ParseTupleAndKeywords(
      args, kw_args, "i|i$CCssCC", (char**) arg_names,
      &size, &precision, &pad, &sign, &nan, &inf, &point, &bad)) 
    return -1;

  self->fmt_ = unique_ptr<fixfmt::Number>(
      new fixfmt::Number(
          size, precision, (char) pad, (char) sign, std::string(nan),
          std::string(inf), (char) point, (char) bad));
  return 0;
}

static PyObject* tp_call(Number* self, PyObject* args, PyObject* kw_args)
{
  static char const* arg_names[] = {"value", nullptr};
  double val;
  if (PyArg_ParseTupleAndKeywords(
      args, kw_args, "d", (char**) arg_names, &val)) {
    fixfmt::Number& fmt = *self->fmt_;
    char buf[fmt.get_width()];
    fmt.format(val, buf);
    return Unicode::FromStringAndSize(buf, fmt.get_width()).release();
  }
  else
    return nullptr;
}


auto methods = Methods<Number>()
;


Object* get_width(Number* const self, void* /* closure */)
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


Type Number::type_ = PyTypeObject{
  PyVarObject_HEAD_INIT(nullptr, 0)
  (char const*)         "fixfmt.Number",                    // tp_name
  (Py_ssize_t)          sizeof(Number),                     // tp_basicsize
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
  (PyObject*)           nullptr,                            // tp_subclasses
  (PyObject*)           nullptr,                            // tp_weaklist
  (destructor)          nullptr,                            // tp_del
  (unsigned int)        0,                                  // tp_version_tag
  (destructor)          nullptr,                            // tp_finalize
};


