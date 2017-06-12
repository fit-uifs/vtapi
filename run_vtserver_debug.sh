#! /usr/bin/env bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

PATH=$DIR/install/bin:$PATH
export PATH

LD_LIBRARY_PATH=$DIR/install/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH

vtserver --config=vtapi_debug.conf

