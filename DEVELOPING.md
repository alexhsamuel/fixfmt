# Releases

1. Update version in `setup.py`.
1. Update version in `conda-recipe/meta.yaml`.
1. Commit and push.
1. `git push`.
1. Create a release in GitHub.
1. `python setup.py sdist upload`
1. `python setup.py bdist_wheel upload`
1. `conda build conda-recipe`
1. `anaconda upload PKG`

Repat wheel and conda packages on other architectures.
