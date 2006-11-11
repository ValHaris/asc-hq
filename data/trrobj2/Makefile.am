MAINTAINERCLEANFILES    = Makefile.in

pkgdata_DATA 		= trrobj2.con

pkgdatadir 		= $(datadir)/games/@PACKAGE@

trrobj2.con: *.asctxt *.pcx *.png *.version 
	../../source/tools/linux/asc_mount *.asctxt *.pcx *.version *.png trrobj2.con

clean-local:
	rm -f trrobj2.con
