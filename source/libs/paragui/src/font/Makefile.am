noinst_LTLIBRARIES = libpgfont.la

libpgfont_la_SOURCES = \
	pgfont_impl.cpp \
	pgfont.cpp

EXTRA_DIST = \
	facecache.h

INCLUDES = \
	$(SIGC_CFLAGS) \
	$(SDL_CFLAGS) \
	-I$(top_srcdir)/include

style_cvs:
	astyle --style=kr --indent=tab=4 --indent-switches *.cpp

style_personal:
	astyle *.cpp
