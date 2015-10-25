#pragma once

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include <Python.h>

//------------------------------------------------------------------------------

namespace py {

class Long;
class Object;
class Unicode;

constexpr PyGetSetDef GETSETDEF_END
    {nullptr, nullptr, nullptr, nullptr, nullptr};

//------------------------------------------------------------------------------

class Exception
{
public:

  Exception() {}
  
  template<typename A>
  Exception(PyObject* exception, A&& message)
  {
    std::string msg(std::forward<A>(message));
    PyErr_SetString(exception, msg.c_str());
  }

  /**
   * Clears up the Python exception state.  
   */
  static void Clear() { PyErr_Clear(); }

};


/**
 * Raises 'Exception' if value is not zero.
 */
inline void check_zero(int value)
{
  assert(value == 0 || value == -1);
  if (value != 0)
    throw Exception();
}


/**
 * Raises 'Exception' if value is not true.
 */
inline void check_true(int value)
{
  if (value == 0)
    throw Exception();
}


//------------------------------------------------------------------------------

template<typename T>
inline T* cast(PyObject* obj)
{
  assert(T::Check(obj));  // FIXME: TypeError?
  return static_cast<T*>(obj);
}


//------------------------------------------------------------------------------

inline PyObject* incref(PyObject* obj)
{
  Py_INCREF(obj);
  return obj;
}


inline PyObject* decref(PyObject* obj)
{
  Py_DECREF(obj);
  return obj;
}


template<typename T>
class ref
{
public:

  /**
   * Takes an existing reference.
   *
   * Call this method on an object pointer that comes with an assumed reference,
   * such as the return value of an API call that returns ownership.
   */
  static ref<T> take(PyObject* obj)
    { return ref(cast<T>(obj)); }

  /**
   * Creates a new reference.
   */
  static ref<T> of(T* obj)
    { incref(obj); return ref{obj}; }

  /**
   * Creates a new reference, casting.
   */
  static ref<T> of(PyObject* obj)
    { return of(cast<T>(obj)); }

  /** 
   * Default ctor: null reference.  
   */
  ref()
    : obj_(nullptr) {}

  /** 
   * Move ctor.  
   */
  ref(ref<T>&& ref)
    : obj_(ref.release()) {}

  /** 
   * Move ctor from another ref type.  
   */
  template<typename U>
  ref(ref<U>&& ref)
    : obj_(ref.release()) {}

  ~ref()
    { clear(); }

  void operator=(ref<T>&& ref)
    { clear(); obj_ = ref.release(); }

  operator T*() const
    { return obj_; }

  T* operator->() const
    { return obj_; }

  T* release()
    { auto obj = obj_; obj_ = nullptr; return obj; }

  void clear()
    { if (obj_ != nullptr) decref(obj_); }

private:

  ref(T* obj)
    : obj_(obj) {}

  T* obj_;

};


inline ref<Object> none_ref()
{
  return ref<Object>::of(Py_None);
}


//==============================================================================

class Object
  : public PyObject
{
public:

  static bool Check(PyObject* obj)
    { return true; }

  auto Length()
    { return PyObject_Length(this); }
  auto Repr()
    { return ref<Unicode>::take(PyObject_Repr(this)); }
  auto Str()
    { return ref<Unicode>::take(PyObject_Str(this)); }

  ref<py::Long> Long();

  long long_value();

};


template<typename T>
inline std::ostream& operator<<(std::ostream& os, ref<T>& ref)
{
  os << ref->Str()->as_utf8();
  return os;
}


//------------------------------------------------------------------------------

class Dict
  : public Object
{
public:

  static bool Check(PyObject* obj)
    { return PyDict_Check(obj); }

};


//------------------------------------------------------------------------------

class Long
  : public Object
{
public:

  static bool Check(PyObject* obj)
    { return PyLong_Check(obj); }
  static auto FromLong(long val)
    { return ref<Long>::take(PyLong_FromLong(val)); }

  operator long()
    { return PyLong_AsLong(this); }

};


//------------------------------------------------------------------------------

class Module
  : public Object
{
public:

  static bool Check(PyObject* obj)
    { return PyModule_Check(obj); }
  static auto Create(PyModuleDef* def)
    { return ref<Module>::take(PyModule_Create(def)); }

  void AddObject(char const* name, PyObject* val)
    { check_zero(PyModule_AddObject(this, name, incref(val))); }

  void add(PyTypeObject* type)
  {
    // Make sure the qualified name of the type includes this module's name.
    std::string const qualname = type->tp_name;
    std::string const mod_name = PyModule_GetName(this);
    auto dot = qualname.find_last_of('.');
    assert(dot != std::string::npos);
    assert(qualname.compare(0, dot, mod_name) == 1);
    // Add it, under its unqualified name.
    AddObject(qualname.substr(dot + 1).c_str(), (PyObject*) type);
  }

};


//------------------------------------------------------------------------------

class Tuple
  : public Object
{
public:

  static bool Check(PyObject* obj)
    { return PyTuple_Check(obj); }

};


//------------------------------------------------------------------------------

class Type
  : public PyTypeObject
{
public:

  Type(PyTypeObject o) : PyTypeObject(o) {}

  void Ready()
    { check_zero(PyType_Ready(this)); }

};


//------------------------------------------------------------------------------

class Unicode
  : public Object
{
public:

  static bool Check(PyObject* obj)
    { return PyUnicode_Check(obj); }

  static auto FromString(char* utf8)
    { return ref<Unicode>::take(PyUnicode_FromString(utf8)); }
  // FIXME: Cast on const here?
  static auto FromStringAndSize(char* utf8, size_t length)
    { return ref<Unicode>::take(PyUnicode_FromStringAndSize(utf8, length)); }

  static auto from(std::string const& str)
    { return FromStringAndSize(const_cast<char*>(str.c_str()), str.length()); }

  char* as_utf8() { return PyUnicode_AsUTF8(this); }

  std::string as_utf8_string()
  {
    Py_ssize_t length;
    char* const utf8 = PyUnicode_AsUTF8AndSize(this, &length);
    if (utf8 == nullptr)
      throw Exception();
    else
      return std::string(utf8, length);
  }

};


template<>
inline std::ostream& operator<<(std::ostream& os, ref<Unicode>& ref)
{
  os << ref->as_utf8();
  return os;
}


//==============================================================================

inline ref<Long>
Object::Long()
{
  // FIXME: Check errors.
  return ref<py::Long>::take(PyNumber_Long(this));
}


inline long
Object::long_value()
{
  return (long) *Long();
}


//==============================================================================

namespace Arg {

inline void ParseTupleAndKeywords(
    Tuple* args, Dict* kw_args, 
    char const* format, char const* const* keywords, ...)
{
  va_list vargs;
  va_start(vargs, keywords);
  auto result = PyArg_VaParseTupleAndKeywords(
      args, kw_args, (char*) format, (char**) keywords, vargs);
  va_end(vargs);
  check_true(result);
}


}  // namespace Arg

//==============================================================================

class ExtensionType
  : public Object
{
public:

  PyObject_HEAD

};


//==============================================================================

// Buffer objects
// See https://docs.python.org/3/c-api/buffer.html.

/**
 * A unique reference to a buffer object.
 *
 * Supports move semantics only; no copy.
 */
class BufferRef
{
public:

  BufferRef(Py_buffer&& buffer)
    : buffer_(buffer)
  {}

  BufferRef(BufferRef&& ref)
    : buffer_(ref.buffer_)
  {
    ref.buffer_.obj = nullptr;
  }

  ~BufferRef()
  {
    // Only releases if buffer_.obj is not null.
    PyBuffer_Release(&buffer_);
    assert(buffer_.obj == nullptr);
  }

  BufferRef(BufferRef const&) = delete;
  void operator=(BufferRef const&) = delete;

private:

  Py_buffer buffer_;

};


//==============================================================================

template<typename CLASS>
using MethodPtr = ref<Object> (*)(CLASS* self, Tuple* args, Dict* kw_args);


/**
 * Wraps a method that takes args and kw_args and returns an object.
 */
template<typename CLASS, MethodPtr<CLASS> METHOD>
PyObject* wrap(PyObject* self, PyObject* args, PyObject* kw_args)
{
  ref<Object> result;
  try {
    result = METHOD(
      reinterpret_cast<CLASS*>(self),
      reinterpret_cast<Tuple*>(args),
      reinterpret_cast<Dict*>(kw_args));
  }
  catch (Exception) {
    return nullptr;
  }
  assert(result != nullptr);
  return result.release();
}


template<typename CLASS>
class Methods
{
public:

  Methods() : done_(false) {}

  template<MethodPtr<CLASS> METHOD>
  Methods& add(char const* name, char const* doc=nullptr)
  {
    assert(name != nullptr);
    assert(!done_);
    methods_.push_back({
      name,
      (PyCFunction) wrap<CLASS, METHOD>,
      METH_VARARGS | METH_KEYWORDS,
      doc
    });
    return *this;
  }

  operator PyMethodDef*()
  {
    if (!done_) {
      // Add the sentry.
      methods_.push_back({nullptr, nullptr, 0, nullptr});
      done_ = true;
    }
    return &methods_[0];
  }

private:

  bool done_;
  std::vector<PyMethodDef> methods_;

};


//------------------------------------------------------------------------------

}  // namespace py


