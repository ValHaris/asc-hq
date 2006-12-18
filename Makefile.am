MAINTAINERCLEANFILES	= Makefile.in

SUBDIRS 		= source data doc installer
EXTRA_DIST 		= asc.kdevprj AUTHORS COPYING ChangeLog INSTALL README TODO asc.lsm
AUTOMAKE_OPTIONS 	= foreign

htmldoc:
	cd source && make htmldoc

installer: 
	cd $(top_srcdir)/installer && make installer

tarball:
	cd $(top_srcdir)/installer && make tarball

pbpinstaller: 
	cd $(top_srcdir)/installer && make pbpinstaller 
