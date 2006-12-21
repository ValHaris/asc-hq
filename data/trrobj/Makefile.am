MAINTAINERCLEANFILES    = Makefile.in

pkgdata_DATA 		= trrobj.con

pkgdatadir 		= $(datadir)/games/@PACKAGE@

trrobj.con: $(srcdir)/*.asctxt $(srcdir)/*.pcx $(srcdir)/*.version $(srcdir)/*.txt 
	cd $(srcdir) && @abs_top_builddir@/source/tools/linux/asc_mount *.asctxt *.pcx *.version *.txt @abs_builddir@/trrobj.con

EXTRA_DIST=$(srcdir)/*.asctxt $(srcdir)/*.pcx $(srcdir)/*.version $(srcdir)/*.txt 

clean-local:
	rm -f trrobj.con
