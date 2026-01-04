
MAINTAINERCLEANFILES	= Makefile.in

AM_CPPFLAGS = -I$(top_srcdir)/source/libs/paragui/include 

AM_CXXFLAGS		= @SDL_CFLAGS@ @SIGC_CFLAGS@ @LIBCURL_CPPFLAGS@  @LokiInclude@ -Dsgmain 

noinst_LTLIBRARIES      = libnetwork.la 

libnetwork_la_SOURCES	= simple_file_transfer.cpp
libnetwork_la_LIBADD		= @SDL_LIBS@ -lpng -lz 

libnetwork_la_SOURCES	+= pbem-server-dummy.cpp

EXTRA_DIST=$(srcdir)/*.h  