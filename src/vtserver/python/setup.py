from setuptools import setup
from setuptools.command.install import install
import subprocess

class MyInstall(install):

    def run(self):
        print("Generating protobufs...")
        subprocess.call(['./compile_interface.sh'])
        install.run(self)

setup(name='VTServer',
      version='0.1',
      description='VTServer',
      url='https://github.com/robofit/videoterror',
      author='FIT',
      author_email='imaterna@fit.vutbr.cz',
      license='tbd',
      packages=['pyvtserver'],
      install_requires=[
          'protodict',
          'rpcz'
      ],
      cmdclass={'install': MyInstall},
      zip_safe=False)
