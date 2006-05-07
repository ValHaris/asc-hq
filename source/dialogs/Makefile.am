
MAINTAINERCLEANFILES    = Makefile.in

AM_CXXFLAGS             = $(SDL_CFLAGS) $(SIGC_CFLAGS) -Dmaindialogs

noinst_LTLIBRARIES      = libdialogs.la libgamedialogs.la libmapeddialogs.la

libdialogs_la_SOURCES   = editmapparam.cpp cargowidget.cpp selectionwindow.cpp fileselector.cpp playersetup.cpp alliancesetup.cpp newgame.cpp soundsettings.cpp emailsetup.cpp unitinfodialog.cpp  editgameoptions.cpp vehicletypeselector.cpp fieldmarker.cpp admingame.cpp
libdialogs_la_LIBADD    = @SDL_LIBS@ 

libgamedialogs_la_SOURCES   = cargodialog.cpp unitcounting.cpp cargowidget.cpp ammotransferdialog.cpp attackpanel.cpp vehicletypeselector.cpp nextcampaignmap.cpp
libgamedialogs_la_LIBADD    = @SDL_LIBS@ 

libmapeddialogs_la_SOURCES   = cargoeditor.cpp cargowidget.cpp eventeditor.cpp newmap.cpp
libmapeddialogs_la_LIBADD    = @SDL_LIBS@
