#include "Number.hh"

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
  if (PyArg_ParseTupleAndKeywords(
      args, kw_args, "i|i$CCssCC", (char**) arg_names,
      &size, &precision, &pad, &sign, &nan, &inf, &point, &bad)) {
    self->fmt_ = unique_ptr<fixfmt::Number>(
        new fixfmt::Number(
            size, precision, (char) pad, (char) sign, std::string(nan),
            std::string(inf), (char) point, (char) bad));
    return 0;
  }
  else
    return -1;
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


PyMethodDef const tp_methods[] = {
  METHODDEF_END
};


}  // anonymous namespace


PyTypeObject Number::type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  "fixfmt.Number",             // tp_name
  sizeof(Number),              // tp_basicsize
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


