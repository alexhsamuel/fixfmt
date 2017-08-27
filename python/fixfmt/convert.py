"""
Automatic array type conversion.
"""

import numpy as np

#-------------------------------------------------------------------------------

def _convert_to_int(arr):
    """
    Tries to convert `arr` to an int array.
    """
    empty = arr == ""
    if empty.all():
        raise ValueError("no non-empty values")
    return 


def _convert_to_float(arr):
    """
    Tries to convert `arr` to a float array, interpreting empty strings as NaN.
    """
    empty = arr == ""
    if empty.all():
        raise ValueError("no non-empty values")

    val         = np.empty(arr.shape, dtype=float)
    val[~empty] = arr[~empty].astype(float)
    val[ empty] = np.nan
    return val


def guess_convert(arr):
    """
    Attempt to convert `arr` to "more specific" types.
    """
    arr = np.asarray(arr)

    try:
        return arr.astype(int)
    except ValueError:
        pass

    try:
        return _convert_to_float(arr)
    except ValueError:
        pass

    try:
        return arr.astype(bool)
    except ValueError:
        pass

    return arr


