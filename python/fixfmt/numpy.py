"""
Script for quick testing.
"""

import numpy as np
import shutil
import math

from   . import Array, Number, String


if __name__ == "__main__":
    arr = np.zeros(10)
    fmt = Array(Number(3, 2), show_index=True, index_at_1=True, axis=1)
    print(fmt(arr))

