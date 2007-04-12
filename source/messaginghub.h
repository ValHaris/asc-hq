
/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-1999  Martin Bickel  and  Marc Schellenberger

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; see the file COPYING. If not, write to the
    Free Software Foundation, Inc., 59 Temple Place, Suite 330,
    Boston, MA  02111-1307  USA
*/


#ifndef messaginghubH
#define messaginghubH

 #include <sigc++/sigc++.h>
 #include "libs/loki/Singleton.h"

 #include "ascstring.h"

  //! A class that hols a MessageWindow. This class ensures that the window is removed if the last copy of the object is deleted
  class StatusMessageWindowHolder {
    public:
       class UserData {
             friend class StatusMessageWindowHolder;
             int counter;  
          public:
             UserData() : counter(1) {};
             virtual ~UserData() {};
       
       };
    private:   
       UserData* userData;
    protected:
       void copy( const StatusMessageWindowHolder& smw );
       void unlink();
    public:
       StatusMessageWindowHolder();
       StatusMessageWindowHolder( UserData* ud ) : userData(ud) {};
       StatusMessageWindowHolder( const StatusMessageWindowHolder& smw );
       StatusMessageWindowHolder& operator=( const StatusMessageWindowHolder& smw );    
       void close();
       virtual ~StatusMessageWindowHolder();
 };


 class MessagingHubBase {
    protected:
       int verbosity;
    
    public:
       MessagingHubBase() : verbosity(0) {};
       void setVerbosity( int v ) { verbosity = v; };
       int  getVerbosity() { return verbosity; };
       
       enum MessageType { FatalError, Error, Warning, InfoMessage, StatusInfo, LogMessage };
       
       //! displays an error message and aborts the game
       SigC::Signal1<void, const ASCString&> fatalError;

       //! exits the program
       SigC::Signal0<void> exitHandler;
              
       //! displays an error message and continues game
       SigC::Signal1<void, const ASCString&> error;
       
       //! displays a warning
       SigC::Signal1<void, const ASCString&> warning;
       
       //! displays an informational message 
       SigC::Signal1<void, const ASCString&> infoMessage;
       
       //! displays a message in the message line
       SigC::Signal1<void, const ASCString&> statusInformation;
       
       //! prints a message to the logging file
       SigC::Signal2<void, const ASCString&,int> logMessage;
       
       //! displays any kind of message, as specified by parameter
       void message( MessageType type, const char* msg, ... );

       //! displays any kind of message, as specified by parameter
       void message( MessageType type, const ASCString& msg );


       /** Displays a status window. As long as a copy of the returned StatusMessageWindowHolder exists, the window is shown.
       Typical usage:
       \code
        if ( doSomething ) {
             MessagingHubBase::StatusMessageWindowHolder smwh = MessagingHub::Instance().infoMessageWindow( "I'm doing something" );
             doIt();
        } // status window is automatically removed when scope is left
        \endcode
        */
       StatusMessageWindowHolder infoMessageWindow( const ASCString& msg );
        
       //! prints a message to the logging file
       SigC::Signal1<StatusMessageWindowHolder, const ASCString&> messageWindowFactory;
              
   };
       
 typedef Loki::SingletonHolder<MessagingHubBase > MessagingHub;


 
 extern void fatalError ( const char* formatstring, ... );
 extern void fatalError ( const ASCString& string );
 extern void errorMessage ( const ASCString& string );
 extern void warning ( const ASCString& string );
 extern void infoMessage ( const ASCString& string );
 extern void statusMessage ( const ASCString& string );
       
 extern void displayLogMessage ( int msgVerbosity, const char* message, ... );
 extern void displayLogMessage ( int msgVerbosity, const ASCString& message );


#endif

