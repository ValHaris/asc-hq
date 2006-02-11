/*! \file loaders.cpp
    \brief procedure for loading and writing savegames, maps etc.

    IO for basic types like vehicletype, buildingtype etc which are also used by the small editors are found in sgstream

*/

/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2005  Martin Bickel  and  Marc Schellenberger

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; see the file COPYING. If not, write to the 
    Free Software Foundation, Inc., 59 Temple Place, Suite 330, 
    Boston, MA  02111-1307  USA
*/

#include <algorithm>
#include <cstring>
#include <stdlib.h>
#include <stdio.h>

#include "typen.h"
#include "misc.h"
#include "basegfx.h"
#include "newfont.h"
#include "buildingtype.h"
#include "vehicletype.h"
#include "spfst.h"
#include "loaders.h"
#include "dlg_box.h"
#include "dlg_box.h"
#include "dialog.h"
#include "sgstream.h"
#include "attack.h"
#include "errors.h"
#include "strtmesg.h"
#include "textfileparser.h"
#include "itemrepository.h"
#include "prehistoricevents.h"
#include "gamemap.h"

#ifdef sgmain
# include "ai/ai.h"
#endif


const int fileterminator = 0xa01a;


 const char* savegameextension = "*.sav";
 const char* mapextension = "*.map";
 const char* tournamentextension = ".ascpbm";



void         seteventtriggers( GameMap* actmap )
{
   #ifdef sgmain
   for ( GameMap::Events::iterator i = actmap->events.begin(); i != actmap->events.end(); ++i )
       (*i)->arm();

   #endif
   
   for ( int i = 0; i < 8; i++ )
      actmap->player[i].queuedEvents = 1;
} 



  #define csm_typid32 1      /*  b1  */
  #define csm_direction 2    /*  b1  */ 
  #define csm_vehicle 4      /*  b1  */
  #define csm_building 8     /*  b1  */
  #define csm_height 32      /*  b1  */
  #define csm_cnt2 64        /*  b1  */
  #define csm_b3 128         /*  b1  */

  #define csm_material 1     /*  b3  */
  #define csm_fuel 2         /*  b3  */
  #define csm_visible 4      /*  b3  */
//  #define csm_mine 8         /*  b3  */
  #define csm_weather 16     /*  b3  */
//  #define csm_fahrspur 32    /*  b3  */
  #define csm_object 64      /*  b3  */
  #define csm_b4 128         /* b3 */ 

  #define csm_resources  1     /* b4 */
  #define csm_connection 2     // b4
  #define csm_newobject  4     // b4







/**************************************************************/
/*     sezierungen schreiben / lesen                         */
/**************************************************************/


void   tspfldloaders::readdissections ( void )
{
   for (int i = 0; i < 8; i ++ ) {
      if ( spfld->player[ i ].__dissectionsToLoad ) {
         int k;
         do {
            Player::Dissection du;
            stream->readInt(); // dummy;
            stream->readInt(); // dummy;

            du.orgpoints = stream->readInt();
            du.points    = stream->readInt();
            du.num       = stream->readInt();

            k = stream->readInt();

            int j = stream->readInt();
            du.fzt = vehicleTypeRepository.getObject_byID ( j );
            if ( !du.fzt )
               throw InvalidID ( "vehicle", j );

            j = stream->readInt();
            du.tech = technologyRepository.getObject_byID  ( j );
            if ( !du.tech )
               throw InvalidID ( "technology", j );

            spfld->player[ i ].dissections.push_back ( du );
         } while ( k );
      }
   } /* endfor */
}

void   tspfldloaders::writedissections ( void )
{
   for (int i = 0; i < 8; i ++ ) {
      Player::DissectionContainer::iterator di = spfld->player[i].dissections.begin();
      while ( di != spfld->player[i].dissections.end() ) {
         stream->writeInt ( 1 ); // dummy
         stream->writeInt ( 1 ); // dummy
         stream->writeInt ( di->orgpoints );
         stream->writeInt ( di->points );
         stream->writeInt ( di->num );

         di++;
         if ( di != spfld->player[i].dissections.end() )
            stream->writeInt ( 1 );
         else
            stream->writeInt ( 0 );

         stream->writeInt ( di->fzt->id );
         stream->writeInt ( di->tech->id );
      }
   }
}



/**************************************************************/
/*        Messages  schreiben / lesen                         */
/**************************************************************/

const int messageVersion = 0xabcdef;

void      tspfldloaders:: writemessages ( void )
{
   stream->writeInt ( messageVersion );

   int id = 0;
   for ( MessageContainer::iterator mi = spfld->messages.begin(); mi != spfld->messages.end();  ) {
      id++;
      (*mi)->id = id;

      stream->writeInt ( (*mi)->from );
      stream->writeInt ( (*mi)->to );
      stream->writeInt ( (unsigned int) (*mi)->time );
      stream->writeInt ( 1 );
      stream->writeInt ( (*mi)->id );
      stream->writeInt ( (*mi)->gametime.turn() );
      stream->writeInt ( (*mi)->gametime.move() );


      ASCString& t = (*mi)->text;
      mi++;
      stream->writeInt ( mi != spfld->messages.end() ? 1 : 0 );

      stream->writeString ( t );
   }

   for ( int i = 0; i < 8; i++ )
      writemessagelist ( spfld->player[ i ].oldmessage );

   for ( int i = 0; i < 8; i++ )
      writemessagelist ( spfld->player[ i ].unreadmessage );

   for ( int i = 0; i < 8; i++ )
      writemessagelist ( spfld->player[ i ].sentmessage );

   writemessagelist ( spfld->unsentmessage );

   stream->writeInt ( messageVersion );

   stream->writeString ( spfld->gameJournal );
   stream->writeString ( spfld->newJournal );

}


void      tspfldloaders:: writemessagelist( MessagePntrContainer& lst )
{
   for ( MessagePntrContainer::iterator i = lst.begin(); i != lst.end(); i++ )
      stream->writeInt ( (*i)->id );
   stream->writeInt ( 0 );
}


/*
class MessageIDequals : public unary_function<Message*,bool>{
      int id;
   public:
      MessageIDequals ( int _id ) { id == _id; };
      bool operator() ( const Message* m ) { return m->id == id; };
};
*/

void      tspfldloaders:: readmessagelist( MessagePntrContainer& lst )
{
   int i = stream->readInt();
   
   while ( i ) {
      // MessageContainer::iterator mi = find ( spfld->messages.begin(), spfld->messages.end(), MessageIDequals ( i ));
      MessageContainer::iterator mi = spfld->messages.end();
      for ( MessageContainer::iterator mi2 = spfld->messages.begin(); mi2 != spfld->messages.end(); mi2++ )
         if ( (*mi2)->id == i )
            mi = mi2;

      if ( mi == spfld->messages.end())
         warning( "message list corrupted !\nplease report this bug!\nthe game will continue, but some messages will probably be missing\nand other instabilities may occur.");
      lst.push_back ( *mi );
      i = stream->readInt();
   }
}


void      tspfldloaders:: readmessages ( void )
{
   int magic = stream->readInt(); 
   assert( magic == messageVersion );

   while ( spfld->__loadmessages ) {
      Message* msg = new Message ( spfld );

      msg->from    = stream->readInt();
      msg->to      = stream->readInt();
      msg->time    = stream->readInt();
      bool msgtext = stream->readInt();
      msg->id      = stream->readInt();
      int t = stream->readInt();
      int m = stream->readInt();
      msg->gametime.set ( t, m );

      spfld->__loadmessages = stream->readInt();

      if ( msgtext )
         msg->text = stream->readString( true );
   }

   for ( int i = 0; i < 8; i++ )
      if ( spfld->player[ i ].__loadoldmessage )
         readmessagelist ( spfld->player[ i ].oldmessage );

   for ( int i = 0; i < 8; i++ )
      if ( spfld->player[ i ].__loadunreadmessage )
         readmessagelist ( spfld->player[ i ].unreadmessage );

   for ( int i = 0; i < 8; i++ )
      if ( spfld->player[ i ].__loadsentmessage )
         readmessagelist ( spfld->player[ i ].sentmessage );

   if ( spfld->__loadunsentmessage )
      readmessagelist ( spfld->unsentmessage );

   stream->readInt(); // magic

   if ( spfld->___loadJournal )
      spfld->gameJournal = stream->readString();

   if (  spfld->___loadNewJournal )
      spfld->newJournal = stream->readString();
}






void   tspfldloaders::readoldevents ( void )
{
   if ( spfld->loadOldEvents ) {
      int  num = stream->readInt();
      while ( num ) {
         for ( int i = 0;i < num; i++ )
            stream->readInt();
         for ( int i = 0;i < num; i++ )
            stream->readInt();

         num  = stream->readInt();
      }
   }
}



/**************************************************************/
/*     map schreiben / lesen / initialisieren          */
/**************************************************************/

void    tspfldloaders::writemap ( void )
{
       if ( !spfld )
          displaymessage ( "tspfldloaders::writemap  ; no map to write ! ",2);

       spfld->write ( *stream );
}


void     tmaploaders::initmap ( void )
{
    spfld->game_parameter = NULL;
}


void     tgameloaders::initmap ( void )
{
    spfld->game_parameter = NULL;
}


void     tspfldloaders::readmap ( void )
{
    spfld = new GameMap;

    spfld->read ( *stream );
}



void tgameloaders :: writeAI ( )
{
 #ifdef sgmain
   int a = 0;
   for ( int i = 0; i< 8; i++ )
      if ( spfld->player[i].ai )
         a += 1 << i;

   stream->writeInt ( a );

   for ( int i = 0; i < 8; i++ )
      if ( spfld->player[i].ai )
         spfld->player[i].ai->write( *stream );
  #else
   stream->writeInt(0);
  #endif
}

void tgameloaders :: readAI ( )
{
#ifdef sgmain
   int a = stream->readInt();
   for ( int i = 0; i< 8; i++ )
      if ( a & ( 1 << i ) ) {
         AI* ai = new AI ( spfld, i );
         ai->read ( *stream );
         spfld->player[i].ai = ai;
      } else {
         spfld->player[i].ai = NULL;
      }
 #else
   for ( int i = 0; i< 9; i++ )
      spfld->player[i].ai = NULL;
 #endif
}


/**************************************************************/
/*     Network schreiben / lesen                             */
/**************************************************************/

extern void readLegacyNetworkData ( tnstream& stream );


void        tspfldloaders::readLegacyNetwork ( void )
{
   if ( spfld->___loadLegacyNetwork )
      readLegacyNetworkData( *stream );
}




/**************************************************************/
/*     fielder schreiben / lesen                              */
/**************************************************************/

const int objectstreamversion = 2;

void   tspfldloaders::writefields ( void )
{
   int l = 0;
   int cnt1 = spfld->xsize * spfld->ysize;
   int cnt2;

   do {
      cnt2 = 0; 
      tfield* fld = &spfld->field[l];
      /*

      RLE encoding not supported any more, since tfield is becomming too complex

      if (l + 2 < cnt1) { 
         l2 = l + 1; 
         fld2 = &spfld->field[l2];
       asfasfdasfd
         while ((l2 + 2 < cnt1) && ( memcmp(fld2, fld, sizeof(*fld2)) == 0) ) {
            cnt2++;
            l2++;
            fld2 = &spfld->field[l2];
         } 
      }
      */


      char b1 = 0;
      char b3 = 0;
      char b4 = 0;

      if (fld->typ->terraintype->id > 255) 
         b1 |= csm_typid32; 
      if (fld->vehicle != NULL) 
         b1 |= csm_vehicle; 
      if ( (fld->bdt & getTerrainBitType( cbbuildingentry )).any() )
         b1 |= csm_building; 


      if (cnt2 > 0) 
         b1 |= csm_cnt2; 

      if (fld->material > 0) 
         b3 |= csm_material; 
      if (fld->fuel > 0) 
         b3 |= csm_fuel; 

      if (fld->typ != fld->typ->terraintype->weather[0])
         b3 |= csm_weather;
      if (fld->visible)
         b3 |= csm_visible;
      if ( !fld->objects.empty() || !fld->mines.empty() )
         b4 |= csm_newobject;

      if ( fld->resourceview )
         b4 |= csm_resources;

      if ( fld->connection )
         b4 |= csm_connection;

      if ( b4 )
         b3 |= csm_b4;

      if ( b3 ) 
         b1 |= csm_b3; 

      stream->writeChar( b1 );

      if (b1 & csm_b3 ) 
         stream->writeChar ( b3 );

      if (b3 & csm_b4 )
         stream->writeChar ( b4 );

      if (b1 & csm_cnt2 ) 
         stream->writeInt ( cnt2 );

      if (b3 & csm_weather ) {
         int k = 1;
         while ( fld->typ != fld->typ->terraintype->weather[k]    &&   k < cwettertypennum ) {
            k++;
         } /* endwhile */
         
         if ( k == cwettertypennum ) {
            k = 0;
            displaymessage ( "invalid terrain ( weather not found ) at position %d \n",1,l );
          }
         stream->writeInt ( k );
      }

      if (b1 & csm_typid32 )                  
         stream->writeInt ( fld->typ->terraintype->id );
      else
         stream->writeChar ( fld->typ->terraintype->id );

      if (b1 & csm_vehicle ) 
         fld->vehicle->write ( *stream );


      if (b1 & csm_building ) 
         fld->building->write ( *stream );

      if (b3 & csm_material ) 
         stream->writeChar ( fld->material );

      if (b3 & csm_fuel ) 
         stream->writeChar ( fld->fuel );

      if (b3 & csm_visible )
         stream->writeWord ( fld->visible );

      if ( b4 & csm_newobject ) {
         stream->writeInt ( objectstreamversion );

         stream->writeInt ( fld->mines.size() );
         for ( tfield::MineContainer::iterator m = fld->mines.begin(); m != fld->mines.end(); m++  ) {
            stream->writeInt ( m->type );
            stream->writeInt ( m->strength );
            stream->writeInt ( m->lifetimer );
            stream->writeInt ( m->player );
         }

         stream->writeInt ( fld->objects.size() );

         for ( tfield::ObjectContainer::iterator o = fld->objects.begin(); o != fld->objects.end(); o++  ) {
            stream->writeInt ( 1 ); // was: pointer to type
            stream->writeInt ( o->damage );
            stream->writeInt ( o->dir );
            stream->writeInt ( o->lifetimer );
            for ( int i = 0; i < 4; i++ )
               stream->writeInt ( 0 );  // dummy
            stream->writeInt ( o->typ->id );
         }
      }

      if (b4 & csm_resources ) {
         stream->writeChar ( fld->resourceview->visible );
         for ( int i = 0; i < 8; i++ )
            stream->writeChar ( fld->resourceview->fuelvisible[i] );
         for ( int i = 0; i < 8; i++ )
            stream->writeChar ( fld->resourceview->materialvisible[i] );
      }

      if ( b4 & csm_connection )
         stream->writeInt ( fld->connection );

      l += 1 + cnt2;
   }  while (l < cnt1);
}


void tspfldloaders::readfields ( void )
{
   int cnt2 = 0;
   int cnt1 = spfld->xsize * spfld->ysize;

   spfld->allocateFields ( spfld->xsize , spfld->ysize );
   
   if (spfld->field == NULL)
      displaymessage ( "Could not allocate memory for map ",2);

   int l = 0;
   tfield* lfld = NULL;

   do {
      tfield* fld2;

      if (cnt2 == 0) { 

         fld2 = & spfld->field[l];

         fld2->bdt.setInt ( 0 , 0 );

         char b1, b3, b4;
         b1 = stream->readChar();

         if (b1 & csm_b3 ) 
            b3 = stream->readChar();
         else 
            b3 = 0; 

         if (b3 & csm_b4 )
            b4 = stream->readChar();
         else
            b4 = 0;

         if (b1 & csm_cnt2 ) 
            cnt2 = stream->readInt();
         else
            cnt2 = 0; 

         int weather;
         if (b3 & csm_weather )
            weather = stream->readInt();
         else 
            weather = 0;


         int k;

         if (b1 & csm_typid32 ) 
            k = stream->readInt();
         else
            k = stream->readChar();

         pterraintype trn = terrainTypeRepository.getObject_byID ( k );
         if ( !trn ) 
            throw InvalidID ( "terrain", k );

         fld2->typ = trn->weather[weather];
         if ( !fld2->typ ) 
            throw InvalidID ( "terrain", k );

         if (b1 & csm_direction )
            stream->readChar();  // fld2->direction = 0; 
            

         if (b1 & csm_vehicle ) {
             fld2->vehicle = Vehicle::newFromStream ( spfld, *stream );
             fld2->vehicle->setnewposition ( l%spfld->xsize, l/spfld->xsize );
         }

         if (b1 & csm_building ) {
            fld2->building = Building::newFromStream ( spfld, *stream );
            fld2->bdt |= getTerrainBitType(cbbuildingentry);
         }

         if (b3 & csm_material) 
            fld2->material = stream->readChar();
         else 
            fld2->material = 0; 

         if (b3 & csm_fuel) 
            fld2->fuel = stream->readChar();
         else 
            fld2->fuel = 0;

         if (b3 & csm_visible)
            fld2->visible = stream->readWord();
         else
            fld2->visible = 0;

         bool tempobjects[16];
         int  tempobjectNum;

         if (b3 & csm_object ) {

            char minetype = stream->readChar();
            char minestrength = stream->readChar();
            if ( minetype >> 4 ) {
               Mine m( MineTypes((minetype >> 1) & 7), minestrength, minetype >> 4, spfld );
               fld2->mines.push_back ( m );
            }

            tempobjectNum = stream->readInt();

            for ( int i = 0; i < 16; i++ )
               tempobjects[i] = stream->readInt();
         }

         int objectversion = 1;
         if ( b4 & csm_newobject ) {
            objectversion = stream->readInt();

            if ( objectversion < 1 || objectversion > objectstreamversion )
               throw tinvalidversion ( "object", objectstreamversion, objectversion );

            int minenum = stream->readInt();

            for ( int i = 0; i < minenum; i++ ) {
               MineTypes type = MineTypes(stream->readInt());
               int strength = stream->readInt();
               int minetime = stream->readInt();
               int player = stream->readInt();
               if ( player < 0 || player > 7 )
                  player = 0;

               Mine m ( type, strength, player, spfld );
               if ( objectversion == 1 ) {
                  int endtime = minetime;
                  int lifetime = spfld->getgameparameter( GameParameter(cgp_antipersonnelmine_lifetime + m.type - 1));
                  if ( lifetime > 0  &&  endtime > 0 )
                     m.lifetimer = endtime - spfld->time.turn() + spfld->getgameparameter( GameParameter(cgp_antipersonnelmine_lifetime + m.type - 1));
                  else
                     if ( lifetime > 0 )
                        m.lifetimer = lifetime;
                     else
                        m.lifetimer = -1;
               } else
                  m.lifetimer = minetime;
               fld2->mines.push_back ( m );
            }

            tempobjectNum = stream->readInt();
         }

         if ( (b3 & csm_object) || (b4 & csm_newobject )) {
            for ( int n = 0; n < tempobjectNum; n++ ) {
               Object o;
               stream->readInt(); // was: type
               o.damage = stream->readInt();
               o.dir = stream->readInt();
               if ( objectversion >= 2 )
                  o.lifetimer = stream->readInt();
               else
                  stream->readInt();

               for ( int i = 0; i < 4; i++ )
                  stream->readInt(); // dummy

               int id = stream->readInt();
               o.typ = objectTypeRepository.getObject_byID ( id );

               if ( !o.typ )
                  throw InvalidID ( "object", id );

               if ( objectversion == 1 )
                  o.lifetimer = o.typ->lifetime;

               fld2->objects.push_back ( o );
            }
            fld2->sortobjects();
         }

         if (b4 & csm_resources ) {
            fld2->resourceview = new tfield::Resourceview;
            fld2->resourceview->visible = stream->readChar();
            for ( int i = 0; i < 8; i++ )
               fld2->resourceview->fuelvisible[i] = stream->readChar();
            for ( int i = 0; i < 8; i++ )
               fld2->resourceview->materialvisible[i] = stream->readChar();
         }

         if ( b4 & csm_connection ) 
            fld2->connection = stream->readInt();
         
         if (b1 & csm_cnt2 )
            lfld = fld2;

      } 
      else {
         spfld->field[l].typ = lfld->typ;
         spfld->field[l].fuel = lfld->material;
         spfld->field[l].visible = lfld->visible;
         spfld->field[l].tempw = 0;
         spfld->field[l].connection = lfld->connection;
         for ( int i = 0; i < 8; i++ )
            spfld->field[l].view[i] = lfld->view[i];
         cnt2--;
      } 
      l++ ;
   }  while (l < cnt1);

}



/**************************************************************/
/*     Chain Items                                           */
/**************************************************************/

void   tspfldloaders::chainitems ( GameMap* actmap )
{
   int i = 0;
   for (int y = 0; y < actmap->ysize; y++)
      for (int x = 0; x < actmap->xsize; x++) {
          tfield* fld = &actmap->field[i];
          fld->setparams();
          i++;
      }
}



/**************************************************************/
/*     Set Player Existencies                                */
/**************************************************************/

SigC::Signal1<void,GameMap*> tspfldloaders::mapLoaded;


tspfldloaders::tspfldloaders ( void )
{
   spfld = NULL;
}


tspfldloaders::~tspfldloaders ( void )
{
  delete spfld;
  spfld = NULL;
}












































int          tmaploaders::savemap( const ASCString& name )
{ 
   #ifdef logging
   logtofile ( "loaders / tmaploaders::savemap / started " );
   #endif

   tnfilestream filestream ( name, tnstream::writing );

   stream = &filestream;

   spfld = actmap;


   /********************************************************************************/
   /*   Stream initialisieren, Dateiinfo schreiben , map schreiben          */
   /********************************************************************************/
   {
       stream->writepchar ( getFullVersionString() );  // description is not used any more
       stream->writeWord ( fileterminator );
       stream->writeInt ( actmapversion  );

       writemap ( );
   }

   writefields ();
   spfld->weatherSystem->write(filestream);
   stream->writeInt ( actmapversion );

   spfld = NULL;

   return 0;
} 




GameMap* tmaploaders::_loadmap( const ASCString& name )
{ 
    displayLogMessage ( 4, "loading map %s ... ", name.c_str() );

    tnfilestream filestream ( name, tnstream::reading);

    stream = &filestream;

    char* description = NULL;
 
    stream->readpchar ( &description );
    delete[] description;
 
    int w = stream->readWord();
 
    if ( w != fileterminator ) 
       throw tinvalidversion ( name, fileterminator, w );


    int version = stream->readInt();
 
    if ( version > actmapversion || version < minmapversion )
       throw tinvalidversion ( name, actmapversion, version );
   

   displayLogMessage ( 8, "map, ");
   readmap ();

   if ( version <= 0xfe27 ) {
      displayLogMessage ( 8, "eventsToCome, ");
      readOldEventLists ( stream, false, spfld );
   }

   displayLogMessage ( 8, "fields, ");
   readfields ();
   if(version >= 0xfe50){  
     if(spfld->weatherSystem != NULL ) {
        delete spfld->weatherSystem;
     }
     spfld->weatherSystem = new WeatherSystem(spfld);
     spfld->weatherSystem->read(filestream);
   }else{
     spfld->weatherSystem->setGlobalWind(WeatherSystem::legacyWindSpeed, static_cast<Direction>(WeatherSystem::legacyWindDirection));   
   }

   version = stream->readInt();
   if (version > actmapversion || version < minmapversion ) 
      throw tinvalidversion ( name, actmapversion, version );
   

   displayLogMessage ( 8, "chainItems, ");
   chainitems ( spfld );

   for ( int sp = spfld->getPlayerCount()-1; sp >= 0; sp--)
      if ( spfld->player[sp].exist() )
         spfld->actplayer = sp;

   displayLogMessage ( 8, "setEventTriggers, ");
   seteventtriggers( spfld );

   displayLogMessage ( 8, "calculateallobjects ");
   calculateallobjects( spfld );


   displayLogMessage ( 8, "init for playing, ");
   
   spfld->time.set ( 1, 0 );
   spfld->levelfinished = false;
   spfld->preferredFileNames.mapname[0] = name ;
   spfld->startGame();

   displayLogMessage ( 4, "done\n");

   GameMap* m  = spfld;
   spfld = NULL;
   
   mapLoaded( m );
   
   return m;
} 


GameMap* tmaploaders::loadmap ( const ASCString& name )
{
     tmaploaders gl;
     return gl._loadmap ( name );
}     
   




/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */ 








void   tsavegameloaders::savegame( pnstream strm, GameMap* gamemap, bool writeReplays )
{
   stream = strm;
   spfld = gamemap;

   stream->writepchar( getFullVersionString() );
   stream->writeWord( fileterminator );

   stream->writeInt( actsavegameversion );
   writemap ();

   writemessages();

   writefields ( );
   
   spfld->weatherSystem->write(*stream);

   writedissections();

   if ( writeReplays && spfld->replayinfo ) {
      stream->writeInt( 1 );
      spfld->replayinfo->write(*stream);
   } else
      stream->writeInt( 0 );

   writeAI();

   stream->writeInt( actsavegameversion );

   spfld = NULL;
}


void         tsavegameloaders::savegame( const ASCString& name)
{ 
   tnfilestream filestream ( name, tnstream::writing );
   savegame ( &filestream, actmap, true );
}



int   tsavegameloaders::loadgame( const ASCString& filename )
{
   tnfilestream filestream ( filename, tnstream::reading );

   GameMap* spfld = loadgame ( &filestream );

   delete actmap;
   actmap = spfld;
   actmap->levelfinished = false;

   if ( actmap->replayinfo ) {
      if ( actmap->replayinfo->actmemstream )
         displaymessage2( "actmemstream already open at begin of turn ",2 );

      if ( actmap->replayinfo->guidata[actmap->actplayer] )
         actmap->replayinfo->actmemstream = new tmemorystream ( actmap->replayinfo->guidata[actmap->actplayer], tnstream::appending );
      else {
         actmap->replayinfo->guidata[actmap->actplayer] = new tmemorystreambuf;
         actmap->replayinfo->actmemstream = new tmemorystream ( actmap->replayinfo->guidata[actmap->actplayer], tnstream::writing );
      }
   }
   
   mapLoaded( actmap );

   return 0;
}

GameMap*          tsavegameloaders::loadgame( pnstream strm )
{
   stream = strm;

   stream->readString(); // was: description

   int w = stream->readWord();

   if ( w != fileterminator ) 
      throw tinvalidversion ( strm->getDeviceName(), fileterminator, w );
   
   int version = stream->readInt();

   if (version > actsavegameversion || version < minsavegameversion ) 
      throw tinvalidversion ( strm->getDeviceName(), actsavegameversion, version );
   

   readmap ();

   readLegacyNetwork ();

   if  ( version <= 0xff39 )
      for ( int i = 0; i < 8; i++ )
         spfld->player[i].research.read_techs ( *stream );

   readmessages();

   if ( version <= 0xff37 ) {
      readOldEventLists ( stream, true, spfld );
      readoldevents ();
   }

   readfields ( );
   
   if(version >= 0xff60){  
     if(spfld->weatherSystem != NULL) {
        delete spfld->weatherSystem;
     }
     spfld->weatherSystem = new WeatherSystem(spfld);
     spfld->weatherSystem->read(*stream);
   }else{
      spfld->weatherSystem->setGlobalWind(WeatherSystem::legacyWindSpeed, static_cast<Direction>(WeatherSystem::legacyWindDirection));   
   }
   
   readdissections();

   bool loadReplay = true;
   if ( version >= 0xff35 ) 
      if ( !stream->readInt() )
         loadReplay = false;

   if ( loadReplay ) {
      spfld->replayinfo = new GameMap::ReplayInfo;
      spfld->replayinfo->read ( *stream );
   } else
      spfld->replayinfo = NULL;


   if ( version >= 0xff34 )
      readAI ();

   version = stream->readInt();
   if (version > actsavegameversion || version < minsavegameversion )
      throw tinvalidversion ( strm->getDeviceName(), actsavegameversion, version );

   chainitems ( spfld );

   seteventtriggers( spfld );

   calculateallobjects( spfld );

   GameMap* s = spfld;
   spfld = NULL;  // to avoid that is is deleted by the destructor of tsavegameloaders
   return s;
}



   /*****************************************************************************************************/
   /*   Netzwerk                                                                                       */
   /*****************************************************************************************************/




int          tnetworkloaders::savenwgame( pnstream strm )
{ 
   spfld = actmap;
   
   stream = strm;

   stream->writepchar ( getFullVersionString() );  // description is not used any more
   stream->writeWord ( fileterminator );
 
   stream->writeInt ( actnetworkversion );

   writemap ();

   actmap->weatherSystem->write(*stream);

   writemessages();

   stream->writeInt ( actnetworkversion );

   writefields ( );

   writedissections();

   if ( spfld->replayinfo )
      spfld->replayinfo->write ( *stream );

   // the AI must be the last data of the file
   writeAI();

   stream->writeInt ( actnetworkversion );

   spfld = NULL;

   return 0;
} 





GameMap*  tnetworkloaders::loadnwgame( pnstream strm )
{ 
   char* name = "network game";

   stream = strm;


   char* description = NULL;
   stream->readpchar ( &description );
   delete[] description;

   int w = stream->readWord();

   if ( w != fileterminator )
      throw tinvalidversion ( name, fileterminator, (int) w );


   int version = stream->readInt();

   if (version > actnetworkversion || version < minnetworkversion )
      throw tinvalidversion ( name, actnetworkversion, version );

   readmap ();

   if ( version <= 0x11 )
      for ( int i = 0; i < 8; i++ )
         spfld->player[i].research.read_techs ( *stream );

   
   //NEW SaveData Weather  
   if(version >= 0x0030){
     if(spfld->weatherSystem != NULL) {
        delete spfld->weatherSystem;
     }
     spfld->weatherSystem = new WeatherSystem(spfld);
     spfld->weatherSystem->read(*stream);
   }else{
      spfld->weatherSystem->setGlobalWind(WeatherSystem::legacyWindSpeed, static_cast<Direction>(WeatherSystem::legacyWindDirection));   
   }
   
   /*if(version > 0xfe28){  //Vielleicht minus 1     
     actmap->weatherSystem->read(*stream);
   }*/
   readmessages();
   readLegacyNetwork ();

   version = stream->readInt();

   if (version > actnetworkversion || version < minnetworkversion )
      throw tinvalidversion ( name, actnetworkversion, version );



   if ( version < 10 ) {
      readOldEventLists ( stream, true, spfld );
      readoldevents    ();
   }

   readfields ( );

   readdissections();

   if ( spfld->__loadreplayinfo ) {
      spfld->replayinfo = new GameMap::ReplayInfo;
      spfld->replayinfo->read ( *stream );
   }

   if ( version > 8 )
      readAI();

   #ifdef sgmain
   stream->readdata2( version );
   if (version > actnetworkversion || version < minnetworkversion )
      throw tinvalidversion ( name, actnetworkversion, version );
   #endif

   chainitems ( spfld );

   seteventtriggers( spfld );

   calculateallobjects( spfld );

   spfld->levelfinished = false;
   
   // there was a bug that made the ammo amount underflow
   for ( int i = 0; i < 8; ++i)
      for ( Player::BuildingList::iterator b = spfld->player[i].buildingList.begin(); b != spfld->player[i].buildingList.end(); ++b )
         for ( int a = 0; a < waffenanzahl; ++a )
            if ( (*b)->ammo[a] > 32000 ) {
               ASCString s;
               s.format( "Player %s had %d ammo of type %s in the building at %d/%d", spfld->player[i].getName().c_str(), (*b)->ammo[a], cwaffentypen[a], (*b)->getEntry().x, (*b)->getEntry().y );
               new Message( s, spfld, 1 );
               (*b)->ammo[a] = 0;
            }

   
   GameMap* spfldcopy = spfld;
   spfld = NULL;

   mapLoaded( spfldcopy );
   
   
   return spfldcopy;

}














void  savemap( const char * name )
{

   #ifdef logging
   logtofile ( "loaders / savemap / started " );
   #endif

   try {
     tmaploaders gl;
     gl.savemap ( name );
   } /* endtry */

   catch ( tfileerror err ) {
      displaymessage( "file error writing map to filename %s ", 1, err.getFileName().c_str() );
   } /* endcatch */
   catch ( ASCexception err) {
      displaymessage( "error writing map ", 1 );
   } /* endcatch */

}


void  savegame( const ASCString& name )
{
   try {
      tsavegameloaders gl;
      gl.savegame ( name );
   }
   catch ( tfileerror err) {
      displaymessage( "error writing map to filename %s ", 1, err.getFileName().c_str() );
   } /* endcatch */
   catch ( ASCexception err) {
      displaymessage( "error writing map ", 1 );
   } /* endcatch */
}


void  loadgame( const ASCString& name )
{
   try {
      tsavegameloaders gl;
      gl.loadgame ( name );
   }
   catch ( InvalidID err ) {
      displaymessage( err.getMessage().c_str(), 1 );
      if ( !actmap || actmap->xsize <= 0)
         throw NoMapLoaded();
   } /* endcatch */
   catch ( tinvalidversion err ) {
      if ( err.expected < err.found )
         displaymessage( "File/module %s has invalid version.\nExpected version %d\nFound version %d\nPlease install the latest version from www.asc-hq.org", 1, err.getFileName().c_str(), err.expected, err.found );
      else
         displaymessage( "File/module %s has invalid version.\nExpected version %d\nFound version %d\nThis is a bug, please report it!", 1, err.getFileName().c_str(), err.expected, err.found );
      if ( !actmap || actmap->xsize <= 0)
         throw NoMapLoaded();
   } /* endcatch */
   catch ( tfileerror err) {
      displaymessage( "error reading map filename %s ", 1, err.getFileName().c_str() );
      if ( !actmap || actmap->xsize <= 0)
         throw NoMapLoaded();
   } /* endcatch */
   catch ( ASCexception ) {
      displaymessage( "error loading game", 1 );
      if ( !actmap || actmap->xsize <= 0)
         throw NoMapLoaded();
   } /* endcatch */

}



void  savereplay( GameMap* gamemap, int num )
{
   try {
      if ( !gamemap->replayinfo )
         fatalError ( "treplayloaders :: savereplay   ;   No replay activated !");

      if ( gamemap->replayinfo->map[num] ) {
         delete gamemap->replayinfo->map[num];
         gamemap->replayinfo->map[num] = NULL;
      }

      gamemap->replayinfo->map[num] = new tmemorystreambuf;
      tmemorystream memstream ( gamemap->replayinfo->map[num], tnstream::writing );

      memstream.writeInt( actreplayversion );

      tsavegameloaders sgl;
      sgl.savegame ( &memstream, gamemap, false );

      memstream.writeInt ( actreplayversion );
   }
   catch ( ASCexception err ) {
      displaymessage( "error saving replay information", 1 );
   } /* endcatch */
}

GameMap*  loadreplay( pmemorystreambuf streambuf )
{
   GameMap* replaymap = NULL;

   try {
      char* name = "memorystream actmap->replayinfo";
      tmemorystream memstream ( streambuf, tnstream::reading );

      int version = memstream.readInt();
      if (version > actreplayversion || version < minreplayversion )
         throw tinvalidversion ( name, actreplayversion, version );

      tsavegameloaders sgl;
      replaymap = sgl.loadgame ( &memstream );

      version = memstream.readInt();
      if (version > actreplayversion || version < minreplayversion ) {
         delete replaymap;
         throw tinvalidversion ( name, actreplayversion, version );
      }

   }
   catch ( InvalidID err ) {
      displaymessage( err.getMessage().c_str(), 1 );
      replaymap = NULL;
   } /* endcatch */
   catch ( tinvalidversion err ) {
      if ( err.expected < err.found )
         displaymessage( "File/module %s has invalid version.\nExpected version %d\nFound version %d\nPlease install the latest version from www.asc-hq.org", 1, err.getFileName().c_str(), err.expected, err.found );
      else
         displaymessage( "File/module %s has invalid version.\nExpected version %d\nFound version %d\nThis is a bug, please report it!", 1, err.getFileName().c_str(), err.expected, err.found );
      replaymap = NULL;
   } /* endcatch */
   catch ( tfileerror err) {
      displaymessage( "error reading map filename %s ", 1, err.getFileName().c_str() );
      replaymap = NULL;
   } /* endcatch */
   catch ( ASCexception ) {
      displaymessage( "error loading replay", 1 );
      replaymap = NULL;
   } /* endcatch */

   return replaymap;
}




GameMap* mapLoadingExceptionChecker( const ASCString& filename, MapLoadingFunction loader )
{
   GameMap* m = NULL;
   try {
      m = loader( filename );
   }
   catch ( InvalidID err ) {
      displaymessage( err.getMessage().c_str(), 1 );
      return NULL;
   } /* endcatch */
   catch ( tinvalidversion err ) {
      if ( err.expected < err.found )
         displaymessage( "File/module %s has invalid version.\nExpected version %d\nFound version %d\nPlease install the latest version from www.asc-hq.org", 1, err.getFileName().c_str(), err.expected, err.found );
      else
         displaymessage( "File/module %s has invalid version.\nExpected version %d\nFound version %d\nThis is a bug, please report it!", 1, err.getFileName().c_str(), err.expected, err.found );
      return NULL;
   } /* endcatch */
   catch ( tfileerror err) {
      displaymessage( "error reading map filename %s ", 1, err.getFileName().c_str() );
      return NULL;
   } /* endcatch */
   catch ( ASCexception ) {
      displaymessage( "error loading file", 1 );
      return NULL;
   } /* endcatch */
   
   return m;
}









bool validatemapfile ( const ASCString& filename )
{

   char* description = NULL;

   try {

      tnfilestream stream ( filename, tnstream::reading );
      stream.readpchar ( &description, 200 );
      if ( description ) {
         delete[]  description ;
         description = NULL;
      }

      int w = stream.readWord();
      if ( w != fileterminator )
         throw tinvalidversion ( filename, fileterminator, (int) w );

      int version = stream.readInt();

      if (version > actmapversion || version < minmapversion ) 
         throw tinvalidversion ( filename, actmapversion, version );

   } /* endtry */

   catch ( ASCexception ) {
       return false;
   } /* endcatch */

   return true;
} 





bool validateemlfile ( const ASCString& filename )
{
   char* description = NULL;

   try {

      tnfilestream stream ( filename, tnstream::reading );
      stream.readpchar ( &description, 200 );
      if ( description ) {
         delete[]  description ;
         description = NULL;
      }

      int w = stream.readWord();
      if ( w != fileterminator )
         throw tinvalidversion ( filename, fileterminator, (int) w );

      int version = stream.readInt();
   
      if (version > actnetworkversion || version < minnetworkversion ) 
         throw tinvalidversion ( filename, actnetworkversion, version );

   } /* endtry */

   catch ( ASCexception ) {
       return false;
   } /* endcatch */


   return true;
} 


bool validatesavfile ( const ASCString& filename )
{

   char* description = NULL;

   try {

      tnfilestream stream ( filename, tnstream::reading );
      stream.readpchar ( &description, 200 );
      if ( description ) {
         delete[]  description ;
         description = NULL;
      }

      int w = stream.readWord();
      if ( w != fileterminator )
         throw tinvalidversion ( filename, fileterminator, w );

      int version = stream.readInt();
      if (version > actsavegameversion || version < minsavegameversion )
         throw tinvalidversion ( filename, actsavegameversion, version );

   } /* endtry */

   catch ( ASCexception ) {
       return false;
   } /* endcatch */

   return true;
} 





void         savecampaignrecoveryinformation( const ASCString& filename,
                                             int id)
{ 
   displaymessage("This has not been implemented yet, sorry!", 2 );
}




#if 0
void         loadicons(void)
{
  int w2;
  int          *w = & w2, i;


  {
      int xl[5] = { cawar, cawarannounce, capeaceproposal, capeace, capeace_with_shareview };
      tnfilestream stream ("allianc2.raw",tnstream::reading);
      for ( i = 0; i < 5; i++ )
         stream.readrlepict( &icons.diplomaticstatus[xl[i]],false,w); 
      icons.diplomaticstatus[canewsetwar1] = icons.diplomaticstatus[cawar];
      icons.diplomaticstatus[canewsetwar2] = icons.diplomaticstatus[cawar];
      icons.diplomaticstatus[canewpeaceproposal] = icons.diplomaticstatus[capeaceproposal];

  }
   
  {
      tnfilestream stream ("iconship.raw",tnstream::reading);
      stream.readrlepict( &icons.statarmy[2],false,w);
  }

  {
      tnfilestream stream ("icontank.raw",tnstream::reading);
      stream.readrlepict( &icons.statarmy[1],false,w);
  }
   
  {
      tnfilestream stream ("iconplan.raw",tnstream::reading);
      stream.readrlepict( &icons.statarmy[0],false,w);
  }
   
  {
      tnfilestream stream ("pfeil2.raw",tnstream::reading);
      stream.readrlepict( &icons.weapinfo.pfeil1, false, w);
  }
   
  {
      tnfilestream stream ("pfeil3.raw",tnstream::reading);
      stream.readrlepict( &icons.weapinfo.pfeil2, false, w);
  }

  {
      tnfilestream stream ("height.raw",tnstream::reading); 
      for (i = 0; i <= 7; i++) 
         stream.readrlepict( &icons.height[i],false,w);
   }
}
#endif
