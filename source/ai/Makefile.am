
MAINTAINERCLEANFILES	= Makefile.in

AM_CXXFLAGS		= @SDL_CFLAGS@ @SDLMM_CXXFLAGS@

noinst_LTLIBRARIES 	= libai.la

libai_la_SOURCES	= base.cpp misc.cpp strategy.cpp valuation.cpp \
buildingcapture.cpp service.cpp tactics.cpp
libai_la_LIBADD		= @SDL_LIBS@ @SDLMM_LIBS@

