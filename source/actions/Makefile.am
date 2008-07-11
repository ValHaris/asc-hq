
MAINTAINERCLEANFILES	= Makefile.in

AM_CXXFLAGS             = $(SDL_CFLAGS) $(SIGC_CFLAGS)  @LokiInclude@ 

noinst_LTLIBRARIES 	= libactions.la

libactions_la_SOURCES	= servicing.cpp jumpdrive.cpp selfdestruct.cpp action.cpp context.cpp actionresult.cpp \
                          consumeammo.cpp inflictunitdamage.cpp spawnobject.cpp destructunit.cpp
libactions_la_LIBADD		= @SDL_LIBS@

EXTRA_DIST=$(srcdir)/*.h
