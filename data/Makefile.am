SUBDIRS 		= dialog @MUSICDIR@

MAINTAINERCLEANFILES    = Makefile.in
# main.con buildings.con mk1.con trrobj.con trrobj2.con

pkgdata_DATA 		= main.con buildings.con mk1.con trrobj.con trrobj2.con asc.gfx

pkgdatadir 		= $(datadir)/games/@PACKAGE@

main.con: @MAINCONDATA@
	../source/tools/linux/asc_mount @MAINCONDATA@ main.con

clean-local:
	rm -f main.con
