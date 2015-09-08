
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include <iostream>

#include "stdio-errorhandler.h"
#include "util/messaginghub.h"

void StdIoErrorHandler::printStdout( const ASCString& msg )
{
   std::cout << msg << "\n";
}
void StdIoErrorHandler::printStderr( const ASCString& msg )
{
   std::cerr << msg << "\n";
}

void StdIoErrorHandler::messageLogger( const ASCString& msg, int level )
{
   // std::cout << "L" << level << ": " << msg << "\n";
   std::cout << msg << std::flush;
}

StdIoErrorHandler::StdIoErrorHandler( bool quitOnFatalError )
{
   MessagingHub::Instance().warning.connect( sigc::mem_fun( *this, &StdIoErrorHandler::printStderr ));
   MessagingHub::Instance().error.connect( sigc::mem_fun( *this, &StdIoErrorHandler::printStderr ));
   MessagingHub::Instance().fatalError.connect( sigc::mem_fun( *this, &StdIoErrorHandler::printStderr ));
   MessagingHub::Instance().infoMessage.connect( sigc::mem_fun( *this, &StdIoErrorHandler::printStdout ));
   MessagingHub::Instance().logMessage.connect( sigc::mem_fun( *this, &StdIoErrorHandler::messageLogger ));

   if ( quitOnFatalError )
      MessagingHub::Instance().exitHandler.connect( sigc::bind( SigC::slot( exit ), -1 ));

}


