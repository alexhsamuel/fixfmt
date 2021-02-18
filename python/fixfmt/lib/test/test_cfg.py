from   fixfmt.lib.cfg import merge

#-------------------------------------------------------------------------------

def test_merge():
    d0 = {
        "a": 1,
        "b": {
            "c": 2,
            "d": {"e": 3, "f": 4},
            "g": {"h": 5, "i": 6},
        },
        "j": {
            "k": 7,
            "l": {"m": 8, "n": 9},
            "o": {"p": 10, "q": 11},
        },
        "r": 12,
        "s": 13,
    }
    d1 = {
        "a": 14,
        "b": {
            "d": 15,
            "g": {"h": 16, "u": 17},
        },
        "j": 18,
        "r": {"t": 19},
    }
    assert merge(d0, d1) == {
        "a": 14,
        "b": {
            "c": 2,
            "d": 15,
            "g": {"h": 16, "i": 6, "u": 17},
        },
        "j": 18,
        "r": {"t": 19},
        "s": 13
    }


