
MAINTAINERCLEANFILES	= Makefile.in

AM_CXXFLAGS		= @SDL_CFLAGS@ @SIGC_CFLAGS@  @LokiInclude@

noinst_LTLIBRARIES 	= libutil.la

libutil_la_SOURCES	= messaginghub.cpp
libutil_la_LIBADD		= @SDL_LIBS@ 

## INCLUDES = -I$(top_srcdir)/source/libs/paragui/include 

EXTRA_DIST=$(srcdir)/*.h  