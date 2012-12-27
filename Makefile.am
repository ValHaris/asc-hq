MAINTAINERCLEANFILES	= Makefile.in

SUBDIRS 		= source data doc installer unittester @extradatadir@
EXTRA_DIST 		= asc.kdevelop AUTHORS COPYING ChangeLog INSTALL README TODO asc.lsm bootstrap asc.suse.spec mac_configure
AUTOMAKE_OPTIONS 	= foreign

.PHONY: htmldoc installer tarball pbpinstaller unitguide cvstag SWIG

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
	
SWIG: 
	cd source/lua && make SWIG	

thoroughcheck:
	cd unittester && ./unittester --valgrind

hgtag:
	hg tag ASC`scripts/getversion source/strtmesg.cpp | sed s/\\\\./-/g`

hgpbptag:
	hg tag -f PBP ASC`scripts/getversion source/strtmesg.cpp | sed s/\\\\./-/g`

dist-hook:
	rm -rf `find $(distdir) -name CVS -o -name "*.ascdat" -o -name asc2_dlg.zip -o -name "*~" -o -name .cvsignore -o -name ".#*"`
	rm -rf `find $(distdir)/data/music -name "*.ogg" `
	chmod ugo-x doc/manpages/*.6 doc/unitguide/output/*.gif doc/unitguide/output/*.html doc/unitguide/output/*.txt
	