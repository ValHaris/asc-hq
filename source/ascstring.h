/***************************************************************************
                          ascstring.h  -  description
                             -------------------
    begin                : Sun Jan 28 2001
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/*! \file ascstring.h
    \brief Interface to a string class that extends ansi C++ strings
*/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef ascstring_h_included
 #define ascstring_h_included

 #include <string>

 class ASCString : public std::string {
       typedef std::string inherited;
     public:
        ASCString ( const std::string& s, size_type pos = 0, size_type n=npos ) : std::string ( s, pos, npos ) {};
        ASCString ( const char* ch, size_type n ) : std::string ( ch, n ) {};
        ASCString ( const char* ch ) : std::string ( ch ) {};
        ASCString ( ) {};
        int compare ( int pos, int n, const ASCString& s );

       // ASCString& sprintf ( const std::string s,  ... );
        ASCString& toLower ( );
        ASCString& toUpper ( );
        ASCString  copyToLower ( ) const;
        ASCString  copyToUpper ( ) const;

        //! compares two string case insensitive
        bool equal_ci ( const ASCString& s ) const;
 };

 class StringTokenizer {
       const ASCString& str;
       int i;
    private:
       int CharSpace ( char c );
    public:
       StringTokenizer ( const ASCString & _str ) : str( _str ), i ( 0 ) {};
       ASCString getNextToken ( );
       ASCString getRemaining ( );
 };


#endif