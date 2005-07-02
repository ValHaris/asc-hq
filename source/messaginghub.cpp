
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


 #include "messaginghub.h"

  
void displayLogMessage ( int msgVerbosity, char* message, ... )
{
   va_list arglist;
   va_start ( arglist, message );
   if ( msgVerbosity <= MessagingHub::Instance().getVerbosity() ) {
      char buf[10000];
      vsprintf ( buf, message, arglist );

      displayLogMessage( msgVerbosity, ASCString(buf) );
   }
   va_end ( arglist );
}

void displayLogMessage ( int msgVerbosity, const ASCString& message )
{
   if ( msgVerbosity <= MessagingHub::Instance().getVerbosity() ) {
      fprintf ( stdout, "%s", message.c_str() );
      fflush ( stdout );
   }
}
 
