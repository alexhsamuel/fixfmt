import numpy as np

#-------------------------------------------------------------------------------

EMPTY   = np.uint8(0)
VALUE   = np.uint8(1)
NAME    = np.uint8(2)
LINE    = np.uint8(3)
STRING  = np.uint8(4)

CONTENT_DTYPE = np.dtype("u8")



class Cols:
    """
    Proxy object for manipulating cols.
    """

    def __init__(self):
        self.__cols = {}


    def set(self, name, arr, *, mask=None):
        arr = np.asarray(arr)
        assert len(arr.shape) == 1
        # FIXME: Check length against other colums

        if mask is None:
            con = np.full(arr.shape, VALUE, CONTENT_DTYPE)
        else:
            mask = np.asarray(mask)
            assert mask.shape == arr.shape
            con = np.where(mask, VALUE, EMPTY)
            assert con.dtype == CONTENT_DTYPE
        self.__cols[name] = Table.Col(arr, con)



class Table:

    class Col:

        def __init__(self, arr, con):
            self.arr = arr
            self.con = con



    def __init__(self):
        self.cols = self.Cols()
        self.__cols = self.cols._Cols__cols


    def __iter__(self):
        if len(self.__cols) == 0:
            return

        # Header.
        yield " | ".join( format(n, "20s") for n in self.__cols )
        yield " | ".join( "-" * 20 for n in self.__cols )

        # Rows.
        cols = list(self.__cols.values())
        n = len(cols[0].arr)
        for i in range(n):
            parts = []
            for c in cols:
                con = c.con[i]
                if con == EMPTY:
                    parts.append(" " * 20)
                elif con == VALUE:
                    parts.append(format(c.arr[i], "20.10f"))
                else:
                    assert False
            yield " | ".join(parts)


    def print(self, print=print):
        for line in self:
            print(line)


