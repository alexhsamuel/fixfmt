#include <sstream>

#include "PyNumber.hh"

using namespace py;
using std::unique_ptr;

//------------------------------------------------------------------------------

namespace {

ref<Unicode> tp_repr(PyNumber* self)
{
  auto const& args = self->fmt_->get_args();
  std::stringstream ss;
  ss << "Number(" << args.size << ", " << args.precision 
     << ", pad='" << args.pad << "', sign='" << args.sign 
     << ", nan='" << args.nan << "', inf='" << args.inf
     << "', point='" << args.point << "', bad='" << args.bad << "')";
  return Unicode::from(ss.str());
}


int
get_precision(
  Object* arg)
{
  int precision;
  if (arg == Py_None)
    precision = fixfmt::Number::PRECISION_NONE;
  else {
    precision = arg->long_value();
    if (precision < 0)
      precision = fixfmt::Number::PRECISION_NONE;
  }
  return precision;
}


fixfmt::Number::Scale
get_scale(
  Object* arg)
{
  fixfmt::Number::Scale scale = {};
  auto const aliases = ((Object*) &PyNumber::type_)->GetAttrString("SCALES");
  if (Dict::Check(aliases)) {
    auto const alias = cast<Dict>(aliases)->GetItem(arg, false);
    if (alias != nullptr)
      arg = alias;
  }
  if (arg == Py_None) 
    ;  // accept default
  else if (!Sequence::Check(arg))
    throw ValueError("scale must be a two-item sequence");
  else {
    Sequence* scale_seq = cast<Sequence>(arg);
    if (scale_seq->Length() != 2)
      throw ValueError("scale must be a two-item sequence");
    scale.factor = scale_seq->GetItem(0)->double_value();
    if (!(scale.factor > 0))
      throw ValueError("invalid scale factor");
    scale.suffix = scale_seq->GetItem(1)->Str()->as_utf8_string();
  }
  return scale;
}


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
  int           pad             = fixfmt::Number::PAD_SPACE;
  int           sign            = fixfmt::Number::SIGN_NEGATIVE;
  int           point           = '.';
  int           bad             = '#';
  char const*   nan             = "NaN";
  char const*   inf             = "inf";
  Object*       scale_arg       = (Object*) Py_None;
  Arg::ParseTupleAndKeywords(
    args, kw_args, 
    "i|O$CCetetCCO",
    arg_names,
    &size, &precision_arg, &pad, &sign, "utf-8", &nan, "utf-8", &inf, 
    &point, &bad, &scale_arg);

  if (size < 0) 
    throw ValueError("negative size");
  auto const precision = get_precision(precision_arg);
  if (   sign != fixfmt::Number::SIGN_NONE
      && sign != fixfmt::Number::SIGN_NEGATIVE
      && sign != fixfmt::Number::SIGN_ALWAYS)
    throw ValueError("invalid sign");
  if (! (pad == fixfmt::Number::PAD_SPACE || pad == fixfmt::Number::PAD_ZERO))
    throw ValueError("invalid pad");

  auto const scale = get_scale(scale_arg);

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
  // FIXME: Handle integer types separately.
  Arg::ParseTupleAndKeywords(args, kw_args, "d", arg_names, &val);

  return Unicode::from((*self->fmt_)(val));
}


auto methods = Methods<PyNumber>()
;


ref<Object> get_bad(PyNumber* const self, void* /* closure */)
{
  return Unicode::from(self->fmt_->get_args().bad);
}


void set_bad(PyNumber* const self, Object* val, void* /* closure */)
{
  auto const bad_str = val->Str();
  if (bad_str->Length() != 1)
    throw ValueError("invalid bad");

  auto args = self->fmt_->get_args();
  // FIXME: Wrong for multibyte characters.
  args.bad = bad_str->as_utf8_string()[0];
  self->fmt_->set_args(args);
}


ref<Object> get_inf(PyNumber* const self, void* /* closure */)
{
  return Unicode::from(self->fmt_->get_args().inf);
}


void set_inf(PyNumber* const self, Object* val, void* /* closure */)
{
  auto args = self->fmt_->get_args();
  args.inf = val->Str()->as_utf8_string().c_str();
  self->fmt_->set_args(args);
}


ref<Object> get_nan(PyNumber* const self, void* /* closure */)
{
  return Unicode::from(self->fmt_->get_args().nan);
}


void set_nan(PyNumber* const self, Object* val, void* /* closure */)
{
  auto args = self->fmt_->get_args();
  args.nan = val->Str()->as_utf8_string().c_str();
  self->fmt_->set_args(args);
}


ref<Object> get_pad(PyNumber* const self, void* /* closure */)
{
  return Unicode::from(self->fmt_->get_args().pad);
}


void set_pad(PyNumber* const self, Object* val, void* /* closure */)
{
  auto const pad_str = val->Str();
  if (pad_str->Length() != 1)
    throw ValueError("invalid pad");
  // FIXME: Wrong for multibyte characters.
  auto const pad = pad_str->as_utf8_string()[0];
  if (   pad != fixfmt::Number::PAD_SPACE
      && pad != fixfmt::Number::PAD_ZERO)
    throw ValueError("invalid pad");

  auto args = self->fmt_->get_args();
  args.pad = pad;
  self->fmt_->set_args(args);
}


ref<Object> get_point(PyNumber* const self, void* /* closure */)
{
  return Unicode::from(self->fmt_->get_args().point);
}


void set_point(PyNumber* const self, Object* val, void* /* closure */)
{
  auto const point_str = val->Str();
  if (point_str->Length() != 1)
    throw ValueError("invalid point");

  auto args = self->fmt_->get_args();
  // FIXME: Wrong for multibyte characters.
  args.point = point_str->as_utf8_string()[0];
  self->fmt_->set_args(args);
}


ref<Object> get_precision(PyNumber* const self, void* /* closure */)
{
  int const precision = self->fmt_->get_args().precision;
  return 
    precision == fixfmt::Number::PRECISION_NONE ? none_ref()
    : (ref<Object>) Long::FromLong(precision);
}


void set_precision(PyNumber* const self, Object* val, void* /* closure */)
{
  auto args = self->fmt_->get_args();
  args.precision = get_precision(val);
  self->fmt_->set_args(args);
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


void set_scale(PyNumber* const self, Object* val, void* /* closure */)
{
  auto args = self->fmt_->get_args();
  args.scale = get_scale(val);
  self->fmt_->set_args(args);
}


ref<Object> get_sign(PyNumber* const self, void* /* closure */)
{
  return Unicode::from(self->fmt_->get_args().sign);
}


void set_sign(PyNumber* const self, Object* const val, void* /* closure */)
{
  auto const sign_str = val->Str();
  if (sign_str->Length() != 1)
    throw ValueError("invalid sign");
  auto const sign = sign_str->as_utf8_string()[0];
  if (   sign != fixfmt::Number::SIGN_NONE
      && sign != fixfmt::Number::SIGN_NEGATIVE
      && sign != fixfmt::Number::SIGN_ALWAYS)
    throw ValueError("invalid sign");

  auto args = self->fmt_->get_args();
  args.sign = sign;
  self->fmt_->set_args(args);
}


ref<Object> get_size(PyNumber* const self, void* /* closure */)
{
  return Long::FromLong(self->fmt_->get_args().size);
}


void set_size(PyNumber* const self, Object* val, void* /* closure */)
{
  auto const size = val->long_value();
  if (size < 0)
    throw ValueError("size out of range");
  auto args = self->fmt_->get_args();
  args.size = size;
  self->fmt_->set_args(args);
}


ref<Object> get_width(PyNumber* const self, void* /* closure */)
{
  return Long::FromLong(self->fmt_->get_width());
}


auto getsets = GetSets<PyNumber>()
  .add_getset<get_bad       , set_bad       >("bad")
  .add_getset<get_inf       , set_inf       >("inf")
  .add_getset<get_nan       , set_nan       >("nan")
  .add_getset<get_pad       , set_pad       >("pad")
  .add_getset<get_point     , set_point     >("point")
  .add_getset<get_precision , set_precision >("precision")
  .add_getset<get_scale     , set_scale     >("scale")
  .add_getset<get_sign      , set_sign      >("sign")
  .add_getset<get_size      , set_size      >("size")
  .add_get<get_width>                        ("width")
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
  (PyAsyncMethods*)     nullptr,                            // tp_as_async
  (reprfunc)            wrap<PyNumber, tp_repr>,            // tp_repr
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
  (destructor)          nullptr,                            // tp_finalize
};


