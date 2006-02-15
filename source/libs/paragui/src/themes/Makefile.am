noinst_LTLIBRARIES = libtheme.la

libtheme_la_SOURCES = \
	themeloader.cpp \
	theme_priv.cpp

INCLUDES = \
	$(SIGC_CFLAGS) \
	$(EXPAT_INCLUDE) \
	$(SDL_CFLAGS) \
	-I$(top_srcdir)/include

EXTRA_DIST = \
	theme_priv.h

style_cvs:
	astyle --style=kr --indent=tab=4 --indent-switches *.cpp
	astyle --style=kr --indent=tab=4 --indent-switches *.h

style_personal:
	astyle *.cpp
	astyle *.h
