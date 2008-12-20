
MAINTAINERCLEANFILES	= Makefile.in

AM_CXXFLAGS          = $(SDL_CFLAGS) $(SIGC_CFLAGS)  @LokiInclude@ 

noinst_LTLIBRARIES 	= libtasks.la

libtasks_la_SOURCES	= task.cpp unittask.cpp task_generator.cpp 
libtasks_la_LIBADD		= @SDL_LIBS@

EXTRA_DIST=$(srcdir)/*.h
