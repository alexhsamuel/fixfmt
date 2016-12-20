A C++ and Python library for formatting values (numbers, strings, etc.) in
fixed-width fields.  Useful for printing tabular data and similar.

# Features

- A single formatter formats any value with the same width: useful for 
  formatting tables, arrays, forms, etc.
- The width is measured in Unicode code points: supports UTF-8 properly
  (but not combining characters and other fancy features).
- [ANSI terminal escape codes](https://en.wikipedia.org/wiki/ANSI_escape_code)
  are ignored when counting width, so they may be used to format output
  for the terminal.

# APIs

See [`cxx/README.md`](cxx/README.md) for an introduction to the C++ API.

See [`python/README.md`](python/README.md) for an introduction to the Python API.

# Setup

## C++

Requires a C++14 compiler.  (Maybe one day I'll try to rewrite it in Rust.)

## C++ unit tests

Requires [Google Test](https://github.com/google/googletest) for building
and running the C++ tests:

At the top of the repository:
```sh
git clone https://github.com/google/googletest
(cd googletest/googletest/make; make)
```

## Python

Requires Python 3.4 or later.


## Python unit tests

Requires [pytest](http://pytest.org) to run Python tests.  Invoke,

`py.test test/`
