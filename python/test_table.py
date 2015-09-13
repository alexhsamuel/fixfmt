import numpy as np
import fixfmt

x0 = np.arange(12).astype("int16")
x1 = (x0 ** 2 + 1).astype("int64")
x2 = np.sqrt(x0).astype("float32")
x3 = x2 > 3
x4 = np.array([ "val-{}".format(i) for i in x0 ], object)

f0 = fixfmt.Number(2)
f1 = fixfmt.Number(3, 1, pad='0')
f2 = fixfmt.Number(1, 3)

t = fixfmt.Table()
t.add_int16(x0, f0)
t.add_string(" || ")
t.add_int64(x1, f1)
t.add_string(" | ")
t.add_float32(x2, f2)
t.add_string(" | ")
t.add_bool(x3, fixfmt.Bool("yes", "no"))
t.add_string(" | ")
t.add_str_object(x4, fixfmt.String(5, ellipsis="/", position=0))

for i in range(len(t)):
  print(t(i))

print("=" * t.width)
print(t.length)
