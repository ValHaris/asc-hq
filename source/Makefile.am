MAINTAINERCLEANFILES	= Makefile.in

SUBDIRS 		= @GENCL@ docs libs sdl ai graphics dialogs widgets actions unix tools doc win32

EXTRA_DIST  = $(srcdir)/*.cpp $(srcdir)/*.h $(srcdir)/doxygen.config $(srcdir)/clparser/*.cpp $(srcdir)/clparser/*.h $(srcdir)/clparser/Makefile.am $(srcdir)/clparser/Makefile.in

htmldoc: doxygen.config
	doxygen doxygen.config
	cd libs/paragui ; make paraguidoc
_SOURCES = memory-measurement.h
_SOURCES = memsize_interface.h
