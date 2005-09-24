/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef cannedmessagesH
 #define cannedmessagesH

//! returns a pointer to the message id. The messages must have been loaded with loadmessages
extern const char* getmessage( int id );

//! loads all messages from the files message?.txt
extern void  loadmessages();


#endif
