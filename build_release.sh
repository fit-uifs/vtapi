#! /usr/bin/env bash

# where to install executables, libraries and headers
# (relative path to ./build or absolute)
INSTALL_PREFIX="/usr/local"
# where to install module libraries
# (relative path to INSTALL_PREFIX or absolute)
MODULES_DIR="lib"


mkdir -p build/release
mkdir -p $INSTALL_PREFIX
cd build/release
cmake ../.. -DCMAKE_BUILD_TYPE:STRING="Release" -DCMAKE_INSTALL_PREFIX:PATH=$INSTALL_PREFIX -DVTAPI_MODULES_DIR=$MODULES_DIR
make
make install
#sudo make install
