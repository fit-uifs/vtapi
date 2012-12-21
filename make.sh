#!/bin/bash
#
# Easy and fast make VTApi
# by Tomas Volf, 
# bugreport: ivolf(at)fit.vutbr.cz

CONFIG=1
DOXYGEN=1
EXECUTABLE=1
REMAKE=0

if [ "$1" == "--help" ]; then
  echo "Script for easy and fast making of VTApi"
  echo "* usage: $0 [--help] | [[--remake] [--without-config] [--without-doc] [--without--exec]]"
  echo "* parameters:"
  echo "     --help                    Show this help."
  echo "     --remake         | -r     Remake all codes and regenerate doc with respect of \"--withoutX\" parameters."
  echo "     --without-config | -c     Do not copy config file to dist directory."
  echo "     --without-doc    | -d     Do not generate documentation."
  echo "     --without-exec   | -e     Do not make executable VTApi program (VTCli)."
  
else
  MAKETXT=Making
  for i in $*; do
    case $i in
      --without-config|-c)
        CONFIG=0
        ;;
    
      --without-doc|-d)
        DOXYGEN=0
        ;;
      
      --without-exec|-e)
        EXECUTABLE=0
        ;;
        
      --remake|-r)
        REMAKE=1
        MAKETXT=Remaking
        ;;
    esac   
  done  

  # documentation
  if [ $REMAKE -eq 1 ]; then
    echo "Clean documentation..."
    cd doc
    rm -rf html latex
    cd ..
  fi
  if [ $DOXYGEN -ne 0 ]; then
    echo "Generating documentation..."
    cd doc
    doxygen
    cd ..
  fi
  
  # liblwgeom
  echo "$MAKETXT liblwgeom..."
  cd postgres/liblwgeom
  if [ $REMAKE -eq 1 ]; then
    make clean
  fi
  make

  # libvtapi
  echo "$MAKETXT libvtapi..."
  cd ../../src
  if [ $REMAKE -eq 1 ]; then
    make clean
  fi
  make
  cd ..

  # VTApi executable
  if [ $EXECUTABLE -ne 0 ]; then
    echo "$MAKETXT executable vtapi (VTCli)..."
    make
  else
    if [ $REMAKE -eq 1 ]; then
      echo "Cleaning documentation because it is not required..."
      make clean
    fi  
  fi
  
  # Configuration file
  if [ $CONFIG -eq 1 ]; then
    echo "Copying configuration file..."
    cp vtapi.conf dist/vtapi.conf
  fi
  
fi
