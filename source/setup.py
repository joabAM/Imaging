'''
Created on Aug 07, 2015

@author: Miguel Urco
'''
from imagpy import __version__
from setuptools import setup, Extension

setup(name="imagpy",
        version=__version__,
        description="Python tools to read, write and process Imaging data",
        author="Miguel Urco",
        author_email="miguel.urco@jro.igp.gob.pe",
        url="http://jro.igp.gob.pe",
#         packages = {'imagpy'},
#                     'imagpy.graphs',
#                     'imagpy.io',
#                     'imagpy.proc',
#                     'imagpy.utils'},
        py_modules=['imagpy.graphs.driver_plplot',
                    'imagpy.graphs.imgplot',
                    'imagpy.iofiles.imgfile',
                    'imagpy.proc.imgproc',
                    'imagpy.utils.mkMovie',
                    'imagpy.utils.misc',
                    'imagpy.genImage',
                    'imagpy.genImagCuts',
                    'imagpy.genImagPlots'],
        package_data={},
        scripts =['imagpy/genImagCuts.py', 'imagpy/genImagPlots.py'],
        install_requires=["numpy >= 1.6.0, <= 1.16.6",
  #                         "plplot >= 5.9.0",
                          "scipy >= 0.11.0, <1.0.0",
                          "h5py >= 2.0.1, <=2.8.0",
                          "setuptools <= 38.2.5",
                          ],
      )
