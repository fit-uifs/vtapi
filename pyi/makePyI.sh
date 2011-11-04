#!/bin/bash

# for the first time, check if...
# apt-get install python-ctypeslib gccxml

mkdir xml

# gccxml ../include/vtapi_commons.h  -fxml=xml/vtapi_commons.h
h2xml $PWD/../include/vtapi_commons.h  -o $PWD/xml/vtapi_commons.xml


xml2py $PWD/xml/vtapi_commons.xml -c -l libpq.so -l libpqtypes.so -l $PWD/../dist/libvtapi.so -o $PWD/vtapi_commons.py
