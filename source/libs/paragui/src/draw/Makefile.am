noinst_LTLIBRARIES = libpgdraw.la

libpgdraw_la_SOURCES = \
	surface.cpp \
	rotozoom.cpp \
	drawline.cpp \
	setpixel.cpp \
	stretch.cpp \
	pgpoint.cpp \
	pgrect.cpp \
	drawtile.cpp \
	gradient.cpp

EXTRA_DIST =

AM_CPPFLAGS = \
	$(SDL_CFLAGS) \
	$(SIGC_CFLAGS) \
	-I$(top_srcdir)/source/libs/paragui/include

style_cvs:
	astyle --style=kr --indent=tab=4 --indent-switches *.cpp

style_personal:
	astyle *.cpp
