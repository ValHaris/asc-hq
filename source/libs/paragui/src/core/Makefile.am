noinst_LTLIBRARIES = libpgcore.la

libpgcore_la_SOURCES = \
	physfsrwops.cpp \
	missing.cpp \
	pgdatacontainer.cpp \
	pgapplication.cpp \
	pgcolors.cpp \
	pgfilearchive.cpp \
	pgfile.cpp \
	pglog.cpp \
	pgmain.cpp \
	pgmessageobject.cpp \
	pgnavigator.cpp \
	pgrectlist.cpp \
	pgsurfacecache.cpp \
	pgtimerobject.cpp \
	pgsigconvert.cpp \
	pgcolor.cpp \
	pgsdleventsupplier.cpp \
	pgsdlscreenupdater.cpp 

libpgcore_la_LIBADD = \
	$(SDL_LIBS) \
	$(SIGC_LIBS)

EXTRA_DIST = \
	physfsrwops.h

INCLUDES = \
	$(SIGC_CFLAGS) \
	$(SDL_CFLAGS) \
	$(PHYSFS_INCLUDE) \
	-I$(top_srcdir)/source/libs/paragui/include

style_cvs:
	astyle --style=kr --indent=tab=4 --indent-switches *.cpp

style_personal:
	astyle *.cpp
