import pytest

from   fixfmt import *

#-------------------------------------------------------------------------------

def test_pad_length():
    assert pad("hello",  0) == "hello"
    assert pad("hello",  4) == "hello"
    assert pad("hello",  5) == "hello"
    assert pad("hello",  6) == "hello "
    assert pad("hello", 10) == "hello     "
    assert pad("hello", length=10) == "hello     "

    assert pad("", 0) == ""
    assert pad("", 5) == "     "


def test_pad_pad():
    assert pad("hello", 4, "x") == "hello"
    assert pad("hello", 6, "x") == "hellox"
    assert pad("hello", 9, "x") == "helloxxxx"
    assert pad("hello", 8, "o") == "helloooo"
    assert pad("hello", 8, "-") == "hello---"
    assert pad("hello", pad="-", length=8) == "hello---"


def test_pad_left():
    assert pad("hello",  4, left=True ) == "hello"
    assert pad("hello", 10, left=False) == "hello     "
    assert pad("hello", 10, left=True ) == "     hello"
    assert pad("hello", 10, left=True, pad="/") == "/////hello"


def test_elide_default():
    assert elide("I am a duck.",  8) == "I am a \u2026"
    assert elide("I am a duck.", 14) == "I am a duck."


def test_elide_length():
    assert elide("Hello, world!", 15, "...") == "Hello, world!"
    assert elide("Hello, world!", 13, "...") == "Hello, world!"
    assert elide("Hello, world!", 12, "...") == "Hello, wo..."
    assert elide("Hello, world!", 11, "...") == "Hello, w..."
    assert elide("Hello, world!", 10, "...") == "Hello, ..."
    assert elide("Hello, world!",  5, "...") == "He..."

    assert elide("foo", 3, "...") == "foo"
    assert elide("fool", 3, "...") == "..."


def test_elide_ellipsis():
    assert elide("Hello, world!", 10, "...") == "Hello, ..."
    assert elide("Hello, world!", 10, ".." ) == "Hello, w.."
    assert elide("Hello, world!", 10, "*"  ) == "Hello, wo*"
    assert elide("Hello, world!", 10, ""   ) == "Hello, wor"

    assert elide("Hello, world!", ellipsis="*", length=10) == "Hello, wo*"


def test_elide_position():
    assert elide("Hello, world!", 10, "...", 1.0) == "Hello, ..."
    assert elide("Hello, world!", 10, "...", 0.7) == "Hello...d!"
    assert elide("Hello, world!", 10, "...", 0.5) == "Hell...ld!"
    assert elide("Hello, world!", 10, "...", 0.4) == "Hel...rld!"
    assert elide("Hello, world!", 10, "...", 0.0) == "... world!"

    assert elide(
        "Hello, world!", position=0.4, length=10, ellipsis="..") == "Hel..orld!"


def test_palide_length():
    assert palide("Hello, world!",  3, "...") == "..."
    assert palide("Hello, world!", 10, "...") == "Hello, ..."
    assert palide("Hello, world!", 11, "...") == "Hello, w..."
    assert palide("Hello, world!", 13, "...") == "Hello, world!"
    assert palide("Hello, world!", 14, "...") == "Hello, world! "
    assert palide("Hello, world!", 20, "...") == "Hello, world!       "


def test_palide_ellipsis():
    assert palide("Hello, world!", 10, "~~~~~") == "Hello~~~~~"
    assert palide("Hello, world!", 10, "..."  ) == "Hello, ..."
    assert palide("Hello, world!", 10, ".."   ) == "Hello, w.."
    assert palide("Hello, world!", 10, ""     ) == "Hello, wor"


def test_palide_pad():
    assert palide("Hello, world!", 13, pad="x") == "Hello, world!"
    assert palide("Hello, world!", 18, pad="x") == "Hello, world!xxxxx"
    assert palide("Hello, world!", 18, pad="!") == "Hello, world!!!!!!"


def test_palide_position():
    assert palide("Hello, world!", 11, "..", position=0.0) == "..o, world!"
    assert palide("Hello, world!", 11, "..", position=0.6) == "Hello..rld!"
    assert palide("Hello, world!", 11, "..", position=0.8) == "Hello, ..d!"


def test_palide_args():
    assert palide(
        ellipsis="-//-", 
        length=20,
        pad="x",
        position=0.4,
        string="The quick brown fox jumped over the lazy dogs.",
        ) == "The qu-//-lazy dogs."


def test_palide_default():
    assert palide("I am a duck.",  8) == "I am a \u2026"
    assert palide("I am a duck.", 14) == "I am a duck.  "

