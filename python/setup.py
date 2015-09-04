from   glob import glob
import os
from   setuptools import setup, Extension
import sys

if sys.platform == "darwin":
  # No C++14 when building for earlier OSX versions.
  os.environ["MACOSX_DEPLOYMENT_TARGET"] = "10.9"

setup(
  name="test",
  ext_modules=[
    Extension(
      "testmod",
      extra_compile_args=["-std=c++14", ],
      sources=["testmod.cc", ],
    ),

    Extension(
      "fixfmt",
      extra_compile_args=["-std=c++14", ],
      include_dirs=["../src", ],
      sources=[
        "Bool.cc",
        "fixfmt.cc",
        "Number.cc",
        "String.cc",
        "Table.cc",
      ],
      depends=glob("../src/*.hh"),
    ),

  ]
)

