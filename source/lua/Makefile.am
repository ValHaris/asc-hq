MAINTAINERCLEANFILES	= Makefile.in

AM_CXXFLAGS             = $(SDL_CFLAGS) $(SIGC_CFLAGS)  @LokiInclude@ @LUA_FLAGS@

noinst_LTLIBRARIES 	= libluainterfacegame.la libluainterfacemaped.la

libluainterfacegame_la_SOURCES	= commands.cpp commands_wrap.cxx luarunner.cpp luastate.cpp common.cpp
libluainterfacegame_la_LIBADD		= @LUA_LIBS@

libluainterfacemaped_la_SOURCES	= mapedcommands.cpp mapedcommands_wrap.cxx luarunner.cpp luastate.cpp common.cpp
libluainterfacemaped_la_LIBADD		= @LUA_LIBS@

EXTRA_DIST=$(srcdir)/*.h $(srcdir)/*.i

.PHONY: SWIG

SWIG:
	swig -c++ -lua commands.i
	swig -c++ -lua -Dmapeditor mapedcommands.i