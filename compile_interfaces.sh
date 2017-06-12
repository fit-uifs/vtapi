#!/bin/bash

protoc --proto_path=src/vtserver --cpp_out=src/vtserver --cpp_rpcz_out=src/vtserver src/vtserver/vtserver_interface.proto
protoc --proto_path=src/vtserver --python_out=src/vtserver/python --python_rpcz_out=src/vtserver/python src/vtserver/vtserver_interface.proto

