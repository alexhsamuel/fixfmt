import builtins
from   math import floor, log10
import re
import shutil
import sys

import numpy as np
import pandas as pd
import pln.ctr

from   . import *

#-------------------------------------------------------------------------------

# FIXME: We need a proper cascading configuration system.

# FIXME: Make hierarchical.
DEFAULT_CFG = {
    "float.min_precision"       : 1,
    "float.max_precision"       : 8,

    "formatters"                : {},

    "separator.between"         : " ",
    "separator.end"             : None,
    "separator.index"           : " | ",
    "separator.start"           : None,

    "str.min_size"              :  1,
    "str.max_size"              : 32,
}

def _add_array_to_table(table, arr, fmt):
    dtype = arr.dtype
    if dtype == np.dtype("int8"):
        table.add_int8(arr, fmt)
    elif dtype == np.dtype("int16"):
        table.add_int16(arr, fmt)
    elif dtype == np.dtype("int32"):
        table.add_int32(arr, fmt)
    elif dtype == np.dtype("int64"):
        builtins.print("adding int64")
        table.add_int64(arr, fmt)
        builtins.print("added int64")
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


def _get_num_digits(value):
    """
    Returns the number of digits required to represent a value.
    """
    return 1 if value == 0 else max(int(floor(log10(abs(value)) + 1)), 1)


def _choose_formatter_int(values, cfg):
    if len(values) == 0:
        return Number(1, None, sign=" ")

    min_val = values.min()
    max_val = values.max()
    size    = _get_num_digits(max(abs(min_val), abs(max_val)))
    sign    = " " if min_val >= 0 else "-"
    return Number(size, sign=sign)


def _choose_formatter_float(values, cfg):
    # FIXME: Consider width of NaN and infinite strings.
    # FIXME: This could be done with a single pass more efficiently.

    # Remove NaN and infinite values.
    is_nan  = np.isnan(values)
    has_nan = is_nan.any()
    is_inf  = np.isinf(values)
    has_inf = is_inf.any()
    vals    = values[~(is_nan | is_inf)]
    
    if len(vals) == 0:
        # No values left.
        return Float(1, 1)

    # Determine the scale.
    min_val = vals.min()
    max_val = vals.max()
    sign    = " " if min_val >= 0 else "-"
    size    = _get_num_digits(max(abs(min_val), abs(max_val)))

    # Try progressively higher precision until rounding won't leave any
    # residuals larger the maximum pecision.
    precision_min   = cfg["float.min_precision"]
    precision_max   = cfg["float.max_precision"]
    tolerance       = (10 ** -precision_max) / 2
    for precision in range(precision_min, precision_max + 1):
        if (abs(np.round(vals, precision) - vals) < tolerance).all():
            break

    return Number(size, precision, sign=sign)


def _choose_formatter_str(values, cfg):
    size = np.vectorize(len)(values).max()
    size = max(size, cfg["str.min_size"])
    size = min(size, cfg["str.max_size"])
    return String(size)


def _get_default_formatter(arr, cfg):
    # FIXME: Choose sizes better.
    dtype = arr.dtype
    if dtype.kind == "i":
        return _choose_formatter_int(arr, cfg=cfg)
    elif dtype.kind == "f":
        return _choose_formatter_float(arr, cfg=cfg)
    elif dtype.kind == "b":
        return Bool()
    elif dtype.kind == "O":
        return _choose_formatter_str(arr, cfg=cfg)
    else:
        raise TypeError("no default formatter for {}".format(dtype))


def _get_formatter(formatters, name, arr, cfg):
    fmt = None
    for regex, value in formatters.items():
        if isinstance(regex, str) and re.match(regex, name) is not None:
            fmt = value
    if fmt is None:
        fmt = formatters.get(arr.dtype, None)
    if fmt is None:
        fmt = _get_default_formatter(arr, cfg=cfg)
    assert fmt is not None
    return fmt


def _table_for_dataframe(df, names, cfg={}):
    table = Table()

    formatters  = cfg["formatters"]
    begin_sep   = cfg["separator.start"] or ""
    sep         = cfg["separator.between"] or ""
    end_sep     = cfg["separator.end"] or "" 

    if begin_sep:
        table.add_string(begin_sep)

    fmts = []
    arrs = []
    for name in names:
        arr = df[name].values
        arrs.append(arr)

        if arr.dtype.kind == "O":
            # Objects are stringified.  Do this now.
            arr = np.vectorize(str, [object])(arr)

        fmt = _get_formatter(formatters, name, arr, cfg)
        # FIXME: Add accessor to table.
        fmts.append(fmt)
        _add_array_to_table(table, arr, fmt)
        if sep:
            table.add_string(sep)

    if end_sep:
        table.add_string(end_sep)
            
    return table, fmts, arrs


# FIXME: By screen (repeating header?)
# FIXME: Index, with underlining.
# FIXME: Special sep after index.
# FIXME: Justify header columns.

# FIXME: Do what when it's too wide???

def _print_dataframe(df, cfg):
    # Slog through configuration.
    formatters  = cfg["formatters"]
    begin_sep   = cfg["separator.start"] or ""
    sep         = cfg["separator.between"] or ""
    end_sep     = cfg["separator.end"] or "" 
    max_rows    = cfg.get("max_rows", "terminal")  # FIXME

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
    table, fmts, arrs = _table_for_dataframe(df, names, cfg)

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


def print(df, **kw_args):
    cfg = dict(DEFAULT_CFG)
    cfg.update(kw_args)
    _print_dataframe(df, cfg)


#-------------------------------------------------------------------------------

def main():
    from   argparse import ArgumentParser
    from   pln.io import load_pickle

    parser = ArgumentParser()
    parser.add_argument(
        "filename", metavar="FILENAME", 
        help="read from FILENAME")
    args = parser.parse_args()
    
    # FIXME: Support "-".
    df = load_pickle(args.filename)
    # df = df[["i", "x", "y", "s", "b", ]]
    builtins.print(df.dtypes)
    print(df)


if __name__ == "__main__":
    main()


