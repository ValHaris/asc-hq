
MAINTAINERCLEANFILES	= Makefile.in

AM_CXXFLAGS		= @SDL_CFLAGS@ 

noinst_LTLIBRARIES 	= libvideo.la

libvideo_la_SOURCES	= videorecorder.cpp
libvideo_la_LIBADD		= @SDL_LIBS@ @VIDEOCODER@ @revellibfile@ 

EXTRA_DIST=$(srcdir)/*.h  