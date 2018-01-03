from setuptools import setup
from setuptools import Extension

from Cython.Build import cythonize

# execute:
# python setup.py build_ext --inplace
setup(
    ext_modules = cythonize("window.py")
)
