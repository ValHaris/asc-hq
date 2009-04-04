MAINTAINERCLEANFILES	= Makefile.in

SUBDIRS 		= @GENCL@ docs libs sdl util ai graphics network dialogs widgets actions tasks @LUA_MAKEDIR@ video unittests unix tools doc win32 

EXTRA_DIST  = $(srcdir)/*.cpp $(srcdir)/*.h $(srcdir)/doxygen.config $(srcdir)/clparser/*.cpp $(srcdir)/clparser/*.h $(srcdir)/clparser/Makefile.am $(srcdir)/clparser/Makefile.in

DIST_SUBDIRS = lua @GENCL@ docs libs sdl util ai graphics network dialogs widgets actions tasks video unittests unix tools doc win32 

htmldoc: doxygen.config
	doxygen doxygen.config
	cd libs/paragui ; make paraguidoc
