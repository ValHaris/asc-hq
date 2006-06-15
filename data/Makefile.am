SUBDIRS 		= dialog @MUSICDIR@

MAINTAINERCLEANFILES    = Makefile.in
# main.con buildings.con mk1.con trrobj.con trrobj2.con

pkgdata_DATA 		= main.con buildings.con mk1.con trrobj.con trrobj2.con asc.gfx units-mk3.con

pkgdatadir 		= $(datadir)/games/@PACKAGE@

main.con: @MAINCONDATA@
	../source/tools/linux/asc_mount @MAINCONDATA@ icons/*.png icons/*.ascgui main.con

clean-local:
	rm -f main.con
