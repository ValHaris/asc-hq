MAINTAINERCLEANFILES  = Makefile.in

pkgdata_DATA 		= pbp.ascdat 
pkgdatadir 		= $(datadir)/games/@PACKAGE@

@extradatainclude@

pbp.ascdat: pbp.d
	cd $(srcdir) && touch pbp.d && @abs_top_builddir@/source/tools/linux/asc_mount \@files_pbp.txt @abs_builddir@/pbp.ascdat 

clean-local:
	rm -f pbp.ascdat pbp.d 


dist-hook:
	cp -pr $(srcdir)/buildings $(srcdir)/unitsets $(srcdir)/icons $(srcdir)/trrobj $(srcdir)/campaign $(distdir)
	cp -p `find $(srcdir) -maxdepth 1 -type f` $(distdir)
	rm -f $(distdir)/main.ascdat
	