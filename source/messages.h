/***************************************************************************
                          messages.h  -  description
                             -------------------
    begin                : Thu Feb 21 2002
    copyright            : (C) 2002 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/
/*! \file messages.h
    \brief Classes and Functions managing the in-game mails and the journal
*/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef messages_h_included
 #define messages_h_included

 #include "typen.h"

//! A Message to a player. It may either be send by another player or by the system.
class  Message {
   public:
     //! bitmapped variable showing the sender of the message. Bit 0 - 7 are the players, Bit 9 is the system.
     int from;

     //! bitmapped variable showing the recipients of the message.
     int to;

     //! the real world time the message was written
     time_t time;

     //! the body of the message
     ASCString text;

     //! an id that identifies the message. It is assigned automatically
     int id;

     //! the game time the messages was written
     GameTime gametime;

     Message ( pmap spfld );

     /** Constructor.
         \param msg      The message text
         \param gamemap  The map that this message is currently being played
         \param rec      The receipient. Bitmapped: each bit one player
         \param from     The sender. Bitmapped too! 512 = system
     */
     Message ( const ASCString& msg, pmap gamemap,int rec, int from = 512 );
};

typedef PointerList<Message*> MessageContainer;
typedef list<Message*> MessagePntrContainer;



#endif
