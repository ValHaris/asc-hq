
MAINTAINERCLEANFILES    = Makefile.in

AM_CXXFLAGS 		= $(SDL_CFLAGS) $(PARAGUI_CFLAGS)  $(SIGC_CFLAGS)

noinst_LTLIBRARIES      = libdialogs.la

libdialogs_la_SOURCES  = editmapparam.cpp selectionwindow.cpp fileselector.cpp playersetup.cpp alliancesetup.cpp newgame.cpp soundsettings.cpp
libdialogs_la_LIBADD   = @SDL_LIBS@ 

