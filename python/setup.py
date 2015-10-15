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
      "fixfmt._ext",
      extra_compile_args=["-std=c++14", ],
      include_dirs=["../cxx", ],
      sources=[
        "fixfmt/Table.cc",
        "fixfmt/Bool.cc",
        "fixfmt/_ext.cc",
        "fixfmt/functions.cc",
        "fixfmt/Number.cc",
        "fixfmt/String.cc",
      ],
      depends=glob("*.hh") + glob("../cxx/*.hh"),
    ),

  ]
)

