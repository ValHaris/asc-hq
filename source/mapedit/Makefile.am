## Process this file with automake to produce Makefile.in
######################################################################
## $Id: Makefile.am,v 1.6 2000-07-29 15:28:37 mbickel Exp $

vpath %.cpp ..

CXXFLAGS = @CXXFLAGS@ -Dkarteneditor -I./ -DHEXAGON -DFREEMAPZOOM -funsigned-char -D_NOASM_ -DNEWKEYB

bin_PROGRAMS = mapedit

mapedit_SOURCES = edevents.cpp edgen.cpp edglobal.cpp edmain.cpp edselfnt.cpp edmisc.cpp weather.cpp typen.cpp strtmesg.cpp stack.cpp spfst.cpp sgstream.cpp pd.cpp palette.cpp newfont.cpp misc.cpp loadpcxc.cpp loadjpg.cpp loaders.cpp loadbi3.cpp dlg_box.cpp dialog.cpp basestrm.cpp basegfx.cpp attack.cpp CLoadable.cpp Property.cpp PropertyGroup.cpp gameoptions.cpp Named.cpp
mapedit_LDADD   = ../sdl/libsdl.a ../libs/triangul/libtriangul.a -lSDL -lpthread -lbz2

# vim: noexpandtab ai

