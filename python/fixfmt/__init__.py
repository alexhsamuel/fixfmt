#-*- encoding: utf-8 -*-

from   __future__ import absolute_import, division, print_function

from   ._ext import Bool, Number, String, TickTime
from   ._ext import center, elide, pad, palide, string_length

__all__ = (
    "Bool",
    "center",
    "elide",
    "is_fmt",
    "Number",
    "pad",
    "palide",
    "String",
    "string_length",
    "TickTime",
)

#-------------------------------------------------------------------------------

# Populate scale aliases: the keys may be passed as the `scale` to `Number()`.
Number.SCALES.update({
    "%"     : (1E-2, "%"),
    "‰"     : (1E-3, "‰"),
    "bps"   : (1E-4, " bps"),

    "T"     : (1E+12, "T"),
    "G"     : (1E+9 , "G"),
    "M"     : (1E+6 , "M"),
    "k"     : (1E+3 , "k"),
    "d"     : (1E-1 , "d"),
    "c"     : (1E-2 , "c"),
    "m"     : (1E-3 , "m"),
   u"µ"     : (1E-6 ,u"µ"),
    "n"     : (1E-9 , "n"),
    "p"     : (1E-12, "p"),
    "f"     : (1E-16, "f"),

    "Gi"    : (1 << 30, "Gi"),
    "Mi"    : (1 << 20, "Mi"),
    "ki"    : (1 << 10, "ki"),

})

def is_fmt(obj):
    """
    Returns true iff `obj` is a formatter instance.
    """
    return callable(obj) and hasattr(obj, "width")


