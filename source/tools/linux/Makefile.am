## Process this file with automake to produce Makefile.in
######################################################################
## $Id: Makefile.am,v 1.10 2001-02-28 21:27:59 mbickel Exp $

vpath %.cpp ..:../..

CXXFLAGS = @CXXFLAGS@ -Dconverter

bin_PROGRAMS = asc_demount asc_mount asc_gfx2pcx asc_makegfx asc_mntgraph

asc_demount_SOURCES = demount.cpp basestrm.cpp misc.cpp ascstring.cpp
asc_mount_SOURCES   = mount.cpp   basestrm.cpp misc.cpp ascstring.cpp
asc_gfx2pcx_SOURCES =  bi2pcx.cpp    basestrm.cpp misc.cpp ascstring.cpp loadpcxc.cpp palette.cpp sgstream.cpp newfont.cpp basegfx.cpp graphicset.cpp gameoptions.cpp CLoadable.cpp Named.cpp Property.cpp PropertyGroup.cpp stack.cpp buildingtype.cpp typen.cpp vehicletype.cpp password.cpp
asc_makegfx_SOURCES =  makegfx.cpp   basestrm.cpp misc.cpp ascstring.cpp loadpcxc.cpp palette.cpp sgstream.cpp newfont.cpp basegfx.cpp graphicset.cpp gameoptions.cpp CLoadable.cpp Named.cpp Property.cpp PropertyGroup.cpp stack.cpp buildingtype.cpp typen.cpp vehicletype.cpp password.cpp
asc_mntgraph_SOURCES = mntgraph.cpp  basestrm.cpp misc.cpp ascstring.cpp loadpcxc.cpp palette.cpp sgstream.cpp newfont.cpp basegfx.cpp graphicset.cpp gameoptions.cpp CLoadable.cpp Named.cpp Property.cpp PropertyGroup.cpp stack.cpp buildingtype.cpp typen.cpp vehicletype.cpp password.cpp

asc_mount_LDADD     = -lSDL -lpthread @LDADDLIBBZ2@
asc_demount_LDADD   = -lSDL -lpthread @LDADDLIBBZ2@
asc_gfx2pcx_LDADD    = ../../sdl/libsdl.a -lSDL -lpthread @LDADDLIBBZ2@
asc_makegfx_LDADD    = ../../sdl/libsdl.a -lSDL -lpthread @LDADDLIBBZ2@
asc_mntgraph_LDADD   = ../../sdl/libsdl.a -lSDL -lpthread @LDADDLIBBZ2@

# vim: noexpandtab ai


