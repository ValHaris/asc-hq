/*! \file gamemap.cpp
    \brief Implementation of THE central asc class: GameMap
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
#include <ctime>
#include <cmath>

#include "global.h"
#include "misc.h"
#include "typen.h"
#include "vehicletype.h"
#include "buildingtype.h"
#include "gamemap.h"
#include "dialog.h"
#include "itemrepository.h"
#include "strtmesg.h"
// #include "graphics/blitter.h"
#include "overviewmapimage.h"
#include "gameeventsystem.h"
#include "spfst.h"


RandomGenerator::RandomGenerator(int seedValue){

}

RandomGenerator::~RandomGenerator(){


}

unsigned int RandomGenerator::getPercentage(){
  return getRandomValue(100);
}

unsigned int RandomGenerator::getRandomValue(int limit) {
  return getRandomValue(0, limit); 
}

unsigned int RandomGenerator::getRandomValue (int lowerLimit, int upperLimit){
   if(upperLimit == 0) {
        return 1;
   }
   int random_integer = rand();
   random_integer = random_integer % upperLimit;
   return (lowerLimit + random_integer);
}




OverviewMapHolder :: OverviewMapHolder( GameMap& gamemap ) : map(gamemap), initialized(false), secondMapReady(false), completed(false), connected(false), x(0),y(0)
{
}

void OverviewMapHolder :: connect()
{
   if ( !connected ) {
      idleEvent.connect ( SigC::slot( *this, &OverviewMapHolder::idleHandler ));
      connected = true;
   }
}


SigC::Signal0<void> OverviewMapHolder::generationComplete;

bool OverviewMapHolder :: idleHandler( )
{
   int t = ticker;
   while ( !completed && (t + 5 > ticker ))
      drawNextField( true );
   return true;
}


bool OverviewMapHolder::updateField( const MapCoordinate& pos )
{
   SPoint imgpos = OverviewMapImage::map2surface( pos );

   tfield* fld = map.getField( pos );
   VisibilityStates visi = fieldVisibility( fld, map.getPlayerView(), &map );
   if ( visi == visible_not ) {
      OverviewMapImage::fill ( overviewMapImage, imgpos, 0xff545454 );
   } else {
      if ( fld->building && fieldvisiblenow( fld, map.getPlayerView(), &map) )
         OverviewMapImage::fill ( overviewMapImage, imgpos, map.player[fld->building->getOwner()].getColor() );
      else {

         int w = fld->getweather();
         fld->typ->getQuickView()->blit( overviewMapImage, imgpos );
         for ( tfield::ObjectContainer::iterator i = fld->objects.begin(); i != fld->objects.end(); ++i )
            if ( visi > visible_ago || i->typ->visibleago )
               i->getOverviewMapImage( w )->blit( overviewMapImage, imgpos );

         if ( fld->vehicle && fieldvisiblenow( fld, map.getPlayerView()) )
            OverviewMapImage::fillCenter ( overviewMapImage, imgpos, map.player[fld->vehicle->getOwner()].getColor() );

         if ( visi == visible_ago )
            OverviewMapImage::lighten( overviewMapImage, imgpos, 0.7 ); 

      }

   }
   return true;
}

void OverviewMapHolder::drawNextField( bool signalOnCompletion )
{
   if ( !init() )
      return;
      
   if ( x == map.xsize ) {
      x = 0;
      ++y;
   }   
   if ( y < map.ysize ) {
      if ( !updateField( MapCoordinate(x,y)))
         return;
      
      ++x;
   }
   if ( y == map.ysize ) {
      completed = true;
      if ( signalOnCompletion )
         generationComplete();

      completedMapImage = overviewMapImage.Duplicate();
      secondMapReady = true;
   }
}

Surface OverviewMapHolder::createNewSurface()
{
   Surface s;
   if ( map.xsize > 0 && map.ysize > 0 ) {
      s =  Surface::createSurface( (map.xsize+1) * 6, 4 + map.ysize * 2 , 32, 0 );
   }
   return s;
}

bool OverviewMapHolder::init()
{
   if ( map.ysize <= 0 || map.xsize <= 0 )
      return false;

   if ( !initialized ) {
      overviewMapImage = createNewSurface();
      initialized = true;
   }
   return initialized;
}   

void OverviewMapHolder::resetSize()
{
   initialized = false;
}


const Surface& OverviewMapHolder::getOverviewMap( bool complete )
{
   bool initialized = init();
   assert( initialized );
   if ( complete )
      while ( !completed )
         drawNextField( false );

   if(  secondMapReady )
      return completedMapImage;
   else
      return overviewMapImage;
}

void OverviewMapHolder::startUpdate()
{
   completed = false;
   x = 0;
   y = 0;
}

void OverviewMapHolder::clear(bool allImages )
{
   if ( !initialized )
      return;
   
   overviewMapImage.Fill( Surface::transparent );
   if ( allImages ) {
      if ( completedMapImage.valid() )
         completedMapImage.Fill( Surface::transparent );
      secondMapReady = false;
   }

   startUpdate();
}

void OverviewMapHolder::clearmap( GameMap* actmap )
{
   if ( actmap )
      actmap->overviewMapHolder.clear();
}


GameMap :: GameMap ( void )
      : overviewMapHolder( *this ), network(NULL)
{
   randomSeed = rand();
   dialogsHooked = false;

   eventID = 0;

   state = Normal;

   int i;

   xsize = 0;
   ysize = 0;
   field = NULL;

   actplayer = -1;
   time.abstime = 0;

   _resourcemode = 0;

   for ( i = 0; i < 9; ++i ) {
      player[i].setParentMap ( this, i );
      if ( i == 0 )
         player[i].stat = Player::human;
      else
         player[i].stat = Player::computer;
      
      player[i].research.chainToMap ( this, i );
   }
          
   unitnetworkid = 0;

   levelfinished = 0;

   messageid = 0;

   continueplaying = false;
   replayinfo = NULL;
   playerView = 0;
   lastjournalchange.abstime = 0;
   graphicset = 0;
   gameparameter_num = 0;
   game_parameter = NULL;
   mineralResourcesDisplayed = 0;

   // sigPlayerUserInteractionBegins.connect( SigC::bind( SigC::slot( GameMap::setPlayerMode ), NormalManual));
   // sigPlayerUserInteractionEnds.connect( SigC::bind( SigC::slot( GameMap::setPlayerMode ), NormalAuto));


#ifdef WEATHERGENERATOR
   weatherSystem  = new WeatherSystem(this, 1, 0.03);
#endif
   setgameparameter( cgp_objectsDestroyedByTerrain, 1 );
}

GameMap::Campaign::Campaign()
{
   avail = false;
   id = 0;
   directaccess = true;
}

void GameMap :: guiHooked()
{
   overviewMapHolder.connect();
   dialogsHooked = true;
}

const int tmapversion = 20;

void GameMap :: read ( tnstream& stream )
{
   int version;
   int i;

   xsize = stream.readWord();
   ysize = stream.readWord();

   if ( xsize == 0xfffe  && ysize == 0xfffc ) {
     version = stream.readInt();
     if ( version > tmapversion )
        throw tinvalidversion ( "GameMap", tmapversion, version );

     xsize = stream.readInt();
     ysize = stream.readInt();
   } else
      version = 1;

   stream.readWord(); // xpos
   stream.readWord(); // ypos
   stream.readInt(); // dummy
   field = NULL;

   if ( version <= 13 ) {
      char buf[11]; 
      stream.readdata ( buf, 11 );
      buf[10] = 0;
      codeWord = buf;
   } else {
      codeWord = stream.readString();
   }

   if ( version < 2 )
      ___loadtitle = stream.readInt();
   else
      ___loadtitle = true;

   bool loadCampaign = stream.readInt();
   actplayer = stream.readChar();
   time.abstime = stream.readInt();   
   if(version < 9 || version >= 17){
     stream.readChar();
     weather.windSpeed = stream.readChar();
     weather.windDirection = stream.readChar();
   }

   if ( version >= 11 ) 
      if ( stream.readInt() != 0x12345678 )
         throw ASCmsgException("marker not matched when loading GameMap");
   
      
   for ( int j = 0; j < 4; j++ )
      stream.readChar(); // was: different wind in different altitudes
   for ( i = 0; i< 12; i++ )
      stream.readChar(); // dummy

   _resourcemode = stream.readInt();

   int alliances[8][8];
   if ( version <= 10 )
      for ( i = 0; i < 8; i++ )
         for ( int j = 0; j < 8; j++ )
            alliances[j][i] = stream.readChar();

   int dummy_playername[9];
   for ( i = 0; i< 9; i++ ) {
      player[i].existanceAtBeginOfTurn = stream.readChar();
      stream.readInt(); // dummy
      stream.readInt(); // dummy
      if ( version <= 5 )
         player[i].research.read_struct ( stream );
      else
         player[i].research.read ( stream );

      player[i].ai = (BaseAI*)stream.readInt() ;
      player[i].stat = Player::PlayerStatus ( stream.readChar() );
      stream.readChar(); // dummy
      dummy_playername[i] = stream.readInt();
      player[i].passwordcrc.read ( stream );
      player[i].__dissectionsToLoad = stream.readInt();
      player[i].__loadunreadmessage = stream.readInt();
      player[i].__loadoldmessage = stream.readInt();
      player[i].__loadsentmessage = stream.readInt();
      if ( version >= 3 )
         player[i].ASCversion = stream.readInt();
      else
         player[i].ASCversion = 0;
         
      if ( version >= 9 )
         player[i].cursorPos.read( stream );
         
      if ( version >= 11 ) 
         player[i].diplomacy.read( stream );
      else {
         if ( i < 8 ) // no alliances for neutral 'player'
            for ( int j = 0; j< 8; ++j ) {
               if ( alliances[i][j] == 0 ) 
                  player[i].diplomacy.setState( j, PEACE, false ); 
               else
                  player[i].diplomacy.setState( j, WAR, false ); 
            }
      }
      
      if ( version >= 12 )
         player[i].email = stream.readString();
   }
   
   if ( version >= 11 ) 
      if ( stream.readInt() != 0x12345678 )
         throw ASCmsgException("marker not matched when loading GameMap");
         



   if ( version <= 4 ) {
   //! practically dummy
      loadOldEvents = stream.readInt();
      stream.readInt(); // int loadeventstocome
      stream.readInt(); // int loadeventpassed
   }


   unitnetworkid = stream.readInt();
   levelfinished = stream.readChar();
   
   bool alliance_names_not_used_any_more[8];
   if ( version <= 9 ) {
      ___loadLegacyNetwork  = stream.readInt();
      for ( i = 0; i < 8; i++ )
         alliance_names_not_used_any_more[i] = stream.readInt(); // dummy
   } else {
      ___loadLegacyNetwork = false;
      for ( i = 0; i < 8; i++ )
         alliance_names_not_used_any_more[i] = 0;
   }

   if ( version <= 12 ) {
      for ( i = 0; i< 8; i++ ) {
         stream.readWord(); // cursorpos.position[i].cx = 
         stream.readWord(); // cursorpos.position[i].sx = 
         stream.readWord(); // cursorpos.position[i].cy = 
         stream.readWord(); // cursorpos.position[i].sy = 
      }
   }

   if ( version <= 9 )
      stream.readInt(); // loadtribute
      
   __loadunsentmessage = stream.readInt();
   __loadmessages = stream.readInt();

   messageid = stream.readInt();

   if(  version < 2 ) {
      ___loadJournal = stream.readInt();
      ___loadNewJournal = stream.readInt();
   } else {
      ___loadJournal = true;
      ___loadNewJournal = true;
   }

   int exist_humanplayername[9];
   for ( i = 0; i < 8; i++ )
      exist_humanplayername[i] = stream.readInt();
   exist_humanplayername[8] = 0;


   int exist_computerplayername[9];
   for ( i = 0; i < 8; i++ )
      exist_computerplayername[i] = stream.readInt();
   exist_computerplayername[8] = 0;

   supervisorpasswordcrc.read ( stream );

   if ( version <= 10 )
      for ( i = 0; i < 8; i++ )
         stream.readChar(); // alliances_at_beginofturn[i] = 

   stream.readInt(); // was objectcrc = (Object*containercrcs)
   bool load_shareview = false;
   if ( version <= 10 )
      load_shareview = stream.readInt();

   continueplaying = stream.readInt();
   __loadreplayinfo =  stream.readInt();
   playerView = stream.readInt();
   lastjournalchange.abstime = stream.readInt();

   for ( i = 0; i< 8; i++ )
      bi_resource[i].read ( stream );

   int preferredfilenames = stream.readInt();

   bool __loadEllipse = stream.readInt();
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

   if ( version >= 11 ) 
      if ( stream.readInt() != 0x12345678 )
         throw ASCmsgException("marker not matched when loading GameMap");

/////////////////////
// Here initmap was called
/////////////////////



    if ( ___loadtitle )
       maptitle = stream.readString();

    if ( loadCampaign ) {
       if ( version <= 14 ) {
         campaign.id = stream.readWord();
         stream.readWord(); // campaign->prevmap 
         stream.readChar(); // campaign->player 
         campaign.directaccess = stream.readChar();
         campaign.avail = true;
         for ( int d = 0; d < 21; d++ )
            stream.readChar(); // dummy
       } else {
          campaign.id = stream.readInt();
          campaign.directaccess = stream.readChar();
          if ( version > 15 )
             campaign.avail = stream.readInt();
       }
    }

    for ( int w=0; w<9 ; w++ ) {
       if (dummy_playername[w] )
          stream.readString();

       player[w].ai = NULL;


       if ( exist_humanplayername[w] )
          player[w].setName( stream.readString() );

       if ( exist_computerplayername[w] )
          stream.readString();

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

    if ( load_shareview && version <= 10 ) {
    
      for ( int i = 0; i < 8; i++ )
         for ( int j =0; j < 8; j++ ) {
            int sv = stream.readChar();
            if ( sv )
               player[i].diplomacy.setState( j, PEACE_SV, false );
         }      
               
      stream.readInt();
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

    if ( __loadEllipse ) {
       for ( int i = 0; i < 5; ++i )
          stream.readInt();
       stream.readFloat();
       stream.readInt();
    }

    int orggpnum = gameparameter_num;
    gameparameter_num = 0;
    for ( int gp = 0; gp < 8; gp ++ )
       setgameparameter ( GameParameter(gp), _oldgameparameter[gp] );

    for ( int ii = 0 ; ii < orggpnum; ii++ ) {
       int gpar = stream.readInt();
       setgameparameter ( GameParameter(ii), gpar );
    }

    if ( version >= 2 ) {
       archivalInformation.author = stream.readString();
       archivalInformation.description = stream.readString();
       archivalInformation.tags = stream.readString();
       archivalInformation.requirements = stream.readString();
       archivalInformation.modifytime = stream.readInt();
    }

    if ( version >= 4 ) {
       int num = stream.readInt();
       for ( int ii = 0; ii < num; ++ii )
          unitProduction.idsAllowed.push_back ( stream.readInt() );

       for ( int ii = 0; ii < 9; ii++ ) {
          int num = stream.readInt( );
          for ( int i = 0; i < num; i++ ) {
             Player::PlayTime pt;
             pt.turn = stream.readInt();
             pt.date = stream.readInt();
             player[ii].playTime.push_back ( pt );
          }
       }

    }

    if ( version >= 5 ) {
       eventID = stream.readInt();

       int num = stream.readInt();
       for ( int i = 0; i< num; ++i ) {
          Event* ev = new Event ( *this );
          ev->read ( stream );
          events.push_back ( ev );
       }
    }

    if ( version >= 8 )
       randomSeed = stream.readInt();
       
    if ( version >= 12 ) {
      bool nw = stream.readInt();
      if ( nw ) 
         network = GameTransferMechanism::read( stream );         
    }
}


void GameMap :: write ( tnstream& stream )
{
   int i;

   stream.writeWord( 0xfffe );
   stream.writeWord( 0xfffc );

   stream.writeInt ( tmapversion );
   stream.writeInt( xsize );
   stream.writeInt( ysize );

   stream.writeWord( 0 );
   stream.writeWord( 0 );
   stream.writeInt (1); // dummy
   stream.writeString ( codeWord );

   
   
   stream.writeInt( campaign.avail  );
   stream.writeChar( actplayer );
   stream.writeInt( time.abstime );
   
   stream.writeChar(0);
   stream.writeChar( weather.windSpeed );
   stream.writeChar( weather.windDirection );
   
   stream.writeInt( 0x12345678 );
   
   for  ( i= 0; i < 4; i++ )
      stream.writeChar( 0 );

   for ( i = 0; i< 12; i++ )
      stream.writeChar( 0 ); // dummy

   stream.writeInt( _resourcemode );

   for ( i = 0; i< 9; i++ ) {
      stream.writeChar( player[i].existanceAtBeginOfTurn );
      stream.writeInt( 1 ); // dummy
      stream.writeInt( 1 ); // dummy
      player[i].research.write ( stream );
      stream.writeInt( player[i].ai != NULL );
      stream.writeChar( player[i].stat );
      stream.writeChar( 0 ); // dummy
      stream.writeInt( 0 );
      player[i].passwordcrc.write ( stream );
      stream.writeInt( !player[i].dissections.empty() );
      stream.writeInt( 1 );
      stream.writeInt( 1 );
      stream.writeInt( 1 );
      stream.writeInt ( player[i].ASCversion );
      player[i].cursorPos.write( stream );
      player[i].diplomacy.write( stream );
      stream.writeString ( player[i].email );
   }

   stream.writeInt( 0x12345678 );
   
   stream.writeInt( unitnetworkid );
   stream.writeChar( levelfinished );

   stream.writeInt( 1 );
   stream.writeInt( !messages.empty() );

   stream.writeInt( messageid );

   for ( i = 0; i < 8; i++ )
      stream.writeInt( 1 );

   for ( i = 0; i < 8; i++ )
      stream.writeInt( 0 );

   supervisorpasswordcrc.write ( stream );

   stream.writeInt( 0 );

   stream.writeInt( continueplaying );
   stream.writeInt( replayinfo != NULL );
   stream.writeInt( playerView );
   stream.writeInt( lastjournalchange.abstime );

   for ( i = 0; i< 8; i++ )
      bi_resource[i].write ( stream );

   stream.writeInt( 1 );
   stream.writeInt( 0 ); // was: ellipse
   stream.writeInt( graphicset );
   stream.writeInt( gameparameter_num );

   stream.writeInt( game_parameter != NULL );
   stream.writeInt( mineralResourcesDisplayed );
   for ( i = 0; i< 9; i++ )
       stream.writeInt( player[i].queuedEvents );

   for ( i = 0; i < 19; i++ )
       stream.writeInt( 0 );

   for ( i = 0; i < 8; i++ )
       stream.writeInt( getgameparameter(GameParameter(i)) );


   stream.writeInt( 0x12345678 );

       
///////////////////
// second part
//////////////////



   stream.writeString( maptitle );

   if ( campaign.avail ) {
      stream.writeInt( campaign.id );
      stream.writeChar( campaign.directaccess );
      stream.writeInt( campaign.avail );
   }

   for (int w=0; w<8 ; w++ ) 
      stream.writeString ( player[w].getName() );

   if ( !tribute.empty() ) {
       stream.writeInt ( -1 );
       tribute.write ( stream );
   } else
       stream.writeInt ( 0 );

    stream.writeInt ( 0 );

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


    for ( int ii = 0 ; ii < gameparameter_num; ii++ )
       stream.writeInt ( game_parameter[ii] );


    stream.writeString ( archivalInformation.author );
    stream.writeString ( archivalInformation.description );
    stream.writeString ( archivalInformation.tags );
    stream.writeString ( archivalInformation.requirements );
    stream.writeInt ( (unsigned int) (::time ( &archivalInformation.modifytime )));


    stream.writeInt( unitProduction.idsAllowed.size() );
    for ( int ii = 0; ii < unitProduction.idsAllowed.size(); ++ii )
       stream.writeInt ( unitProduction.idsAllowed[ii] );


    for ( int ii = 0; ii < 9; ii++ ) {
       stream.writeInt( player[ii].playTime.size() );
       for ( Player::PlayTimeContainer::iterator i = player[ii].playTime.begin(); i != player[ii].playTime.end(); ++i ) {
          stream.writeInt( i->turn );
          stream.writeInt( (unsigned int) i->date );
       }
    }

    stream.writeInt( eventID );

    stream.writeInt ( events.size());
    for ( Events::iterator i = events.begin(); i != events.end(); ++i )
       (*i)->write( stream );

    stream.writeInt( randomSeed );

    if ( network ) {
      stream.writeInt( 1 );
      network->write( stream );
    } else
      stream.writeInt( 0 );
}




MapCoordinate& GameMap::getCursor()
{
   #ifdef sgmain
   if ( actplayer >= 0 ) {
      if ( !player[actplayer].cursorPos.valid() ) {
         bool found = false;
         for ( int y = 0; y < ysize && !found; ++y )
            for ( int x = 0; x < xsize  && !found; ++x )
               if ( getField(x,y)->getContainer() )
                  if ( getField(x,y)->getContainer()->getOwner() == actplayer ) {
                     player[actplayer].cursorPos = getField(x,y)->getContainer()->getPosition();
                     found = true;
                  }
      }
      return player[actplayer].cursorPos;
   } else
      return player[0].cursorPos;
#else
   return player[8].cursorPos;
   #endif
}


void GameMap :: cleartemps( int b, int value )
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

void GameMap :: allocateFields ( int x, int y, TerrainType::Weather* terrain )
{
   field = new tfield[x*y];
   for ( int i = 0; i < x*y; i++ ) {
      if ( terrain ) {
         field[i].typ = terrain;
         field[i].setparams();
      }
      field[i].setMap ( this );
   }
   xsize = x;
   ysize = y;
   overviewMapHolder.connect();
}


void GameMap :: calculateAllObjects ( void )
{
   calculateallobjects();
}

tfield*  GameMap :: getField(int x, int y)
{
   if ((x < 0) || (y < 0) || (x >= xsize) || (y >= ysize))
      return NULL;
   else
      return (   &field[y * xsize + x] );
}

tfield*  GameMap :: getField(const MapCoordinate& pos )
{
   return getField ( pos.x, pos.y );
}

int   GameMap :: getPlayerView() const
{
#ifdef karteneditor
   return -1;
#else
   return playerView;
#endif
}


void  GameMap :: setPlayerView( int player )
{
   playerView = player;
}



bool GameMap :: isResourceGlobal ( int resource )
{
   if ( _resourcemode == 1 ) { // BI-Mode
      if ( resource == 1 ) // material
         return false;
      else
         if ( resource == 2 )
            return getgameparameter(cgp_globalfuel);
         else
            return true;
   } else {
      /*
      if ( resource == 0 )
         return getgameparameter(cgp_globalenergy);
      if ( resource == 1 )
         return false;
      if ( resource == 2 )
         return getgameparameter(cgp_globalfuel);
      */
      return false;
   }
}

int GameMap :: getgameparameter ( GameParameter num )
{
  if ( game_parameter && num < gameparameter_num ) {
     return game_parameter[num];
  } else
     if ( num < gameparameternum )
        return gameParameterSettings[num].defaultValue;
     else
        return 0;
}

void GameMap :: setgameparameter ( GameParameter num, int value )
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
              game_parameter[j] = gameParameterSettings[j].defaultValue;
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
             game_parameter[j] = gameParameterSettings[j].defaultValue;
          else
             game_parameter[j] = 0;
       game_parameter[num] = value;
       gameparameter_num = num + 1;
   }
}

void GameMap :: setupResources ( void )
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




int GameMap :: eventpassed( int saveas, int action, int mapid )
{
   return eventpassed ( (action << 16) | saveas, mapid );
}



int GameMap :: eventpassed( int id, int mapid )
{
   return 0;
}


int GameMap :: getNewNetworkID()
{
   ++unitnetworkid;
   return unitnetworkid;
}

Vehicle* GameMap :: getUnit ( Vehicle* eht, int nwid )
{
   if ( !eht )
      return NULL;
   else {
      if ( eht->networkid == nwid )
         return eht;
      else
         for ( ContainerBase::Cargo::const_iterator i = eht->getCargo().begin(); i != eht->getCargo().end(); ++i )
            if ( *i ) 
               if ( (*i)->networkid == nwid )
                  return *i;
               else {
                  Vehicle* ld = getUnit ( *i, nwid );
                  if ( ld )
                     return ld;
               }
      return NULL;
   }
}

Vehicle* GameMap :: getUnit ( int nwid )
{
   VehicleLookupCache::iterator i = vehicleLookupCache.find( nwid );
   if ( i != vehicleLookupCache.end() )
      return i->second;


   for ( int p = 0; p < 9; p++ )
      for ( Player::VehicleList::iterator i = player[p].vehicleList.begin(); i != player[p].vehicleList.end(); i++ )
         if ( (*i)->networkid == nwid ) {
            displaymessage("warning: id not registered in VehicleLookupCache!",1);
            return *i;

         }

   return NULL;
}


Vehicle* GameMap :: getUnit ( int x, int y, int nwid )
{
   tfield* fld  = getField ( x, y );
   if ( !fld )
      return NULL;

   if ( fld->vehicle && fld->vehicle->networkid == nwid )
         return fld->vehicle;
         
   if ( fld->getContainer() )
      return fld->getContainer()->findUnit( nwid );
      
   return NULL;
     
}

ContainerBase* GameMap::getContainer ( int nwid )
{
   if ( nwid > 0 )
      return getUnit(nwid);
   else {
      int x = (-nwid) & 0xffff;
      int y = (-nwid) >> 16;
      tfield* fld = getfield(x,y);
      if ( !fld )
         return NULL;

      return fld->building;
   }
}



void GameMap::beginTurn()
{
   if ( !player[actplayer].exist() )
      if ( replayinfo )
         if ( replayinfo->guidata[actplayer] ) {
            delete replayinfo->guidata[actplayer];
            replayinfo->guidata[actplayer] = NULL;
         }
            
   if ( player[actplayer].exist() && player[actplayer].stat != Player::off ) {
      sigPlayerTurnBegins( player[actplayer] );
      
//      if ( player[actplayer].stat == Player::human || player[actplayer].stat == Player::supervisor )
//         sigPlayerUserInteractionEnds( player[actplayer] );

   }
}


void GameMap::endTurn()
{
   player[actplayer].ASCversion = getNumericVersion();
   Player::PlayTime pt;
   pt.turn = time.turn();
   ::time ( &pt.date );
   player[actplayer].playTime.push_back ( pt );
   
   sigPlayerTurnEnds( player[actplayer] );

   for ( int i = 0; i < 9; ++i )
     for ( Player::BuildingList::iterator v = player[i].buildingList.begin(); v != player[i].buildingList.end(); ++v ) {
         if ( i == actplayer )
            (*v)->endOwnTurn();
         (*v)->endAnyTurn();
     }


   Player::VehicleList toRemove;
   for ( Player::VehicleList::iterator v = player[actplayer].vehicleList.begin(); v != player[actplayer].vehicleList.end(); ++v ) {
      Vehicle* actvehicle = *v;

      // Bei Aenderungen hier auch die Windanzeige dashboard.PAINTWIND aktualisieren !!!

      if (( actvehicle->height >= chtieffliegend )   &&  ( actvehicle->height <= chhochfliegend ) && ( getfield(actvehicle->xpos,actvehicle->ypos)->vehicle == actvehicle)) {
         if ( getmaxwindspeedforunit ( actvehicle ) < weather.windSpeed*maxwindspeed ){
            new Message ( getUnitReference( *v ) + " crashed because of the strong wind", this, 1<<(*v)->getOwner());
            toRemove.push_back ( *v );
         } else {

            int j = actvehicle->getTank().fuel - UnitHooveringLogic::calcFuelUsage( actvehicle );
            if (j < 0) {
               new Message ( getUnitReference( *v ) + " crashed due to lack of fuel", this, 1<<(*v)->getOwner());
               toRemove.push_back ( *v );
               // logtoreplayinfo( rpl_removeunit, actvehicle->getPosition().x, actvehicle->getPosition().y, actvehicle->networkid );
            } else {
               // logtoreplayinfo( rpl_refuel2, actvehicle->getPosition().x, actvehicle->getPosition().y, actvehicle->networkid, 1002, j, actvehicle->tank.fuel );
               actvehicle->getResource( actvehicle->getTank().fuel - j, Resources::Fuel, false);
            }
         }
      }

      if ( actvehicle )
         actvehicle->endOwnTurn();

   }

   for ( Player::VehicleList::iterator v = toRemove.begin(); v != toRemove.end(); v++ )
      delete *v;

   checkunitsforremoval();

  for ( int i = 0; i < 9; ++i ) 
     for ( Player::VehicleList::iterator v = player[i].vehicleList.begin(); v != player[i].vehicleList.end(); ++v ) 
         (*v)->endAnyTurn();

 
   if ( replayinfo )
      replayinfo->closeLogging();
      
   processJournal();   

   sigPlayerTurnHasEnded( player[actplayer] );
   
}


void GameMap::endRound()
{
    actplayer = 0;
    time.set ( time.turn()+1, 0 );

    for ( int y = 0; y < ysize; ++y )
       for ( int x = 0; x < xsize; ++x )
           getField(x,y)->endRound( time.turn() );

    for (int i = 0; i <= 7; i++) {
       if (player[i].exist() && player[i].stat != Player::off ) {

          for ( Player::VehicleList::iterator j = player[i].vehicleList.begin(); j != player[i].vehicleList.end(); j++ )
             (*j)->endRound();

          for ( Player::BuildingList::iterator j = player[i].buildingList.begin(); j != player[i].buildingList.end(); j++ )
             (*j)->endRound();
          
          typedef PointerList<ContainerBase::Work*> BuildingWork;
          BuildingWork buildingWork;

          for ( Player::BuildingList::iterator j = player[i].buildingList.begin(); j != player[i].buildingList.end(); j++ ) {
             ContainerBase::Work* w = (*j)->spawnWorkClasses( false );
             if ( w )
                buildingWork.push_back ( w );
          }

          for ( Player::VehicleList::iterator j = player[i].vehicleList.begin(); j != player[i].vehicleList.end(); j++ ) {
             ContainerBase::Work* w = (*j)->spawnWorkClasses( false );
             if ( w )
                buildingWork.push_back ( w );
          }
          
          
          bool didSomething;
          do {
             didSomething = false;
             for ( BuildingWork::iterator j = buildingWork.begin(); j != buildingWork.end(); j++ )
                if ( ! (*j)->finished() ) 
    //weatherSystem->update(time);
                   if ( (*j)->run() )
                      didSomething = true;

          } while ( didSomething );
          doresearch( this, i );
       }
    }

    if ( getgameparameter( cgp_objectGrowthMultiplier) > 0 )
       objectGrowth();
}

#include "libs/rand/rand_r.h"
#include "libs/rand/rand_r.c"

int GameMap::random( int max )
{
   return asc_rand_r( &randomSeed ) % max;
}

void GameMap::objectGrowth()
{
   typedef vector< pair<tfield*,int> > NewObjects;
   map<tfield*,int> remainingGrowthTime;

   NewObjects newObjects;
   for ( int y = 0; y < ysize; ++y )
      for ( int x = 0; x < xsize; ++x ) {
          tfield* fld = getField( x, y );
          for ( tfield::ObjectContainer::iterator i = fld->objects.begin(); i != fld->objects.end(); ++i)
             if ( i->typ->growthRate > 0 && i->remainingGrowthTime != 0 )
                for ( int d = 0; d < 6; ++d ) {
                   tfield* fld2 = getField ( getNeighbouringFieldCoordinate( MapCoordinate(x,y), d ));
                   if ( fld2 ) 
                      if ( i->typ->growOnUnits || ((!fld2->vehicle || fld2->vehicle->height >= chtieffliegend) && !fld2->building ))
                        if ( fld2->objects.empty() || getgameparameter( cgp_objectGrowOnOtherObjects ) > 0 ) {
                           double d = i->typ->growthRate * getgameparameter( cgp_objectGrowthMultiplier) / 100;
                           if ( d > 0 ) {
                              if ( d > 0.9 )
                                 d = 0.9;

                              int p = static_cast<int>(std::ceil ( double(1) / d));
                              if ( p > 1 )
                                 if ( random ( p ) == 1 )
                                    if ( i->typ->fieldModification[fld2->getweather()].terrainaccess.accessible( fld2->bdt) > 0 ) {
                                       newObjects.push_back( make_pair( fld2, i->typ->id ));
                                       i->remainingGrowthTime -= 1;
                                       remainingGrowthTime[fld2] = i->remainingGrowthTime;
                                    }
                           }
                        }
                }
      }

   for ( NewObjects::iterator i = newObjects.begin(); i != newObjects.end(); ++i )
      if ( !i->first->checkforobject( getobjecttype_byid( i->second ))) {
         if ( i->first->addobject ( getobjecttype_byid( i->second ))) {
            Object* o = i->first->checkforobject( getobjecttype_byid( i->second ));
            assert(o);
            o->remainingGrowthTime = remainingGrowthTime[i->first];
         }
      }
}

SigC::Signal1<void,GameMap&> GameMap::sigMapDeletion;

GameMap :: ~GameMap ()
{
   sigMapDeletion( *this );
   state = Destruction;

   if ( field )

      for ( int l=0 ;l < xsize * ysize ; l++ ) {
         if ( (field[l].bdt & getTerrainBitType(cbbuildingentry)).any() )
            delete field[l].building;


         Vehicle* aktvehicle = field[l].vehicle;
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

   if ( replayinfo ) {
      delete replayinfo;
      replayinfo = NULL;
   }

   if ( game_parameter ) {
      delete[] game_parameter;
      game_parameter = NULL;
   }
   
   if ( network ) {
      delete network;
      network = NULL;
   }   
      

#ifdef WEATHERGENERATOR
   delete weatherSystem;
#endif

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

bool GameMap :: ResourceTribute :: empty ( )
{
   for (int i = 0; i < 8; i++)
      for (int j = 0; j < 8; j++)
         for (int k = 0; k < 3; k++) {
            if ( avail[i][j].resource(k) )
               return false;
            if ( paid[i][j].resource(k) )
               return false;
         }

   return true;
}

const int tributeVersion = 1;  // we are counting backwards, -2 is newer than -1

void GameMap :: ResourceTribute :: read ( tnstream& stream )
{
   int version = stream.readInt();
   bool noVersion;

   if ( -version >= tributeVersion ) {
      for ( int a = 0; a < 8; ++a )
         for ( int b = 0; b < 8; ++b )
            payStatusLastTurn[a][b].read( stream );
      noVersion = false;
   } else
      noVersion = true;


   for ( int a = 0; a < 3; a++ )
      for ( int b = 0; b < 8; b++ )
         for ( int c = 0; c < 8; c++ ) {
             if ( noVersion ) {
                avail[b][c].resource(a) = version;
                noVersion = false;
             } else
                avail[b][c].resource(a) = stream.readInt();
          }

   for ( int a = 0; a < 3; a++ )
      for ( int b = 0; b < 8; b++ )
         for ( int c = 0; c < 8; c++ )
             paid[b][c].resource(a) = stream.readInt();
}

void GameMap :: ResourceTribute :: write ( tnstream& stream )
{
   stream.writeInt ( -tributeVersion );
   for ( int a = 0; a < 8; a++ )
      for ( int b = 0; b < 8; b++ )
         payStatusLastTurn[a][b].write( stream );

   for ( int a = 0; a < 3; a++ )
      for ( int b = 0; b < 8; b++ )
         for ( int c = 0; c < 8; c++ )
             stream.writeInt ( avail[b][c].resource(a) );


   for ( int a = 0; a < 3; a++ )
      for ( int b = 0; b < 8; b++ )
         for ( int c = 0; c < 8; c++ )
             stream.writeInt ( paid[b][c].resource(a) );
}



int  GameMap::resize( int top, int bottom, int left, int right )  // positive: larger
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

  for (int s = 0; s < 9; s++)
     for ( Player::BuildingList::iterator i = player[s].buildingList.begin(); i != player[s].buildingList.end(); i++ )
        (*i)->unchainbuildingfromfield();


  int newx = xsize + left + right;
  int newy = ysize + top + bottom;

  tfield* newfield = new tfield [ newx * newy ];
  for ( int i = 0; i < newx * newy; i++ )
     newfield[i].setMap ( this );

  int x;
  for ( x = ox1; x < ox2; x++ )
     for ( int y = oy1; y < oy2; y++ ) {
        tfield* org = getField ( x, y );
        tfield* dst = &newfield[ (x + left) + ( y + top ) * newx];
        *dst = *org;
     }

  tfield defaultfield;
  defaultfield.setMap ( this );
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


  for (int s = 0; s < 9; s++)
     for ( Player::BuildingList::iterator i = player[s].buildingList.begin(); i != player[s].buildingList.end(); i++ ) {
        MapCoordinate mc = (*i)->getEntry();
        mc.x += left;
        mc.y += top;
        (*i)->chainbuildingtofield ( mc );
     }

  for (int s = 0; s < 9; s++)
     for ( Player::VehicleList::iterator i = player[s].vehicleList.begin(); i != player[s].vehicleList.end(); i++ ) {
        (*i)->xpos += left;
        (*i)->ypos += top;
     }


  /*
  if (xpos + idisplaymap.getscreenxsize() > xsize)
     xpos = xsize - idisplaymap.getscreenxsize() ;
  if (ypos + idisplaymap.getscreenysize()  > ysize)
     ypos = ysize - idisplaymap.getscreenysize() ;
   */

   overviewMapHolder.resetSize();

   return 0;
}

pterraintype GameMap :: getterraintype_byid ( int id )
{
   return terrainTypeRepository.getObject_byID ( id );
}

ObjectType* GameMap :: getobjecttype_byid ( int id )
{
   return objectTypeRepository.getObject_byID ( id );
}

Vehicletype* GameMap :: getvehicletype_byid ( int id )
{
   return vehicleTypeRepository.getObject_byID ( id );
}

BuildingType* GameMap :: getbuildingtype_byid ( int id )
{
   return buildingTypeRepository.getObject_byID ( id );
}

const Technology* GameMap :: gettechnology_byid ( int id )
{
   return technologyRepository.getObject_byID ( id );
}


pterraintype GameMap :: getterraintype_bypos ( int pos )
{
   return terrainTypeRepository.getObject_byPos ( pos );
}

ObjectType* GameMap :: getobjecttype_bypos ( int pos )
{
   return objectTypeRepository.getObject_byPos ( pos );
}

Vehicletype* GameMap :: getvehicletype_bypos ( int pos )
{
   return vehicleTypeRepository.getObject_byPos ( pos );
}

BuildingType* GameMap :: getbuildingtype_bypos ( int pos )
{
   return buildingTypeRepository.getObject_byPos ( pos );
}

const Technology* GameMap :: gettechnology_bypos ( int pos )
{
   return technologyRepository.getObject_byPos ( pos );
}

int GameMap :: getTerrainTypeNum ( )
{
   return  terrainTypeRepository.getNum();
}

int GameMap :: getObjectTypeNum ( )
{
   return  objectTypeRepository.getNum();
}

int GameMap :: getVehicleTypeNum ( )
{
   return  vehicleTypeRepository.getNum();
}

int GameMap :: getBuildingTypeNum ( )
{
   return  buildingTypeRepository.getNum();
}

int GameMap :: getTechnologyNum ( )
{
   return  technologyRepository.getNum();
}

void GameMap::processJournal()
{
  if ( !newJournal.empty() ) {
     ASCString add = gameJournal;

     char tempstring[100];
     char tempstring2[100];
     sprintf( tempstring, "#color0# %s ; turn %d #color0##crt##crt#", player[actplayer].getName().c_str(), time.turn() );
     sprintf( tempstring2, "#color%d#", getplayercolor ( actplayer ));

     int fnd;
     do {
        fnd = 0;
        if ( !add.empty() )
           if ( add.find ( '\n', add.length()-1 ) != add.npos ) {
              add.erase ( add.length()-1 );
              fnd++;
           } else
             if ( add.length() > 4 )
                if ( add.find ( "#crt#", add.length()-5 ) != add.npos ) {
                  add.erase ( add.length()-5 );
                  fnd++;
                }

     } while ( fnd ); /* enddo */

     add += tempstring2;
     add += newJournal;
     add += tempstring;

     gameJournal = add;
     newJournal.erase();

     lastjournalchange.set ( time.turn(), actplayer );
  }
 
}


void GameMap :: startGame ( )
{
   time.set ( 1, 0 );

   for ( int j = 0; j < 8; j++ )
      player[j].queuedEvents = 1;

   levelfinished = false;

   for ( int n = 0; n< 8; n++ ) {
      bi_resource[n].energy = 0;
      bi_resource[n].material = 0;
      bi_resource[n].fuel = 0;
      player[n].research.setMultiplier( getgameparameter(cgp_researchOutputMultiplier) );
   }

   
   
   #ifndef karteneditor
   actplayer = -1;
   #else
   actplayer = 0;
   #endif
   
   setupResources();
   
   // calling signal
   newRound();
} 

bool GameMap::UnitProduction::check ( int id )
{
   for ( GameMap::UnitProduction::IDsAllowed::iterator i = idsAllowed.begin(); i != idsAllowed.end(); i ++ )
      if( *i == id )
         return true;

    return false;
}

VisibilityStates GameMap::getInitialMapVisibility( int player )
{
   VisibilityStates c = VisibilityStates( getgameparameter ( cgp_initialMapVisibility ));

   if ( this->player[player].ai ) {
      if ( this->player[player].ai->isRunning() ) {
         if ( c < this->player[player].ai->getVision() )
            c = this->player[player].ai->getVision();
      } else
         // this is a hack to make the replays of the AI work
         if ( c < visible_ago )
            c = visible_ago;
   }
   return c;
}

void GameMap::setPlayerMode(  Player& p, State s )
{
   p.getParentMap()->state = s;
}

int GameMap::getMemoryFootprint() const
{
   int size = sizeof(*this);
   if( replayinfo )
      for ( int i = 0; i < 8; ++i ) {
         if ( replayinfo->guidata[i] )
            size += replayinfo->guidata[i]->getMemoryFootprint();
         if ( replayinfo->map[i] )
            size += replayinfo->map[i]->getMemoryFootprint();
      }
   return size;
}



void GameMap::operator= ( const GameMap& map )
{
  throw ASCmsgException ( "GameMap::operator= undefined");
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

const int aiParamVersion = 3002;

void AiParameter::write ( tnstream& stream )
{
   stream.writeInt ( aiParamVersion );
   stream.writeInt ( lastDamage );
   stream.writeInt ( damageTime.abstime );
   stream.writeInt ( dest.x );
   stream.writeInt ( dest.y );
   stream.writeInt ( dest.getNumericalHeight() );
   stream.writeInt ( dest_nwid );
   stream.writeInt ( data );
   AiValue::write( stream );
   stream.writeInt ( task );
   stream.writeInt ( jobPos );
   stream.writeInt ( jobs.size() );
   for ( int i = 0; i < jobs.size(); i++ )
      stream.writeInt( jobs[i] );
   stream.writeInt ( resetAfterJobCompletion );
}

void AiParameter::read ( tnstream& stream )
{
   int version = stream.readInt();
   if ( version >= 3000 && version <= aiParamVersion ) {
      lastDamage = stream.readInt();
      damageTime.abstime = stream.readInt();
      int x = stream.readInt();
      int y = stream.readInt();
      int z = stream.readInt();
      dest.setnum ( x, y, z );
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
      if ( version >= 3002 )
         resetAfterJobCompletion = stream.readInt();
      else
         resetAfterJobCompletion = false;
   }
}

void AiThreat :: reset ( void )
{
   for ( int i = 0; i < aiValueTypeNum; i++ )
      threat[i] = 0;
}

AiParameter :: AiParameter ( Vehicle* _unit ) : AiValue ( log2( _unit->height ))
{
   reset( _unit );
}

void AiParameter :: resetTask ( )
{
   dest.setnum ( -1, -1, -1 );
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

void AiParameter::setJob ( Job j )
{
   int pos = 0;
   for ( JobList::iterator i = jobs.begin(); i != jobs.end(); ++i, ++pos )
      if ( *i == j ) {
         jobPos = pos;
         return;
      }

   addJob ( j, true );
}


bool AiParameter::hasJob ( AiParameter::Job j )
{
   return find ( jobs.begin(), jobs.end(), j ) != jobs.end();
}

void AiParameter :: reset ( Vehicle* _unit )
{
   unit = _unit;
   lastDamage = unit->damage;
   AiValue::reset ( log2( _unit->height ) );
   data = 0;

   clearJobs();
   resetTask();
   resetAfterJobCompletion = false;
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




GameMap :: ReplayInfo :: ReplayInfo ( void )
{
   for (int i = 0; i < 8; i++) {
      guidata[i] = NULL;
      map[i] = NULL;
   }
   actmemstream = NULL;
   stopRecordingActions = 0;
}

void GameMap :: ReplayInfo :: read ( tnstream& stream )
{
   bool loadgui[8];
   bool loadmap[8];

   for ( int i = 0; i < 8; i++ )
      loadgui[i] = stream.readInt();

   for ( int i = 0; i < 8; i++ )
      loadmap[i] = stream.readInt();

   stream.readInt(); // was: actmemstream

   for ( int i = 0; i < 8; i++ ) {
      if ( loadgui[i] ) {
         guidata[i] = new tmemorystreambuf;
         guidata[i]->readfromstream ( &stream );
      } else
         guidata[i] = NULL;

      if ( loadmap[i] ) {
         map[i] = new tmemorystreambuf;
         map[i]->readfromstream ( &stream );
      } else
         map[i] = NULL;
   }

   actmemstream = NULL;
}

void GameMap :: ReplayInfo :: write ( tnstream& stream )
{
   for ( int i = 0; i < 8; i++ )
      stream.writeInt ( guidata[i] != NULL );

   for ( int i = 0; i < 8; i++ )
      stream.writeInt ( map[i] != NULL );

   stream.writeInt ( actmemstream != NULL );

   for ( int i = 0; i < 8; i++ ) {
      // printf("GameMap :: ReplayInfo :: write  i=%d\n", i );
      if ( guidata[i] )
         guidata[i]->writetostream ( &stream );

      if ( map[i] )
         map[i]->writetostream ( &stream );
   }
}

void GameMap :: ReplayInfo :: closeLogging()
{
   if ( actmemstream ) {
      delete actmemstream;
      actmemstream = NULL;
   }
}

GameMap :: ReplayInfo :: ~ReplayInfo ( )
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

GameParameterSettings gameParameterSettings[gameparameternum ] = {
      {  "LifetimeTrack",                      1,                    1,   maxint,             true,   false,   "lifetime of tracks"},//       cgp_fahrspur                        
      {  "LifetimeBrokenIce",                  2,                    1,   maxint,             true,   false,   "freezing time of icebreaker fairway"},   //       cgp_eis,                            
      {  "MoveFromInaccessibleFields",         1,                    0,   1,                  true,   false,    "move vehicles from unaccessible fields"},   //       cgp_movefrominvalidfields,          
      {  "BuildingConstructionFactorMaterial", 100,                  0,   maxint,             true,   false,   "building construction material factor (percent)"},   //       cgp_building_material_factor,
      {  "BuildingConstructionFactorEnergy",   100,                  0,   maxint,             true,   false,   "building construction fuel factor (percent)"},   //       cgp_building_fuel_factor,
      {  "ForbidBuildingConstruction",         0,                    0,   1,                  true,   false,   "forbid construction of buildings"},   //       cgp_forbid_building_construction,
      {  "LimitUnitProductionByUnit",          0,                    0,   2,                  true,   false,   "limit construction of units by other units"},   //       cgp_forbid_unitunit_construction,   
      {  "Bi3Training",                        0,                    0,   maxunitexperience,  true,   false,   "use BI3 style training factor "},   //       cgp_bi3_training,                   
      {  "MaxMinesOnField",                    1,                    0,   maxint,             true,   false,   "maximum number of mines on a single field"},   //       cgp_maxminesonfield,                
      {  "LifetimeAntipersonnelMine",          0,                    0,   maxint,             true,   false,   "lifetime of antipersonnel mine"},   //       cgp_antipersonnelmine_lifetime,     
      {  "LifetimeAntiTankMine",               0,                    0,   maxint,             true,   false,   "lifetime of antitank mine"},   //       cgp_antitankmine_lifetime,          
      {  "LifetimeAntiSubMine",                0,                    0,   maxint,             true,   false,   "lifetime of antisub mine"},   //       cgp_mooredmine_lifetime,            
      {  "LifetimeAntiShipMine",               0,                    0,   maxint,             true,   false,   "lifetime of antiship mine"},   //       cgp_floatingmine_lifetime,          
      {  "BuildingArmorFactor",                100,                  1,   maxint,             true,   false,   "building armor factor (percent)"},   //       cgp_buildingarmor,                  
      {  "MaxBuildingRepair",                  100,                  0,   100,                true,   false,   "max building damage repair / turn"},   //       cgp_maxbuildingrepair,              
      {  "BuildingRepairCostIncrease",         100,                  1,   maxint,             true,   false,   "building repair cost increase (percent)"},   //       cgp_buildingrepairfactor,           
      {  "GlobalFuel",                         1,                    0,   1,                  true,   false,   "fuel globally available (BI Resource Mode)"},   //       cgp_globalfuel,                     
      {  "MaxTrainingExperience",              maxunitexperience,    0,   maxunitexperience,  true,   false,   "maximum experience that can be gained by training"},   //       cgp_maxtrainingexperience,          
      {  "InitialMapVisibility",               0,                    0,   2,                  true,   false,   "initial map visibility"},   //       cgp_initialMapVisibility,           
      {  "AttackPower",                        40,                   1,   100,                true,   false,   "attack power (EXPERIMENTAL!)"},   //       cgp_attackPower,                    
      {  "JammingAmplifier",                   100,                  0,   1000,               true,   false,   "jamming amplifier (EXPERIMENTAL!)"},   //       cgp_jammingAmplifier,               
      {  "JammingSlope",                       10,                   0,   100,                true,   false,   "jamming slope (EXPERIMENTAL!)"},   //       cgp_jammingSlope,                   
      {  "SupervisorMapSave",                  0,                    0,   1,                  false,  false,   "The Supervisor may save a game as new map (spying!!!)"},  //       cgp_superVisorCanSaveMap,           
      {  "ObjectsDestroyedByTerrain",          1,                    0,   1,                  true,   false,   "objects can be destroyed by terrain"},   //       cgp_objectsDestroyedByTerrain,      
      {  "TrainingIncrement",                  2,                    1,   maxunitexperience,  true,   false,   "training centers: training increment"},   //       cgp_trainingIncrement,              
      {  "ExperienceEffectDivisorAttack",      1,                    1,   10,                 false,  false,   "experience effect divisor for attack"},  //       gp_experienceDivisorAttack
      {  "DisableDirectView",                  0,                    0,   1,                  false,  false,   "disable direct View"},  //       cgp_disableDirectView
      {  "DisableUnitTrade",                   0,                    0,   1,                  false,  false,   "disable transfering units/buildings to other players"},  //       cgp_disableUnitTransfer
      {  "ExperienceEffectDivisorDefense",     1,                    1,   10,                 false,  false,   "experience effect divisor for defense"},  //       cgp_experienceDivisorDefense
      {  "DebugGameEvents",                    0,                    0,   2,                  true,   false,   "debug game events"},  //       cgp_debugEvents
      {  "ObjectGrowthRate",                   0,                    0,   maxint,             true,   false,   "Object growth rate (percentage)" },  //       cgp_objectGrowthMultiplier
      {  "ObjectsGrowOnOtherObjects",          1,                    0,   1,                  false,  false,   "Objects can grow on fields with other objects"  },  //       cgp_objectGrowOnOtherObjects
      {  "ResearchOutputMultiplier",           1,                    1,   maxint,             false,   false,   "Multiplies the research output of all labs"  }  //       cgp_researchOutputMultiplier
};

