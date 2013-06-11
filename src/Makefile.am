lib_LTLIBRARIES = libvtapi-@VTAPI_API_VERSION@.la

libvtapi_@VTAPI_API_VERSION@_la_SOURCES = backend_pg.cpp backend_sl.cpp commons.cpp dataset.cpp interval.cpp keyvalues.cpp logger.cpp method.cpp process.cpp query.cpp sequence.cpp timexer.cpp tkey.cpp vtapi.cpp vtapi_settings.cpp vtapi_misc.cpp
libvtapi_@VTAPI_API_VERSION@_la_CFLAGS =  $(DEPS_CFLAGS) 
libvtapi_@VTAPI_API_VERSION@_la_CXXFLAGS = $(DEPS_CFLAGS) 
libvtapi_@VTAPI_API_VERSION@_la_LDFLAGS = -version-info $(VTAPI_SO_VERSION) $(DEPS_LIBS)
INCLUDES=-I$(top_builddir)/include $(PQ_CFLAGS)