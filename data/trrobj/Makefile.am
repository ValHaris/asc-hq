MAINTAINERCLEANFILES    = Makefile.in

pkgdata_DATA 		= trrobj.con

pkgdatadir 		= $(datadir)/games/@PACKAGE@

trrobj.con: *.asctxt *.pcx *.version *.txt
	../../source/tools/linux/asc_mount *.asctxt *.pcx *.version *.txt trrobj.con

clean-local:
	rm -f trrobj.con
