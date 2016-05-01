"""
Array class and utility methods.
"""

from   contextlib import suppress
import numpy as np
import shutil

from   . import Bool, Number, String


#-------------------------------------------------------------------------------
# Configuration

# TODO. See Array __init__ function.

#-------------------------------------------------------------------------------
# Array class definition


class Array:

    def __init__(self, fmt, axis=0, dim_sep=",", elem_sep=", ", 
                 show_index=False, index_at_1=False, ix_fmt=None):
        """
        Construct Array instance.
        """
        self.__fmt = fmt
        # TODO: These should be configurable from the cfg object.
        self.__indent_sym = " "
        self.__index_border = "|"
        self.__axis = axis
        self.__dim_sep = dim_sep
        self.__elem_sep = elem_sep
        self.__show_index = show_index
        self.__index_at_1 = index_at_1
        self.__ix_fmt = ix_fmt


    def __call__(self, arr):
        """
        Call Array formatter.
        """
        if type(arr) != np.ndarray:
            # numpy will cast an inconsistently typed list to Unicode strings.
            # Rather than check every type and change formatters, we insist
            # every element is the same type.
            #
            # TODO: Why can't the user format a multi-typed list with a string
            # formatter?
            if not _all_same_type(arr):
                raise TypeError("Every element must have the same type.")
            else:
                arr = np.array(arr)

        rank = len(arr.shape)
        # Skip first "[".
        # TODO: This should be configurable if no decoration is desired.
        indent = self.__indent_sym
        if rank == 1:
            s = "[" + self._format_vector(arr, indent) + "]"
        else:
            s = self._format_array(arr, rank, indent)
        return s


    def _format_array(self, arr, rank, indent):
        """
        Converts a numpy.ndarray to a formatted string.
        """
        # If rank == 1, there are no arrays nested in the current one. Just
        # print the vector.
        ind = self.__indent_sym
        if rank == 1:
            s = "[" + self._format_vector(arr, indent=indent) + "]"
        else:
            s = "["
            size = len(arr)

            # Loop through every array in reverse. We write the innermost
            # elements first and work our way back up the call stack.
            for i in range(size, 1, -1):
                # Skip indenting the first element in the array. We do not
                # want to write "[<indent>[", but rather "[[".
                if i != size:
                    s += indent
                word = self._format_array(arr[-i], rank-1, ind+indent)
                word = word + self.__dim_sep + '\n'
                s += word
            if size > 1:
                # Whenever we have more than one element in the current array,
                # we need to indent the last line. If size == 1, then we would
                # want to write "[" back-to-back.
                s += indent
            
            s += self._format_array(arr[-1], rank-1, ind+indent)
            s += "]"
        return s


    def _format_vector(self, arr, indent=""):
        """
        Converts a one-dimensional row vector to a formatted string.
        """
        if self.__axis == 1:
            return self._format_column_vector(arr)

        idx_fmt = self._get_index_formatter(arr)
        ix_width = self._get_index_width(idx_fmt)
        cwidth = self._get_column_width() + ix_width
        ind_width = len(indent)
        max_cols = shutil.get_terminal_size().columns
        
        s = ""
        end = ind_width
        for i, elem in enumerate(arr):
            end += cwidth
            if end > max_cols:
                s += "\n" + indent
                # Reset index to account for indent and column we are
                # about to add.
                end = ind_width + cwidth
            if self.__show_index:
                s += self._format_index(i, idx_fmt)
            s += self.__fmt(elem) + self.__elem_sep
        s = self._rm_trailing_sep(s)
        return s

    def _format_column_vector(self, arr):
        """
        Converts a one-dimensional column vector to a formatted
        string.
        """
        idx_fmt = self._get_index_formatter(arr)
        s = ""
        for i, elem in enumerate(arr):
            if self.__show_index:
                s += self._format_index(i, idx_fmt)
            s += self.__fmt(elem) + "\n" + self.__indent_sym
        s = self._rm_trailing_sep(s)
        return s

    
    def _get_column_width(self):
        """
        Returns width a formatted column, including separator if applicable.
        """
        return self.__fmt.width + len(self.__elem_sep)


    def _rm_trailing_sep(self, s):
        """
        Remove trailing element separator.
        """
        n = len(self.__elem_sep)
        return s[:-n]


    def _format_index(self, i, idx_fmt):
        """
        Returns formatted index, accounting for if the index begins at 0 or 1
        and the index border.
        """
        if self.__index_at_1:
            i += 1
        return idx_fmt(i) + self.__index_border


    def _get_index_formatter(self, arr):
        """
        Returns index formatter if necessary, None otherwise.
        """
        if not self.__show_index:
            return None
        if self.__ix_fmt:
            return self.__ix_fmt

        # If the index starts at 0, the width computed by len(str(len(arr)))
        # is off by 1. For example:
        #
        # >>> arr = list(range(10))
        # >>> len(str(len(arr)))
        # 2
        #
        # This is wrong since the max index is 9. This is why we subtract 1
        # below. But if the index begins at 1, 2 is the correct length since
        # the max index is 10.
        offset = 0 if self.__index_at_1 else 1
        width = len(str(len(arr) - offset))
        fmt = Number(width)
        return fmt

    def _get_index_width(self, fmt):
        """
        Returns index width accounting for border if necessary, 0 otherwise.
        """
        if not self.__show_index:
            return 0
        return fmt.width + len(self.__index_border)


# TODO: Implement in C++ for performance?
def _all_same_type(lst):
    """
    Return True if all elements are the same type, False otherwise.
    """
    type_ = type(lst[0])
    return all(isinstance(x, type_) for x in lst)

