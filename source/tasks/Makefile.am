
MAINTAINERCLEANFILES	= Makefile.in

AM_CXXFLAGS          = $(SDL_CFLAGS) $(SIGC_CFLAGS)  @LokiInclude@ 

noinst_LTLIBRARIES 	= libtasks.la libtaskcore.la

libtaskcore_la_SOURCES	= task.cpp taskcontainer.cpp 
libtaskcore_la_LIBADD	= @SDL_LIBS@

libtasks_la_SOURCES	= unittask.cpp task_generator.cpp moveunittask.cpp 
libtasks_la_LIBADD	= @SDL_LIBS@

EXTRA_DIST=$(srcdir)/*.h
