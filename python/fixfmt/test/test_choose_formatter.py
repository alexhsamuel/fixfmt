from   __future__ import absolute_import, division, print_function

import numpy as np

import fixfmt
import fixfmt.npfmt

#-------------------------------------------------------------------------------

def test_number_0():
    a = np.array([5.20934, 0, -2.1209, 100, 92.823])
    fmt = fixfmt.npfmt.choose_formatter(a)
    assert isinstance(fmt, fixfmt.Number)
    assert fmt.size == 3
    assert fmt.precision == 5


