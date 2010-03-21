
MAINTAINERCLEANFILES	= Makefile.in

AM_CXXFLAGS          = $(SDL_CFLAGS) $(SIGC_CFLAGS)  @LokiInclude@ 

noinst_LTLIBRARIES 	= libtasks.la libtaskcore.la

libtaskcore_la_SOURCES	= abstracttaskcontainer.cpp taskhibernatingcontainer.cpp 
libtaskcore_la_LIBADD	= @SDL_LIBS@

libtasks_la_SOURCES	= abstracttaskcontainer.cpp taskcontainer.cpp
libtasks_la_LIBADD	= @SDL_LIBS@

EXTRA_DIST=$(srcdir)/*.h
