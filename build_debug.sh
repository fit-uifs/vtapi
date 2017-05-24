#! /usr/bin/env bash

# where to install executables, libraries and headers
# (relative path to ./build/debug or absolute)
INSTALL_PREFIX="../../install"
# where to install module libraries
# (relative path to INSTALL_PREFIX or absolute)
MODULES_DIR="lib/modules"

BASEPATH="$( cd "$( dirname "${0}" )" && pwd )"
cd ${BASEPATH}
mkdir -p build/debug
mkdir -p $INSTALL_PREFIX
cd build/debug
cmake ../.. -DCMAKE_BUILD_TYPE:STRING="Debug" -DCMAKE_INSTALL_PREFIX:PATH=$INSTALL_PREFIX -DVTAPI_MODULES_DIR=$MODULES_DIR "$@"
make
make install
