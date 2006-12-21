
MAINTAINERCLEANFILES    = Makefile.in

AM_CXXFLAGS             = $(SDL_CFLAGS) $(SIGC_CFLAGS) 

noinst_LTLIBRARIES      = libwidgets.la 

libwidgets_la_SOURCES   = textrenderer.cpp bargraphwidget.cpp dropdownselector.cpp  multilistbox.cpp autoprogressbar.cpp playerselector.cpp targetcoordinatelocator.cpp 
libwidgets_la_LIBADD    = @SDL_LIBS@

INCLUDES = -I$(top_srcdir)/source/libs/paragui/include 

EXTRA_DIST=$(srcdir)/*.h
