## Process this file with automake to produce Makefile.in
######################################################################
## $Id: Makefile.am,v 1.14 2001-07-08 20:55:14 mbickel Exp $

vpath %.cpp ..:../..

CXXFLAGS = @CXXFLAGS@ -Dconverter

bin_PROGRAMS = asc_demount asc_mount asc_gfx2pcx asc_makegfx asc_mntgraph asc_viewid asc_tank2pcx

asc_demount_SOURCES =  demount.cpp   basestrm.cpp misc.cpp ascstring.cpp strtmesg.cpp
asc_mount_SOURCES   =  mount.cpp     basestrm.cpp misc.cpp ascstring.cpp strtmesg.cpp
asc_gfx2pcx_SOURCES =  bi2pcx.cpp    basestrm.cpp misc.cpp ascstring.cpp loadpcxc.cpp palette.cpp sgstream.cpp newfont.cpp basegfx.cpp graphicset.cpp gameoptions.cpp CLoadable.cpp Named.cpp Property.cpp PropertyGroup.cpp stack.cpp buildingtype.cpp typen.cpp vehicletype.cpp password.cpp strtmesg.cpp
asc_makegfx_SOURCES =  makegfx.cpp   basestrm.cpp misc.cpp ascstring.cpp loadpcxc.cpp palette.cpp sgstream.cpp newfont.cpp basegfx.cpp graphicset.cpp gameoptions.cpp CLoadable.cpp Named.cpp Property.cpp PropertyGroup.cpp stack.cpp buildingtype.cpp typen.cpp vehicletype.cpp password.cpp strtmesg.cpp
asc_mntgraph_SOURCES = mntgraph.cpp  basestrm.cpp misc.cpp ascstring.cpp loadpcxc.cpp palette.cpp sgstream.cpp newfont.cpp basegfx.cpp graphicset.cpp gameoptions.cpp CLoadable.cpp Named.cpp Property.cpp PropertyGroup.cpp stack.cpp buildingtype.cpp typen.cpp vehicletype.cpp password.cpp strtmesg.cpp
asc_viewid_SOURCES =   viewid.cpp    basestrm.cpp misc.cpp ascstring.cpp loadpcxc.cpp palette.cpp sgstream.cpp newfont.cpp basegfx.cpp graphicset.cpp gameoptions.cpp CLoadable.cpp Named.cpp Property.cpp PropertyGroup.cpp stack.cpp buildingtype.cpp typen.cpp vehicletype.cpp password.cpp strtmesg.cpp
asc_tank2pcx_SOURCES = pcxtank.cpp   basestrm.cpp misc.cpp ascstring.cpp loadpcxc.cpp palette.cpp sgstream.cpp newfont.cpp basegfx.cpp graphicset.cpp gameoptions.cpp CLoadable.cpp Named.cpp Property.cpp PropertyGroup.cpp stack.cpp buildingtype.cpp typen.cpp vehicletype.cpp password.cpp strtmesg.cpp

asc_mount_LDADD     = -lSDL -lpthread @LDADDLIBBZ2@
asc_demount_LDADD   = -lSDL -lpthread @LDADDLIBBZ2@
asc_gfx2pcx_LDADD    = ../../sdl/libsdl.a -lSDL -lpthread @LDADDLIBBZ2@
asc_makegfx_LDADD    = ../../sdl/libsdl.a -lSDL -lpthread @LDADDLIBBZ2@
asc_mntgraph_LDADD   = ../../sdl/libsdl.a -lSDL -lpthread @LDADDLIBBZ2@
asc_viewid_LDADD     = ../../sdl/libsdl.a -lSDL -lpthread @LDADDLIBBZ2@
asc_tank2pcx_LDADD   = ../../sdl/libsdl.a -lSDL -lpthread @LDADDLIBBZ2@

# vim: noexpandtab ai


