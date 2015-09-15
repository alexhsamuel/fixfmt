import builtins
import re

import numpy as np
import pandas as pd

from   . import *

#-------------------------------------------------------------------------------

# FIXME: We need a proper cascading configuration system.

# DEFAULT_CFG = {
#     "formatters": {},
#     "separators": {
#         "start"     : None,
#         "end"       : None,
#         "between"   : " ",
#         "index"     : " | ",
#     },
# }

def _add_array_to_table(table, arr, fmt):
    dtype = arr.dtype
    if dtype == np.dtype("int8"):
        table.add_int8(arr, fmt)
    elif dtype == np.dtype("int16"):
        table.add_int16(arr, fmt)
    elif dtype == np.dtype("int32"):
        table.add_int32(arr, fmt)
    elif dtype == np.dtype("int64"):
        table.add_int64(arr, fmt)
    elif dtype == np.dtype("float32"):
        table.add_float32(arr, fmt)
    elif dtype == np.dtype("float64"):
        table.add_float64(arr, fmt)
    elif dtype == np.dtype("bool"):
        table.add_bool(arr, fmt)
    elif dtype == np.dtype("object"):
        table.add_str_object(arr, fmt)
    else:
        raise TypeError("unsupported dtype: {}".format(dtype))


def _get_default_formatter(arr):
    # FIXME: Choose sizes better.
    dtype = arr.dtype
    if dtype.kind == "i":
        return Number(6)
    elif dtype.kind == "f":
        return Number(6, 6)
    elif dtype.kind == "b":
        return Bool()
    elif dtype.kind == "O":
        return String(16)
    else:
        raise TypeError("no default formatter for {}".format(dtype))


def _get_formatter(formatters, name, arr):
    fmt = None
    for regex, value in formatters.items():
        if re.match(key, name) is not None:
            fmt = value
    if fmt is None:
        fmt = formatters.get(arr.dtype, None)
    if fmt is None:
        fmt = _get_default_formatter(arr)
    assert fmt is not None
    return fmt


def _table_for_dataframe(df, cfg={}):
    formatters = cfg.get("formatters", {})
    separators = cfg.get("separators", {})
    show_header = cfg.get("header", True)

    table = Table()

    begin_sep   = separators.get("begin", "") or ""
    sep         = separators.get("between", " ") or ""
    end_sep     = separators.get("end", "") or "" 

    if begin_sep:
        table.add_string(begin_sep)

    names = df.columns
    fmts = []
    for name in names:
        arr = df[name].values
        fmt = _get_formatter(formatters, name, arr)
        # FIXME: Add accessor to table.
        fmts.append(fmt)
        _add_array_to_table(table, arr, fmt)
        if sep:
            table.add_string(sep)

    if end_sep:
        table.add_string(end_sep)
            
    if show_header:
        header = []
        for name, fmt in zip(names, fmts):
            # FIXME: Palide.
            width = fmt.width
            # FIXME: Choose just.
            header.append(name[: width].rjust(width))
        builtins.print(
              " " * len(begin_sep)
            + (" " * len(sep)).join(header)
            + " " * len(end_sep)
        )
        builtins.print(
              " " * len(begin_sep)
            + (" " * len(sep)).join( "-" * len(h) for h in header )
            + " " * len(end_sep)
        )


    return table


def print(df, **cfg):
    table = _table_for_dataframe(df, cfg)
    for i in range(len(table)):
        builtins.print(table(i))


