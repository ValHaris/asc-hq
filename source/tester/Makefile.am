
.y.cpp:
	bisonc++ --thread-safe $<

.lpp.cpp:
	flex -i --header-file=scanner.h -o $@ $<

MAINTAINERCLEANFILES	= Makefile.in

AM_CXXFLAGS             = $(SDL_CFLAGS) $(SIGC_CFLAGS)  @LokiInclude@ 

noinst_LTLIBRARIES 	= libtester.la 

libtester_la_SOURCES	= scanner.cpp parser.cpp scannerwrapper.cpp

noinst_PROGRAMS = testparser

testparser_SOURCES = testmain.cpp
testparser_LDADD = libtester.la 

EXTRA_DIST=$(srcdir)/*.h $(srcdir)/parser.cpp $(srcdir)/scanner.cpp
