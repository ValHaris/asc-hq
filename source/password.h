/***************************************************************************
                          password.h  -  description
                             -------------------
    begin                : Mon Nov 27 2000
    copyright            : (C) 2000 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/*! \file password.h
    \brief A class for holding, encoding and comparing passwords
*/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef password_h_included
 #define password_h_included

 #include <string>
 #include "basestreaminterface.h"
 class Password {
          string password;
       protected:
         int encodepassword ( const char* pw ) const ;
         string encodedpassword2string ( int pwd ) const;

       public:
         void setUnencoded ( const string& s );
         void setEncoded   ( const string& s );
         void setInt  ( int pwd );
         bool empty() const;
         void reset();
         string toString ( ) const ;

         bool operator== ( const Password& p ) const;
         bool operator!= ( const Password& p ) const;
         void read ( tnstream& stream );
         void write ( tnstream& stream ) const;

 };


#endif
