vpath %.cpp $(srcdir)/../..

MAINTAINERCLEANFILES	= Makefile.in

INCLUDES      = -I$(top_srcdir) -I$(top_srcdir)/source -I$(top_srcdir)/source/libs/paragui/include @LokiInclude@

AM_CXXFLAGS   = $(SDL_CFLAGS) -Dsgmain $(SIGC_CFLAGS) @LUA_TAGS@

bin_PROGRAMS  = asc

nodist_asc_SOURCES   = unitctrl.cpp soundList.cpp typen.cpp \
strtmesg.cpp stack.cpp spfst.cpp sgstream.cpp sg.cpp palette.cpp \
newfont.cpp misc.cpp loadpcxc.cpp loaders.cpp tos.cpp \
loadbi3.cpp gamedlg.cpp dlg_box.cpp dialog.cpp controls.cpp \
basestrm.cpp basegfx.cpp attack.cpp replaymapdisplay.cpp memory-measurement.cpp \
gameoptions.cpp astar2.cpp vehicletype.cpp objects.cpp stdio-errorhandler.cpp \
buildingtype.cpp containerbase.cpp mapalgorithms.cpp viewcalculation.cpp \
gamemap.cpp password.cpp replay.cpp research.cpp fieldimageloader.cpp \
mapdisplay.cpp resourcenet.cpp dashboard.cpp ascstring.cpp graphicset.cpp \
vehicle.cpp buildings.cpp networkinterface.cpp textfileparser.cpp terraintype.cpp \
objecttype.cpp textfiletags.cpp itemrepository.cpp stringtokenizer.cpp music.cpp \
messages.cpp paradialog.cpp textfile_evaluation.cpp containerbasetype.cpp messagedlg.cpp \
simplestream.cpp mappolygons.cpp prehistoricevents.cpp gameevents.cpp gameeventsystem.cpp \
gameevent_dialogs.cpp statisticdialog.cpp clipboard.cpp geometry.cpp \
guidimension.cpp guiiconhandler.cpp guifunctions.cpp iconrepository.cpp gamedialog.cpp \
infopopup.cpp overviewmapimage.cpp asc-mainscreen.cpp mapfield.cpp overviewmappanel.cpp \
explosivemines.cpp mainscreenwidget.cpp networksupervisor.cpp statistics.cpp \
legacynetworkloaders.cpp player.cpp cannedmessages.cpp reactionfire.cpp mapimageexport.cpp \
containercontrols.cpp containerbase-functions.cpp windowing.cpp turncontrol.cpp unitset.cpp \
playercolor.cpp pg_mapdisplay.cpp

asc_LDADD   		= $(top_builddir)/source/libs/triangul/libtriangul.la \
$(top_builddir)/source/ai/libai.la $(top_builddir)/source/libs/sdlmm/src/libSDLmm.la \
$(top_builddir)/source/sdl/libsdl.la  $(top_builddir)/source/libs/loki-0.1.6/libloki.la \
@LUA_LOCAL_LIB@  \
$(top_builddir)/source/util/libutil.la  $(top_builddir)/source/tasks/libtasks.la \
$(top_builddir)/source/graphics/libgraphics.la $(top_builddir)/source/dialogs/libdialogs.la \
$(top_builddir)/source/dialogs/libgamedialogs.la $(top_builddir)/source/actions/libactions.la \
$(top_builddir)/source/libs/paragui/src/libparagui.la $(top_builddir)/source/widgets/libwidgets.la \
$(top_builddir)/source/sdl/libsdlsnd.la $(top_builddir)/source/network/libnetwork.la  \
$(top_builddir)/source/video/libvideo.la  \
$(top_builddir)/source/actions/libactions.la  \
$(top_builddir)/source/actions/libdirectunitactions.la  \
$(top_builddir)/source/actions/libfullgameactions.la  \
$(top_builddir)/source/actions/libmapactions.la  \
$(top_builddir)/source/actions/libmiscactions.la  \
@LDADDLIBBZ2@  $(SIGC_LIBS) $(SDL_LIBS) $(FREETYPE_LIBS) $(BOOST_LIBS) $(SDL_SOUND_LIB) $(LUA_LIBS)

