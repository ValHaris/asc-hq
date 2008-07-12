
MAINTAINERCLEANFILES	= Makefile.in

AM_CXXFLAGS             = $(SDL_CFLAGS) $(SIGC_CFLAGS)  @LokiInclude@ 

noinst_LTLIBRARIES 	= libactions.la libdirectunitactions.la libmiscactions.la libmapactions.la libfullgameactions.la

libactions_la_SOURCES	= action.cpp context.cpp actionresult.cpp actioncontainer.cpp
libactions_la_LIBADD		= @SDL_LIBS@

libdirectunitactions_la_SOURCES	= consumeammo.cpp inflictunitdamage.cpp destructunit.cpp  \
                          changeunitmovement.cpp unitaction.cpp changeunitproperty.cpp registerunitrftarget.cpp
libdirectunitactions_la_LIBADD		= @SDL_LIBS@

libmiscactions_la_SOURCES	= servicing.cpp jumpdrive.cpp selfdestruct.cpp 
libmiscactions_la_LIBADD		= @SDL_LIBS@

libmapactions_la_SOURCES	= spawnobject.cpp 
libmapactions_la_LIBADD		= @SDL_LIBS@

libfullgameactions_la_SOURCES	= vehicleattack.cpp
libfullgameactions_la_LIBADD		= @SDL_LIBS@


EXTRA_DIST=$(srcdir)/*.h
