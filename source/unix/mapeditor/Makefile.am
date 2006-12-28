vpath %.cpp $(srcdir)/../..

MAINTAINERCLEANFILES	= Makefile.in

INCLUDES		= -I$(top_src_dir) -I$(top_src_dir)/source -I$(top_srcdir)/source/libs/paragui/include

AM_CXXFLAGS 		= $(SDL_CFLAGS) -Dkarteneditor  $(SIGC_CFLAGS)

bin_PROGRAMS 		= asc_mapedit
#asc_map2pcx

nodist_asc_mapedit_SOURCES 	= edgen.cpp edglobal.cpp edmain.cpp \
edselfnt.cpp edmisc.cpp typen.cpp strtmesg.cpp stack.cpp \
spfst.cpp sgstream.cpp pd.cpp palette.cpp newfont.cpp misc.cpp loadpcxc.cpp \
loaders.cpp loadbi3.cpp dlg_box.cpp dialog.cpp basestrm.cpp basegfx.cpp \
attack.cpp gameoptions.cpp maped-mainscreen.cpp stdio-errorhandler.cpp\
buildingtype.cpp vehicletype.cpp containerbase.cpp ed_mapcomponent.cpp \
mapalgorithms.cpp viewcalculation.cpp gamemap.cpp password.cpp \
research.cpp mapdisplay.cpp ascstring.cpp graphicset.cpp \
vehicle.cpp buildings.cpp networkinterface.cpp resourcenet.cpp textfileparser.cpp \
terraintype.cpp objecttype.cpp textfiletags.cpp itemrepository.cpp \
stringtokenizer.cpp music.cpp messages.cpp paradialog.cpp \
textfile_evaluation.cpp containerbasetype.cpp simplestream.cpp \
mappolygons.cpp prehistoricevents.cpp gameevents.cpp gameeventsystem.cpp \
gameevent_dialogs.cpp clipboard.cpp resourceplacement.cpp resourceplacementdialog.cpp \
geometry.cpp guidimension.cpp unitset.cpp fieldimageloader.cpp \
iconrepository.cpp overviewmapimage.cpp mapfield.cpp objects.cpp overviewmappanel.cpp \
messaginghub.cpp explosivemines.cpp legacynetworkloaders.cpp \
player.cpp cannedmessages.cpp mapimageexport.cpp mainscreenwidget.cpp windowing.cpp

asc_mapedit_LDADD   	= $(top_builddir)/source/sdl/libsdl.la \
$(top_builddir)/source/libs/triangul/libtriangul.la @LDADDLIBBZ2@ \
$(SIGC_LIBS) $(top_builddir)/source/libs/sdlmm/src/libSDLmm.la \
$(top_builddir)/source/graphics/libgraphics.la $(top_builddir)/source/dialogs/libdialogs.la \
$(top_builddir)/source/dialogs/libmapeddialogs.la  $(top_builddir)/source/widgets/libwidgets.la \
$(top_builddir)/source/libs/paragui/src/libparagui.la \
$(top_builddir)/source/libs/loki/libloki.la $(SDL_LIBS) $(FREETYPE_LIBS) $(BOOST_LIBS)


