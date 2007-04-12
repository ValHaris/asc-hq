/** \file misc.h
    miscellaneous functions which are used by ASC and all its tools
*/

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



#ifndef miscH
   #define miscH

   #include "global.h"
   #include <string>

   #define dblue lightgray
   //153
   
  #ifdef UseMemAvail 
   extern int memavail ( void );       // dummy function
  #endif


   extern int firstBit(int i);
  
  
   //! Count the number of zero bits on the LSB side of "zahl"
   inline int log2(int i) { return firstBit(i);};

   /** converts a to a string.
      \returns a pointer to a newly allocated array of characters, which must be freed with delete[]
    */
   extern char* strr ( int a ) ;
   
   extern const char* digit[] ; 
   extern const char* letter[] ;

   /** converts a to a string.
      \returns a pointer to a static array, which will be overwritten in the next call
    */
   extern char* strrr ( int a ) ;
   // extern char* strrr ( Uint32 a );

   /** converts a to a string.
      \returns a pointer to a static array, which will be overwritten in the next call
    */
   extern char* strrr ( double a ) ;

   extern void beep( void );

   extern int  crc32buf( const void *vbuf, int len);
   
   extern int atoi ( const std::string& s );
   extern double atof ( const std::string& s );

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

/*
template<typename T>
T min ( T a, T b )
{
   if ( a > b )
      return b;
   else
      return a;
}
*/

template<typename T>
const T& min ( const T& a, const T& b, const T&  c )
{
   return min ( a, min ( b, c ));
}

/*
template<typename T>
T max ( T a, T b )
{
   if ( a > b )
      return a;
   else
      return b;
}
*/

template<typename T>
const T& max ( const T& a, const T& b, const T& c )
{
   return max ( a, max ( b, c ));
}


   extern char *strupr (const char *a);

  #ifndef HAVE_ITOA
   extern char* itoa ( int a, char* b, int c);
  #endif

   #if defined(MEMCHK) | defined(sgmain) | defined(karteneditor)
    extern void* asc_malloc ( size_t size );
    extern void asc_free ( void* p );
   #else
    #ifdef asc_malloc
     #undef asc_malloc
    #endif
    #define asc_malloc malloc

    #ifdef asc_free
     #undef asc_free
    #endif
    #define asc_free   free
   #endif

#endif
