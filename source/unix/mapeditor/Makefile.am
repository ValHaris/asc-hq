## Process this file with automake to produce Makefile.in
######################################################################
## $Id: Makefile.am,v 1.8 2002-02-14 20:58:14 mbickel Exp $

vpath %.cpp ../..

MAINTAINERCLEANFILES	= Makefile.in

INCLUDES		= -I$(top_builddir) -I$(top_src_dir) \
			  -I$(top_builddir)/source -I$(top_src_dir)/source

CXXFLAGS 		= @CXXFLAGS@ @SDL_CFLAGS@ @SDLMM_CXXFLAGS@ \
-Dkarteneditor -DFREEMAPZOOM -DNEWKEYB

bin_PROGRAMS 		= asc_mapedit asc_map2pcx

asc_mapedit_SOURCES 	= edevents.cpp edgen.cpp edglobal.cpp edmain.cpp \
edselfnt.cpp edmisc.cpp weather.cpp typen.cpp strtmesg.cpp stack.cpp \
spfst.cpp sgstream.cpp pd.cpp palette.cpp newfont.cpp misc.cpp loadpcxc.cpp \
loaders.cpp loadbi3.cpp dlg_box.cpp dialog.cpp basestrm.cpp basegfx.cpp \
attack.cpp CLoadable.cpp Property.cpp PropertyGroup.cpp gameoptions.cpp \
Named.cpp buildingtype.cpp vehicletype.cpp containerbase.cpp \
mapalgorithms.cpp viewcalculation.cpp gamemap.cpp password.cpp \
password_dialog.cpp research.cpp mapdisplay.cpp ascstring.cpp graphicset.cpp \
vehicle.cpp buildings.cpp networkdata.cpp resourcenet.cpp textfileparser.cpp \
terraintype.cpp objecttype.cpp textfiletags.cpp itemrepository.cpp \
stringtokenizer.cpp music.cpp
asc_mapedit_LDADD   	= $(top_builddir)/source/sdl/libsdl.la \
$(top_builddir)/source/libs/triangul/libtriangul.la @SDLMM_LIBS@ @LDADDLIBBZ2@

asc_map2pcx_SOURCES	= map2pcx.cpp weather.cpp typen.cpp strtmesg.cpp \
stack.cpp spfst.cpp sgstream.cpp pd.cpp palette.cpp newfont.cpp misc.cpp \
loadpcxc.cpp loaders.cpp loadbi3.cpp dlg_box.cpp dialog.cpp basestrm.cpp \
basegfx.cpp attack.cpp CLoadable.cpp Property.cpp PropertyGroup.cpp \
gameoptions.cpp Named.cpp buildingtype.cpp vehicletype.cpp containerbase.cpp \
mapalgorithms.cpp viewcalculation.cpp gamemap.cpp password.cpp \
password_dialog.cpp research.cpp mapdisplay.cpp ascstring.cpp graphicset.cpp \
vehicle.cpp buildings.cpp networkdata.cpp resourcenet.cpp textfileparser.cpp \
terraintype.cpp objecttype.cpp textfiletags.cpp itemrepository.cpp \
stringtokenizer.cpp music.cpp

asc_map2pcx_LDADD   	= $(top_builddir)/source/libs/triangul/libtriangul.la \
$(top_builddir)/source/sdl/libsdl.la @SDLMM_LIBS@ @LDADDLIBBZ2@

# vim: noexpandtab ai


