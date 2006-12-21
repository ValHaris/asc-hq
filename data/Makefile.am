SUBDIRS 		= dialog units trrobj trrobj2 buildings @MUSICDIR@
DIST_SUBDIRS= dialog units trrobj trrobj2 buildings music
MAINTAINERCLEANFILES    = Makefile.in

EXTRA_DIST = $(srcdir)/* $(srcdir)/icons/* $(srcdir)/sounds/*

pkgdata_DATA 		= main.con 

pkgdatadir 		= $(datadir)/games/@PACKAGE@

main.con: $(srcdir)/*.* $(srcdir)/icons/*.png $(srcdir)/icons/*.ascgui $(srcdir)/sounds/*.wav $(srcdir)/sounds/*.ogg $(srcdir)/sounds/*.asctxt
	cd $(srcdir) && @abs_top_builddir@/source/tools/linux/asc_mount @MAINCONDATA@ icons/*.png icons/*.ascgui sounds/*.wav sounds/*.ogg sounds/*.asctxt @abs_builddir@/main.con

clean-local:
	rm -f main.con
