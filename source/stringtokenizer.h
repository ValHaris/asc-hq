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


#ifndef stringtokenizerH
 #define stringtokenizerH

 #include "ascstring.h"

 //! This String Tokenizer is NOT intended to be a general purpose tool. It is exclusively used by the TextFileParser
 class StringTokenizer {
       const ASCString& str;
       int i;
       bool includeOperators;
       ASCString delimitter;
    private:
       int CharSpace ( char c );
    public:
       StringTokenizer ( const ASCString& _str, bool includeOperators_ = false );
       StringTokenizer ( const ASCString& _str, const ASCString& delimitter_ );
       StringTokenizer ( const ASCString& _str, const char* delimitter_ );
       ASCString getNextToken ( );
       ASCString getRemaining ( );
 };

  class StringSplit {
       const ASCString& str;
       int i;
       ASCString delimitter;
       bool isDelimitter ( char c );
     public:
       StringSplit ( const ASCString& _str, const ASCString& delimitter_ );
       ASCString getNextToken ( );
  };


#endif
