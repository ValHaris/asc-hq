/* sigc++/config/sigcconfig.h.  Generated automatically by configure.  */
/* sigc++/config/sigcconfig.h.in. */
/*
  This file controls all the configurablity of sigc++ with regards
  to different compilers.  If you are begining a new port of sigc++
  to a compiler this is where to start.

  Unix compilers are handled automatically by configure.  Other
  platforms require proper identification here.  To add a new
  port, first identify your compilers unique predefine and
  create a LIBSIGC_{compiler} in the detection stage.  Then
  place a section which defines for the behavior of your compiler
  in the platform section.
*/
#ifndef _SIGC_CONFIG_H_
#define _SIGC_CONFIG_H_

// autoconf likes to place a lot of stuff we don't want.
#if 0

/* Define if you have the <dlfcn.h> header file.  */
#define HAVE_DLFCN_H 1

/* Name of package */
#define PACKAGE "libsigc++"

/* Version number of package */
#define VERSION "1.2.1"

#endif /* 0 */

#define SIGC_MAJOR_VERSION 1
#define SIGC_MINOR_VERSION 2
#define SIGC_MICRO_VERSION 1

// Detect common platforms

#if defined(__MWERKS__)
#define LIBSIGC_MWERKS
#elif defined(_WIN32)
// Win32 compilers have a lot of varation
#if defined(__BORLANDC__)
#define LIBSIGC_BC
#define LIBSIGC_DLL
#elif defined(_MSC_VER)
#define LIBSIGC_MSC
#define LIBSIGC_DLL
#elif defined(__CYGWIN__)
// cygwin is considered unix and doesn't need DLL decl.
#define LIBSIGC_GCC
#elif defined(__MINGW32__)
// LIBSIGC_DLL not strictly required for latest gcc on mingw
// (that uses auto-import).
#define LIBSIGC_DLL
#define LIBSIGC_GCC
#else
#error "Unknown architecture (send me gcc --dumpspecs)"
#endif
#else
#define LIBSIGC_GCC
#endif /* _WIN32 */

// Compiler specific definitions

#ifdef LIBSIGC_GCC
#define SIGC_CXX_INT_CTOR 1
#define SIGC_CXX_NAMESPACES 1
#define SIGC_CXX_PARTIAL_SPEC 1
#define SIGC_CXX_SPECIALIZE_REFERENCES 1
#define SIGC_CXX_VOID_RETURN 1
#define SIGC_CXX_VOID_CAST_RETURN 1
#define SIGC_CXX_EXPLICIT explicit
#define SIGC_CXX_EXPLICIT_COPY explicit
#endif /* LIBSIGC_GCC */

#ifdef LIBSIGC_BC
#define SIGC_CXX_INT_CTOR 1
#define SIGC_CXX_NAMESPACES 1
#define SIGC_CXX_PARTIAL_SPEC 1
#define SIGC_CXX_SPECIALIZE_REFERENCES 1
#define SIGC_CXX_VOID_RETURN 1
#define SIGC_CXX_VOID_CAST_RETURN 1
//#define SIGC_CXX_MEMBER_FUNC_TEMPLATES 1
//#define SIGC_CXX_MEMBER_CLASS_TEMPLATES 1
//#define SIGC_CXX_MUTABLE 1
//#define SIGC_CXX_FRIEND_TEMPLATES 1
#define SIGC_CXX_EXPLICIT explicit
#define SIGC_CXX_EXPLICIT_COPY explicit
#endif /* LIBSIGC_BC */

#ifdef LIBSIGC_MSC
#define SIGC_CXX_INT_CTOR 1
#define SIGC_CXX_NAMESPACES 1
//#define SIGC_CXX_MEMBER_FUNC_TEMPLATES 1
//#define SIGC_CXX_MEMBER_CLASS_TEMPLATES 1
//#define SIGC_CXX_TEMPLATE_CCTOR 1
//#define SIGC_CXX_MUTABLE 1
#define SIGC_CXX_EXPLICIT explicit
#define SIGC_CXX_EXPLICIT_COPY explicit
#endif /* LIBSIGC_MSC */

#ifdef LIBSIGC_MWERKS
#define SIGC_CXX_INT_CTOR 1
#define SIGC_CXX_NAMESPACES 1
#define SIGC_CXX_PARTIAL_SPEC 1
#define SIGC_CXX_SPECIALIZE_REFERENCES 1
#define SIGC_CXX_VOID_RETURN 1
#define SIGC_CXX_VOID_CAST_RETURN 1
//#define SIGC_CXX_MEMBER_FUNC_TEMPLATES 1
//#define SIGC_CXX_MEMBER_CLASS_TEMPLATES 1
//#define SIGC_CXX_MUTABLE 1
#define SIGC_CXX_EXPLICIT explicit
#define SIGC_CXX_EXPLICIT_COPY explicit
#endif /* LIBSIGC_MWERKS */


// Window DLL declarations

#ifdef LIBSIGC_DLL
 #if defined(LIBSIGC_COMPILATION) && defined(DLL_EXPORT)
  #define LIBSIGC_API 
  #define LIBSIGC_TMPL
 #elif !defined(LIBSIGC_COMPILATION)
  #define LIBSIGC_API 
  #define LIBSIGC_TMPL extern
 #else
  #define LIBSIGC_API
 #endif /* LIBSIGC_COMPILATION - DLL_EXPORT */
#else
 #define LIBSIGC_API
#endif /* LIBSIGC_DLL */


#endif /* _SIGC_CONFIG_H_ */

