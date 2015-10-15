Things to do:

- proper Unicode support in formatters
- Unicode support in pad/elide/palide
- rename Number -> PyNumber, etc.
- support UTF-8 ellipsis etc. (get strlen right)
- category types
- datetime types
- np Array formatter
- add iterable to Table
- fixed-length string columns
- unsigned columns
- add % / scale to Number formatter
- add exponential E+ formatter

Fancy features:
- use more fancy Unicode characters
- ANSI escapes
- truncate dataframe output to screen
- rule-based coloring

Internal:
- use char* more? or some kind of string builder?
