/*! \file dlg_box.h
    \brief Interface for some basic classes from which all of ASC's dialogs are derived
*/

/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2010  Martin Bickel  and  Marc Schellenberger

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

#ifndef dlg_boxH
#define dlg_boxH

#include "newfont.h"
#include "events.h"
#include "basegfx.h"

  #define dlg_wintitle 1  
  #define dlg_in3d 2  
  #define dlg_notitle 4  
  #define dlg_3dtitle 8




/** displays a dialog box with a message
   \param formatstring the text, which may contain the same format arguments as sprintf
   \param num 0 normal text, the dialog box will stay visible until removemessage() is called
              1 red text for error message
              2 fatal error, the program will be closed
              3 normal text
*/
extern void displaymessage( const char* formatstring, int num, ... );


/** displays a dialog box with a message
   \param text the text
   \param num 0 normal text, the dialog box will stay visible until removemessage() is called
              1 red text for error message
              2 fatal error, the program will be closed
              3 normal text
*/
extern void displaymessage( const ASCString& text, int num );

//! closes a message dialog box that has been opened by displaymessage()
extern void removemessage( void );

//! displays a dialogbog with the given help topic \sa viewtext2(int)
extern void  help( int id);

//! displays a dialogbog with the given message \sa help(int)
extern void viewtext2 ( int id);

extern int  viewtextquery( int        id,
                           const char *       title,
                           const char *       s1,
                           const char *       s2);





extern int actdisplayedmessage;
extern long lastdisplayedmessageticker ;

class tvirtualscreenbuf {
      public:
        int size; 
        tvirtualscreenbuf ( void );
        void init ( void );
        ~tvirtualscreenbuf ();
        void* buf;
      } ;

extern tvirtualscreenbuf virtualscreenbuf; 
extern int getplayercolor ( int i );

extern char*  strrrd8d(int  l);           // abrunden


/** returns the message with the given ID from the message system. This system spans
    the helpfiles and the message files associated with the maps */
extern ASCString readtextmessage( int id );


extern int getid( const ASCString& title, int lval,int min,int max, const ASCString& valueName = "");
extern ASCString editString( const ASCString& title, const ASCString& defaultValue = "" );



/** displays a dialogbox which lets you choose one of a number of strings.
    \param title the title of the dialog box
    \param entries the list of strings
    \param defaultEntry the entry that is selected by default (-1 if none)
    \returns the selected index or -1 if nothing was selected
 */
extern int chooseString ( const ASCString& title, const vector<ASCString>& entries, int defaultEntry = -1 );

/** displays a dialogbox which lets you choose one of a number of strings.
    \param title the title of the dialog box
    \param entries the list of strings
    \param buttons the names of the buttons that are going to be displayed
    \param defaultEntry the entry that is selected by default (-1 if none)
    \returns button-number, selected index 
 */
extern pair<int,int> chooseString ( const ASCString& title, const vector<ASCString>& entries, const vector<ASCString>& buttons, int defaultEntry = -1 );

#endif

