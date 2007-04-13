

INCLUDE = -I$(top_src_dir)

noinst_LTLIBRARIES	= libSDL_sound_int.la

AM_CFLAGS   = -DSOUND_SUPPORTS_OGG  $(SDL_CFLAGS) $(VORBIS_CFLAGS)

libSDL_sound_int_la_SOURCES = ogg.c SDL_sound.c audio_convert.c
libSDL_sound_int_la_LIBADD = @VORBIS_LIBS@ @VORBISFILE_LIBS@ @VORBISENC_LIBS@