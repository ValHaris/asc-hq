## Process this file with automake to produce Makefile.in
######################################################################
## $Id: Makefile.am,v 1.33 2004-01-16 21:17:25 mbickel Exp $

vpath %.cpp ..:../..

MAINTAINERCLEANFILES	= Makefile.in

INCLUDES		= -I $(top_builddir) -I$(top_src_dir) \
-I$(top_builddir)/source -I$(top_src_dir)/source
## @OS_INCLUDE@

AM_CXXFLAGS 		= @SDL_CFLAGS@ @SDLMM_CXXFLAGS@ -Dconverter @SIGC_CFLAGS@

bin_PROGRAMS 		= asc_demount \
			asc_mount \
			asc_makegfx \
			asc_viewid \
			asc_bin2text \
			asc_weaponguide \
			asc_raw2pcx

asc_demount_SOURCES	    = demount.cpp      basestrm.cpp simplestream.cpp misc.cpp ascstring.cpp strtmesg.cpp stringtokenizer.cpp
asc_mount_SOURCES       = mount.cpp        basestrm.cpp simplestream.cpp misc.cpp ascstring.cpp strtmesg.cpp stringtokenizer.cpp
asc_makegfx_SOURCES     = makegfx.cpp      basestrm.cpp simplestream.cpp misc.cpp ascstring.cpp loadpcxc.cpp palette.cpp sgstream.cpp newfont.cpp basegfx.cpp graphicset.cpp gameoptions.cpp CLoadable.cpp Named.cpp Property.cpp PropertyGroup.cpp stack.cpp buildingtype.cpp typen.cpp vehicletype.cpp password.cpp strtmesg.cpp textfileparser.cpp containerbasetype.cpp terraintype.cpp objecttype.cpp textfiletags.cpp textfile_evaluation.cpp stringtokenizer.cpp
asc_viewid_SOURCES      = viewid.cpp       basestrm.cpp simplestream.cpp misc.cpp ascstring.cpp loadpcxc.cpp palette.cpp sgstream.cpp newfont.cpp basegfx.cpp graphicset.cpp gameoptions.cpp CLoadable.cpp Named.cpp Property.cpp PropertyGroup.cpp stack.cpp buildingtype.cpp typen.cpp vehicletype.cpp password.cpp strtmesg.cpp textfileparser.cpp containerbasetype.cpp terraintype.cpp objecttype.cpp textfiletags.cpp textfile_evaluation.cpp stringtokenizer.cpp itemrepository.cpp
asc_bin2text_SOURCES    = bin2text.cpp     basestrm.cpp simplestream.cpp misc.cpp ascstring.cpp loadpcxc.cpp palette.cpp sgstream.cpp newfont.cpp basegfx.cpp graphicset.cpp gameoptions.cpp CLoadable.cpp Named.cpp Property.cpp PropertyGroup.cpp stack.cpp buildingtype.cpp typen.cpp vehicletype.cpp password.cpp strtmesg.cpp textfileparser.cpp containerbasetype.cpp terraintype.cpp objecttype.cpp textfiletags.cpp textfile_evaluation.cpp stringtokenizer.cpp itemrepository.cpp
asc_weaponguide_SOURCES = weaponguide.cpp  basestrm.cpp simplestream.cpp misc.cpp ascstring.cpp loadpcxc.cpp palette.cpp sgstream.cpp newfont.cpp basegfx.cpp graphicset.cpp gameoptions.cpp CLoadable.cpp Named.cpp Property.cpp PropertyGroup.cpp stack.cpp buildingtype.cpp typen.cpp vehicletype.cpp password.cpp strtmesg.cpp textfileparser.cpp containerbasetype.cpp terraintype.cpp objecttype.cpp textfiletags.cpp textfile_evaluation.cpp stringtokenizer.cpp itemrepository.cpp
asc_raw2pcx_SOURCES     = raw2pcx.cpp      basestrm.cpp simplestream.cpp misc.cpp ascstring.cpp loadpcxc.cpp palette.cpp sgstream.cpp newfont.cpp basegfx.cpp graphicset.cpp gameoptions.cpp CLoadable.cpp Named.cpp Property.cpp PropertyGroup.cpp stack.cpp buildingtype.cpp typen.cpp vehicletype.cpp password.cpp strtmesg.cpp textfileparser.cpp containerbasetype.cpp terraintype.cpp objecttype.cpp textfiletags.cpp textfile_evaluation.cpp stringtokenizer.cpp

asc_mount_LDADD       = @SDL_LIBS@ @SDLMM_LIBS@ @LDADDLIBBZ2@ $(top_builddir)/source/tools/lib/libasc_cmd.la
asc_demount_LDADD     = @SDL_LIBS@ @SDLMM_LIBS@ @LDADDLIBBZ2@ $(top_builddir)/source/tools/lib/libasc_cmd.la
asc_makegfx_LDADD     = $(top_builddir)/source/sdl/libsdl.la @LDADDLIBBZ2@ @SIGC_LIBS@ $(top_builddir)/source/tools/lib/libasc_cmd.la
asc_viewid_LDADD      = $(top_builddir)/source/sdl/libsdl.la @LDADDLIBBZ2@ @SIGC_LIBS@ $(top_builddir)/source/tools/lib/libasc_cmd.la
asc_bin2text_LDADD    = $(top_builddir)/source/sdl/libsdl.la @LDADDLIBBZ2@ @SIGC_LIBS@ $(top_builddir)/source/tools/lib/libasc_cmd.la
asc_weaponguide_LDADD = $(top_builddir)/source/sdl/libsdl.la @LDADDLIBBZ2@ @SIGC_LIBS@ $(top_builddir)/source/tools/lib/libasc_cmd.la
asc_raw2pcx_LDADD     = $(top_builddir)/source/sdl/libsdl.la @LDADDLIBBZ2@ @SIGC_LIBS@ $(top_builddir)/source/tools/lib/libasc_cmd.la

# vim: noexpandtab ai


