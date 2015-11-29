
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include <iostream>

#include "loggingoutput.h"
#include "util/messaginghub.h"
#include "basestrm.h"


void LoggingOutputHandler::messageLogger( const ASCString& msg, int level )
{
   logfile << msg << std::flush;
}

ASCString LoggingOutputHandler::getFilename( const ASCString& path )
{
   ASCString p = path;
   appendbackslash ( p );
   p += "log.txt";
   return p;
}


LoggingOutputHandler::LoggingOutputHandler( const ASCString& path )
   : filename( getFilename( path )), logfile( filename.c_str() )
{
   MessagingHub::Instance().logMessage.connect( sigc::mem_fun( *this, &LoggingOutputHandler::messageLogger ));
}




