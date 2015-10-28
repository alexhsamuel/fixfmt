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
# FIXME: Don't support None; it's silly.
DEFAULT_CFG = {
    "bottom.line"                   : "-",
    "bottom.separator.between"      : " ",
    "bottom.separator.end"          : "",
    "bottom.separator.index"        : "  ",
    "bottom.separator.start"        : "",
    "bottom.show"                   : False,

    "float.min_precision"           : 1,
    "float.max_precision"           : 8,

    "formatters"                    : {},

    "header.separator.between"      : None,
    "header.separator.end"          : None,
    "header.separator.index"        : None,
    "header.separator.start"        : None,
    "header.show"                   : True,

    "separator.between"             : " ",
    "separator.end"                 : "",
    "separator.index"               : " | ",
    "separator.start"               : "",

    "str.min_size"                  :  1,
    "str.max_size"                  : 32,

    "top.line"                      : "-",
    "top.separator.between"         : " ",
    "top.separator.end"             : "",
    "top.separator.index"           : "  ",
    "top.separator.start"           : "",
    "top.show"                      : False,

    "underline.line"                : "=",
    "underline.separator.between"   : None,
    "underline.separator.end"       : None,
    "underline.separator.index"     : None,
    "underline.separator.start"     : None,
    "underline.show"                : True,
}

UNICODE_BOX_CFG = {
    "bottom.line"                   : "\u2500",
    "bottom.separator.between"      : "\u2500\u2534\u2500",
    "bottom.separator.end"          : "\u2500\u2518",
    "bottom.separator.index"        : "\u2500\u2534\u2500",
    "bottom.separator.start"        : "\u2514\u2500",
    "bottom.show"                   : True,
    "separator.between"             : " \u2502 ",
    "separator.end"                 : " \u2502",
    "separator.index"               : " \u2551 ",
    "separator.start"               : "\u2502 ",
    "top.line"                      : "\u2500",
    "top.separator.between"         : "\u2500\u252c\u2500",
    "top.separator.end"             : "\u2500\u2510",
    "top.separator.index"           : "\u2500\u252c\u2500",
    "top.separator.start"           : "\u250c\u2500",
    "top.show"                      : True,
    "underline.line"                : "\u2500",
    "underline.separator.between"   : "\u2500\u253c\u2500",
    "underline.separator.end"       : "\u2500\u2524",
    "underline.separator.index"     : " \u253c ",
    "underline.separator.start"     : "\u251c\u2500",
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


def _get_num_digits(value):
    """
    Returns the number of digits required to represent a value.
    """
    return 1 if value == 0 else max(int(floor(log10(abs(value)) + 1)), 1)


def _choose_formatter_bool(values, cfg):
    # Not much to do.
    return Bool()


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


def _choose_formatter_int(values, cfg):
    if len(values) == 0:
        return Number(1, None, sign=" ")

    min_val = values.min()
    max_val = values.max()
    size    = _get_num_digits(max(abs(min_val), abs(max_val)))
    sign    = " " if min_val >= 0 else "-"
    return Number(size, sign=sign)


def _choose_formatter_str(values, cfg):
    size = np.vectorize(len)(values).max()
    size = max(size, cfg["str.min_size"])
    size = min(size, cfg["str.max_size"])
    return String(size)


def _get_default_formatter(arr, cfg):
    dtype = arr.dtype
    if dtype.kind == "b":
        return _choose_formatter_bool(arr, cfg=cfg)
    elif dtype.kind == "f":
        return _choose_formatter_float(arr, cfg=cfg)
    elif dtype.kind == "i":
        return _choose_formatter_int(arr, cfg=cfg)
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
    begin_sep   = cfg["separator.start"]
    sep         = cfg["separator.between"]
    end_sep     = cfg["separator.end"]

    if begin_sep:
        table.add_string(begin_sep)

    fmts = []
    arrs = []
    for i, name in enumerate(names):
        arr = df[name].values
        arrs.append(arr)

        if arr.dtype.kind == "O":
            # Objects are stringified.  Do this now.
            arr = np.vectorize(str, [object])(arr)

        fmt = _get_formatter(formatters, name, arr, cfg)
        # FIXME: Add accessor to table.
        fmts.append(fmt)
        _add_array_to_table(table, arr, fmt)
        if i == len(names) - 1:
            if end_sep:
                table.add_string(end_sep)
        else:
            if sep:
                table.add_string(sep)

            
    return table, fmts, arrs


# FIXME: By screen (repeating header?)
# FIXME: Index, with underlining.
# FIXME: Special sep after index.
# FIXME: Justify header columns.

def _print_header(names, fmts, cfg):
    if cfg["header.show"]:
        begin_sep   = cfg["header.separator.start"]
        if begin_sep is None:
            begin_sep = cfg["separator.start"]
        sep         = cfg["header.separator.between"]
        if sep is None:
            sep     = cfg["separator.between"]
        end_sep     = cfg["header.separator.end"]
        if end_sep is None:
            end_sep = cfg["separator.end"]

        header = [
            # FIXME: Palide.
            # FIXME: Choose just.
            n[: f.width].rjust(f.width)
            for n, f in zip(names, fmts)
        ]
        builtins.print(begin_sep + sep.join(header) + end_sep)


# FIXME: Support subconfigs.
def _print_line(names, fmts, prefix, cfg):
    if cfg[prefix + ".show"]:
        begin_sep   = cfg[prefix + ".separator.start"]
        sep         = cfg[prefix + ".separator.between"]
        end_sep     = cfg[prefix + ".separator.end"]
        line        = cfg[prefix + ".line"]

        # FIXME: Relax this.
        if len(line) != 1:
            raise ValueError("{}.line must be one character".format(prefix))

        builtins.print(
              begin_sep
            + sep.join( line * f.width for f in fmts )
            + end_sep
        )


# FIXME: Do what when it's too wide???

def _print_dataframe(df, cfg):
    # Slog through configuration.
    formatters  = cfg["formatters"]
    max_rows    = cfg.get("max_rows", "terminal")  # FIXME

    if max_rows == "terminal":
        # FIXME
        max_rows = shutil.get_terminal_size().lines - 1
    # FIXME: Configuration and line drawing!
    row_ellipsis = cfg.get(
        "row_ellipsis", 
        lambda nr: "... skipping {} rows ...".format(nr))
    if not callable(row_ellipsis):
        row_ellipsis = lambda nr: row_ellipsis
    row_fraction = cfg.get("row_fraction", 0.85)
    names = cfg.get("names", pln.ctr.ALL)

    names = pln.ctr.select_ordered(df.columns, names)
    # FIXME: Get formats from table columns.
    table, fmts, arrs = _table_for_dataframe(df, names, cfg)

    _print_line(names, fmts, "top", cfg)
    _print_header(names, fmts, cfg)
    _print_line(names, fmts, "underline", cfg)

    num_rows = len(table)
    if num_rows <= max_rows:
        for i in range(len(table)):
            builtins.print(table(i))
    else:
        extra_rows          = sum([
            1,
            cfg["top.show"],
            cfg["header.show"],
            cfg["underline.show"],
            cfg["bottom.show"],
        ])
        num_rows_top        = int(row_fraction * max_rows)
        num_rows_bottom     = max_rows - extra_rows - num_rows_top
        num_rows_skipped    = num_rows - num_rows_top - num_rows_bottom
        for i in range(num_rows_top):
            builtins.print(table(i))
        builtins.print(row_ellipsis(num_rows_skipped).center(table.width))
        for i in range(num_rows - num_rows_bottom, num_rows):
            builtins.print(table(i))
    _print_line(names, fmts, "bottom", cfg)


def print(df, *, cfg=DEFAULT_CFG):
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
    
    # FIXME
    cfg = DEFAULT_CFG.copy()
    cfg.update(UNICODE_BOX_CFG)

    # FIXME: Support "-".
    df = load_pickle(args.filename)
    print(df, cfg=cfg)


if __name__ == "__main__":
    main()


