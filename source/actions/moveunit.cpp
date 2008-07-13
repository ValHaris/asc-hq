/*
     This file is part of Advanced Strategic Command; http://www.asc-hq.de
     Copyright (C) 1994-2008  Martin Bickel  and  Marc Schellenberger
 
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

#include <cmath>

#include "moveunit.h"
#include "unitfieldregistration.h"
#include "changeunitproperty.h"
#include "consumeresource.h"

#include "../vehicle.h"
#include "../gamemap.h"
#include "../reactionfire.h"
#include "../soundList.h"
#include "../controls.h"
#include "../gameoptions.h"
#include "../mapdisplayinterface.h"
#include "../viewcalculation.h"
#include "../spfst.h"
#include "../gameeventsystem.h"
     
     
void printTimer( int i )
{
#if 0
   static int lastTimer = 0;
   if ( i == 1 )
      lastTimer = SDL_GetTicks();
   else {
      printf("%d - %d : %d \n", i-1, i, SDL_GetTicks() - lastTimer);
      lastTimer = SDL_GetTicks();
   }
#endif
}
     
     
MoveUnit::MoveUnit( Vehicle* veh, AStar3D::Path& pathToMove, bool dontInterrupt )
   : UnitAction( veh->getMap(), veh->networkid )
{
   this->pathToMove = pathToMove;
   this->dontInterrupt = dontInterrupt;
}
      
      
ASCString MoveUnit::getDescription() const
{
   ASCString res = "Move unit ";
   if ( getUnit(false) ) 
      res += getUnit(false)->getName();
   
   return  res;
}
      
      
void MoveUnit::readData ( tnstream& stream ) 
{
   UnitAction::readData( stream );
   int version = stream.readInt();
   if ( version != 1 )
      throw tinvalidversion ( "ChangeUnitMovement", 1, version );
   
   dontInterrupt = stream.readInt();
   readClassContainerStaticConstructor( pathToMove, stream );   
   
};
      
      
void MoveUnit::writeData ( tnstream& stream )
{
   UnitAction::readData( stream );
   stream.writeInt( 1 );
   stream.writeInt( dontInterrupt );
   writeClassContainer( pathToMove, stream );
};


GameActionID MoveUnit::getID()
{
   return ActionRegistry::MoveUnit;
}


ActionResult MoveUnit::runAction( const Context& context )
{
   Vehicle* vehicle = getUnit();
   
   
   WindMovement* wind;

   if ( (vehicle->typ->height & ( chtieffliegend | chfliegend | chhochfliegend )) && getMap()->weather.windSpeed ) {
      wind = new WindMovement ( vehicle );
   } else
      wind = NULL;

   tfield* oldfield = getMap()->getField( vehicle->getPosition() );

   AStar3D::Path::iterator pos = pathToMove.begin();
   AStar3D::Path::iterator stop = pathToMove.end()-1;

   tsearchreactionfireingunits srfu;
   treactionfire* rf = &srfu;

   int orgMovement = vehicle->getMovement( false );
   int orgHeight = vehicle->height;

   rf->init( vehicle, pathToMove );

   if ( oldfield->vehicle == vehicle) 
      (new UnitFieldRegistration( vehicle, vehicle->getPosition(), UnitFieldRegistration::RemoveView, false ))->execute( context );

   
   (new UnitFieldRegistration( vehicle, vehicle->getPosition(), UnitFieldRegistration::UnregisterOnField, false ))->execute( context );
   
   int soundHeight = -1;
   if ( pos->getRealHeight() >= 0 )
      soundHeight = pos->getRealHeight();
   else
      soundHeight = stop->getRealHeight();

   SoundLoopManager slm ( SoundList::getInstance().getSound( SoundList::moving, vehicle->typ->movemalustyp, vehicle->typ->movementSoundLabel, soundHeight ), false );

   int cancelmovement = 0;

   int movedist = 0;
   int fueldist = 0;
   int networkID = vehicle->networkid;
   int operatingPlayer = context.actingPlayer->getPosition();

   bool viewInputChanged= false;
   bool mapDisplayUpToDate = true;
   bool finalRedrawNecessary = false;

   bool inhibitAttack = false;
   while ( pos != stop  && vehicle && cancelmovement!=1 ) {

      if ( cancelmovement > 1 )
         cancelmovement--;

      AStar3D::Path::iterator next = pos+1;


      bool container2container = pos->getNumericalHeight()==-1 && next->getNumericalHeight() == -1;
      pair<int,int> mm = calcMoveMalus( *pos, next->getRealPos(), vehicle, wind, &inhibitAttack, container2container );
      movedist += mm.first;
      fueldist += mm.second;

      if ( next->hasAttacked )
         vehicle->setAttacked( context );


      if ( next->getRealHeight() != pos->getRealHeight() && next->getRealHeight() >= 0 )
         (new ChangeUnitProperty( vehicle, ChangeUnitProperty::Height, 1 << next->getRealHeight() ))->execute( context );

      int pathStepNum = beeline ( *pos, *next ) / maxmalq;
      int pathStep = 0;
      if ( !pathStepNum )
         pathStepNum = 1;

      MapCoordinate3D to = *pos;
      do {
         MapCoordinate3D from;
         from.setnum ( to.x, to.y, pos->getRealHeight() );
         if ( next->x != from.x || next->y != from.y )
            to = getNeighbouringFieldCoordinate ( to, getdirection ( to, *next ));
         to.setnum ( to.x, to.y, next->getRealHeight() );

         tfield* dest = getMap()->getField ( to );


         if ( vehicle ) {
            (new UnitFieldRegistration( vehicle, to, UnitFieldRegistration::Position ))->execute( context );
            (new UnitFieldRegistration( vehicle, to, UnitFieldRegistration::AddView ))->execute( context );
         }

         
         printTimer(1);
         
         {
            int dir = getdirection( from, to );
            if ( dir >= 0 && dir <= 5 ) 
               (new ChangeUnitProperty( vehicle, ChangeUnitProperty::Direction, dir ))->execute( context );
         }
         
         
         if ( context.display ) {
            // displaymap();
            if ( next == stop && to.x==next->x && to.y==next->y) // the unit will reach its destination
               slm.fadeOut ( CGameOptions::Instance()->movespeed * 10 );
            context.display->displayMovingUnit ( from, to, vehicle, pathStep, pathStepNum, MapDisplayInterface::SoundStartCallback( &slm, &SoundLoopManager::activate ));
            finalRedrawNecessary = true;
            mapDisplayUpToDate = false;
         }
         pathStep++;

         printTimer(4);

         if ( vehicle ) {
            if ( vehicle->spawnMoveObjects( from, to, context ) )
               mapDisplayUpToDate = false;
            
            
            if ( inhibitAttack )
               vehicle->setAttacked( context );
         }

         
         printTimer(5);
         {
            Vehicle* temp = dest->vehicle;
            
            if ( dest->vehicle )
               (new UnitFieldRegistration( dest->vehicle, to, UnitFieldRegistration::UnregisterOnField ))->execute( context );
            
            (new UnitFieldRegistration( vehicle, to, UnitFieldRegistration::RegisterOnField, true ))->execute( context );
            
            
            
            int fieldsWidthChangedVisibility; 
            if ( context.viewingPlayer >= 0 ) 
               fieldsWidthChangedVisibility = evaluateviewcalculation ( getMap(), 1 << context.viewingPlayer );
            else 
               fieldsWidthChangedVisibility = evaluateviewcalculation ( getMap(), 0);
            
            
            if ( fieldsWidthChangedVisibility )
               mapDisplayUpToDate = false;
            
            // mapDisplayUpToDate = false;
            
            
         (new UnitFieldRegistration( vehicle, to, UnitFieldRegistration::UnregisterOnField ))->execute( context );
         if ( temp )
               (new UnitFieldRegistration( temp, to, UnitFieldRegistration::RegisterOnField ))->execute( context );
            
            printTimer(6);
         }

         viewInputChanged = false;

         if ( vehicle ) {

            if ( context.display && fieldvisiblenow ( dest, vehicle, context.viewingPlayer ) ) {
               // here comes an ugly hack to get the shadow of starting / descending aircraft right

               int oldheight = vehicle->height;
               if ( next->getRealHeight() > pos->getRealHeight() && pathStep < pathStepNum )
                  vehicle->height = 1 << pos->getRealHeight();
               
               if ( !mapDisplayUpToDate ) {
                  context.display->displayMap( vehicle );
                  mapDisplayUpToDate = true;
                  finalRedrawNecessary = false;
               }

               vehicle->height = oldheight;
            }

            if ( rf->checkfield ( to, vehicle, context )) {
               cancelmovement = 1;
               vehicle = getMap()->getUnit ( networkID );
            }


            if ( !vehicle && context.display ) {
               context.display->displayMap();
               mapDisplayUpToDate = true;
               finalRedrawNecessary = false;
               
               viewInputChanged = true;
            }
         } else
            if ( context.display ) {
               context.display->displayMap();
               mapDisplayUpToDate = true;
               finalRedrawNecessary = false;
             }

         printTimer(7);
            
            
         if ( vehicle ) {
            if ( !(stop->x == to.x && stop->y == to.y && next == stop ))
               (new UnitFieldRegistration( vehicle, to, UnitFieldRegistration::RemoveView ))->execute( context );
            
            if ( dest->mineattacks ( vehicle )) {
               tmineattacksunit battle ( dest, -1, vehicle );

               if ( context.display && (fieldvisiblenow ( dest, context.viewingPlayer) || dest->mineowner() == context.viewingPlayer ))
                  context.display->showBattle( battle );
               else
                  battle.calc();

               battle.setresult ( context );
               if ( battle.dv.damage >= 100 ) {
                  vehicle = NULL;
                  viewInputChanged = true;
               }
               
               updateFieldInfo();
               cancelmovement = 1;
           }
         }
         if ( cancelmovement == 1 )
            if ( dest->vehicle || dest->building )
               cancelmovement++;

         if ( vehicle )
            if ( dontInterrupt )
               cancelmovement = 0;

         if ( vehicle ) {
            Vehicle* temp = dest->vehicle;
            if ( dest->vehicle )
               (new UnitFieldRegistration( dest->vehicle, to, UnitFieldRegistration::UnregisterOnField ))->execute( context );
            
            (new UnitFieldRegistration( vehicle, to, UnitFieldRegistration::RegisterOnField ))->execute( context );
         
            if ( dest->connection & cconnection_areaentered_anyunit )
               fieldCrossed( context );

            if ((dest->connection & cconnection_areaentered_specificunit ) && ( vehicle->connection & cconnection_areaentered_specificunit ))
               fieldCrossed( context );
               
            (new UnitFieldRegistration( vehicle, to, UnitFieldRegistration::UnregisterOnField ))->execute( context );
            if ( temp )
               (new UnitFieldRegistration( temp, to, UnitFieldRegistration::RegisterOnField ))->execute( context );
                  
         }
         printTimer(8);
      } while ( (to.x != next->x || to.y != next->y) && vehicle );

      pos = next;
   }

   tfield* fld = getfield ( pos->x, pos->y );

   if ( vehicle ) {

      int newMovement = orgMovement - pos->dist;

      (new UnitFieldRegistration( vehicle, *pos, UnitFieldRegistration::Position ))->execute( context );

      if ( vehicle->typ->movement[log2(orgHeight)] ) {
         int nm = int(floor(vehicle->maxMovement() * float(newMovement) / float(vehicle->typ->movement[log2(orgHeight)]) + 0.5));
//         if ( nm < 0 )
//            result = -1;
         
         
         (new ChangeUnitProperty( vehicle, ChangeUnitProperty::Movement, nm ))->execute( context );
         
         // the unit will be shaded if movement is exhausted
         if ( vehicle->getMovement() < 10 )
            finalRedrawNecessary = true;
      }


      (new ConsumeResource( vehicle, Resources(0,0,fueldist * vehicle->typ->fuelConsumption / maxmalq )))->execute( context );

      if ( fld->vehicle || fld->building ) {
         (new ChangeUnitProperty( vehicle, ChangeUnitProperty::Movement, 0 ))->execute( context );
         (new ChangeUnitProperty( vehicle, ChangeUnitProperty::AttackedFlag, 1 ))->execute( context );
      }

      if ( vehicle ) {
         if ((fld->vehicle == NULL) && (fld->building == NULL)) {
            if ( !vehicle->isViewing() ) {
               (new UnitFieldRegistration( vehicle, *pos, UnitFieldRegistration::AddView ))->execute( context );
               viewInputChanged = true;

               // do we really need this check?
               if ( rf->checkfield ( *pos, vehicle, context )) {
                  vehicle = getMap()->getUnit ( networkID );
               }
               
               (new UnitFieldRegistration( vehicle, *pos, UnitFieldRegistration::RegisterOnField ))->execute( context );

            } else {
               (new UnitFieldRegistration( vehicle, *pos, UnitFieldRegistration::RegisterOnField ))->execute( context );
               for ( int i = 0; i < getMap()->getPlayerCount(); ++i )
                  evaluatevisibilityfield ( getMap(), fld, i, -1, getMap()->getgameparameter ( cgp_initialMapVisibility ) );
            }

         } else {
            ContainerBase* cn = fld->getContainer();
            if ( vehicle->isViewing() ) {
               vehicle->removeview();
               viewInputChanged = true;
            }
            
            (new UnitFieldRegistration( vehicle, *pos, UnitFieldRegistration::RegisterOnField ))->execute( context );
            
            if (cn->getOwner() != vehicle->getOwner() && fld->building && getMap()->getPlayer(fld->building).diplomacy.isHostile( vehicle) ) {
               fld->building->convert( vehicle->color / 8 );
               if ( fieldvisiblenow ( fld, context.viewingPlayer ) || context.viewingPlayer  == vehicle->getOwner() )
                  SoundList::getInstance().playSound ( SoundList::conquer_building, 0 );
           }
           mapDisplayUpToDate = false;

         }

      }
   }

   if ( rf->finalCheck( operatingPlayer, context ))
      finalRedrawNecessary = true;
   
   // finalRedrawNecessary = true;
   
   if ( viewInputChanged ) {
      int fieldschanged;
      if ( context.viewingPlayer >= 0 )
         fieldschanged = evaluateviewcalculation ( getMap(), 1 << context.viewingPlayer );
      else
         fieldschanged = evaluateviewcalculation ( getMap(), 0 );
      
      if ( fieldschanged )
         mapDisplayUpToDate = false;
         
   }

   if ( context.display ) {
      context.display->resetMovement();
      // if ( fieldschanged > 0 )
      if (finalRedrawNecessary || !mapDisplayUpToDate)
         context.display->displayMap();
      // else
      //   mapDisplay->displayPosition ( pos->x, pos->y );
   }
   
   return ActionResult(0);
}


ActionResult MoveUnit::undoAction( const Context& context )
{
   return ActionResult(0);
}

ActionResult MoveUnit::preCheck()
{
   return ActionResult(0);
}

ActionResult MoveUnit::postCheck()
{
   return ActionResult(0);
}


