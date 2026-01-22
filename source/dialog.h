/*! \file dialog.h
    \brief Interface for all the dialog boxes used by the game and the mapeditor
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

#ifndef dialogH
#define dialogH


#include <sigc++/sigc++.h>
#include "ascstring.h"
#include "dlg_box.h"
#include "password.h"
#include "typen.h"

#include "actions/actionresult.h"

const int dbluedark = 248;

// ASCString  selectFile( const ASCString& ext, bool load );

extern void displaymessage2( const char* formatstring, ... );
   
//! displays a message in the message line
extern void  dispmessage2(int id, const char * st = NULL );
extern void  dispmessage2( const ActionResult& result );

extern Uint8 mix3colors ( int p1, int p2, int p3 );
extern Uint8 mix2colors ( int a, int b );
extern Uint8 mix4colors ( int a, int b, int c, int d );

//! the dialog box for setting up how to load bi3 graphics and maps. Since ASC now uses its own graphics, this dialog is not used any more.
extern void bi3preferences  ( void );


//! a dialog box that lets a user resize the active map. Should only be used in the mapeditor
extern void resizemap ( void );

extern void viewUnitSetinfo ( void );

/*! displays a dialog with two buttons, to select one of them

    \param title: the message text; printf style arguments allowed
    \param leftButton the text on the left button
    \param rightButton the text on the right button
    \returns 1 if the left button has been pressed; 2 if the right button has been pressed
 */
extern int   choice_dlg(const ASCString& title,
                        const ASCString& leftButton,
                        const ASCString& rightButton );

extern void selectgraphicset ( void );

extern int editInt( const ASCString& title, int defaultValue, int minValue = 0, int maxValue = maxint );


class ActionResult;
extern void displayActionError( const ActionResult& result, const ASCString& additionalInfo = "" );

extern void showTipDialog(const ASCString& text, const ASCString& key);


#endif
