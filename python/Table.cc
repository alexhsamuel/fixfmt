#include <iostream>
#include <string>
#include <utility>

#include "Number.hh"
#include "Table.hh"

using namespace py;
using std::unique_ptr;

//------------------------------------------------------------------------------

namespace {

int tp_init(Table* self, PyObject* args, PyObject* kw_args)
{
  // No arguments.
  static char const* arg_names[] = {nullptr};
  if (! PyArg_ParseTupleAndKeywords(args, kw_args, "", (char**) arg_names)) 
    return -1;

  self->table_ = unique_ptr<fixfmt::Table>(new fixfmt::Table());
  return 0;
}


ref<Object> tp_call(Table* self, Object* args, Object* kw_args)
{
  static char const* arg_names[] = {"index", nullptr};
  long index;
  if (!PyArg_ParseTupleAndKeywords(
      args, kw_args, "l", (char**) arg_names, &index)) 
    throw Exception();

  long const width = self->table_->get_width();
  char buf[width];
  self->table_->format(index, buf);
  return Unicode::FromStringAndSize(buf, width);
}


ref<Object> add_string(Table* self, Object* args, Object* kw_args)
{
  static char const* arg_names[] = {"str", nullptr};
  char* str;
  if (!PyArg_ParseTupleAndKeywords(
      args, kw_args, "s", (char**) arg_names, &str)) 
    throw Exception();

  self->table_->add_string(std::string(str));
  return ref<Object>::of(Py_None);
}


ref<Object> add_float64(Table* self, Object* args, Object* kw_args)
{
  static char const* arg_names[] = {"buf", "format", nullptr};
  const char* buf;
  int buf_len;
  Number* format;
  if (!PyArg_ParseTupleAndKeywords(
      args, kw_args, "y#O!", (char**) arg_names, 
      &buf, &buf_len, &Number::type_, &format))
    throw Exception();

  unique_ptr<fixfmt::Column> col(
      new fixfmt::ColumnImpl<double, fixfmt::Number>(
        reinterpret_cast<double const*>(buf), *format->fmt_));
  self->table_->add_column(std::move(col));
  return ref<Object>::of(Py_None);
}


PyMethodDef const tp_methods[] = {
  {"add_string", 
   (PyCFunction) wrap_method<Table, add_string>, 
   METH_VARARGS | METH_KEYWORDS, 
   nullptr},
  {"add_float64", 
   (PyCFunction) wrap_method<Table, add_float64>, 
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


