/***************************************************************************
                          stringtokenizer.cpp  -  description
                             -------------------
    begin                : Sun Jan 28 2001
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

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
#include "stringtokenizer.h"
#include "errors.h"


StringTokenizer :: StringTokenizer ( const ASCString& _str, bool includeOperators_ )
                 : str( _str ), i ( 0 )
{
   includeOperators = includeOperators_ ;
   // if ( includeOperators_ ) {
      delimitter = "=*/+-";
//   }
}

int StringTokenizer::CharSpace ( char c )
{
  if ( c <= ' ' )
     return 0;

  const char* d = delimitter.c_str();
  do {
     if( *d == c && !includeOperators )
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
