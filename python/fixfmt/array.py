"""
Array class and utility methods.
"""

from   contextlib import suppress
import numpy as np
import shutil

from   . import Bool, Number, String


#-------------------------------------------------------------------------------
# Configuration

#-------------------------------------------------------------------------------
# Array class definition

class Array:

    def __init__(self, fmt, axis):
        self.__fmt = fmt
        self.__axis = axis
        # TODO: This should be configurable from the cfg object.
        self.__sep = ","


    def __call__(self, a):
        """
        Call Array formatter.
        
        Parameters
        ----------
        a : numpy.ndarray
            Array to be formatted.

        Examples
        --------
        One-dimensional array:

        >>> fmt = fixfmt.numpy.NdArray( fixfmt.Number(4) )
        >>> fmt(np.array(range(5)))
        ['    0.00', '    1.00', '    2.00', '    3.00', '    4.00']

        Multidimensional array:

        >>> fmt = fixfmt.numpy.NdArray( Number(4, 4) )
        >>> arr = np.array(
        ...     [[[8, math.pi], [999999, -1]], [[4.0, -2.0], [2, math.e]]]
        ... )
        ...
        >>> fmt(arr)
        [
            [
                ['    8.0000', '    3.1416'],
                ['##########', '   -1.0000']
            ],
            [
                ['    4.0000', '   -2.0000'],
                ['    2.0000', '    2.7183']
            ]
        ]

        Multi-typed array:

        >>> fmt = fixfmt.numpy.NdArray( fixfmt.String(8) )
        >>> fmt(np.array([[1, "hello, world"], [True, False]]))
        [
            ['1       ', 'hello, …'],
            ['True    ', 'False   ']
        ]
        """
        
        rank = len(a.shape)
        if rank == 1:
            s = self._format_vector(a)
        else:
            indent = " " * rank
            s = self._format_array0(a, rank, indent)
        print(s)


    def _format_array0(self, a, rank, indent):
        """
        Converts numpy.ndarray to formatted string using configured formatter.
        """

        if rank == 1:
            s = self._format_vector(a)
        else:
            s = "["
            for i in range(len(a), 1, -1):
                elem = self._format_array0(a[-i], rank-1, indent)
                s += elem + self.__sep + '\n'
            s += self._format_array(a[-1], rank-1, indent)
            s += "]"
        return s


    def _format_vector(self, a):
        """
        """

        cwidth = self.__fmt.width + 1
        max_cols = shutil.get_terminal_size().columns
        if self.__axis == 0:
            s = ""
            i = 0
            for x in a:
                i += cwidth
                if i > max_cols:
                    s += "\n"
                    i = 0
                s += self.__fmt(x) + " "
            return s
        elif self.__axis == 1:
            return "\n".join([self.__fmt(x) for x in a])

