
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include <windows.h>
#include <winuser.h>

#include "win32-errormsg.h"
#include "../util/messaginghub.h"


void Win32IoErrorHandler::printError( const ASCString& msg )
{
   MessageBox(NULL, msg.c_str(), "Error", MB_ICONERROR | MB_OK | MB_TASKMODAL );
}

Win32IoErrorHandler::Win32IoErrorHandler()
{
   MessagingHub::Instance().error.connect( sigc::mem_fun( *this, &Win32IoErrorHandler::printError ));
   MessagingHub::Instance().fatalError.connect( sigc::mem_fun( *this, &Win32IoErrorHandler::printError ));
}

