Things to do:

- format methods, not just print
- Use cfg in fixfmt.pandas.
- category types
- datetime types
- np Array formatter
- add iterable to Table
- fixed-length string columns
- unsigned columns
- add % / scale to Number formatter
- add exponential E+ formatter

Dataframe features:
- show Pandas index
- show Pandas multiindex
- print row numbers

Fancy features:
- truncate dataframe output to screen
- rule-based coloring

Internal:
- Speed up _choose_formatter_* with extension code.  See branch guess_types_cxx.
- Add wrap<> for Python functions other than Method.
- Format into buffers of preallocated length (which must accommodate multibyte
  characters).

Cleanup:
- rename Number -> PyNumber, etc.
- add accessors to formatters, C++ and Python
- Use wrapped methods consistently.
- check up on FIXMEs
- banners

