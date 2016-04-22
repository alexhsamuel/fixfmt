from   ._ext import Bool, Number, String, TickTime
from   ._ext import center, elide, pad, palide, string_length

class Scale:
    """
    Container for scale constants.
    """

    # FIXME: These are synchronized manually with the corresponding C++ 
    # constants in number.cc.

    NONE        = (float("NaN"), "")

    PERCENT     = (1 /   100, "%")
    PER_MILLE   = (1 /  1000, "‰")
    BASIS_POINTS= (1 / 10000, " bps")

    TERA        = (1 / 1E+12, "T")
    GIGA        = (1 / 1E+9 , "G")
    MEGA        = (1 / 1E+6 , "M")
    KILO        = (1 / 1E+3 , "k")
    DECI        = (1 / 1E-1 , "d")
    CENTI       = (1 / 1E-2 , "c")
    MILLI       = (1 / 1E-3 , "m")
    MICRO       = (1 / 1E-6 , "µ")
    NANO        = (1 / 1E-9 , "n")
    PICO        = (1 / 1E-12, "p")
    FEMTO       = (1 / 1E-16, "f")

    GIBI        = (1 / (1 << 30), "Gi")
    MEBI        = (1 / (1 << 20), "Mi")
    KIBI        = (1 / (1 << 10), "Ki")



