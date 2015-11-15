Things to do:  (* for MVP)

* _cascading_ config for formatters
* category types
  - pass through to category array
  - pre-format categories (?)
* datetime types
- format methods, not just print
- np Array formatter
- add iterable to Table
- fixed-length string columns
- unsigned columns
- add exponential E+ formatter
- support UTF-8/ANSI in decimal point, sign, etc.

Dataframe features:
* show Pandas multiindex
- show nullable columns (object for bool, object for str, etc.)
- page at a time, with header

Fancy features:
- truncate dataframe output to screen
- rule-based coloring
- add % / scale to Number formatter
- add () for negative numbers to Number formatter
- add currency to Number formatter
- show summary statistics
- hex or other bases for Number formatter

Internal:
* Use iostream formatting for floats. (or implement properly!)
- Speed up _choose_formatter_* with extension code.  See branch guess_types_cxx.
- Add wrap<> for Python functions other than Method.
- Format into buffers of preallocated length (which must accommodate multibyte
  characters).

Cleanup:
* add accessors to formatters, C++ and Python
* banners
- Use wrapped methods consistently.
- check up on FIXMEs

