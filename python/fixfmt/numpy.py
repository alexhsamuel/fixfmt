class Array:

    def __init__(self, fmt, width=None, *, left="", right="", margin=" "):
        self.__fmt = fmt
        self.__width = width
        self.__left = left
        self.__right = right
        self.__margin = margin


    def __call__(self, arr):
        twidth = get_terminal_width() if self.__width is None else self.__width

        fmt = self.__fmt
        width = fmt.width
        assert twidth >= len(self.__left) + len(self.__right) + width
        num_cols = 1 + (
            (twidth - len(self.__left) - len(self.__right) - width)
            // (width + len(self.__marign)))

        for row in arr:
            print(
                self.__left 
                + self.__margin.join( fmt(c) for c in row[: num_cols] )
                + self.__right
            )



