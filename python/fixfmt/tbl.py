# FIXME: Merge this with fixfmt.table!

from   __future__ import absolute_import, division, print_function

from   . import palide
from   .lib import ansi, box
from   .table import _get_header_position  # FIXME

#-------------------------------------------------------------------------------

class _Table:
    """
    Base class for tables.
    """

    @staticmethod
    def _normalize_column(col):
        try:
            name, fmt = col
        except TypeError:
            if callable(fmt):
                name = getattr(fmt, "name", "col{}".format(i))
            else:
                raise TypeError(
                    "column is not a pair or callable: {}".format(col))
        return fmt, name


    def __init__(self, columns):
        columns = ( self._normalize_column(c) for c in columns )
        self._fmts, names = zip(*columns)
        self._names = [
            palide(n, f.width, pad_position=_get_header_position(f))
            for f, n in zip(self._fmts, names)
        ]


    def generate(self, rows):
        def gen():
            yield self.top()
            yield self.header()
            yield self.line()
            for row in rows:
                yield self.row(*row)
            yield self.bottom()

        return ( l for l in gen() if l is not None )


    def print(self, rows, print=print):
        for line in self.generate(rows):
            print(line)



class TextTable(_Table):
    """
    Simple table using plain ASCII decorations."
    """

    def top(self):
        return None


    def header(self):
        return " ".join(self._names)


    def line(self):
        return " ".join( "-" * f.width for f in self._fmts )


    def row(self, *values):
        return " ".join( f(v) for f, v in zip(self._fmts, values) )


    def bottom(self):
        return None



class BoxTable(_Table):
    """
    Table framed with Unicode box drawing characters.
    """

    DEFAULT_CONFIG = dict(
        box_line=(box.SINGLE, None),
        sep_line=box.SINGLE,
        style=ansi.style(fg="light_gray"),
        pad=" ",
    )

    def __init__(self, columns, config={}):
        super().__init__(columns)

        self.__cfg = dict(self.DEFAULT_CONFIG)
        self.__cfg.update(config)
        
        self.__frame = box.Frame(
            ( box.Frame.Column(
                f.width, sep=self.__cfg["sep_line"], pad=self.__cfg["pad"])
              for f in self._fmts ),
            edge=self.__cfg["box_line"], 
            style=self.__cfg["style"])


    def top(self):
        return self.__frame.top()


    def header(self):
        return self.__frame.row(self._names)


    def line(self):
        return self.__frame.line(box.SINGLE)


    def row(self, *values):
        return self.__frame.row([ f(v) for f, v in zip(self._fmts, values) ])


    def bottom(self):
        return self.__frame.bottom()



