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

         player[i].dissectedunit = 0;
         player[i].unreadmessage = NULL;
         player[i].oldmessage = NULL;
         player[i].sentmessage = NULL;
         player[i].queuedEvents = 0;
         player[i].humanname = "human ";
         player[i].humanname += strrr( i );
         player[i].computername = "computer ";
         player[i].computername += strrr( i );
      }

      oldevents = NULL;
      firsteventtocome = NULL;
      firsteventpassed = NULL;

      unitnetworkid = 0;

      levelfinished = 0;

      network = 0;

      for ( i = 0; i < 8; i++ ) {
         cursorpos.position[i].cx = 0;
         cursorpos.position[i].sx = 0;
         cursorpos.position[i].cy = 0;
         cursorpos.position[i].sy = 0;
      }

      unsentmessage = NULL;
      message = NULL;
      messageid = 0;
      journal = NULL;
      newjournal = NULL;
      for ( i = 0; i< 8; i++ )
         alliances_at_beginofturn[i] = 0;

      shareview = NULL;
      continueplaying = 0;
      replayinfo = NULL;
      playerView = -1;
      lastjournalchange.abstime = 0;
      ellipse = 0;
      graphicset = 0;
      gameparameter_num = 0;
      game_parameter = NULL;
      mineralResourcesDisplayed = 0;
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
   bool loadCampaign = stream.readInt();
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

   int dummy_playername[9];
   for ( i = 0; i< 9; i++ ) {
      player[i].existent = stream.readChar();
      player[i].firstvehicle = NULL; stream.readInt(); // dummy
      player[i].firstbuilding = NULL; stream.readInt(); // dummy
      player[i].research.read ( stream );
      player[i].ai = (BaseAI*) stream.readInt();
      player[i].stat = stream.readChar();
      stream.readChar(); // dummy
      dummy_playername[i] = stream.readInt();
      player[i].passwordcrc.read ( stream );
      player[i].dissectedunit = (pdissectedunit) stream.readInt();
      player[i].unreadmessage = (pmessagelist) stream.readInt();
      player[i].oldmessage = (pmessagelist) stream.readInt();
      player[i].sentmessage = (pmessagelist) stream.readInt();
   }

   oldevents = NULL;

   //! practically dummy
   int loadeventstore = stream.readInt();
   int loadeventstocome = stream.readInt();
   int loadeventpassed = stream.readInt();

   unitnetworkid = stream.readInt();
   levelfinished = stream.readChar();
   network = (pnetwork) stream.readInt();
   bool alliance_names_not_used_any_more[8];
   for ( i = 0; i < 8; i++ )
      alliance_names_not_used_any_more[i] = stream.readInt(); // dummy

   for ( i = 0; i< 8; i++ ) {
      cursorpos.position[i].cx = stream.readWord();
      cursorpos.position[i].sx = stream.readWord();
      cursorpos.position[i].cy = stream.readWord();
      cursorpos.position[i].sy = stream.readWord();
   }

   bool loadtribute = stream.readInt();
   unsentmessage = (pmessagelist) stream.readInt();
   message = (pmessage) stream.readInt();

   messageid = stream.readInt();
   journal = (char*) stream.readInt();
   newjournal = (char*) stream.readInt();

   int exist_humanplayername[9];
   for ( i = 0; i < 8; i++ )
      exist_humanplayername[i] = stream.readInt();
   exist_humanplayername[8] = 0;


   int exist_computerplayername[9];
   for ( i = 0; i < 8; i++ )
      exist_computerplayername[i] = stream.readInt();
   exist_computerplayername[8] = 0;

   supervisorpasswordcrc.read ( stream );

   for ( i = 0; i < 8; i++ )
      alliances_at_beginofturn[i] = stream.readChar();

   stream.readInt(); // was objectcrc = (pobjectcontainercrcs)
   shareview = (pshareview) stream.readInt();

   continueplaying = stream.readInt();
   replayinfo = (treplayinfo*) stream.readInt();
   playerView = stream.readInt();
   lastjournalchange.abstime = stream.readInt();

   for ( i = 0; i< 8; i++ )
      bi_resource[i].read ( stream );

   int preferredfilenames = stream.readInt();

   ellipse = (EllipseOnScreen*) stream.readInt();
   graphicset = stream.readInt();
   gameparameter_num = stream.readInt();

   stream.readInt(); // dummy
   mineralResourcesDisplayed = stream.readInt();
   for ( i = 0; i< 9; i++ )
       player[i].queuedEvents = stream.readInt();

   for ( i = 0; i < 19; i++ )
       stream.readInt();

   int _oldgameparameter[8];
   for ( i = 0; i < 8; i++ )
       _oldgameparameter[i] = stream.readInt();

// return;

/////////////////////
// Here initmap was called
/////////////////////


    if ( title )
       stream.readpchar( &title );

    if ( loadCampaign ) {
       campaign = new Campaign;
       campaign->id = stream.readWord();
       campaign->prevmap = stream.readWord();
       campaign->player = stream.readChar();
       campaign->directaccess = stream.readChar();
       for ( int d = 0; d < 21; d++ )
          stream.readChar(); // dummy
    }

    for (char w=0; w<9 ; w++ ) {
       if (dummy_playername[w] )
          stream.readString();

      /*
       if (player[w].aiparams) {
          player[w].aiparams = new ( taiparams );
          stream.readdata2 ( *player[w].aiparams );
       }
      */
       player[w].ai = NULL;


       if ( exist_humanplayername[w] )
          player[w].humanname = stream.readString();

       if ( exist_computerplayername[w] )
          player[w].computername = stream.readString();

    } /* endfor */

    if ( stream.readInt() )
       tribute.read ( stream );

    for ( int aa = 0; aa < 8; aa++ )
       if ( alliance_names_not_used_any_more[aa] ) {
          char* tempname = NULL;
          stream.readpchar ( &tempname );
          delete[] tempname;
       }

    int h = stream.readInt();


/*
    if ( objectcrc ) {
       #ifdef logging
       logtofile ( "loaders / tspfldloaders::readmap / vor objectcrcs" );
       #endif

       objectcrc = new tobjectcontainercrcs;
       stream.readdata2 ( *objectcrc );

       #ifdef logging
       logtofile ( "loaders / tspfldloaders::readmap / vor unitcrcs" );
       #endif
       if ( objectcrc->unit.crcnum ) {
          objectcrc->unit.crc = new tcrc[objectcrc->unit.crcnum];
          stream.readdata ( objectcrc->unit.crc, objectcrc->unit.crcnum * sizeof ( tcrc ) );
       } else
          objectcrc->unit.crc = NULL;

       #ifdef logging
       logtofile ( "loaders / tspfldloaders::readmap / vor buildingcrcs" );
       #endif
       if ( objectcrc->building.crcnum ) {
          objectcrc->building.crc = new tcrc[objectcrc->building.crcnum];
          stream.readdata ( objectcrc->building.crc, objectcrc->building.crcnum * sizeof ( tcrc ) );
       } else
          objectcrc->building.crc = NULL;

       #ifdef logging
       logtofile ( "loaders / tspfldloaders::readmap / vor object.crcs" );
       #endif
       if ( objectcrc->object.crcnum ) {
          objectcrc->object.crc = new tcrc[objectcrc->object.crcnum];
          stream.readdata ( objectcrc->object.crc, objectcrc->object.crcnum * sizeof ( tcrc ) );
       } else
          objectcrc->object.crc = NULL;

       #ifdef logging
       logtofile ( "loaders / tspfldloaders::readmap / vor terraincrcs" );
       #endif
       if ( objectcrc->terrain.crcnum ) {
          objectcrc->terrain.crc = new tcrc[objectcrc->terrain.crcnum];
          stream.readdata ( objectcrc->terrain.crc, objectcrc->terrain.crcnum * sizeof ( tcrc ) );
       } else
          objectcrc->terrain.crc = NULL;

       #ifdef logging
       logtofile ( "loaders / tspfldloaders::readmap / vor techcrcs" );
       #endif
       if ( objectcrc->technology.crcnum ) {
          objectcrc->technology.crc = new tcrc[objectcrc->technology.crcnum];
          stream.readdata ( objectcrc->technology.crc, objectcrc->technology.crcnum * sizeof ( tcrc ) );
       } else
          objectcrc->technology.crc = NULL;

       #ifdef logging
       logtofile ( "loaders / tspfldloaders::readmap / vor speedcrccheck" );
       #endif
       objectcrc->speedcrccheck = new tspeedcrccheck ( objectcrc );

    }
*/

    if ( shareview ) {
       shareview = new tshareview;
       stream.readdata2 ( *(shareview) );
    }

    if ( preferredfilenames ) {
       int p;
       int mapname[8];
       int mapdescription_not_used_any_more[8];
       int savegame[8];
       int savegamedescription_not_used_any_more[8];
       for ( p = 0; p < 8; p++ )
          mapname[p] = stream.readInt();
       for ( p = 0; p < 8; p++ )
          mapdescription_not_used_any_more[p] = stream.readInt();
       for ( p = 0; p < 8; p++ )
          savegame[p] = stream.readInt();
       for ( p = 0; p < 8; p++ )
          savegamedescription_not_used_any_more[p] = stream.readInt();

       for ( int i = 0; i < 8; i++ ) {
          if ( mapname[i] )
             preferredFileNames.mapname[i] = stream.readString ();

          if ( mapdescription_not_used_any_more[i] )
             stream.readString(); // dummy

          if ( savegame[i] )
             preferredFileNames.savegame[i] = stream.readString ();

          if ( savegamedescription_not_used_any_more[i] )
             stream.readString();
       }
    }

    if ( ellipse ) {
       ellipse = new EllipseOnScreen;
       stream.readdata2 ( *(ellipse) );
    }

    int orggpnum = gameparameter_num;
    gameparameter_num = 0;
    for ( int gp = 0; gp < 8; gp ++ )
       setgameparameter ( gp, _oldgameparameter[gp] );

    for ( int ii = 0 ; ii < orggpnum; ii++ ) {
       int gpar;
       stream.readdata2 ( gpar );
       setgameparameter ( ii, gpar );
    }

       #ifdef logging
       logtofile ( "loaders / tspfldloaders::readmap / returning" );
       #endif



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
      stream.writeInt( 0 );
      player[i].passwordcrc.write ( stream );
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

   stream.writeInt( -1 );
   stream.writeInt( unsentmessage != NULL );
   stream.writeInt( message != NULL );

   stream.writeInt( messageid );
   stream.writeInt( journal != NULL );
   stream.writeInt( newjournal != NULL );

   for ( i = 0; i < 8; i++ )
      stream.writeInt( !player[i].humanname.empty() );

   for ( i = 0; i < 8; i++ )
      stream.writeInt( !player[i].computername.empty() );

   supervisorpasswordcrc.write ( stream );

   for ( i = 0; i < 8; i++ )
      stream.writeChar( alliances_at_beginofturn[i] );

   stream.writeInt( 0 );
   stream.writeInt( shareview != NULL );

   stream.writeInt( continueplaying );
   stream.writeInt( replayinfo != NULL );
   stream.writeInt( playerView );
   stream.writeInt( lastjournalchange.abstime );

   for ( i = 0; i< 8; i++ )
      bi_resource[i].write ( stream );

   stream.writeInt( 1 );
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
       stream.writeInt( -2 );


///////////////////
// second part
//////////////////



       if ( title )
          stream.writepchar( title );

       if ( campaign ) {
          stream.writeWord( campaign->id );
          stream.writeWord( campaign->prevmap );
          stream.writeChar( campaign->player );
          stream.writeChar( campaign->directaccess );
          for ( int d = 0; d < 21; d++ )
             stream.writeChar(0); // dummy
       }

       for (int w=0; w<8 ; w++ ) {

          // if (player[w].name)
          //    stream.writepchar ( player[w].name );


         // if (player[w].ai)
         //    stream.writedata2 ( *player[w].aiparams );


          if ( !player[w].humanname.empty() )
             stream.writeString ( player[w].humanname );

          if ( !player[w].computername.empty() )
             stream.writeString ( player[w].computername );
       }

       #ifdef logging
       logtofile ( "loaders / tspfldloaders::writemap / names written" );
       #endif

       if ( !tribute.empty() ) {
           stream.writeInt ( -1 );
           tribute.write ( stream );
       } else
           stream.writeInt ( 0 );

        /*
        for ( int bb = 0; bb < 8; bb++ )
           if ( alliance_names_not_used_any_more[bb] ) {
              char nl = 0;
              stream.writedata2 ( nl );
           }
        */

        int h = 0;
        stream.writedata2 ( h );


       #ifdef logging
       logtofile ( "loaders / tspfldloaders::writemap / vor crc" );
       #endif

        if ( shareview )
           stream.writedata2 ( *(shareview) );

       #ifdef logging
       logtofile ( "loaders / tspfldloaders::writemap / shareview written" );
       #endif

        int p;
        for ( p = 0; p < 8; p++ )
           stream.writeInt( 1 );

        for ( p = 0; p < 8; p++ )
           stream.writeInt( 0 );

        for ( p = 0; p < 8; p++ )
           stream.writeInt( 1 );

        for ( p = 0; p < 8; p++ )
           stream.writeInt( 0 );

        for ( int k = 0; k < 8; k++ ) {
           stream.writeString ( preferredFileNames.mapname[k] );
           stream.writeString ( preferredFileNames.savegame[k] );
        }


        if ( ellipse )
           stream.writedata2 ( *(ellipse) );

        for ( int ii = 0 ; ii < gameparameter_num; ii++ )
           stream.writedata2 ( game_parameter[ii] );




}


void tmap :: cleartemps( int b, int value )
{
  if ( xsize <= 0 || ysize <= 0)
     return;

  int l = 0;
  for ( int x = 0; x < xsize ; x++)
     for ( int y = 0; y <  ysize ; y++) {

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

   return player[playernum].getName().c_str();
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


void deletemessagelist ( pmessagelist list )
{
   if ( list ) {
      while ( list->prev )
         list = list->prev;

      while ( list->next )
         delete list->next;

      delete list;
   }
}


tmap :: ~tmap ()
{
   if ( field )
      for ( int l=0 ;l < xsize * ysize ; l++ ) {
         if ( field[l].bdt & cbbuildingentry ) {
            pbuilding aktbuilding = field[l].building;
            for ( int i=0; i<31 ; i++ )
               if (aktbuilding->loading[i])
                  delete aktbuilding->loading[i] ;

            delete aktbuilding ;
         }

         pvehicle aktvehicle = field[l].vehicle;
         if ( aktvehicle ) {
            for ( int i=0; i<31 ; i++ )
               if (aktvehicle->loading[i])
                  delete aktvehicle->loading[i] ;

            delete aktvehicle;
         }
      } /* endfor */

   int i;
   for ( i = 0; i <= 8; i++) {
      if ( player[i].ai ) {
         delete player[i].ai;
         player[i].ai = NULL;
      }
   }


   pevent       event;
   pevent       event2;

   event = firsteventtocome;
   while (event != NULL) {
      event2 = event;
      event = event->next;
      delete  event2;
   }
   firsteventtocome = NULL;

   event = firsteventpassed;
   while (event != NULL) {
      event2 = event;
      event = event->next;
      delete event2;
   }
   firsteventpassed = NULL;

   if ( journal ) {
      delete[] journal;
      journal = NULL;
   }
   if ( newjournal ) {
      delete[] newjournal;
      newjournal = NULL;
   }

   if ( shareview ) {
      delete shareview;
      shareview = NULL;
   }

   if ( replayinfo ) {
      delete replayinfo;
      replayinfo = NULL;
   }

   if ( game_parameter ) {
      delete[] game_parameter;
      game_parameter = NULL;
   }

   if ( field ) {
      delete[] field;
      field = NULL;
   }


   /****************************************/
   /*        Messages l”schen            ÿ */
   /****************************************/

   pmessage msg = message;
   while ( msg ) {
      pmessage temp = msg->next;
      delete msg;
      msg= temp;
   }

   for ( i = 0; i < 8; i++ ) {
      deletemessagelist ( player[ i ].sentmessage ) ;
      player[ i ].sentmessage = NULL;

      deletemessagelist ( player[ i ].unreadmessage );
      player[ i ].unreadmessage = NULL;

      deletemessagelist ( player[ i ].oldmessage );
      player[ i ].oldmessage = NULL;
   }
   deletemessagelist ( unsentmessage );
   unsentmessage = NULL;



   /****************************************/
   /*     Sezierungen l”schen            ÿ */
   /****************************************/

   for ( i = 0; i < 8; i++ ) {
      pdissectedunit du = player[ i ].dissectedunit;
      while ( du ) {
         pdissectedunit du2 = du->next;
         delete du;
         du = du2;
      }
   }
}

/*
gamemap :: ResourceTribute :: tresourcetribute ( )
{
   for ( int a = 0; a < 3; a++ )
      for ( int b = 0; b < 8; b++ )
         for ( int c = 0; c < 8; c++ ) {
            avail.resource[a][b][c] = 0;
            paid.resource[a][b][c] = 0;
         }
}
*/

bool tmap :: ResourceTribute :: empty ( )
{
   for (int i = 0; i < 8; i++)
      for (int j = 0; j < 8; j++)
         for (int k = 0; k < 3; k++) {
            if ( avail[i][j].resource(k) )
               return true;
            if ( paid[i][j].resource(k) )
               return true;
         }

   return false;
}

void tmap :: ResourceTribute :: read ( tnstream& stream )
{
   int a,b,c;
   for ( a = 0; a < 3; a++ )
      for ( b = 0; b < 8; b++ )
         for ( c = 0; c < 8; c++ )
             avail[b][c].resource(a) = stream.readInt();
   for ( a = 0; a < 3; a++ )
      for ( b = 0; b < 8; b++ )
         for ( c = 0; c < 8; c++ )
             paid[b][c].resource(a) = stream.readInt();
}

void tmap :: ResourceTribute :: write ( tnstream& stream )
{
   int a,b,c;
   for ( a = 0; a < 3; a++ )
      for ( b = 0; b < 8; b++ )
         for ( c = 0; c < 8; c++ )
             stream.writeInt ( avail[b][c].resource(a) );

   for ( a = 0; a < 3; a++ )
      for ( b = 0; b < 8; b++ )
         for ( c = 0; c < 8; c++ )
             stream.writeInt ( paid[b][c].resource(a) );
}
