#pragma once

#include <cassert>
#include <iostream>
#include <string>

#include <Python.h>

using std::string;

//------------------------------------------------------------------------------

namespace py {

class Object;
class Unicode;

constexpr PyMethodDef METHODDEF_END{nullptr, nullptr, 0, nullptr};

//------------------------------------------------------------------------------

class Exception
{
public:

  Exception() {}

};


inline void check_return(int value)
{
  assert(value == 0 || value == -1);
  if (value != 0)
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

template<typename T>
class ref
{
public:

  // FIXME: Do we need this?
  static ref<T> take(T* obj)
    { return ref(obj); }

  static ref<T> take(PyObject* obj)
    { return ref(cast<T>(obj)); }

  static ref<T> of(T* obj)
    { incref(obj); return ref{obj}; }

  static ref<T> of(PyObject* obj)
    { return of(cast<T>(obj)); }

  ref()
    : obj_(nullptr) {}
  ref(ref<T>&& ref)
    : obj_(ref.release()) {}
  ~ref()
    { if (obj_ != nullptr) decref(obj_); }

  operator T*() const
    { return obj_; }

  T* operator->() const
    { return obj_; }

  T* release()
    { auto obj = obj_; obj_ = nullptr; return obj; }


private:

  ref(T* obj)
    : obj_(obj) {}

  T* obj_;

};


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


//------------------------------------------------------------------------------

class Object
  : public PyObject
{
public:

  static bool Check(PyObject* obj)
    { return true; }

  Object() = delete;
  ~Object() = delete;

  auto length()
    { return PyObject_Length(this); }
  auto repr()
    { return ref<Unicode>::take(PyObject_Repr(this)); }
  auto str()
    { return ref<Unicode>::take(PyObject_Str(this)); }

};


template<typename T>
inline std::ostream& operator<<(std::ostream& os, ref<T>& ref)
{
  os << ref->str()->as_utf8();
  return os;
}


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


//------------------------------------------------------------------------------

class Long
  : public Object
{
public:

  static bool Check(PyObject* obj)
    { return PyLong_Check(obj); }
  static auto create(long val)
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
    { check_return(PyModule_AddObject(this, name, incref(val))); }

  // FIXME: Deprecated; remove.
  void add(PyTypeObject* type)
  {
    // Extract the unqualified name.
    char const* name = strrchr(type->tp_name, '.');
    if (name == nullptr)
      name = type->tp_name;
    else
      ++name;
    AddObject(name, (PyObject*) type);
  }

  /**
   * Readies a type and adds it to this module.
   *
   * The type's name must be qualified by this module's name.  For example,
   * to add type 'Val' to module 'foo', the type's name must be 'foo.Val'.
   *
   * '*type' must have storage lifetime as long as the type is in the module,
   * generally static.
   */
  void ready_and_add_type(PyTypeObject* type)
  {
    // Make sure the qualified name of the type includes this module's name.
    string const qualname = type->tp_name;
    string const mod_name = PyModule_GetName(this);
    auto dot = qualname.find_last_of('.');
    assert(dot != string::npos);
    assert(qualname.compare(0, dot, mod_name) == 0);
    // Ready the type.
    int const result = PyType_Ready(type);
    assert(result == 0);
    unused(result);
    // Add it, under its unqualified name.
    AddObject(qualname.substr(dot + 1).c_str(), (PyObject*) type);
  }

};


//------------------------------------------------------------------------------

class ExtensionType
  : public Object
{
public:

  PyObject_HEAD

  static void Ready(PyTypeObject* type)
    { check_return(PyType_Ready(type)); }

};


//------------------------------------------------------------------------------

}  // namespace py


