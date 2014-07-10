#!/bin/sh
mkdir -p m4
touch README NEWS AUTHORS ChangeLog
autoreconf -i # \ 
autoreconf -f # - This distribution is needed for OS Windows
autoreconf -i # / (one autoreconf with both parameters causes crash)
if [ "`uname | grep MINGW`" != "" ]; then # for OS Windows only
  patch -f -r NUL < ltmain.patch >NUL 2>NUL
fi
