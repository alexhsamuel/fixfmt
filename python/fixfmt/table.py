from   contextlib import suppress
from   math import floor, log10
import re
import shutil
import sys

import numpy as np
from   pln.cfg import Group, Var, Cfg
from   pln.terminal import ansi

from   . import *
from   . import _ext

#-------------------------------------------------------------------------------
# Configuration

SEPARATOR_CONFIGURATION = Group(
    between                 = " ",
    end                     = "",
    index                   = " | ",
    start                   = "",
)

CONFIGURATION = Group(
    bottom = Group(
        line                        = "-",
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
            true                    = "True",
            false                   = "False",
        ),
        float = Group(
            inf                     = "\u221e",
            max_precision           = 8,
            min_precision           = None,
            nan                     = "NaN",
        ),
        min_width                   = 4,
        str = Group(
            min_size                =  1,
            max_size                = 32,
        ),
    ),
    header = Group(
        elide = Group(
            position                = 0.7,
        ),
        prefix                      = "",
        separator                   = SEPARATOR_CONFIGURATION,
        show                        = True,
        style = Group(
            prefix                  = "",
            suffix                  = "",
        ),
        suffix                      = "",
    ),
    index = Group(
        show                        = True,
    ),
    row = Group(
        separator                   = SEPARATOR_CONFIGURATION,
    ),
    row_ellipsis = Group(
        separator                   = SEPARATOR_CONFIGURATION,
        pad                         = " ",
        position                    = 0.85,
        format                      = "\u2026 skipping {skipped} rows \u2026",
    ),
    top = Group(
        line                        = "-",
        separator                   = SEPARATOR_CONFIGURATION,
        show                        = False,
    ),
    underline = Group(
        line                        = "=",
        separator                   = SEPARATOR_CONFIGURATION,
        show                        = True,
    ),
)



DEFAULT_CFG = Cfg(CONFIGURATION)

UNICODE_BOX_CFG = Cfg(CONFIGURATION)(
    bottom = dict(
        line                        = "\u2500",
        separator = dict(
            between                 = "\u2500\u2534\u2500",
            end                     = "\u2500\u2518",
            index                   = "\u2500\u2568\u2500",
            start                   = "\u2514\u2500",
        ),
        show                        = True,
    ),
    formatters = dict(
        bool = dict(
            true                    = "\u2714",
            false                   = "\u00b7",
        ),
        min_width                   = 4,
    ),
    header = dict(
        separator = dict(
            between                 = " \u2502 ",
            end                     = " \u2502",
            index                   = " \u2551 ",
            start                   = "\u2502 ",
        ),
    ),
    row_ellipsis = dict(
        separator = dict(
            end                     = "\u2561",
            start                   = "\u255e",
        ),
        pad                         = "\u2550",
        format                      = " skipped {skipped} rows ",
    ),
    row = dict(
        separator = dict(
            between                 = " \u2502 ",
            end                     = " \u2502",
            index                   = " \u2551 ",
            start                   = "\u2502 ",
        ),
    ),
    top = dict(
        line                        = "\u2500",
        separator = dict(
            between                 = "\u2500\u252c\u2500",
            end                     = "\u2500\u2510",
            index                   = "\u2500\u2565\u2500",
            start                   = "\u250c\u2500",
        ),
        show                        = True,
    ),
    underline = dict(
        line                        = "\u2500",
        separator = dict(
            between                 = "\u2500\u253c\u2500",
            end                     = "\u2500\u2524",
            index                   = "\u2500\u256b\u2500",
            start                   = "\u251c\u2500",
        ),
    ),
)


def _colorize(cfg):
    """
    ANSI-colorizes a configuration.
    """
    # Color true and false.
    cfg.formatters.bool.true = ansi.style(
        color=ansi.BLACK, bold=False, light=False)(cfg.formatters.bool.true)
    cfg.formatters.bool.false = ansi.style(
        color=ansi.BLACK, bold=False, light=True)(cfg.formatters.bool.false)

    # Color Inf and Nan, for visibility.
    cfg.formatters.float.inf = ansi.style(
        color=ansi.BLUE, bold=True, light=False)(cfg.formatters.float.inf)
    cfg.formatters.float.nan = ansi.style(
        color=ansi.GRAY, light=False)(cfg.formatters.float.nan)

    # Color lines gray.
    line_style = ansi.style(color=ansi.BLACK, light=True)
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
    cfg.row_ellipsis.format = ansi.style(color=ansi.BLACK, light=True)(
        cfg.row_ellipsis.format)


#-------------------------------------------------------------------------------

def _get_num_digits(value):
    """
    Returns the number of digits required to represent a value.
    """
    return 1 if value == 0 else max(int(floor(log10(abs(value)) + 1)), 1)


def _choose_formatter_bool(values, cfg):
    true    = cfg.formatters.bool.true
    false   = cfg.formatters.bool.false
    size    = max(
        cfg.formatters.min_width,
        string_length(true),
        string_length(false),
    )
    return Bool(true, false, size=size)


def _choose_formatter_float(values, cfg):
    # FIXME: This could be done with a single pass more efficiently.
    inf         = cfg.formatters.float.inf
    nan         = cfg.formatters.float.nan
    # Remove NaN and infinite values.
    is_nan      = np.isnan(values)
    has_nan     = is_nan.any()
    is_inf      = np.isinf(values)
    has_inf     = is_inf.any()
    has_neg_inf = (values[is_inf] < 0).any()
    # FIXME: We can't render numbers this big for now.
    too_big     = abs(values) > sys.maxsize
    vals        = values[~(is_nan | is_inf | too_big)]

    special_width = max(
        string_length(nan) if has_nan else 0,
        string_length(inf) if has_inf else 0)

    if len(vals) == 0:
        # No values left.
        return Number(max(1, special_width))

    # Determine the scale.
    min_val = vals.min()
    max_val = vals.max()
    sign    = "-" if has_neg_inf or min_val < 0 else " "
    size    = _get_num_digits(max(abs(min_val), abs(max_val)))

    # Try progressively higher precision until rounding won't leave any
    # residuals larger the maximum pecision.
    precision_min   = cfg.formatters.float.min_precision
    precision_min   = 0 if precision_min is None else precision_min
    precision_min   = max(precision_min, special_width - size - 1)
    precision_max   = cfg.formatters.float.max_precision
    tolerance       = (10 ** -precision_max) / 2
    for precision in range(precision_min, precision_max + 1):
        if (abs(np.round(vals, precision) - vals) < tolerance).all():
            break
    if cfg.formatters.float.min_precision is None and precision == 0:
        precision = None

    # Make the formatter.
    fmt = Number(size, precision, sign=sign, nan=nan, inf=inf)

    if fmt.width < cfg.formatters.min_width:
        # Expand size to achieve minimum width.
        size += cfg.formatters.min_width - fmt.width
        fmt = Number(size, precision, sign=sign, nan=nan, inf=inf)

    return fmt


def _choose_formatter_int(values, cfg):
    if len(values) == 0:
        return Number(1, None, sign=" ")

    min_val = values.min()
    max_val = values.max()
    sign    = " " if min_val >= 0 else "-"
    size    = _get_num_digits(max(abs(min_val), abs(max_val)))
    size    = max(size, cfg.formatters.min_width - (sign != " "))

    return Number(size, sign=sign)


def _choose_formatter_str(values, cfg):
    size = np.vectorize(len)(values).max()
    size = max(size, cfg.formatters.str.min_size, cfg.formatters.min_width)
    size = min(size, cfg.formatters.str.max_size)
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


def _get_formatter(name, arr, cfg):
    if name is not None:
        for key, fmt in cfg.formatters.by_name.items():
            # Check for a container match to the name.
            if name in key:
                return fmt

    # Check for the dtype name and kind.
    with suppress(KeyError):
        return cfg.formatters.by_dtype[arr.dtype.name]
    with suppress(KeyError):
        return cfg.formatters.by_dtype[arr.dtype.kind]

    return _get_default_formatter(arr, cfg=cfg)


def _get_header_justification(fmt):
    """
    Returns true if the format is right-justified.
    """
    if isinstance(fmt, Bool):
        return fmt.pad_left
    elif isinstance(fmt, Number):
        return True
    elif isinstance(fmt, String):
        return fmt.pad_left
    else:
        raise TypeError("unrecognized formatter: {!r}".format(fmt))


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
        else:
            raise TypeError("unsupported dtype: {}".format(arr.dtype))


    def add_string(self, string):
        self.__table.add_string(string)


    def add_index_column(self, name, arr, *, fmt=None):
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


    def add_column(self, name, arr, *, fmt=None):
        if self.__num_idx == len(self.__fmts):
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
                left = _get_header_justification(fmt)
                name = palide(
                    name, fmt.width, position=cfg.elide.position, left=left)
                name = cfg.style.prefix + name + cfg.style.suffix
                if i == self.__num_idx:
                    name = sep.index + name
                elif i > 0:
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
                      (sep.index if i == self.__num_idx
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
            max_rows = shutil.get_terminal_size().lines - 1

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



