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

int tp_init(Table* self, Tuple* args, Dict* kw_args)
{
  // No arguments.
  static char const* arg_names[] = {nullptr};
  Arg::ParseTupleAndKeywords(args, kw_args, "", (char**) arg_names);

  self->table_ = unique_ptr<fixfmt::Table>(new fixfmt::Table());
  return 0;
}


ref<Object> tp_call(Table* self, Tuple* args, Dict* kw_args)
{
  static char const* arg_names[] = {"index", nullptr};
  long index;
  Arg::ParseTupleAndKeywords(args, kw_args, "l", arg_names, &index);

  long const width = self->table_->get_width();
  char buf[width];
  self->table_->format(index, buf);
  return Unicode::FromStringAndSize(buf, width);
}


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
  const char* buf;
  int buf_len;
  PYFMT* format;
  Arg::ParseTupleAndKeywords(
      args, kw_args, "y#O!", arg_names, 
      &buf, &buf_len, &PYFMT::type_, &format);

  unique_ptr<fixfmt::Column> col(
      new fixfmt::ColumnImpl<TYPE, typename PYFMT::Formatter>(
        reinterpret_cast<TYPE const*>(buf), *format->fmt_));
  self->table_->add_column(std::move(col));
  return none_ref();
}


PyMethodDef const tp_methods[] = {
  {"add_string", 
   (PyCFunction) wrap_method<Table, add_string>, 
   METH_VARARGS | METH_KEYWORDS, 
   nullptr},
  {"add_bool",
   (PyCFunction) wrap_method<Table, add_column<bool, Bool>>,
   METH_VARARGS | METH_KEYWORDS,
   nullptr},
  {"add_int8",
   (PyCFunction) wrap_method<Table, add_column<char, Number>>,
   METH_VARARGS | METH_KEYWORDS,
   nullptr},
  {"add_int16",
   (PyCFunction) wrap_method<Table, add_column<short, Number>>,
   METH_VARARGS | METH_KEYWORDS,
   nullptr},
  {"add_int32",
   (PyCFunction) wrap_method<Table, add_column<int, Number>>,
   METH_VARARGS | METH_KEYWORDS,
   nullptr},
  {"add_int64",
   (PyCFunction) wrap_method<Table, add_column<long, Number>>,
   METH_VARARGS | METH_KEYWORDS,
   nullptr},
  {"add_float32", 
   (PyCFunction) wrap_method<Table, add_column<float, Number>>, 
   METH_VARARGS | METH_KEYWORDS, 
   nullptr},
  {"add_float64", 
   (PyCFunction) wrap_method<Table, add_column<double, Number>>, 
   METH_VARARGS | METH_KEYWORDS, 
   nullptr},
  METHODDEF_END
};


}  // anonymous namespace


Type Table::type_ = PyTypeObject{
  PyVarObject_HEAD_INIT(nullptr, 0)
  "fixfmt.Table",              // tp_name
  sizeof(Table),               // tp_basicsize
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
  (ternaryfunc) wrap_method<Table, tp_call>,
                                // tp_call
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


