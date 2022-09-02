import numpy as np
import fixfmt
import fixfmt.npfmt

NAT = np.datetime64("NAT")

def test_nat():
    arr = np.array([
        "1973-12-03T10:45:00",
        NAT,
        "2019-11-01T02:37:51.79211199",
    ], dtype="datetime64[ns]")

    fmt = fixfmt.npfmt.choose_formatter(arr)
    assert fmt(arr[0]) == "1973-12-03T10:45:00.000000000+00:00"
    assert fmt(arr[1]) == "NaT                                "
    assert fmt(arr[2]) == "2019-11-01T02:37:51.792111990+00:00"


