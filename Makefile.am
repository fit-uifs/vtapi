AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4

SUBDIRS = src include
if HAVE_DOXYGEN
  SUBDIRS += doc
endif

dist_noinst_SCRIPTS = autogen.sh

bin_PROGRAMS = vtcli
vtcli_SOURCES = vtcli.cpp
vtcli_CFLAGS = $(PQ_CFLAGS) $(LIBLWGEOM_CFLAGS) $(GEOS_CFLAGS) $(OPENCV_CFLAGS) $(SQLITE_CFLAGS) $(LTDL_CFLAGS) $(LIBXMLXX_CFLAGS) $(READPROC_CFLAGS)
vtcli_CXXFLAGS = $(PQ_CFLAGS) $(LIBLWGEOM_CFLAGS) $(GEOS_CFLAGS) $(OPENCV_CFLAGS) $(SQLITE_CFLAGS) $(LTDL_CFLAGS) $(LIBXMLXX_CFLAGS) $(READPROC_CFLAGS)
#vtcli_LDFLAGS = -no-install -L$(abs_top_builddir)/src/.libs $(DEPS_LIBS) $(OPENCV_LIBS) $(SQLITE_LIBS) $(LIBPQTYPES_LIBS) $(PQ_LIBS) $(LIBLWGEOM_LIBS) $(GEOS_LIBS) $(LTDL_LIBS) $(LIBXMLXX_LIBS) $(READPROC_LIBS) -lvtapi
vtcli_LDFLAGS = -no-install -L$(abs_top_builddir)/src/.libs $(DEPS_LIBS) -lvtapi -lopencv_core -lopencv_highgui

#LDADD = -lvtapi

AM_CPPFLAGS = -I$(abs_top_builddir)/include
#AM_LDFLAGS = 
