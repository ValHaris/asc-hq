/***************************************************************************
                          stringtokenizer.h  -  description
                             -------------------
    begin                : Sun Jan 28 2001
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/*! \file stringtokenizer.h
    \brief A simple string tokenizer
*/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef stringtokenizer_h_included
 #define stringtokenizer_h_included

 #include "ascstring.h"

 class StringTokenizer {
       const ASCString& str;
       int i;
       bool includeOperators;
    private:
       int CharSpace ( char c );
    public:
       StringTokenizer ( const ASCString & _str, bool includeOperators_ = false ) : str( _str ), i ( 0 ), includeOperators ( includeOperators_ ) {};
       ASCString getNextToken ( );
       ASCString getRemaining ( );
 };


#endif
