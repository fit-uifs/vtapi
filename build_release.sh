#! /usr/bin/env bash

# where to install executables, libraries and headers
INSTALL_PREFIX="/usr/local"
# where to install module libraries (relative path to INSTALL_PREFIX or absolute)
MODULES_DIR="/usr/local/bin"

mkdir -p build
mkdir -p $INSTALL_PREFIX
cd build
cmake .. -DCMAKE_BUILD_TYPE:STRING="Release" -DCMAKE_INSTALL_PREFIX:PATH=$INSTALL_PREFIX -DVTAPI_MODULES_DIR=$MODULES_DIR
make
make install
