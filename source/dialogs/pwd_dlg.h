/***************************************************************************
                          pwd_dlg.h  -  description
                             -------------------
    begin                : Mon Nov 27 2000
    copyright            : (C) 2000 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef password_dialog_h_included
 #define password_dialog_h_included

#include "../password.h"

/*! opens a dialog box for entering a password.
      \param pwd Reference to a password class which may either contain a 
                 password or may be empty.
      \param firstTime If true, the dialog will ask for a new password.
                 If false and pwd is empty, the function returns immediately 
                 true. If pwd contains a password, a dialog pops up that will
                 ask for the password
      \param cancelAllowed Is there a cancel button (which will return false).
                 If there is no cancel button and firsttime is false, there will
                 be a exit button which will throw a #NoMapLoaded exception
                 when pressed.   
      \param defaultAllowed If firstTime is also true, there will be default
                 button which copies the CGameoptions::defaultPassword to 
                 pwd      

      \returns true if password successfully entered. 
               false if the cancel button has been pressed (if available).
               the current map will be deleted and a #NoMapLoaded exception 
                   thrown if the exit button has been pressed (if available)
*/
  extern bool enterpassword ( Password& pwd, bool firstTime = false,
                                             bool cancelAllowed = false,
                                             bool defaultAllowed = true,
                                             const ASCString& username = "" );

#endif
