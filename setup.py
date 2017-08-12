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

with open("long_description.rst", "rt") as file:
    long_description = file.read()

setup(
    name            ="fixfmt",
    version         ="0.5.0",
    description     ="fixed-width formatters for C++ and Python",
    long_description=long_description,
    url             ="https://github.com/alexhsamuel/fixfmt",
    author          ="Alex Samuel",
    author_email    ="alexhsamuel@gmail.com",
    license         ="Apache 2.0",
    classifiers     =[
        "Development Status :: 4 - Beta",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: Apache Software License",
        "Programming Language :: Python :: 2",
        "Programming Language :: Python :: 2.7",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.6",
    ],    

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
    ],
    cmdclass={
        "build_ext" : BuildExt,
        "install"   : Install,
    },
)

