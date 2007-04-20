SUBDIRS      = dialog  @MUSICDIR@
DIST_SUBDIRS = dialog unitsets trrobj buildings icons sounds music
MAINTAINERCLEANFILES  = Makefile.in

EXTRA_DIST = $(srcdir)/* 

pkgdata_DATA 		= main.ascdat 
pkgdatadir 		= $(datadir)/games/@PACKAGE@

@maindatainclude@

main.ascdat: main.d
	cd $(srcdir) && @abs_top_builddir@/source/tools/linux/asc_mount \@files.txt @abs_builddir@/main.ascdat && touch main.d

clean-local:
	rm -f main.ascdat main.d main.mak
