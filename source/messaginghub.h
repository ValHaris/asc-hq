
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

  class StatusMessageWindow {
    public:
       class UserData {
             friend class StatusMessageWindow;
             int counter;  
          public:
             UserData() : counter(1) {};
             virtual ~UserData() {};
       
       };
    private:   
       UserData* userData;
    protected:
       void copy( const StatusMessageWindow& smw );
       void unlink();
    public:
       StatusMessageWindow();
       StatusMessageWindow( UserData* ud ) : userData(ud) {};
       StatusMessageWindow( const StatusMessageWindow& smw );
       StatusMessageWindow& operator=( const StatusMessageWindow& smw );    
       void close();
       virtual ~StatusMessageWindow();
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
       void message( MessageType type, const ASCString& msg, ... );
       
       StatusMessageWindow infoMessageWindow( const ASCString& msg );
        
       //! prints a message to the logging file
       SigC::Signal1<StatusMessageWindow, const ASCString&> messageWindowFactory;
              
   };
       
 typedef Loki::SingletonHolder<MessagingHubBase > MessagingHub;


 
 extern void fatalError ( const char* formatstring, ... );
 extern void fatalError ( const ASCString& string );
 extern void errorMessage ( const ASCString& string );
 extern void warning ( const ASCString& string );
 extern void longWarning ( const ASCString& string );
 extern void infoMessage ( const ASCString& string );

 extern void displayLogMessage ( int msgVerbosity, char* message, ... );
 extern void displayLogMessage ( int msgVerbosity, const ASCString& message );
      

#endif

