
MAINTAINERCLEANFILES	= Makefile.in

AM_CXXFLAGS             = $(SDL_CFLAGS) $(SIGC_CFLAGS)  @LokiInclude@ -I$(top_srcdir)/source -I$(top_srcdir)/source/libs/paragui/include

noinst_LTLIBRARIES 	= libunittests.la

libunittests_la_SOURCES	= movementtest.cpp attacktest.cpp unittestutil.cpp main.cpp ai-move1.cpp viewtest.cpp ai-service1.cpp transfercontroltest.cpp \
                          recyclingtest.cpp diplomacytest.cpp researchtest.cpp objectconstructiontest.cpp actiontest.cpp eventtest.cpp jumptest.cpp \
                          testversionidentifier.cpp streamencoding.cpp maptest.cpp repairtest.cpp
                          
libunittests_la_LIBADD	= @SDL_LIBS@


EXTRA_DIST=$(srcdir)/*.h
