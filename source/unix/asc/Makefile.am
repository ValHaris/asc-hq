vpath %.cpp $(srcdir)/../.. $(srcdir)/../../unittests 

MAINTAINERCLEANFILES	= Makefile.in

INCLUDES      = -I$(top_srcdir) -I$(top_srcdir)/source -I$(top_srcdir)/source/libs/paragui/include @LokiInclude@

AM_CXXFLAGS   = $(SDL_CFLAGS) -Dsgmain $(SIGC_CFLAGS)

bin_PROGRAMS  = asc 
noinst_PROGRAMS = unittester

noinst_bindir = $(abs_top_builddir)/source/unix/asc/


commonSources   = soundList.cpp typen.cpp applicationstarter.cpp \
strtmesg.cpp stack.cpp spfst.cpp sgstream.cpp palette.cpp \
newfont.cpp misc.cpp loadpcxc.cpp loaders.cpp tos.cpp \
loadbi3.cpp gamedlg.cpp dlg_box.cpp dialog.cpp controls.cpp \
basestrm.cpp basegfx.cpp attack.cpp replaymapdisplay.cpp memory-measurement.cpp \
gameoptions.cpp astar2.cpp vehicletype.cpp objects.cpp stdio-errorhandler.cpp \
buildingtype.cpp containerbase.cpp mapalgorithms.cpp viewcalculation.cpp \
gamemap.cpp password.cpp replay.cpp research.cpp fieldimageloader.cpp \
mapdisplay.cpp resourcenet.cpp ascstring.cpp graphicset.cpp \
vehicle.cpp buildings.cpp networkinterface.cpp textfileparser.cpp terraintype.cpp \
objecttype.cpp textfiletags.cpp itemrepository.cpp stringtokenizer.cpp music.cpp \
messages.cpp paradialog.cpp textfile_evaluation.cpp containerbasetype.cpp messagedlg.cpp \
simplestream.cpp mappolygons.cpp prehistoricevents.cpp gameevents.cpp gameeventsystem.cpp \
gameevent_dialogs.cpp statisticdialog.cpp clipboard.cpp geometry.cpp \
guidimension.cpp guiiconhandler.cpp guifunctions.cpp iconrepository.cpp \
overviewmapimage.cpp mapfield.cpp overviewmappanel.cpp dashboard.cpp asc-mainscreen.cpp  \
explosivemines.cpp mainscreenwidget.cpp networksupervisor.cpp statistics.cpp \
legacynetworkloaders.cpp player.cpp cannedmessages.cpp reactionfire.cpp mapimageexport.cpp \
containercontrols.cpp containerbase-functions.cpp windowing.cpp turncontrol.cpp unitset.cpp \
playercolor.cpp pg_mapdisplay.cpp autotraining.cpp researchexecution.cpp luacommandwriter.cpp \
campaignactionrecorder.cpp loggingoutput.cpp versionidentifier.cpp package.cpp packagemanager.cpp \
packagerepository.cpp dataversioncheck.cpp i18n.cpp accessconstraints-game.cpp contextutils.cpp

commonLibs =  $(top_builddir)/source/libs/triangul/libtriangul.la \
$(top_builddir)/source/ai/libai.la $(top_builddir)/source/libs/sdlmm/src/libSDLmm.la \
$(top_builddir)/source/sdl/libsdl.la  $(LOKI_LIBS) $(LUA_GAME_LIB) \
$(top_builddir)/source/util/libutil.la  \
$(top_builddir)/source/tasks/libtaskcore.la \
$(top_builddir)/source/tasks/libtasks.la \
$(top_builddir)/source/graphics/libgraphics.la $(top_builddir)/source/dialogs/libdialogs.la \
$(top_builddir)/source/dialogs/libgamedialogs.la $(top_builddir)/source/actions/libactions.la \
$(top_builddir)/source/libs/paragui/src/libparagui.la $(top_builddir)/source/widgets/libwidgets.la \
$(top_builddir)/source/sdl/libsdlsnd.la $(top_builddir)/source/network/libnetwork.la  \
$(top_builddir)/source/video/libvideo.la  \
$(top_builddir)/source/actions/libactions.la  \
$(top_builddir)/source/actions/libdirectunitactions.la  \
$(top_builddir)/source/actions/libmapactions.la  \
$(top_builddir)/source/lua/libluainterfacegame.la \
$(BZIP2_LIBS) $(SIGC_LIBS) $(SDL_LIBS) $(FREETYPE_LIBS) $(BOOST_LIBS) $(SDL_SOUND_LIB) $(WX_LIBS)

nodist_asc_SOURCES = $(commonSources) sg.cpp gamedialog.cpp 
asc_LDADD = $(commonLibs)

nodist_unittester_SOURCES  =  $(commonSources) 
unittester_LDADD = $(top_builddir)/source/unittests/libunittests.la  $(commonLibs) 

