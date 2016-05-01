import math
import numpy as np
import pytest
import shutil

from   fixfmt import *

#-------------------------------------------------------------------------------
# Mocks

@pytest.fixture
def patch_shutil_get_terminal_size(monkeypatch):

    class TerminalSize:
        columns = 90

    def mock():
        return TerminalSize()

    monkeypatch.setattr(shutil, 'get_terminal_size', mock)

#-------------------------------------------------------------------------------
# Tests

def test_shutil_mock(patch_shutil_get_terminal_size):
    pass


def test_vertical_formatter():
    fmt  = NdArray(Number(4, 4), axis=1)
    arr  = np.array([8, math.pi, 4.88])
    assert fmt(arr) == "    8.0000\n    3.1416\n    4.8800"
