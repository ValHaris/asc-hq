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
      delimitter = "=*/+->";
//   }
}

StringTokenizer :: StringTokenizer ( const ASCString& _str, const ASCString& delimitter_ )
                 : str( _str ), i ( 0 ), includeOperators ( false ), delimitter(delimitter_)
{
}

StringTokenizer :: StringTokenizer ( const ASCString& _str, const char* delimitter_ )
                 : str( _str ), i ( 0 ), includeOperators ( false ), delimitter(delimitter_)
{
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



StringSplit :: StringSplit ( const ASCString& _str, const ASCString& delimitter_ )
                 : str( _str ), delimitter(delimitter_),i ( 0 )
{
}



bool StringSplit::isDelimitter ( char c )
{
  const char* d = delimitter.c_str();
  do {
     if( *d == c  )
        return true;
     if ( *d == 0 )
        return false;
     d++;
  } while ( true );
}


ASCString StringSplit::getNextToken( )
{
   const char* sp = str.c_str();
   while ( i < str.length() && isDelimitter(sp[i]) )
     i++;

   if ( i == str.length() )
      return "";

   int begin = i;
   while ( i < str.length() && !isDelimitter(sp[i]) )
     i++;
     
   return str.substr(begin, i-begin);
}

