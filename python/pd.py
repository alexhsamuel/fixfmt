from   pathlib import Path
import pickle
import re
import sys
import time

import numpy as np
import shutil

import pandas as pd

import fixfmt

#-------------------------------------------------------------------------------

def get_terminal_width():
    return shutil.get_terminal_size().columns



def get_default_formatter(name, arr):
    if arr.dtype.kind == "i":
        return fixfmt.Number(6)
    elif arr.dtype.kind == "f":
        return fixfmt.Number(6, 6)
    elif arr.dtype.kind == "b":
        return fixfmt.Bool()
    elif arr.dtype.kind == "O":
        return fixfmt.String(16)
    else:
        raise TypeError("no default formatter for {}".format(arr.dtype))


def print_df(df, formatters={}):
    table = fixfmt.Table()
    for name in df.columns:
        arr = df[name].values
        dtype = arr.dtype

        fmt = None
        for key, value in formatters.items():
            if re.match(key, name) is not None:
                fmt = value
                break
        if fmt is None:
            fmt = formatters.get(arr.dtype, None)
        if fmt is None:
            fmt = get_default_formatter(name, arr)

        if dtype == np.dtype("int8"):
            table.add_int8(arr, fmt)
        elif dtype == np.dtype("int16"):
            table.add_int16(arr, fmt)
        elif dtype == np.dtype("int32"):
            table.add_int32(arr, fmt)
        elif dtype == np.dtype("int64"):
            table.add_int64(arr, fmt)
        elif dtype == np.dtype("float32"):
            table.add_float32(arr, fmt)
        elif dtype == np.dtype("float64"):
            table.add_float64(arr, fmt)
        elif dtype == np.dtype("bool"):
            table.add_bool(arr, fmt)
        elif dtype == np.dtype("object"):
            table.add_str_object(arr, fmt)

        table.add_string(" | ")

    for i in range(len(table)):
        print(table(i))


if __name__ == "__main__":
    # df = pd.read_csv("/Users/samuel/github/twosigma/ngrid/test/sample0.csv")
    path = Path(__file__).parent.parent / "test1.pickle"
    with path.open("rb") as file:
        df = pickle.load(file)
    start = time.time()
    print_df(df)
    print("{:d} rows {:.1f} rows/s".format(len(df), len(df) / (time.time() - start)), file=sys.stderr)

