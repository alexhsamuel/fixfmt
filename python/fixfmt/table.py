import copy
import numpy as np

from   . import string_length, palide, center, Bool, Number, String, is_fmt
from   . import _ext
from   . import npfmt
from   .lib import ansi

#-------------------------------------------------------------------------------

# FIXME: Elsewhere.
def update_cfg(old, updates):
    new = copy.deepcopy(old)
    for key, val in updates.items():
        if isinstance(val, dict):
            val = update_cfg(old.get(key, {}), val)
        new[key] = val
    return new


SEPARATOR_CFG = {
    "between"               : u" ",
    "end"                   : u"",
    "index"                 : u" | ",
    "start"                 : u"",
}

DEFAULT_CFG = {
    "bottom": {
        "line"                      : u"-",
        "separator"                 : SEPARATOR_CFG,
        "show"                      : False,
    },
    "data": {
        "max_rows"                  : "terminal",
    },
    "formatters": {
        "by_name"                   : {},
        "by_dtype"                  : {},
        "default"                   : npfmt.DEFAULT_CFG,
    },
    "header": {
        "elide": {
            "position"              : 0.7,
            "ellipsis"              : u"/",
        },
        "prefix"                    : u"",
        "separator"                 : SEPARATOR_CFG,
        "show"                      : True,
        "style": {
            "prefix"                : u"",
            "suffix"                : u"",
        },
        "suffix"                    : u"",
    },
    "index": {
        "show"                      : True,
    },
    "row": {
        "separator"                 : SEPARATOR_CFG,
    },
    "row_ellipsis": {
        "separator"                 : SEPARATOR_CFG,
        "pad"                       : u" ",
        "position"                  : 0.85,
        "format"                    : u"... skipping {skipped} rows ...",
    },
    "top": {
        "line"                      : u"-",
        "separator"                 : SEPARATOR_CFG,
        "show"                      : False,
    },
    "underline": {
        "line"                      : u"=",
        "separator"                 : SEPARATOR_CFG,
        "show"                      : True,
    },
}



UNICODE_CFG = update_cfg(DEFAULT_CFG, {
    "formatters": {
        "default": {
            "bool": {
                "true"                  : u"\u2714",
                "false"                 : u"\u00b7",
            },
            "number": {
                "inf"                   : u"\u221e",
            },
            "string": {
                "ellipsis"              : u"\u2026",
            },
        },
    },
    "header": {
        "elide": {
            "ellipsis"              : u"\u2026",
        },
    },
    "row_ellipsis": {
        "format"                    : u"\u2026 skipping {skipped} rows \u2026",
    },
    "underline": {
        "line"                      : u"\u2550",
    },
})


UNICODE_BOX_CFG = update_cfg(DEFAULT_CFG, {
    "bottom": {
        "line"                      : u"\u2500",
        "separator": {
            "between"               : u"\u2500\u2534\u2500",
            "end"                   : u"\u2500\u2518",
            "index"                 : u"\u2500\u2568\u2500",
            "start"                 : u"\u2514\u2500",
        },
        "show"                      : True,
    },
    "formatters": {
        "default": {
            "number": {
                "inf"                   : u"\u221e",
            },
            "string": {
                "ellipsis"              : u"\u2026",
            },
        },
    },
    "header": {
        "elide": {
            "ellipsis"              : u"\u2026",
        },
        "separator": {
            "between"               : u" \u2502 ",
            "end"                   : u" \u2502",
            "index"                 : u" \u2551 ",
            "start"                 : u"\u2502 ",
        },
    },
    "row_ellipsis": {
        "separator": {
            "end"                   : u"\u2561",
            "start"                 : u"\u255e",
        },
        "pad"                       : u"\u2550",
        "format"                    : u" skipped {skipped} rows ",
    },
    "row": {
        "separator": {
            "between"               : u" \u2502 ",
            "end"                   : u" \u2502",
            "index"                 : u" \u2551 ",
            "start"                 : u"\u2502 ",
        },
    },
    "top": {
        "line"                      : u"\u2500",
        "separator": {
            "between"               : u"\u2500\u252c\u2500",
            "end"                   : u"\u2500\u2510",
            "index"                 : u"\u2500\u2565\u2500",
            "start"                 : u"\u250c\u2500",
        },
        "show"                      : True,
    },
    "underline": {
        "line"                      : u"\u2500",
        "separator": {
            "between"               : u"\u2500\u253c\u2500",
            "end"                   : u"\u2500\u2524",
            "index"                 : u"\u2500\u256b\u2500",
            "start"                 : u"\u251c\u2500",
        },
    },
})


def _colorize(cfg):
    """
    ANSI-colorizes a configuration.

    Modifies `cfg` in place.
    """
    fmt_cfg = cfg["formatters"]["default"]

    # Color true and false.
    c = fmt_cfg["bool"]
    c.update({
        "true"  : ansi.style(fg="black"    )(c["true"]),
        "false" : ansi.style(fg="dark_gray")(c["false"]),
    })

    # Color Inf and Nan, for visibility.
    c = fmt_cfg["number"]
    c.update({
        "inf"   : ansi.style(fg="blue", bold=True)(c["inf"]),
        "nan"   : ansi.style(fg="dark_gray"      )(c["nan"]),
    })

    # Color lines gray.
    line_style = ansi.style(fg="light_gray")
    style_line = lambda s: s if s is None else line_style(s)
    for c in (cfg["bottom"], cfg["header"], cfg["row"], cfg["row_ellipsis"],
              cfg["top"], cfg["underline"], ):
        c = c["separator"]
        c.update({
            "between"   : style_line(c["between"]),
            "end"       : style_line(c["end"]),
            "index"     : style_line(c["index"]),
            "start"     : style_line(c["start"]),
        })
    for c in cfg["bottom"], cfg["top"], cfg["underline"]:
        c["line"] = style_line(c["line"])

    # Use underlining instead of drawing an underline.
    # FIXME: Hacky.
    if not cfg["underline"]["show"]:
        cfg["header"]["style"].update({
            "prefix": ansi.sgr(underline=True, bold=True),
            "suffix": ansi.RESET,
        })

    # Color the ellipsis row.
    c = cfg["row_ellipsis"]
    c.update({
        "pad"   : style_line(c["pad"]),
        "format": ansi.style(fg="light_gray")(c["format"]),
    })

    return cfg


#-------------------------------------------------------------------------------

def _get_formatter(name, arr, cfg):
    """
    Constructs a formatter for a named array.
    """
    # Start with the overall default formatter configuration
    fmt_cfg = cfg["default"]

    # For each of the name, dtype name, and dtype kind, look up in the
    # corresponding cfg dict.  If we find a formatter, return it outright.
    # Otherwise, update the formatter cfg.
    for c, k in (
            (cfg["by_dtype"], arr.dtype.kind),
            (cfg["by_dtype"], arr.dtype.name),
            (cfg["by_name"], name),
    ):
        try:
            val = c[k]
        except KeyError:
            pass
        else:
            if is_fmt(val):
                return val
            else:
                fmt_cfg = update_cfg(fmt_cfg, val)

    min_width = fmt_cfg["min_width"]
    if fmt_cfg["name_width"]:
        min_width = max(min_width, string_length(name))

    return npfmt.choose_formatter(arr, min_width=min_width, cfg=fmt_cfg)


def _get_header_position(fmt):
    """
    Returns the pad position for justifying the header.
    """
    if isinstance(fmt, Bool):
        return fmt.pos
    elif isinstance(fmt, Number):
        return 0  # FIXME: Constant.
    elif isinstance(fmt, String):
        return fmt.pad_pos
    else:
        # Assume everythign else is left-justified.
        return 1  # FIXME: Constant.


#-------------------------------------------------------------------------------

class Table:

    def __init__(self, cfg=DEFAULT_CFG):
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
        self.add_string(self.__cfg["row"]["separator"]["start"])


    def __add_array(self, arr, fmt):
        table = self.__table

        name = arr.dtype.name
        if name in {
            "int8", "int16", "int32", "int64",
            "uint8", "uint16", "uint32", "uint64",
            "float32", "float64", "bool"
        }:
            getattr(table, "add_" + name)(arr, fmt)
        elif name == "object":
            table.add_str_object(arr, fmt)
        elif arr.dtype.kind in "U":
            table.add_ucs32(arr.dtype.itemsize, arr, fmt)
        elif arr.dtype.kind in "S":
            table.add_utf8(arr.dtype.itemsize, arr, fmt)
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
            self.add_string(self.__cfg["row"]["separator"]["between"])

        if fmt is None:
            fmt = _get_formatter(name, arr, self.__cfg["formatters"])
        self.__add_array(arr, fmt)
        self.__names.append(name)
        self.__fmts.append(fmt)
        self.__num_idx += 1


    def add_column(self, name, arr, fmt=None):
        if self.__num_idx > 0 and self.__num_idx == len(self.__fmts):
            self.add_string(self.__cfg["row"]["separator"]["index"])
        elif len(self.__fmts) > 0:
            self.add_string(self.__cfg["row"]["separator"]["between"])

        if fmt is None:
            fmt = _get_formatter(name, arr, self.__cfg["formatters"])
        self.__add_array(arr, fmt)
        self.__names.append(name)
        self.__fmts.append(fmt)


    def finish(self):
        self.add_string(self.__cfg["row"]["separator"]["end"])


    def _fmt_header(self):
        cfg = self.__cfg["header"]
        assert string_length(cfg["style"]["prefix"]) == 0
        assert string_length(cfg["style"]["suffix"]) == 0

        if cfg["show"]:
            sep = cfg["separator"]

            def format_name(i, name, fmt):
                name = name or ""
                name = cfg["prefix"] + name + cfg["suffix"]
                pad_pos = _get_header_position(fmt)
                name = palide(
                    name, fmt.width,
                    elide_pos=cfg["elide"]["position"],
                    ellipsis=cfg["elide"]["ellipsis"],
                    pad_pos=pad_pos)
                name = cfg["style"]["prefix"] + name + cfg["style"]["suffix"]
                if i > 0:
                    if i == self.__num_idx:
                        name = sep["index"] + name
                    else:
                        name = sep["between"] + name
                return name

            header = sep["start"] + "".join(
                format_name(i, n, f)
                for i, (n, f) in enumerate(zip(self.__names, self.__fmts))
            ) + sep["end"]
            return header


    def _fmt_line(self, cfg):
        if cfg["show"]:
            # FIXME: Relax this.
            if string_length(cfg["line"]) != 1:
                raise ValueError("line must be one character")

            sep = cfg["separator"]
            return (
                  sep["start"]
                + "".join(
                      (sep["index"] if i > 0 and i == self.__num_idx
                       else sep["between"] if i > 0
                       else "")
                    + cfg["line"] * f.width
                    for i, f in enumerate(self.__fmts)
                )
                + sep["end"]
            )


    def _fmt_top(self):
        return self._fmt_line(self.__cfg["top"])


    def _fmt_underline(self):
        return self._fmt_line(self.__cfg["underline"])


    def _fmt_bottom(self):
        return self._fmt_line(self.__cfg["bottom"])


    # FIXME: By screen (repeating header?)
    # FIXME: Do what when it's too wide???

    def _format(self):
        cfg = self.__cfg

        num_extra_rows  = sum([
            cfg["top"]["show"],
            cfg["header"]["show"],
            cfg["underline"]["show"],
            cfg["bottom"]["show"],
        ])

        max_rows = cfg["data"]["max_rows"]
        if max_rows == "terminal":
            # FIXME
            max_rows = ansi.get_terminal_size().lines - 1

        yield self._fmt_top()
        yield self._fmt_header()
        yield self._fmt_underline()

        table = self.__table
        num_rows = len(table)
        if max_rows is None or num_rows <= max_rows - num_extra_rows:
            for i in range(len(table)):
                yield table(i)
        else:
            cfg_ell             = cfg["row_ellipsis"]
            num_rows_top        = int(cfg_ell["position"] * max_rows)
            num_rows_bottom     = max_rows - num_extra_rows - num_rows_top - 1
            num_rows_skipped    = num_rows - num_rows_top - num_rows_bottom

            # Print rows from the top.
            for i in range(num_rows_top):
                yield table(i)

            # Print the row ellipsis.
            ell = cfg_ell["format"].format(
                bottom  =num_rows_bottom,
                rows    =num_rows,
                skipped =num_rows_skipped,
                top     =num_rows_top,
            )
            ell_start   = cfg_ell["separator"]["start"]
            ell_end     = cfg_ell["separator"]["end"]
            ell_pad     = cfg_ell["pad"]
            ell_width   = (
                  table.width
                - string_length(ell_start)
                - string_length(ell_end))
            ell         = center(ell, ell_width, ell_pad)
            yield ell_start + ell + ell_end

            # Print rows from the bottom.
            for i in range(num_rows - num_rows_bottom, num_rows):
                yield table(i)

        yield self._fmt_bottom()


    def format(self):
        yield from filter(None, self._format())


    def print(self, print=print):
        for line in self.format():
            print(line)



def from_arrays(arrs, cfg=DEFAULT_CFG):
    """
    Constructs a table from arrays.

    :param arrs:
      A dict or iterable of items from name to array.
    """
    try:
        arrs = arrs.items()
    except AttributeError:
        pass

    tbl = Table(cfg)
    for name, arr in arrs:
        # FIXME: Since Table doesn't support S and U arrays, convert these
        # to objects for now.
        if arr.dtype.kind in "SU":
            arr = arr.astype(object)
        tbl.add_column(name, arr)
    tbl.finish()
    return tbl


#-------------------------------------------------------------------------------

class RowTable:
    """
    :ivar names:
      List of column names to show.  T
    """

    DEFAULT_CFG = _colorize(update_cfg(UNICODE_CFG, {
        "header": {
            "separator": {
                "between": "  ",
            },
            "style": {
                "prefix": ansi.sgr(bold=True),
                "suffix": ansi.sgr(bold=False),
            },
        },
        "formatters": {
            "default": {
                "name_width": True,
            },
        },
        "row": {
            "separator": {
                "between": "  ",
            },
        },
        "underline": {
            "separator": {
                "between": "  ",
            },
        },
    }))

    LINE = object()


    def __init__(self, cfg=DEFAULT_CFG):
        self.cfg        = cfg
        self.names      = []
        self.rows       = []
        self.fmts       = {}
        self.defaults   = {}
        self.titles     = {}


    def append(self, **fields):
        self.rows.append(fields)
        for name in fields:
            if name not in self.names:
                self.names.append(name)


    def extend(self, rows):
        for row in rows:
            self.append(**row)


    @property
    def num_rows(self):
        return len(self.rows)


    def line(self):
        self.rows.append(self.LINE)


    def text(self, text):
        self.rows.append(text)


    def order(self, names):
        old_names = list(names)
        self.names.clear()
        for name in names:
            if name in old_names:
                self.names.append(name)
        for name in old_names:
            if name not in self.names:
                self.names.append(name)


    def set_fmts(self):
        cfg = self.cfg["formatters"]
        for name in self.names:
            if name not in self.fmts:
                # FIXME: Do better.
                arr = np.array([
                    r[name] for r in self.rows
                    if r is not self.LINE 
                    and name in r 
                    and r[name] is not None
                ])
                self.fmts[name] = _get_formatter(name, arr, cfg=cfg)


    def __line(self, fmts, cfg):
        if not cfg["show"]:
            return

        sep = cfg["separator"]
        fmts = ( f for f in fmts if f is not None )
        yield (
              sep["start"]
            + "".join(
                  (sep["between"] if i > 0 else "")
                + cfg["line"] * f.width
                for i, f in enumerate(fmts)
            )
            + sep["end"]
        )


    def __header(self, names, fmts, cfg):
        if not cfg["show"]:
            return

        assert string_length(cfg["style"]["prefix"]) == 0
        assert string_length(cfg["style"]["suffix"]) == 0

        sep = cfg["separator"]

        def format_name(name, fmt):
            name = name or ""
            name = cfg["prefix"] + name + cfg["suffix"]
            pad_pos = _get_header_position(fmt)
            name = palide(
                name,
                fmt.width,
                elide_pos   =cfg["elide"]["position"],
                ellipsis    =cfg["elide"]["ellipsis"],
                pad_pos     =pad_pos
            )
            name = cfg["style"]["prefix"] + name + cfg["style"]["suffix"]
            return name

        yield sep["start"] + sep["between"].join(
            format_name(n, f)
            for n, f in zip(names, fmts)
            if f is not None
        ) + sep["end"]


    def __row(self, names, fmts, vals, cfg):
        sep = cfg["separator"]
        vals = (
            " " * f.width if v is None else f(v)
            for f, v in zip(fmts, vals)
            if f is not None
        )
        yield (
              sep["start"]
            + sep["between"].join(vals)
            + sep["end"]
        )


    def __iter__(self):
        self.set_fmts()

        names = self.names
        fmts = [ self.fmts.get(n, None) for n in names ]

        yield from self.__line(fmts, self.cfg["top"])
        yield from self.__header(names, fmts, self.cfg["header"])
        yield from self.__line(fmts, self.cfg["underline"])

        defs = [ self.defaults.get(n, None) for n in names ]

        for row in self.rows:
            if row is self.LINE:
                yield from self.__line()
            elif isinstance(row, str):
                yield row
            else:
                vals = ( row.get(n, d) for n, d in zip(names, defs) )
                yield from self.__row(names, fmts, vals, self.cfg["row"])

        yield from self.__line(fmts, self.cfg["bottom"])


    def print(self, print=print):
        for line in self:
            print(line)


