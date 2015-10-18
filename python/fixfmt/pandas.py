import builtins
from   math import floor, log10
import re
import shutil

import numpy as np
import pandas as pd
import pln.ctr

from   . import *

#-------------------------------------------------------------------------------

# FIXME: We need a proper cascading configuration system.

DEFAULT_CFG = {
    # "formatters": {},
    # "separators": {
    #     "start"     : None,
    #     "end"       : None,
    #     "between"   : " ",
    #     "index"     : " | ",
    # },

    "float.min_precision": 1,
    "float.max_precision": 8,
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


def _get_default_formatter(arr, cfg=DEFAULT_CFG):
    # FIXME: Choose sizes better.
    dtype = arr.dtype
    if dtype.kind == "i":
        return _choose_formatter_int(arr, cfg=cfg)
    elif dtype.kind == "f":
        return _choose_formatter_float(arr, cfg=cfg)
    elif dtype.kind == "b":
        return Bool()
    elif dtype.kind == "O":
        return String(16)
    else:
        raise TypeError("no default formatter for {}".format(dtype))


def get_default_formatter(type, values, cfg={}):
    """
    Chooses a default formatter based on type and values.

    @param values
      A sequence or `ndarray` of values.
    """
    values = np.array(values)

    if type is int:
        # Int types.
        size = get_size(abs(values).max())
        return formatters.IntFormatter(size)

    elif type is float:
        # Float types.  
        vals = values[~(np.isnan(values) | np.isinf(values))]
        if len(vals) == 0:
            # No normal values.
            return formatters.FloatFormatter(1, 1)
        # First determine the scale.
        neg = (vals < 0).any()
        abs_vals = abs(vals)
        max_val = abs_vals.max()
        if (max_val == 0 
            or float(cfg["scientific_max"]) < max_val 
                 < float(cfg["scientific_min"])):
            fmt = formatters.FloatFormatter
            size = 1 if len(vals) == 0 else get_size(max_val)
        else:
            # Use scientific notation for very small or very large.
            fmt = formatters.EFloatFormatter
            # Find the number of digits in the exponent.
            size = max(1, int(ceil(log10(floor(abs(log10(max_val)))))))
        # Guess precision.  Try progressively higher precision until we find
        # one where rounding there won't leave any residuals larger than
        # we are willing to represent at all.
        precision_min = int(cfg["precision_min"])
        precision_max = int(cfg["precision_max"])
        tol = (10 ** -precision_max) / 2
        for precision in range(precision_min, precision_max + 1):
            if (abs(np.round(vals, precision) - vals) < tol).all():
                break
        return fmt(
            size, precision, 
            sign="-" if neg else None,
            nan_str=cfg["nan_string"],
            inf_str=cfg["inf_string"])

    elif type is str:
        width = np.vectorize(len)(np.vectorize(str)(values)).max()
        str_width_min = int(cfg["str_width_min"])
        str_width_max = int(cfg["str_width_max"])
        width = clip(str_width_min, width, str_width_max)
        return formatters.StrFormatter(width, ellipsis=cfg["ellipsis"])

    elif type is bool:
        return formatters.BoolFormatter("TRUE", "FALSE", size=1, pad_left=True)

    elif type is datetime:
        return formatters.DatetimeFormatter("ISO 8601 extended")

    else:
        raise NotImplementedError("type: {}".format(type))
        

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


