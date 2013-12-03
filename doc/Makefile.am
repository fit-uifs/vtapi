# FIXME
# John Calcote: Autotools: A Practitioner's Guide to GNU Autoconf, Automake, and Libtool, page 246

docdir=$(prefix)/share/doc/@PACKAGE@

EXTRA_DIST = Doxyfile.in white-xifish.png

doc_DATA = doxygen-build.stamp

if HAVE_DOXYGEN
doxygen-build.stamp: Doxyfile
	doxygen
	touch doxygen-build.stamp
else
doxygen-build.stamp:
	echo "*** Warning: Doxygen not found; documentation will not be built."
	touch doxygen-build.stamp
endif

dist_docdir = $(distdir)


uninstall-local:
	rm -rf $(docdir)

clean-local:
	if test -f doxygen-build.stamp; then rm -f doxygen-build.stamp; fi
