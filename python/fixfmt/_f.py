def get_default_formatter(type, values, cfg={}):
    """
    Chooses a default formatter based on type and values.

    @param values
      A sequence or `ndarray` of values.
    """
    values = np.array(values)

    if type is str:
        width = np.vectorize(len)(np.vectorize(str)(values)).max()
        str_width_min = int(cfg["str_width_min"])
        str_width_max = int(cfg["str_width_max"])
        width = clip(str_width_min, width, str_width_max)
        return formatters.StrFormatter(width, ellipsis=cfg["ellipsis"])

    elif type is bool:
        return formatters.BoolFormatter("TRUE", "FALSE", size=1, pad_left=True)

    elif type is datetime:
        return formatters.DatetimeFormatter("ISO 8601 extended")

    else:
        raise NotImplementedError("type: {}".format(type))
        

