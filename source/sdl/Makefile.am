
MAINTAINERCLEANFILES	= Makefile.in

AM_CXXFLAGS		= @SDL_CFLAGS@ @SDLMM_CXXFLAGS@

noinst_LTLIBRARIES	= libsdl.la

libsdl_la_SOURCES 	= sound.cpp graphics.cpp events.cpp SDLStretch.cpp \
loadimage.cpp asc_IMG_jpg.c
libsdl_la_LIBADD	= @SDL_LIBS@ @SDLMM_LIBS@ -lSDL_image -lSDL_mixer \
-ljpeg



