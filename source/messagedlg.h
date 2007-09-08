/***************************************************************************
                          messagedlg.h  -  description
                             -------------------
    begin                : Mon Mär 24 2003
    copyright            : (C) 2003 by Martin Bickel
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

#ifndef messagedlgH
 #define messagedlgH

 #include "messages.h"

class Player;
class Message;

extern void editmessage( Message& msg );
extern void newmessage();
extern void viewmessages( char* title, const MessagePntrContainer& msgc, bool editable  ) ; 
extern void viewmessage( const Message& message );
extern void editmessage( Message& msg );
extern void editjournal();
extern void viewjournal( bool showEmptyDialog );
extern void viewunreadmessages( Player& player );
extern void checkJournal( Player& player );

#endif
