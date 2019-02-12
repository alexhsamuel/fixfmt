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


### Linux Docker build (manual)

```
docker run -ti --rm continuumio/conda_builder_linux bash
git clone https://github.com/alexhsamuel/fixfmt
/opt/miniconda/bin/conda build fixfmt/conda-recipe --python 3.6
anaconda upload ...
```

