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
 # else if nbwinconfig
else
  MAKETXT=Making
  for i in $*; do
    case $i in
      --remake|-r)
        REMAKE=1
        MAKETXT=Remaking
        ;;
        
      --without-config|-c)
        CONFIG=0
        ;;
    
      --without-doc|-d)
        DOXYGEN=0
        ;;
      
      --without-exec|-e)
        EXECUTABLE=0
        ;;
    esac   
  done  
  
  # Arguments of making depends of operating system, which is detected by uname
  if [ "`uname -s | grep "MINGW32_NT"`" != "" ]; then
    MAKEARGS=Windows
    echo "OS Windows was detected - making configuration is set to Windows..."
  else
    MAKEARGS=Debug
    echo "Not Windows OS was detected (most probably it is *nix-like OS) - making configuration is set to Debug..."
  fi
  MAKEARGS="-f Makefile CONF=$MAKEARGS"

  # documentation
  if [ $REMAKE -eq 1 ]; then
    echo "Cleaning documentation..."
    cd doc
    rm -rf html latex
    cd ..
  fi
  if [ $DOXYGEN -eq 1 ]; then
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
  make $MAKEARGS

  # libvtapi
  echo "$MAKETXT libvtapi..."
  cd ../../src
  if [ $REMAKE -eq 1 ]; then
    make clean
  fi
  make $MAKEARGS
  cd ..

  # VTApi executable
  if [ $REMAKE -eq 1 ]; then
    make clean
  fi
  if [ $EXECUTABLE -eq 1 ]; then
    echo "$MAKETXT executable vtapi (VTCli)..."
    make $MAKEARGS
  fi
  
  # Configuration file
  if [ $CONFIG -eq 1 ]; then
    echo "Copying configuration file..."
    cp vtapi.conf dist/vtapi.conf
  fi
  
fi
