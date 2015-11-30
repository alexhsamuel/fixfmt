# C++ API

The C++ API provides fixed-width formatters for these types:

- `fixfmt::Number` for integer and floating-point numbers
- `fixfmt::String` for strings
- `fixfmt::Bool` for booleans

An instance of one of these classes represenets a formatter with fixed
configuration, and will always produce a string of the same width when
formatting any value.  Use the overloaded `operator()` to format a value.

## Booleans

For example, the `Bool` formatter with default arguments formats true as `True `
and false as `False`, padding the true value to five characters to match the
length of the false value.

```c++
fixfmt::Bool fmt;
std::cout << "[" << fmt(true) << "]\n"
          << "[" << fmt(false) << "]\n";
```

produces,

```
[True ]
[False]
```

Likewise,

```c++
fixfmt::Bool fmt("ABSOLUTELY", "no");
std::cout << "[" << fmt(true) << "]\n"
          << "[" << fmt(false) << "]\n";
```

produces,

```
[ABSOLUTELY]
[no        ]
```

The formatter's `get_width()` accessor returns the formatted width of any value.

The constructor's signature is,

```c++
Bool(string const& true_str, string const& false_str, size_t size, bool pad_left)
```

The `size` parameter allows you to specify a fixed width; otherwise the longer
of the length of `true_str` and `false_str` is used.  If `pad_left` is true, the
shorter string is padded on the left instead of on the right.

