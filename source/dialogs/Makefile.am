
MAINTAINERCLEANFILES    = Makefile.in

AM_CXXFLAGS             = $(SDL_CFLAGS) $(PARAGUI_CFLAGS)  $(SIGC_CFLAGS) -Dmaindialogs

noinst_LTLIBRARIES      = libdialogs.la libgamedialogs.la

libdialogs_la_SOURCES   = editmapparam.cpp selectionwindow.cpp fileselector.cpp playersetup.cpp alliancesetup.cpp newgame.cpp soundsettings.cpp emailsetup.cpp unitinfodialog.cpp 
libdialogs_la_LIBADD    = @SDL_LIBS@ 

libgamedialogs_la_SOURCES   = cargodialog.cpp vehicletypeselector.cpp unitcounting.cpp
libgamedialogs_la_LIBADD    = @SDL_LIBS@ 
