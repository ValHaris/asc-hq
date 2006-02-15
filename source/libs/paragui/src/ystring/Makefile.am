noinst_LTLIBRARIES = libystring.la

libystring_la_SOURCES = \
	ychar.cpp \
	ystring.cpp

EXTRA_DIST =

INCLUDES = \
	$(SDL_CFLAGS) \
	-I$(top_srcdir)/include

style_cvs:
	astyle --style=kr --indent=tab=4 --indent-switches *.cpp

style_personal:
	astyle *.cpp
