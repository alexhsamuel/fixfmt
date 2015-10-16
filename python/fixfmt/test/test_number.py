import pytest

from   fixfmt import *

#-------------------------------------------------------------------------------

def test_precision():
    x = 123.45678910
    assert Number(4      )(x) == "  123"
    assert Number(4,   -1)(x) == "  123"
    assert Number(4, None)(x) == "  123"
    assert Number(4,    0)(x) == "  123."
    assert Number(4,    1)(x) == "  123.5"
    assert Number(4,    3)(x) == "  123.457"


