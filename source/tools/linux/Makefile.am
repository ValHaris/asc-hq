## Process this file with automake to produce Makefile.in
######################################################################
## $Id: Makefile.am,v 1.20 2001-10-02 14:06:29 mbickel Exp $

vpath %.cpp ..:../..

CXXFLAGS = @CXXFLAGS@ -Dconverter

bin_PROGRAMS = asc_demount \
               asc_mount   \
               asc_makegfx \
               asc_mntgraph \
               asc_viewid \
               asc_tank2pcx \
               asc_bin2text \
               asc_weaponguide

asc_demount_SOURCES     = demount.cpp      basestrm.cpp misc.cpp ascstring.cpp strtmesg.cpp
asc_mount_SOURCES       = mount.cpp        basestrm.cpp misc.cpp ascstring.cpp strtmesg.cpp
#asc_gfx2pcx_SOURCES     = bi2pcx.cpp       basestrm.cpp misc.cpp ascstring.cpp loadpcxc.cpp palette.cpp sgstream.cpp newfont.cpp basegfx.cpp graphicset.cpp gameoptions.cpp CLoadable.cpp Named.cpp Property.cpp PropertyGroup.cpp stack.cpp buildingtype.cpp typen.cpp vehicletype.cpp password.cpp strtmesg.cpp textfileparser.cpp terraintype.cpp objecttype.cpp textfiletags.cpp
asc_makegfx_SOURCES     = makegfx.cpp      basestrm.cpp misc.cpp ascstring.cpp loadpcxc.cpp palette.cpp sgstream.cpp newfont.cpp basegfx.cpp graphicset.cpp gameoptions.cpp CLoadable.cpp Named.cpp Property.cpp PropertyGroup.cpp stack.cpp buildingtype.cpp typen.cpp vehicletype.cpp password.cpp strtmesg.cpp textfileparser.cpp terraintype.cpp objecttype.cpp textfiletags.cpp
asc_mntgraph_SOURCES    = mntgraph.cpp     basestrm.cpp misc.cpp ascstring.cpp loadpcxc.cpp palette.cpp sgstream.cpp newfont.cpp basegfx.cpp graphicset.cpp gameoptions.cpp CLoadable.cpp Named.cpp Property.cpp PropertyGroup.cpp stack.cpp buildingtype.cpp typen.cpp vehicletype.cpp password.cpp strtmesg.cpp textfileparser.cpp terraintype.cpp objecttype.cpp textfiletags.cpp
asc_viewid_SOURCES      = viewid.cpp       basestrm.cpp misc.cpp ascstring.cpp loadpcxc.cpp palette.cpp sgstream.cpp newfont.cpp basegfx.cpp graphicset.cpp gameoptions.cpp CLoadable.cpp Named.cpp Property.cpp PropertyGroup.cpp stack.cpp buildingtype.cpp typen.cpp vehicletype.cpp password.cpp strtmesg.cpp textfileparser.cpp terraintype.cpp objecttype.cpp textfiletags.cpp
asc_tank2pcx_SOURCES    = pcxtank.cpp      basestrm.cpp misc.cpp ascstring.cpp loadpcxc.cpp palette.cpp sgstream.cpp newfont.cpp basegfx.cpp graphicset.cpp gameoptions.cpp CLoadable.cpp Named.cpp Property.cpp PropertyGroup.cpp stack.cpp buildingtype.cpp typen.cpp vehicletype.cpp password.cpp strtmesg.cpp textfileparser.cpp terraintype.cpp objecttype.cpp textfiletags.cpp
asc_bin2text_SOURCES    = bin2text.cpp     basestrm.cpp misc.cpp ascstring.cpp loadpcxc.cpp palette.cpp sgstream.cpp newfont.cpp basegfx.cpp graphicset.cpp gameoptions.cpp CLoadable.cpp Named.cpp Property.cpp PropertyGroup.cpp stack.cpp buildingtype.cpp typen.cpp vehicletype.cpp password.cpp strtmesg.cpp textfileparser.cpp terraintype.cpp objecttype.cpp textfiletags.cpp itemrepository.cpp 
asc_weaponguide_SOURCES = weaponguide.cpp  basestrm.cpp misc.cpp ascstring.cpp loadpcxc.cpp palette.cpp sgstream.cpp newfont.cpp basegfx.cpp graphicset.cpp gameoptions.cpp CLoadable.cpp Named.cpp Property.cpp PropertyGroup.cpp stack.cpp buildingtype.cpp typen.cpp vehicletype.cpp password.cpp strtmesg.cpp textfileparser.cpp terraintype.cpp objecttype.cpp textfiletags.cpp itemrepository.cpp


asc_mount_LDADD     = -lSDL -lSDLmm -lpthread @LDADDLIBBZ2@
asc_demount_LDADD   = -lSDL -lSDLmm -lpthread @LDADDLIBBZ2@
#asc_gfx2pcx_LDADD    = ../../sdl/libsdl.a -lSDL -lSDLmm -lpthread @LDADDLIBBZ2@
asc_makegfx_LDADD    = ../../sdl/libsdl.a -lSDL -lSDLmm -lpthread @LDADDLIBBZ2@
asc_mntgraph_LDADD   = ../../sdl/libsdl.a -lSDL -lSDLmm -lpthread @LDADDLIBBZ2@
asc_viewid_LDADD     = ../../sdl/libsdl.a -lSDL -lSDLmm -lpthread @LDADDLIBBZ2@
asc_tank2pcx_LDADD   = ../../sdl/libsdl.a -lSDL -lSDLmm -lpthread @LDADDLIBBZ2@
asc_bin2text_LDADD   = ../../sdl/libsdl.a -lSDL -lSDLmm -lpthread @LDADDLIBBZ2@
asc_weaponguide_LDADD =../../sdl/libsdl.a -lSDL -lSDLmm -lpthread @LDADDLIBBZ2@

# vim: noexpandtab ai


