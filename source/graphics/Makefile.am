
MAINTAINERCLEANFILES	= Makefile.in

AM_CXXFLAGS		= @SDL_CFLAGS@ @SIGC_CFLAGS@ 

noinst_LTLIBRARIES 	= libgraphics.la

libgraphics_la_SOURCES	= surface.cpp blitter.cpp drawing.cpp surface2png.cpp
libgraphics_la_LIBADD		= @SDL_LIBS@ 

## INCLUDES = -I$(top_srcdir)/source/libs/paragui/include 

EXTRA_DIST=$(srcdir)/*.h  