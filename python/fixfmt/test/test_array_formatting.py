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
    Mock terminal size to make tests independent of actual terminal size.
    """

    class TerminalSize:
        columns = 50

    def mock():
        return TerminalSize()

    monkeypatch.setattr(shutil, 'get_terminal_size', mock)

#-------------------------------------------------------------------------------
# Tests

def test_shutil_mock(mock_terminal_size):
    pass


def test_1d_array(mock_terminal_size):
    fmt = Array(Number(3, 2))
    arr = np.array([8, math.pi, 4.88])
    assert fmt(arr) == "[   8.00,    3.14,    4.88]"
    fmt = Array(String(6))
    arr = np.array(["Aleksandrov", "Kolmogorov", "Lavrent'ev"])
    assert fmt(arr) == "[Aleks…, Kolmo…, Lavre…]"


def test_vertical_orientation(mock_terminal_size):
    fmt = Array(Number(3, 2), axis=1)
    arr = np.array([8, math.pi, 4.88])
    assert fmt(arr) == "[   8.00\n.   3.14\n.   4.88]"


def test_dimension_separator(mock_terminal_size):
    fmt = Array(Number(2), dim_sep=';')
    arr = np.array([[1,2,3], [4,55,6]])
    assert fmt(arr) == "[[  1,   2,   3];\n.[  4,  55,   6]]"


def test_element_separator(mock_terminal_size):
    fmt = Array(Number(3, 2), elem_sep='|')
    arr = np.array([8, math.pi, 4.88])
    assert fmt(arr) == "[   8.00|   3.14|   4.88]"


def test_element_separator(mock_terminal_size):
    fmt = Array(Number(3, 2), elem_sep='|')
    arr = np.array([8, math.pi, 4.88])
    assert fmt(arr) == "[   8.00|   3.14|   4.88]"


def test_wrap_1d_array(mock_terminal_size):
    fmt = Array(Number(3, 2))
    arr = np.zeros(20)
    assert fmt(arr) == ""\
        "[   0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        ".   0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        ".   0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        ".   0.00,    0.00,    0.00,    0.00,    0.00]"


def test_wrap_3d_array(mock_terminal_size):
    fmt = Array(Number(3, 2))
    arr = np.zeros((3,2,20))
    assert fmt(arr) == ""\
        "[[[   0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        "...   0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        "...   0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        "...   0.00,    0.00,    0.00,    0.00,    0.00],\n"\
        "..[   0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        "...   0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        "...   0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        "...   0.00,    0.00,    0.00,    0.00,    0.00]],\n"\
        ".[[   0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        "...   0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        "...   0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        "...   0.00,    0.00,    0.00,    0.00,    0.00],\n"\
        "..[   0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        "...   0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        "...   0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        "...   0.00,    0.00,    0.00,    0.00,    0.00]],\n"\
        ".[[   0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        "...   0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        "...   0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        "...   0.00,    0.00,    0.00,    0.00,    0.00],\n"\
        "..[   0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        "...   0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        "...   0.00,    0.00,    0.00,    0.00,    0.00, \n"\
        "...   0.00,    0.00,    0.00,    0.00,    0.00]]]"
    
