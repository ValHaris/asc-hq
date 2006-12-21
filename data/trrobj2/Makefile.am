MAINTAINERCLEANFILES    = Makefile.in

pkgdata_DATA 		= trrobj2.con

pkgdatadir 		= $(datadir)/games/@PACKAGE@
EXTRA_DIST=$(srcdir)/*.asctxt $(srcdir)/*.pcx $(srcdir)/*.version $(srcdir)/*.png 

trrobj2.con: $(srcdir)/*.asctxt $(srcdir)/*.pcx $(srcdir)/*.png $(srcdir)/*.version 
	cd $(srcdir) && @abs_top_builddir@/source/tools/linux/asc_mount *.asctxt *.pcx *.version *.png @abs_builddir@/trrobj2.con

clean-local:
	rm -f trrobj2.con
