"""
Array class and utility methods.
"""

from   contextlib import suppress
import numpy as np
import shutil

from   . import Bool, Number, String


#-------------------------------------------------------------------------------
# Configuration

#-------------------------------------------------------------------------------
# Array class definition


class Array:

    def __init__(self, fmt, axis, dim_sep=",", elem_sep="|"):
        self.__fmt = fmt
        self.__axis = axis
        # This should be a space in production mode.
        self.__indent_sym = "." 
        # TODO: These should be configurable from the cfg object.
        self.__dim_sep = dim_sep
        self.__elem_sep = elem_sep


    def __call__(self, arr):
        """
        Call Array formatter.
        """
        rank = len(arr.shape)
        # Skip first "[".
        # TODO: This should be configurable if no decoration is desired.
        indent = self.__indent_sym
        if rank == 1:
            s = self._format_vector(arr, indent, is_1d=True)
        else:
            s = self._format_array(arr, rank, indent)
        return s


    def _format_array(self, arr, rank, indent):
        """
        Converts numpy.ndarray to formatted string using configured formatter.
        """
        # If rank == 1, there are no arrays nested in the current one. Just
        # print the vector.
        ind = self.__indent_sym
        if rank == 1:
            s = "[" + self._format_vector(arr, indent=ind+indent) + "]"
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


    def _format_vector(self, arr, indent="", is_1d=False):
        """
        Converts a 1-vector numpy.ndarray to a formatted string using
        configured formatter.
        """
        cwidth = self._get_column_width()
        iwidth = len(indent)
        max_cols = shutil.get_terminal_size().columns

        if self.__axis == 0:
            s = "["
            i = iwidth
            for x in arr:
                i += cwidth
                if i > max_cols:
                    s += "\n" + indent
                    # Reset index to account for indent and column we are
                    # about to add.
                    i = iwidth + cwidth
                word = self.__fmt(x)
                word += self.__elem_sep
                s += word
            s = self._rm_trailing_sep(s)
            return s + "]"
        elif self.__axis == 1:
            return "\n".join([self.__fmt(x) for x in arr])

    
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
