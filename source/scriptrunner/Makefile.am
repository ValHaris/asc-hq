
MAINTAINERCLEANFILES	= Makefile.in

AM_CXXFLAGS             = $(SDL_CFLAGS) $(SIGC_CFLAGS)  @LokiInclude@ -I$(top_srcdir)/source -I$(top_srcdir)/source/libs/paragui/include

noinst_LTLIBRARIES 	= libscriptrunner.la

libscriptrunner_la_SOURCES	= scriptmain.cpp
                          
libscriptrunner_la_LIBADD	= @SDL_LIBS@


EXTRA_DIST=$(srcdir)/*.h
