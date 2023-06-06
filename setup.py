from setuptools import Extension, setup

module = Extension("kmeanssp", sources=['kmeans_clustering.c','kmeansmodule.c'])
setup(name='kmeanssp',
     version='1.0',
     description='Python wrapper for custom C extension',
     ext_modules=[module])