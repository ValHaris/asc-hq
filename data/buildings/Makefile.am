

MAINTAINERCLEANFILES    = Makefile.in

EXTRA_DIST=$(srcdir)/*.asctxt $(srcdir)/*.pcx $(srcdir)/*.version $(srcdir)/*.txt   

pkgdata_DATA 		= buildings.con
pkgdatadir 		= $(datadir)/games/@PACKAGE@


buildings.con: $(srcdir)/*.asctxt $(srcdir)/*.pcx $(srcdir)/*.version $(srcdir)/*.txt 
	cd $(srcdir) && @abs_top_builddir@/source/tools/linux/asc_mount *.asctxt *.pcx *.version *.txt @abs_builddir@/buildings.con

clean-local:
	rm -f buildings.con
