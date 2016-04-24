from   pln.cfg import Group


#-------------------------------------------------------------------------------
# Configuration

DEFAULT_CFG = Group()


#-------------------------------------------------------------------------------

class Array:

    def __init__(self, fmt, cfg, sep):
        # TODO: A single formatter is too simplistic.
        self.__fmt = fmt
        self.__cfg = cfg
        self.__sep = sep


    def __call__(self, a):
        """
        Call Array formatter.
        
        Parameters
        ----------
        a : numpy.ndarray
            Array to be formatted.

        Examples
        --------

        >>> fmt = fixfmt.Number(2)
        >>> ndfmt = fixfmt.numpy.NdArray(fmt)
        >>> ndfmt(np.array(range(10)))
        ['  0', '  1', '  2', '  3', '  4', '  5', '  6', '  7', '  8', '  9']
        """

        rank = len(a.shape)
        fmt = self.__fmt
        s = self._format_array(a, rank)
        print(s)


    def _format_array(self, a, rank):
        """
        Prints numpy.ndarray using configured formatter.
        """

        # TODO: Array should be highly configurable, i.e. type- or column-
        #       dependent.
        fmt = self.__fmt
        sep = self.__sep

        if rank == 1:
            s = str([fmt(x) for x in a])
        else:
            s = '['
            for i in range(len(a), 1, -1):
                item = self._format_array(a[-i], rank-1)
                s += item + sep

            s += self._format_array(a[-1], rank-1)
            s += ']'
        return s

