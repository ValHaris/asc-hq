/***************************************************************************
                          password.cpp  -  description
                             -------------------
    begin                : Mon Nov 27 2000
    copyright            : (C) 2000 by Martin Bickel
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

#include "password.h"
#include "misc.h"


void Password :: setUnencoded ( const string& s )
{
   password = encodedpassword2string ( encodepassword ( s.c_str() ) );
}

void Password :: setEncoded   ( const string& s )
{
   if ( !s.empty () ) {
      if ( s[0] == 'A' )
         password = s;
      else
         password = encodedpassword2string ( atoi ( s.c_str() ));
   } else
      password = s;
}


void Password :: setInt  ( int pwd )
{
   password = encodedpassword2string ( pwd );
}

bool Password :: operator== ( const Password& p ) const
{
   return p.password == password;
}

bool Password :: operator!= ( const Password& p ) const
{
   return p.password != password;
}


int Password :: encodepassword ( const char* pw ) const
{
   if ( !pw )
      return 0;

   int len = strlen ( pw );

   if ( len )
      return crc32buf( pw, len+1 );
   else
      return 0;
}

string Password :: encodedpassword2string ( int pwd ) const
{
   string s;
   if ( !pwd )
      return s;

   s = "A";
   if ( pwd > 0 )
      s+= "A";
   else
      s+= "B";
   s += strrr ( abs (pwd) );
   return s;
}


bool Password :: empty () const
{
   return password.empty();
}

string Password :: toString ( ) const
{
   return password;
}


void Password::read ( tnstream& stream )
{
   int i = stream.readInt();
   password = encodedpassword2string ( i );
}

void Password::write ( tnstream& stream ) const
{
   int i;
   if ( password[0] == 'A' ) {
      if ( password[1] == 'A' )
         i = atoi ( password.substr ( 2 ).c_str() );
      else
         i = -atoi ( password.substr ( 2 ).c_str() );

      stream.writeInt ( i );
   } else
      stream.writeInt ( 0 );
}

void Password::reset()
{
  password = "";
}
