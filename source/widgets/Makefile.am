
MAINTAINERCLEANFILES    = Makefile.in

AM_CXXFLAGS             = $(SDL_CFLAGS) $(SIGC_CFLAGS)  @LokiInclude@

noinst_LTLIBRARIES      = libwidgets.la 

libwidgets_la_SOURCES   = textrenderer.cpp bargraphwidget.cpp dropdownselector.cpp  multilistbox.cpp autoprogressbar.cpp playerselector.cpp \
                          targetcoordinatelocator.cpp textrenderer-addons.cpp vehicletypeimage.cpp
libwidgets_la_LIBADD    = @SDL_LIBS@

AM_CPPFLAGS = -I$(top_srcdir)/source/libs/paragui/include 

EXTRA_DIST=$(srcdir)/*.h
