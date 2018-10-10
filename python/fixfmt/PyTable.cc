#include <iostream>
#include <string>
#include <utility>

#include "PyBool.hh"
#include "PyNumber.hh"
#include "PyString.hh"
#include "PyTable.hh"
#include "PyTickTime.hh"

using namespace py;
using std::unique_ptr;

//------------------------------------------------------------------------------

namespace {

void tp_dealloc(PyTable* self)
{
  self->~PyTable();
  self->ob_type->tp_free(self);
}


int tp_init(PyTable* self, Tuple* args, Dict* kw_args)
{
  // No arguments.
  static char const* arg_names[] = {nullptr};
  Arg::ParseTupleAndKeywords(args, kw_args, "", (char**) arg_names);

  new(self) PyTable;
  self->table_ = unique_ptr<fixfmt::Table>(new fixfmt::Table());
  return 0;
}


ref<Object> tp_call(PyTable* self, Tuple* args, Dict* kw_args)
{
  static char const* arg_names[] = {"index", nullptr};
  long index;
  Arg::ParseTupleAndKeywords(args, kw_args, "l", arg_names, &index);

  if (index < 0)
    throw IndexError("negative index");
  if (index >= self->table_->get_length())
    throw IndexError("index larger than length");

  return Unicode::from((*self->table_)(index));
}


Py_ssize_t sq_length(PyTable* table)
{
  return table->table_->get_length();
}


PySequenceMethods const tp_as_sequence = {
  (lenfunc)         sq_length,          // sq_length
  (binaryfunc)      nullptr,            // sq_concat
  (ssizeargfunc)    nullptr,            // sq_repeat
  (ssizeargfunc)    nullptr,            // sq_item
  (void*)           nullptr,            // was_sq_slice
  (ssizeobjargproc) nullptr,            // sq_ass_item
  (void*)           nullptr,            // was_sq_ass_slice
  (objobjproc)      nullptr,            // sq_contains
  (binaryfunc)      nullptr,            // sq_inplace_concat
  (ssizeargfunc)    nullptr,            // sq_inplace_repeat
};


ref<Object> add_string(PyTable* self, Tuple* args, Dict* kw_args)
{
  static char const* arg_names[] = {"str", nullptr};
  Object* str_arg;
  Arg::ParseTupleAndKeywords(args, kw_args, "O", arg_names, &str_arg);

  if (str_arg != Py_None) {
    std::string const str{str_arg->Str()->as_utf8_string()};
    if (str.length() > 0)
      self->table_->add_string(std::move(str));
  }

  return none_ref();
}


/**
 * Template method for adding a column to the table.
 *
 * 'buf' is a 'bytes' object containing contiguous values of type 'TYPE', e.g.
 * 'int' or 'double'.  'PYFMT' is a Python object that wraps a formatter for
 * 'TYPE' values.
 */
template<typename TYPE, typename PYFMT>
ref<Object> add_column(PyTable* self, Tuple* args, Dict* kw_args)
{
  // Parse args.
  static char const* arg_names[] = {"buf", "format", nullptr};
  PyObject* array;
  PYFMT* format;
  Arg::ParseTupleAndKeywords(
      args, kw_args, "OO!", arg_names, 
      &array, &PYFMT::type_, &format);

  // Validate args.
  BufferRef buffer(array, PyBUF_ND);
  if (buffer->ndim != 1)
    throw TypeError("not a one-dimensional array");
  if (buffer->itemsize != sizeof(TYPE))
    throw TypeError("wrong itemsize");

  // Add the column.
  using Column = fixfmt::ColumnImpl<TYPE, typename PYFMT::Formatter>;
  self->table_->add_column(std::make_unique<Column>(
    reinterpret_cast<TYPE*>(buffer->buf), 
    buffer->shape[0], 
    *format->fmt_));
  // Hold on to the buffer ref.
  self->buffers_.push_back(std::move(buffer));

  return none_ref();
}


/**
 * Column of Python object pointers, with an object first converted with 'str()'
 * and then formatted as a string.
 */
class StrObjectColumn
  : public fixfmt::Column
{
public:

  StrObjectColumn(Object** values, long const length, fixfmt::String format)
  : values_(values),
    length_(length),
    format_(std::move(format))
  {
  }

  virtual ~StrObjectColumn() override {}

  virtual int get_width() const override { return format_.get_width(); }

  virtual long get_length() const override { return length_; }

  virtual std::string operator()(long const index) const override
  {
    // Convert (or cast) to string.
    auto str = values_[index]->Str();
    // Format the string.
    return format_(str->as_utf8_string());
  }

private:

  Object** const values_;
  long const length_;
  fixfmt::String const format_;

};


ref<Object> add_tick_time_column(PyTable* self, Tuple* args, Dict* kw_args)
{
  // Parse args.
  static char const* arg_names[] = { "buf", "format", nullptr };
  PyObject* array;
  PyTickTime* format;
  Arg::ParseTupleAndKeywords(
    args, kw_args, "OO!", arg_names,
    &array, &PyTickTime::type_, &format);

  // Validate args.
  BufferRef buffer(array, PyBUF_ND);
  if (buffer->ndim != 1)
    throw TypeError("not a one-dimensional array");
  if (buffer->itemsize != sizeof(long))
    throw TypeError("wrong itemsize");

  // Add the column.
  using Column = fixfmt::ColumnImpl<long, typename PyTickTime::Formatter>;
  self->table_->add_column(std::make_unique<Column>(
    reinterpret_cast<long const*>(buffer->buf),
    buffer->shape[0], 
    *format->fmt_));
  // Hold on to the buffer ref.
  self->buffers_.emplace_back(std::move(buffer));

  return none_ref();
}


ref<Object> add_str_object_column(PyTable* self, Tuple* args, Dict* kw_args)
{
  // Parse args.
  static char const* arg_names[] = {"buf", "format", nullptr};
  PyObject* array;
  PyString* format;
  Arg::ParseTupleAndKeywords(
      args, kw_args, "OO!", arg_names,
      &array, &PyString::type_, &format);
  
  // Validate args.
  BufferRef buffer(array, PyBUF_ND);
  if (buffer->ndim != 1)
    throw TypeError("not a one-dimensional array");
  if (buffer->itemsize != sizeof(Object*))
    throw TypeError("wrong itemsize");

  // Add the column.
  self->table_->add_column(std::make_unique<StrObjectColumn>(
    reinterpret_cast<Object**>(buffer->buf),
    buffer->shape[0], 
    *format->fmt_));
  // Hold on to the buffer ref.
  self->buffers_.emplace_back(std::move(buffer));

  return none_ref();
}


auto methods = Methods<PyTable>()
  .add<add_string>                      ("add_string")
  .add<add_column<bool,    PyBool>>     ("add_bool")
  .add<add_column<char,    PyNumber>>   ("add_int8")
  .add<add_column<short,   PyNumber>>   ("add_int16")
  .add<add_column<int,     PyNumber>>   ("add_int32")
  .add<add_column<long,    PyNumber>>   ("add_int64")
  .add<add_column<float,   PyNumber>>   ("add_float32")
  .add<add_column<double,  PyNumber>>   ("add_float64")
  .add<add_tick_time_column>            ("add_tick_time")
  .add<add_str_object_column>           ("add_str_object")
;


ref<Object> get_length(PyTable* const self, void* /* closure */)
{
  return Long::FromLong(self->table_->get_length());
}


ref<Object> get_width(PyTable* const self, void* /* closure */)
{
  return Long::FromLong(self->table_->get_width());
}


auto getsets = GetSets<PyTable>()
  .add_get<get_length>      ("length")
  .add_get<get_width>       ("width")
  ;


}  // anonymous namespace


Type PyTable::type_ = PyTypeObject{
  PyVarObject_HEAD_INIT(nullptr, 0)
  (char const*)         "fixfmt._ext.Table",                // tp_name
  (Py_ssize_t)          sizeof(PyTable),                    // tp_basicsize
  (Py_ssize_t)          0,                                  // tp_itemsize
  (destructor)          tp_dealloc,                         // tp_dealloc
  (printfunc)           nullptr,                            // tp_print
  (getattrfunc)         nullptr,                            // tp_getattr
  (setattrfunc)         nullptr,                            // tp_setattr
  (PyAsyncMethods*)     nullptr,                            // tp_as_async
  (reprfunc)            nullptr,                            // tp_repr
  (PyNumberMethods*)    nullptr,                            // tp_as_number
  (PySequenceMethods*)  &tp_as_sequence,                    // tp_as_sequence
  (PyMappingMethods*)   nullptr,                            // tp_as_mapping
  (hashfunc)            nullptr,                            // tp_hash
  (ternaryfunc)         wrap<PyTable, tp_call>,             // tp_call
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


