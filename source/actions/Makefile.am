
MAINTAINERCLEANFILES	= Makefile.in

AM_CXXFLAGS             = $(SDL_CFLAGS) $(SIGC_CFLAGS) -Dmaindialogs

noinst_LTLIBRARIES 	= libactions.la

libactions_la_SOURCES	= servicing.cpp jumpdrive.cpp
libactions_la_LIBADD		= @SDL_LIBS@

EXTRA_DIST=$(srcdir)/*.h
