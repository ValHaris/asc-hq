
MAINTAINERCLEANFILES	= Makefile.in

AM_CXXFLAGS             = $(SDL_CFLAGS) $(SIGC_CFLAGS)  @LokiInclude@ -I.. -I$(top_srcdir)/source/libs/paragui/include

noinst_LTLIBRARIES 	= libunittests.la

libunittests_la_SOURCES	= movementtest.cpp unittestutil.cpp
libunittests_la_LIBADD	= @SDL_LIBS@


EXTRA_DIST=$(srcdir)/*.h
