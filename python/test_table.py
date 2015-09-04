import numpy as np
import fixfmt

x0 = np.arange(12).astype("float64")
x1 = x0 ** 2 + 1
x2 = np.sqrt(x0)

f0 = fixfmt.Number(2, 0)
f1 = fixfmt.Number(3, 1)
f2 = fixfmt.Number(1, 3)

t = fixfmt.Table()
t.add_float64(x0, f0)
t.add_string(" || ")
t.add_float64(x1, f1)
t.add_string(" | ")
t.add_float64(x2, f2)

for i in range(12):
  print(t(i))

