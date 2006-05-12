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

 #include "ascstring.h"
 #include "global.h"
 #include "misc.h"

 extern void fatalError ( const char* formatstring, ... );
 extern void fatalError ( const ASCString& string );
 extern void errorMessage ( const ASCString& string );
 extern void warning ( const ASCString& string );
 extern void longWarning ( const ASCString& string );
 extern void infoMessage ( const ASCString& string );


 #ifdef HAVE_EXCEPTION
  #include <exception>
  class ASCexception : public exception { };
 #else
  class ASCexception {};
 #endif

  class ASCmsgException : public ASCexception {
      protected:
         ASCString message;
      public:
         ASCmsgException ( const ASCString& msg ) : message ( msg ) {};
         const ASCString& getMessage ( void ) const { return message; };
         virtual ~ASCmsgException() {};
  };

  class InvalidID : public ASCmsgException {
               public:
                 InvalidID ( string msg, int id ) : ASCmsgException ( "Could not find a " + msg )
                 {
                    message += " with an ID of ";
                    message += strrr ( id );
                    message += "\nThis is usually caused when the file you are trying to load uses objects\n"
                               "from optional data packages that you don't have installed." ;

                 };
              };


  class NoMapLoaded : public ASCexception {};
  class ShutDownMap : public ASCexception {};
  class OutOfRange  : public ASCexception {};

  
  class AssertionException : public ASCmsgException {
     public:
        AssertionException ( const ASCString& check, const ASCString& file, int line ) : ASCmsgException ( ASCString("Assertion failed: ") + check + " at " + file + ":" + ASCString::toString(line)  ) {};
  };

#define assertOrThrow(expr)  (static_cast<void> ( (expr) ? 0 : (throw AssertionException (__STRING(expr), __FILE__, __LINE__))))
  
  

#endif
