import numpy as np

import fixfmt
import fixfmt.npfmt

#-------------------------------------------------------------------------------

def test_choose_formatter_int():
    arr = np.arange(800, 1200)
    fmt = fixfmt.npfmt.choose_formatter(arr)
    assert isinstance(fmt, fixfmt.Number)
    assert fmt.size == 4
    assert fmt.precision is None


def test_choose_formatter_float0():
    arr = np.arange(1001) / 100
    fmt = fixfmt.npfmt.choose_formatter(arr)
    assert isinstance(fmt, fixfmt.Number)
    assert fmt.size == 2
    assert fmt.precision == 2


def test_choose_formatter_float1():
    arr = np.arange(1001) / 100000
    fmt = fixfmt.npfmt.choose_formatter(arr)
    assert isinstance(fmt, fixfmt.Number)
    assert fmt.size == 1
    assert fmt.precision == 5


def test_choose_formatter_inf():
    arr = np.array([3, 4, 5, np.inf])
    fmt = fixfmt.npfmt.choose_formatter(arr)
    assert isinstance(fmt, fixfmt.Number)
    assert fmt.width >= 3
    for val in arr:
        assert float(fmt(val)) == val


def test_choose_formatter_neginf():
    arr = np.array([3, 4, 5, -np.inf])
    fmt = fixfmt.npfmt.choose_formatter(arr)
    assert isinstance(fmt, fixfmt.Number)
    assert fmt.width >= 4
    for val in arr:
        assert float(fmt(val)) == val


def test_choose_formatter_float2():
    arr = np.arange(2e8, 3e8, 1234500) / 100000000
    fmt = fixfmt.npfmt.choose_formatter(arr)
    assert isinstance(fmt, fixfmt.Number)
    assert fmt.size == 1
    assert fmt.precision == 6


def test_choose_formatter_datetime1():
    t0 = np.datetime64("2018-01-18T17:05:00")
    t1 = np.datetime64("2018-01-18T18:05:02")
    arr = np.arange(t0, t1, np.timedelta64(100, "ms"))
    fmt = fixfmt.npfmt.choose_formatter(arr)
    assert isinstance(fmt, fixfmt.TickTime)
    assert fmt.scale == 1000
    assert fmt.precision == 1


def test_choose_formatter_datetime2():
    t0 = np.datetime64("2018-01-18T17:05:00")
    t1 = np.datetime64("2018-01-18T18:05:02")
    arr = np.arange(t0, t1, np.timedelta64(10000, "us"))
    fmt = fixfmt.npfmt.choose_formatter(arr)
    assert isinstance(fmt, fixfmt.TickTime)
    assert fmt.scale == 1000000
    assert fmt.precision == 2


def test_choose_formatter_date():
    t0 = np.datetime64("2018-01-01")
    t1 = np.datetime64("2019-01-01")
    arr = np.arange(t0, t1, np.timedelta64(1, "D"))
    fmt = fixfmt.npfmt.choose_formatter(arr)
    assert isinstance(fmt, fixfmt.TickDate)
    assert fmt(t0.astype(int)) == "2018-01-01"
    assert fmt.width == len(fmt(t1.astype(int)))


