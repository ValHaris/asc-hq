
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


 #include "messaginghub.h"

 
StatusMessageWindowHolder :: StatusMessageWindowHolder( const StatusMessageWindowHolder& smw )
   : userData( NULL )
{
   copy(smw);
}
 
StatusMessageWindowHolder::StatusMessageWindowHolder()
{
   userData = new UserData;
}


void StatusMessageWindowHolder::unlink()
{
   if ( userData ) {
      userData->counter -= 1;
      if ( userData->counter <= 0 ) {
         delete userData;
         userData = NULL;
      }
   }
}

void StatusMessageWindowHolder::copy( const StatusMessageWindowHolder& smw )
{
   if ( smw.userData ) {
      unlink();
      userData = smw.userData;
      userData->counter += 1;
   } else {
      userData = new UserData;
   }   
}

void StatusMessageWindowHolder::close()
{
   unlink();
}   

void StatusMessageWindowHolder::SetText( const ASCString& text )
{
   if ( userData )
      userData->SetText( text );
}


StatusMessageWindowHolder& StatusMessageWindowHolder::operator=( const StatusMessageWindowHolder& smw )
{
   copy(smw);
   return *this;
}


StatusMessageWindowHolder::~StatusMessageWindowHolder()
{
   unlink();
}

StatusMessageWindowHolder MessagingHubBase::infoMessageWindow( const ASCString& msg ) { 
   return StatusMessageWindowHolder( messageWindowFactory( msg )); 
}


void MessagingHubBase::message( MessageType type, const ASCString& message )
{
   switch ( type ) {
      case FatalError: fatalError( message ); 
                       exitHandler();
                       break;
      case Error:      error ( message ); break;
      case Warning:    warning( message ); break;
      case InfoMessage: infoMessage( message ); break;
      case StatusInfo: statusInformation( message ); break;
      case LogMessage: logMessage( message, 0 ); break;
   };   

}

void MessagingHubBase::setLoggingCategory( const ASCString& category, bool enable )
{
   if ( enable ) {
      if ( enabledLogCategories.find( category ) == enabledLogCategories.end())
         enabledLogCategories.insert( category );
   } else {
      if ( enabledLogCategories.find( category ) != enabledLogCategories.end())
         enabledLogCategories.erase( category );
   }
}

 
void MessagingHubBase::message( MessageType type, const char* msg, ... )
{
   va_list arglist;
   va_start ( arglist, msg );

   ASCString my_message;
   my_message.vaformat( msg, arglist );
   
   message( type, my_message );  
   
   va_end ( arglist );
}
 
  
void displayLogMessage ( int msgVerbosity, const char* message, ... )
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
   if ( msgVerbosity <= MessagingHub::Instance().getVerbosity() ) 
      MessagingHub::Instance().logMessage( message, msgVerbosity );

}


bool MessagingHubBase::logCategoryEnabled(const ASCString& category)
{
   return enabledLogCategories.find( category ) != enabledLogCategories.end();
}

void logMessage ( const ASCString& category, const ASCString& message )
{
   if ( MessagingHub::Instance().logCategoryEnabled( category ) )
      MessagingHub::Instance().logCategorizedMessage( category, message );
}

void fatalError ( const ASCString& string )
{
   MessagingHub::Instance().message( MessagingHubBase::FatalError, string );
}

void fatalError ( const char* msg, ... )
{
   va_list arglist;
   va_start ( arglist, msg );

   ASCString message;
   message.vaformat( msg, arglist );

   fatalError( message );
      
   va_end ( arglist );
}


void warning ( const ASCString& str )
{
   MessagingHub::Instance().message( MessagingHubBase::Warning, str );
}

void warningMessage ( const ASCString& str )
{
   MessagingHub::Instance().message( MessagingHubBase::Warning, str );
}

void errorMessage ( const ASCString& string )
{
   MessagingHub::Instance().message( MessagingHubBase::Error, string );
}

void infoMessage ( const ASCString& string )
{
   MessagingHub::Instance().message( MessagingHubBase::InfoMessage, string );
}

void statusMessage ( const ASCString& string )
{
   MessagingHub::Instance().message( MessagingHubBase::StatusInfo, string );
}

