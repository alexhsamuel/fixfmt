It should behave like this:

```py
t = Table()

t.append_cols(tid=tids, symbol=symbols)
t.append_rows([
    {"tid": 123, "symbol": "FOO"},
    {"tid": 234, "symbol": "BAR"},
])
t.choose_cols(["tid", "symbol"])

t.fmts["tid"] = Number(6)

t.print()
```


or better,

```py
t = Table()

t.rows.append(NAMES)
t.rows.append(UNDERLINE)
t.rows.append({"tid": 123, "symbol": "ABC"})
t.rows.append(get_cols())

t.cols.update(
    cusip=cusips,
    sedol=sedols,
)

t.rows.append(LINE)
t.rows.append(totals)
```


