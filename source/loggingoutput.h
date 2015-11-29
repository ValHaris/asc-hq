
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef loggingoutputH
#define loggingoutputH

#include <sigc++/sigc++.h>
#include "ascstring.h"
#include <fstream>

 class LoggingOutputHandler : public sigc::trackable {
       ASCString filename;
       std::ofstream logfile;
       void messageLogger( const ASCString& msg, int level );
       static ASCString getFilename( const ASCString& path );
    public:
       LoggingOutputHandler( const ASCString& path );
 };


#endif
