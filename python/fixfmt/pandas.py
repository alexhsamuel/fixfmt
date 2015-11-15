import pandas as pd
import pln.ctr

from   . import table, Bool, Number, String

#-------------------------------------------------------------------------------

def from_dataframe(df, cfg, *, names=pln.ctr.ALL):
    tbl = table.Table(cfg)

    def get_values(series):
        if series.dtype.name == "category":
            # Construct an explicit array of values.
            # FIXME: Wasteful.  We should instead read through the categories.
            return series.cat.categories.values[series.cat.codes]
        else:
            return series.values

    if cfg.index.show:
        # FIXME: Handle multi-index.
        tbl.add_index_column(df.index.name, get_values(df.index))

    names = tuple(pln.ctr.select_ordered(df.columns, names))
    for name in names:
        series = df[name]
        tbl.add_column(series.name, get_values(series))

    tbl.finish()
    return tbl


#-------------------------------------------------------------------------------

def main():
    from   argparse import ArgumentParser
    from   pln.io import load_pickle

    # FIXME: Add format arguments.

    parser = ArgumentParser()
    parser.add_argument(
        "filename", metavar="FILENAME", 
        help="read from FILENAME")
    args = parser.parse_args()
    
    # FIXME
    cfg = table.DEFAULT_CFG
    cfg = table.UNICODE_BOX_CFG
    table._colorize(cfg)

    # FIXME: Support "-".
    df = load_pickle(args.filename)
    from_dataframe(df, cfg).print()


if __name__ == "__main__":
    main()


