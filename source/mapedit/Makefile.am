## Process this file with automake to produce Makefile.in
######################################################################
## $Id: Makefile.am,v 1.9 2000-10-11 14:26:56 mbickel Exp $

vpath %.cpp ..

CXXFLAGS = @CXXFLAGS@ -Dkarteneditor -I./ -DHEXAGON -DFREEMAPZOOM -funsigned-char -D_NOASM_ -DNEWKEYB

bin_PROGRAMS = ascmapedit

ascmapedit_SOURCES = edevents.cpp edgen.cpp edglobal.cpp edmain.cpp edselfnt.cpp edmisc.cpp weather.cpp typen.cpp strtmesg.cpp stack.cpp spfst.cpp sgstream.cpp pd.cpp palette.cpp newfont.cpp misc.cpp loadpcxc.cpp loaders.cpp loadbi3.cpp dlg_box.cpp dialog.cpp basestrm.cpp basegfx.cpp attack.cpp CLoadable.cpp Property.cpp PropertyGroup.cpp gameoptions.cpp Named.cpp buildingtype.cpp vehicletype.cpp containerbase.cpp mapalgorithms.cpp viewcalculation.cpp
ascmapedit_LDADD   = ../sdl/libsdl.a ../libs/triangul/libtriangul.a -lSDL -lpthread ../libs/bzlib/libbz2.a

# vim: noexpandtab ai

