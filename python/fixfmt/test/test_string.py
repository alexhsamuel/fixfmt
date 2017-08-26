#-*- encoding: utf-8 -*-

import pytest

from   fixfmt import String

#-------------------------------------------------------------------------------

def test_basic():
    fmt = String(8)
    assert fmt("hello")         ==  "hello   "
    assert fmt("Hello, world!") == u"Hello, …"


def test_stringify():
    fmt = String(8)
    assert fmt(None)            ==  "None    "
    assert fmt(10000)           ==  "10000   "
    assert fmt(1234567890)      == u"1234567…"
    assert fmt(False)           ==  "False   "
    assert fmt(object())        == u"<object…"


def test_getters():
    fmt = String(8)
    assert fmt.size == 8
    assert fmt.ellipsis == u"…"
    assert fmt.pad == " "
    assert fmt.elide_pos == 1
    assert fmt.pad_pos == 1


def test_setters():
    fmt = String(8)
    assert fmt("hello")         ==  "hello   "
    assert fmt("Hello, world!") == u"Hello, …"

    fmt.size = 10
    assert fmt.size == 10
    assert fmt("hello")         ==  "hello     "
    assert fmt("Hello, world!") == u"Hello, wo…"
    assert fmt.width == 10

    fmt.pad = "-+"
    assert fmt.pad == "-+"
    assert fmt("hello")         ==  "hello-+-+-"
    assert fmt("Hello, world!") == u"Hello, wo…"

    fmt.elide_pos = 0
    assert fmt.elide_pos == 0
    assert fmt("hello")         ==  "hello-+-+-"
    assert fmt("Hello, world!") == u"…o, world!"

    fmt.pad = "-"
    fmt.pad_pos = 0.4375
    assert fmt.pad_pos == 0.4375
    assert fmt("hello")         ==  "---hello--"
    assert fmt("Hello, world!") == u"…o, world!"

    fmt.ellipsis = "::"
    assert fmt.ellipsis == "::"
    assert fmt("hello")         ==  "---hello--"
    assert fmt("Hello, world!") ==  "::, world!"


