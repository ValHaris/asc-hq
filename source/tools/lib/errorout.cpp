/* errorout.cpp ... is for outputting errors in command line tools
 *                  Copyright by Martin Bickel
 *                               Michael Moerz <mikem@debian.org>
 */

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#ifdef HAVE_STDIO_H
#  include <stdio.h>
#endif

/* mikem: BIG FAT WARNING-> this is not really portable */
#include <stdarg.h>

#include "ascstring.h"

void fatalError ( const ASCString& text )
{
   fprintf ( stderr, text.c_str() );

   #ifdef _WIN32_
   MessageBox(NULL, text.c_str(), "Fatal Error", MB_ICONERROR | MB_OK | MB_TASKMODAL );
   #endif

   exit ( 1 );
}


void fatalError ( const char* formatstring, ... )
{
   va_list paramlist;
   va_start ( paramlist, formatstring );

   char buf[10000];
   vsprintf ( buf, formatstring, paramlist );
   va_end ( paramlist );
   fatalError ( ASCString ( buf ));
}



void warning ( const ASCString& output )
{
   cerr << "ASC: " << output.c_str() << endl;
}

