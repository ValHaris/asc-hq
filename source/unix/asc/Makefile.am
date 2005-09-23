vpath %.cpp ../..

MAINTAINERCLEANFILES	= Makefile.in

INCLUDES		= -I$(top_builddir) -I$(top_src_dir) \
			  -I$(top_builddir)/source -I$(top_src_dir)/source

AM_CXXFLAGS 		= $(SDL_CFLAGS) -Dsgmain $(PARAGUI_CFLAGS)  $(SIGC_CFLAGS)

bin_PROGRAMS		= asc

asc_SOURCES 		= unitctrl.cpp soundList.cpp typen.cpp \
strtmesg.cpp stack.cpp spfst.cpp sgstream.cpp sg.cpp pd.cpp palette.cpp \
newfont.cpp network.cpp misc.cpp loadpcxc.cpp loaders.cpp \
loadbi3.cpp gui.cpp gamedlg.cpp dlg_box.cpp dialog.cpp controls.cpp \
building.cpp basestrm.cpp basegfx.cpp attack.cpp \
gameoptions.cpp astar2.cpp vehicletype.cpp objects.cpp \
buildingtype.cpp containerbase.cpp mapalgorithms.cpp viewcalculation.cpp \
gamemap.cpp password.cpp password_dialog.cpp replay.cpp research.cpp \
mapdisplay.cpp resourcenet.cpp dashboard.cpp ascstring.cpp graphicset.cpp \
vehicle.cpp buildings.cpp networkinterface.cpp textfileparser.cpp terraintype.cpp \
objecttype.cpp textfiletags.cpp itemrepository.cpp stringtokenizer.cpp music.cpp \
messages.cpp paradialog.cpp textfile_evaluation.cpp containerbasetype.cpp messagedlg.cpp \
simplestream.cpp mappolygons.cpp prehistoricevents.cpp gameevents.cpp gameeventsystem.cpp \
gameevent_dialogs.cpp statisticdialog.cpp clipboard.cpp geometry.cpp weatherarea.cpp \
guidimension.cpp guiiconhandler.cpp guifunctions.cpp iconrepository.cpp gamedialog.cpp \
infopopup.cpp overviewmapimage.cpp asc-mainscreen.cpp mapfield.cpp overviewmappanel.cpp \
weathercast.cpp unitinfodialog.cpp messaginghub.cpp explosivemines.cpp  \
legacynetworkloaders.cpp player.cpp


asc_LDADD   		= $(top_builddir)/source/libs/triangul/libtriangul.la \
$(top_builddir)/source/ai/libai.la $(top_builddir)/source/libs/sdlmm/src/libSDLmm.la \
$(top_builddir)/source/sdl/libsdl.la  $(top_builddir)/source/libs/loki/libloki.la \
$(top_builddir)/source/graphics/libgraphics.la $(top_builddir)/source/dialogs/libdialogs.la \
$(PARAGUI_LIBS) @LDADDLIBBZ2@  $(SIGC_LIBS) $(SDL_LIBS)
