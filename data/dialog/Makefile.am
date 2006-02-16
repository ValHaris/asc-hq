pkgdatadir 		= $(datadir)/games/@PACKAGE@

pkgdata_DATA = asc2_dlg.zip

asc2_dlg.zip: asc2_dlg.theme
	chmod -R g-w asc*
	chmod -R o-w asc*
	(zip -9 -r asc2_dlg.zip asc_dlg.theme asc2_dlg.theme ./asc/* -i *.png *.bmp *.ttf *.theme)
