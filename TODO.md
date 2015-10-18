Things to do:

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
- Format into buffers of preallocated length (which must accommodate multibyte
  characters).

Cleanup:
- check up on FIXMEs
- banners

