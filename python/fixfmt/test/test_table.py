import numpy as np

from   fixfmt.table import Table

#-------------------------------------------------------------------------------

def test_array_dtypes():
    tbl = Table()

    for dtn in (
            "int8", "int16", "int32", "int64", 
            "uint8", "uint16", "uint32", "uint64",
            "float32", "float64",
    ):
        tbl.add_column(dtn, np.arange(100, 108, dtype=dtn))

    tbl.add_column("bool", np.arange(8) % 3 == 0)

    words = ["foo", "bar", "baz", "bif", "pineapple", "bom", "rok", "jip"]
    for dtn in (
            "S4", "S8", 
    ):
        tbl.add_column(dtn, np.array(words, dtype=dtn))

    words = ["foo", "bar", "baz", "bif", "pineapple", "xü§", "푒푓푔푒푓푔", "jip"]
    for dtn in (
            "U4", "U8",
            "object",
    ):
        tbl.add_column(dtn, np.array(words, dtype=dtn))

    tbl.print()


