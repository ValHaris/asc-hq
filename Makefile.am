MAINTAINERCLEANFILES	= Makefile.in

SUBDIRS 		= source data doc
EXTRA_DIST 		= asc.kdevprj AUTHORS COPYING ChangeLog INSTALL README TODO asc.lsm
AUTOMAKE_OPTIONS 	= foreign

htmldoc:
	cd source ; make htmldoc

installer: 
	cd $(top_srcdir)/installer && makensis pbp.nsi

pbpinstaller: 
	cd $(top_srcdir)/installer && makensis pbp.nsi
