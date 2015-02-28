lib_LTLIBRARIES = libvtapi.la

libvtapi_la_SOURCES = backend_factory.cpp backend_pg.cpp backend_sl.cpp commons.cpp dataset.cpp interval.cpp keyvalues.cpp intervalevent.cpp logger.cpp method.cpp process.cpp query.cpp sequence.cpp timexer.cpp tkey.cpp vtapi.cpp vtapi_settings.cpp vtapi_misc.cpp
libvtapi_la_CFLAGS =  $(PQ_CFLAGS) $(LIBLWGEOM_CFLAGS) $(GEOS_CFLAGS) $(OPENCV_CFLAGS) $(SQLITE_CFLAGS) $(LTDL_CFLAGS) $(READPROC_CFLAGS)
libvtapi_la_CXXFLAGS = $(PQ_CFLAGS) $(LIBLWGEOM_CFLAGS) $(GEOS_CFLAGS) $(OPENCV_CFLAGS) $(SQLITE_CFLAGS) $(LTDL_CFLAGS) $(READPROC_CFLAGS)
libvtapi_la_LIBADD = $(DEPS_LIBS)  $(LIBPQTYPES_LIBS) $(PQ_LIBS) $(LIBLWGEOM_LIBS) $(GEOS_LIBS) $(OPENCV_LIBS) $(SQLITE_LIBS) $(LTDL_LIBS) $(READPROC_LIBS) $(WIN32_EXTRA_LDFLAGS) 
libvtapi_la_LDFLAGS = -version-info $(VTAPI_SO_VERSION) 

AM_CPPFLAGS=-I$(top_builddir)/include $(PQ_CFLAGS)
#AM_LDFLAGS= -rpath $(abs_top_builddir)/src/.libs
