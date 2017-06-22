from   __future__ import absolute_import, division, print_function

from   math import floor, log10
import numpy as np

from   ._ext import *

#-------------------------------------------------------------------------------

DEFAULTS = {
    "bool": {
        "true"          : "true",
        "false"         : "false",
    },
    "number": {
        "size"          : None,
        "precision"     : None,
        "min_precision" : None,
        "max_precision" : None,
        "pad"           : " ",
        "sign"          : None,
        "point"         : ".",
        "bad"           : "#",
        "nan"           : "NaN",
        "inf"           : "inf",
        "scale"         : None,
    },
    "string": {
        "size"          : None,
        "min_size"      : 0,
        "max_size"      : 64,
        "ellipsis"      : "\u2026",
        "pad"           : " ",
        "elide_position": 1.0,
        "pad_position"  : 1.0,
    },
}

def num_digits(value):
    """
    Returns the number of decimal digits required to represent a value.
    """
    return 1 if value == 0 else max(int(floor(log10(abs(value)) + 1)), 1)


def choose_formatter_bool(arr, min_width=0, cfg=DEFAULTS["bool"]):
    true    = cfg["true"]
    false   = cfg["false"]
    size    = max(
        min_width,
        string_length(true),
        string_length(false),
    )
    return Bool(true, false, size=size)


def choose_formatter_number(arr, min_width=0, cfg=DEFAULTS["number"]):
    # Analyze the array to determine relevant properties.
    scale = cfg["scale"]
    if scale is not None:
        scale_factor, _ = scale
        arr /= scale_factor

    if arr.dtype.kind == "f":
        max_precision = cfg["max_precision"]
        if max_precision is None:
            max_precision = 16 if arr.dtype.itemsize == 8 else 8
        # FIXME: Push down type check into C++.
        analyze = analyze_double if arr.dtype.itemsize == 8 else analyze_float
        (has_nan, has_pos_inf, has_neg_inf, num_vals, min_val, max_val, 
            val_prec) = analyze(arr, max_precision)
    elif arr.dtype.kind == "i":
        has_nan = has_pos_inf = has_neg_inf = False
        num_vals = len(arr)
        min_val = arr.min()
        max_val = arr.max()
        val_prec = 0
    else:
        raise TypeError("not a number dtype: {}".format(arr.dtype))

    inf = cfg["inf"]
    nan = cfg["nan"]
    special_width = max(
        string_length(nan) if has_nan else 0,
        string_length(inf) if has_pos_inf or has_neg_inf else 0)

    sign = cfg["sign"]
    if sign is None:
        sign = "-" if has_neg_inf or (num_vals > 0 and min_val < 0) else " "

    size = cfg["size"]
    if size is None:
        size = num_digits(max(abs(min_val), abs(max_val)))

    precision = cfg["precision"]
    if precision is None:
        precision = val_prec
        min_precision = cfg["min_precision"]
        if min_precision is not None:
            precision = max(min_precision, precision)
        max_precision = cfg["max_precision"]
        if max_precision is not None:
            precision = min(precision, max_precision)
        # If no min precision was given and no fractional digits are needed,
        # suppress the decimal point altogether.
        if min_precision is None and precision == 0:
            precision = None

    # Make the formatter.
    fmt = Number(
        size, precision, sign=sign, nan=nan, inf=inf, scale=scale,
        pad=cfg["pad"], point=cfg["point"], bad=cfg["bad"])

    if fmt.width < min_width:
        # Expand size to achieve minimum width.
        size += min_width - fmt.width
        fmt = Number(
            size, precision, sign=sign, nan=nan, inf=inf, scale=scale,
            pad=cfg["pad"], point=cfg["point"], bad=cfg["bad"])

    return fmt


# FIXME
def choose_formatter_datetime64(values, cfg):
    # FIXME: Is this really the right way to extract the datetime64 tick scale??
    match = re.match(r"datetime64\[(.*)\]$", values.dtype.name)
    assert match is not None
    try:
        scale = {
            "s"     : 0,
            "ms"    : 3,
            "us"    : 6,
            "ns"    : 9,
        }[match.group(1)]
    except KeyError:
        raise TypeError(
            "no default formatter for datetime64 scale {}".format(scale))

    # FIXME: Accelerate this with an extension module.
    values = values.astype("long")
    max_prec = min(scale, cfg.time.max_precision)
    min_prec = 0 if cfg.time.min_precision is None else cfg.time.min_precision
    for precision in range(max_prec, min_prec, -1):
        if not (values % (10 ** (scale - precision + 1)) == 0).all():
            break
    else:
        precision = cfg.time.min_precision

    return TickTime(10 ** scale, precision)


def choose_formatter_str(arr, min_width=0, cfg=DEFAULTS["string"]):
    size = cfg["size"]
    if size is None:
        min_size = cfg["min_size"]
        max_size = cfg["max_size"]
        if arr.dtype.kind == "S":
            size = arr.dtype.itemsize
        elif arr.dtype.kind == "U":
            size = arr.dtype.itemsize // np.dtype("U1").itemsize
        else:
            size = np.vectorize(lambda x: string_length(str(x)))(values).max()
        size = max(min_width, min_size, min(size, max_size))

    return String(
        size, ellipsis=cfg["ellipsis"], pad=cfg["pad"],
        elide_position=cfg["elide_position"], pad_position=cfg["pad_position"])


def choose_formatter(arr, min_width=0):
    dtype = arr.dtype
    if dtype.kind == "b":
        return choose_formatter_bool(arr, min_width)
    elif dtype.kind in "fi":
        return choose_formatter_number(arr, min_width)
    elif dtype.kind == "M":
        return choose_formatter_datetime64(arr, min_width)
    elif dtype.kind in "OSU":
        return choose_formatter_str(arr, min_width)
    else:
        raise TypeError("no default formatter for {}".format(dtype))


