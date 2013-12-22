SUBDIRS = draw font themes core widgets ystring

noinst_LTLIBRARIES = libparagui.la

libparagui_la_SOURCES = paragui.cpp

libparagui_la_LIBADD = \
	$(SIGC_LIBS) \
	$(SDL_LIBS) \
	$(EXPAT_LIB) \
	$(PHYSFS_LIB) \
	./draw/libpgdraw.la \
	./font/libpgfont.la \
	./core/libpgcore.la \
	./widgets/libpgwidgets.la \
	./themes/libtheme.la \
	./ystring/libystring.la \
	$(LIBSTDCPP)

AM_CPPFLAGS = \
	$(SIGC_CFLAGS) \
	$(SDL_CFLAGS) \
	-I$(top_srcdir)/source/libs/paragui/include 
