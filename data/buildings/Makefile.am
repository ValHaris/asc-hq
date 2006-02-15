MAINTAINERCLEANFILES    = Makefile.in

pkgdata_DATA 		= buildings.con

pkgdatadir 		= $(datadir)/games/@PACKAGE@

buildings.con: *.asctxt *.pcx *.version *.txt
	../../source/tools/linux/asc_mount *.asctxt *.pcx *.version *.txt buildings.con

clean-local:
	rm -f buildings.con
