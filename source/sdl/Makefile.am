
MAINTAINERCLEANFILES	= Makefile.in

AM_CXXFLAGS = @SDL_CFLAGS@  @SIGC_CFLAGS@ @LokiInclude@
AM_CFLAGS   = @SDL_CFLAGS@  

INCLUDES      = -I$(src_dir) -I$(top_builddir) 

noinst_LTLIBRARIES	= libsdl.la libsdlsnd.la

libsdl_la_SOURCES 	= graphics.cpp events.cpp 
libsdl_la_LIBADD	= @SDL_LIBS@ -lSDL_image -ljpeg 

libsdlsnd_la_SOURCES 	= sound.cpp  
libsdlsnd_la_LIBADD	= @SDL_LIBS@ -lSDL_mixer @SDL_SOUND_LIB@ 

EXTRA_DIST=$(srcdir)/*.h $(srcdir)/sdlstretch.cpp $(srcdir)/sdl.vcproj  