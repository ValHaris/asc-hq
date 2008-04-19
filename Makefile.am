MAINTAINERCLEANFILES	= Makefile.in

SUBDIRS 		= source data doc installer 
EXTRA_DIST 		= asc.kdevelop AUTHORS COPYING ChangeLog INSTALL README TODO asc.lsm bootstrap
AUTOMAKE_OPTIONS 	= foreign

.PHONY: htmldoc installer tarball pbpinstaller unitguide cvstag

htmldoc:
	cd source && make htmldoc

installer: 
	cd installer && make installer

tarball:
	cd installer && make tarball

pbpinstaller: 
	cd installer && make pbpinstaller 

unitguide:
	cd doc && make unitguide

cvstag:
	cvs tag ASC`scripts/getversion source/strtmesg.cpp | sed s/\\\\./-/g`

cvspbptag:
	scripts/pbptag

dist-hook:
	rm -rf `find $(distdir) -name CVS -o -name "*.ascdat" -o -name asc2_dlg.zip -o -name "*~" -o -name .cvsignore -o -name ".#*"`
	rm -rf `find $(distdir)/data/music -name "*.ogg" `