import math
import numpy as np
import pytest
import shutil

from   fixfmt import *

#-------------------------------------------------------------------------------
# Mocks

@pytest.fixture
def mock_terminal_size(monkeypatch):
    """
    Mock to make tests independent of actual terminal size.
    """

    class TerminalSize:
        columns = 50
        lines = 50

    def mock():
        return TerminalSize()

    monkeypatch.setattr(shutil, "get_terminal_size", mock)


#-------------------------------------------------------------------------------
# Tests

def test_row_vector(mock_terminal_size):
    fmt = Array(Number(3, 2))
    arr = np.array([8, math.pi, 4.88])
    assert fmt(arr) == "[   8.00,    3.14,    4.88]"
    fmt = Array(String(6))
    arr = np.array(["Aleksandrov", "Kolmogorov", "Lavrent'ev"])
    assert fmt(arr) == "[Aleks…, Kolmo…, Lavre…]"


def test_column_vector(mock_terminal_size):
    fmt = Array(Number(3, 2), print_col_vec=True)
    arr = np.array([8, math.pi, 4.88])
    assert fmt(arr) == ""\
        "[   8.00\n"\
        "    3.14\n"\
        "    4.88]"


def test_dimension_separator(mock_terminal_size):
    arr = np.array([[1,2,3], [4,55,6]])
    fmt = Array(Number(2))
    assert fmt(arr) == ""\
        "[[  1,   2,   3],\n"\
        " [  4,  55,   6]]"
    fmt = Array(Number(2), dim_sep=';')
    assert fmt(arr) == ""\
        "[[  1,   2,   3];\n"\
        " [  4,  55,   6]]"


def test_element_separator(mock_terminal_size):
    arr = np.array([8, math.pi, 4.88])
    fmt = Array(Number(3, 2))
    assert fmt(arr) == "[   8.00,    3.14,    4.88]"
    fmt = Array(Number(3, 2), elem_sep='|')
    assert fmt(arr) == "[   8.00|   3.14|   4.88]"


def test_wrap_1d_array(mock_terminal_size):
    fmt = Array(Number(3, 2))
    arr = np.zeros(20)
    assert fmt(arr) == ""\
        "[   0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        "    0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        "    0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        "    0.00,    0.00,    0.00,    0.00,    0.00]"


def test_wrap_3d_array(mock_terminal_size):
    fmt = Array(Number(3, 2))
    arr = np.zeros((3,2,20))
    assert fmt(arr) == ""\
        "[[[   0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        "      0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        "      0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        "      0.00,    0.00,    0.00,    0.00,    0.00],\n"\
        "  [   0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        "      0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        "      0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        "      0.00,    0.00,    0.00,    0.00,    0.00]],\n"\
        " [[   0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        "      0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        "      0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        "      0.00,    0.00,    0.00,    0.00,    0.00],\n"\
        "  [   0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        "      0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        "      0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        "      0.00,    0.00,    0.00,    0.00,    0.00]],\n"\
        " [[   0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        "      0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        "      0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        "      0.00,    0.00,    0.00,    0.00,    0.00],\n"\
        "  [   0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        "      0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        "      0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        "      0.00,    0.00,    0.00,    0.00,    0.00]]]"
    

def test_index(mock_terminal_size):
    arr = np.zeros(10)
    fmt = Array(Number(3, 2), show_index=True)
    assert fmt(arr) == ""\
        "[ 0|   0.00,  1|   0.00,  2|   0.00,  3|   0.00, \n"\
        "  4|   0.00,  5|   0.00,  6|   0.00,  7|   0.00, \n"\
        "  8|   0.00,  9|   0.00]"


def test_index_custom_formatter(mock_terminal_size):
    arr = np.zeros(10)
    fmt = Array(Number(3, 2), show_index=True, ix_fmt=Number(5,3))
    assert fmt(arr) == ""\
        "[     0.000|   0.00,      1.000|   0.00, \n"\
        "      2.000|   0.00,      3.000|   0.00, \n"\
        "      4.000|   0.00,      5.000|   0.00, \n"\
        "      6.000|   0.00,      7.000|   0.00, \n"\
        "      8.000|   0.00,      9.000|   0.00]"


def test_index_at_1(mock_terminal_size):
    arr = np.zeros(10)
    fmt = Array(Number(3, 2), show_index=True, index_at_1=True)
    assert fmt(arr) == ""\
        "[  1|   0.00,   2|   0.00,   3|   0.00, \n"\
        "   4|   0.00,   5|   0.00,   6|   0.00, \n"\
        "   7|   0.00,   8|   0.00,   9|   0.00, \n"\
        "  10|   0.00]"


def test_index_column_vector(mock_terminal_size):
    arr = np.zeros(10)
    fmt = Array(Number(3, 2), print_col_vec=True, show_index=True)
    assert fmt(arr) == ""\
        "[ 0|   0.00\n"\
        "  1|   0.00\n"\
        "  2|   0.00\n"\
        "  3|   0.00\n"\
        "  4|   0.00\n"\
        "  5|   0.00\n"\
        "  6|   0.00\n"\
        "  7|   0.00\n"\
        "  8|   0.00\n"\
        "  9|   0.00]"


def test_cast_sequence_all_same_type(mock_terminal_size):
    fmt = Array(Number(3))
    arr = [0, 1, 2, 3, 4]
    assert fmt(arr) == "[   0,    1,    2,    3,    4]"
    arr = (0, 1, 2, 3, 4)
    assert fmt(arr) == "[   0,    1,    2,    3,    4]"
    arr = range(5)
    assert fmt(arr) == "[   0,    1,    2,    3,    4]"

def test_raise_type_error_if_sequence_not_all_same_type(mock_terminal_size):
    fmt = Array(Number(3))
    arr = [1, 2, 'foo']
    with pytest.raises(TypeError) as e:
        fmt(arr)
    arr = (1, 2, 'foo')
    with pytest.raises(TypeError) as e:
        fmt(arr)
    # Cannot have multi-typed range.
