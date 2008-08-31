
MAINTAINERCLEANFILES	= Makefile.in

AM_CXXFLAGS             = $(SDL_CFLAGS) $(SIGC_CFLAGS)  @LokiInclude@ 

noinst_LTLIBRARIES 	= libactions.la libdirectunitactions.la libmiscactions.la libmapactions.la libfullgameactions.la

libactions_la_SOURCES	= action.cpp command.cpp context.cpp actionresult.cpp actioncontainer.cpp containeraction.cpp
libactions_la_LIBADD		= @SDL_LIBS@

libdirectunitactions_la_SOURCES	=  consumeammo.cpp inflictdamage.cpp destructcontainer.cpp \
                          changeunitmovement.cpp unitaction.cpp changeunitproperty.cpp registerunitrftarget.cpp \
                          containeraction.cpp changecontainerproperty.cpp unitfieldregistration.cpp 
libdirectunitactions_la_LIBADD		= @SDL_LIBS@

libmiscactions_la_SOURCES	= servicing.cpp jumpdrive.cpp selfdestruct.cpp putobjectcommand.cpp
libmiscactions_la_LIBADD		= @SDL_LIBS@

libmapactions_la_SOURCES	= spawnobject.cpp changeview.cpp removemine.cpp removeobject.cpp changeobjectproperty.cpp
libmapactions_la_LIBADD		= @SDL_LIBS@

libfullgameactions_la_SOURCES	= containeraction.cpp consumeresource.cpp vehicleattack.cpp moveunit.cpp \
										unitcommand.cpp attackcommand.cpp moveunitcommand.cpp convertcontainer.cpp
libfullgameactions_la_LIBADD		= @SDL_LIBS@


EXTRA_DIST=$(srcdir)/*.h
