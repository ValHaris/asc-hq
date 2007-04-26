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

#include <vector>
#include <algorithm> 
#include <cmath>
#include <SDL.h>
#include <SDL_thread.h>

#include "unitctrl.h"
#include "controls.h"
#include "dialog.h"
#include "attack.h"
#include "stack.h"
#include "vehicletype.h"
#include "buildingtype.h"
#include "viewcalculation.h"
#include "replay.h"
#include "gameoptions.h"
#include "itemrepository.h"
#include "astar2.h"
#include "containercontrols.h"
#include "mapdisplayinterface.h"
#include "gameeventsystem.h"
#include "actions/servicing.h"
#include "soundList.h"
#include "reactionfire.h"

PendingVehicleActions pendingVehicleActions;

SigC::Signal0<void> fieldCrossed;




void BaseVehicleMovement :: PathFinder :: getMovementFields ( IntFieldList& reachableFields, IntFieldList& reachableFieldsIndirect, int height )
{
   Path dummy;
   findPath ( dummy, MapCoordinate3D(-1, -1, veh->height) );  //this field does not exist...

   int unitHeight = veh->getPosition().getNumericalHeight();
   if ( !actmap->getField ( veh->getPosition())->unitHere ( veh ))
      unitHeight = -1;

   // there are different entries for the same x/y coordinate but different height.
   // Since the UI is only in xy, we need to find the height which is the easiest to reach
   typedef multimap<MapCoordinate,Container::iterator > Fields;
   Fields fields;
   int orgHeight=-1;
   int minMovement = maxint;
   for ( Container::iterator i = visited.begin(); i != visited.end(); ++i ) {
      if ( i->h.x != veh->getPosition().x || i->h.y != veh->getPosition().y || i->h.getNumericalHeight() != unitHeight ) {
         int h = i->h.getNumericalHeight();
         // if ( h == -1 )
         //   h = i->enterHeight;
         if ( h == -1 || height == -1 || h == height ) {
            if ( i->canStop )
               fields.insert(make_pair(MapCoordinate(i->h),  i));
            else
               reachableFieldsIndirect.addField ( i->h, i->h.getNumericalHeight() );
         }
      }
      if ( i->h.getNumericalHeight() >= 0 )
         if ( i->gval < minMovement ) {
            orgHeight = i->h.getNumericalHeight();
            minMovement = int (i->gval);
         }
   }
   for ( Fields::iterator i = fields.begin(); i != fields.end();  ) {
      int height = i->second->h.getNumericalHeight();
      int move = int(i->second->gval);
      Fields::key_type key = i->first;
      ++i;
      while ( i != fields.end() && i->first == key ) {
         if ( i->second->gval  < move || ( i->second->gval == move && abs(i->second->h.getNumericalHeight()-orgHeight) < abs(height-orgHeight) ))
            height = i->second->h.getNumericalHeight();
         ++i;
      }
      reachableFields.addField ( key, height );
   }
}

bool multiThreadedViewCalculation = false;



class BackgroundViewCalculator {
   private:
      SDL_Thread *viewThreat;
      SDL_sem* sem;
      bool endThreat;
      int changedFields;

      enum Status { waiting, dataavail, running, finished } status;
      
   public:


      struct Data {
         GameMap* gamemap;
         int view;
         Data( GameMap* map, int v ) : gamemap ( map ), view( v) {};
         Data() : gamemap(NULL), view(-1) {};
      };
      
      BackgroundViewCalculator() : endThreat(false), changedFields(0)
      {
         status = waiting;
         sem = SDL_CreateSemaphore( 1 );
         viewThreat = SDL_CreateThread( &BackgroundViewCalculator::calculator, this );
      }

      void postData( Data data )
      {
         SDL_SemWait( sem );
         if ( status == waiting || status == finished ) {
            this->data = data;
            status = dataavail;
         } else
            fatalError( "Sequence error in BackgroundViewCalculator");
         SDL_SemPost(sem );

      }
      
      bool dataAvail( Data& data )
      {
         bool result;
         SDL_SemWait( sem );
         if ( status == dataavail ) {
            result = true;
            data = this->data;
         } else
            result = false;
         SDL_SemPost(sem );
         return result;
      }

      void setCalculationCompletion( int changedFields )
      {
         SDL_SemWait( sem );
         status = finished;
         this->changedFields = changedFields;
         SDL_SemPost(sem );
      }

      bool isCalculationCompleted()
      {
         bool result;
         SDL_SemWait( sem );
         if ( status == finished ) {
            result = true;
         } else
            result = false;
         SDL_SemPost(sem );
         return result;
      }

      int waitForCompletion()
      {
         while ( !isCalculationCompleted() )
            SDL_Delay(10);
         return changedFields;
      }

      bool haltThreat()
      {
         return endThreat;
      }
      
      ~BackgroundViewCalculator()
      {
         endThreat = true;
         SDL_WaitThread( viewThreat, NULL );
         SDL_DestroySemaphore( sem );
      }

   private:

      Data data;
      
      static int calculator( void* object )
      {
         BackgroundViewCalculator* bvc = static_cast<BackgroundViewCalculator*>(object);
         Data data;
         do {
            SDL_Delay(10);
            if ( bvc->dataAvail( data )) {
               int changedFields = evaluateviewcalculation ( data.gamemap, data.view );
               bvc->setCalculationCompletion( changedFields );
            }
         } while ( !bvc->haltThreat() && !exitprogram );
         return 0;
      }

};
      


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
      
      

int  BaseVehicleMovement :: moveunitxy(AStar3D::Path& pathToMove, int noInterrupt )
{
   WindMovement* wind;

#ifdef WEATHERGENERATOR
   if ( (vehicle->typ->height & ( chtieffliegend | chfliegend | chhochfliegend )) && actmap->weatherSystem->getCurrentWindSpeed() ) {
      wind = new WindMovement ( vehicle );
   } else
      wind = NULL;
#else
   if ( (vehicle->typ->height & ( chtieffliegend | chfliegend | chhochfliegend )) && actmap->weather.windSpeed ) {
      wind = new WindMovement ( vehicle );
   } else
      wind = NULL;
#endif

   tfield* oldfield = getfield( vehicle->xpos, vehicle->ypos );

   AStar3D::Path::iterator pos = path.begin();
   AStar3D::Path::iterator stop = path.end()-1;

   tsearchreactionfireingunits srfu;
   treactionfire* rf = &srfu;

   int orgMovement = vehicle->getMovement( false );
   int orgHeight = vehicle->height;

   rf->init( vehicle, pathToMove );

   if ( oldfield->vehicle == vehicle) {
      vehicle->removeview();
      oldfield->vehicle = NULL;
   } else {
      ContainerBase* cn = oldfield->getContainer();
      cn->removeUnitFromCargo( vehicle );      
   }

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
   int operatingPlayer = vehicle->getOwner();

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
         vehicle->setAttacked();


      if ( next->getRealHeight() != pos->getRealHeight() && next->getRealHeight() >= 0 )
         vehicle->setNewHeight ( 1 << next->getRealHeight() );

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

         tfield* dest = getfield ( to.x, to.y );


         if ( vehicle ) {
            vehicle->xpos = to.x;
            vehicle->ypos = to.y;
            vehicle->addview();
         }

         
         printTimer(1);
         static BackgroundViewCalculator* bvc = NULL;
         if ( multiThreadedViewCalculation ) {
            
            
            if ( !bvc )
               bvc = new BackgroundViewCalculator;

            printTimer(2);
            
            int view;
            if ( actmap->getPlayerView() >= 0 )
               view = 1 << actmap->getPlayerView() ;
            else
               view = 0;
               
            
            bvc->postData( BackgroundViewCalculator::Data( actmap, view ));
            printTimer(3);
         }

         
         if ( mapDisplay ) {

            if ( next == stop && to.x==next->x && to.y==next->y) // the unit will reach its destination
               slm.fadeOut ( CGameOptions::Instance()->movespeed * 10 );
            mapDisplay->displayMovingUnit ( from, to, vehicle, pathStep, pathStepNum, MapDisplayInterface::SoundStartCallback( &slm, &SoundLoopManager::activate ));
            finalRedrawNecessary = true;
            mapDisplayUpToDate = false;
         }
         pathStep++;

         printTimer(4);

         if ( vehicle ) {
            if ( vehicle->spawnMoveObjects( from, to ) )
               mapDisplayUpToDate = false;
            
            int dir = getdirection( from, to );
            if ( dir >= 0 && dir <= 5 )
               vehicle->direction = dir;
            if ( inhibitAttack )
               vehicle->setAttacked();
         }

         
         printTimer(5);
         if ( multiThreadedViewCalculation ) {
            if ( bvc->waitForCompletion())
               mapDisplayUpToDate = false;
               
         } else {
            Vehicle* temp = dest->vehicle;
            
            dest->vehicle = vehicle;
            int fieldsWidthChangedVisibility; 
            if ( actmap->getPlayerView() >= 0 ) 
               fieldsWidthChangedVisibility = evaluateviewcalculation ( actmap, 1 << actmap->getPlayerView() );
            else 
               fieldsWidthChangedVisibility = evaluateviewcalculation ( actmap, 0);
            
            if ( fieldsWidthChangedVisibility )
               mapDisplayUpToDate = false;
            dest->vehicle = temp;
         }
         printTimer(6);

         viewInputChanged = false;

         if ( vehicle ) {

            if ( mapDisplay && fieldvisiblenow ( dest, vehicle, actmap->getPlayerView() ) ) {
               // here comes an ugly hack to get the shadow of starting / descending aircraft right

               int oldheight = vehicle->height;
               if ( next->getRealHeight() > pos->getRealHeight() && pathStep < pathStepNum )
                  vehicle->height = 1 << pos->getRealHeight();
               
               if ( !mapDisplayUpToDate ) {
                  mapDisplay->displayMap( vehicle );
                  mapDisplayUpToDate = true;
                  finalRedrawNecessary = false;
               }

               vehicle->height = oldheight;
            }

            if ( rf->checkfield ( to, vehicle, mapDisplay )) {
               cancelmovement = 1;
               attackedByReactionFire = true;
               vehicle = actmap->getUnit ( networkID );
            }
            if ( !vehicle && mapDisplay ) {
               mapDisplay->displayMap();
               mapDisplayUpToDate = true;
               finalRedrawNecessary = false;
               
               viewInputChanged = true;
            }
         } else
            if ( mapDisplay ) {
               mapDisplay->displayMap();
               mapDisplayUpToDate = true;
               finalRedrawNecessary = false;
             }

         printTimer(7);
            
            
         if ( vehicle ) {
            if ( !(stop->x == to.x && stop->y == to.y && next == stop ))
               vehicle->removeview();
            
            if ( dest->mineattacks ( vehicle )) {
               tmineattacksunit battle ( dest, -1, vehicle );

               if ( mapDisplay && (fieldvisiblenow ( dest, actmap->getPlayerView()) || dest->mineowner() == actmap->getPlayerView() ))
                  mapDisplay->showBattle( battle );
               else
                  battle.calc();

               battle.setresult ();
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
            if ( noInterrupt > 0 )
               cancelmovement = 0;

         if ( vehicle ) {
            npush ( dest->vehicle );
            dest->vehicle = vehicle;
            if ( dest->connection & cconnection_areaentered_anyunit )
               fieldCrossed();

            if ((dest->connection & cconnection_areaentered_specificunit ) && ( vehicle->connection & cconnection_areaentered_specificunit ))
               fieldCrossed();
               
            npop ( dest->vehicle );
         }
         printTimer(8);
      } while ( (to.x != next->x || to.y != next->y) && vehicle );

      pos = next;
   }

   tfield* fld = getfield ( pos->x, pos->y );

   actmap->time.set ( actmap->time.turn(), actmap->time.move()+1);

   int result = 0;

   if ( vehicle ) {

      int newMovement = orgMovement - pos->dist;

      vehicle->setnewposition ( pos->x, pos->y );

      if ( vehicle->typ->movement[log2(orgHeight)] ) {
         int nm = int(floor(vehicle->maxMovement() * float(newMovement) / float(vehicle->typ->movement[log2(orgHeight)]) + 0.5));
//         if ( nm < 0 )
//            result = -1;
         vehicle->setMovement ( nm );
         
         // the unit will be shaded if movement is exhausted
         if ( vehicle->getMovement() < 10 )
            finalRedrawNecessary = true;
      }


      vehicle->getResource ( fueldist * vehicle->typ->fuelConsumption / maxmalq, Resources::Fuel, false );

      if ( fld->vehicle || fld->building ) {
         vehicle->setMovement ( 0 );
         vehicle->attacked = true;
      }

      if ( vehicle ) {
         if ((fld->vehicle == NULL) && (fld->building == NULL)) {
            if ( !vehicle->isViewing() ) {
               vehicle->addview();
               viewInputChanged = true;

               // do we really need this check?
               if ( rf->checkfield ( *pos, vehicle, mapDisplay )) {
                  attackedByReactionFire = true;
                  vehicle = actmap->getUnit ( networkID );
               }

            }
            fld->vehicle = vehicle;

         } else {
            ContainerBase* cn = fld->getContainer();
            if ( vehicle->isViewing() ) {
               vehicle->removeview();
               viewInputChanged = true;
            }
            cn->addToCargo( vehicle );
            if (cn->getOwner() != vehicle->getOwner() && fld->building && actmap->getPlayer(fld->building).diplomacy.isHostile( vehicle) ) {
               fld->building->convert( vehicle->color / 8 );
               if ( fieldvisiblenow ( fld, actmap->getPlayerView() ) || actmap->getPlayerView()  == vehicle->getOwner() )
                  SoundList::getInstance().playSound ( SoundList::conquer_building, 0 );
           }
           mapDisplayUpToDate = false;

         }

      }
   }

   if ( rf->finalCheck( mapDisplay, operatingPlayer ))
      finalRedrawNecessary = true;
   
   // finalRedrawNecessary = true;
   
   if ( viewInputChanged ) {
      int fieldschanged;
      if ( actmap->getPlayerView() >= 0 )
         fieldschanged = evaluateviewcalculation ( actmap, 1 << actmap->getPlayerView() );
      else
         fieldschanged = evaluateviewcalculation ( actmap, 0 );
      
      if ( fieldschanged )
         mapDisplayUpToDate = false;
         
   }

   if ( mapDisplay ) {
      mapDisplay->resetMovement();
      // if ( fieldschanged > 0 )
      if (finalRedrawNecessary || !mapDisplayUpToDate)
         mapDisplay->displayMap();
      // else
      //   mapDisplay->displayPosition ( pos->x, pos->y );
   }
   return result;
}



int BaseVehicleMovement :: execute ( Vehicle* veh, int x, int y, int step, int height, int noInterrupt )
{
   if ( step != status )
      return -1;

   if ( status == 0 ) {
      vehicle = veh ;
      if ( !vehicle ) {
         status = -101;
         return status;
      }

      MapCoordinate3D dest;
      dest.setnum ( x, y, height );

      AStar3D ast ( actmap, vehicle, false, maxint );
      ast.findPath ( path, dest );

      status = 3;
      return status;
   } else
    if ( status == 3 ) {
       if ( path.empty() || path.rbegin()->x != x || path.rbegin()->y != y  ) {
          status = -105;
          return status;
       }

       int nwid = vehicle->networkid;
       int xp = vehicle->xpos;
       int yp = vehicle->ypos;

       if ( mapDisplay )
          mapDisplay->startAction();
       int stat = moveunitxy( path, noInterrupt );
       if ( mapDisplay )
          mapDisplay->stopAction();


       int destDamage;
       if ( actmap->getUnit( nwid ))
          destDamage = vehicle->damage;
       else
          destDamage = 100;

       logtoreplayinfo ( rpl_move5, xp, yp, x, y, nwid, path.rbegin()->getNumericalHeight(), noInterrupt, destDamage );

       if ( stat < 0 )
          status = stat;
       else
          status = 1000;

       return status;

    } else
       status = 0;
  return status;
}


int BaseVehicleMovement :: available ( Vehicle* veh ) const
{
   if ( status == 0 )
      if ( veh )
         return veh->canMove();
   return 0;
}





VehicleAction :: VehicleAction ( VehicleActionType _actionType, PPendingVehicleActions _pva  )
{
   if ( _pva ) {
      if ( _pva->action )
         displaymessage ( " VehicleAction :: VehicleAction type %d / another action (type %d) is running !", 2 , _actionType, _pva->actionType );

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
         displaymessage ( " VehicleAction :: VehicleAction type %d / another action (type %d) is running !", 2 , _actionType, _pva->actionType );

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
                 : BaseVehicleMovement ( vat_move, _pva, md )
{
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

bool VehicleMovement :: avail ( Vehicle* veh ) 
{
   if ( veh )
      return veh->canMove();
   return false;
}


      class HeightChangeLimitation: public AStar3D::OperationLimiter {
              bool allow_Height_Change;
           public:
              HeightChangeLimitation ( bool allow_Height_Change_ ) : allow_Height_Change ( allow_Height_Change_ ) {};
              virtual bool allowHeightChange() { return allow_Height_Change; };
              virtual bool allowMovement() { return true; };
              virtual bool allowEnteringContainer() { return true; };
              virtual bool allowLeavingContainer() { return true; };
              virtual bool allowDocking() { return true; };
      };


int VehicleMovement :: execute ( Vehicle* veh, int x, int y, int step, int height, int capabilities )
{
   if ( step != status )
      return -1;

   if ( status == 0 ) {
      vehicle = veh ;
      if ( !vehicle ) {
         status = -101;
         return status;
      }

      int h;
      if ( actmap->getField(veh->getPosition())->unitHere(veh) )
         h = log2(veh->height); // != height-change: true
      else {
         h = -1; // height-change = false

         // resetting unit movement
         int mx = -1;
         int height = veh->height;
         for ( int h = 0; h < 8; h++ )
            if ( veh->typ->height & ( 1 << h))
               if ( veh->typ->movement[h] > mx ) {
                  mx = veh->typ->movement[h];
                  height = 1 << h;
               }
         veh->setNewHeight( height );
      }
      if ( height == -2 )
         h = -1;

      bool heightChange = true; //  = h != -1;
      if ( capabilities & DisableHeightChange )
        heightChange = false;
      HeightChangeLimitation hcl ( heightChange );

      PathFinder pf ( actmap, vehicle, veh->getMovement() );
      pf.registerOperationLimiter ( &hcl );
      pf.getMovementFields ( reachableFields, reachableFieldsIndirect, h );

      if ( reachableFields.getFieldNum() == 0 ) {
         status =  -107;
         return status;
      }

      status = 2;
      return status;
  } else
   if ( status == 2 ) {
      if ( !reachableFields.isMember ( x, y )) {
         status = -105;
         return status;
      }

      MapCoordinate3D dest;
      if ( height >= 0 )
         dest.setnum ( x, y, height );
      else
         dest.setnum ( x, y, reachableFields.getData(x,y) );

      AStar3D ast ( actmap, vehicle, false, maxint );
      ast.findPath ( path, dest );

      status = 3;
      return status;
   } else
      if ( status == 3 ) {
         int res = BaseVehicleMovement::execute ( veh, x, y, step, height, capabilities & NoInterrupt );
         mapChanged( actmap );
         return res;
      }
      else
         status = 0;

  return status;
}









ChangeVehicleHeight :: ChangeVehicleHeight ( MapDisplayInterface* md, PPendingVehicleActions _pva, VehicleActionType vat, int _dir )
                 : BaseVehicleMovement ( vat, _pva, md )
{
   dir = _dir;
}

      class MovementLimitation: public AStar3D::OperationLimiter {
              bool simpleMode;
              int hcNum;
           public:
              MovementLimitation ( bool simpleMode_ ) : simpleMode ( simpleMode_ ), hcNum(0) {};
              virtual bool allowHeightChange() { ++hcNum; if ( simpleMode) return hcNum <= 1 ; else return true; };
              virtual bool allowMovement() { return !simpleMode; };
              virtual bool allowEnteringContainer() { return true; };
              virtual bool allowLeavingContainer() { return true; };
              virtual bool allowDocking() { return true; };
      };


int ChangeVehicleHeight :: execute ( Vehicle* veh, int x, int y, int step, int noInterrupt, int disableMovement )
{
   if ( step != status )
      return -1;

   if ( status == 0 ) {
      vehicle = veh ;
      if ( !vehicle ) {
         status = -101;
         return status;
      }

      if ( !actmap->getField(veh->getPosition())->unitHere(veh) )
         return -118;

      const Vehicletype::HeightChangeMethod* hcm = veh->getHeightChange( dir );
      if ( !hcm )
         fatalError ( "Inconsistend call to changeheight ");

      newheight = veh->getPosition().getNumericalHeight() + hcm->heightDelta;



      PathFinder pf ( actmap, vehicle, vehicle->getMovement() );
      MovementLimitation ml ( disableMovement );
      pf.registerOperationLimiter ( &ml );

      IntFieldList reachableFieldsIndirect;
      pf.getMovementFields ( reachableFields, reachableFieldsIndirect, newheight );

      if ( reachableFields.getFieldNum() == 0 ) {
         status =  -107;
         return status;
      }

      status = 2;
      return status;

  } else
   if ( status == 2 ) {
      if ( !reachableFields.isMember ( x, y )) {
         status = -105;
         return status;
      }

      MapCoordinate3D dest;
      dest.setnum ( x, y, reachableFields.getData(x,y) );

      AStar3D ast ( actmap, vehicle, false, maxint );
      ast.findPath ( path, dest );

      status = 3;
      return status;
   } else
    if ( status == 3 )
       return BaseVehicleMovement::execute ( veh, x, y, step, newheight, noInterrupt );
    else
       status = 0;
  return status;
}


IncreaseVehicleHeight :: IncreaseVehicleHeight ( MapDisplayInterface* md, PPendingVehicleActions _pva )
                 : ChangeVehicleHeight ( md, _pva, vat_ascent, +1 )
{
   if ( pva )
      pva->ascent = this;
}

bool IncreaseVehicleHeight :: avail ( Vehicle* veh )
{
   if ( veh )
      if ( veh->getHeightChange( +1 ))
         return true;
   return false;
}

int IncreaseVehicleHeight :: available ( Vehicle* veh ) const
{
   return avail( veh );
}


IncreaseVehicleHeight :: ~IncreaseVehicleHeight ( )
{
   if ( pva )
      pva->ascent = NULL;
}



DecreaseVehicleHeight :: DecreaseVehicleHeight ( MapDisplayInterface* md, PPendingVehicleActions _pva )
                 : ChangeVehicleHeight ( md, _pva, vat_descent, -1 )
{
   if ( pva )
      pva->descent = this;
}

bool DecreaseVehicleHeight :: avail ( Vehicle* veh )
{
   if ( veh )
         if ( veh->getHeightChange( -1 ))
            return true;
   return false;
}

int DecreaseVehicleHeight :: available ( Vehicle* veh ) const
{
   return avail(veh);
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









VehicleAttack :: VehicleAttack ( MapDisplayInterface* md, PPendingVehicleActions _pva )
               : VehicleAction ( vat_attack, _pva ), search ( actmap )
{
   status = 0;
   mapDisplay = md;
   if ( pva )
      pva->attack = this;
}


bool VehicleAttack :: avail ( Vehicle* eht )
{
   if ( eht )
      if ( eht->attacked == false )
         if ( eht->weapexist() )
            if (eht->typ->wait == false  ||  !eht->hasMoved() )
                  return true;
   return false;
}


int VehicleAttack :: execute ( Vehicle* veh, int x, int y, int step, int _kamikaze, int weapnum )
{
   if ( step != status )
      return -1;

   if ( status == 0 ) {
      vehicle = veh ;
      if ( !vehicle ) {
         status = -101;
         return status;
      }

      int weaponCount = 0;
      int shootableWeaponCount = 0;
      for ( int w = 0; w < vehicle->typ->weapons.count; w++ )
         if ( vehicle->typ->weapons.weapon[w].shootable() ) {
              weaponCount++;
              if ( vehicle->typ->weapons.weapon[w].sourceheight & vehicle->height )
                 shootableWeaponCount++;
         }

      if ( weaponCount == 0 )
         return -214;

      if ( shootableWeaponCount == 0 )
         return -213;


      kamikaze = _kamikaze;

      search.init( veh, kamikaze, this ); 
      int res = search.run();
      if ( res < 0 ) {
         status = res;
         return status;
      }

      if ( search.anzahlgegner <= 0 ) {
         status =  -206;
         return status;
      }

      status = 2;
      return status;
  } else
  if ( status == 2 ) {
      pattackweap atw = NULL;
      if ( attackableVehicles.isMember ( x, y ))
         atw = &attackableVehicles.getData ( x, y );
      else
         if ( attackableObjects.isMember ( x, y ))
            atw = &attackableObjects.getData ( x, y );
         else
            if ( attackableBuildings.isMember ( x, y ))
               atw = &attackableBuildings.getData ( x, y );

      if ( !atw ) {
         status = -1;
         return status;
      }
      tfight* battle = NULL;
      switch ( atw->target ) {
         case AttackWeap::vehicle: battle = new tunitattacksunit ( vehicle, getfield(x,y)->vehicle, 1, weapnum );
            break;
         case AttackWeap::building: battle = new tunitattacksbuilding ( vehicle, x, y , weapnum );
            break;
         case AttackWeap::object: battle = new tunitattacksobject ( vehicle, x, y, weapnum );
            break;
         default : status = -1;
                   return status;
      } /* endswitch */

      int ad1 = battle->av.damage;
      int dd1 = battle->dv.damage;

      int xp1 = vehicle->xpos;
      int yp1 = vehicle->ypos;

      int shown;
      if ( mapDisplay && fieldvisiblenow ( getfield ( x, y ), actmap->getPlayerView()) ) {
         mapDisplay->displayActionCursor ( vehicle->xpos, vehicle->ypos, x, y );
         mapDisplay->showBattle( *battle );
         mapDisplay->removeActionCursor ( );
         shown = 1;
      } else {
         battle->calc();
         shown = 0;
      }

      int ad2 = battle->av.damage;
      int dd2 = battle->dv.damage;

      if ( !vehicle->typ->hasFunction( ContainerBaseType::MoveAfterAttack )) 
         vehicle->setMovement ( 0 );

      battle->setresult ();

      logtoreplayinfo ( rpl_attack, xp1, yp1, x, y, ad1, ad2, dd1, dd2, weapnum );

      evaluateviewcalculation( actmap );

      if ( mapDisplay && shown )
         mapDisplay->displayMap();


      status = 1000;
  }
  return status;
}



void     VehicleAttack :: tsearchattackablevehicles :: init( const Vehicle* eht, int _kamikaze, VehicleAttack* _va )
{ 
   angreifer = eht; 
   kamikaze = _kamikaze; 
   va = _va;
}



void     VehicleAttack :: tsearchattackablevehicles::testfield( const MapCoordinate& mc )
{ 
   if ( fieldvisiblenow( gamemap->getField(mc)) ) {
      if ( !kamikaze ) {
         pattackweap atw = attackpossible( angreifer, mc.x, mc.y );
         if (atw->count > 0) { 
            switch ( atw->target ) {
               case AttackWeap::vehicle:  va->attackableVehicles.addField  ( mc, *atw );
                  break;                                    
               case AttackWeap::building: va->attackableBuildings.addField ( mc, *atw );
                  break;
               case AttackWeap::object:   va->attackableObjects.addField   ( mc, *atw );
                  break;
               default:;
            } /* endswitch */
            anzahlgegner++;
         } 
         delete atw;
      } else {
          tfield* fld = gamemap->getField(mc);
          if (fieldvisiblenow( fld )) {
             Vehicle* eht = fld->vehicle;
             if (eht != NULL) 
                if (((angreifer->height >= chtieffliegend) && (eht->height <= angreifer->height) && (eht->height >= chschwimmend)) 
                  || ((angreifer->height == chfahrend) && (eht->height == chfahrend)) 
                  || ((angreifer->height == chschwimmend) && (eht->height == chschwimmend))
                  || ((angreifer->height == chgetaucht) && (eht->height >=  chgetaucht) && (eht->height <= chschwimmend))) {
                   fld->a.temp = dist;
                   anzahlgegner++; 
                } 
          } 
      }
   } 
} 


int      VehicleAttack :: tsearchattackablevehicles::run( void )
{ 
   anzahlgegner = 0; 

   if (angreifer == NULL) 
      return -201;

   moveparams.movesx = angreifer->xpos;  // this is currently still needed for wepselguihost
   moveparams.movesy = angreifer->ypos;
   if (fieldvisiblenow(getfield(angreifer->xpos,angreifer->ypos)) == false)
      return -1;

   if (angreifer->attacked)
      return -202;
   

   if (angreifer->typ->weapons.count == 0)
      return -204;

   if ( angreifer->typ->wait && angreifer->hasMoved() && angreifer->reactionfire.getStatus() != Vehicle::ReactionFire::ready )
         return -215;


   int d = 0;
   int maxdist = 0;
   int mindist = 20000;
   for ( int a = 0; a < angreifer->typ->weapons.count; a++)
      if (angreifer->ammo[a] > 0) {
         d++;
         maxdist = max( maxdist, angreifer->typ->weapons.weapon[a].maxdistance / maxmalq );
         mindist = min ( mindist, (angreifer->typ->weapons.weapon[a].mindistance + maxmalq - 1) / maxmalq);
      }


   if (d == 0)
      return -204;

   initsearch( angreifer->getPosition(), maxdist, mindist );
   startsearch();

   return 0;
}

void VehicleAttack :: registerPVA ( VehicleActionType _actionType, PPendingVehicleActions _pva )
{
   VehicleAction::registerPVA ( _actionType, _pva );
   if ( pva )
      pva->attack = this;
}


VehicleAttack :: ~VehicleAttack ( )
{
   if ( pva )
      pva->attack = NULL;
}




////////////////////////////////////////////////////////////////////////////////////////////////////////

VehicleService :: VehicleService ( MapDisplayInterface* md, PPendingVehicleActions _pva )
               : VehicleAction ( vat_service, _pva ), fieldSearch ( *this, actmap )
{
   building = NULL;
   vehicle = NULL;
   status = 0;
   mapDisplay = md;
   guimode = 0;
   if ( pva )
      pva->service = this;
}


int  VehicleService :: avail ( const Vehicle* veh )
{
   int av = 0;
   if ( veh && !veh->attacked ) {
      if ( veh->canRepair( NULL ) && (veh->typ->hasFunction( ContainerBaseType::ExternalRepair  )))
         for ( int i = 0; i < veh->typ->weapons.count; i++ )
            if ( veh->typ->weapons.weapon[i].service() )
               av++;


      const Vehicletype* fzt = veh->typ;
      for ( int i = 0; i < fzt->weapons.count; i++ ) {
         if ( fzt->weapons.weapon[i].service() ) {

            if ( veh->typ->hasFunction( ContainerBaseType::ExternalEnergyTransfer  ) )
               if ( veh->getStorageCapacity().energy )
                  av++;

            if ( veh->typ->hasFunction( ContainerBaseType::ExternalMaterialTransfer  ) )
               if ( veh->getStorageCapacity().material )
                  av++;

            if ( veh->typ->hasFunction( ContainerBaseType::ExternalFuelTransfer  ) )
               if ( veh->getStorageCapacity().fuel )
                  av++;

         }
         if ( fzt->weapons.weapon[i].canRefuel() )
            av++;
      }
   }
   if ( veh->reactionfire.getStatus() == Vehicle::ReactionFire::off )
      return av;
   else
      return 0;
}

int VehicleService :: available ( Vehicle* veh ) const
{
   return avail(veh);
}

int VehicleService :: getServices ( Vehicle* veh ) 
{
   int res = 0;
   if ( veh ) {
      if ( veh->canRepair( NULL ) && (veh->typ->hasFunction( ContainerBaseType::ExternalRepair  )))
         for ( int i = 0; i < veh->typ->weapons.count; i++ )
            if ( veh->typ->weapons.weapon[i].service() )
               if ( !veh->attacked )
                  res |= 1 << srv_repair;


      const Vehicletype* fzt = veh->typ;
      for ( int i = 0; i < fzt->weapons.count; i++ ) {
         if ( fzt->weapons.weapon[i].service() ) {
            if ( veh->typ->hasFunction( ContainerBaseType::ExternalEnergyTransfer  ) )
               if ( veh->getStorageCapacity().energy )
                  res |= 1 << srv_resource;
            if ( veh->typ->hasFunction( ContainerBaseType::ExternalMaterialTransfer  ) )
               if ( veh->getStorageCapacity().material )
                  res |= 1 << srv_resource;
            if ( veh->typ->hasFunction( ContainerBaseType::ExternalFuelTransfer  ) )
               if ( veh->getStorageCapacity().fuel )
                  res |= 1 << srv_resource;
         }


         if ( fzt->weapons.weapon[i].canRefuel() )
            res |= 1 << srv_ammo;
      }
   }
   return res;
}




void             VehicleService :: FieldSearch :: checkVehicle2Vehicle ( Vehicle* targetUnit, int xp, int yp )
{
   VehicleService::Target targ;
   targ.dest = targetUnit;

   if ( xp == startPos.x && yp == startPos.y )
      return;

   int dist;
   if ( bypassChecks.distance )
      dist = maxmalq;
   else
      dist = beeline ( xp, yp , startPos.x, startPos.y );


   const SingleWeapon* serviceWeapon = NULL;
   for (int i = 0; i < veh->typ->weapons.count ; i++)
      if ( veh->typ->weapons.weapon[i].service() )
         serviceWeapon = &veh->typ->weapons.weapon[i];


   if ( serviceWeapon ) {
      int targheight = 0;

      for ( int h = 0; h < 8; h++ )
         if ( serviceWeapon->targ & ( 1 << h ))
            if ( serviceWeapon->efficiency[ 6 + getheightdelta ( log2(veh->height), h ) ] )
               targheight |= 1 << h;

      if ( (serviceWeapon->sourceheight & veh->height) || ( bypassChecks.height && (serviceWeapon->sourceheight & veh->typ->height)))
         for (int i = 0; i < veh->typ->weapons.count ; i++) {
            const SingleWeapon& sourceWeapon = veh->typ->weapons.weapon[i];
            if ( sourceWeapon.service() || sourceWeapon.canRefuel() ) {
               if ( targetUnit && serviceWeapon )
                  if ( !targetUnit->typ->hasFunction(ContainerBaseType::NoInairRefuelling) || targetUnit->height <= chfahrend )
                     if ( serviceWeapon->targetingAccuracy[targetUnit->typ->movemalustyp] > 0  )
                        if ( actmap->player[veh->getOwner()].diplomacy.getState( targetUnit->getOwner()) >= PEACE )
                           if ( (serviceWeapon->maxdistance >= dist && serviceWeapon->mindistance <= dist) || bypassChecks.distance )
                              if ( targetUnit->height & targheight || ( bypassChecks.height && ( targetUnit->typ->height & targheight) )) {
                                 if ( sourceWeapon.canRefuel() ) {
                                    for ( int j = 0; j < targetUnit->typ->weapons.count ; j++) {
                                       const SingleWeapon& targetWeapon = targetUnit->typ->weapons.weapon[j];
                                       if ( targetWeapon.getScalarWeaponType() == sourceWeapon.getScalarWeaponType()
                                            && targetWeapon.requiresAmmo() ) {
                                          VehicleService::Target::Service s;
                                          s.type = VehicleService::srv_ammo;
                                          s.sourcePos = i;
                                          s.targetPos = j;
                                          s.curAmount = targetUnit->ammo[j];
                                          s.orgSourceAmount = veh->ammo[i];
                                          s.maxAmount = min ( targetWeapon.count, s.curAmount+s.orgSourceAmount );
                                          int sourceSpace = sourceWeapon.count - veh->ammo[i];
                                          s.minAmount = max ( s.curAmount - sourceSpace, 0 );
                                          if ( targetWeapon.count )
                                             s.maxPercentage = 100 * s.maxAmount/ targetWeapon.count;
                                          else
                                             s.maxPercentage = 0;
                                          targ.service.push_back ( s );
                                       }
                                    }
                                 }

                                 if ( sourceWeapon.service() ) {
                                    static ContainerBaseType::ContainerFunctions resourceVehicleFunctions[resourceTypeNum] = { ContainerBaseType::ExternalEnergyTransfer,
                                                                                             ContainerBaseType::ExternalMaterialTransfer,
                                                                                             ContainerBaseType::ExternalFuelTransfer };
                                    for ( int r = 0; r < resourceTypeNum; r++ )
                                       if ( veh->getStorageCapacity().resource(r) && targetUnit->getStorageCapacity().resource(r) && veh->typ->hasFunction(resourceVehicleFunctions[r])) {
                                          VehicleService::Target::Service s;
                                          s.type = VehicleService::srv_resource;
                                          s.sourcePos = r;
                                          s.targetPos = r;
                                          s.curAmount = targetUnit->getTank().resource(r);
                                          s.orgSourceAmount = veh->getTank().resource(r);
                                          s.maxAmount = s.curAmount + min ( targetUnit->putResource(maxint, r, 1) , s.orgSourceAmount );
                                          int sourceSpace = veh->putResource(maxint, r, 1);
                                          s.minAmount = max ( s.curAmount - sourceSpace, 0 );
                                          s.maxPercentage = 100 * s.maxAmount/ veh->getStorageCapacity().resource(r);
                                          targ.service.push_back ( s );
                                       }

                                       if ( veh->canRepair( targetUnit ) && (veh->typ->hasFunction( ContainerBaseType::ExternalRepair )))
                                          if ( veh->getTank().fuel && veh->getTank().material )
                                         // if ( targetUnit->getMovement() >= movement_cost_for_repaired_unit )
                                             if ( targetUnit->damage ) {
                                                VehicleService::Target::Service s;
                                                s.type = VehicleService::srv_repair;
                                                s.sourcePos = -1;
                                                s.targetPos = -1;
                                                s.curAmount = targetUnit->damage;
                                                s.orgSourceAmount = 100;
                                                s.maxAmount = targetUnit->damage;
                                                s.minAmount = veh->getMaxRepair ( targetUnit );
                                                s.maxPercentage = 100 - s.minAmount;
                                                targ.service.push_back ( s );
                                             }

                                 }
                              }
            }
         }
      }

   if ( vs.dest.find ( targ.dest->networkid ) != vs.dest.end() ) {
      vs.dest[ targ.dest->networkid ] = targ;
   } else
      if ( targ.service.size() > 0 )
         vs.dest[ targ.dest->networkid ] = targ;

}

void             VehicleService :: FieldSearch :: checkBuilding2Vehicle ( Vehicle* targetUnit )
{
   if ( actmap->player[bld->getOwner()].diplomacy.getState( targetUnit->getOwner()) < PEACE )
      return;
      
   VehicleService::Target targ;
   MapCoordinate pos = bld->getEntry();
   int dist = beeline ( pos.x, pos.y, targetUnit->xpos, targetUnit->ypos );
   if ( dist > 0 )
      if ( ! (targetUnit->height & bld->typ->externalloadheight))
         return;

   targ.dest = targetUnit;


/*   if ( bld->typ->special & (cgexternalloadingb | cgexternalresourceloadingb ))
      for ( int r = 1; r < resourceTypeNum; r++ )  // no energy !!
         if ( targetUnit->typ->tank.resource(r) ) {
            VehicleService::Target::Service s;
            s.type = VehicleService::srv_resource;
            s.sourcePos = r;
            s.targetPos = r;
            s.curAmount = targetUnit->getTank().resource(r);
            // s.orgSourceAmount = bld->getResource (maxint, r, 1 );
            s.orgSourceAmount = buildingResources.resource(r);
            s.maxAmount = s.curAmount + min ( targetUnit->putResource(maxint, r, 1) , s.orgSourceAmount );
            // int sourceSpace = bld->putResource(maxint, r, 1);
            int sourceSpace = resourcesCapacity.resource(r);
            s.minAmount = max ( s.curAmount - sourceSpace, 0 );
            targ.service.push_back ( s );
         }
*/
   for (int i = 0; i < targetUnit->typ->weapons.count ; i++)
      if ( targetUnit->typ->weapons.weapon[i].requiresAmmo() ) {
         int type = targetUnit->typ->weapons.weapon[i].getScalarWeaponType();
         if ( type >= 0 )
            if ( (bld->ammo[type] || targetUnit->ammo[i] || (bld->typ->hasFunction(ContainerBaseType::AmmoProduction) && bld->typ->hasFunction(ContainerBaseType::ExternalAmmoTransfer)) || dist == 0 )) {
               const SingleWeapon& destWeapon = targetUnit->typ->weapons.weapon[i];

               VehicleService::Target::Service s;
               s.type = VehicleService::srv_ammo;
               s.sourcePos = type;
               s.targetPos = i;
               s.curAmount = targetUnit->ammo[i];
               s.orgSourceAmount = bld->ammo[type];
               int stillNeeded = destWeapon.count - targetUnit->ammo[i] - s.orgSourceAmount;
               int produceable;
               if ( (stillNeeded > 0) && (bld->typ->hasFunction( ContainerBaseType::AmmoProduction  ))) {
                  Resources res;
                  for( int j = 0; j< resourceTypeNum; j++ )
                     res.resource(j) = cwaffenproduktionskosten[type][j] * stillNeeded;

                  // ContainerBase* cb = bld;
                  // Resources res2 = cb->getResource ( res, 1 );

                  int perc = 100;
                  for ( int i = 0; i< resourceTypeNum; i++ )
                      if ( res.resource(i) )
                         perc = min ( perc, 100 * buildingResources.resource(i) / res.resource(i) );

                  produceable = stillNeeded * perc / 100 ;
               } else
                  produceable = 0;

               s.maxAmount = min ( destWeapon.count,  targetUnit->ammo[i] + s.orgSourceAmount + produceable );
               s.minAmount = 0;
               targ.service.push_back ( s );
            }
      }

      static ContainerBaseType::ContainerFunctions resourceVehicleFunctions[resourceTypeNum] = { ContainerBaseType::ExternalEnergyTransfer,
         ContainerBaseType::ExternalMaterialTransfer,
         ContainerBaseType::ExternalFuelTransfer };
      for ( int r = 1; r < resourceTypeNum; r++ )  // no energy !!
         if ( (bld->typ->hasFunction( resourceVehicleFunctions[r]) || dist == 0) && targetUnit->getStorageCapacity().resource(r) ) {
            VehicleService::Target::Service s;
            s.type = VehicleService::srv_resource;
            s.sourcePos = r;
            s.targetPos = r;
            s.curAmount = targetUnit->getTank().resource(r);
            // s.orgSourceAmount = bld->getResource (maxint, r, 1 );
            s.orgSourceAmount = buildingResources.resource(r);
            s.maxAmount = s.curAmount + min ( targetUnit->putResource(maxint, r, 1) , s.orgSourceAmount );
            // int sourceSpace = bld->putResource(maxint, r, 1);
            int sourceSpace = resourcesCapacity.resource(r);
            s.minAmount = max ( s.curAmount - sourceSpace, 0 );
            targ.service.push_back ( s );
         }


   if ( bld->canRepair( targetUnit ) )
      if ( targetUnit->damage ) {
         VehicleService::Target::Service s;
         s.type = VehicleService::srv_repair;
         s.sourcePos = -1;
         s.targetPos = -1;
         s.curAmount = targetUnit->damage;
         s.orgSourceAmount = 100;
         s.maxAmount = targetUnit->damage;
         s.minAmount = bld->getMaxRepair ( targetUnit );
         targ.service.push_back ( s );
      }

   if ( vs.dest.find ( targ.dest->networkid ) != vs.dest.end() ) {
      vs.dest[ targ.dest->networkid ] = targ;
   } else
      if ( targ.service.size() > 0 )
         vs.dest[ targ.dest->networkid ] = targ;
}


void  VehicleService :: FieldSearch :: testfield( const MapCoordinate& mc )
{
   tfield* fld = gamemap->getField ( mc );
   if ( fld && veh && fld->vehicle ) {
      if ( fld->vehicle == veh ) {
         for ( ContainerBase::Cargo::const_iterator i = veh->getCargo().begin(); i != veh->getCargo().end(); ++i )
            if ( *i )
              checkVehicle2Vehicle ( *i, mc.x, mc.y );
      }
      if ( fld->vehicle )
         checkVehicle2Vehicle ( fld->vehicle, mc.x, mc.y );
   }

   if ( fld && bld ) {
      if ( fld->building == bld && beeline( mc.x, mc.y, startPos.x, startPos.y)== 0) {
         for ( ContainerBase::Cargo::const_iterator i = bld->getCargo().begin(); i != bld->getCargo().end(); ++i )
            if ( *i )
               checkBuilding2Vehicle ( *i );
      }
      if ( fld->vehicle )
         checkBuilding2Vehicle ( fld->vehicle );
   }
}


bool  VehicleService :: FieldSearch ::initrefuelling( int xp1, int yp1 )
{
   int mindist = maxint;
   int maxdist = minint;
   if ( veh ) {
      for ( int i = 0; i < veh->typ->weapons.count; i++ ) {
         const SingleWeapon& w = veh->typ->weapons.weapon[i];
         if ( w.service() || w.canRefuel() ) {
            maxdist = max( maxdist, w.maxdistance / maxmalq );
            mindist = min ( mindist, (w.mindistance + maxmalq - 1) / maxmalq);
         }
      }
   }

   if ( bld ) {
      if ( bld->typ->hasFunction( ContainerBaseType::ExternalEnergyTransfer  ) ||
           bld->typ->hasFunction( ContainerBaseType::ExternalMaterialTransfer  ) ||
           bld->typ->hasFunction( ContainerBaseType::ExternalFuelTransfer  ) ||
           bld->typ->hasFunction( ContainerBaseType::ExternalAmmoTransfer  ) )
         maxdist = 1;
      else
         maxdist = 0;
      mindist = 0;
   }

   initsearch( MapCoordinate( xp1,yp1),mindist,maxdist);
   return true;
}



void VehicleService :: FieldSearch :: init ( Vehicle* _veh, Building* _bld )
{
   if ( !_veh && !_bld)
      return;

   veh = _veh;
   bld = _bld;

   if ( veh )
      initrefuelling ( veh->xpos, veh->ypos );
   else {
      MapCoordinate mc = bld->getEntry();
      initrefuelling ( mc.x, mc.y );
   }

}

void VehicleService :: FieldSearch :: run (  )
{
   if ( bld )
      for ( int r = 0; r < 3; ++r ) {
         buildingResources.resource(r) = bld->getResource( maxint, r, 1 );
         resourcesCapacity.resource(r) = bld->putResource( maxint, r, 1 );
      }
   startsearch();
}


int VehicleService :: execute ( Vehicle* veh, int targetNWID, int dummy, int step, int pos, int amount )
{
   if ( step != status )
      return -1;

   if ( status == 0 ) {
      vehicle = veh ;
      if ( !vehicle ) {
         int xp = targetNWID;
         int yp = dummy;
         if ( xp >= 0 && yp >= 0 )
            building = getfield ( xp, yp )->building;

         if ( !building ) {
            status = -101;
            return status;
         }
      }

      fieldSearch.init ( vehicle, building );
      fieldSearch.run (  );
      if ( dest.size() > 0 )
         status = 2;
      else
         status = -210;

      return status;
  } else
  if ( status == 2 ) {
     TargetContainer::iterator i = dest.find(targetNWID);
     if ( i == dest.end() )
        return -211;

     Target& t = i->second;
     if ( pos < 0 || pos >= t.service.size())
        return -211;

     Target::Service& serv = t.service[pos];
     if ( amount < serv.minAmount || amount > serv.maxAmount )
        return -212;

     if ( vehicle ) {
        int delta;
        switch ( serv.type ) {
           case srv_resource: {
                                 delta = amount - serv.curAmount;
                                 int put = t.dest->putResource ( delta, serv.targetPos, 0 );
                                 int oldavail = vehicle->getTank().resource(serv.sourcePos);
                                 vehicle->getResource ( put, serv.sourcePos, 0 );
                                 logtoreplayinfo ( rpl_refuel2, t.dest->xpos, t.dest->ypos, t.dest->networkid, int(1000+serv.targetPos), amount, serv.curAmount );
                                 logtoreplayinfo ( rpl_refuel2, vehicle->xpos, vehicle->ypos, vehicle->networkid, int(1000+serv.sourcePos), vehicle->getTank().resource(serv.sourcePos), oldavail );
                              }
                              break;
           case srv_repair: vehicle->repairItem ( t.dest, amount );
                            logtoreplayinfo ( rpl_repairUnit, vehicle->networkid, t.dest->networkid, amount, vehicle->getTank().material, vehicle->getTank().fuel );
                            /*
                            if ( mapDisplay )
                               if ( fieldvisiblenow ( fld, actmap->playerView ) || actmap->playerView*8  == vehicle->color )
                                  SoundList::getInstance().playSound ( SoundList::conquer_building, 0 );
                            */
                            break;
           case srv_ammo: delta = amount - serv.curAmount;
                          t.dest->ammo[ serv.targetPos ] += delta;
                          vehicle->ammo[ serv.sourcePos ] -= delta;
                          logtoreplayinfo ( rpl_refuel, t.dest->xpos, t.dest->ypos, t.dest->networkid, serv.targetPos, t.dest->ammo[ serv.targetPos ] );
                          logtoreplayinfo ( rpl_refuel, vehicle->xpos, vehicle->ypos, vehicle->networkid, serv.targetPos, vehicle->ammo[ serv.sourcePos ] );
                          break;
        }
     } else if ( building ) {
        int delta;
        switch ( serv.type ) {
           case srv_resource: {
                                 delta = amount - serv.curAmount;
                                 int put = t.dest->putResource ( delta, serv.targetPos, 0 );
                                 building->getResource ( put, serv.sourcePos, 0 );
                                 logtoreplayinfo ( rpl_refuel2, t.dest->xpos, t.dest->ypos, t.dest->networkid, int(1000+serv.targetPos), amount, serv.curAmount );
                                 MapCoordinate mc = building->getEntry();
                                 logtoreplayinfo ( rpl_bldrefuel, mc.x, mc.y, int(1000+serv.sourcePos), put );
                              }
                              break;
           case srv_repair: building->repairItem ( t.dest, amount );
                            // logtoreplayinfo ( rpl_refuel, eht->xpos, eht->ypos, eht->networkid, int(1002), newfuel );
                            break;
           case srv_ammo: delta = amount - serv.curAmount;
                          t.dest->ammo[ serv.targetPos ] += delta;
                          building->ammo[ serv.sourcePos ] -= delta;
                          MapCoordinate mc = building->getEntry();
                          if ( building->ammo[ serv.sourcePos ] < 0 ) {
                             ContainerControls cc ( building );
                             cc.produceAmmo ( serv.sourcePos, -building->ammo[ serv.sourcePos ] );
                          }
                          if ( building->ammo[ serv.sourcePos ] < 0 ) 
                             fatalError("negative amount of ammo available! \nPlease report this to bugs@asc-hq.org" );

                          logtoreplayinfo ( rpl_refuel, t.dest->xpos, t.dest->ypos, t.dest->networkid, serv.targetPos, t.dest->ammo[ serv.targetPos ] );
                          logtoreplayinfo ( rpl_bldrefuel, mc.x, mc.y, serv.targetPos, building->ammo[ serv.sourcePos ] );
                          break;
        }
     }
     fieldSearch.init ( vehicle, building );
     fieldSearch.run (  );
  }
  return status;
}


int VehicleService :: fillEverything ( int targetNWID, bool repairsToo )
{
   if ( status != 2 )
      return -1;

   TargetContainer::iterator i = dest.find(targetNWID);
   if ( i == dest.end() )
      return -211;

   Target& t = i->second;

   for ( int j = 0; j< t.service.size(); j++ ) {
      switch( t.service[j].type ) {
         case srv_repair: 
               if ( repairsToo )
                  execute ( NULL, targetNWID, -1, 2, j, t.service[j].minAmount );
               break;
         case srv_ammo:
         case srv_resource:
               execute ( NULL, targetNWID, -1, 2, j, t.service[j].maxAmount );
               break;
      }
   }

   return 0;
}


void VehicleService :: registerPVA ( VehicleActionType _actionType, PPendingVehicleActions _pva )
{
   VehicleAction::registerPVA ( _actionType, _pva );
   if ( pva )
      pva->service = this;
}


VehicleService :: ~VehicleService ( )
{
   if ( pva )
      pva->service = NULL;
}








////////////////////////////////////////////////////////////////////////////////////////////////////////


#if 1

NewVehicleService :: NewVehicleService ( MapDisplayInterface* md, PPendingVehicleActions _pva )
   : VehicleAction ( vat_newservice, _pva )
{
   container = NULL;
   status = 0;
   mapDisplay = md;
   if ( pva )
      pva->newservice = this;
}

bool NewVehicleService :: targetAvail( const ContainerBase* target )
{
   return find( targets.begin(), targets.end(), target) != targets.end();
}


int  NewVehicleService :: avail ( ContainerBase* veh  )
{
   ServiceTargetSearcher sts( veh );
   return sts.available();
}

int NewVehicleService :: available (  ContainerBase* veh ) const
{
   return avail(veh);
}


int NewVehicleService :: available (  Vehicle* veh ) const
{
   return avail(veh);
}


int NewVehicleService :: execute ( Vehicle* veh, int x, int y, int step, int pos, int amount )
{
   return executeContainer( veh, x, y, step, pos, amount );
}

int NewVehicleService :: executeContainer ( ContainerBase* veh, int x, int y, int step, int pos, int amount )
{
   if ( step != status )
      return -1;

   if ( status == 0 ) {
      container = veh ;
      if ( !container ) {
         status = -101;
         return status;
      }

      ServiceTargetSearcher sts( container );
      sts.startSearch();
      targets = sts.getTargets();
            
      if ( targets.size() > 0 )
         status = 2;
      else
         status = -210;

      return status;
   } 
#if 0
else
      if ( status == 2 ) {
         TargetContainer::iterator i = dest.find(targetNWID);
         if ( i == dest.end() )
            return -211;

         Target& t = i->second;
         if ( pos < 0 || pos >= t.service.size())
            return -211;

         Target::Service& serv = t.service[pos];
         if ( amount < serv.minAmount || amount > serv.maxAmount )
            return -212;

         if ( vehicle ) {
            int delta;
            switch ( serv.type ) {
               case srv_resource: {
                  delta = amount - serv.curAmount;
                  int put = t.dest->putResource ( delta, serv.targetPos, 0 );
                  int oldavail = vehicle->getTank().resource(serv.sourcePos);
                  vehicle->getResource ( put, serv.sourcePos, 0 );
                  logtoreplayinfo ( rpl_refuel2, t.dest->xpos, t.dest->ypos, t.dest->networkid, int(1000+serv.targetPos), amount, serv.curAmount );
                  logtoreplayinfo ( rpl_refuel2, vehicle->xpos, vehicle->ypos, vehicle->networkid, int(1000+serv.sourcePos), vehicle->getTank().resource(serv.sourcePos), oldavail );
               }
               break;
               case srv_repair: vehicle->repairItem ( t.dest, amount );
               logtoreplayinfo ( rpl_repairUnit, vehicle->networkid, t.dest->networkid, amount, vehicle->getTank().material, vehicle->getTank().fuel );
                            /*
               if ( mapDisplay )
               if ( fieldvisiblenow ( fld, actmap->playerView ) || actmap->playerView*8  == vehicle->color )
               SoundList::getInstance().playSound ( SoundList::conquer_building, 0 );
                            */
               break;
               case srv_ammo: delta = amount - serv.curAmount;
               t.dest->ammo[ serv.targetPos ] += delta;
               vehicle->ammo[ serv.sourcePos ] -= delta;
               logtoreplayinfo ( rpl_refuel, t.dest->xpos, t.dest->ypos, t.dest->networkid, serv.targetPos, t.dest->ammo[ serv.targetPos ] );
               logtoreplayinfo ( rpl_refuel, vehicle->xpos, vehicle->ypos, vehicle->networkid, serv.targetPos, vehicle->ammo[ serv.sourcePos ] );
               break;
            }
         } else if ( building ) {
            int delta;
            switch ( serv.type ) {
               case srv_resource: {
                  delta = amount - serv.curAmount;
                  int put = t.dest->putResource ( delta, serv.targetPos, 0 );
                  building->getResource ( put, serv.sourcePos, 0 );
                  logtoreplayinfo ( rpl_refuel2, t.dest->xpos, t.dest->ypos, t.dest->networkid, int(1000+serv.targetPos), amount, serv.curAmount );
                  MapCoordinate mc = building->getEntry();
                  logtoreplayinfo ( rpl_bldrefuel, mc.x, mc.y, int(1000+serv.sourcePos), put );
               }
               break;
               case srv_repair: building->repairItem ( t.dest, amount );
                            // logtoreplayinfo ( rpl_refuel, eht->xpos, eht->ypos, eht->networkid, int(1002), newfuel );
               break;
               case srv_ammo: delta = amount - serv.curAmount;
               t.dest->ammo[ serv.targetPos ] += delta;
               building->ammo[ serv.sourcePos ] -= delta;
               MapCoordinate mc = building->getEntry();
               if ( building->ammo[ serv.sourcePos ] < 0 ) {
                  ContainerControls cc ( building );
                  cc.produceAmmo ( serv.sourcePos, -building->ammo[ serv.sourcePos ] );
               }
               if ( building->ammo[ serv.sourcePos ] < 0 )
                  fatalError("negative amount of ammo available! \nPlease report this to bugs@asc-hq.org" );

               logtoreplayinfo ( rpl_refuel, t.dest->xpos, t.dest->ypos, t.dest->networkid, serv.targetPos, t.dest->ammo[ serv.targetPos ] );
               logtoreplayinfo ( rpl_bldrefuel, mc.x, mc.y, serv.targetPos, building->ammo[ serv.sourcePos ] );
               break;
            }
         }
         fieldSearch.init ( vehicle, building );
         fieldSearch.run (  );
      }
#endif
      return status;
}


int NewVehicleService :: fillEverything ( ContainerBase* target, bool repairsToo )
{
   if ( status != 2 )
      return -1;

   if ( !targetAvail( target ))
      return -211;

   TransferHandler tf( getContainer(), target );
   tf.fillDest();
   tf.commit();

   return 0;
}


void NewVehicleService :: registerPVA ( VehicleActionType _actionType, PPendingVehicleActions _pva )
{
   VehicleAction::registerPVA ( _actionType, _pva );
   if ( pva )
      pva->newservice = this;
}


NewVehicleService :: ~NewVehicleService ( )
{
   if ( pva )
      pva->newservice = NULL;
}



#endif


