# C++ API

The C++ API provides fixed-width formatters for these types:

- `fixfmt::Bool` for booleans
- `fixfmt::String` for strings
- `fixfmt::Number` for integer and floating-point numbers

An instance of one of these classes represenets a formatter with fixed
configuration, and will always produce a string of the same width when
formatting any value.  Use the overloaded `operator()` to format a value.

All formatters have a `get_width()` accessor, which returns the formatted width
of any value formatted by that formatter.


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

The constructor's signature is,

```c++
Bool(string const& true_str, string const& false_str, size_t size, bool pad_left)
```

The `size` parameter allows you to specify a fixed width; otherwise the longer
of the length of `true_str` and `false_str` is used.  If `pad_left` is true, the
shorter string is padded on the left instead of on the right.


## Strings

The string formatter formats a string to a fixed width either by _padding_ to
lengthen or _eliding_ to shorten.  Padding consists of adding characters either
to the left or right of the string.  Eliding consists of replacing a sequence of
characters in the beginning, middle, or end of the string with a shorter
_ellipsis_.  

```c++
fixfmt::String fmt(10);
std::cout << "[" << fmt("testing") << "]\n"
          << "[" << fmt("Hello, world!") << "]\n";
```

produces,

```
[testing   ]
[Hello, wo…]
```

Observe that the first string has been padded on the right with three spaces to
achieve a length of 10, while in the second string, the last four characters
have been replaced with a single character, a Unicode ellipsis ("three
periods").  

The constructor's signature is,

```c++
String(int size, string ellipsis, string pad, double position, bool pad_left)
```

`ellipsis` is the ellipsis used for shortening, while `pad` is the pad used for
lengthening.  `position` is the position in the string at which the elision
occurs: 0.0 for the left, 1.0 for the right, or an intermediate number for
elsewhere in the string.  


# Numbers

The number formatter formats a number with a fixed number of digits for each of
the integral and fractional parts, called the "size" and "precision"
respectively.

```c++
double const x = 123.45678;
std::cout << "[" << fixfmt::Number(5, 1)(x) << "]\n"
          << "[" << fixfmt::Number(3, 3)(x) << "]\n";
```

produces

```
[   123.5]
[ 123.457]
```

If the precision is 0, the decimal point is included but without any fractional
digits.  If the precision is `Number::PRECISION_NONE`, the decimal point is
omitted and the number is formatted as an integer.

```c++
std::cout << "[" << fixfmt::Number(5)(x) << "]\n";
```

produces

```
[   123]
```

The constructor's signature is,

```c++
Number(int size, int precision, char pad, char sign, string nan, string inf, char point, char bad)
```

`pad` is the character used to pad the integral part to the size, and may be
either `Number::PAD_SPACE` or `Number::PAD_ZERO`.  

`sign` determines how the sign is presented: 

- `Number::SIGN_NEGATIVE` (the default) reserves one position for the sign, and
  displays a space for positive numbers, a minus sign for negative numbers.  
- `Number::SIGN_ALWAYS` is similar except that a plus sign is shown for positive
  numbers. 
- `Number::SIGN_NONE` does not reserve a position for the sign; negative numbers
  cannot be formatted.

`nan` and `inf` are the renderings for NaN and infinite values, respectively.

`point` is the character used for the decimal point.

`bad` is a single character that is used to fill the entire width if the number
cannot be formatted, for instance if its integral part exceeds the capacity of
the format size.

