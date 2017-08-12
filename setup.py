from   glob import glob
import os
from   setuptools import setup, Extension
import setuptools.command.build_ext
import setuptools.command.install
import subprocess
import sys

#-------------------------------------------------------------------------------

if sys.platform == "darwin":
    # No C++14 when building for earlier OSX versions.
    os.environ["MACOSX_DEPLOYMENT_TARGET"] = "10.9"


#-------------------------------------------------------------------------------

# Convince setuptools to call our C++ build.

class BuildExt(setuptools.command.build_ext.build_ext):

    def run(self):
        subprocess.check_call(["make", "lib-cxx"])
        setuptools.command.build_ext.build_ext.run(self)



class Install(setuptools.command.install.install):

    def run(self):
        subprocess.check_call(["make", "install-cxx"])
        setuptools.command.install.install.run(self)



#-------------------------------------------------------------------------------

setup(
    cmdclass={
        "build_ext" : BuildExt,
        "install"   : Install,
    },
    name="fixfmt",
    package_dir={"": "python"},
    packages=["fixfmt"],
    ext_modules=[
        Extension(
            "fixfmt._ext",
            extra_compile_args    =["-std=c++14", ],
            include_dirs          =["./cxx", ],
            sources               =glob("python/fixfmt/*.cc"),
            library_dirs          =["./cxx",],
            libraries             =["fixfmt",],
            depends               =glob("cxx/fixfmt/*.hh") + glob("python/fixfmt/*.hh"),
        ),
    ]
)

