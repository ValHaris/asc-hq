/***************************************************************************
                          ascstring.cpp  -  description
                             -------------------
    begin                : Sun Jan 28 2001
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/*! \file ascstring.cpp
    \brief Implementation of a string class that extends ansi C++ strings

    The purpose of ASCString is to provide extra functionality, hide holes
    of GNU's STL implementation and it should make a potential transition to 
    unicode as easy as possible
*/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <cctype>
#include <stdio.h>
#include "ascstring.h"
#include "errors.h"

/*
ASCString& ASCString::sprintf ( const std::string* s, ... )
{
   va_list argptr;
   va_start(argptr, s );

   int size = 10000;
   bool finished = false;
   while ( !finished ) {
       int iLen;

       char *buf = new char[size];
       if ( buf )
         iLen = vsnprintf(buf, size, s.c_str(), argptr);
       else
         throw ASCmsgException ( "ASCString::sprintf  -- memory allocation error");

       if ( iLen != -1 ) {
           // ok, there was enough space

          *this = buf;
           finished = true;
       }
       delete[] buf;

       size *= 10;
   }

   va_end(argptr);
   return *this;
}
*/

ASCString& ASCString :: toLower ( )
{
   for ( int i = 0; i < length(); i++ )
      (*this)[i] = tolower( (*this)[i] );

   return *this;
}

ASCString& ASCString :: toUpper ( )
{
   for ( int i = 0; i < length(); i++ )
      (*this)[i] = toupper( (*this)[i] );

   return *this;
}

ASCString  ASCString :: copyToLower ( ) const
{
   ASCString s = *this;
   s.toLower();
   return s;
}

ASCString  ASCString :: copyToUpper ( ) const
{
   ASCString s = *this;
   s.toUpper();
   return s;
}

int ASCString :: compare ( int pos, int n, const ASCString& s )
{
   #ifdef _UNIX_
    return inherited::compare ( s, pos, n );
   #else
    
    return inherited::compare (  pos, n, s );
   #endif
}

bool ASCString :: equal_ci ( const ASCString& s ) const
{
   if ( length() != s.length() )
      return false;

   for ( int i = 0; i < length(); i++  )
      if ( s[i] != this->operator[] (i) )
         if ( ! ( s[i] >= 'A' && s[i] <= 'Z' && s[i] == this->operator[](i)+'A'-'a' ||
                  s[i] >= 'a' && s[i] <= 'z' && s[i] == this->operator[](i)+'a'-'A' ) )
               return false;

   return true;
}



int StringTokenizer::CharSpace ( char c )
{
  if ( c <= ' ' )
     return 0;

  const char* ops = "=*/+-";
  const char* d = ops;
  do {
     if( *d == c )
        return 2;
     if ( *d == 0 )
        return 1;
     d++;
  } while ( true );
}


ASCString StringTokenizer::getNextToken( )
{
   while ( i < str.length() && !CharSpace(str[i]) )
     i++;

   if ( i == str.length() )
      return "";

   int begin = i;
   int cs = CharSpace( str[i] );
   do {
      i++;
   } while ( i < str.length() && CharSpace( str[i] ) == cs );
   return str.substr(begin, i-begin);
}


ASCString StringTokenizer::getRemaining( )
{
   return str.substr(i);
}
