import numpy as np

from   . import array
from   . import Number


#-------------------------------------------------------------------------------

def main():
    import math
    fmt = array.Array(Number(4, 4))
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

