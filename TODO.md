Things to do:

- Use cfg in fixfmt.pandas.

- rename Number -> PyNumber, etc.
- category types
- datetime types
- np Array formatter
- add iterable to Table
- fixed-length string columns
- unsigned columns
- add % / scale to Number formatter
- add exponential E+ formatter
- add accessors to formatters, C++ and Python

Dataframe features:
- print index
- print row numbers

Fancy features:
- use more fancy Unicode characters
- ANSI escapes
- truncate dataframe output to screen
- rule-based coloring

Internal:
- Speed up _choose_formatter_* with extension code.  See branch guess_types_cxx.
- Add wrap<> for Python functions other than Method.
- Format into buffers of preallocated length (which must accommodate multibyte
  characters).

Cleanup:
- Use wrapped methods consistently.
- check up on FIXMEs
- banners

