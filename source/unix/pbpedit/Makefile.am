## Process this file with automake to produce Makefile.in
######################################################################
## $Id: Makefile.am,v 1.1.2.2 2006-12-20 22:28:42 mbickel Exp $

vpath %.cpp ../..

MAINTAINERCLEANFILES	= Makefile.in

INCLUDES		= -I$(top_src_dir) -I$(top_src_dir)/source -I$(top_srcdir)/source/libs/paragui/include

AM_CXXFLAGS 		= $(SDL_CFLAGS) -Dkarteneditor -Dpbpeditor -DNEWKEYB $(PARAGUI_CFLAGS) $(SIGC_CFLAGS)

bin_PROGRAMS 		= asc_pbpedit

asc_pbpedit_SOURCES 	= edevents.cpp edgen.cpp edglobal.cpp edmain.cpp \
edselfnt.cpp edmisc.cpp typen.cpp strtmesg.cpp stack.cpp \
spfst.cpp sgstream.cpp pd.cpp palette.cpp newfont.cpp misc.cpp loadpcxc.cpp \
loaders.cpp loadbi3.cpp dlg_box.cpp dialog.cpp basestrm.cpp basegfx.cpp \
attack.cpp CLoadable.cpp Property.cpp PropertyGroup.cpp gameoptions.cpp \
Named.cpp buildingtype.cpp vehicletype.cpp containerbase.cpp \
mapalgorithms.cpp viewcalculation.cpp gamemap.cpp password.cpp \
password_dialog.cpp research.cpp mapdisplay.cpp ascstring.cpp graphicset.cpp \
vehicle.cpp buildings.cpp networkdata.cpp resourcenet.cpp textfileparser.cpp \
terraintype.cpp objecttype.cpp textfiletags.cpp itemrepository.cpp \
stringtokenizer.cpp music.cpp messages.cpp paradialog.cpp \
textfile_evaluation.cpp containerbasetype.cpp soundList.cpp simplestream.cpp \
mappolygons.cpp prehistoricevents.cpp gameevents.cpp gameeventsystem.cpp \
gameevent_dialogs.cpp clipboard.cpp

asc_pbpedit_LDADD   	= $(top_builddir)/source/sdl/libsdl.la \
$(top_builddir)/source/libs/triangul/libtriangul.la @LDADDLIBBZ2@ \
$(PARAGUI_LIBS) $(SIGC_LIBS) $(top_builddir)/source/libs/sdlmm/src/libSDLmm.la \
$(SDL_LIBS)



