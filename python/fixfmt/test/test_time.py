import numpy as np
import fixfmt

NAT = np.datetime64("NAT")

def test_nat():
    arr = np.array([
        "1973-12-03T10:45:00",
        NAT,
        "2019-11-01T02:37:51.79211199",
    ], dtype="datetime64[ns]")

    fmt = fixfmt.npfmt.choose_formatter(arr)
    arr = arr.astype(int)
    assert fmt(arr[0]) == "1973-12-03T10:45:00.000000000+00:00"
    assert fmt(arr[1]) == "NaT                                "
    assert fmt(arr[2]) == "2019-11-01T02:37:51.792111990+00:00"


def test_nat_custom():
    arr = np.array([
        "1973-12-03T10:45:00",
        NAT,
        "2019-11-01T02:37:51.79211199",
    ], dtype="datetime64[ns]")

    fmt = fixfmt.TickTime(scale=1000000000, precision=6, nat="INVALID")
    arr = arr.astype(int)
    assert fmt(arr[0]) == "1973-12-03T10:45:00.000000+00:00"
    assert fmt(arr[1]) == "INVALID                         "
    assert fmt(arr[2]) == "2019-11-01T02:37:51.792112+00:00"
    

