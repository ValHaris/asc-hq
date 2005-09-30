
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


 #include "messaginghub.h"

 
StatusMessageWindow :: StatusMessageWindow( const StatusMessageWindow& smw )
{
   copy(smw);
}
 
StatusMessageWindow::StatusMessageWindow()
{
   userData = new UserData;
}


void StatusMessageWindow::unlink()
{
   if ( userData ) {
      userData->counter -= 1;
      if ( userData->counter <= 0 ) {
         delete userData;
         userData = NULL;
      }
   }
}

void StatusMessageWindow::copy( const StatusMessageWindow& smw )
{
   if ( smw.userData ) {
      unlink();
      userData = smw.userData;
      userData->counter += 1;
   } else {
      userData = new UserData;
   }   
}

void StatusMessageWindow::close()
{
   unlink();
}   


StatusMessageWindow& StatusMessageWindow::operator=( const StatusMessageWindow& smw )
{
   copy(smw);
   return *this;
}


StatusMessageWindow::~StatusMessageWindow()
{
   unlink();
}

StatusMessageWindow MessagingHubBase::infoMessageWindow( const ASCString& msg ) { 
   return StatusMessageWindow( messageWindowFactory( msg )); 
}

 
void MessagingHubBase::message( MessageType type, const ASCString& msg, ... )
{
   va_list arglist;
   va_start ( arglist, msg );

   ASCString message;
   message.vaformat( msg.c_str(), arglist );
   
     
   switch ( type ) {
      case FatalError: fatalError( message ); break;
      case Error:      error ( message ); break;
      case Warning:    warning( message ); break;
      case InfoMessage: infoMessage( message ); break;
      case StatusInfo: statusInformation( message ); break;
      case LogMessage: logMessage( message, 0 ); break;
   };   
   
   va_end ( arglist );
}
 
  
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
 
