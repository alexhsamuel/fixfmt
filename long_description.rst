A C++ and Python library for formatting values (numbers, strings, etc.)
in fixed-width fields. Useful for printing tabular data and similar.

.. code:: py

    >>> fmt = fixfmt.String(10)
    >>> fmt("testing")
    'testing   '
    >>> fmt("Hello, world!")
    'Hello, wo…'

.. code:: py

    >>> fmt = fixfmt.Number(3, 3)
    >>> fmt(math.pi)
    '   3.142'

Features
========

-  A single formatter formats any value with the same width: useful for
   formatting tables, arrays, forms, etc.

-  The width is measured in Unicode code points.

-  `ANSI terminal escape
   codes <https://en.wikipedia.org/wiki/ANSI_escape_code>`__ are ignored
   when counting width, so they may be used to format output for the
   terminal.

APIs
====

See ```cxx/README.md`` <cxx/README.md>`__ for an introduction to the C++
API.

See ```python/README.md`` <python/README.md>`__ for an introduction to
the Python API.

Setup
=====

Requirements
------------

Requires a C++14 compiler, GNU Make, and setuptools to build.

Tested with Python 2.7 and Python 3.6 or later.

C++ unit tests
--------------

Requires `Google Test <https://github.com/google/googletest>`__ for
building and running the C++ tests:

At the top of the repository:

.. code:: sh

    git clone https://github.com/google/googletest
    (cd googletest/googletest/make; make)

Python unit tests
-----------------

Requires `pytest <http://pytest.org>`__ to run Python tests. Invoke,

``py.test test/``

Limitations
===========

Unicode
-------

`Counting code points is broken <http://utf8everywhere.org/>`__, as is
it is not a perfect proxy for display width.

However, for primarily data-oriented applications that this library
targets, it does a reasonable job and produces better output than most
similar systems. If you know a realible way to count grapheme clusters
or *invertibly* determine display width in a fixed-width Linux and OS/X
terminal, please contact the author.
