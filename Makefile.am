AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4

SUBDIRS = src
if HAVE_DOXYGEN
SUBDIRS += doc
endif

dist_noinst_SCRIPTS = autogen.sh

bin_PROGRAMS = vtcli
vtcli_SOURCES = vtcli.cpp
vtcli_CFLAGS = $(PQ_CFLAGS) $(LIBLWGEOM_CFLAGS) $(GEOS_CFLAGS) $(OPENCV_CFLAGS) $(SQLITE_CFLAGS) $(LTDL_CFLAGS) $(READPROC_CFLAGS)
vtcli_CXXFLAGS = $(PQ_CFLAGS) $(LIBLWGEOM_CFLAGS) $(GEOS_CFLAGS) $(OPENCV_CFLAGS) $(SQLITE_CFLAGS) $(LTDL_CFLAGS) $(READPROC_CFLAGS)
vtcli_LDFLAGS = $(DEPS_LIBS) $(OPENCV_LIBS) $(SQLITE_LIBS) $(LIBPQTYPES_LIBS) $(PQ_LIBS) $(LIBLWGEOM_LIBS) $(GEOS_LIBS) $(LTDL_LIBS) $(READPROC_LIBS)

#LDADD = -L/home/imlich/workspace/trecvid2013/vtapi-autotools/lib/.libs/libexample-1.0.so -lexample 
#LDADD = $(top_builddir)/src/.libs/libvtapi-1.6.so.0.1.6 
LDADD = -L./src/.libs/ -lvtapi-2.0

#LDADD=$(top_builddir)/lib/.libs/libvtapi-1.5.so.0.1.5 \
#  -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy -lopencv_flann -lpq -lpqtypes -lgeos_c -lprocps -llwgeom

#LDADD = -L../lib -lexample 

AM_CPPFLAGS = -I$(top_builddir)/include
