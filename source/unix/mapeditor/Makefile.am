## Process this file with automake to produce Makefile.in
######################################################################
## $Id: Makefile.am,v 1.2 2001-07-27 21:13:36 mbickel Exp $

vpath %.cpp ../..

CXXFLAGS = @CXXFLAGS@ -Dkarteneditor -I./ -DFREEMAPZOOM -DNEWKEYB

bin_PROGRAMS = asc_mapedit asc_map2pcx

asc_mapedit_SOURCES = edevents.cpp edgen.cpp edglobal.cpp edmain.cpp edselfnt.cpp edmisc.cpp weather.cpp typen.cpp strtmesg.cpp stack.cpp spfst.cpp sgstream.cpp pd.cpp palette.cpp newfont.cpp misc.cpp loadpcxc.cpp loaders.cpp loadbi3.cpp dlg_box.cpp dialog.cpp basestrm.cpp basegfx.cpp attack.cpp CLoadable.cpp Property.cpp PropertyGroup.cpp gameoptions.cpp Named.cpp buildingtype.cpp vehicletype.cpp containerbase.cpp mapalgorithms.cpp viewcalculation.cpp gamemap.cpp password.cpp password_dialog.cpp research.cpp mapdisplay.cpp ascstring.cpp graphicset.cpp vehicle.cpp buildings.cpp networkdata.cpp resourcenet.cpp textfileparser.cpp terraintype.cpp objecttype.cpp textfiletags.cpp
asc_mapedit_LDADD   = ../../sdl/libsdl.a ../../libs/triangul/libtriangul.a -lSDL -lpthread @LDADDLIBBZ2@

asc_map2pcx_SOURCES = map2pcx.cpp weather.cpp typen.cpp strtmesg.cpp stack.cpp spfst.cpp sgstream.cpp pd.cpp palette.cpp newfont.cpp misc.cpp loadpcxc.cpp loaders.cpp loadbi3.cpp dlg_box.cpp dialog.cpp basestrm.cpp basegfx.cpp attack.cpp CLoadable.cpp Property.cpp PropertyGroup.cpp gameoptions.cpp Named.cpp buildingtype.cpp vehicletype.cpp containerbase.cpp mapalgorithms.cpp viewcalculation.cpp gamemap.cpp password.cpp password_dialog.cpp research.cpp mapdisplay.cpp ascstring.cpp graphicset.cpp vehicle.cpp buildings.cpp networkdata.cpp resourcenet.cpp textfileparser.cpp terraintype.cpp objecttype.cpp textfiletags.cpp
asc_map2pcx_LDADD   = ../../sdl/libsdl.a ../../libs/triangul/libtriangul.a -lSDL -lpthread -lSDL_image @LDADDLIBBZ2@
		
		
# vim: noexpandtab ai


