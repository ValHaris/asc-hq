MAINTAINERCLEANFILES    = Makefile.in

pkgdata_DATA               = main.con

pkgdatadir = $(datadir)/games/@PACKAGE@

main.con: @MAINCONDATA@
	../source/tools/linux/asc_mount @MAINCONDATA@ main.con

clean-local:
	rm main.con
