EXTRA_DIST = $(srcdir)/*.h $(srcdir)/README.TXT

INCLUDE = -I$(top_src_dir) -I$(top_builddir)

noinst_LTLIBRARIES	= libSDL_sound_int.la

AM_CFLAGS   = -DSOUND_SUPPORTS_OGG -DSOUND_SUPPORTS_WAV $(SDL_CFLAGS) $(VORBIS_CFLAGS)

libSDL_sound_int_la_SOURCES = ogg.c wav.c SDL_sound.c audio_convert.c
libSDL_sound_int_la_LIBADD = @VORBIS_LIBS@ @VORBISFILE_LIBS@ @VORBISENC_LIBS@