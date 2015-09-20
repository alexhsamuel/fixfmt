import builtins
import re
import shutil

import numpy as np
import pandas as pd
import pln.ctr

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
        if isinstance(regex, str) and re.match(regex, name) is not None:
            fmt = value
    if fmt is None:
        fmt = formatters.get(arr.dtype, None)
    if fmt is None:
        fmt = _get_default_formatter(arr)
    assert fmt is not None
    return fmt


def _table_for_dataframe(df, names, cfg={}):
    table = Table()

    formatters  = cfg.get("formatters", {})
    separators  = cfg.get("separators", {})
    begin_sep   = separators.get("begin", "") or ""
    sep         = separators.get("between", " ") or ""
    end_sep     = separators.get("end", "") or "" 

    if begin_sep:
        table.add_string(begin_sep)

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
            
    return table, fmts


# FIXME: By screen (repeating header?)
# FIXME: Index, with underlining.
# FIXME: Special sep after index.
# FIXME: Justify header columns.

# FIXME: Do what when it's too wide???

def _print_dataframe(df, cfg={}):
    # Slog through configuration.
    separators  = cfg.get("separators", {})
    begin_sep   = separators.get("begin", "") or ""
    sep         = separators.get("between", " ") or ""
    end_sep     = separators.get("end", "") or "" 
    max_rows    = cfg.get("max_rows", "terminal")
    if max_rows == "terminal":
        # FIXME
        max_rows = shutil.get_terminal_size().lines - 1
    row_ellipsis = cfg.get(
        "row_ellipsis", 
        lambda nr: "... skipping {} rows ...".format(nr))
    if not callable(row_ellipsis):
        row_ellipsis = lambda nr: row_ellipsis
    row_fraction = cfg.get("row_fraction", 0.85)
    show_header = cfg.get("header", True)
    underline = str(cfg.get("underline", "-"))
    if len(underline) != 1:
        raise ValueError("unerline must be one character")
    names = cfg.get("names", pln.ctr.ALL)

    names = pln.ctr.select_ordered(df.columns, names)
    # FIXME: Get formats from table columns.
    table, fmts = _table_for_dataframe(df, names, cfg)

    if show_header:
        header = []
        for name, fmt in zip(names, fmts):
            # FIXME: Palide.
            width = fmt.width
            # FIXME: Choose just.
            header.append(name[: width].rjust(width))
        # FIXME: Is it right to use spaces?
        builtins.print(
              " " * len(begin_sep)
            + (" " * len(sep)).join(header)
            + " " * len(end_sep)
        )
        builtins.print(
              " " * len(begin_sep)
            + (" " * len(sep)).join( underline * len(h) for h in header )
            + " " * len(end_sep)
        )

    num_rows = len(table)
    if num_rows <= max_rows:
        for i in range(len(table)):
            builtins.print(table(i))
    else:
        extra_rows          = 1 + (2 if show_header else 0)
        num_rows_top        = int(row_fraction * max_rows)
        num_rows_bottom     = max_rows - extra_rows - num_rows_top
        num_rows_skipped    = num_rows - num_rows_top - num_rows_bottom
        for i in range(num_rows_top):
            builtins.print(table(i))
        builtins.print(row_ellipsis(num_rows_skipped).center(table.width))
        for i in range(num_rows - num_rows_bottom, num_rows):
            builtins.print(table(i))


def print(df, **cfg):
    _print_dataframe(df, cfg)


