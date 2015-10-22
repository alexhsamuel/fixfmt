#include <iostream>
#include <string>
#include <utility>

#include "Bool.hh"
#include "Number.hh"
#include "String.hh"
#include "Table.hh"

using namespace py;
using std::unique_ptr;

//------------------------------------------------------------------------------

namespace {

void tp_dealloc(Table* self)
{
  self->~Table();
  self->ob_type->tp_free(self);
}


int tp_init(Table* self, Tuple* args, Dict* kw_args)
{
  // No arguments.
  static char const* arg_names[] = {nullptr};
  Arg::ParseTupleAndKeywords(args, kw_args, "", (char**) arg_names);

  new(self) Table();
  self->table_ = unique_ptr<fixfmt::Table>(new fixfmt::Table());
  return 0;
}


ref<Object> tp_call(Table* self, Tuple* args, Dict* kw_args)
{
  static char const* arg_names[] = {"index", nullptr};
  long index;
  Arg::ParseTupleAndKeywords(args, kw_args, "l", arg_names, &index);

  if (index < 0)
    throw Exception(PyExc_IndexError, "negative index");
  if (index >= self->table_->get_length())
    throw Exception(PyExc_IndexError, "index larger than length");

  return Unicode::from((*self->table_)(index));
}


Py_ssize_t sq_length(Table* table)
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


ref<Object> add_string(Table* self, Tuple* args, Dict* kw_args)
{
  static char const* arg_names[] = {"str", nullptr};
  char* str;
  Arg::ParseTupleAndKeywords(args, kw_args, "s", arg_names, &str);

  self->table_->add_string(std::string(str));
  return none_ref();
}


/**
 * Template method for adding a column to the table.
 *
 * 'buf' is a 'bytes' object containing contiguous values of type 'TYPE', e.g.
 * 'int' or 'double'.  'PYFMT" is a Python object that wraps a formatter for
 * 'TYPE' values.
 */
template<typename TYPE, typename PYFMT>
ref<Object> add_column(Table* self, Tuple* args, Dict* kw_args)
{
  static char const* arg_names[] = {"buf", "format", nullptr};
  Py_buffer buffer;
  PYFMT* format;
  Arg::ParseTupleAndKeywords(
      args, kw_args, "y*O!", arg_names, 
      &buffer, &PYFMT::type_, &format);

  if (buffer.ndim != 0)
    throw Exception(PyExc_TypeError, "not a one-dimensional array");
  if (buffer.itemsize != sizeof(TYPE))
    throw Exception(PyExc_TypeError, "wrong itemsize");

  using ColumnUptr = unique_ptr<fixfmt::Column>;
  using Column = fixfmt::ColumnImpl<TYPE, typename PYFMT::Formatter>;

  long const len = buffer.len / buffer.itemsize;
  self->table_->add_column(
    ColumnUptr(new Column((TYPE*) buffer.buf, len, *format->fmt_)));
  self->hold_buffer(std::move(buffer));
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


ref<Object> add_str_object_column(Table* self, Tuple* args, Dict* kw_args)
{
  static char const* arg_names[] = {"buf", "format", nullptr};
  Py_buffer buffer;
  String* format;
  Arg::ParseTupleAndKeywords(
      args, kw_args, "y*O!", arg_names,
      &buffer, &String::type_, &format);
  
  if (buffer.ndim != 0)
    throw Exception(PyExc_TypeError, "not a one-dimensional array");
  if (buffer.itemsize != sizeof(Object*))
    throw Exception(PyExc_TypeError, "wrong itemsize");

  using ColumnUptr = unique_ptr<fixfmt::Column>;

  long const len = buffer.len / buffer.itemsize;
  self->table_->add_column(ColumnUptr(
    new StrObjectColumn((Object**) buffer.buf, len, *format->fmt_)));
  self->hold_buffer(std::move(buffer));
  return none_ref();
}


auto methods = Methods<Table>()
  .add<add_string>                  ("add_string")
  .add<add_column<bool,    Bool>>   ("add_bool")
  .add<add_column<char,    Number>> ("add_int8")
  .add<add_column<short,   Number>> ("add_int16")
  .add<add_column<int,     Number>> ("add_int32")
  .add<add_column<long,    Number>> ("add_int64")
  .add<add_column<float,   Number>> ("add_float32")
  .add<add_column<double,  Number>> ("add_float64")
  .add<add_str_object_column>       ("add_str_object")
;


Object* get_length(Table* const self, void* /* closure */)
{
  return Long::FromLong(self->table_->get_length()).release();
}


Object* get_width(Table* const self, void* /* closure */)
{
  return Long::FromLong(self->table_->get_width()).release();
}


PyGetSetDef const tp_getset[] = {
  {
    (char*)     "length",                                   // name
    (getter)    get_length,                                 // get
    (setter)    nullptr,                                    // set
    (char*)     nullptr,                                    // doc
    (void*)     nullptr,                                    // closure
  },
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


Type Table::type_ = PyTypeObject{
  PyVarObject_HEAD_INIT(nullptr, 0)
  (char const*)         "fixfmt.Table",                     // tp_name
  (Py_ssize_t)          sizeof(Table),                      // tp_basicsize
  (Py_ssize_t)          0,                                  // tp_itemsize
  (destructor)          tp_dealloc,                         // tp_dealloc
  (printfunc)           nullptr,                            // tp_print
  (getattrfunc)         nullptr,                            // tp_getattr
  (setattrfunc)         nullptr,                            // tp_setattr
  (void*)               nullptr,                            // tp_reserved
  (reprfunc)            nullptr,                            // tp_repr
  (PyNumberMethods*)    nullptr,                            // tp_as_number
  (PySequenceMethods*)  &tp_as_sequence,                    // tp_as_sequence
  (PyMappingMethods*)   nullptr,                            // tp_as_mapping
  (hashfunc)            nullptr,                            // tp_hash
  (ternaryfunc)         wrap<Table, tp_call>,               // tp_call
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


