import pytest

from   fixfmt import *

NAN = float("NaN")

#-------------------------------------------------------------------------------

def test_precision():
    x = 123.45678910
    assert Number(4      )(x) == "  123"
    assert Number(4,   -1)(x) == "  123"
    assert Number(4, None)(x) == "  123"
    assert Number(4,    0)(x) == "  123."
    assert Number(4,    1)(x) == "  123.5"
    assert Number(4,    3)(x) == "  123.457"


def test_scale():
    assert Number(8, scale="c")(1) == "      100c"
    assert Number(8, scale="µ")(1) == "  1000000µ"

    assert Number(5, 2, scale="k")(123456) == "   123.46k"
    assert Number(5, 2, scale="k")(NAN)    == "   NaN    "


