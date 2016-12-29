from   __future__ import absolute_import, division, print_function

from   math import floor, log10
import re
import sys

import numpy as np

from   . import *
from   . import _ext
from   .lib import ansi
from   .lib.cfg import *

#-------------------------------------------------------------------------------

SEPARATOR_CONFIGURATION = Group(
    between                 = u" ",
    end                     = u"",
    index                   = u" | ",
    start                   = u"",
)

CONFIGURATION = Group(
    bottom = Group(
        line                        = u"-",
        separator                   = SEPARATOR_CONFIGURATION,
        show                        = False,
    ),
    data = Group(
        max_rows                    = Var(default="terminal"),
    ),
    formatters = Group(
        by_name                     = {},
        by_dtype                    = {},
        bool = Group(
            true                    = u"True",
            false                   = u"False",
        ),
        float = Group(
            inf                     = u"inf",
            max_precision           = 8,
            min_precision           = None,
            nan                     = u"NaN",
        ),
        min_width                   = 4,
        str = Group(
            min_size                =  1,
            max_size                = 32,
            ellipsis                = u"..",
        ),
        time = Group(
            max_precision           = 9,
            min_precision           = None,
        ),
    ),
    header = Group(
        elide = Group(
            position                = 0.7,
            ellipsis                = u"/",
        ),
        prefix                      = u"",
        separator                   = SEPARATOR_CONFIGURATION,
        show                        = True,
        style = Group(
            prefix                  = u"",
            suffix                  = u"",
        ),
        suffix                      = u"",
    ),
    index = Group(
        show                        = True,
    ),
    row = Group(
        separator                   = SEPARATOR_CONFIGURATION,
    ),
    row_ellipsis = Group(
        separator                   = SEPARATOR_CONFIGURATION,
        pad                         = u" ",
        position                    = 0.85,
        format                      = u"... skipping {skipped} rows ...",
    ),
    top = Group(
        line                        = u"-",
        separator                   = SEPARATOR_CONFIGURATION,
        show                        = False,
    ),
    underline = Group(
        line                        = u"=",
        separator                   = SEPARATOR_CONFIGURATION,
        show                        = True,
    ),
)



DEFAULT_CFG = Cfg(CONFIGURATION)

UNICODE_CFG = Cfg(CONFIGURATION)(
    formatters = dict(
        float = dict(
            inf                     = u"\u221e",
        ),
        str = dict(
            ellipsis                = u"\u2026",
        ),
    ),
    header = dict(
        elide = dict(
            ellipsis                = u"\u2026",
        ),
    ),
    row_ellipsis = dict(
        format                      = u"\u2026 skipping {skipped} rows \u2026",
    ),
)

UNICODE_BOX_CFG = Cfg(CONFIGURATION)(
    bottom = dict(
        line                        = u"\u2500",
        separator = dict(
            between                 = u"\u2500\u2534\u2500",
            end                     = u"\u2500\u2518",
            index                   = u"\u2500\u2568\u2500",
            start                   = u"\u2514\u2500",
        ),
        show                        = True,
    ),
    formatters = dict(
        bool = dict(
            true                    = u"\u2714",
            false                   = u"\u00b7",
        ),
        float = dict(
            inf                     = u"\u221e",
        ),
        min_width                   = 4,
        str = dict(
            ellipsis                = u"\u2026",
        ),
    ),
    header = dict(
        elide = dict(
            ellipsis                = u"\u2026",
        ),
        separator = dict(
            between                 = u" \u2502 ",
            end                     = u" \u2502",
            index                   = u" \u2551 ",
            start                   = u"\u2502 ",
        ),
    ),
    row_ellipsis = dict(
        separator = dict(
            end                     = u"\u2561",
            start                   = u"\u255e",
        ),
        pad                         = u"\u2550",
        format                      = u" skipped {skipped} rows ",
    ),
    row = dict(
        separator = dict(
            between                 = u" \u2502 ",
            end                     = u" \u2502",
            index                   = u" \u2551 ",
            start                   = u"\u2502 ",
        ),
    ),
    top = dict(
        line                        = u"\u2500",
        separator = dict(
            between                 = u"\u2500\u252c\u2500",
            end                     = u"\u2500\u2510",
            index                   = u"\u2500\u2565\u2500",
            start                   = u"\u250c\u2500",
        ),
        show                        = True,
    ),
    underline = dict(
        line                        = u"\u2500",
        separator = dict(
            between                 = u"\u2500\u253c\u2500",
            end                     = u"\u2500\u2524",
            index                   = u"\u2500\u256b\u2500",
            start                   = u"\u251c\u2500",
        ),
    ),
)


def _colorize(cfg):
    """
    ANSI-colorizes a configuration.
    """
    # Color true and false.
    cfg.formatters.bool.true = ansi.style(
        fg="black", bold=False)(cfg.formatters.bool.true)
    cfg.formatters.bool.false = ansi.style(
        fg="dark_gray", bold=False)(cfg.formatters.bool.false)

    # Color Inf and Nan, for visibility.
    cfg.formatters.float.inf = ansi.style(
        fg="blue", bold=True)(cfg.formatters.float.inf)
    cfg.formatters.float.nan = ansi.style(
        fg="dark_gray")(cfg.formatters.float.nan)

    # Color lines gray.
    line_style = ansi.style(fg="light_gray")
    style_line = lambda s: s if s is None else line_style(s)
    for c in (cfg.bottom, cfg.header, cfg.row, cfg.row_ellipsis, cfg.top, 
              cfg.underline, ):
        c = c.separator
        c.between   = style_line(c.between)
        c.end       = style_line(c.end)
        c.index     = style_line(c.index)
        c.start     = style_line(c.start)
    for c in cfg.bottom, cfg.top, cfg.underline:
        c.line      = style_line(c.line)
    cfg.row_ellipsis.pad = style_line(cfg.row_ellipsis.pad)

    # Use underlining instead of drawing an underline.
    # FIXME: Hacky.
    if not cfg.underline.show:
        cfg.header.style.prefix = ansi.sgr(underline=True, bold=True)
        cfg.header.style.suffix = ansi.RESET

    # Color the ellipsis row.
    cfg.row_ellipsis.format = ansi.style(fg="light_gray")(
        cfg.row_ellipsis.format)


#-------------------------------------------------------------------------------

def _get_num_digits(value):
    """
    Returns the number of digits required to represent a value.
    """
    return 1 if value == 0 else max(int(floor(log10(abs(value)) + 1)), 1)


def _choose_formatter_bool(values, cfg):
    true    = cfg.bool.true
    false   = cfg.bool.false
    size    = max(
        cfg.min_width,
        string_length(true),
        string_length(false),
    )
    return Bool(true, false, size=size)


def _choose_formatter_float(values, cfg):
    # Analyze the array to determine relevant properties.
    # FIXME: Push down type check into C++.
    analyze = (     _ext.analyze_double if values.dtype.itemsize == 8
               else _ext.analyze_float)
    has_nan, has_pos_inf, has_neg_inf, num_vals, min_val, max_val, precision = \
      analyze(values, cfg.float.max_precision)

    inf = cfg.float.inf
    nan = cfg.float.nan
    special_width = max(
        string_length(nan) if has_nan else 0,
        string_length(inf) if has_pos_inf or has_neg_inf else 0)

    if num_vals == 0:
        # No values left.
        return Number(max(1, special_width))

    sign    = "-" if has_neg_inf or min_val < 0 else " "
    size    = _get_num_digits(max(abs(min_val), abs(max_val)))
    if cfg.float.min_precision is None:
        precision = None if precision == 0 else precision
    else:
        precision = max(cfg.float.min_precision, precision)

    # Make the formatter.
    fmt = Number(size, precision, sign=sign, nan=nan, inf=inf)

    if fmt.width < cfg.min_width:
        # Expand size to achieve minimum width.
        size += cfg.min_width - fmt.width
        fmt = Number(size, precision, sign=sign, nan=nan, inf=inf)

    return fmt


def _choose_formatter_int(values, cfg):
    if len(values) == 0:
        return Number(1, None, sign=" ")

    min_val = values.min()
    max_val = values.max()
    sign    = " " if min_val >= 0 else "-"
    size    = _get_num_digits(max(abs(min_val), abs(max_val)))
    size    = max(size, cfg.min_width - (sign != " "))

    return Number(size, sign=sign)


def _choose_formatter_datetime64(values, cfg):
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


def _choose_formatter_str(values, cfg):
    size = np.vectorize(lambda x: len(str(x)))(values).max()
    size = max(size, cfg.str.min_size, cfg.min_width)
    size = min(size, cfg.str.max_size)
    return String(size, ellipsis=cfg.str.ellipsis)


def _get_default_formatter(arr, cfg):
    dtype = arr.dtype
    if dtype.kind == "b":
        return _choose_formatter_bool(arr, cfg=cfg.formatters)
    elif dtype.kind == "f":
        return _choose_formatter_float(arr, cfg=cfg.formatters)
    elif dtype.kind == "i":
        return _choose_formatter_int(arr, cfg=cfg.formatters)
    elif dtype.kind == "M":
        return _choose_formatter_datetime64(arr, cfg=cfg.formatters)
    elif dtype.kind == "O":
        return _choose_formatter_str(arr, cfg=cfg.formatters)
    else:
        raise TypeError("no default formatter for {}".format(dtype))


def _get_formatter(name, arr, cfg):
    if name is not None:
        for key, fmt in cfg.formatters.by_name.items():
            # Check for a container match to the name.
            if name in key:
                return fmt

    # Check for the dtype name and kind.
    try:
        return cfg.formatters.by_dtype[arr.dtype.name]
    except KeyError:
        pass
    try:
        return cfg.formatters.by_dtype[arr.dtype.kind]
    except KeyError:
        pass

    return _get_default_formatter(arr, cfg=cfg)


def _get_header_position(fmt):
    """
    Returns the pad position for justifying the header.
    """
    if isinstance(fmt, Bool):
        return fmt.pad_position
    elif isinstance(fmt, Number):
        return 0  # FIXME: Constant.
    elif isinstance(fmt, String):
        return fmt.pad_position
    else:
        # Assume everythign else is left-justified.
        return 1  # FIXME: Constant.


#-------------------------------------------------------------------------------

class Table:

    def __init__(self, cfg):
        # The configuration.
        self.__cfg      = cfg
        # Column names and formatters.
        self.__names    = []
        self.__fmts     = []
        # Number of leading index columns.
        self.__num_idx  = 0
        # The underlying table.
        self.__table    = _ext.Table()

        # Start the table.
        self.add_string(self.__cfg.row.separator.start)


    def __add_array(self, arr, fmt):
        table = self.__table

        name = arr.dtype.name
        if name in {
            "int8", "int16", "int32", "int64", 
            "float32", "float64", "bool"
        }:
            getattr(table, "add_" + name)(arr, fmt)
        elif name == "object":
            table.add_str_object(arr, fmt)
        elif name.startswith("datetime64"):  # FIXME: Sloppy.
            table.add_tick_time(arr.astype("int64"), fmt)
        else:
            raise TypeError("unsupported dtype: {}".format(arr.dtype))


    def add_string(self, string):
        self.__table.add_string(string)


    def add_index_column(self, name, arr, fmt=None):
        assert self.__num_idx == len(self.__fmts), \
            "can't add index after normal column"

        if self.__num_idx > 0:
            self.add_string(self.__cfg.row.separator.between)

        if fmt is None:
            fmt = _get_formatter(name, arr, self.__cfg)
        self.__add_array(arr, fmt)
        self.__names.append(name)
        self.__fmts.append(fmt)
        self.__num_idx += 1


    def add_column(self, name, arr, fmt=None):
        if self.__num_idx > 0 and self.__num_idx == len(self.__fmts):
            self.add_string(self.__cfg.row.separator.index)
        elif len(self.__fmts) > 0:
            self.add_string(self.__cfg.row.separator.between)

        if fmt is None:
            fmt = _get_formatter(name, arr, self.__cfg)
        self.__add_array(arr, fmt)
        self.__names.append(name)
        self.__fmts.append(fmt)


    def finish(self):
        self.add_string(self.__cfg.row.separator.end)


    def _print_header(self):
        cfg = self.__cfg.header
        assert string_length(cfg.style.prefix) == 0
        assert string_length(cfg.style.suffix) == 0

        if cfg.show:
            sep = cfg.separator

            def format_name(i, name, fmt):
                name = name or ""
                name = cfg.prefix + name + cfg.suffix
                pad_position = _get_header_position(fmt)
                name = palide(
                    name, fmt.width, 
                    elide_position=cfg.elide.position, 
                    ellipsis=cfg.elide.ellipsis,
                    pad_position=pad_position)
                name = cfg.style.prefix + name + cfg.style.suffix
                if i > 0:
                    if i == self.__num_idx:
                        name = sep.index + name
                    else:
                        name = sep.between + name
                return name

            header = sep.start + "".join(
                format_name(i, n, f) 
                for i, (n, f) in enumerate(zip(self.__names, self.__fmts))
            ) + sep.end
            print(header)


    def _print_line(self, cfg):
        if cfg.show:
            # FIXME: Relax this.
            if string_length(cfg.line) != 1:
                raise ValueError("line must be one character")

            sep = cfg.separator
            print(
                  sep.start
                + "".join( 
                      (sep.index if i > 0 and i == self.__num_idx
                       else sep.between if i > 0
                       else "")
                    + cfg.line * f.width 
                    for i, f in enumerate(self.__fmts)
                )
                + sep.end
            )


    def _print_top(self):
        self._print_line(self.__cfg.top)


    def _print_underline(self):
        self._print_line(self.__cfg.underline)


    def _print_bottom(self):
        self._print_line(self.__cfg.bottom)


    # FIXME: By screen (repeating header?)
    # FIXME: Do what when it's too wide???

    def print(self):
        cfg = self.__cfg

        num_extra_rows  = sum([
            cfg.top.show,
            cfg.header.show,
            cfg.underline.show,
            cfg.bottom.show,
        ])

        max_rows = cfg.data.max_rows
        if max_rows == "terminal":
            # FIXME
            max_rows = ansi.get_terminal_size().lines - 1

        self._print_top()
        self._print_header()
        self._print_underline()

        table = self.__table
        num_rows = len(table)
        if num_rows <= max_rows - num_extra_rows:
            for i in range(len(table)):
                print(table(i))
        else:
            cfg_ell             = cfg.row_ellipsis
            num_rows_top        = int(cfg_ell.position * max_rows)
            num_rows_bottom     = max_rows - num_extra_rows - num_rows_top - 1
            num_rows_skipped    = num_rows - num_rows_top - num_rows_bottom

            # Print rows from the top.
            for i in range(num_rows_top):
                print(table(i))

            # Print the row ellipsis.
            ell = cfg_ell.format.format(
                bottom  =num_rows_bottom,
                rows    =num_rows,
                skipped =num_rows_skipped,
                top     =num_rows_top,
            )
            ell_start   = cfg_ell.separator.start
            ell_end     = cfg_ell.separator.end
            ell_pad     = cfg_ell.pad
            ell_width   = (
                  table.width 
                - string_length(ell_start) 
                - string_length(ell_end))
            ell         = center(ell, ell_width, ell_pad)
            print(ell_start + ell + ell_end)

            # Print rows from the bottom.
            for i in range(num_rows - num_rows_bottom, num_rows):
                print(table(i))

        self._print_bottom()



#-------------------------------------------------------------------------------

class Simple:  # FIXME

    def __init__(self, columns):
        def norm(cols):
            for i, col in enumerate(cols):
                try:
                    name, fmt = col
                except TypeError:
                    if callable(fmt):
                        name = getattr(fmt, "name", "col{}".format(i))
                    else:
                        raise TypeError(
                            "column is not a pair or callable: {}".format(col))
                yield fmt, name

        self.__fmts, self.__names = zip(*norm(columns))


    @property
    def header(self):
        return " ".join(
            palide(n, f.width) for f, n in zip(self.__fmts, self.__names))


    @property
    def underline(self):
        return " ".join( "-" * f.width for f in self.__fmts )


    def row(self, *values):
        return " ".join( f(v) for f, v in zip(self.__fmts, values) )



