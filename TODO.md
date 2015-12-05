Things to do:  (* for MVP) 

- ** optionally, choose format for visible lines only
- _cascading_ config for formatters
- format methods, not just print
- np Array formatter
- add iterable to Table
- fixed-length string columns
- unsigned columns
- add exponential E+ formatter
- auto choose exponential E+ formatter
- support UTF-8/ANSI in decimal point, sign, etc.

Dataframe features:
- show nullable columns (object for bool, object for str, etc.)
- page at a time, with header

Fancy features:
- real datetime type
- monkey-patch DataFrame.print()
- truncate dataframe output to screen
- rule-based coloring
- add % / scale to Number formatter
- add () for negative numbers to Number formatter
- add currency to Number formatter
- show summary statistics
- hex or other bases for Number formatter
- print column offsets from some base instead of values

Internal:
- Add wrap<> for Python functions other than Method.
- Format into buffers of preallocated length (which must accommodate multibyte
  characters).
- category types
  - look up categories through codes
  - likewise for multiindex
  - pre-format categories (?)

Cleanup:
* docstrings 
* banners
* README docs
- Use wrapped methods consistently.
- check up on FIXMEs

