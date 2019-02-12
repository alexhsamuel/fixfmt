import numpy as np
import pytest

import fixfmt.npfmt
from   fixfmt.npfmt import choose_formatter

#-------------------------------------------------------------------------------

@pytest.mark.parametrize(
    "arr,size,precision",
    [
        ([5.20934, 0, -2.1209, 100, 92.823], 3, 5),
        ([1, 23, 456, 7890, 12345, 678901, 2345678], 7, None),
        ([0.1, 0.2, 0.3, 0.5, 0.7, 0.8, 0.9], 1, 1),
        (np.arange(100) / 100, 1, 2),
        (np.arange(1000) / 1000, 1, 3),
        (np.arange(0, 100000, 23) / 100000, 1, 5),
        (np.arange(0, 100000000, 7919) / 100000000, 1, 8),
        (np.arange(0, 1000000000, 16127) / 1000000000, 1, 9),
        ([0.123456789, 0.000000001, 0.999999999], 1, 9),
    ]
)
def test_precision(arr, size, precision):
    arr = np.array(arr)
    fmt = choose_formatter(arr)
    assert isinstance(fmt, fixfmt.Number)
    assert fmt.size == size
    assert fmt.precision == precision


def test_non_contiguous():
    t0 = np.datetime64("2018-01-20T19:01:37")
    t1 = np.datetime64("2018-01-20T19:01:47")
    t2 = np.datetime64("2018-01-20T19:01:37.1")
    arr = np.rec.fromarrays(
        [
            np.arange(10),
            np.round(np.random.normal(3, 1, 10), 4),
            np.random.choice(["foo", "bar", "baz"], 10),
            np.arange(t0, t1, np.timedelta64(1, "s")),
            np.arange(t0, t2, np.timedelta64(10, "ms")),
        ],
        names=("i", "x", "label", "time", "fast")
    )

    fmt = choose_formatter(arr.i)
    assert isinstance(fmt, fixfmt.Number)
    assert fmt.size == 1
    assert fmt.precision is None

    fmt = choose_formatter(arr.x)
    assert isinstance(fmt, fixfmt.Number)
    assert fmt.size == 1
    assert fmt.precision == 4

    fmt = choose_formatter(arr.label)
    assert isinstance(fmt, fixfmt.String)
    assert fmt.size == 3

    fmt = choose_formatter(arr.time)
    assert isinstance(fmt, fixfmt.TickTime)
    assert fmt.precision == -1

    fmt = choose_formatter(arr.fast)
    assert isinstance(fmt, fixfmt.TickTime)
    assert fmt.precision == 2


