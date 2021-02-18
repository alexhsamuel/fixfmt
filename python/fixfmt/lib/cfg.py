from   .itr import partition

#-------------------------------------------------------------------------------

def _merge(d, k, new):
    """
    Sets `d[k] = new`, unless the old and new values are both dicts, in which
    case merges recursively.
    """
    if isinstance(new, dict):
        try:
            old = d[k]
        except KeyError:
            pass
        else:
            if isinstance(old, dict):
                new = merge_dicts(old, new)
    d[k] = new


def merge_dicts(*dicts):
    """
    Merges multiple dicts.

    The merge dict contains combined keys and values from `dicts`.  A key that
    appears in a later dict overrides the previous, except that if the old and
    new values are both dicts, these are merged recursively.
    """
    res, *dicts = dicts
    res = dict(res)
    for d in dicts:
        for k, new in d.items():
            _merge(res, k, new)
    return res


def unflatten_dict(d, *, sep="."):
    """
    Unflattens dotted names into nested dicts.
    """
    res, dotted = partition(lambda i: sep in i[0], d.items())
    res = dict(res)
    for k, val in dotted:
        *parts, name = k.split(sep)
        sub = res
        for part in parts:
            sub = sub.setdefault(part, {})
        _merge(sub, name, val)
    return res
 

def flatten_dict(d, *, sep="."):
    """
    Flattens nested dicts into dotted names.
    """
    def _flatten(items):
        for k, v in items:
            if isinstance(v, dict):
                for kk, vv in _flatten(v.items()):
                    yield k + sep + kk, vv
            else:
                yield k, v

    return dict(_flatten(d.items()))


