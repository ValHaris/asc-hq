pkgdatadir 		= $(datadir)/games/@PACKAGE@

pkgdata_DATA = asc_dlg.zip

asc_dlg.zip: asc_dlg.theme asc_dlg2.theme
	chmod -R g-w asc*
	chmod -R o-w asc*
	(zip -9 -r asc_dlg.zip asc_dlg.theme asc_dlg2.theme ./asc/* -i *.png *.bmp *.ttf *.theme)

