#ifndef MACOS_CONFIG_INCLUDED
#define MACOS_CONFIG_INCLUDED

/* Define to 1 if UNICODE support is enabled */
/* #undef UNICODE_ENABLED */

/* Define if you the the SDL_image library (-lSDL_Image) */
#define HAVE_SDLIMAGE 1

/* Define the default path to the theme files */
#define PARAGUI_THEMEDIR ":data:"

/* Define if RTTI is enabled in your compiler (for dynamic_cast and typeid) */
#define RTTI_ENABLED 1

/* Define if the exception handling is enabled in your compiler */
#undef EXCEPTIONS_ENABLED

/* Define if mpatrol memory profiler support is enabled */
/* #undef MPATROL_ENABLED */

/* Define if you have the fnmatch function.  */
/* #undef HAVE_FNMATCH */

/* Define if you have the strdup function.  */
/* #undef HAVE_STRDUP */

/* Define if you have the strtok function.  */
#define HAVE_STRTOK 1

/* Define if you have the vsnprintf function.  */
#define HAVE_VSNPRINTF 1

/* Define if you have the <dlfcn.h> header file.  */
#define HAVE_DLFCN_H 1

/* Define if you have the <ext/hash_map> header file.  */
/* #undef HAVE_EXT_HASH_MAP */

/* Define if you have the <hash_map> header file.  */
/*#define HAVE_HASH_MAP 1*/

/* Define if you have the jpeg library (-ljpeg).  */
#define HAVE_LIBJPEG 1

/* Define if you have the png library (-lpng).  */
#define HAVE_LIBPNG 1

/* Define if you have the tiff library (-ltiff).  */
#define HAVE_LIBTIFF 1

/* SDL_image shared library */
#define SDLIMAGE_LIB "???"

/* Name of package */
#define PACKAGE "paragui"

/* Version number of package */
#define VERSION "1.1.8"

#endif
