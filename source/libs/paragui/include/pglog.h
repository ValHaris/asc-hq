/*
    ParaGUI - crossplatform widgetset
    Copyright (C) 2000,2001,2002  Alexander Pipelka
 
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
 
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.
 
    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 
    Alexander Pipelka
    pipelka@teleweb.at
 
    Last Update:      $Author: mbickel $
    Update Date:      $Date: 2009-04-18 13:48:39 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pglog.h,v $
    CVS/RCS Revision: $Revision: 1.3 $
    Status:           $State 
*/

/**
	\file pglog.h
	ParaGUI logging facility.
	This header declares the PG_LogConsole namespace and all functions for log processing.
*/

#ifndef PGLOG_H
#define PGLOG_H

#include <stdarg.h>
#include "SDL.h"
#include "paragui.h"
#include "pglabel.h"

//! ParaGUI logging level
typedef enum {
    PG_LOG_NONE,  //!< log nothing
    PG_LOG_ERR, //!< only errors
    PG_LOG_WRN, //!< errors and warnings
    PG_LOG_MSG, //!< display errors, warnings and messages
    PG_LOG_DBG //!< log everything
} PG_LOG_LEVEL;

/**
	Logmethod: STDOUT.
	Send all log messages to stdout.
*/
#define	PG_LOGMTH_STDOUT 	1
/**
	Logmethod: STDERR.
	Send all log messages to stderr.
*/
#define	PG_LOGMTH_STDERR	2
/**
	Logmethod: CONSOLE.
	Send all log messages to the console window.
*/
#define	PG_LOGMTH_CONSOLE  	4

#ifdef __cplusplus

/**
	@short logging functions.
 
	These functions can be used for any purpose logging
*/
namespace PG_LogConsole {
/**
Log a message
 
Don't use this function directly, it is unconvenient and meant for
internal use only. Use The PG_Log functions instead if you want to
log something.
 
@param	id		the log level of the message
@param	Text	the message text *printf format string)
@param	ap		the variable list
*/
DECLSPEC void LogVA(PG_LOG_LEVEL id, const char *Text, va_list ap);

/**
Clears the list of log messages and removes the log window
*/
DECLSPEC void Done();

/**
Updates the log window
 
This function creates a log window if it doesn't exist and writes
all log messages to this window.
*/
DECLSPEC void Update();

/**
Set the minimum log level
 
@param	newlevel	the new minimum log level
All log messages with a log level smaller than newlevel will be
quietly discarded.
*/
DECLSPEC void SetLogLevel(PG_LOG_LEVEL newlevel);

/**
Show the log window
*/
DECLSPEC void Show();

/**
Hide the log window
*/
DECLSPEC void Hide();

/**
Toggle between log window shown/hidden
*/
DECLSPEC void Toggle();

/**
Set the log method
 
@param	method	determines where to log messages to
*/
DECLSPEC void SetMethod(int method);

/**
Get the log method
 
@return	current log method
*/
DECLSPEC int GetMethod();

/**
Set the key to toggle the console (default F12)
 
@param	key	the key to toggle the console
The key will be used by PG_Application to toggle the log console.
*/
DECLSPEC void SetConsoleKey(SDLKey key);

/**
Get the current key to toggle console
 
@return	current key to toggle the console
*/
DECLSPEC SDLKey GetConsoleKey();

/**
Set the title of the log console window
 
@param	title	the title of the window
@param	alignment	the alingment of the title
*/
DECLSPEC void SetTitle(const char* title, PG_Label::TextAlign alignment = PG_Label::CENTER);

/**
Set a limit for the amount of log lines (200 lines by default).
 
@param	max maximum amount of log lines.
*/
DECLSPEC void SetMaxLogLines(Uint32 max);
}

#endif

#ifdef __cplusplus
extern "C" {
#endif

	/**
	Log a message
	 
	@param	id	the log level id of the message
	@param	Text	a printf format string, arbitrary arguments may follow
	*/
	DECLSPEC void PG_Log(PG_LOG_LEVEL id, const char *Text, ...);

	/**
	Log a message with log level "message"
	 
	@param	fmt	printf format log string
	*/
	DECLSPEC void PG_LogMSG(const char *fmt, ...);

	/**
	Log a message with log level "error"
	 
	@param	fmt	printf format string
	*/
	DECLSPEC void PG_LogERR(const char *fmt, ...);

	/**
	Log a message with log level "warning"
	 
	@param	fmt	printf format string
	*/
	DECLSPEC void PG_LogWRN(const char *fmt, ...);

	/**
	Log a message with log level "debug"
	 
	@param	fmt	printf format string
	*/
	DECLSPEC void PG_LogDBG(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif // PGLOG_H

/*
 * Local Variables:
 * c-basic-offset: 8
 * End:
 */
