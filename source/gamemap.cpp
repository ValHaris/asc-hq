/*! \file gamemap.cpp
    \brief Implementation of THE central asc class: tmap 
*/

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

#include <algorithm>
#include "global.h"
#include "misc.h"
#include "typen.h"
#include "vehicletype.h"
#include "buildingtype.h"
#include "spfst.h"
#include "dlg_box.h"
#include "dialog.h"
#include "itemrepository.h"

#ifdef sgmain
 #include "network.h"
 #include "gameoptions.h"
 #include "missions.h"
#endif





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
      player[i].ai = NULL;

      if ( i == 0 )
         player[i].stat = Player::human;
      else
         player[i].stat = Player::off;

      player[i].queuedEvents = 0;
      player[i].humanname = "human ";

      player[i].humanname += strrr( i );
      player[i].computername = "computer ";
      player[i].computername += strrr( i );
      player[i].research.chainToMap ( this, i );
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
      player[i].existanceAtBeginOfTurn = stream.readChar();
      stream.readInt(); // dummy
      stream.readInt(); // dummy
      player[i].research.read_struct ( stream );
      player[i].ai = (BaseAI*) stream.readInt();
      player[i].stat = Player::tplayerstat ( stream.readChar() );
      stream.readChar(); // dummy
      dummy_playername[i] = stream.readInt();
      player[i].passwordcrc.read ( stream );
      player[i].__dissectionsToLoad = stream.readInt();
      player[i].__loadunreadmessage = stream.readInt();
      player[i].__loadoldmessage = stream.readInt();
      player[i].__loadsentmessage = stream.readInt();
   }

   oldevents = NULL;

   //! practically dummy
   loadOldEvents = stream.readInt();
   stream.readInt(); // int loadeventstocome
   stream.readInt(); // int loadeventpassed

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

   stream.readInt(); // loadtribute
   __loadunsentmessage = stream.readInt();
   __loadmessages = stream.readInt();

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
   bool load_shareview = stream.readInt();

   continueplaying = stream.readInt();
   __loadreplayinfo =  stream.readInt();
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

    stream.readInt();


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

    if ( load_shareview ) {
       shareview = new tmap::Shareview;
       for ( int i = 0; i < 8; i++ )
          for ( int j = 0; j < 8; j++ )
             shareview->mode[i][j] = stream.readChar();
       shareview->recalculateview = stream.readInt();
    } else
       shareview = NULL;

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
      stream.writeChar( player[i].existanceAtBeginOfTurn );
      stream.writeInt( 1 ); // dummy
      stream.writeInt( 1 ); // dummy
      player[i].research.write_struct ( stream );
      stream.writeInt( player[i].ai != NULL );
      stream.writeChar( player[i].stat );
      stream.writeChar( 0 ); // dummy
      stream.writeInt( 0 );
      player[i].passwordcrc.write ( stream );
      stream.writeInt( !player[i].dissections.empty() );
      stream.writeInt( 1 );
      stream.writeInt( 1 );
      stream.writeInt( 1 );
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
   stream.writeInt( 1 );
   stream.writeInt( !messages.empty() );

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
       stream.writeInt( getgameparameter(i) );


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

        if ( shareview )
           stream.writedata2 ( *(shareview) );

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



void tmap :: calculateAllObjects ( void )
{
   calculateallobjects();
}

pfield  tmap :: getField(int x, int y)
{
   if ((x < 0) || (y < 0) || (x >= xsize) || (y >= ysize))
      return NULL;
   else
      return (   &field[y * xsize + x] );
}

pfield  tmap :: getField(const MapCoordinate& pos )
{
   return getField ( pos.x, pos.y );
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
   for ( int n = 0; n< 8; n++ ) {
      bi_resource[n].energy = 0;
      bi_resource[n].material = 0;
      bi_resource[n].fuel = 0;

     #ifdef sgmain

      for ( Player::BuildingList::iterator i = player[n].buildingList.begin(); i != player[n].buildingList.end(); i++ )
         for ( int r = 0; r < 3; r++ )
            if ( isResourceGlobal( r )) {
               bi_resource[n].resource(r) += (*i)->actstorage.resource(r);
               (*i)->actstorage.resource(r) = 0;
            }
     #endif
   }
}

/*

void tmap :: chainunit ( pvehicle eht )
{
   if ( eht ) {
      for ( int i = 0; i< 32; i++ )
         if ( eht->loading[i] == eht )
            fatalError ( "tmap::chainunit - unit carries itself as cargo !\nposition is %d / %d", eht->xpos, eht->ypos );



      eht->next = player[ eht->color / 8 ].firstvehicle;
      if ( eht->next )
         eht->next->prev = eht;
      eht->prev = NULL;
      player[ eht->color / 8 ].firstvehicle = eht;
      if ( eht->typ->loadcapacity > 0)
         for ( int i = 0; i <= 31; i++)
            if ( eht->loading[i] )
               if ( eht->loading[i] == eht )
                  fatalError ( "tmap::chainunit - unit carries itself as cargo !\nposition is %d / %d", eht->xpos, eht->ypos );
               else
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
      bld->chainToMap ( this );
   }
}

*/

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
   for ( int p = 0; p < 9; p++ )
      for ( Player::VehicleList::iterator i = player[p].vehicleList.begin(); i != player[p].vehicleList.end(); i++ )
         if ( (*i)->networkid == nwid )
            return *i;

   return NULL;
}


pvehicle tmap :: getUnit ( int x, int y, int nwid )
{
   pfield fld  = getField ( x, y );
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
}




tmap :: ~tmap ()
{
   if ( field )

      for ( int l=0 ;l < xsize * ysize ; l++ ) {
         if ( (field[l].bdt & getTerrainBitType(cbbuildingentry)).any() )
            delete field[l].building;


         pvehicle aktvehicle = field[l].vehicle;
         if ( aktvehicle )
            delete aktvehicle;

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

   if ( actmap == this )
      actmap = NULL;
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



int  tmap::resize( int top, int bottom, int left, int right )  // positive: larger
{ 
  if ( !top && !bottom && !left && !right )
     return 0;

  if ( -(top + bottom) > ysize )
     return 1;

  if ( -(left + right) > xsize )
     return 2;

  if ( bottom & 1 || top & 1 )
     return 3;

  int ox1, oy1, ox2, oy2;

  if ( top < 0 ) {
     for ( int x = 0; x < xsize; x++ )
        for ( int y = 0; y < -top; y++ )
           getField(x,y)->deleteeverything();

     oy1 = -top;
  } else
     oy1 = 0;

  if ( bottom < 0 ) {
     for ( int x = 0; x < xsize; x++ )
        for ( int y = ysize+bottom; y < ysize; y++ )
           getField(x,y)->deleteeverything();

     oy2 = ysize + bottom;
  } else
     oy2 = ysize;
   
  if ( left < 0 ) {
     for ( int x = 0; x < -left; x++ )
        for ( int y = 0; y < ysize; y++ )
           getField(x,y)->deleteeverything();
     ox1 = -left;
  } else
     ox1 = 0;

  if ( right < 0 ) {
     for ( int x = xsize+right; x < xsize; x++ )
        for ( int y = 0; y < ysize; y++ )
           getField(x,y)->deleteeverything();
     ox2 = xsize + right;
  } else
     ox2 = xsize;


  int newx = xsize + left + right;
  int newy = ysize + top + bottom;

  pfield newfield = new tfield [ newx * newy ];

  int x;
  for ( x = ox1; x < ox2; x++ )
     for ( int y = oy1; y < oy2; y++ ) {
        pfield org = getField ( x, y );
        pfield dst = &newfield[ (x + left) + ( y + top ) * newx];
        *dst = *org;
     }

  tfield defaultfield;
  defaultfield.typ = getterraintype_byid ( 30 )->weather[0];

  for ( x = 0; x < left; x++ )
     for ( int y = 0; y < newy; y++ )
        newfield[ x + y * newx ] = defaultfield;

  for ( x = xsize + left; x < xsize + left + right; x++ )
     for ( int y = 0; y < newy; y++ )
        newfield[ x + y * newx ] = defaultfield;


  int y;
  for ( y = 0; y < top; y++ )
     for ( int x = 0; x < newx; x++ )
        newfield[ x + y * newx ] = defaultfield;

  for ( y = ysize + top; y < ysize + top + bottom; y++ )
     for ( int x = 0; x < newx; x++ )
        newfield[ x + y * newx ] = defaultfield;

  calculateallobjects();

  for ( int p = 0; p < newx*newy; p++ )
     newfield[p].setparams();

  delete[] field;
  field = newfield;
  xsize = newx;
  ysize = newy;

  /*
  if (xpos + idisplaymap.getscreenxsize() > xsize)
     xpos = xsize - idisplaymap.getscreenxsize() ;
  if (ypos + idisplaymap.getscreenysize()  > ysize)
     ypos = ysize - idisplaymap.getscreenysize() ;
   */

  return 0;
}

bool tmap::Player::exist()
{
  return !(buildingList.empty() && vehicleList.empty());
}

pterraintype tmap :: getterraintype_byid ( int id )
{
   return getterraintype_forid ( id );
}

pobjecttype tmap :: getobjecttype_byid ( int id )
{
   return getobjecttype_forid ( id );
}

pvehicletype tmap :: getvehicletype_byid ( int id )
{
   return getvehicletype_forid ( id );
}

pbuildingtype tmap :: getbuildingtype_byid ( int id )
{
   return getbuildingtype_forid ( id );
}

ptechnology tmap :: gettechnology_byid ( int id )
{
   return gettechnology_forid ( id );
}


pterraintype tmap :: getterraintype_bypos ( int pos )
{
   return getterraintype_forpos ( pos );
}

pobjecttype tmap :: getobjecttype_bypos ( int pos )
{
   return getobjecttype_forpos ( pos );
}

pvehicletype tmap :: getvehicletype_bypos ( int pos )
{
   return getvehicletype_forpos ( pos );
}

pbuildingtype tmap :: getbuildingtype_bypos ( int pos )
{
   return getbuildingtype_forpos ( pos );
}

ptechnology tmap :: gettechnology_bypos ( int pos )
{
   return gettechnology_forpos ( pos );
}

int tmap :: getTerrainTypeNum ( )
{
   return  terraintypenum;
}

int tmap :: getObjectTypeNum ( )
{
   return  vehicletypenum;
}

int tmap :: getVehicleTypeNum ( )
{
   return  buildingtypenum;
}

int tmap :: getBuildingTypeNum ( )
{
   return  technologynum;
}

int tmap :: getTechnologyNum ( )
{
   return  objecttypenum;
}

void tmap :: startGame ( )
{ 
   time.a.turn = 1;
   time.a.move = 0;

   for ( int j = 0; j < 8; j++ )
      player[j].queuedEvents = 1;
   #ifndef karteneditor
   getnexteventtime();
   #endif

   levelfinished = false;
   firsteventpassed = NULL;
   network = NULL;
   int num = 0;
   int cols[72];

   memset ( cols, 0, sizeof ( cols ));
   int i;
   for ( i = 0; i < 8 ; i++) {
      if ( player[i].exist() ) {
         num++;
         cols[ i * 8 ] = 1;
      } else
         cols[ i * 8 ] = 0;

      cursorpos.position[ i ].sx = 0;
      cursorpos.position[ i ].sy = 0;

   }

   i = 0;                                                                        
   int sze = xsize * ysize;
   do {
      if ( field[i].vehicle ) 
         if ( cols[ field[i].vehicle->color] ) {
            cursorpos.position[ field[i].vehicle->color / 8 ].cx = field[i].vehicle->xpos;
            cursorpos.position[ field[i].vehicle->color / 8 ].cy = field[i].vehicle->ypos;
            num--;
            cols[ field[i].vehicle->color] = 0;
         }

      if ( field[i].building && field[i].building->color < 64 ) 
         if ( cols[ field[i].building->color] ) {
            cursorpos.position[ field[i].building->color / 8 ].cx = field[i].building->getEntry().x;
            cursorpos.position[ field[i].building->color / 8 ].cy = field[i].building->getEntry().y;
            num--;
            cols[ field[i].building->color] = 0;
         }
      i++;
   } while ( num   &&   i <= sze ); /* enddo */


   for ( int n = 0; n< 8; n++ ) {
      bi_resource[n].energy = 0;
      bi_resource[n].material = 0;
      bi_resource[n].fuel = 0;
   }


   #ifndef karteneditor
   actplayer = -1;
   #else
   actplayer = 0;
   #endif
} 


void tmap::operator= ( const tmap& map )
{
  throw ASCmsgException ( "tmap::operator= undefined");
}

bool Mine :: attacksunit ( const pvehicle veh )
{
     if  (!( ( veh->functions & cfmineimmune ) || 
              ( veh->height > chfahrend ) ||
              ( getdiplomaticstatus2 ( veh->color, color*8 ) == capeace ) || 
              ( (veh->functions & cf_trooper) && (type != cmantipersonnelmine)) || 
              ( veh->height <= chgetaucht && type != cmmooredmine ) || 
              ( veh->height == chschwimmend && type != cmfloatmine ) ||
              ( veh->height == chfahrend && type != cmantipersonnelmine  && type != cmantitankmine )
            ))
         return true;
     return false;
}


tfield :: tfield ( )
{
   bdt.set ( 0 );
   typ = NULL;
   picture = NULL;
   vehicle = NULL;
   building = NULL;
   a.temp = 0;
   a.temp2 = 0;
   visible = 0;
   direction = 0;
   fuel = 0;
   material = 0;
   resourceview = NULL;
   connection = 0;
}


void tfield::operator= ( const tfield& f )
{
   typ = f.typ;
   fuel = f.fuel;
   material = f.material;
   visible = f.visible;
   direction = f.direction;
   tempw = f.tempw;
   temp3 = f.temp3;
   temp4 = f.temp4;
   vehicle = f.vehicle;
   building = f.building;
   if ( f.resourceview ) {
      resourceview = new Resourceview;
      *resourceview = *f.resourceview;
   } else
      resourceview = NULL;
   mines = f.mines;
   objects = f.objects;
   connection = f.connection;
   bdt = f.bdt;
   for ( int i = 0; i < 8; i++ )
      view[i] = f.view[i];
}

void tfield :: checkminetime ( int time )
{
   for ( MineContainer::iterator m = mines.begin(); m != mines.end();  ) {
      int lt = actmap->getgameparameter ( cgp_antipersonnelmine_lifetime + m->type - 1);
      if ( lt && m->time + lt < time )
         m = mines.erase( m );
      else
         m++;
   }
}


int tfield :: mineattacks ( const pvehicle veh )
{
   int i = 1;
   for ( MineContainer::iterator m = mines.begin(); m != mines.end(); m++, i++ )
      if ( m->attacksunit ( veh ))
         return i;

   return 0;
}

Mine& tfield::getMine ( int n )
{
  int c = 0;
  MineContainer::iterator i;
  for ( i = mines.begin(); c < n; i++,c++ );
  return *i;
}

void  tfield :: addobject( pobjecttype obj, int dir, int force )
{ 
   if ( !obj )
      return;

   pobject i = checkforobject ( obj );
   if ( !i ) {
     int buildable = obj->buildable ( this );
     #ifdef karteneditor
     if ( !buildable ) 
          if ( force )
             buildable = 1;
          else
             if (choice_dlg("object cannot be built here","~i~gnoe","~c~ancel") == 1) 
                buildable = 1;
     #else
     if ( !buildable )
          if ( force )
             buildable = 1;
     #endif

     if ( buildable ) {
         Object o ( obj );
         o.time = actmap->time.a.turn;
         if ( dir != -1 )
            o.dir = dir;
         else
            o.dir = 0;

         objects.push_back ( o );

         setparams();

         if ( dir == -1 )
            calculateobject( getx(), gety(), true, obj ); 

         sortobjects();
     }
   } else {
      if ( dir != -1 ) 
         i->dir |= dir;
      
      sortobjects();
   } 
} 


void tfield :: removeobject( pobjecttype obj )
{ 
   if ( building ) 
      return;

   #ifndef karteneditor
   if ( vehicle ) 
      if ( vehicle->color != actmap->actplayer << 3)
        return;
   #endif

   for ( ObjectContainer::iterator o = objects.begin(); o != objects.end(); )
      if ( o->typ == obj )
         o = objects.erase( o );
      else
         o++;

   setparams();
   calculateobject( getx(), gety(), true, obj );
} 

void tfield :: deleteeverything ( void )
{
   if ( vehicle ) {
      delete vehicle;
      vehicle = NULL;
   }

   if ( building ) {
      delete building;
      building = NULL;
   }

   setparams();
}


bool tfield :: unitHere ( const pvehicle veh )
{
   if ( vehicle == veh )
      return true;

   if ( vehicle && veh && vehicle->networkid == veh->networkid )
      return true;
   return false;
}

int tfield :: getweather ( void )
{
   if ( !typ )
      return 0;
   for ( int w = 0; w < cwettertypennum; w++ )
      if ( typ == typ->terraintype->weather[w] )
         return w;
   return -1;
}

bool compareObjectHeight ( const Object& o1, const Object& o2 )
{
   return o1.typ->height > o2.typ->height;
}

void tfield :: sortobjects ( void )
{
   sort ( objects.begin(), objects.end(), compareObjectHeight );
}


bool  tfield :: putmine( int col, int typ, int strength )
{
   if ( mineowner() >= 0  && mineowner() != col )
      return 0;

   if ( mines.size() >= actmap->getgameparameter ( cgp_maxminesonfield ))
      return 0;

   Mine m;
   m.strength = strength ;
   m.color = col;
   m.type = typ;
   if ( actmap && actmap->time.a.turn>= 0 )
      m.time = actmap->time.a.turn;
   else
      m.time = 0;

   mines.push_back ( m );
   return 1;
}

int tfield :: mineowner( void )
{
   if ( mines.empty() )
      return -1;
   else
      return mines.begin()->color;
}


void tfield :: removemine( int num )
{ 
   if ( num == -1 )
      num = mines.size() - 1;

   int i = 0;
   for ( MineContainer::iterator m = mines.begin(); m != mines.end(); i++)
      if ( i == num )
         m = mines.erase ( m );
      else
          m++;
}


int tfield :: getx( void )
{
   int n = this - actmap->field;
   return n % actmap->xsize;
}

int tfield :: gety( void )
{
   int n = this - actmap->field;
   return n / actmap->xsize;
}


int tfield :: getattackbonus ( void )
{
   int a = typ->attackbonus;
   for ( ObjectContainer::iterator o = objects.begin(); o != objects.end(); o++ ) {
      if ( o->typ->attackbonus_abs != -1 )
         a = o->typ->attackbonus_abs;
      else
         a += o->typ->attackbonus_plus;
   }

   if ( a > -8 )
      return a;
   else
      return -7;
}

int tfield :: getdefensebonus ( void )
{
   int a = typ->defensebonus;
   for ( ObjectContainer::iterator o = objects.begin(); o != objects.end(); o++ ) {
      if ( o->typ->defensebonus_abs != -1 )
         a = o->typ->defensebonus_abs;
      else
         a += o->typ->defensebonus_plus;
   }

   if ( a > -8 )
      return a;
   else
      return -7;
}

int tfield :: getjamming ( void )
{
   int a = typ->basicjamming;
   for ( ObjectContainer::iterator o = objects.begin(); o != objects.end(); o++ ) {
      if ( o->typ->basicjamming_abs >= 0 )
         a = o->typ->basicjamming_abs;
      else
         a += o->typ->basicjamming_plus;
   }
   return a;
}

int tfield :: getmovemalus ( const pvehicle veh )
{       
   int mnum = mines.size();
   if ( mnum ) {
      int movemalus = _movemalus[veh->typ->movemalustyp];
      int col = mineowner();
      if ( veh->color == col*8 ) 
         movemalus += movemalus * mine_movemalus_increase * mnum / 100;

      return movemalus;
   } else
      return _movemalus[veh->typ->movemalustyp];
}

int tfield :: getmovemalus ( int type )
{
  return _movemalus[type];
}

void tfield :: setparams ( void )
{
   int i;
   bdt = typ->art;

   for ( i = 0; i < cmovemalitypenum; i++ )
      _movemalus[i] = typ->move_malus[i];

   for ( ObjectContainer::iterator o = objects.begin(); o != objects.end(); o++ ) {
      bdt  &=  o->typ->terrain_and;
      bdt  |=  o->typ->terrain_or;

      for ( i = 0; i < cmovemalitypenum; i++ ) {
         _movemalus[i] += o->typ->movemalus_plus[i];
         if ( (o->typ->movemalus_abs[i] != 0) && (o->typ->movemalus_abs[i] != -1) )
            _movemalus[i] = o->typ->movemalus_abs[i];
         if ( _movemalus[i] < minmalq )
            _movemalus[i] = minmalq;
      }
   }

   if ( building ) {
      if ( this == building->getField( building->typ->entry ))
         bdt |= getTerrainBitType(cbbuildingentry);

      if ( building )
         for (int x = 0; x < 4; x++)
            for ( int y = 0; y < 6; y++ )
               if ( building->getField ( BuildingType::LocalCoordinate(x, y) ) == this )
                  if ( building->getpicture ( BuildingType::LocalCoordinate(x, y) ) )
                     picture = building->getpicture ( BuildingType::LocalCoordinate(x, y) );
   }
}


tfield :: ~tfield()
{
   if ( resourceview ) {
      delete resourceview;
      resourceview = NULL;
   }
}


Object :: Object ( void )
{
   typ = NULL;
   dir = 0;
   damage = 0;
}

Object :: Object ( pobjecttype t )
{
   typ = t;
   dir = 0;
   damage = 0;
}


void Object :: setdir ( int direc )
{
   dir = direc;
}

int  Object :: getdir ( void )
{
   return dir;
}

void Object :: display ( int x, int y, int weather )
{
  if ( typ->id == 7 ) {
        if ( dir < typ->weatherPicture[weather].images.size() )
           putshadow  ( x, y,  typ->getpic ( dir, weather ) , &xlattables.a.dark1);
        else
           putshadow  ( x, y,  typ->getpic ( 0, weather ) , &xlattables.a.dark1);
  } else
     if ( typ->id == 30 ) {   // pipeline
           if ( dir < typ->weatherPicture[weather].images.size() )
              putshadow  ( x, y,  typ->getpic ( dir, weather ) , &xlattables.a.dark1);
           else
              putshadow  ( x, y,  typ->getpic ( 0, weather ) , &xlattables.a.dark1);
     } else
        typ->display ( x, y, dir, weather );
}


pobject tfield :: checkforobject ( pobjecttype o )
{
   for ( ObjectContainer::iterator i = objects.begin(); i != objects.end(); i++ )
      if ( i->typ == o )
         return &(*i);

   return NULL;
}



tfield::Resourceview :: Resourceview ( void )
{
   visible = 0;
   memset ( &fuelvisible, 0, sizeof ( fuelvisible ));
   memset ( &materialvisible, 0, sizeof ( materialvisible ));
}

bool tmap::Weather::Wind::operator== ( const tmap::Weather::Wind& b ) const
{

   return ( (speed == b.speed) && ( direction == b.direction));
}

tmap::Shareview :: Shareview ( const tmap::Shareview* org )
{
   memcpy ( mode, org->mode, sizeof ( mode ));
   recalculateview = org->recalculateview;
}

void AiThreat :: write ( tnstream& stream )
{
   const int version = 1000;
   stream.writeInt ( version );
   stream.writeInt ( threatTypes );
   for ( int i = 0; i < threatTypes; i++ )
      stream.writeInt ( threat[i] );
}

void AiThreat:: read ( tnstream& stream )
{

   int version = stream.readInt();
   if ( version == 1000 ) {
      threatTypes = stream.readInt();
      for ( int i = 0; i < threatTypes; i++ )
         threat[i] = stream.readInt();
   }
}


void AiValue :: write ( tnstream& stream )
{
   const int version = 2000;
   stream.writeInt ( version );
   stream.writeInt ( value );
   stream.writeInt ( addedValue );
   threat.write ( stream );
   stream.writeInt ( valueType );
}

void AiValue:: read ( tnstream& stream )
{
   int version = stream.readInt();
   if ( version == 2000 ) {
      value = stream.readInt (  );
      addedValue= stream.readInt (  );
      threat.read ( stream );
      valueType = stream.readInt (  );
   }
}

void AiParameter::write ( tnstream& stream )
{
   const int version = 3001;
   stream.writeInt ( version );
   stream.writeInt ( lastDamage );
   stream.writeInt ( damageTime.abstime );
   stream.writeInt ( dest.x );
   stream.writeInt ( dest.y );
   stream.writeInt ( dest.z );
   stream.writeInt ( dest_nwid );
   stream.writeInt ( data );
   AiValue::write( stream );
   stream.writeInt ( task );
   stream.writeInt ( jobPos );
   stream.writeInt ( jobs.size() );
   for ( int i = 0; i < jobs.size(); i++ )
      stream.writeInt( jobs[i] );
}

void AiParameter::read ( tnstream& stream )
{
   int version = stream.readInt();
   if ( version >= 3000 && version <= 3001 ) {
      lastDamage = stream.readInt();
      damageTime.abstime = stream.readInt();
      dest.x = stream.readInt();
      dest.y = stream.readInt();
      dest.z = stream.readInt();
      dest_nwid = stream.readInt();
      data = stream.readInt();
      AiValue::read( stream );
      task = (Task) stream.readInt();
      if ( version == 3000 ) {
         jobs.clear();
         jobs.push_back ( Job( stream.readInt() ));
      } else {
         jobPos = stream.readInt();
         int num = stream.readInt();
         jobs.clear();
         for ( int i = 0; i < num; i++ )
            jobs.push_back ( Job( stream.readInt() ));
      }

   }
}

void AiThreat :: reset ( void )
{
   for ( int i = 0; i < aiValueTypeNum; i++ )
      threat[i] = 0;
}

AiParameter :: AiParameter ( pvehicle _unit ) : AiValue ( log2( _unit->height ))
{
   reset( _unit );
}

void AiParameter :: resetTask ( )
{
   dest.x = -1;
   dest.y = -1;
   dest.z = -1;
   dest_nwid = -1;
   task = tsk_nothing;
}

void AiParameter::addJob ( Job j, bool front )
{
   if ( front )
      jobs.insert ( jobs.begin(), j );
   else
      jobs.push_back ( j );
}

void AiParameter::setJob ( const JobList& jobs )
{
   this->jobs = jobs;
}


void AiParameter :: reset ( pvehicle _unit )
{
   unit = _unit;
   AiValue::reset ( log2( _unit->height ) );

   clearJobs();
   resetTask();
}

void AiParameter :: setNextJob()
{
   jobPos++;
}

void AiParameter :: restartJobs()
{
   jobPos = 0;
}

void AiParameter :: clearJobs()
{
   jobs.clear();
   jobPos = 0;
}



treplayinfo :: treplayinfo ( void )
{
   for (int i = 0; i < 8; i++) {
      guidata[i] = NULL;
      map[i] = NULL;
   }
   actmemstream = NULL;
}

treplayinfo :: ~treplayinfo ( )
{
   for (int i = 0; i < 8; i++)  {
      if ( guidata[i] ) {
         delete guidata[i];
         guidata[i] = NULL;
      }
      if ( map[i] ) {
         delete map[i];
         map[i] = NULL;
      }
  }
  if ( actmemstream ) {
     delete actmemstream ;
     actmemstream = NULL;
  }
}

