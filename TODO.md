Things to do:

- page at a time, with header
- format methods, not just print
- Use cfg in fixfmt.pandas.
- category types
  - pass through to category array
  - pre-format categories (?)
- datetime types
- np Array formatter
- add iterable to Table
- fixed-length string columns
- unsigned columns
- add exponential E+ formatter

Dataframe features:
- show nullable columns (object for bool, object for str, etc.)
- NaN treatment
- show Pandas index
- show Pandas multiindex
- print row numbers

Fancy features:
- truncate dataframe output to screen
- rule-based coloring
- add % / scale to Number formatter
- add () for negative numbers to Number formatter
- add currency to Number formatter
- show summary statistics

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

