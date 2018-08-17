import numpy as np

import fixfmt.npfmt
from   fixfmt.npfmt import choose_formatter

#-------------------------------------------------------------------------------

def test_number_0():
    a = np.array([5.20934, 0, -2.1209, 100, 92.823])
    fmt = choose_formatter(a)
    assert isinstance(fmt, fixfmt.Number)
    assert fmt.size == 3
    assert fmt.precision == 5


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


