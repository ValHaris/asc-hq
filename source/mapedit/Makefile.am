## Process this file with automake to produce Makefile.in
######################################################################
## $Id: Makefile.am,v 1.14 2001-01-23 21:05:26 mbickel Exp $

vpath %.cpp ..

CXXFLAGS = @CXXFLAGS@ -Dkarteneditor -I./ -DFREEMAPZOOM -DNEWKEYB

bin_PROGRAMS = ascmapedit

ascmapedit_SOURCES = edevents.cpp edgen.cpp edglobal.cpp edmain.cpp edselfnt.cpp edmisc.cpp weather.cpp typen.cpp strtmesg.cpp stack.cpp spfst.cpp sgstream.cpp pd.cpp palette.cpp newfont.cpp misc.cpp loadpcxc.cpp loaders.cpp loadbi3.cpp dlg_box.cpp dialog.cpp basestrm.cpp basegfx.cpp attack.cpp CLoadable.cpp Property.cpp PropertyGroup.cpp gameoptions.cpp Named.cpp buildingtype.cpp vehicletype.cpp containerbase.cpp mapalgorithms.cpp viewcalculation.cpp gamemap.cpp password.cpp password_dialog.cpp research.cpp
ascmapedit_LDADD   = ../sdl/libsdl.a ../libs/triangul/libtriangul.a -lSDL \
		    -lpthread @LDADDLIBBZ2@

# vim: noexpandtab ai


