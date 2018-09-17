A C++ and Python library for formatting values (numbers, strings, etc.) in
fixed-width fields.  Useful for printing tabular data and similar.

[![Build Status](https://travis-ci.org/alexhsamuel/fixfmt.svg?branch=master)](https://travis-ci.org/alexhsamuel/fixfmt)


# Features

- A single formatter formats any value with the same width: useful for 
  formatting tables, arrays, forms, etc.

- The width is measured in Unicode code points.

- [ANSI terminal escape codes](https://en.wikipedia.org/wiki/ANSI_escape_code)
  are ignored when counting width, so they may be used to format output
  for the terminal.
  

# APIs

See [`cxx/README.md`](cxx/README.md) for an introduction to the C++ API.

See [`python/README.md`](python/README.md) for an introduction to the Python API.


# Installing

With conda (binary packages for Linux and OS/X, Python 3.6):

```
conda install -c alexhsamuel fixfmt
```

From PyPI (a requires C++14 compiler and GNU make):

```
pip install fixfmt
```


# Building

Requires a C++14 compiler, GNU Make, and setuptools to build.

Tested with Python 3.6.


## C++ unit tests

Requires [Google Test](https://github.com/google/googletest) for building
and running the C++ tests:

At the top of the repository:
```sh
git clone https://github.com/google/googletest
(cd googletest/googletest/make; make)
```


## Python unit tests

Requires [pytest](http://pytest.org) to run Python tests.  Invoke,

```
py.test test
```


# Limitations

[Counting code points is broken](http://utf8everywhere.org/), as is it is not a
perfect proxy for display width.

However, for primarily data-oriented applications that this library targets,
it does a reasonable job and produces better output than most similar systems.
If you know a realible way to count grapheme clusters or _invertibly_ determine 
display width in a fixed-width Linux and OS/X terminal, please contact the author.

