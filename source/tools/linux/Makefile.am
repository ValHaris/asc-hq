## Process this file with automake to produce Makefile.in
######################################################################
## $Id: Makefile.am,v 1.9 2001-02-28 14:10:05 mbickel Exp $

vpath %.cpp ..:../..

CXXFLAGS = @CXXFLAGS@ -Dconverter

bin_PROGRAMS = ascdemount ascmount asc_gfx2pcx asc_makegfx asc_mntgraph

ascdemount_SOURCES = demount.cpp basestrm.cpp misc.cpp ascstring.cpp
ascmount_SOURCES   = mount.cpp   basestrm.cpp misc.cpp ascstring.cpp
asc_gfx2pcx_SOURCES =  bi2pcx.cpp    basestrm.cpp misc.cpp ascstring.cpp loadpcxc.cpp palette.cpp sgstream.cpp newfont.cpp basegfx.cpp graphicset.cpp gameoptions.cpp CLoadable.cpp Named.cpp Property.cpp PropertyGroup.cpp stack.cpp buildingtype.cpp typen.cpp vehicletype.cpp password.cpp
asc_makegfx_SOURCES =  makegfx.cpp   basestrm.cpp misc.cpp ascstring.cpp loadpcxc.cpp palette.cpp sgstream.cpp newfont.cpp basegfx.cpp graphicset.cpp gameoptions.cpp CLoadable.cpp Named.cpp Property.cpp PropertyGroup.cpp stack.cpp buildingtype.cpp typen.cpp vehicletype.cpp password.cpp
asc_mntgraph_SOURCES = mntgraph.cpp  basestrm.cpp misc.cpp ascstring.cpp loadpcxc.cpp palette.cpp sgstream.cpp newfont.cpp basegfx.cpp graphicset.cpp gameoptions.cpp CLoadable.cpp Named.cpp Property.cpp PropertyGroup.cpp stack.cpp buildingtype.cpp typen.cpp vehicletype.cpp password.cpp

ascmount_LDADD     = -lSDL -lpthread @LDADDLIBBZ2@
ascdemount_LDADD   = -lSDL -lpthread @LDADDLIBBZ2@
asc_gfx2pcx_LDADD    = ../../sdl/libsdl.a -lSDL -lpthread @LDADDLIBBZ2@
asc_makegfx_LDADD    = ../../sdl/libsdl.a -lSDL -lpthread @LDADDLIBBZ2@
asc_mntgraph_LDADD   = ../../sdl/libsdl.a -lSDL -lpthread @LDADDLIBBZ2@

# vim: noexpandtab ai


