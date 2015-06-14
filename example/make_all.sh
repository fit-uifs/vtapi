#!/bin/sh

_DIR=$PWD

cd $_DIR/../src
make

cd $_DIR
make

cd $_DIR/mod_demo1
make

cd $_DIR/mod_demo2
make

