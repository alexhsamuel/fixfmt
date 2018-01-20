from   __future__ import absolute_import, division, print_function

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
    arr = np.rec.fromarrays(
        [
            np.arange(10),
            np.round(np.random.normal(3, 1, 10), 4),
            np.random.choice(["foo", "bar", "baz"], 10),
        ],
        names=("i", "x", "label")
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



