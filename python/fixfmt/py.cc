#include "py.hh"

using namespace py;

//------------------------------------------------------------------------------

auto ref<Bool> 
Bool::TRUE = ref<Bool>::of(Py_True);

auto ref<Bool>
Bool::FALSE = ref<Bool>::of(Py_False);

