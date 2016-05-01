import numpy as np
import shutil
import math

from   . import array
from   . import Number, String


#-------------------------------------------------------------------------------

def _l(arr, axis=1):
    rank = len(arr.shape)
    axis_str = 'horizontal' if axis == 1 else 0
    type_ = arr.dtype.name
    msg = '%sD array, %s, %s' % (rank, axis_str, type_)
    print('\n')
    print('=' * shutil.get_terminal_size().columns)
    print(msg)
    print('-' * shutil.get_terminal_size().columns)


def NdArray(fmt, axis=0):
    """
    Returns an Array formatter.
    """
    return array.Array(fmt, axis)


def main():
    """
    Tests.
    """

    # TODO: Tests for when terminal is not wide enough. Should wrap automatically, e.g.:
    # [[    8.0000     3.1416 ##########    -1.0000     4.0000    -2.0000 
    #       2.0000     2.7183]]


    fmt  = NdArray(Number(4, 4), axis=1)
    arr  = np.array([8, math.pi, 4.88])
    _l(arr=arr, axis=1)
    fmt(arr)

    fmt = NdArray(Number(4, 4), axis=0)
    arr = np.array([8, math.pi, 4.88])
    _l(arr)
    fmt(arr)

    fmt = NdArray(String(10), axis=1)
    arr = np.array(['hello, world', 'programming', 'mathematics', 'javascript', 'shakespeare', 'tolkien', 'piketty', 'jamesbaldwin', 'alekandrov', 'kolomgorovo', 'lavrentev'])
    _l(arr)
    fmt(arr)

    fmt = NdArray(String(10), axis=0)
    arr = np.array(['hello, world', 'programming', 'mathematics', 'javascript', 'shakespeare', 'tolkien', 'piketty', 'jamesbaldwin', 'alekandrov', 'kolomgorovo', 'lavrentev'])
    _l(arr)
    fmt(arr)

    fmt = NdArray(Number(4, 4))
    arr = np.array([[8, math.pi,999999, -1],[4.0, -2.0,2, math.e]])
    _l(arr)
    fmt(arr)

    arr = np.array([[[8, math.pi],[999999, -1],[4.0, -2.0],[2, math.e]]])
    _l(arr)
    fmt(arr)
    
    arr = np.array([[[8, math.pi],[999999, -1]],[[4.0, -2.0],[2, math.e]]])
    _l(arr)
    fmt(arr)

    arr = np.array(np.empty((4,9,2)))
    _l(arr)
    fmt(arr)

    arr = np.array(np.empty((1,5,5)))
    _l(arr)
    fmt(arr)

    arr = np.array(np.empty((1,5,20)))
    _l(arr)
    fmt(arr)   

if __name__ == "__main__":
    main()

