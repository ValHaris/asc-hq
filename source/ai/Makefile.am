
MAINTAINERCLEANFILES	= Makefile.in

AM_CXXFLAGS		= @SDL_CFLAGS@ @SIGC_CFLAGS@ @PARAGUI_CFLAGS@ -Dsgmain

noinst_LTLIBRARIES 	= libai.la

libai_la_SOURCES	= base.cpp misc.cpp strategy.cpp valuation.cpp \
buildingcapture.cpp service.cpp tactics.cpp
libai_la_LIBADD		= @SDL_LIBS@ 

