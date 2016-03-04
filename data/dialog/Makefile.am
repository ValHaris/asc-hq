pkgdatadir 		= $(datadir)/games/@PACKAGE@

pkgdata_DATA = asc2_dlg.zip

asc2_dlg.zip: $(srcdir)/asc2_dlg.theme $(srcdir)/asc/*.bmp $(srcdir)/asc/*.ttf $(srcdir)/asc/*.png
	cd $(srcdir) && zip -X -9 -r @abs_builddir@/asc2_dlg.zip asc2_dlg.theme ./asc/* -i *.png *.bmp *.ttf *.theme

CLEANFILES = asc2_dlg.zip 

EXTRA_DIST=$(srcdir)/asc2_dlg.theme $(srcdir)/asc/*

