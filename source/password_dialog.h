/***************************************************************************
                          password_dialog.h  -  description
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

 #include "password.h"

  extern bool enterpassword ( Password& pwd, bool firstTime = false,
                                             bool cancelAllowed = false,
                                             bool defaultAllowed = true );

#endif