# Configure paths for SDL
# Sam Lantinga 9/21/99
# stolen from Manish Singh
# stolen back from Frank Belew
# stolen from Manish Singh
# Shamelessly stolen from Owen Taylor

dnl AM_PATH_SDL([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for SDL, and define SDL_CFLAGS and SDL_LIBS
dnl
AC_DEFUN(AM_PATH_SDL,
[dnl 
dnl Get the cflags and libraries from the sdl-config script
dnl
AC_ARG_WITH(sdl-prefix,[  --with-sdl-prefix=PFX   Prefix where SDL is installed (optional)],
            sdl_prefix="$withval", sdl_prefix="")
AC_ARG_WITH(sdl-exec-prefix,[  --with-sdl-exec-prefix=PFX Exec prefix where SDL is installed (optional)],
            sdl_exec_prefix="$withval", sdl_exec_prefix="")
AC_ARG_ENABLE(sdltest, [  --disable-sdltest       Do not try to compile and run a test SDL program],
		    , enable_sdltest=yes)

  if test x$sdl_exec_prefix != x ; then
     sdl_args="$sdl_args --exec-prefix=$sdl_exec_prefix"
     if test x${SDL_CONFIG+set} != xset ; then
        SDL_CONFIG=$sdl_exec_prefix/bin/sdl-config
     fi
  fi
  if test x$sdl_prefix != x ; then
     sdl_args="$sdl_args --prefix=$sdl_prefix"
     if test x${SDL_CONFIG+set} != xset ; then
        SDL_CONFIG=$sdl_prefix/bin/sdl-config
     fi
  fi

  AC_PATH_PROG(SDL_CONFIG, sdl-config, no)
  min_sdl_version=ifelse([$1], ,0.11.0,$1)
  AC_MSG_CHECKING(for SDL - version >= $min_sdl_version)
  no_sdl=""
  if test "$SDL_CONFIG" = "no" ; then
    no_sdl=yes
  else
    SDL_CFLAGS=`$SDL_CONFIG $sdlconf_args --cflags`
    SDL_LIBS=`$SDL_CONFIG $sdlconf_args --libs`

    sdl_major_version=`$SDL_CONFIG $sdl_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    sdl_minor_version=`$SDL_CONFIG $sdl_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    sdl_micro_version=`$SDL_CONFIG $sdl_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x$enable_sdltest" = "xyes" ; then
      ac_save_CFLAGS="$CFLAGS"
      ac_save_LIBS="$LIBS"
      CFLAGS="$CFLAGS $SDL_CFLAGS"
      LIBS="$LIBS $SDL_LIBS"
dnl
dnl Now check if the installed SDL is sufficiently new. (Also sanity
dnl checks the results of sdl-config to some extent
dnl
      rm -f conf.sdltest
      AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL.h"

char*
my_strdup (char *str)
{
  char *new_str;
  
  if (str)
    {
      new_str = (char *)malloc ((strlen (str) + 1) * sizeof(char));
      strcpy (new_str, str);
    }
  else
    new_str = NULL;
  
  return new_str;
}

int main (int argc, char *argv[])
{
  int major, minor, micro;
  char *tmp_version;

  /* This hangs on some systems (?)
  system ("touch conf.sdltest");
  */
  { FILE *fp = fopen("conf.sdltest", "a"); if ( fp ) fclose(fp); }

  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = my_strdup("$min_sdl_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string\n", "$min_sdl_version");
     exit(1);
   }

   if (($sdl_major_version > major) ||
      (($sdl_major_version == major) && ($sdl_minor_version > minor)) ||
      (($sdl_major_version == major) && ($sdl_minor_version == minor) && ($sdl_micro_version >= micro)))
    {
      return 0;
    }
  else
    {
      printf("\n*** 'sdl-config --version' returned %d.%d.%d, but the minimum version\n", $sdl_major_version, $sdl_minor_version, $sdl_micro_version);
      printf("*** of SDL required is %d.%d.%d. If sdl-config is correct, then it is\n", major, minor, micro);
      printf("*** best to upgrade to the required version.\n");
      printf("*** If sdl-config was wrong, set the environment variable SDL_CONFIG\n");
      printf("*** to point to the correct copy of sdl-config, and remove the file\n");
      printf("*** config.cache before re-running configure\n");
      return 1;
    }
}

],, no_sdl=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi
  if test "x$no_sdl" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$SDL_CONFIG" = "no" ; then
       echo "*** The sdl-config script installed by SDL could not be found"
       echo "*** If SDL was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the SDL_CONFIG environment variable to the"
       echo "*** full path to sdl-config."
     else
       if test -f conf.sdltest ; then
        :
       else
          echo "*** Could not run SDL test program, checking why..."
          CFLAGS="$CFLAGS $SDL_CFLAGS"
          LIBS="$LIBS $SDL_LIBS"
          AC_TRY_LINK([
#include <stdio.h>
#include "SDL.h"
],      [ return 0; ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding SDL or finding the wrong"
          echo "*** version of SDL. If it is not finding SDL, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means SDL was incorrectly installed"
          echo "*** or that you have moved SDL since it was installed. In the latter case, you"
          echo "*** may want to edit the sdl-config script: $SDL_CONFIG" ])
          CFLAGS="$ac_save_CFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     SDL_CFLAGS=""
     SDL_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(SDL_CFLAGS)
  AC_SUBST(SDL_LIBS)
  rm -f conf.sdltest
])


dnl Embedding the prefix dir into the ASC source
AC_DEFUN(MY_EXPAND_DIR, [
                $1=$2
                $1=`(
                     test "x$prefix" = xNONE && prefix="$ac_default_prefix"
                     test "x$exec_prefix" = xNONE && exec_prefix="${prefix}"
                     eval echo \""[$]$1"\"
                    )`
              ])

dnl ##################################################################
dnl We cannot use the AC_CHECK_TYPE macros because AC_CHECK_TYPE
dnl #includes only <sys/types.h>, <stdlib.h>, and <stddef.h>.
dnl Unfortunately, many implementations today hide typedefs in wierd
dnl locations: Solaris 2.5.1 has uint8_t and uint32_t in <pthread.h>.
dnl SunOS 4.1.x has int8_t in <sys/bitypes.h>.
dnl So we define our own macro AC_UNP_CHECK_TYPE that does the same
dnl #includes as "unp.h", and then looks for the typedef.
dnl
dnl This macro should be invoked after all the header checks have been
dnl performed, since we #include "confdefs.h" below, and then use the
dnl HAVE_foo_H values that it can #define.
dnl
AC_DEFUN(AC_UNP_CHECK_TYPE,
        [AC_MSG_CHECKING(if $1 defined)
        AC_CACHE_VAL(ac_cv_type_$1,
                AC_TRY_COMPILE(
[
#include        "confdefs.h"    /* the header built by configure so far */
#ifdef  HAVE_SYS_TYPES_H
#  include      <sys/types.h>
#endif
#ifdef  HAVE_SYS_SOCKET_H
#  include      <sys/socket.h>
#endif
#ifdef  HAVE_SYS_TIME_H
#  include    <sys/time.h>
#endif
#ifdef  HAVE_NETINET_IN_H
#  include    <netinet/in.h>
#endif
#ifdef  HAVE_ARPA_INET_H
#  include    <arpa/inet.h>
#endif
#ifdef  HAVE_ERRNO_H
#  include    <errno.h>
#endif
#ifdef  HAVE_FCNTL_H
#  include    <fcntl.h>
#endif
#ifdef  HAVE_NETDB_H
#  include      <netdb.h>
#endif
#ifdef  HAVE_SIGNAL_H
#  include      <signal.h>
#endif
#ifdef  HAVE_STDIO_H
#  include      <stdio.h>
#endif
#ifdef  HAVE_STDLIB_H
#  include      <stdlib.h>
#endif
#ifdef  HAVE_STRING_H
#  include      <string.h>
#endif
#ifdef  HAVE_SYS_STAT_H
#  include      <sys/stat.h>
#endif
#ifdef  HAVE_SYS_UIO_H
#  include      <sys/uio.h>
#endif
#ifdef  HAVE_UNISTD_H
#  include      <unistd.h>
#endif
#ifdef  HAVE_SYS_WAIT_H
#  include      <sys/wait.h>
#endif
#ifdef  HAVE_SYS_UN_H
#  include      <sys/un.h>
#endif
#ifdef  HAVE_SYS_SELECT_H
# include   <sys/select.h>
#endif
#ifdef  HAVE_STRINGS_H
# include   <strings.h>
#endif
#ifdef  HAVE_SYS_IOCTL_H
# include   <sys/ioctl.h>
#endif
#ifdef  HAVE_SYS_FILIO_H
# include   <sys/filio.h>
#endif
#ifdef  HAVE_SYS_SOCKIO_H
# include   <sys/sockio.h>
#endif
#ifdef  HAVE_PTHREAD_H
#  include      <pthread.h>
#endif],
                [ $1 foo ],
                ac_cv_type_$1=yes,
                ac_cv_type_$1=no))
        AC_MSG_RESULT($ac_cv_type_$1)
        if test $ac_cv_type_$1 = no ; then
                AC_DEFINE($1, $2)
        fi
])
