
MAINTAINERCLEANFILES	= Makefile.in

AM_CXXFLAGS = @SDL_CFLAGS@  @SIGC_CFLAGS@
AM_CFLAGS   = @SDL_CFLAGS@ 

noinst_LTLIBRARIES	= libsdl.la

libsdl_la_SOURCES 	= sound.cpp graphics.cpp events.cpp SDLStretch.cpp \
loadimage.cpp asc_IMG_jpg.c
libsdl_la_LIBADD	= @SDL_LIBS@ -lSDL_image -lSDL_mixer -ljpeg



