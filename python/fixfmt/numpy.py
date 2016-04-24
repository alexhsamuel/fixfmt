import numpy as np

from   . import array
from   . import Number


#-------------------------------------------------------------------------------

def NdArray(fmt, cfg=array.DEFAULT_CFG, sep=','):
    """
    """
    arr = array.Array(fmt, cfg, sep)
    return arr


def main():
    #fmt = Number(2)
    #ndarray = np.empty(2)
    #f = NdArray(fmt)
    #f(ndarray)

    fmt = Number(2)
    ndarray = np.empty((4,3,6))
    f = NdArray(fmt)
    f(ndarray)


if __name__ == "__main__":
    main()

