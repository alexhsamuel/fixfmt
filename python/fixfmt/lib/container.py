"""
Containers.

A container is an object that supports `__contains__`. 
"""

#-------------------------------------------------------------------------------

import re
import six

if six.PY3:
    from collections.abc import Sequence
else:
    from collections import Sequence

__all__ = [
    "select",
    "select_ordered",
    "ALL",
    "NONE",
    "all_but",
]

#-------------------------------------------------------------------------------

def select(items, container, ctor=None):
    """
    Returns `items` that are in `container`.

    @param ctor
      Constructor for returned value.  If `None`, uses the type of `items`.
      Use `iter` to return an iterable.
    """
    if ctor is None:
        ctor = type(items)

    return ctor( i for i in items if i in container )


def select_ordered(items, container, ctor=None):
    """
    Returns `items` that are in `container`.

    If `container` is a sequence, returns them in the order they appear there.
    """
    if ctor is None:
        ctor = type(items)

    if isinstance(container, Sequence):
        # A frozenset would be more efficient, but there's no guarantee that
        # the items are hashable.
        members = tuple(items)
        result = ( i for i in container if i in members )
    else:
        result = ( i for i in items if i in container )
    return ctor(result)


#-------------------------------------------------------------------------------

class All:
    """
    Contains all itmes.
    """

    def __contains__(self, item):
        return True



ALL = All()


#-------------------------------------------------------------------------------

class None_:
    """
    Contains no items.
    """

    def __contains__(self, item):
        return False



NONE = None_()


#-------------------------------------------------------------------------------

class all_but:
    """
    Contains an item iff. the base container does not.
    """

    def __init__(self, container):
        """
        Creates a container that contains anything not in `container`.
        """
        self.__container = container


    def __contains__(self, item):
        return item not in self.__container



#-------------------------------------------------------------------------------

class only:
    """
    Contains only a particular item.
    """

    def __init__(self, item):
        self.__item = item


    def __contains__(self, item):
        return item == self.__item



#-------------------------------------------------------------------------------

class regex:
    """
    Contains an item iff. its `str` matches a regular expression.
    """

    def __init__(self, regex):
        self.__regex = re.compile(regex)


    def __contains__(self, item):
        return self.__regex.match(str(item)) is not None



