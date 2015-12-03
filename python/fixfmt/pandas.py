import pandas as pd
import pln.container

from   . import table, Bool, Number, String

#-------------------------------------------------------------------------------

def from_dataframe(df, cfg, *, names=pln.container.ALL):
    tbl = table.Table(cfg)

    def get_values(series):
        if series.dtype.name == "category":
            # Construct an explicit array of values.
            # FIXME: Wasteful.  We should instead read through the categories.
            return series.cat.categories.values[series.cat.codes]
        else:
            return series.values

    if cfg.index.show:
        idx = df.index
        if isinstance(idx, pd.core.index.MultiIndex):
            for name, labels, levels in zip(idx.names, idx.labels, idx.levels):
                # FIXME: Wasteful.  Handle like category columns.
                tbl.add_index_column(name, levels.values[labels])
        else:
            tbl.add_index_column(idx.name, get_values(idx))

    names = pln.container.select_ordered(tuple(df.columns), names)
    for name in names:
        series = df[name]
        arr = get_values(series)
        tbl.add_column(series.name, arr)

    tbl.finish()
    return tbl


def print_dataframe(df, cfg=table.DEFAULT_CFG, *, names=pln.container.ALL):
    tbl = from_dataframe(df, cfg, names=names)
    tbl.print()


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
    print_dataframe(df, cfg)


if __name__ == "__main__":
    main()


