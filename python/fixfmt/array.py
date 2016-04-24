"""
Array class and utility methods.
"""

from   contextlib import suppress

from   . import Bool, Number, String


#-------------------------------------------------------------------------------
# Configuration

#-------------------------------------------------------------------------------
# Array class definition

class Array:

    def __init__(self, fmt):
        self.__fmt = fmt
        # TODO: This should be configurable from the cfg object.
        self.__sep = ','


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

        >>> fmt = fixfmt.Number(4)
        >>> ndfmt = fixfmt.numpy.NdArray(fmt)
        >>> ndfmt(np.array(range(5)))
        ['    0.00', '    1.00', '    2.00', '    3.00', '    4.00']

        Multidimensional array:

        >>> fmt = fixfmt.numpy.NdArray(Number(4, 4))
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

        >>> fmt = fixfmt.String(8)
        >>> ndfmt = fixfmt.numpy.NdArray(fmt)
        >>> ndfmt(np.array([[1, 'hello, world'], [True, False]]))
        [
            ['1       ', 'hello, …'],
            ['True    ', 'False   ']
        ]
        """

        rank = len(a.shape)
        indent = ''
        s = self._format_array(a, rank, indent)
        print(s)


    def _format_array(self, a, rank, indent):
        """
        Prints numpy.ndarray using configured formatter.
        """

        if rank == 1:
            s = indent + str([self.__fmt(x) for x in a])
        else:
            s = indent + '[\n'
            for i in range(len(a), 1, -1):
                item = self._format_array(a[-i], rank-1, indent + '  ')
                s += item + self.__sep + '\n'
            s += self._format_array(a[-1], rank-1, indent + '  ')
            s += '\n' + indent + ']'
        return s

