/** \file errors.h
    \brief The (base-) classes which are thrown as exceptions
*/

/***************************************************************************
                          errors.h  -  description
                             -------------------
    begin                : Fri Oct 6 2000
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

#ifndef errors_h_included
 #define errors_h_included

 #include <string>
 #include "global.h"
 #include "misc.h"

 extern void fatalError ( const char* formatstring, ... );
 extern void fatalError ( const ASCString& string );
 extern void warning ( const ASCString& string );


 #ifdef HAVE_EXCEPTION
  #include <exception>
  class ASCexception : public exception { };
 #else
  class ASCexception {};
 #endif

  class ASCmsgException : public ASCexception {
      protected:
         string message;
      public:
         ASCmsgException ( string msg ) : message ( msg ) {};
         const string& getMessage ( void ) const { return message; };
         virtual ~ASCmsgException() {};
  };

  class InvalidID : public ASCmsgException {
               public:
                 InvalidID ( string msg, int id ) : ASCmsgException ( "Could not find a " + msg )
                 {
                    message += " with an ID of ";
                    message += strrr ( id );
                 };
              };


  class NoMapLoaded : public ASCexception {};
  class OutOfRange  : public ASCexception {};


#endif