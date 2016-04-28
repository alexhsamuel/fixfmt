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

IND = " "
# TODO: This should be configurable from the cfg object.
SEP = ","

class Array:

    def __init__(self, fmt, axis):
        self.__fmt = fmt
        self.__axis = axis


    def __call__(self, a):
        """
        Call Array formatter.
        
        Parameters
        ----------
        a : numpy.ndarray
            Array to be formatted.

        Examples
        --------
        One-sizesional array:

        >>> fmt = fixfmt.numpy.NdArray( fixfmt.Number(4) )
        >>> fmt(np.array(range(5)))
        ['    0.00', '    1.00', '    2.00', '    3.00', '    4.00']

        Multisizesional array:

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
            s = self._format_vector(a, is_1d=True)
        else:
            indent = IND
            s = self._format_array(a, rank, indent)

        print()
        print(s)


    def _format_array(self, a, rank, indent):
        """
        Converts numpy.ndarray to formatted string using configured formatter.
        """

        # If rank == 1, there are no arrays nested in the current one. Just
        # print the vector.
        if rank == 1:
            s = "[" + self._format_vector(a) + "]"
        else:
            s = "["
            size = len(a)

            # Loop through every array in reverse. We write the innermost
            # elements first and work our way back up the call stack.
            for i in range(size, 1, -1):
                # Skip indenting the first element in the array. We do not
                # want to write "[<indent>[", but rather "[[".
                if i != size:
                    s += indent
                word = self._format_array(a[-i], rank-1, IND+indent)
                word = word + SEP + '\n'
                s += word
            if size > 1:
                # Whenever we have more than one element in the current array,
                # we need to indent the last line. If size == 1, then we would
                # want to write "[" back-to-back.
                s += indent

            s += self._format_array(a[-1], rank-1, IND+indent)
            s += "]"
        return s


    def _format_vector(self, a, is_1d=False):
        """
        Converts a 1-vector numpy.ndarray to a formatted string using
        configured formatter.
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
                word = self.__fmt(x)
                if is_1d and self.__axis == 0:
                    word += " "
                s += word
            return s
        elif self.__axis == 1:
            return "\n".join([self.__fmt(x) for x in a])


    def _write_line():
        """
        Writes a single line of the array, handling column widths and newlines.
        """
        pass

