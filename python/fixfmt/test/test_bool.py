import pytest

from   fixfmt import Bool

#-------------------------------------------------------------------------------

def test_basic():
    fmt = Bool("TRU", "FALSE", size=9, pos=0.5)
    assert fmt(True ) == "   TRU   "
    assert fmt(False) == "  FALSE  "
    assert fmt(1)     == "   TRU   "
    assert fmt(0)     == "  FALSE  "


def test_getters():
    fmt = Bool("TRU", "FALSE", size=9, pos=0.5)
    assert fmt.true == "TRU"
    assert fmt.false == "FALSE"
    assert fmt.size == 9
    assert fmt.pos == 0.5
    assert fmt.width == 9


def test_setters():
    fmt = Bool("TRU", "FALSE", size=9, pos=0.5)
    assert fmt(True ) == "   TRU   "
    assert fmt(False) == "  FALSE  "

    fmt.true = "FO SURE"
    assert fmt.true == "FO SURE"
    assert fmt(True ) == " FO SURE "
    assert fmt(False) == "  FALSE  "

    fmt.false = "NOT"
    assert fmt.false == "NOT"
    assert fmt(True ) == " FO SURE "
    assert fmt(False) == "   NOT   "

    fmt.size = 11
    assert fmt.size == 11
    assert fmt(True ) == "  FO SURE  "
    assert fmt(False) == "    NOT    "

    with pytest.raises(ValueError):
        fmt.size = -5
    with pytest.raises(ValueError):
        fmt.size = "foobar"
    with pytest.raises(TypeError):
        fmt.size = None

    fmt.pos = 0
    assert fmt.pos == 0
    assert fmt(True ) == "    FO SURE"
    assert fmt(False) == "        NOT"

    with pytest.raises(ValueError):
        fmt.pos = -1
    with pytest.raises(ValueError):
        fmt.pos = 1.1
    with pytest.raises(TypeError):
        fmt.pos = None


