## Process this file with automake to produce Makefile.in
######################################################################

vpath %.cpp $(srcdir)/..:$(srcdir)/../..

MAINTAINERCLEANFILES	= Makefile.in

AM_CPPFLAGS		= -I $(top_builddir) -I$(top_src_dir)/source

AM_CXXFLAGS 		= $(SDL_CFLAGS) -Dconverter $(SIGC_CFLAGS)  @LokiInclude@

bin_PROGRAMS 		= asc_demount \
			asc_mount \
			asc_weaponguide
#			asc_makegfx \
#			asc_viewid \
#			asc_bin2text

noinst_PROGRAMS		= asc_raw2png 

nodist_asc_demount_SOURCES     = demount.cpp      basestrm.cpp simplestream.cpp misc.cpp ascstring.cpp strtmesg.cpp stringtokenizer.cpp
nodist_asc_mount_SOURCES       = mount.cpp        basestrm.cpp simplestream.cpp misc.cpp ascstring.cpp strtmesg.cpp stringtokenizer.cpp
#asc_makegfx_SOURCES     = makegfx.cpp      basestrm.cpp simplestream.cpp misc.cpp ascstring.cpp loadpcxc.cpp palette.cpp sgstream.cpp newfont.cpp basegfx.cpp graphicset.cpp gameoptions.cpp  stack.cpp buildingtype.cpp typen.cpp vehicletype.cpp password.cpp strtmesg.cpp textfileparser.cpp containerbasetype.cpp   terraintype.cpp objecttype.cpp textfiletags.cpp textfile_evaluation.cpp stringtokenizer.cpp research.cpp itemrepository.cpp overviewmapimage.cpp iconrepository.cpp messaginghub.cpp
#asc_viewid_SOURCES      = viewid.cpp       basestrm.cpp simplestream.cpp misc.cpp ascstring.cpp loadpcxc.cpp palette.cpp sgstream.cpp newfont.cpp basegfx.cpp graphicset.cpp gameoptions.cpp stack.cpp buildingtype.cpp typen.cpp vehicletype.cpp password.cpp strtmesg.cpp textfileparser.cpp containerbasetype.cpp  terraintype.cpp objecttype.cpp textfiletags.cpp textfile_evaluation.cpp stringtokenizer.cpp research.cpp itemrepository.cpp
#asc_bin2text_SOURCES    = bin2text.cpp     basestrm.cpp simplestream.cpp misc.cpp ascstring.cpp loadpcxc.cpp palette.cpp sgstream.cpp newfont.cpp basegfx.cpp graphicset.cpp gameoptions.cpp stack.cpp buildingtype.cpp typen.cpp vehicletype.cpp password.cpp strtmesg.cpp textfileparser.cpp containerbasetype.cpp  terraintype.cpp objecttype.cpp textfiletags.cpp textfile_evaluation.cpp stringtokenizer.cpp research.cpp itemrepository.cpp
nodist_asc_weaponguide_SOURCES = weaponguide.cpp  basestrm.cpp simplestream.cpp misc.cpp ascstring.cpp loadpcxc.cpp palette.cpp sgstream.cpp newfont.cpp \
                                 basegfx.cpp graphicset.cpp gameoptions.cpp  stack.cpp buildingtype.cpp typen.cpp vehicletype.cpp password.cpp strtmesg.cpp \
                                 textfileparser.cpp containerbasetype.cpp   terraintype.cpp objecttype.cpp textfiletags.cpp textfile_evaluation.cpp \
                                 stringtokenizer.cpp research.cpp itemrepository.cpp groupfile.cpp guidegenerator.cpp infopage.cpp overviewmapimage.cpp \
                                 iconrepository.cpp objects.cpp unitset.cpp fieldimageloader.cpp stdio-errorhandler.cpp playercolor.cpp versionidentifier.cpp \
                                 package.cpp packagerepository.cpp accessconstraints-editor.cpp unitcostcalculator-standard.cpp unitcostcalculator-pbp.cpp
nodist_asc_raw2png_SOURCES     = raw2png.cpp      basestrm.cpp simplestream.cpp misc.cpp ascstring.cpp loadpcxc.cpp palette.cpp sgstream.cpp newfont.cpp \
                                 basegfx.cpp graphicset.cpp gameoptions.cpp  stack.cpp buildingtype.cpp typen.cpp vehicletype.cpp password.cpp strtmesg.cpp \
                                 textfileparser.cpp containerbasetype.cpp   terraintype.cpp objecttype.cpp textfiletags.cpp textfile_evaluation.cpp \
                                 stringtokenizer.cpp research.cpp itemrepository.cpp groupfile.cpp guidegenerator.cpp infopage.cpp overviewmapimage.cpp \
                                 iconrepository.cpp objects.cpp unitset.cpp fieldimageloader.cpp stdio-errorhandler.cpp playercolor.cpp versionidentifier.cpp \
                                 package.cpp packagerepository.cpp accessconstraints-editor.cpp unitcostcalculator-standard.cpp unitcostcalculator-pbp.cpp

asc_mount_LDADD       = $(SDL_LIBS) $(SIGC_LIBS) $(BZIP2_LIBS) $(top_builddir)/source/tools/lib/libasc_cmd.la
asc_demount_LDADD     = $(SDL_LIBS) $(SIGC_LIBS) $(BZIP2_LIBS) $(top_builddir)/source/tools/lib/libasc_cmd.la
#asc_makegfx_LDADD     = $(SDL_LIBS) $(top_builddir)/source/sdl/libsdl.la @LDADDLIBBZ2@ $(SIGC_LIBS) $(top_builddir)/source/tools/lib/libasc_cmd.la $(top_builddir)/source/libs/sdlmm/src/libSDLmm.la 
#asc_viewid_LDADD      = $(top_builddir)/source/sdl/libsdl.la @LDADDLIBBZ2@ @SIGC_LIBS@ $(top_builddir)/source/tools/lib/libasc_cmd.la
#asc_bin2text_LDADD    = $(top_builddir)/source/sdl/libsdl.la @LDADDLIBBZ2@ @SIGC_LIBS@ $(top_builddir)/source/tools/lib/libasc_cmd.la
asc_weaponguide_LDADD = $(SDL_LIBS) $(top_builddir)/source/sdl/libsdl.la $(top_builddir)/source/util/libutil.la $(BZIP2_LIBS) $(SIGC_LIBS) $(top_builddir)/source/tools/lib/libasc_cmd.la $(top_builddir)/source/libs/sdlmm/src/libSDLmm.la  $(top_builddir)/source/graphics/libgraphics.la $(LOKI_LIBS) $(BOOST_LIBS)
asc_raw2png_LDADD     = $(SDL_LIBS) $(top_builddir)/source/sdl/libsdl.la $(top_builddir)/source/util/libutil.la $(BZIP2_LIBS) $(SIGC_LIBS) $(top_builddir)/source/tools/lib/libasc_cmd.la $(top_builddir)/source/libs/sdlmm/src/libSDLmm.la  $(top_builddir)/source/graphics/libgraphics.la $(LOKI_LIBS) $(BOOST_LIBS)

# vim: noexpandtab ai


