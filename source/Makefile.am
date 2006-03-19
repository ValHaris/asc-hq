MAINTAINERCLEANFILES	= Makefile.in

SUBDIRS 		= @GENCL@ docs sdl libs ai graphics dialogs actions unix tools 

EXTRA_DIST 		= *.cpp *.h *.inc

htmldoc: doxygen.config
	doxygen doxygen.config
	cd libs/paragui
	make htmldoc
_SOURCES = mapimageexport.cpp
