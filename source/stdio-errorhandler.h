
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef stdioerrorhandlerH
#define stdioerrorhandlerH

#include <sigc++/sigc++.h>
#include "ascstring.h"


 class StdIoErrorHandler : public SigC::Object {
       void printStdout( const ASCString& msg );
       void printStderr( const ASCString& msg );
       void messageLogger( const ASCString& msg, int level );
    public:
       StdIoErrorHandler();
 };


#endif
