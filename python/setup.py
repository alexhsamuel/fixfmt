#===============================================================================
# NOTE: This is not the primary build mechanism; it's provided for reference,
# compatibility, and debugging only.  See ../Makefile for build.
#===============================================================================

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
        "fixfmt/PyBool.cc",
        "fixfmt/PyNumber.cc",
        "fixfmt/PyString.cc",
        "fixfmt/PyTable.cc",
        "fixfmt/_ext.cc",
        "fixfmt/functions.cc",
      ],
      library_dirs=[
        "../cxx",
      ],
      libraries=[
        "fixfmt",
      ],
      depends=glob("*.hh") + glob("../cxx/*.hh"),
    ),

  ]
)

