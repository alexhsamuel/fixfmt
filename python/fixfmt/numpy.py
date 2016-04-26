import numpy as np

from   . import array
from   . import Number, String


#-------------------------------------------------------------------------------

def NdArray(fmt, axis=0):
    """
    Returns an Array formatter.
    """
    return array.Array(fmt, axis)


def main():
    import math

    #fmt = NdArray(Number(4, 4), axis=1)
    #arr = np.array([8, math.pi, 4.88])
    #fmt(arr)
    #print('')
#
#    fmt = NdArray(Number(4, 4), axis=0)
#    arr = np.array([8, math.pi, 4.88])
#    fmt(arr)
#    print('')
#
    fmt = NdArray(String(10), axis=1)
    arr = np.array(['hello, world', 'programming', 'mathematics', 'javascript', 'shakespeare', 'tolkien', 'piketty', 'jamesbaldwin', 'alekandrov', 'kolomgorovo', 'lavrentev'])
    fmt(arr)
    print('')

    fmt = NdArray(String(10), axis=0)
    arr = np.array(['hello, world', 'programming', 'mathematics', 'javascript', 'shakespeare', 'tolkien', 'piketty', 'jamesbaldwin', 'alekandrov', 'kolomgorovo', 'lavrentev'])
    fmt(arr)

    #fmt = NdArray(Number(4, 4), axis=0)
    #arr = np.array(list(range(10)))
    #fmt(arr)

if __name__ == "__main__":
    main()

