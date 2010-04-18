
MAINTAINERCLEANFILES	= Makefile.in

AM_CXXFLAGS             = $(SDL_CFLAGS) $(SIGC_CFLAGS)  @LokiInclude@ 

noinst_LTLIBRARIES 	= libactions.la libdirectunitactions.la  libmapactions.la 

libactions_la_SOURCES	= action.cpp command.cpp context.cpp actionresult.cpp actioncontainer.cpp containeraction.cpp
libactions_la_LIBADD		= @SDL_LIBS@

libdirectunitactions_la_SOURCES	=  inflictdamage.cpp destructcontainer.cpp constructbuildingcommand.cpp spawnbuilding.cpp destructbuildingcommand.cpp \
                          changeunitmovement.cpp unitaction.cpp changeunitproperty.cpp registerunitrftarget.cpp consumeresource.cpp \
                          containeraction.cpp changecontainerproperty.cpp unitfieldregistration.cpp viewregistration.cpp consumeammo.cpp \
                          recycleunitcommand.cpp destructunitcommand.cpp reactionfireswitchcommand.cpp repairunitcommand.cpp jumpdrivecommand.cpp \
                          powergenerationswitchcommand.cpp repairbuildingcommand.cpp transfercontrolcommand.cpp directresearchcommand.cpp \
                          buildproductionlinecommand.cpp removeproductionlinecommand.cpp cancelresearchcommand.cpp setresourceprocessingratecommand.cpp \
                          servicing.cpp spawnmine.cpp putobjectcommand.cpp putminecommand.cpp trainunitcommand.cpp servicecommand.cpp \
                          containeraction.cpp recycleunitcommand.cpp consumeresource.cpp vehicleattack.cpp moveunit.cpp discoverresources.cpp \
                          unitcommand.cpp attackcommand.cpp moveunitcommand.cpp convertcontainer.cpp containercommand.cpp \
                          constructunitcommand.cpp spawnunit.cpp cargomovecommand.cpp diplomacycommand.cpp changediplomaticstate.cpp \
                          internalammotransfercommand.cpp actionavailability.cpp
                          
libdirectunitactions_la_LIBADD		= @SDL_LIBS@

# this are actions that are also needed by mapeditor
libmapactions_la_SOURCES	= spawnobject.cpp spawnmine.cpp changeview.cpp removemine.cpp removeobject.cpp changeobjectproperty.cpp 
                                  
libmapactions_la_LIBADD		= @SDL_LIBS@

EXTRA_DIST=$(srcdir)/*.h
