from   fixfmt.lib.cfg import merge_dicts, unflatten_dict

#-------------------------------------------------------------------------------

def test_merge_dict():
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
    assert merge_dicts(d0, d1) == {
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


def test_unflatten_dict():
    assert unflatten_dict({
        "a": 1,
        "b": {"d": 3,},
        "b.c": 2,
        "b.e.f": 4,
        "b.g": {"h": 5, "i": 6},
        "j.k": {"l": 7},
        "j.k.m": 8,
        "b.d": 9,
        "b.g.n": 10,
    }) == {
        "a": 1,
        "b": {
            "c": 2,
            "d": 9,
            "e": {
                "f": 4,
            },
            "g": {
                "h": 5,
                "i": 6,
                "n": 10,
            },
        },
        "j": {
            "k": {
                "l": 7,
                "m": 8,
            },
        },
    }


