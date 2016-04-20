To use the Python API, add this directory (`.../fixfmt/python/`) to your
`PYTHONPATH`.

# API

The Python API is a wrapper around the [C++ API](../cxx/README.md).  For
example, 

```python
>>> import fixfmt
>>> fmt = fixfmt.Bool()
>>> fmt(True)
'true '
>>> fmt(False)
'false'
```

```python
>>> fmt = fixfmt.String(10)
>>> fmt("testing")
'testing   '
>>> fmt("Hello, world!")
'Hello, wo…'
```

```python
>>> fmt = fixfmt.Number(3, 3)
>>> fmt(math.pi)
'   3.142'
```

## Pandas

`fixfmt.pandas.print_dataframe()` provides a replacement for Pandas's built-in
dataframe formatting routines.  

```python
>>> print(df)
            label     value
0              pi  3.141590
1    negative one -1.000000
2  Euler's number  2.718282
>>> fixfmt.pandas.print_dataframe(df)
     | label               value
==== | ============== ==========
   0 | pi              3.1415900
   1 | negative one   -1.0000000
   2 | Euler's number  2.7182818
```

This function takes a second configuration argument, which may be customized to
adjust most aspects of the formatting.  For example, an alternate configuration
uses [Unicode box drawing characters](http://unicode.org/charts/PDF/U2500.pdf)
to surround the table.

```
>>> fixfmt.pandas.print_dataframe(df, fixfmt.table.UNICODE_BOX_CFG)
┌──────╥────────────────┬────────────┐
│      ║ label          │      value │
├──────╫────────────────┼────────────┤
│    0 ║ pi             │  3.1415900 │
│    1 ║ negative one   │ -1.0000000 │
│    2 ║ Euler's number │  2.7182818 │
└──────╨────────────────┴────────────┘
```

