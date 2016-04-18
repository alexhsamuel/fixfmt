#include "PyNumber.hh"

using namespace py;
using std::unique_ptr;

//------------------------------------------------------------------------------

namespace {

// FIXME: Wrap for exceptions.
// FIXME: Accept sign=None.
static int tp_init(PyNumber* self, PyObject* args, PyObject* kw_args)
{
  static char const* arg_names[] = {
      "size", "precision", "pad", "sign", "nan", "inf", "point", "bad",
      nullptr
  };

  int            size;
  Object*        precision_arg   = (Object*) Py_None;
  int            pad             = fixfmt::Number::PAD_SPACE;
  int            sign            = fixfmt::Number::SIGN_NEGATIVE;
  char const*    nan             = "NaN";
  char const*    inf             = "inf";
  int            point           = '.';
  int            bad             = '#';
  if (!PyArg_ParseTupleAndKeywords(
      args, kw_args, "i|O$CCssCC", (char**) arg_names,
      &size, &precision_arg, &pad, &sign, &nan, &inf, &point, &bad)) 
    return -1;

  if (size < 0) {
    // FIXME
    PyErr_SetString(PyExc_ValueError, "negative size");
    return 1;
  }
  int precision;
  if (precision_arg == Py_None)
    precision = fixfmt::Number::PRECISION_NONE;
  else {
    precision = precision_arg->long_value();
    if (precision < 0)
      precision = fixfmt::Number::PRECISION_NONE;
  }
  if (!(size > 0 || precision > 0)) {
    // FIXME
    PyErr_SetString(PyExc_ValueError, "no sign or precision");
    return 1;
  }
  if (   sign != fixfmt::Number::SIGN_NONE
      && sign != fixfmt::Number::SIGN_NEGATIVE
      && sign != fixfmt::Number::SIGN_ALWAYS) {
    // FIXME
    PyErr_SetString(PyExc_ValueError, "invalid sign");
    return 1;
  }

  new(self) PyNumber;
  self->fmt_ = unique_ptr<fixfmt::Number>(
      new fixfmt::Number({
          size, precision, 
          (char) sign, (char) pad, (char) point, 
          std::string(nan), std::string(inf), (char) bad}));
  return 0;
}


ref<Object> tp_call(PyNumber* self, Tuple* args, Dict* kw_args)
{
  static char const* arg_names[] = {"value", nullptr};
  double val;
  Arg::ParseTupleAndKeywords(args, kw_args, "d", arg_names, &val);

  return Unicode::from((*self->fmt_)(val));
}


auto methods = Methods<PyNumber>()
;


ref<Object> get_bad(PyNumber* const self, void* /* closure */)
{
  return Unicode::from(self->fmt_->get_bad());
}


ref<Object> get_inf(PyNumber* const self, void* /* closure */)
{
  return Unicode::from(self->fmt_->get_inf());
}


ref<Object> get_nan(PyNumber* const self, void* /* closure */)
{
  return Unicode::from(self->fmt_->get_nan());
}


ref<Object> get_pad(PyNumber* const self, void* /* closure */)
{
  return Unicode::from(self->fmt_->get_pad());
}


ref<Object> get_point(PyNumber* const self, void* /* closure */)
{
  return Unicode::from(self->fmt_->get_point());
}


ref<Object> get_precision(PyNumber* const self, void* /* closure */)
{
  int const precision = self->fmt_->get_precision();
  return 
    precision == fixfmt::Number::PRECISION_NONE ? none_ref()
    : (ref<Object>) Long::FromLong(precision);
}


ref<Object> get_sign(PyNumber* const self, void* /* closure */)
{
  return Unicode::from(self->fmt_->get_sign());
}


ref<Object> get_size(PyNumber* const self, void* /* closure */)
{
  return Long::FromLong(self->fmt_->get_size());
}


ref<Object> get_width(PyNumber* const self, void* /* closure */)
{
  return Long::FromLong(self->fmt_->get_width());
}


auto getsets = GetSets<PyNumber>()
  .add_get<get_bad>         ("bad")
  .add_get<get_inf>         ("inf")
  .add_get<get_nan>         ("nan")
  .add_get<get_pad>         ("pad")
  .add_get<get_point>       ("point")
  .add_get<get_precision>   ("precision")
  .add_get<get_sign>        ("sign")
  .add_get<get_size>        ("size")
  .add_get<get_width>       ("width")
  ;


}  // anonymous namespace


Type PyNumber::type_ = PyTypeObject{
  PyVarObject_HEAD_INIT(nullptr, 0)
  (char const*)         "fixfmt.ext_.Number",               // tp_name
  (Py_ssize_t)          sizeof(PyNumber),                   // tp_basicsize
  (Py_ssize_t)          0,                                  // tp_itemsize
  (destructor)          nullptr,                            // tp_dealloc
  (printfunc)           nullptr,                            // tp_print
  (getattrfunc)         nullptr,                            // tp_getattr
  (setattrfunc)         nullptr,                            // tp_setattr
#if PY_MAJOR_VERSION >= 3 && PY_MINOR_VERSION >= 5
  (PyAsyncMethods*)     nullptr,                            // tp_as_async
#else
  (void*)               nullptr,                            // tp_reserved
#endif
  (reprfunc)            nullptr,                            // tp_repr
  (PyNumberMethods*)    nullptr,                            // tp_as_number
  (PySequenceMethods*)  nullptr,                            // tp_as_sequence
  (PyMappingMethods*)   nullptr,                            // tp_as_mapping
  (hashfunc)            nullptr,                            // tp_hash
  (ternaryfunc)         wrap<PyNumber, tp_call>,            // tp_call
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


