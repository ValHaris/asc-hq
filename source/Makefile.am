MAINTAINERCLEANFILES	= Makefile.in

SUBDIRS 		= @GENCL@ docs libs sdl ai graphics dialogs widgets actions unix tools 

EXTRA_DIST 		= *.cpp *.h *.inc

htmldoc: doxygen.config
	doxygen doxygen.config
	cd libs/paragui ; make paraguidoc
