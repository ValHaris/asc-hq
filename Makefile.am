MAINTAINERCLEANFILES	= Makefile.in

SUBDIRS 		= source data doc installer 
EXTRA_DIST 		= asc.kdevelop AUTHORS COPYING ChangeLog INSTALL README TODO asc.lsm bootstrap
AUTOMAKE_OPTIONS 	= foreign

.PHONY: htmldoc installer tarball pbpinstaller unitguide

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

dist-hook:
	if (find --help | grep -q wholename  ); then 
		rm -rf `find $(distdir) -name CVS -o -name "*.ascdat" -o -wholename "*music/*.ogg" -o -name asc2_dlg.zip -o -name "*~" -o -name .cvsignore`
	else
		rm -rf `find $(distdir) -name CVS -o -name "*.ascdat" -o -name asc2_dlg.zip -o -name "*~" -o -name .cvsignore`
	fi
	rm -rf `find $(distdir)/data/music -name "*.ogg" `