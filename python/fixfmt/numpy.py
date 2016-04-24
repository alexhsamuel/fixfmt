import numpy as np

from   . import array
from   . import Number


#-------------------------------------------------------------------------------

def NdArray(fmt):
    """
    Returns an Array formatter.
    """
    return array.Array(fmt)


def main():
    import math
    fmt = NdArray(Number(4, 4))
    arr = np.array([
        [
            [8, math.pi],
            [999999, -1]
        ],
        [
            [4.0, -2.0],
            [2, math.e]
        ],
    ])
    fmt(arr)


if __name__ == "__main__":
    main()

