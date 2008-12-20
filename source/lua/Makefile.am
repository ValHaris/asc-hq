MAINTAINERCLEANFILES	= Makefile.in

AM_CXXFLAGS             = $(SDL_CFLAGS) $(SIGC_CFLAGS)  @LokiInclude@ @LUA_FLAGS@

noinst_LTLIBRARIES 	= libluainterface.la 

libluainterface_la_SOURCES	= luastate.cpp luarunner.cpp binder.cpp
libluainterface_la_LIBADD		= @LUA_LIBS@

EXTRA_DIST=$(srcdir)/*.h 
