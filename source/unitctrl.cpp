//     $Id: unitctrl.cpp,v 1.1 2000-04-27 16:25:31 mbickel Exp $
//
//     $Log: not supported by cvs2svn $

/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-1999  Martin Bickel  and  Marc Schellenberger

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


#include "unitctrl.h"
#include "controls.h"
#include "dialog.h"
#include "attack.h"
#include "stack.h"
#include "missions.h"



PendingVehicleActions pendingVehicleActions;



FieldList :: FieldList ( void )
{
   fieldnum = 0;
   localmap = NULL;
}

int FieldList :: getFieldNum ( void )
{
   return fieldnum;
}

pfield FieldList :: getField ( int num )
{
   if ( num < fieldnum && num >= 0 )
      return getfield ( xpos[num], ypos[num] );
   else
      return NULL;
}

void FieldList :: getFieldCoordinates ( int num, int* x, int* y )
{
   if ( num < fieldnum && num >= 0 ) {
      *x = xpos[num];
      *y = ypos[num];
   } else {
      *x = -1;
      *y = -1;
   }
}

void FieldList :: addField ( int x, int y )
{
   xpos[fieldnum] = x;
   ypos[fieldnum] = y;
   fieldnum++;
}

void FieldList :: setMap ( pmap map )
{
   localmap = map;
}

pmap FieldList :: getMap ( void )
{
   return localmap;
}

int FieldList :: isMember ( int x, int y )
{
   for ( int i = 0; i < fieldnum; i++ )
      if ( xpos[i] == x && ypos[i] == y )
         return 1;
   return 0;
}





VehicleAction :: VehicleAction ( VehicleActionType _actionType, PPendingVehicleActions _pva  )
{
   if ( _pva ) {
      if ( _pva->action )
         displaymessage ( " VehicleAction :: VehicleAction / another action is running !", 2 );

      _pva->action = this;
      _pva->actionType = _actionType;
   }
   pva = _pva;
   actionType = _actionType;
}

void VehicleAction :: registerPVA ( VehicleActionType _actionType, PPendingVehicleActions _pva )
{
   if ( _pva ) {
      if ( _pva->action )
         displaymessage ( " VehicleAction :: VehicleAction / another action is running !", 2 );

      _pva->action = this;
      _pva->actionType = _actionType;
   }
   pva = _pva;
   actionType = _actionType;
}


VehicleAction :: ~VehicleAction (  )
{
   if ( pva ) {
      pva->action = NULL;
      pva->actionType = 0;
   }
}



VehicleMovement :: VehicleMovement ( MapDisplayInterface* md, PPendingVehicleActions _pva )
                 : BaseVehicleMovement ( vat_move, _pva )
{
   status = 0;
   mapDisplay = md;
   if ( pva )
      pva->move = this;
}

void VehicleMovement :: registerPVA ( VehicleActionType _actionType, PPendingVehicleActions _pva )
{
   VehicleAction::registerPVA ( _actionType, _pva );
   if ( pva )
      pva->move = this;
}


VehicleMovement :: ~VehicleMovement ( )
{
   if ( pva )
      pva->move = NULL;
}

int VehicleMovement :: available ( pvehicle veh ) const
{
   if ( status == 0 )
     if ( veh ) 
       if ( veh->color == actmap->actplayer * 8 ) 
          if ( veh->movement >= minmalq ) 
             if ( terrainaccessible ( getfield ( veh->xpos, veh->ypos ), veh ) || actmap->getgameparameter( cgp_movefrominvalidfields) )
                return 1; 

   return 0;
}


int VehicleMovement :: execute ( pvehicle veh, int x, int y, int step, int height, int param2 )
{
   if ( step != status )
      return -1;

   if ( status == 0 ) {
      vehicle = veh ;
      if ( !vehicle ) {
         status = -101;
         return status;
      }
      newheight = height;

      searchstart ( veh->xpos, veh->ypos, height );
      if ( search.fieldnum <= 0 ) {
         status =  -107;
         return status;
      }

      status = 2;
      return status;
  } else
   if ( status == 2 ) {
      if ( height == -1)
         height = newheight;
      else
         displaymessage ( "VehicleMovement :: execute / unit heights not matching !", 2 );

      if ( !reachableFields.isMember ( x, y )) {
         status = -105;
         return status;
      }

      fieldReachableRek.run( x, y, vehicle, 1, height, &path );

      status = 3;
      return status;
   } else
    if ( status == 3 ) {
       if ( !path.isMember ( x, y )) {
          status = -105;
          return status;
       }

       int x1 = vehicle->xpos;
       int y1 = vehicle->ypos;
 
      
       if ( newheight != -1 && newheight != vehicle->height )
          logtoreplayinfo ( rpl_changeheight, x1, y1, x, y, vehicle->networkid, (int) vehicle->height, (int) newheight );
       else 
          logtoreplayinfo ( rpl_move2, x1, y1, x, y, vehicle->networkid, (int) vehicle->height );
 
       if ( mapDisplay )
          mapDisplay->startAction();
       int stat = moveunitxy( x, y );
       if ( mapDisplay )
          mapDisplay->stopAction();

       if ( stat < 0 ) 
          status = stat;
       else
          status = 1000;

       return status;
      
    } else
       status = 0;
  return status;
}




#ifdef HEXAGON
 int sef_dirnum = 3;
 static const  int          sef_directions[3]  = {0, 1, 5 };
 static const  int          sef_searchorder[6]  = {0, 1, 2, 3, 4, 5 };
#else
 int sef_dirnum = 5;
 static const  int          sef_directions[5]  = {0, 1, 7, 2, 6};
 static const  int          sef_searchorder[8]  = {0, 2, 4, 6, 1, 3, 5, 7};
#endif


void VehicleMovement :: searchmove(int         x,
                             int         y,
                             int         direc,
                             int         streck,
                             int         fuelneeded)
{ 
   search.tiefe++;

   int ox = x; 
   int oy = y; 

   getnextfield( x, y, direc);

   if ((x == search.startx) && (y == search.starty)) 
      return;

   pfield fld = getfield( x, y ); 
   if ( !fld )
      return;

   int c = fieldaccessible ( fld, vehicle, search. height ); 

   if ( c == 0) 
      return;
   else { 
      int mm1;   // fuelconsumption
      int mm2;   // movementdist

      calcmovemalus(ox,oy,x,y,vehicle,direc, mm1, mm2);

      streck -= mm2;
      fuelneeded   += mm1;
   } 

   if (streck < 0) 
      return;

   if ( fuelneeded * vehicle->typ->fuelconsumption / 8 > vehicle->fuel)   
      return;

   if (search.mode == 1) { 

      #ifdef fastsearch   
      if ((fld->special & cbmovetempb) != 0) {
         if ((((fld->special & cbmovetempb) >> cbmovetempv) != maindir / 2 + 1) || ((maindir & 1) == 0))
            return;
      } 
      #endif   

      if ( fld->a.temp < streck + 1 ) { 
         fld->a.temp = streck + 1; 
         if ( c == 2 ) {
            reachableFields.addField ( x, y );
            search.fieldnum++;
         }
      } 
      else 
         return;
   } 
   else { 
      // displaymessage (" was soll denn der K„se hier ?? ", 2 );
      /*
      fld->special |= ((direc / 2 + 1) << cbmovetempv);
      fld->a.temp = 1; 
      fieldnum++;
      */
   } 
   if (search.mode == 1) { 
      for ( int b = 0; b < sef_dirnum; b++) { 
         int c = sef_directions[b] + direc; 

         if (c >= sidenum ) 
            c -= sidenum; 

         searchmove( x, y, c, streck, fuelneeded ); 
         search.tiefe--; 
      } 
   } 
   else { 
      searchmove( x, y, direc, streck, fuelneeded ); 
      search.tiefe--; 
   } 
} 



void VehicleMovement :: searchstart( int x1, int y1, int hgt )
{ 
   search.startx = x1; 
   search.starty = y1; 
   search.fieldnum = 0;
   search.height = hgt;
   search.tiefe = 0; 

   cleartemps(7); 
   initwindmovement( vehicle );

   int maindir; 

   #ifdef fastsearch   
   mode = 0; 
   strck = vehicle->movement(); 
   for (maindir = 0; maindir <= 3; maindir++) { 
      move(startx,starty,maindir * 2 + 1,strck);
      tiefe--; 
   } 
   #endif   


   search.mode = 1; 
   search.strck = vehicle->movement; 

   for (maindir = 0; maindir < sidenum; maindir++) { 
      searchmove( search.startx, search.starty, sef_searchorder[maindir], search.strck, 0 );
      search.tiefe--;
   }

   cleartemps(7); 
} 


#ifdef HEXAGON
  static const trichtungen  directions[3][3]  = {{{5, 0, 4, 1, 3, 2 }, {0, 1, 5, 2, 4, 3 }, {1, 0, 2, 5, 3, 4 }},
                                                 {{5, 4, 0, 3, 1, 2 }, {0, 1, 2, 3, 4, 5 }, {1, 2, 0, 3, 5, 4 }}, 
                                                 {{4, 3, 5, 2, 0, 1 }, {3, 4, 2, 5, 1, 0 }, {2, 3, 1, 4, 0, 5 }}}; 
#else
  static const trichtungen  directions[3][3]  = {{{7, 6, 0, 5, 1, 4, 2, 3}, {0, 1, 7, 2, 6, 3, 5, 4}, {1, 0, 2, 7, 3, 6, 4, 5}},
                                                 {{6, 7, 5, 0, 4, 1, 3, 2}, {0, 1, 2, 3, 4, 5, 6, 7}, {2, 1, 3, 0, 4, 7, 5, 6}}, 
                                                 {{5, 6, 4, 7, 3, 0, 2, 1}, {4, 3, 5, 2, 6, 1, 7, 0}, {3, 2, 4, 1, 5, 0, 6, 7}}}; 
#endif


 
void   VehicleMovement :: FieldReachableRek :: move(int          x,
                                                    int          y,
                                                    int          direc,
                                                    int          streck,
                                                    int          fuel )
{ 
    int ox = x; 
    int oy = y; 
    strecke.tiefe++;
    if ( zielerreicht ) 
       return;

    if (mode == 1)
       if (streck > vehicle->movement)
          return;                      

    getnextfield ( x, y, direc);

    if ((x < 0) || (y < 0) || (x >= actmap->xsize) || (y >= actmap->ysize)) 
       return;

    if (actmap->weather.wind[ getwindheightforunit ( vehicle ) ].speed && vehicle->height >= chtieffliegend && vehicle->height <= chhochfliegend) {
       int ll = windbeeline(x,y,x2,y2);
       if (ll > maxwegstrecke - streck) 
          return; 
    } else {
       int ll = beeline(x2,y2,x,y);
       if (ll > maxwegstrecke - streck) 
          return; 
    }

    pfield fld = getfield(x,y); 
    for ( int b = 1; b < strecke.tiefe ; b++)
       if ((strecke.field[b].x == x) && (strecke.field[b].y == y)) 
          return;

    int c = fieldaccessible( fld, vehicle, height); 

    if ( c == 0 ) 
       return;
    else { 
       int mm1;      // fuelusage
       int mm2;      // movementusage

       calcmovemalus(ox, oy, x, y, vehicle, direc, mm1, mm2);

       fuel += mm1;
       streck += mm2;
    } 
    if (streck > maxwegstrecke) 
       return;

    if (fuel * vehicle->typ->fuelconsumption / minmalq  > vehicle->fuel) 
       return;

    if (mode == 1) 
       if ((fld->a.temp > 0) && (streck > fld->a.temp)) return;

    if ((fld->a.temp > streck) || (fld->a.temp == 0)) 
        fld->a.temp = streck;

    strecke.field[strecke.tiefe].x = x; 
    strecke.field[strecke.tiefe].y = y; 
    if ((x == x2) && (y == y2)) { 
       distance = streck; 

       shortestway = strecke; 
       if (actmap->weather.wind[ getwindheightforunit ( vehicle ) ].speed && vehicle->height >= chtieffliegend && vehicle->height <= chhochfliegend) {
          if ((mode == 2) || ((mode == 1) && (streck == windbeeline(x1,y1,x2,y2))))
             zielerreicht = true;
       } else {
          if ((mode == 2) || ((mode == 1) && (streck == beeline(x2,y2,x1,y1))))
             zielerreicht = true;
       }
    } 
    else { 
       int a,b;

       int dx = (2 * x2 + (y2 & 1)) - (2 * x + (y & 1)); 
       int dy = y2 - y; 

       if (dx < 0) 
          a = 0; 
       else 
          if (dx == 0) 
             a = 1; 
          else 
             a = 2; 

       if (dy < 0) 
          b = 0; 
       else 
          if (dy == 0) 
             b = 1; 
          else 
             b = 2; 

       const trichtungen*  direc2 = &directions[b][a]; 
       for (int i = 0; i <= 4; i++) { 
          move( x, y, (*direc2)[i], streck, fuel );
          strecke.tiefe--;
          if ( zielerreicht ) 
             return;
       } 
    } 
 } 


void   VehicleMovement :: FieldReachableRek :: run(int          x22,
                                                   int          y22,
                                                   pvehicle     eht,
                                                   int          md,
                                                   int          _height,
                                                   FieldList*   path )
               
/*  mode :  1 krzesten weg finden
            2 irgendeinen weg finden  */ 
{ 
   cleartemps(7); 

   x2 = x22;
   y2 = y22;
   mode = md;
   vehicle = eht;
   height = _height;

   x1 = vehicle->xpos; 
   y1 = vehicle->ypos; 

   maxwegstrecke = vehicle->movement; 
   zielerreicht = false; 
   distance = 0; 

   int a,b;
   int dx = (2 * x2 + (y2 & 1)) - (2 * x1 + (y1 & 1)); 
   int dy = y2 - y1; 
   if (dx < 0) 
      a = 0;
   else 
      if (dx == 0) 
         a = 1;
      else 
         a = 2;
   if (dy < 0) 
      b = 0;
   else 
      if (dy == 0) 
         b = 1;
      else 
         b = 2;

   const trichtungen*  direc = &directions[b][a];
   for ( int c = 0; c < sidenum; c++ ) { 
      strecke.tiefe = 0; 
      move( x1, y1, (*direc)[c], 0, 0 );
      /*
      if ((mode == 2) && zielerreicht) 
         if (distance <= maxwegstrecke) {
            moveparams.movepath = shortestway; 
            moveparams.movedist = distance; 
            return;
         }  */

      if ((mode == 1) && zielerreicht) 
         break; 
   } 

   path->addField ( eht->xpos, eht->ypos );
   for ( int d = 1; d <= shortestway.tiefe; d++) 
      path->addField ( shortestway.field[d].x, shortestway.field[d].y ); 

   cleartemps(7); 
} 




int  BaseVehicleMovement :: moveunitxy(int xt1, int yt1 )
{ 
   pfield fld = getfield( xt1, yt1 ); 

   int fieldnum;
   for ( fieldnum = 0; fieldnum < path.getFieldNum(); fieldnum++) 
      if ( path.getField( fieldnum ) == fld ) 
         break; 
   
   
   if ( fieldaccessible(fld, vehicle, newheight) < 2) 
      return -106;

   pfield oldfield = getfield( vehicle->xpos, vehicle->ypos ); 


   int x ;
   int y ;
   path.getFieldCoordinates ( 0, &x, &y );

   tsearchreactionfireingunits srfu;
   treactionfirereplay rfr;    

   treactionfire* rf;
   if ( runreplay.status > 0  )
      rf = &rfr;
   else
      rf = &srfu;

   rf->init( vehicle, &path );

   if ( oldfield->vehicle == vehicle) {
      vehicle->removeview();
      oldfield->vehicle = NULL; 
   } else { 
      if ( oldfield->vehicle ) { 
         int i = 0; 
         while (oldfield->vehicle->loading[i] != vehicle) 
            i++; 
         oldfield->vehicle->loading[i] = NULL; 
      } 
      else 
         if ( oldfield->building ) { 
            int i = 0; 
            while (oldfield->building->loading[i] != vehicle) 
               i++; 
            oldfield->building->loading[i] = NULL; 
         } 
   } 

   int i = 0;
   int cancelmovement = 0;

   int movedist = 0;
   int fueldist = 0;

   while ( (x != xt1 || y != yt1) && vehicle && cancelmovement!=1 ) { 

      if ( cancelmovement > 1 )
         cancelmovement--;

      int x2, y2;
      path.getFieldCoordinates ( i+1, &x2, &y2 );

      int mm1,mm2;
      calcmovemalus( x, y, x2, y2, vehicle, 0 , mm1, mm2);
      movedist += mm2;
      fueldist += mm1;

      pfield fld1 = getfield ( x,  y  );
      pfield fld2 = getfield ( x2, y2 );


      if ( mapDisplay )
         mapDisplay->displayMovingUnit ( x, y, x2, y2, vehicle, vehicle->height, newheight, i, fieldnum );

      int dir;
      if (vehicle->functions & ( cffahrspur | cficebreaker ))  {
        dir = getdirection( x, y, x2, y2 );
      
        if ( vehicle->functions & cffahrspur )
           if ( fahrspurobject )
              if ( fld1->bdt & cbfahrspur )
                 if ( ! (actmap->objectcrc   &&   !actmap->objectcrc->speedcrccheck->checkobj2 ( fahrspurobject, 2 ))) 
                    fld1 -> addobject ( fahrspurobject, 1 << dir );

        if ( vehicle->functions & cficebreaker )
           if ( eisbrecherobject )
              if ( ! (actmap->objectcrc   &&   !actmap->objectcrc->speedcrccheck->checkobj2 ( eisbrecherobject, 2 ))) 
                 if ( (fld1->bdt & cbsnow1 )  || ( fld1->bdt & cbsnow2 ) || fld1->checkforobject ( eisbrecherobject ) ) {
                    fld1 -> addobject ( eisbrecherobject, 1 << dir );
                    fld1->checkforobject ( eisbrecherobject )->time = actmap->time.a.turn;
                 }

      } 

      i++;
      path.getFieldCoordinates ( i, &x, &y );

      pfield field3 = fld2; 
      if (vehicle->functions & cffahrspur) {
         if ( fahrspurobject )
            if ( field3->bdt & cbfahrspur )
               if ( ! (actmap->objectcrc   &&   !actmap->objectcrc->speedcrccheck->checkobj2 ( fahrspurobject, 2 ))) 
                  if (dir >= sidenum/2) 
                    getfield ( x, y ) -> addobject ( fahrspurobject, 1 << (dir - sidenum/2));
                  else 
                    getfield ( x, y ) -> addobject ( fahrspurobject, 1 << (dir + sidenum/2));
      } 
      if ( vehicle->functions & cficebreaker )
         if ( eisbrecherobject )
            if ( ! (actmap->objectcrc   &&   !actmap->objectcrc->speedcrccheck->checkobj2 ( eisbrecherobject, 2 ))) 
                 if ( (field3->bdt & cbsnow1 )  || ( field3->bdt & cbsnow2 ) || field3->checkforobject ( eisbrecherobject ) ) {
                  if (dir >= sidenum/2) 
                    getfield ( x, y ) -> addobject ( eisbrecherobject, 1 << (dir - sidenum/2));
                  else 
                    getfield ( x, y ) -> addobject ( eisbrecherobject, 1 << (dir + sidenum/2));
      
                    field3->checkforobject ( eisbrecherobject )->time = actmap->time.a.turn;
      
               }

      if ( vehicle ) {
         vehicle->xpos = x;
         vehicle->ypos = y;
         vehicle->addview();
      }

      int fieldschanged = evaluateviewcalculation ( 1 << actmap->playerview );

      if ( rf->checkfield ( x, y, vehicle, mapDisplay ))
         cancelmovement = 1;

      if ( mapDisplay ) {
         if ( !vehicle )
            mapDisplay->deleteVehicle( vehicle );
   
         if ( fieldschanged > 0 )
            mapDisplay->displayMap(); 
     }

      /*
      if ( vehicle ) 
         if ( vehicleplattfahrbar(vehicle,field3) ) 
            bulldoze_trooper.fight( &vehicle, &field3->vehicle);
      */

      if ( vehicle ) {
         vehicle->removeview();
         if ( field3->mineattacks ( vehicle )) {
            tmineattacksunit battle;
            battle.setup ( field3, -1, vehicle );

            if ( mapDisplay && fieldvisiblenow ( getfield ( x, y ), actmap->playerview) || field3->mineowner() == actmap->playerview )
               battle.calcdisplay ();
            else
               battle.calc();

            battle.setresult ();
            dashboard.x = 0xffff;
            cancelmovement = 1;
        }
      }
      if ( cancelmovement == 1 )
         if ( field3->vehicle || field3->building )
            cancelmovement++;

      if ( newheight != -1 && newheight != vehicle->height && cancelmovement ) 
         cancelmovement = 0;


      if ( vehicle ) {
         npush ( field3->vehicle );
         field3->vehicle = vehicle;
         if ( field3->connection & cconnection_areaentered_anyunit )
            releaseevent ( vehicle, NULL, cconnection_areaentered_anyunit );

         if ((field3->connection & cconnection_areaentered_specificunit ) && ( vehicle->connection & cconnection_areaentered_specificunit ))
            releaseevent ( vehicle, NULL, cconnection_areaentered_specificunit );
         npop ( field3->vehicle );
      }
   }                           

   fld = getfield ( x, y );

   if ( vehicle ) { 
      if ((fld->vehicle == NULL) && (fld->building == NULL)) {
         fld->vehicle = vehicle; 
         vehicle->addview();
      } else { 
         if ( fld->vehicle  &&  fld->vehicle->typ->loadcapacity ) { 
            i = 0; 
            while ((fld->vehicle->loading[i] != NULL) && (i < 31)) 
              i++; 
            fld->vehicle->loading[i] = vehicle; 
         }                                                           
         else 
            if ( fld->building ) { 
               i = 0; 
               while ( fld->building->loading[i]  && (i < 31)) 
                  i++; 
               fld->building->loading[i] = vehicle; 
               if (fld->building->color != (actmap->actplayer << 3)) 
                  fld->building->convert( actmap->actplayer ); 

            } 
      } 


      vehicle->fuel -= fueldist * vehicle->typ->fuelconsumption / 8;
      if ( vehicle->fuel < 0 )
         vehicle->fuel = 0;


      if (fld->vehicle == vehicle) { 
         i = vehicle->movement - movedist;
         if (i > 0) 
           vehicle->movement = i;
         else 
           vehicle->movement = 0;

         if ( (vehicle->movement >> 3) > (vehicle->fuel / vehicle->typ->fuelconsumption) )
            vehicle->movement = (vehicle->fuel << 3) / vehicle->typ->fuelconsumption;
      } 
      else { 
         vehicle->movement = 0;
         vehicle->attacked = true; 
         if (vehicle->height == chtieffliegend) 
            vehicle->height = chfahrend; 
      } 

      vehicle->setnewposition ( x, y );

      
      dashboard.x = 0xffff; 
   } 

   int fieldschanged = computeview( 1 << actmap->playerview ); 

   if ( mapDisplay ) {
      mapDisplay->resetMovement();
      if ( fieldschanged > 0 )
         mapDisplay->displayMap(); 
      else
         mapDisplay->displayPosition ( x, y );
   }
   return 0;
} 











ChangeVehicleHeight :: ChangeVehicleHeight ( MapDisplayInterface* md, PPendingVehicleActions _pva, VehicleActionType vat )
                 : BaseVehicleMovement ( vat, _pva )
{
   status = 0;
   mapDisplay = md;
}




int ChangeVehicleHeight :: execute_withmove ( void )
{
   if ( !vehicle ) 
      return -unspecified_error;

   pfield fld = getfield( vehicle->xpos, vehicle->ypos ); 

   int steigung = vehicle->typ->steigung; 

   if ( vehicle->typ->height & (chtieffliegend | chfliegend | chhochfliegend) ) {
      int w;
      if ( newheight > vehicle->height ) { 
         w = (vehicle->height << 1);
         if ( w != chtieffliegend) { 
            if (vehicle->movement < steigung * maxmalq + air_heightincmovedecrease) 
               return -110;

         } 
         else { 
            if (vehicle->movement < steigung * maxmalq ) 
               return -110;
         } 

      } 
      else { 
         w = (vehicle->height >> 1);
         if (vehicle->movement < steigung * maxmalq) 
            return -111;

      } 
      if ((w & vehicle->typ->height) == 0)
         return -112;

      newheight = w;

      return moveheight( ); 
   } else {
      displaymessage("changing height with movement of non-airplanes has not been defined yet !",2 );
      return -1;
   }

}


int ChangeVehicleHeight :: moveheight( void )
{ 

   initwindmovement( vehicle );

   int ok2 = false; 

   int xx = vehicle->xpos;
   int yy = vehicle->ypos;
   for ( int direc = 0; direc < sidenum; direc++) { 
      int ok = true; 
      int x = xx; 
      int y = yy; 

      int dist = 0;
      int mx = vehicle->movement;
      while ( dist < vehicle->typ->steigung * minmalq  && mx > 0 ) {
         getnextfield( x, y, direc );

         dist += minmalq - windmovement[direc];
         mx -= minmalq - windmovement[direc];

         if ((x < 0) || (y < 0) || (x >= actmap->xsize) || (y >= actmap->ysize)) 
            ok = false; 
         else { 
            pfield fld = getfield(x,y); 
            if ( fieldaccessible(fld, vehicle, vehicle->height ) < 1) 
               ok = false; 
            if ( fieldaccessible(fld, vehicle, newheight ) < 1) 
               ok = false; 
         } 
      } 
      if ( mx < 0 )
         ok = false;


      if ( fieldaccessible( getfield(x,y), vehicle, newheight ) < 2) 
         ok = false;
/*
      if ( ok )
         if ( fld->building || fld->vehicle )
            ok = false;
*/

      if (ok) { 
         reachableFields.addField ( x, y );
         ok2 = true; 
      } 

   } 
   if ( ok2 ) 
      return 0;
   else
      return -116;
} 


int ChangeVehicleHeight :: moveunitxy ( int xt1, int yt1 )
{
   int oldheight = vehicle->height;
   int res = BaseVehicleMovement :: moveunitxy ( xt1, yt1 );
   if ( res < 0 )
      return res;

   if ( oldheight < newheight ) {
      if ( oldheight == chfahrend ) {
         vehicle->fuel -= vehicle->typ->fuelconsumption * vehicle->typ->steigung;
         if ( vehicle->fuel < 0 )
            vehicle->fuel = 0;
         vehicle->movement = airplanemoveafterstart;
         vehicle->attacked = 1;
         vehicle->height = chtieffliegend;
      } else {
         vehicle->movement -= air_heightincmovedecrease;
         if ( vehicle->movement < 0 )
            vehicle->movement = 0;

         vehicle->fuel -= vehicle->typ->fuelconsumption * air_heightincmovedecrease / 8;
         if ( vehicle->fuel < 0 )
            vehicle->fuel = 0;

         int newmovement = vehicle->typ->movement[log2(newheight)] * vehicle->movement / vehicle->typ->movement[log2(vehicle->height)];
         vehicle->movement = newmovement;
         if ( vehicle->movement < 0 )
            vehicle->movement = 0;

         vehicle->height = newheight;
      }
   } else {
      if ( vehicle->height == chtieffliegend ) {
         // vehicle->fuel -= vehicle->typ->fuelconsumption * vehicle->typ->steigung;
         // if ( vehicle->fuel < 0 )
         //    vehicle->fuel = 0;
         vehicle->movement = airplanemoveafterlanding;
         vehicle->attacked = 1;
         vehicle->height = chfahrend;
      } else {
         vehicle->movement -= air_heightdecmovedecrease;
         if ( vehicle->movement < 0 )
            vehicle->movement = 0;

         vehicle->fuel -= vehicle->typ->fuelconsumption * air_heightdecmovedecrease / 8;
         if ( vehicle->fuel < 0 )
            vehicle->fuel = 0;

         int newmovement = vehicle->typ->movement[log2(newheight)] * vehicle->movement / vehicle->typ->movement[log2(vehicle->height)];
         vehicle->movement = newmovement;
         if ( vehicle->movement < 0 )
            vehicle->movement = 0;

         vehicle->height = newheight;
      }
   }
   return 0;
}


int ChangeVehicleHeight :: verticalHeightChange ( void )
{
   if ( !vehicle ) 
      return -unspecified_error;

   pfield fld = getfield(vehicle->xpos,vehicle->ypos); 

   int steigung = vehicle->typ->steigung; 
   int oldheight = vehicle->height;

   if (vehicle->typ->height & (chtieffliegend | chfliegend | chhochfliegend)) {
      if ( (newheight < oldheight) && (newheight == chfahrend) ) { 
         if ( !terrainaccessible ( fld, vehicle, newheight ))
            return -109;

         int newmovement = vehicle->typ->movement[log2(newheight)] * (vehicle->movement - (helicopter_landing_move_cost + air_heightdecmovedecrease)) / vehicle->typ->movement[log2(vehicle->height)];
         if ( newmovement < 0)                      
            return -111;

         int fuelcost = vehicle->typ->fuelconsumption * (helicopter_landing_move_cost + air_heightdecmovedecrease) / 8;
         if ( fuelcost > vehicle->fuel )
            return -115;


         vehicle->movement = newmovement ; 
         if ( !helicopter_attack_after_descent )
            vehicle->attacked = 0; 
         vehicle->fuel -= fuelcost;

      } else
      if ( newheight < oldheight ) { 
         int newmovement = vehicle->typ->movement[log2(newheight)] * vehicle->movement / vehicle->typ->movement[log2(vehicle->height)];
         if ( newmovement < air_heightdecmovedecrease )
            return -111;

         int fuelcost = vehicle->typ->fuelconsumption * air_heightdecmovedecrease / 8;
         if ( fuelcost > vehicle->fuel )
            return -115;


         vehicle->movement = newmovement - air_heightdecmovedecrease; 
         vehicle->fuel -= fuelcost;
      } else
      if (( newheight > oldheight ) && (newheight > chtieffliegend)) { 
         int newmovement = vehicle->typ->movement[log2(newheight)] * vehicle->movement / vehicle->typ->movement[log2(vehicle->height)];
         if (newmovement < air_heightincmovedecrease) 
            return -110;
         int fuelcost = vehicle->typ->fuelconsumption * air_heightincmovedecrease / 8;
         if ( fuelcost > vehicle->fuel )
            return -115;

         vehicle->movement = newmovement - air_heightincmovedecrease ; 
         vehicle->fuel -= fuelcost;

         if ((newheight == chtieffliegend) && helicopter_attack_after_ascent ) 
            vehicle->attacked = 1; 
      } else
         if ( newheight > oldheight  &&  newheight == chtieffliegend ) {
            int newmovement = vehicle->typ->movement[log2(newheight)] * vehicle->movement / vehicle->typ->movement[log2(vehicle->height)];
            if ( newmovement < air_heightincmovedecrease )
               return -110;

            if ( helicopter_attack_after_ascent ) 
               vehicle->attacked = 1; 

            vehicle->movement = newmovement - air_heightincmovedecrease;
         }
         
      logtoreplayinfo ( rpl_changeheight, (int) vehicle->xpos, (int) vehicle->ypos, 
                                          (int) vehicle->xpos, (int) vehicle->ypos, vehicle->networkid, (int) vehicle->height, (int) newheight );
      vehicle->height = newheight; 
   } 
   else {   /*  not an aircraft */ 

      int md;

      if ( newheight > oldheight ) { 
         md = sub_heightincmovedecrease; 
         if ( vehicle->movement < md )
            return -110;
      } 
      else { 
         md = sub_heightdecmovedecrease; 
         if ( vehicle->movement < md )
            return -111;
      } 

      int newmovement = vehicle->typ->movement[log2(newheight)] * (vehicle->movement - md ) / vehicle->typ->movement[log2(vehicle->height)];

      int res = terrainaccessible2 ( fld, vehicle, newheight );
      if ( res <= 0 ) {
         if ( newheight > vehicle->height ) 
            return -108;
         else {
            if ( res == -1 )
               return -113;
            else
               if ( res == -2 )
                  return -114;
               else
                  return -109;
         }
      }

      int fuelcost = vehicle->typ->fuelconsumption * md / 8;
      if ( fuelcost > vehicle->fuel )
         return -115;

      vehicle->movement = newmovement;
      vehicle->fuel -= fuelcost; 

      logtoreplayinfo ( rpl_changeheight, (int) vehicle->xpos, (int) vehicle->ypos, 
                                          (int) vehicle->xpos, (int) vehicle->ypos, vehicle->networkid, (int) vehicle->height, (int) newheight );

      vehicle->height = newheight; 
     
   } 
   return 0;
}


int ChangeVehicleHeight :: execute ( pvehicle veh, int x, int y, int step, int height, int param2 )
{
   if ( step != status )
      return -1;

   if ( status == 0 ) {
      vehicle = veh ;
      if ( !vehicle ) {
         status = -101;
         return status;
      }
      newheight = height;

      if ( vehicle->typ->steigung ) {
         int res = execute_withmove ( );
         if ( res < 0 ) {
            status = res;
            return status;
         }
         if ( reachableFields.getFieldNum() <= 0 ) {
            status =  -107;
            return status;
         }
   
         status = 2;
      } else {
         status = verticalHeightChange ();
         if ( status >= 0 ) {
            status = 1000;
            if ( mapDisplay )
               mapDisplay->displayPosition( vehicle->xpos, vehicle->ypos );
         }
      }
      return status;
  } else
   if ( status == 2 ) {
      if ( height == -1)
         height = newheight;
      else
         if ( height != newheight )
            displaymessage ( "ChangeVehicleHeight :: execute / unit heights not matching !", 2 );

      if ( !reachableFields.isMember ( x, y )) {
         status = -105;
         return status;
      }

      fieldReachableRek.run( x, y, vehicle, 1, height, &path );

      status = 3;
      return status;
   } else
    if ( status == 3 ) {
       if ( !path.isMember ( x, y )) {
          status = -105;
          return status;
       }

       int x1 = vehicle->xpos;
       int y1 = vehicle->ypos;
 
       logtoreplayinfo ( rpl_changeheight, x1, y1, x, y, vehicle->networkid, (int) vehicle->height, (int) newheight );
 
       if ( mapDisplay )
          mapDisplay->startAction();
       int stat = moveunitxy( x, y );
       if ( mapDisplay ) {
          // mapDisplay->displayMap();
          mapDisplay->stopAction();
       }

       if ( stat < 0 ) 
          status = stat;
       else
          status = 1000;

       return status;
      
    } else
       status = 0;
  return status;
}


IncreaseVehicleHeight :: IncreaseVehicleHeight ( MapDisplayInterface* md, PPendingVehicleActions _pva )
                 : ChangeVehicleHeight ( md, _pva, vat_ascent )
{
   if ( pva )
      pva->ascent = this;
}

void IncreaseVehicleHeight :: registerPVA ( VehicleActionType _actionType, PPendingVehicleActions _pva )
{
   VehicleAction::registerPVA ( _actionType, _pva );
   if ( pva )
      pva->ascent = this;
}


int IncreaseVehicleHeight :: available ( pvehicle veh ) const
{
   if ( veh ) 
      if ( veh->movement )
         if (veh->height < 128) 
            if ((veh->height << 1) & veh->typ->height ) 
               if ( veh->typ->steigung )
                  return 1;
               else
                  return 2;
   return 0;
}


IncreaseVehicleHeight :: ~IncreaseVehicleHeight ( )
{
   if ( pva )
      pva->ascent = NULL;
}




DecreaseVehicleHeight :: DecreaseVehicleHeight ( MapDisplayInterface* md, PPendingVehicleActions _pva )
                 : ChangeVehicleHeight ( md, _pva, vat_descent )
{
   if ( pva )
      pva->descent = this;
}

void DecreaseVehicleHeight :: registerPVA ( VehicleActionType _actionType, PPendingVehicleActions _pva )
{
   VehicleAction::registerPVA ( _actionType, _pva );
   if ( pva )
      pva->descent = this;
}


int DecreaseVehicleHeight :: available ( pvehicle veh ) const
{
   if ( veh ) 
      if ( veh->movement )
         if (veh->height > 1) 
            if ((veh->height >> 1) & veh->typ->height ) 
               if ( veh->typ->steigung )
                  return 1;
               else
                  return 2;
   return 0;
}

DecreaseVehicleHeight :: ~DecreaseVehicleHeight ( )
{
   if ( pva )
      pva->descent = NULL;
}



PendingVehicleActions :: PendingVehicleActions ( void )
{
   move = NULL;
   action = NULL;
}

PendingVehicleActions :: ~PendingVehicleActions ( )
{
   if ( action ) 
      delete action;

}


