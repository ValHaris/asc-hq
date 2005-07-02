
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

 class MessagingHubBase {
    protected:
       int verbosity;
    
    public:
       MessagingHubBase() : verbosity(0) {};
       void setVerbosity( int v ) { verbosity = v; };
       int  getVerbosity() { return verbosity; };
       SigC::Signal1<void, const ASCString&> fatalError;
       SigC::Signal1<void, const ASCString&> error;
       SigC::Signal1<void, const ASCString&> warning;
       SigC::Signal1<void, const ASCString&> infoMessage;
       SigC::Signal1<void, const ASCString&> statusInformation;
       SigC::Signal2<void, const ASCString&,int> logMessage;
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

