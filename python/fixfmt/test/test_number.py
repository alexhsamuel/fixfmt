#-*- encoding: utf-8 -*-

import pytest

from   fixfmt import Number

NAN = float("NaN")
INF = float("inf")

#-------------------------------------------------------------------------------

def test_precision():
    x = 123.45678910
    assert Number(4      )(x) == "  123"
    assert Number(4,   -1)(x) == "  123"
    assert Number(4, None)(x) == "  123"
    assert Number(4,    0)(x) == "  123."
    assert Number(4,    1)(x) == "  123.5"
    assert Number(4,    3)(x) == "  123.457"


def test_pad():
    assert Number(5)(10) == "    10"
    assert Number(5, pad="0")(10) == " 00010"

    with pytest.raises(ValueError):
        Number(5, pad="X")


def test_bad():
    assert Number(1)(10) == "##"
    assert Number(1, bad="?")(10) == "??"


def test_scale():
    assert Number(8, scale= "%"  )(1) == u"      100%"
    assert Number(8, scale= "c"  )(1) == u"      100c"
    assert Number(8, scale= "bps")(1) == u"    10000 bps"
    assert Number(8, scale=u"µ"  )(1) == u"  1000000µ"

    assert Number(1, 3, scale="Mi")(1000000) == " 0.954Mi"

    assert Number(5, 2, scale="k")(123456) == "   123.46k"
    assert Number(5, 2, scale="k")(NAN)    == "   NaN    "


def test_getters():
    f = Number(4, 3)
    assert f.bad == "#"
    assert f.inf == "inf"
    assert f.nan == "NaN"
    assert f.pad == " "
    assert f.point == "."
    assert f.precision == 3
    assert f.scale is None
    assert f.sign == "-"
    assert f.size == 4


def test_basic():
    f = Number(2, 4)
    assert f(  0     ) == "  0.0000"
    assert f(-99.9999) == "-99.9999"
    assert f(100     ) == "########"
    assert f(NAN     ) == "NaN     "
    assert f(INF     ) == " inf    "


def test_set_bad():
    f = Number(2, 4)
    f.bad = "?"
    assert f.bad == "?"
    assert f(  0     ) == "  0.0000"
    assert f(-99.9999) == "-99.9999"
    assert f(100     ) == "????????"
    assert f(NAN     ) == "NaN     "
    assert f(INF     ) == " inf    "

    with pytest.raises(ValueError):
        f.bad = None
    with pytest.raises(ValueError):
        f.bad = ""
    with pytest.raises(ValueError):
        f.bad = "!!"


def test_set_inf():
    f = Number(2, 4)
    f.inf = u"∞"
    assert f.inf == u"∞"
    assert f(  0     ) == "  0.0000"
    assert f(-99.9999) == "-99.9999"
    assert f(100     ) == "########"
    assert f(NAN     ) == "NaN     "
    assert f(INF     ) ==u"  ∞     "


def test_set_nan():
    f = Number(2, 4)
    f.nan = ""
    assert f.nan == ""
    assert f(  0     ) == "  0.0000"
    assert f(-99.9999) == "-99.9999"
    assert f(100     ) == "########"
    assert f(NAN     ) == "        "
    assert f(INF     ) == " inf    "


def test_set_pad():
    f = Number(2, 4)
    f.pad = "0"
    assert f.pad == "0"
    assert f(  0     ) == " 00.0000"
    assert f(-99.9999) == "-99.9999"
    assert f(100     ) == "########"
    assert f(NAN     ) == "NaN     "
    assert f(INF     ) == " inf    "

    with pytest.raises(ValueError):
        f.pad = None
    with pytest.raises(ValueError):
        f.pad = 10
    with pytest.raises(ValueError):
        f.pad = ""
    with pytest.raises(ValueError):
        f.pad = "x"
    with pytest.raises(ValueError):
        f.pad = "00"


def test_set_point():
    f = Number(2, 4)
    f.point = ","
    assert f.point == ","
    assert f(  0     ) == "  0,0000"
    assert f(-99.9999) == "-99,9999"
    assert f(100     ) == "########"
    assert f(NAN     ) == "NaN     "
    assert f(INF     ) == " inf    "

    with pytest.raises(ValueError):
        f.point = None
    with pytest.raises(ValueError):
        f.point = ""
    with pytest.raises(ValueError):
        f.point = "!!"

def test_set_precision():
    f = Number(2, 4)
    f.precision = 6
    assert f.precision == 6
    assert f(  0     ) == "  0.000000"
    assert f(-99.9999) == "-99.999900"
    assert f(100     ) == "##########"
    assert f(NAN     ) == "NaN       "
    assert f(INF     ) == " inf      "

    with pytest.raises(ValueError):
        f.precision = "full"


def test_set_scale():
    f = Number(2, 4)
    f.scale = "k"
    assert f.scale == (1000, "k")
    assert f(  0     ) == "  0.0000k"
    assert f(-99.9999) == " -0.1000k"
    assert f(100     ) == "  0.1000k"
    assert f(NAN     ) == "NaN      "
    assert f(INF     ) == " inf     "

    with pytest.raises(ValueError):
        f.scale = "really big"


def test_set_sign():
    f = Number(2, 4)
    f.sign = "+"
    assert f.sign == "+"
    assert f(  0     ) == " +0.0000"
    assert f(-99.9999) == "-99.9999"
    assert f(100     ) == "########"
    assert f(NAN     ) == "NaN     "
    assert f(INF     ) == "+inf    "

    with pytest.raises(ValueError):
        f.sign = "plus"
    with pytest.raises(ValueError):
        f.sign = "*"


def test_set_size():
    f = Number(2, 4)
    f.size = 4
    assert f.size == 4
    assert f(  0     ) == "    0.0000"
    assert f(-99.9999) == "  -99.9999"
    assert f(100     ) == "  100.0000"
    assert f(NAN     ) == "  NaN     "
    assert f(INF     ) == "  inf     "

    with pytest.raises(ValueError):
        f.size = -1
    with pytest.raises(TypeError):
        f.size = None
    with pytest.raises(ValueError):
        f.size = "uuuuge"


