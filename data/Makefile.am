SUBDIRS      = dialog  @MUSICDIR@
DIST_SUBDIRS = dialog music
# unitsets trrobj buildings icons sounds 
MAINTAINERCLEANFILES  = Makefile.in

EXTRA_DIST = $(srcdir)/* $(srcdir)/sounds/*

pkgdata_DATA 		= main.ascdat 
pkgdatadir 		= $(datadir)/games/@PACKAGE@

@maindatainclude@

# the sleep 1 is necessary because on some systems make thinks that main.d is newer than main.ascdat 
# although it has been created after that, but having the same mtime

main.ascdat: main.d
	cd $(srcdir) && @abs_top_builddir@/source/tools/linux/asc_mount \@files.txt @abs_builddir@/main.ascdat && sleep 1 && touch main.d

clean-local:
	rm -f main.ascdat main.d 


dist-hook:
	cp -pr $(srcdir)/buildings $(srcdir)/unitsets $(srcdir)/icons $(srcdir)/trrobj $(srcdir)/campaign $(distdir)
	cp -p `find $(srcdir) -maxdepth 1 -type f` $(distdir)
	rm -f $(distdir)/main.ascdat
	