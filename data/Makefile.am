SUBDIRS 		= dialog units trrobj trrobj2 buildings @MUSICDIR@

MAINTAINERCLEANFILES    = Makefile.in
# main.con buildings.con mk1.con trrobj.con trrobj2.con

pkgdata_DATA 		= main.con 

pkgdatadir 		= $(datadir)/games/@PACKAGE@

main.con: @MAINCONDATA@ icons/*.png icons/*.ascgui sounds/*.wav sounds/*.ogg sounds/*.asctxt
	../source/tools/linux/asc_mount @MAINCONDATA@ icons/*.png icons/*.ascgui sounds/*.wav sounds/*.ogg sounds/*.asctxt main.con

clean-local:
	rm -f main.con
