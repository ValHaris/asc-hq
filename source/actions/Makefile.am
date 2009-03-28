
MAINTAINERCLEANFILES	= Makefile.in

AM_CXXFLAGS             = $(SDL_CFLAGS) $(SIGC_CFLAGS)  @LokiInclude@ 

noinst_LTLIBRARIES 	= libactions.la libdirectunitactions.la libmiscactions.la libmapactions.la libfullgameactions.la

libactions_la_SOURCES	= action.cpp command.cpp context.cpp actionresult.cpp actioncontainer.cpp containeraction.cpp
libactions_la_LIBADD		= @SDL_LIBS@

libdirectunitactions_la_SOURCES	=  inflictdamage.cpp destructcontainer.cpp constructbuildingcommand.cpp spawnbuilding.cpp destructbuildingcommand.cpp \
                          changeunitmovement.cpp unitaction.cpp changeunitproperty.cpp registerunitrftarget.cpp consumeresource.cpp \
                          containeraction.cpp changecontainerproperty.cpp unitfieldregistration.cpp viewregistration.cpp consumeammo.cpp \
                          recycleunitcommand.cpp destructunitcommand.cpp reactionfireswitchcommand.cpp repairunitcommand.cpp jumpdrivecommand.cpp \
                          powergenerationswitchcommand.cpp trainunitcommand.cpp
                          
libdirectunitactions_la_LIBADD		= @SDL_LIBS@

libmiscactions_la_SOURCES	= servicing.cpp spawnmine.cpp putobjectcommand.cpp putminecommand.cpp
libmiscactions_la_LIBADD		= @SDL_LIBS@

libmapactions_la_SOURCES	= spawnobject.cpp spawnmine.cpp changeview.cpp removemine.cpp removeobject.cpp changeobjectproperty.cpp
libmapactions_la_LIBADD		= @SDL_LIBS@

libfullgameactions_la_SOURCES	= containeraction.cpp recycleunitcommand.cpp consumeresource.cpp vehicleattack.cpp moveunit.cpp discoverresources.cpp \
                                  unitcommand.cpp attackcommand.cpp moveunitcommand.cpp convertcontainer.cpp containercommand.cpp \
                                  constructunitcommand.cpp spawnunit.cpp cargomovecommand.cpp servicecommand.cpp 
libfullgameactions_la_LIBADD		= @SDL_LIBS@


EXTRA_DIST=$(srcdir)/*.h
