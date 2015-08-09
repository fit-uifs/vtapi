#! /usr/bin/env bash

# where to install executables, libraries and headers
# (relative path to ./build or absolute)
INSTALL_PREFIX="../install"
# where to install module libraries
# (relative path to INSTALL_PREFIX or absolute)
MODULES_DIR="bin/modules"


mkdir -p build
mkdir -p $INSTALL_PREFIX
cd build
cmake .. -DCMAKE_BUILD_TYPE:STRING="Debug" -DCMAKE_INSTALL_PREFIX:PATH=$INSTALL_PREFIX -DVTAPI_MODULES_DIR=$MODULES_DIR
make
make install
