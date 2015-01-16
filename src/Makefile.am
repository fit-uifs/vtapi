lib_LTLIBRARIES = libvtapi-@VTAPI_API_VERSION@.la

libvtapi_@VTAPI_API_VERSION@_la_SOURCES = backend_factory.cpp backend_pg.cpp backend_sl.cpp commons.cpp dataset.cpp interval.cpp keyvalues.cpp logger.cpp method.cpp process.cpp query.cpp sequence.cpp timexer.cpp tkey.cpp vtapi.cpp vtapi_settings.cpp vtapi_misc.cpp
libvtapi_@VTAPI_API_VERSION@_la_CFLAGS =  $(PQ_CFLAGS) $(LIBLWGEOM_CFLAGS) $(GEOS_CFLAGS) $(OPENCV_CFLAGS) $(SQLITE_CFLAGS) $(LTDL_CFLAGS) $(READPROC_CFLAGS)
libvtapi_@VTAPI_API_VERSION@_la_CXXFLAGS = $(PQ_CFLAGS) $(LIBLWGEOM_CFLAGS) $(GEOS_CFLAGS) $(OPENCV_CFLAGS) $(SQLITE_CFLAGS) $(LTDL_CFLAGS) $(READPROC_CFLAGS)
libvtapi_@VTAPI_API_VERSION@_la_LDFLAGS = -version-info $(VTAPI_SO_VERSION) $(DEPS_LIBS) $(OPENCV_LIBS) $(SQLITE_LIBS) $(LIBPQTYPES_LIBS) $(PQ_LIBS) $(LIBLWGEOM_LIBS) $(GEOS_LIBS) $(LTDL_LIBS) $(READPROC_LIBS) $(WIN32_EXTRA_LDFLAGS)

AM_CPPFLAGS=-I$(top_builddir)/include $(PQ_CFLAGS)
