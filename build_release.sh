#! /usr/bin/env bash

# Usage: ./build_release [-D<variable>=<value>]
#
# Path customization variables:
#   * POCO_PATH: path to root dir of Poco project (where are dirs "include", "lib", ...)
#   * POCO_INCLUDE_PATH: path to include dir of Poco project
#   * POCO_LIBRARY_PATH: path to library dir of Poco project
#   * POCOFOUNDATION_INCLUDE_PATH, POCOUTIL_INCLUDE_PATH, POCOXML_INCLUDE_PATH:
#          path to include dir of individual parts of Poco project
#   * POCOFOUNDATION_LIBRARY_PATH, POCOUTIL_LIBRARY_PATH, POCOXML_LIBRARY_PATH:
#          path to library dir of individual parts of Poco project
#
#   * OPENCV_PATH: path in which OpenCV package could be found
#   * OPENCV_PC_PATH: path to opencv.pc file for pkg-config (including file name)
#
#   * PG_CONFIG_PATH: path to pg_config executable (including file name)
#   * LIBPQTYPES_PATH: path to root dir of PQtypes library (where are dirs "include", "lib", ...)
#
#   * SQLITE3_PC_PATH: path to sqlite3.pc file for pkg-config (including file name)


mkdir -p build/release
cd build/release
cmake ../.. -Wno-dev 
if test $? = 0
then
  make
  if test $? = 0
  then
    sudo make install
  fi
fi
