/***************************************************************************
                          gamemap.cpp  -  description
                             -------------------
    begin                : Tue Oct 24 2000
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

#include "global.h"
#include "misc.h"
#include "typen.h"

#ifndef converter
#include "spfst.h"
#endif

#ifdef sgmain
 #include "network.h"
 #include "gameoptions.h"
#endif

#include "vehicletype.h"
#include "buildingtype.h"


tmap :: tmap ( void )
{
   int i;

      xsize = 0;
      ysize = 0;
      xpos = 0;
      ypos = 0;
      field = NULL;
      codeword[0] = 0;
      title = 0;
      campaign = NULL;

      actplayer = -1;
      time.abstime = 0;

      weather.fog = 0;

      _resourcemode = 0;

      for ( i = 0; i < 8; i++ )
         for ( int j = 0; j < 8; j++ )
             alliances[i][j] = 0;

      for ( i = 0; i< 9; i++ ) {
         player[i].existent = 0;
         player[i].firstvehicle = NULL;
         player[i].firstbuilding = NULL;

         player[i].ai = NULL;

         if ( i == 0 )
            player[i].stat = 0;
         else
            player[i].stat = 2;

         player[i].name = NULL;
         player[i].passwordcrc = 0;
         player[i].dissectedunit = 0;
         player[i].unreadmessage = NULL;
         player[i].oldmessage = NULL;
         player[i].sentmessage = NULL;
         player[i].queuedEvents = 0;
      }

      oldevents = NULL;
      firsteventtocome = NULL;
      firsteventpassed = NULL;

      unitnetworkid = 0;

      levelfinished = 0;

      network = 0;

      for ( i = 0; i< 8; i++ )
         alliance_names_not_used_any_more[i] = 0;

      for ( i = 0; i < 8; i++ ) {
         cursorpos.position[i].cx = 0;
         cursorpos.position[i].sx = 0;
         cursorpos.position[i].cy = 0;
         cursorpos.position[i].sy = 0;
      }

      tribute = NULL;
      unsentmessage = NULL;
      message = NULL;
      messageid = 0;
      journal = NULL;
      newjournal = NULL;
      for ( i = 0; i< 8; i++ ) {
         humanplayername[i] = NULL;
         computerplayername[i] = NULL;
         alliances_at_beginofturn[i] = 0;
      }
      supervisorpasswordcrc = 0;
      objectcrc = NULL;
      shareview = NULL;
      continueplaying = 0;
      replayinfo = NULL;
      playerView = -1;
      lastjournalchange.abstime = 0;
      preferredfilenames = 0;
      ellipse = 0;
      graphicset = 0;
      gameparameter_num = 0;
      game_parameter = NULL;
      mineralResourcesDisplayed = 0;
      for ( i = 0; i< 8; i++ )
         _oldgameparameter[ i ] = 0;
}


void tmap :: read ( tnstream& stream )
{
   int i;

   xsize = stream.readWord();
   ysize = stream.readWord();
   xpos = stream.readWord();
   ypos = stream.readWord();
   stream.readInt(); // dummy
   field = NULL;
   stream.readdata ( codeword, 11 );

   title = (char*) stream.readInt();
   campaign = (pcampaign) stream.readInt();
   actplayer = stream.readChar();
   time.abstime = stream.readInt();

   weather.fog = stream.readChar();
   for  ( i= 0; i < 3; i++ ) {
      weather.wind[i].speed = stream.readChar();
      weather.wind[i].direction  = stream.readChar();
   }
   for ( i = 0; i< 12; i++ )
      stream.readChar(); // dummy

   _resourcemode = stream.readInt();

   for ( i = 0; i < 8; i++ )
      for ( int j = 0; j < 8; j++ )
         alliances[j][i] = stream.readChar();

   for ( i = 0; i< 9; i++ ) {
      player[i].existent = stream.readChar();
      player[i].firstvehicle = NULL; stream.readInt(); // dummy
      player[i].firstbuilding = NULL; stream.readInt(); // dummy
      player[i].research.read ( stream );
      player[i].ai = (BaseAI*) stream.readInt();
      player[i].stat = stream.readChar();
      player[i].dummy = stream.readChar();
      player[i].name = (char*) stream.readInt();
      player[i].passwordcrc = stream.readInt();
      player[i].dissectedunit = (pdissectedunit) stream.readInt();
      player[i].unreadmessage = (pmessagelist) stream.readInt();
      player[i].oldmessage = (pmessagelist) stream.readInt();
      player[i].sentmessage = (pmessagelist) stream.readInt();
   }

   oldevents = (peventstore) stream.readInt();
   firsteventtocome = (pevent) stream.readInt();
   firsteventpassed = (pevent) stream.readInt();
   unitnetworkid = stream.readInt();
   levelfinished = stream.readChar();
   network = (pnetwork) stream.readInt();
   for ( i = 0; i < 8; i++ ) {
      stream.readInt(); // dummy
      alliance_names_not_used_any_more[i] = 0;
   }

   for ( i = 0; i< 8; i++ ) {
      cursorpos.position[i].cx = stream.readWord();
      cursorpos.position[i].sx = stream.readWord();
      cursorpos.position[i].cy = stream.readWord();
      cursorpos.position[i].sy = stream.readWord();
   }

   tribute = (presourcetribute) stream.readInt();
   unsentmessage = (pmessagelist) stream.readInt();
   message = (pmessage) stream.readInt();

   messageid = stream.readInt();
   journal = (char*) stream.readInt();
   newjournal = (char*) stream.readInt();

   for ( i = 0; i < 8; i++ )
      humanplayername[i] = (char*) stream.readInt();

   for ( i = 0; i < 8; i++ )
      computerplayername[i] = (char*) stream.readInt();

   supervisorpasswordcrc = stream.readInt();

   for ( i = 0; i < 8; i++ )
      alliances_at_beginofturn[i] = stream.readChar();

   objectcrc = (pobjectcontainercrcs) stream.readInt();
   shareview = (pshareview) stream.readInt();

   continueplaying = stream.readInt();
   replayinfo = (treplayinfo*) stream.readInt();
   playerView = stream.readInt();
   lastjournalchange.abstime = stream.readInt();

   for ( i = 0; i< 8; i++ )
      bi_resource[i].read ( stream );

   preferredfilenames = (PreferredFilenames*) stream.readInt();
   ellipse = (EllipseOnScreen*) stream.readInt();
   graphicset = stream.readInt();
   gameparameter_num = stream.readInt();

   game_parameter = (int*) stream.readInt();
   mineralResourcesDisplayed = stream.readInt();
   for ( i = 0; i< 9; i++ )
       player[i].queuedEvents = stream.readInt();

   for ( i = 0; i < 19; i++ )
       stream.readInt();

   for ( i = 0; i < 8; i++ )
       _oldgameparameter[i] = stream.readInt();

}


void tmap :: write ( tnstream& stream )
{
   int i;

   stream.writeWord( xsize );
   stream.writeWord( ysize );
   stream.writeWord( xpos );
   stream.writeWord( ypos );
   stream.writeInt (1); // dummy
   stream.writedata ( codeword, 11 );

   stream.writeInt( title != NULL );
   stream.writeInt( campaign != NULL);
   stream.writeChar( actplayer );
   stream.writeInt( time.abstime );

   stream.writeChar( weather.fog );
   for  ( i= 0; i < 3; i++ ) {
      stream.writeChar( weather.wind[i].speed );
      stream.writeChar( weather.wind[i].direction );
   }

   for ( i = 0; i< 12; i++ )
      stream.writeChar( 0 ); // dummy

   stream.writeInt( _resourcemode );

   for ( i = 0; i < 8; i++ )
      for ( int j = 0; j < 8; j++ )
         stream.writeChar( alliances[j][i] );

   for ( i = 0; i< 9; i++ ) {
      stream.writeChar( player[i].existent );
      stream.writeInt( 1 ); // dummy
      stream.writeInt( 1 ); // dummy
      player[i].research.write ( stream );
      stream.writeInt( player[i].ai != NULL );
      stream.writeChar( player[i].stat );
      stream.writeChar( 0 ); // dummy
      stream.writeInt( player[i].name != NULL );
      stream.writeInt( player[i].passwordcrc );
      stream.writeInt( player[i].dissectedunit != NULL );
      stream.writeInt( player[i].unreadmessage != NULL );
      stream.writeInt( player[i].oldmessage != NULL );
      stream.writeInt( player[i].sentmessage != NULL );
   }

   stream.writeInt( oldevents != NULL );
   stream.writeInt( firsteventtocome != NULL );
   stream.writeInt( firsteventpassed != NULL );
   stream.writeInt( unitnetworkid );
   stream.writeChar( levelfinished );
   stream.writeInt( network != NULL );
   for ( i = 0; i < 8; i++ )
      stream.writeInt( 0 ); // dummy

   for ( i = 0; i< 8; i++ ) {
      stream.writeWord( cursorpos.position[i].cx );
      stream.writeWord( cursorpos.position[i].sx );
      stream.writeWord( cursorpos.position[i].cy );
      stream.writeWord( cursorpos.position[i].sy );
   }

   stream.writeInt( tribute != NULL );
   stream.writeInt( unsentmessage != NULL );
   stream.writeInt( message != NULL );

   stream.writeInt( messageid );
   stream.writeInt( journal != NULL );
   stream.writeInt( newjournal != NULL );

   for ( i = 0; i < 8; i++ )
      stream.writeInt( humanplayername[i] != NULL );

   for ( i = 0; i < 8; i++ )
      stream.writeInt( computerplayername[i] != NULL );

   stream.writeInt( supervisorpasswordcrc );

   for ( i = 0; i < 8; i++ )
      stream.writeChar( alliances_at_beginofturn[i] );

   stream.writeInt( objectcrc != NULL );
   stream.writeInt( shareview != NULL );

   stream.writeInt( continueplaying );
   stream.writeInt( replayinfo != NULL );
   stream.writeInt( playerView );
   stream.writeInt( lastjournalchange.abstime );

   for ( i = 0; i< 8; i++ )
      bi_resource[i].write ( stream );

   stream.writeInt( preferredfilenames != NULL );
   stream.writeInt( ellipse != NULL );
   stream.writeInt( graphicset );
   stream.writeInt( gameparameter_num );

   stream.writeInt( game_parameter != NULL );
   stream.writeInt( mineralResourcesDisplayed );
   for ( i = 0; i< 9; i++ )
       stream.writeInt( player[i].queuedEvents );

   for ( i = 0; i < 19; i++ )
       stream.writeInt( 0 );

   for ( i = 0; i < 8; i++ )
       stream.writeInt( _oldgameparameter[i] );
}


void tmap :: cleartemps( int b, int value )
{
  if ( xsize <= 0 || ysize <= 0)
     return;

  int l = 0;
  for ( int x = 0; x < actmap->xsize ; x++)
     for ( int y = 0; y <  actmap->ysize ; y++) {

         if (b & 1 )
           field[l].a.temp = value;
         if (b & 2 )
           field[l].a.temp2 = value;
         if (b & 4 )
           field[l].temp3 = value;
         if (b & 8 )
           field[l].temp4 = value;

         l++;
     }
}



#ifndef converter
void tmap :: calculateAllObjects ( void )
{
   calculateallobjects();
}
#else
void tmap :: calculateAllObjects ( void )
{
}
#endif

pfield  tmap :: getField(int x, int y)
{
   if ((x < 0) || (y < 0) || (x >= xsize) || (y >= ysize))
      return NULL;
   else
      return (   &field[y * xsize + x] );
}

int tmap :: isResourceGlobal ( int resource )
{
   if ( resource != 1 && !(resource == 2 && getgameparameter(cgp_globalfuel)==0)  &&   _resourcemode == 1 )
      return 1;
   else
      return 0;
}

int tmap :: getgameparameter ( int num )
{
  if ( game_parameter && num < gameparameter_num ) {

     if ( num == cgp_maxminesonfield )
        if ( game_parameter[num] > maxminesonfield )
           return maxminesonfield;

     return game_parameter[num];
  } else
     if ( num < gameparameternum )
        return gameparameterdefault[ num ];
     else
        return 0;
}

void tmap :: setgameparameter ( int num, int value )
{
   if ( game_parameter ) {
     if ( num < gameparameter_num )
        game_parameter[num] = value;
     else {
        int* oldparam = game_parameter;
        game_parameter = new int[num+1];
        for ( int i = 0; i < gameparameter_num; i++ )
           game_parameter[i] = oldparam[i];
        for ( int j = gameparameter_num; j < num; j++ )
           if ( j < gameparameternum )
              game_parameter[j] = gameparameterdefault[j];
           else
              game_parameter[j] = 0;
        game_parameter[num] = value;
        gameparameter_num = num + 1;
        delete[] oldparam;
     }
   } else {
       game_parameter = new int[num+1];
       for ( int j = 0; j < num; j++ )
          if ( j < gameparameternum )
             game_parameter[j] = gameparameterdefault[j];
          else
             game_parameter[j] = 0;
       game_parameter[num] = value;
       gameparameter_num = num + 1;
   }
}

void tmap :: setupResources ( void )
{
  #ifndef converter
   for ( int n = 0; n< 8; n++ ) {
      actmap->bi_resource[n].energy = 0;
      actmap->bi_resource[n].material = 0;
      actmap->bi_resource[n].fuel = 0;

     #ifdef sgmain

      for ( pbuilding bld = actmap->player[n].firstbuilding; bld ; bld = bld->next )
         for ( int r = 0; r < 3; r++ )
            if ( actmap->isResourceGlobal( r )) {
               actmap->bi_resource[n].resource(r) += bld->actstorage.resource(r);
               bld->actstorage.resource(r) = 0;
            }
     #endif
   }
  #endif
}

void tmap :: chainunit ( pvehicle eht )
{
   if ( eht ) {
      eht->next = player[ eht->color / 8 ].firstvehicle;
      if ( eht->next )
         eht->next->prev = eht;
      eht->prev = NULL;
      player[ eht->color / 8 ].firstvehicle = eht;
      if ( eht->typ->loadcapacity > 0)
         for ( int i = 0; i <= 31; i++)
            if ( eht->loading[i] )
               chainunit ( eht->loading[i] );
   }
}

void tmap :: chainbuilding ( pbuilding bld )
{
   if ( bld ) {
      bld->next = player[ bld->color / 8 ].firstbuilding;
      if ( bld->next )
         bld->next->prev = bld;
      bld->prev = NULL;
      player[ bld->color / 8 ].firstbuilding = bld;
   }
}

const char* tmap :: getPlayerName ( int playernum )
{
   if ( playernum >= 8 )
      playernum /= 8;

   switch ( player[playernum].stat ) {
      case 0: return humanplayername[playernum];
      case 1: return computerplayername[playernum];
      default: return "off";
   } /* endswitch */
}



int tmap :: eventpassed( int saveas, int action, int mapid )
{
   return eventpassed ( (action << 16) | saveas, mapid );
}



int tmap :: eventpassed( int id, int mapid )
{
  pevent       ev2;
  peventstore  oldevent;
  char      b;
  word         i;

   b = false;
   if ( !mapid ) {
      ev2 = firsteventpassed;
      while (ev2 != NULL) {
         if (ev2->id == id)
            return 1;
         ev2 = ev2->next;
      }
   }

   if (b == false) {
      oldevent = oldevents;
      while ( oldevent ) {
         if (oldevent->num > 0)
            for (i = 0; i < oldevent->num ; i++)
               if (oldevent->eventid[i] == id && oldevent->mapid[i] == mapid )
                  return 1;
      }
   }
   return 0;
}


pvehicle tmap :: getUnit ( pvehicle eht, int nwid )
{
   if ( !eht )
      return NULL;
   else {
      if ( eht->networkid == nwid )
         return eht;
      else
         for ( int i = 0; i < 32; i++ )
            if ( eht->loading[i] )
               if ( eht->loading[i]->networkid == nwid )
                  return eht->loading[i];
               else {
                  pvehicle ld = getUnit ( eht->loading[i], nwid );
                  if ( ld )
                     return ld;
               }
      return NULL;
   }
}

pvehicle tmap :: getUnit ( int nwid )
{
   for ( int i = 0; i < 9; i++ ) {
      pvehicle veh = player[i].firstvehicle;
      while ( veh ) {
         if ( veh->networkid == nwid )
            return veh;
         veh = veh->next;
      };
   }
   return NULL;
}


pvehicle tmap :: getUnit ( int x, int y, int nwid )
{
  #ifndef converter
   pfield fld  = getfield ( x, y );
   if ( !fld )
      return NULL;

   if ( !fld->vehicle )
      if ( fld->building ) {
         for ( int i = 0; i < 32; i++ ) {
            pvehicle ld = getUnit ( fld->building->loading[i], nwid );
            if ( ld )
               return ld;
         }
         return NULL;
      } else
         return NULL;
   else
      if ( fld->vehicle->networkid == nwid )
         return fld->vehicle;
      else
         return getUnit ( fld->vehicle, nwid );
 #else
  return NULL;
 #endif
}



pvehicletype tmap :: getVehicleType_byId ( int id )
{
   #ifdef converter
   return NULL;
   #else
   return getvehicletype_forid ( id, 0 );
   #endif
}



tmap :: ~tmap ()
{
   pevent       event;
   pevent       event2;

   event = firsteventtocome;
   while (event != NULL) {
      event2 = event;
      event = event->next;
      delete  event2;
   }
   event = firsteventpassed;
   while (event != NULL) {
      event2 = event;
      event = event->next;
      delete event2;
   }

}

