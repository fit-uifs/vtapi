# not usable now

from setuptools import setup
from setuptools.command.install import install
import subprocess

class MyInstall(install):

    def run(self):
        print("Generating protobufs...")
        subprocess.call(['../compile_interfaces.sh'])
        install.run(self)

setup(name='VTServer client',
      version='0.1',
      description='VTServer client',
      url='https://github.com/fit-uifs/vtapi',
      author='Vojtech Froml',
      author_email='ifroml@fit.vutbr.cz',
      license='tbd',
      packages=['vtclient'],
      install_requires=[
          'protodict',
          'rpcz'
      ],
      cmdclass={'install': MyInstall},
      zip_safe=False)
