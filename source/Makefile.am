MAINTAINERCLEANFILES	= Makefile.in

SUBDIRS 		= @GENCL@ docs libs sdl util ai graphics network dialogs widgets actions tasks lua video unix tools doc win32 

EXTRA_DIST  = $(srcdir)/*.cpp $(srcdir)/*.h $(srcdir)/doxygen.config $(srcdir)/clparser/*.cpp $(srcdir)/clparser/*.h $(srcdir)/clparser/Makefile.am $(srcdir)/clparser/Makefile.in

htmldoc: doxygen.config
	doxygen doxygen.config
	cd libs/paragui ; make paraguidoc
