/***************************************************************************
                          messages.cpp  -  description
                             -------------------
    begin                : Thu Feb 21 2002
    copyright            : (C) 2002 by Martin Bickel
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

#include "global.h"
#include "messages.h"
// #include "gamedlg.h"
#include "gamemap.h"
#include "spfst.h"

Message :: Message ( GameMap* spfld  )  : cc(0)
{
   from = 1 << spfld->actplayer;
         
   gametime = spfld->time;
   time = ::time( NULL );
   to = 0;
   spfld->messageid++;
   id = spfld->messageid;

   spfld->messages.push_back ( this );
}


Message :: Message ( const ASCString& msg, GameMap* gamemap, int rec, int _from ) : cc(0)  // f?r Meldungen vom System
{
   from = _from;
   gametime = gamemap->time;
   time = ::time( NULL );
   to = rec;
   text = msg;
   gamemap->messageid++;
   id = gamemap->messageid;

   gamemap->messages.push_back ( this );

   for ( int i = 0; i < 8; i++ )
      if ( to & ( 1 << i ))
         gamemap->player[i].unreadmessage.push_back ( this );
  
}


ASCString Message::bitMap2PlayerName( int p, const GameMap* gamemap  ) const
{
   ASCString s;
   for ( int i = 0; i < gamemap->getPlayerCount(); ++i )
      if ( p & ( 1 << i)) {
         if ( !s.empty() )
            s += ", ";
         s += gamemap->getPlayer(i).getName();
      }
   return s;
}


ASCString Message::getFromText( const GameMap* gamemap ) const
{
   if ( from <= 0 )
      return "";
   
   if ( from == (1<<9))
      return "system";
   
   return gamemap->player[log2(from)].getName();
}


// #endif



