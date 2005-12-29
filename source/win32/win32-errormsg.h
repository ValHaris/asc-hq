
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef win32errorhandlerH
#define win32errorhandlerH

#include <sigc++/sigc++.h>
#include "ascstring.h"


  class Win32IoErrorHandler : public SigC::Object {
       void printError( const ASCString& msg );
     public:
        Win32IoErrorHandler();
  };




#endif
