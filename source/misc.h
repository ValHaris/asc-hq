/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-1999  Martin Bickel  and  Marc Schellenberger

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; see the file COPYING. If not, write to the 
    Free Software Foundation, Inc., 59 Temple Place, Suite 330, 
    Boston, MA  02111-1307  USA
*/

#ifndef misc_h
   #define misc_h
   
   #include "tpascal.inc"
   #include <string.h>
   
   #define dblue lightgray
   //153
   
   extern word log2(int zahl);
   extern int filesize( char *name);
   
   extern int maxavail();
   extern int memavail();
   
   extern char* strr ( int a ) ;
   
   extern const char* digit[] ; 
   extern const char* letter[] ;
   
//   extern boolean exist ( char* s );
   
   extern void initmisc ( void ) ;
   extern char* strrr ( int a ) ;

   extern void beep( void );

   void passtring2cstring ( char* s );
   
   int  crc32buf(void *vbuf, int len);


struct _test_structure {
    char a;
    char b;
    int c;
    char d;
    void* e[10];
};


// is there any way to do this test at compile time ??
class structure_size_tester {       
     public: 
       structure_size_tester ( void );
    };

   #ifdef MEMCHK
   #define __a
   #endif
   

   #ifdef sgmain
   #define __a
   #endif

   #ifdef karteneditor
   #define __a
   #endif


   #ifdef __a
   extern void* asc_malloc ( size_t size );
   extern void asc_free ( void* p );
   #else
   #define asc_malloc malloc
   #define asc_free   free
   #endif
#endif
