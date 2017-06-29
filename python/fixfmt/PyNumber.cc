#include "PyNumber.hh"

using namespace py;
using std::unique_ptr;

//------------------------------------------------------------------------------

namespace {

// FIXME: Accept sign=None.
static void
tp_init(
  PyNumber* self, 
  Tuple* args, 
  Dict* kw_args)
{
  static char const* arg_names[] = {
    "size", "precision", "pad", "sign", "nan", "inf", "point", "bad",
    "scale", nullptr
  };

  int           size;
  Object*       precision_arg   = (Object*) Py_None;
#if PY3K
  int           pad             = fixfmt::Number::PAD_SPACE;
  int           sign            = fixfmt::Number::SIGN_NEGATIVE;
  int           point           = '.';
  int           bad             = '#';
#else
  char          pad             = fixfmt::Number::PAD_SPACE;
  char          sign            = fixfmt::Number::SIGN_NEGATIVE;
  char          point           = '.';
  char          bad             = '#';
#endif
  char const*   nan             = "NaN";
  char const*   inf             = "inf";
  Object*       scale_arg       = (Object*) Py_None;
  Arg::ParseTupleAndKeywords(
    args, kw_args, 
#if PY3K
    "i|O$CCetetCCO",
#else
    "i|OccetetccO",
#endif
    arg_names,
    &size, &precision_arg, &pad, &sign, "utf-8", &nan, "utf-8", &inf, 
    &point, &bad, &scale_arg);

  if (size < 0) 
    throw ValueError("negative size");
  int precision;
  if (precision_arg == Py_None)
    precision = fixfmt::Number::PRECISION_NONE;
  else {
    precision = precision_arg->long_value();
    if (precision < 0)
      precision = fixfmt::Number::PRECISION_NONE;
  }
  if (!(size > 0 || precision > 0))
    throw ValueError("no digits to show");
  if (   sign != fixfmt::Number::SIGN_NONE
      && sign != fixfmt::Number::SIGN_NEGATIVE
      && sign != fixfmt::Number::SIGN_ALWAYS)
    throw ValueError("invalid sign");
  if (! (pad == fixfmt::Number::PAD_SPACE || pad == fixfmt::Number::PAD_ZERO))
    throw ValueError("invalid pad");

  fixfmt::Number::Scale scale = {};
  auto const aliases = ((Object*) &PyNumber::type_)->GetAttrString("SCALES");
  if (Dict::Check(aliases)) {
    auto const alias = cast<Dict>(aliases)->GetItem(scale_arg, false);
    if (alias != nullptr)
      scale_arg = alias;
  }
  if (scale_arg == Py_None) 
    ;  // accept default
  else if (!Sequence::Check(scale_arg))
    throw ValueError("scale must be a two-item sequence");
  else {
    Sequence* scale_seq = cast<Sequence>(scale_arg);
    if (scale_seq->Length() != 2)
      throw ValueError("scale must be a two-item sequence");
    scale.factor = scale_seq->GetItem(0)->double_value();
    if (!(scale.factor > 0))
      throw ValueError("invalid scale factor");
    scale.suffix = scale_seq->GetItem(1)->Str()->as_utf8_string();
  }

  new(self) PyNumber;
  self->fmt_ = std::make_unique<fixfmt::Number>(
      fixfmt::Number::Args{
        size, precision, (char) pad, (char) sign, scale, (char) point,
        (char) bad, nan, inf});
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
  return Unicode::from(self->fmt_->get_args().bad);
}


ref<Object> get_inf(PyNumber* const self, void* /* closure */)
{
  return Unicode::from(self->fmt_->get_args().inf);
}


ref<Object> get_nan(PyNumber* const self, void* /* closure */)
{
  return Unicode::from(self->fmt_->get_args().nan);
}


ref<Object> get_pad(PyNumber* const self, void* /* closure */)
{
  return Unicode::from(self->fmt_->get_args().pad);
}


ref<Object> get_point(PyNumber* const self, void* /* closure */)
{
  return Unicode::from(self->fmt_->get_args().point);
}


ref<Object> get_precision(PyNumber* const self, void* /* closure */)
{
  int const precision = self->fmt_->get_args().precision;
  return 
    precision == fixfmt::Number::PRECISION_NONE ? none_ref()
    : (ref<Object>) Long::FromLong(precision);
}


ref<Object>
get_scale(
  PyNumber* const self,
  void* /* closure */)
{
  auto const& scale = self->fmt_->get_args().scale;
  if (scale.enabled())
    return Tuple::builder 
      << Float::from(scale.factor) 
      << Unicode::from(scale.suffix);
  else
    return none_ref();
}


ref<Object> get_sign(PyNumber* const self, void* /* closure */)
{
  return Unicode::from(self->fmt_->get_args().sign);
}


ref<Object> get_size(PyNumber* const self, void* /* closure */)
{
  return Long::FromLong(self->fmt_->get_args().size);
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
  .add_get<get_scale>       ("scale")
  .add_get<get_sign>        ("sign")
  .add_get<get_size>        ("size")
  .add_get<get_width>       ("width")
  ;


}  // anonymous namespace


Type PyNumber::type_ = PyTypeObject{
  PyVarObject_HEAD_INIT(nullptr, 0)
  (char const*)         "fixfmt._ext.Number",               // tp_name
  (Py_ssize_t)          sizeof(PyNumber),                   // tp_basicsize
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
  (initproc)            wrap<PyNumber, tp_init>,            // tp_init
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


