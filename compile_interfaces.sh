#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

protoc --proto_path=$DIR/src/vtserver --cpp_out=$DIR/src/vtserver --cpp_rpcz_out=$DIR/src/vtserver $DIR/src/vtserver/vtserver_interface.proto
protoc --proto_path=$DIR/src/vtserver --python_out=$DIR/pyclient/vtclient --python_rpcz_out=$DIR/pyclient/vtclient $DIR/src/vtserver/vtserver_interface.proto

