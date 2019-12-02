# Setup

```
$ git clone https://github.com/google/googletest.git
$ cd googletest/googletest/make
$ CXXFLAGS=-std=c++14 make
```

# Releases

Source:
```
bumpversion patch  # or minor, major
git push --tags
git push
python setup.py sdist upload
```

Binary wheel:
```
python setup.py bdist_wheel upload
```

Conda:
```
conda build conda-recipe
anaconda upload PKG
```

Repeat wheel and conda packages on other architectures.

Binary wheel and conda builds on Linux should be run in a CentOS7 container.


