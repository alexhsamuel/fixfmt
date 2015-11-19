Fast double formatting implementation, based on [Florian Loitsch, "Printing
Floating-Point Numbers Quickly and Accurately with
Integers"](http://florian.loitsch.com/publications/dtoa-pldi2010.pdf).

This directory contains code taken from Google's
[double-conversion](https://github.com/google/double-conversion) project.  See
the source files for copyright and terms of use.

We've made the following local changes:

1. Disabled trimming of trailing zeros.

1. Replaced rounding up with [bankers
rounding](http://c2.com/cgi/wiki?BankersRounding).

1. Move the code into `namespace fixfmt::double_conversion`, to avoid collisions
with other instances of this code that may be linked in.

**FIXME:** We only use the fixed length double to string conversion, not the
shortest correct double conversion logic or the parsing logic.  These can be
stripped from the library.


