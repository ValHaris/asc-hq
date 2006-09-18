/***************************************************************************
                          resourcenet.cpp  -  description
                             -------------------
    begin                : Tue Jan 28 2001
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/*! \file resourcenet.cpp
    \brief Handling the connection of buildings by pipelines, powerlines etc.
*/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "typen.h"
#include "vehicletype.h"
#include "buildingtype.h"
#include "spfst.h"
#include "resourcenet.h"
#include "mapalgorithms.h"
#include "dlg_box.h"
#include "dialog.h"
#include "cannedmessages.h"

SigC::Signal0<void> tributeTransferred;

void MapNetwork :: searchfield ( int x, int y, int dir )
{
  int s;

   tfield* fld = actmap->getField ( x, y );
   if ( !fld )
      return;

   int arr[sidenum]; 

   do { 

      if ( fld->a.temp )
         return;

      #ifdef netdebug
      cursor.gotoxy ( a, b );
      displaymap();
      #endif

      if ( fld->building ) { 
         searchbuilding ( x, y );
         return;
      }

      fld->a.temp = 1;

      int d = fieldavail( x, y );
      if ( d <= 0 )
         return;

      searchvehicle ( x, y );

      int olddir = dir + sidenum/2; 
      while (olddir >= sidenum ) 
         olddir -= sidenum; 

         int r = 0; 
         for (s = 0; s < sidenum; s++) { 
            if ( (d & (1 << s))  &&  ( s != olddir )) {
               arr[ r ] = s; 
               r++;
            } 
         } 


         if (r > 1) {      // Kreuzungsstelle 
            for ( s = 0; s < r; s++) { 
               int nx = x;
               int ny = y;
               getnextfield ( nx, ny, arr[s] );
               searchfield( nx, ny, arr[s] );
               if ( searchfinished() )
                  return ;
            } 
            return;
         } else  
            if ( r == 1 ) {
               dir = arr[0];
               getnextfield ( x, y, dir );
               fld = actmap->getField( x, y ); 
               if ( !fld )
                  return;
            } else
               return;
   }  while ( 1 ); 

}

void MapNetwork :: searchvehicle ( int x, int y )
{
   if ( pass == 2 ) {
      tfield* newfield = actmap->getField ( x, y );
      if ( newfield )
         if ( !newfield->a.temp2 )
           if ( newfield->vehicle ) {
              checkvehicle ( newfield->vehicle );
              newfield->a.temp2 = 1;
           }
   }
}


void MapNetwork :: searchbuilding ( int x, int y )
{
   Building* bld = actmap->getField( x, y )->building;
   if ( !bld )
      return;

   tfield* entry = bld->getEntryField();
   if ( entry->a.temp )
      return;

   if ( pass == 1 )
      checkbuilding( bld );

   entry->a.temp = 1;

   if ( !searchfinished() ) 
      for( int i = 0; i < 4; i++ )
         for ( int j = 0; j < 6; j++ ) {
            MapCoordinate mc = bld->getFieldCoordinates ( BuildingType::LocalCoordinate(i, j) );
            tfield* fld2 = actmap->getField ( mc );
            if ( fld2 && fld2->building == bld )
               for ( int d = 0; d < sidenum; d++ ) {
                  int xp2 = mc.x;
                  int yp2 = mc.y;
                  getnextfield ( xp2, yp2, d );
                  tfield* newfield = actmap->getField ( xp2, yp2 );
                  if ( newfield && newfield->building != bld  && !newfield->a.temp )
                     searchfield ( xp2, yp2, d );

                  searchvehicle ( xp2, yp2 );

               } /* endfor */
         }
}


int MapNetwork :: instancesrunning = 0;

MapNetwork :: MapNetwork ( GameMap* gamemap, int checkInstances ) : actmap ( gamemap )
{
   if ( checkInstances ) {
      if ( instancesrunning )
         displaymessage(" fatal error at MapNetwork; there are other running instances ", 2 );
   } 
   // else displaymessage("warning: Mapnetwork instance check disabled !", 1 );
// this could be resolved by using the different bits of field->a.temp 

   instancesrunning++;
   pass = 1;
}


MapNetwork :: ~MapNetwork ()
{ 
   instancesrunning--;
}



void MapNetwork :: searchAllVehiclesNextToBuildings ( int player )
{
   pass++;
   for ( Player::VehicleList::iterator j = actmap->player[player].vehicleList.begin(); j != actmap->player[player].vehicleList.end(); j++ ) {
      MapCoordinate3D mc = (*j)->getPosition();
      for ( int s = 0; s < sidenum; s++ ) {
         tfield* fld = actmap->getField ( getNeighbouringFieldCoordinate ( mc, s ));
         if ( fld ) {
            Building* bld = fld->building;
            if ( bld && bld->color == (*j)->color ) {
               tfield* fld2 = actmap->getField( (*j)->getPosition());
               if ( !fld2->a.temp2 ) {
                  fld2->a.temp2 = 1;
                  checkvehicle ( *j );
               }
            }
         }
      }
   }
}

void MapNetwork :: start ( int x, int y )
{
   if ( globalsearch() == 2 ) {
      for ( int i = 0; i < 8; i++ )
         if ( actmap->player[i].exist() ) {

            for ( Player::BuildingList::iterator j = actmap->player[i].buildingList.begin(); j != actmap->player[i].buildingList.end(); j++ )
               checkbuilding(*j);

            // if ( !searchfinished() )
            actmap->cleartemps(7);
            searchAllVehiclesNextToBuildings ( i );
            actmap->cleartemps(7);

         }
   } else 
      if ( globalsearch() == 1 ) {
         actmap->cleartemps(7);
         startposition.x = x;
         startposition.y = y;
         searchfield ( x, y, -1 );
         actmap->cleartemps(7);
         if ( !searchfinished() ) {
            pass++;
            startposition.x = x;
            startposition.y = y;
            searchfield ( x, y, -1 );
            actmap->cleartemps(7);
         }
      } else  
         if ( globalsearch() == 0 ) {
            tfield* fld = actmap->getField ( x, y );
            if ( fld )
               if ( fld->building ) {
                  if ( pass == 1 )
                     checkbuilding( fld->building );
               } else
                  if ( fld->vehicle )

                     if ( pass == 2 )
                        checkvehicle ( fld->vehicle );
         }
}



int ResourceNet :: fieldavail ( int x, int y )
{
    tfield* fld = actmap->getField ( x, y );
/*    Object* o = fld->checkforobject ( pipelineobject ) ; 
    if ( o )
       return o->dir;
    else */

    if ( fld ) {
       TerrainBits tb = getTerrainBitType(cbpipeline);
       assert( resourcetype >= 0 );
       
       if ( resourcetype == 0)
         tb |= getTerrainBitType(cbpowerline);

       if ( (fld->bdt & tb).any() ) {
          int d = 0;
          for ( int i = 0; i < sidenum; i++ ) {
             int xp = x;
             int yp = y;
             getnextfield ( xp, yp , i );
             tfield* fld2 = actmap->getField ( xp, yp );
             if ( fld2 )
                if ( (fld2->bdt & tb).any() ||  fld2->building )
                   d |= ( 1 << i );
          }
          return d;

       } else
          return 0;
    }
    return 0;

}


int StaticResourceNet :: getresource ( int x, int y, int resource, int _need, int _queryonly, int _player, int _scope )
{
   if ( actmap->isResourceGlobal ( resource ))
      scope = 3;
   else
      scope = _scope;

   player = _player;
   resourcetype = resource;
   got = 0;
   need = _need;
   queryonly = _queryonly;

   if (scope == 3 && player == 8 )     // neutral player has no map-wide pool
      scope = 0;

   start ( x , y );
   return got;
}


int StaticResourceNet :: searchfinished ( void )
{
   return got >= need;
}





GetResource :: GetResource ( GameMap* gamemap, int scope )
             : StaticResourceNet ( gamemap, scope )
{
   memset ( tributegot, 0, sizeof ( tributegot ));
}


void GetResource :: checkvehicle ( Vehicle* v )
{
   if ( v->color/8 == player && resourcetype == 0 )
      got += v->getResource( need-got, Resources::Energy, queryonly, 1, player );
}


void GetResource :: checkbuilding ( Building* b )
{
   if ( b->color/8 == player ) {
      if ((b->netcontrol & (cnet_stopenergyoutput << resourcetype)) == 0) {
         int toget = need-got;
         if ( b->actstorage.resource( resourcetype ) < 0 ) {
            displaymessage("map corruption detected; building %s storage for %d/%d is negative!",1,resourceNames[resourcetype],b->getEntry().x, b->getEntry().y );
            b->actstorage.resource( resourcetype ) = 0;
         }


         if ( b->actstorage.resource( resourcetype ) < toget )
            toget = b->actstorage.resource( resourcetype );

         if ( !queryonly )
            b->actstorage.resource( resourcetype ) -= toget;
         got += toget;
      }
   } else
      if ( b->color/8 < 8 ) { // no neutral buildings

         // Codeguard reports "Pointer arithmetic in invalid memory". Whats wrong here ?
         // Resources tttt = actmap->tribute.avail[ b->color / 8 ][ player ];
         int gettable = actmap->tribute.avail[ b->color / 8 ][ player ].resource(resourcetype) - tributegot[ resourcetype ][ b->color / 8];
         if ( gettable > 0 ) {
            int toget = need-got;
            if ( toget > gettable )
               toget = gettable;

            // int found = b->getResource( toget, resourcetype, queryonly );
            int found = b->actstorage.resource(resourcetype);
            if ( toget < found )
               found = toget;

            if ( !queryonly )
               b->actstorage.resource(resourcetype) -= found;

            tributegot[ resourcetype ][ b->color / 8] += found;

            if ( !queryonly ) {
               actmap->tribute.avail[ b->color / 8 ][ player ].resource( resourcetype ) -= found;
               actmap->tribute.paid [ player ][ b->color / 8 ].resource( resourcetype ) += found;
               if ( found )
                  tributeTransferred();
            }

            got += found;
         }
      }
}

void GetResource :: start ( int x, int y )
{
   if ( scope == 3 ) {
      got = need;
      if ( got > actmap->bi_resource[player].resource( resourcetype ) )
         got = actmap->bi_resource[player].resource( resourcetype );

      if ( !queryonly )
         actmap->bi_resource[player].resource( resourcetype ) -= got;

      if ( resourcetype == 0 )
         searchAllVehiclesNextToBuildings ( player );

   } else
      MapNetwork :: start ( x, y );
}








void PutResource :: checkbuilding ( Building* b )
{
   if ( b->color/8 == player ) {
      if ((b->netcontrol & (cnet_stopenergyinput << resourcetype)) == 0) {
         int tostore = need-got;

         if ( b->actstorage.resource( resourcetype ) < 0 ) {
            displaymessage("map corruption detected; building %s storage for %d/%d is negative!",1,resourceNames[resourcetype],b->getEntry().x, b->getEntry().y );
            b->actstorage.resource( resourcetype ) = 0;
         }
         
         if ( b->getStorageCapacity().resource( resourcetype ) - b->actstorage.resource( resourcetype ) < tostore )
            tostore = b->getStorageCapacity().resource( resourcetype ) - b->actstorage.resource( resourcetype );
      
         if ( !queryonly )
            b->actstorage.resource( resourcetype ) += tostore;
         got += tostore;
      }
   }
}


void PutResource :: start ( int x, int y )
{
   if ( scope == 3 ) {

      got = need;
      if ( got > maxint - actmap->bi_resource[player].resource( resourcetype ) )
         got = maxint - actmap->bi_resource[player].resource( resourcetype );

      if ( !queryonly )
         actmap->bi_resource[player].resource( resourcetype ) += got;

   } else
      MapNetwork :: start ( x, y );
}





void PutTribute :: checkbuilding ( Building* b )
{
   if ( b->color/8 == targplayer ) {
      if ((b->netcontrol & (cnet_stopenergyinput << resourcetype)) == 0) {
         int tostore = need-got;
         if ( b->getStorageCapacity().resource( resourcetype ) - b->actstorage.resource( resourcetype ) < tostore )
            tostore = b->getStorageCapacity().resource( resourcetype ) - b->actstorage.resource( resourcetype );
      
         if ( !queryonly ) {
            b->actstorage.resource( resourcetype ) += tostore;
            actmap->tribute.avail[ player ][ targplayer ].resource( resourcetype ) -= tostore;
            actmap->tribute.paid [ targplayer ][ player ].resource( resourcetype ) += tostore;
         }
         got += tostore;
      }
   }
}


void PutTribute :: start ( int x, int y )
{
   int pl = targplayer;
   if ( pl == -1 )
      targplayer = 0;

   do {
      if ( targplayer != player )
         if ( actmap->player[targplayer].exist() ) {
            need = actmap->tribute.avail[ player ][ targplayer ].resource( resourcetype );
            if ( need > 0 ) {
               if ( scope == 3 ) {

                  got = need;
                  if ( got > maxint - actmap->bi_resource[targplayer].resource( resourcetype ) )
                     got = maxint - actmap->bi_resource[targplayer].resource( resourcetype );

                  if ( got > actmap->bi_resource[player].resource( resourcetype ) )
                     got = actmap->bi_resource[player].resource( resourcetype );

                  if ( !queryonly ) {
                     actmap->bi_resource[targplayer].resource( resourcetype ) += got;
                     actmap->bi_resource[player].resource( resourcetype ) -= got;

                     actmap->tribute.avail[ player ][ targplayer ].resource( resourcetype ) -= got;
                     actmap->tribute.paid [ targplayer ][ player ].resource( resourcetype ) += got;
                  }

               } else {
                  int avail = startbuilding->getResource ( need, resourcetype, 1, 0 );
                  if ( need > avail )
                     need = avail;
                  MapNetwork :: start ( x, y );
                  if ( !queryonly )
                     startbuilding->getResource ( got, resourcetype, queryonly, 0 );

               }
            }
         }
      targplayer++;
   } while ( targplayer < 8  && pl == -1 );
}

int PutTribute :: puttribute ( Building* start, int resource, int _queryonly, int _forplayer, int _fromplayer, int _scope )
{
   startbuilding = start;
   targplayer = _forplayer;
   return getresource ( startbuilding->getEntry().x, startbuilding->getEntry().y, resource, 0, _queryonly, _fromplayer, _scope );
}


void transfer_all_outstanding_tribute ( Player& player )
{
   int targplayer = player.getPosition();
   GameMap* actmap = player.getParentMap();
   
   if ( actmap->player[targplayer].exist() ) {
      ASCString text;

      for ( int player = 0; player < 8; player++ ) {
         if ( targplayer != player )
            if ( actmap->player[player].exist() ) {
               int topay[3];
               int got[3];
               for ( int resourcetype = 0; resourcetype < 3; resourcetype++ ) {
                  got[ resourcetype ] = actmap->tribute.paid[ targplayer ][ player ].resource( resourcetype ) - actmap->tribute.payStatusLastTurn[ targplayer ][ player ].resource( resourcetype );
                  topay[ resourcetype ] = actmap->tribute.avail[ player ][ targplayer ].resource( resourcetype ) + got[ resourcetype ];

                  if ( !actmap->isResourceGlobal (resourcetype) ) {
                     for ( Player::BuildingList::iterator j = actmap->player[player].buildingList.begin(); j != actmap->player[player].buildingList.end() &&  topay[resourcetype] > got[resourcetype] ; j++ ) {
                        PutTribute pt ( actmap );
                        got[resourcetype] += pt.puttribute ( *j, resourcetype, 0, targplayer, player, 1 );
                     }
                  } else {
                     int i;
                     if ( actmap->bi_resource[ player ].resource(resourcetype) < topay[resourcetype] )
                        i = actmap->bi_resource[ player ].resource(resourcetype);
                     else
                        i = topay[resourcetype];
                     got [resourcetype ] += i;
                     actmap->bi_resource[ player ].resource(resourcetype) -= i;
                     actmap->bi_resource[ targplayer ].resource(resourcetype) += i;

                     actmap->tribute.avail[ player ][ targplayer ].resource( resourcetype ) -= got[resourcetype];
                     actmap->tribute.paid[ targplayer ][ player ].resource( resourcetype ) += got[resourcetype];
                  }
               }
               actmap->tribute.payStatusLastTurn[ targplayer ][ player ] = actmap->tribute.paid[ targplayer ][ player ];
               if ( topay[0] || topay[1] || topay[2] || got[0] || got[1] || got[2]) {
                  Resources tp ( topay[0], topay[1], topay[2] );
                  Resources gt ( got[0],   got[1],   got[2]   );

                  ASCString topayStr = tp.toString();

                  ASCString gotStr;
                  if ( gt == tp )
                     gotStr = "all";
                  else {
                     gotStr = gt.toString();
                     if ( gotStr.empty() )
                        gotStr = "nothing";
                  }

                  ASCString msg;
                  msg.format ( getmessage( 10020 ) , actmap->player[player].getName().c_str(), topayStr.c_str(), gotStr.c_str() );

                  text += msg;


                  // next message, this time for the providing player

                  if ( got[0] || got[1] || got[2] ) {
                     msg = "";
                     gotStr = gt.toString();
                     msg.format ( getmessage( 10021 ) , gotStr.c_str(), actmap->player[targplayer].getName().c_str());
                     new Message ( msg, actmap, 1 << player );
                  }


               }

               if ( got[0] || got[1] || got[2] )
                 tributeTransferred();

            }
      }
      if ( !text.empty() )
         new Message ( text, actmap, 1 << targplayer );

   }
}


void GetResourceCapacity :: checkbuilding ( Building* b )
{
   if ( b->color/8 == player ) {
      if ((b->netcontrol & (cnet_stopenergyinput << resourcetype)) == 0) {
         int t = b->getStorageCapacity().resource( resourcetype );
         if ( t > maxint - got )
            got = maxint;
         else
            got += t;
      }
   }
}


void GetResourceCapacity :: start ( int x, int y )
{
   if ( scope == 3 ) {
      got = maxint;
   } else
      MapNetwork :: start ( x, y );
}










int ResourceChangeNet :: getresource ( int x, int y, int resource, int _player, int _scope )
{
   if ( (_scope > 0)  &&  (actmap->isResourceGlobal ( resource )) )
      scope = 2;
   else
      scope = _scope;

   player = _player;
   resourcetype = resource;
   got = 0;
   start ( x , y );
   return got;
}




void GetResourcePlus :: checkvehicle ( Vehicle* v )
{/*
   if ( resourcetype == 0 )
      if ( v->getGeneratorStatus() )
         got += v->typ->tank.energy;
         */
}


void GetResourcePlus :: checkbuilding ( Building* bld )
{
   if ( bld->color/8 == player )
      got += bld->getResourcePlus().resource( resourcetype );
}





void GetResourceUsage :: checkbuilding ( Building* b )
{
   if ( b->color/8 == player ) 
      got += b->getResourceUsage().resource( resourcetype );
}



bool compareMapResources( GameMap* currentMap, GameMap* replaymap, int player, ASCString* log )
{
   ASCString s;
   bool diff  = false;
   for ( int r = 0; r < 3; ++r ) {
      if ( currentMap->isResourceGlobal( r )) {
         if ( currentMap->bi_resource[player].resource(r) != replaymap->bi_resource[player].resource(r) ) {
            diff = true;
            if ( log ) {
               s.format ( "Global resource mismatch: %d %s available after replay, but %d available in actual map\n", replaymap-> bi_resource[player].resource(r), resourceNames[r], currentMap->bi_resource[player].resource(r) );
               *log += s;
            }
         }
      } else {
         GetConnectedBuildings::BuildingContainer cb;
         for ( Player::BuildingList::iterator b = currentMap->player[player].buildingList.begin(); b != currentMap->player[player].buildingList.end(); ++b ) {
            Building* b1 = *b;
            ContainerBase* b2 = replaymap->getContainer( b1->getIdentification() );
            if ( !b1 || !b2 ) {
               if ( log ) {
                  s.format ( "Building missing! \n");
                  *log += s;
               }
            } else {
               if ( find ( cb.begin(), cb.end(), b1 ) == cb.end()) {
                  int ab1 = b1->getResource( maxint, r, true, 1, player );
                  int ab2 = b2->getResource( maxint, r, true, 1, player);
                  if ( ab1 != ab2 ) {
                     diff = true;
                     if ( log ) {
                        s.format ( "Building (%d,%d) resource mismatch: %d %s available after replay, but %d available in actual map\n", b1->getPosition().x, b1->getPosition().y, ab1, resourceNames[r], ab2 );
                        *log += s;
                     }
                  }
                  cb.push_back ( b1 );
                  GetConnectedBuildings::BuildingContainer cbl;
                  GetConnectedBuildings gcb ( cbl, b1->getMap(), r );
                  gcb.start ( b1->getPosition().x, b1->getPosition().y );
                  cb.insert ( cb.end(), cbl.begin(), cbl.end() );
               }
            }
         }
      }
      for ( Player::VehicleList::iterator v = currentMap->player[player].vehicleList.begin(); v != currentMap->player[player].vehicleList.end(); ++v ) {
         Vehicle* v1 = *v;
         Vehicle* v2 = replaymap->getUnit( v1->networkid );
         if ( !v1 || !v2 ) {
            if ( log ) {
               s.format ( "Vehicle missing! \n");
               *log += s;
            }
         } else {
            int av1 = v1->getResource( maxint, r, true, 1, player );
            int av2 = v2->getResource( maxint, r, true, 1, player );
            if ( av1 != av2 ) {
               diff = true;
               if ( log ) {
                  s.format ( "Vehicle (%d,%d) resource mismatch: %d %s available after replay, but %d available in actual map\n", v1->getPosition().x, v1->getPosition().y, av2, resourceNames[r], av1 );
                  *log += s;
               }
            }
         }
      }

   }
   if ( currentMap->player[player].vehicleList.size() != replaymap->player[player].vehicleList.size() ) {
      diff = true;
      if ( log ) {
         s.format ( "The number of units differ. Replay: %d ; actual map: %d", replaymap->player[player].vehicleList.size(), currentMap->player[player].vehicleList.size());
         *log += s;

         GameMap* more;
         GameMap* less;
         if ( currentMap->player[player].vehicleList.size() > replaymap->player[player].vehicleList.size() ) {
            more = currentMap;
            less = replaymap;
         } else {
            more = replaymap;
            less = currentMap;
         }

         for ( Player::VehicleList::iterator i = more->player[player].vehicleList.begin(); i != more->player[player].vehicleList.end(); ++i )
            if ( !less->getUnit( (*i)->networkid )) {
               s.format( "Type: %s at %d/%d\n", (*i)->getName().c_str(), (*i)->getPosition().x, (*i)->getPosition().y );
               *log += s;
            }


      }
   }
   if ( currentMap->player[player].buildingList.size() != replaymap->player[player].buildingList.size() ) {
      diff = true;
      if ( log ) {
         s.format ( "The number of buildings differ. Replay: %d ; actual map: %d", replaymap->player[player].buildingList.size(), currentMap->player[player].buildingList.size());
         *log += s;
      }
   }

   if ( currentMap->player[player].research.progress != replaymap->player[player].research.progress ) {
      diff = true;
      if ( log ) {
         s.format ( "Research points mismatch! Replay: %d ; actual map: %d", replaymap->player[player].research.progress, currentMap->player[player].research.progress);
         *log += s;
      }
   }

   sort ( currentMap->player[player].research.developedTechnologies.begin(), currentMap->player[player].research.developedTechnologies.end() );
   sort ( replaymap->player[player].research.developedTechnologies.begin(), replaymap->player[player].research.developedTechnologies.end() );
   if ( replaymap->player[player].research.developedTechnologies.size() != currentMap->player[player].research.developedTechnologies.size() ) {
      diff = true;
      if ( log ) {
         s.format ( "Number of developed technologies differ !\n" );
         *log += s;
      }
   } else {
      for ( int i = 0; i < replaymap->player[player].research.developedTechnologies.size(); ++i )
         if ( replaymap->player[player].research.developedTechnologies[i] != currentMap->player[player].research.developedTechnologies[i] ) {
         diff = true;
         if ( log ) {
            s.format ( "Different technologies developed !\n" );
            *log += s;
         }
         }
   }

   for ( Player::BuildingList::iterator b = currentMap->player[player].buildingList.begin(); b != currentMap->player[player].buildingList.end(); ++b ) {
      Building* b1 = *b;
      Building* b2 = dynamic_cast<Building*>(replaymap->getContainer( b1->getIdentification() ));
      if ( !b1 || !b2 ) {
         if ( log ) {
            s.format ( "Building missing! \n");
            *log += s;
         }
      } else {
         bool mismatch = false;
         for ( int i = 0; i < b1->unitProduction.size(); ++i )
            if ( b1->unitProduction[i] ) {
            bool found = false;
            for ( int j = 0; j < b2->unitProduction.size(); ++j)
               if ( b2->unitProduction[j] == b1->unitProduction[i] )
                  found = true;
            if ( !found)
               mismatch = true;
            }

            for ( int j = 0; j < b2->unitProduction.size(); ++j )
               if ( b2->unitProduction[j] ) {
               bool found = false;
               for ( int i = 0; i < b1->unitProduction.size(); ++i)
                  if ( b1->unitProduction[i] == b2->unitProduction[j] )
                     found = true;
               if ( !found)
                  mismatch = true;
               }

               if ( mismatch ) {
                  diff = true;
                  if ( log ) {
                     s.format ( "Building (%d,%d) production line mismatch !\n", b1->getPosition().x, b1->getPosition().y );
                     *log += s;
                  }
               }
      }
   }

   return diff;
}

