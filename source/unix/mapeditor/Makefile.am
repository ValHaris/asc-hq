## Process this file with automake to produce Makefile.in
######################################################################

vpath %.cpp ../..

MAINTAINERCLEANFILES	= Makefile.in

INCLUDES		= -I$(top_builddir) -I$(top_src_dir) \
			  -I$(top_builddir)/source -I$(top_src_dir)/source

AM_CXXFLAGS 		= $(SDL_CFLAGS) -Dkarteneditor -DNEWKEYB $(PARAGUI_CFLAGS) $(SIGC_CFLAGS)

bin_PROGRAMS 		= asc_mapedit
#asc_map2pcx

asc_mapedit_SOURCES 	= edevents.cpp edgen.cpp edglobal.cpp edmain.cpp \
edselfnt.cpp edmisc.cpp typen.cpp strtmesg.cpp stack.cpp \
spfst.cpp sgstream.cpp pd.cpp palette.cpp newfont.cpp misc.cpp loadpcxc.cpp \
loaders.cpp loadbi3.cpp dlg_box.cpp dialog.cpp basestrm.cpp basegfx.cpp \
attack.cpp gameoptions.cpp maped-mainscreen.cpp \
buildingtype.cpp vehicletype.cpp containerbase.cpp \
mapalgorithms.cpp viewcalculation.cpp gamemap.cpp password.cpp \
password_dialog.cpp research.cpp mapdisplay.cpp ascstring.cpp graphicset.cpp \
vehicle.cpp buildings.cpp networkdata.cpp resourcenet.cpp textfileparser.cpp \
terraintype.cpp objecttype.cpp textfiletags.cpp itemrepository.cpp \
stringtokenizer.cpp music.cpp messages.cpp paradialog.cpp \
textfile_evaluation.cpp containerbasetype.cpp soundList.cpp simplestream.cpp \
mappolygons.cpp prehistoricevents.cpp gameevents.cpp gameeventsystem.cpp \
gameevent_dialogs.cpp clipboard.cpp resourceplacement.cpp resourceplacementdialog.cpp \
geometry.cpp weatherarea.cpp guidimension.cpp weatherdialog.cpp \
iconrepository.cpp overviewmapimage.cpp mapfield.cpp objects.cpp overviewmappanel.cpp \
unitinfodialog.cpp messaginghub.cpp

asc_mapedit_LDADD   	= $(top_builddir)/source/sdl/libsdl.la \
$(top_builddir)/source/libs/triangul/libtriangul.la @LDADDLIBBZ2@ \
$(PARAGUI_LIBS) $(SIGC_LIBS) $(top_builddir)/source/libs/sdlmm/src/libSDLmm.la \
$(top_builddir)/source/graphics/libgraphics.la @SDL_TTF_LDADD@ \
$(top_builddir)/source/libs/loki/libloki.la $(SDL_LIBS)


#asc_map2pcx_SOURCES	= map2pcx.cpp typen.cpp strtmesg.cpp \
#stack.cpp spfst.cpp sgstream.cpp pd.cpp palette.cpp newfont.cpp misc.cpp \
#loadpcxc.cpp loaders.cpp loadbi3.cpp dlg_box.cpp dialog.cpp basestrm.cpp \
#basegfx.cpp attack.cpp \
#gameoptions.cpp buildingtype.cpp vehicletype.cpp containerbase.cpp \
#mapalgorithms.cpp viewcalculation.cpp gamemap.cpp password.cpp \
#password_dialog.cpp research.cpp mapdisplay.cpp ascstring.cpp graphicset.cpp \
#vehicle.cpp buildings.cpp networkdata.cpp resourcenet.cpp textfileparser.cpp \
#terraintype.cpp objecttype.cpp textfiletags.cpp itemrepository.cpp \
#stringtokenizer.cpp music.cpp messages.cpp paradialog.cpp \
#textfile_evaluation.cpp containerbasetype.cpp soundList.cpp simplestream.cpp \
#mappolygons.cpp prehistoricevents.cpp gameevents.cpp gameeventsystem.cpp \
#gameevent_dialogs.cpp clipboard.cpp

#asc_map2pcx_LDADD   	= $(top_builddir)/source/libs/triangul/libtriangul.la \
#$(top_builddir)/source/sdl/libsdl.la  @LDADDLIBBZ2@ $(PARAGUI_LIBS) \
#$(SIGC_LIBS)

# vim: noexpandtab ai


