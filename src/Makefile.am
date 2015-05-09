AUTOMAKE_OPTIONS = subdir-objects

lib_LTLIBRARIES = libvtapi.la

libvtapi_la_SOURCES_PG = backend_pg/pgbackendbase.cpp backend_pg/pgconnection.cpp backend_pg/pgquerybuilder.cpp backend_pg/pgresultset.cpp
libvtapi_la_SOURCES_SL = backend_sl/slbackendbase.cpp backend_sl/slconnection.cpp backend_sl/slquerybuilder.cpp backend_sl/slresultset.cpp
libvtapi_la_SOURCES = $(libvtapi_la_SOURCES_PG) $(libvtapi_la_SOURCES_SL) backend_factory.cpp commons.cpp compat.cpp keyvalues.cpp dataset.cpp interval.cpp intervalevent.cpp logger.cpp method.cpp process.cpp processstate.cpp processcontrol.cpp query.cpp sequence.cpp timexer.cpp tkey.cpp vtapi.cpp vtapi_settings.cpp vtapi_misc.cpp
libvtapi_la_CFLAGS =  $(PQ_CFLAGS) $(LIBLWGEOM_CFLAGS) $(GEOS_CFLAGS) $(OPENCV_CFLAGS) $(SQLITE_CFLAGS) $(LTDL_CFLAGS) $(READPROC_CFLAGS)
libvtapi_la_CXXFLAGS = $(BOOST_CPPFLAGS) $(PQ_CFLAGS) $(LIBLWGEOM_CFLAGS) $(GEOS_CFLAGS) $(OPENCV_CFLAGS) $(SQLITE_CFLAGS) $(LTDL_CFLAGS) $(READPROC_CFLAGS)
libvtapi_la_LIBADD = $(DEPS_LIBS) $(BOOST_LIBS) $(LIBPQTYPES_LIBS) $(PQ_LIBS) $(LIBLWGEOM_LIBS) $(GEOS_LIBS) $(OPENCV_LIBS) $(SQLITE_LIBS) $(LTDL_LIBS) $(READPROC_LIBS) $(WIN32_EXTRA_LDFLAGS) 
libvtapi_la_LDFLAGS = -version-info $(VTAPI_SO_VERSION) 

AM_CPPFLAGS=-I$(top_builddir)/include $(PQ_CFLAGS) 
#AM_LDFLAGS= -rpath $(abs_top_builddir)/src/.libs
