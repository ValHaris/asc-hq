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


void MapNetwork :: searchfield ( int x, int y, int dir )
{
  int s;

   pfield fld = getfield ( x, y );
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
               fld = getfield( x, y ); 
               if ( !fld )
                  return;
            } else
               return;
   }  while ( 1 ); 

}

void MapNetwork :: searchvehicle ( int x, int y )
{
   if ( pass == 2 ) {
      pfield newfield = getfield ( x, y );
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
   pbuilding bld = getfield( x, y )->building;
   if ( !bld )
      return;

   pfield entry = bld->getEntryField();
   if ( entry->a.temp )
      return;

   if ( pass == 1 )
      checkbuilding( bld );

   entry->a.temp = 1;

   if ( !searchfinished() ) 
      for( int i = 0; i < 4; i++ )
         for ( int j = 0; j < 6; j++ ) {
            MapCoordinate mc = bld->getFieldCoordinates ( BuildingType::LocalCoordinate(i, j) );
            pfield fld2 = actmap->getField ( mc );
            if ( fld2 && fld2->building == bld )
               for ( int d = 0; d < sidenum; d++ ) {
                  int xp2 = mc.x;
                  int yp2 = mc.y;
                  getnextfield ( xp2, yp2, d );
                  pfield newfield = getfield ( xp2, yp2 );
                  if ( newfield && newfield->building != bld  && !newfield->a.temp )


                     searchfield ( xp2, yp2, d );

                  searchvehicle ( xp2, yp2 );

               } /* endfor */
         }
}


int MapNetwork :: instancesrunning = 0;

MapNetwork :: MapNetwork ( int checkInstances )
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
   for ( tmap::Player::VehicleList::iterator j = actmap->player[player].vehicleList.begin(); j != actmap->player[player].vehicleList.end(); j++ ) {
      MapCoordinate3D mc = (*j)->getPosition();
      for ( int s = 0; s < sidenum; s++ ) {
         pfield fld = actmap->getField ( getNeighbouringFieldCoordinate ( mc, s ));
         if ( fld ) {
            pbuilding bld = fld->building;
            if ( bld && bld->color == (*j)->color )
               checkvehicle ( *j );
         }
      }
   }
}

void MapNetwork :: start ( int x, int y )
{
   if ( globalsearch() == 2 ) {
      for ( int i = 0; i < 8; i++ )
         if ( actmap->player[i].exist() ) {

            for ( tmap::Player::BuildingList::iterator j = actmap->player[i].buildingList.begin(); j != actmap->player[i].buildingList.end(); j++ )
               checkbuilding(*j);

            // if ( !searchfinished() )
            searchAllVehiclesNextToBuildings ( i );

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
            pfield fld = getfield ( x, y );
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
    pfield fld = getfield ( x, y );
/*    pobject o = fld->checkforobject ( pipelineobject ) ; 
    if ( o )
       return o->dir;
    else */

    if ( fld ) {
       TerrainBits tb = getTerrainBitType(cbpipeline);
       if ( resourcetype == 0)
         tb |= getTerrainBitType(cbpowerline);

       if ( (fld->bdt & tb).any() ) {
          int d = 0;
          for ( int i = 0; i < sidenum; i++ ) {
             int xp = x;
             int yp = y;
             getnextfield ( xp, yp , i );
             pfield fld2 = getfield ( xp, yp );
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





GetResource :: GetResource ( int scope )
             : StaticResourceNet ( scope )
{
   memset ( tributegot, 0, sizeof ( tributegot ));
}


void GetResource :: checkvehicle ( pvehicle v )
{
   if ( v->color/8 == player && resourcetype == 0 ) {
      int toget = need-got;
      if ( v->tank.energy < toget )
         toget = v->tank.energy;
   
      if ( !queryonly )
         v->tank.energy -= toget;
      got += toget;
   }
}


void GetResource :: checkbuilding ( pbuilding b )
{
   if ( b->color/8 == player ) {
      if ((b->netcontrol & (cnet_stopenergyoutput << resourcetype)) == 0) {
         int toget = need-got;
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
               actmap->tribute.paid [ b->color / 8 ][ player ].resource( resourcetype ) += found;
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








void PutResource :: checkbuilding ( pbuilding b )
{
   if ( b->color/8 == player ) {
      if ((b->netcontrol & (cnet_stopenergyinput << resourcetype)) == 0) {
         int tostore = need-got;
         if ( b->gettank ( resourcetype ) - b->actstorage.resource( resourcetype ) < tostore )
            tostore = b->gettank ( resourcetype ) - b->actstorage.resource( resourcetype );
      
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





void PutTribute :: checkbuilding ( pbuilding b )
{
   if ( b->color/8 == targplayer ) {
      if ((b->netcontrol & (cnet_stopenergyinput << resourcetype)) == 0) {
         int tostore = need-got;
         if ( b->gettank ( resourcetype ) - b->actstorage.resource( resourcetype ) < tostore )
            tostore = b->gettank ( resourcetype ) - b->actstorage.resource( resourcetype );
      
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

int PutTribute :: puttribute ( pbuilding start, int resource, int _queryonly, int _forplayer, int _fromplayer, int _scope )
{
   startbuilding = start;
   targplayer = _forplayer;
   return getresource ( startbuilding->getEntry().x, startbuilding->getEntry().y, resource, 0, _queryonly, _fromplayer, _scope );
}


void transfer_all_outstanding_tribute ( void )
{
   int targplayer = actmap->actplayer;
     // for ( int player = 0; player < 8; player++ )
   if ( actmap->player[targplayer].exist() ) {
      char text[10000];
      text[0] = 0;

      for ( int player = 0; player < 8; player++ ) {
         if ( targplayer != player )
            if ( actmap->player[player].exist() ) {
               int topay[3];
               int got[3];
               for ( int resourcetype = 0; resourcetype < 3; resourcetype++ ) {
                  topay[ resourcetype ] = actmap->tribute.avail[ player ][ targplayer ].resource( resourcetype );
                  got[ resourcetype ] = 0;

                  if ( !actmap->isResourceGlobal (resourcetype) ) {
                     for ( tmap::Player::BuildingList::iterator j = actmap->player[player].buildingList.begin(); j != actmap->player[player].buildingList.end() &&  topay[resourcetype] > got[resourcetype] ; j++ ) {
                        PutTribute pt;
                        got[resourcetype] += pt.puttribute ( *j, resourcetype, 0, targplayer, player, 1 );
                     }
                  } else {
                     int i;
                     if ( actmap->bi_resource[ player ].resource(resourcetype) < topay[resourcetype] )
                        i = actmap->bi_resource[ player ].resource(resourcetype);
                     else
                        i = topay[resourcetype];
                     got [resourcetype ] = i;
                     actmap->bi_resource[ player ].resource(resourcetype) -= i;
                     actmap->bi_resource[ targplayer ].resource(resourcetype) += i;

                     actmap->tribute.avail[ player ][ targplayer ].resource( resourcetype ) -= got[resourcetype];
                     actmap->tribute.paid[ targplayer ][ player ].resource( resourcetype ) += got[resourcetype];
                  }


               }
               if ( topay[0] || topay[1] || topay[2] ) {
                  char txt1b[1000];
                  char txt_topay[100];
                  txt_topay[0] = 0;
                  int r;
                  int cnt = 0;
                  for ( r = 0; r < 3; r++ )
                     if ( topay[r] )
                        cnt++;

                  int ps = 0;
                  for ( r = 0; r < 3; r++ )
                     if ( topay[r] ) {
                        ps++;
                        char txt3[100];
                        sprintf( txt3, "%d %s", topay[r], resourceNames[r] );
                        if ( ps>1 && ps < cnt )
                           strcat ( txt_topay, ", ");
                        if ( ps>1 && ps == cnt )
                           strcat ( txt_topay, " and ");
                        strcat ( txt_topay, txt3 );
                     }

                  char txt_got[100];
                  txt_got[0] = 0;
                  cnt = 0;
                  ps = 0;
                  for ( r = 0; r < 3; r++ )
                     if ( got[r] )
                        cnt++;

                  for ( r = 0; r < 3; r++ )
                     if ( got[r] ) {
                        ps++;
                        char txt3[100];
                        sprintf( txt3, "%d %s", got[r], resourceNames[r] );
                        if ( ps>1  && ps < cnt )
                           strcat ( txt_got, ", ");
                        if ( ps>1 && ps == cnt )
                           strcat ( txt_got, " and ");
                        strcat ( txt_got, txt3 );
                     }
                  if ( !txt_got[0] )
                     strcpy ( txt_got, "nothing" );

                  const char* sp = getmessage( 10020 );
                  sprintf ( txt1b, sp, txt_topay, actmap->player[player].getName().c_str(), txt_got );
                  strcat ( text, txt1b );
               }
            }
      }
      if ( text[0] )
         new Message ( text, actmap, 1 << targplayer );

   }
}


void GetResourceCapacity :: checkbuilding ( pbuilding b )
{
   if ( b->color/8 == player ) {
      int t = b->gettank ( resourcetype );
      if ( t > maxint - got )
         got = maxint;
      else
         got += t;
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




void GetResourcePlus :: checkvehicle ( pvehicle v )
{/*
   if ( resourcetype == 0 )
      if ( v->getGeneratorStatus() )
         got += v->typ->tank.energy;
         */
}


void GetResourcePlus :: checkbuilding ( pbuilding bld )
{
   if ( bld->color/8 == player )
      got += bld->getResourcePlus().resource( resourcetype );
}





void GetResourceUsage :: checkbuilding ( pbuilding b )
{
   if ( b->color/8 == player ) 
      got += b->getResourceUsage().resource( resourcetype );
}

