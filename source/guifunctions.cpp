/*! \file guiiconhandler.cpp
    \brief All the buttons of the user interface with which the unit actions are
      controlled.
*/

/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2003  Martin Bickel  and  Marc Schellenberger
 
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

#include <stdio.h>
#include <cstring>
#include <stdlib.h>

#include "guifunctions.h"
#include "unitctrl.h"
#include "controls.h"
#include "dialog.h"
#include "gameoptions.h"
#include "iconrepository.h"
#include "replay.h"
#include "attack.h"

namespace GuiFunctions
{



class AttackGui : public GuiIconHandler, public GuiFunction {
     VehicleAttack* attackEngine;

     pair<pattackweap, int> getEntry( const MapCoordinate& pos, int num );
     
   protected:  
      bool available( const MapCoordinate& pos, int num );
      void execute( const MapCoordinate& pos, int num );
      Surface& getImage( const MapCoordinate& pos, int num );
      ASCString getName( const MapCoordinate& pos, int num );
     
   public:
      AttackGui() : attackEngine( NULL ) {};
      void setupWeapons( VehicleAttack* va ) { attackEngine = va; };
      void eval();

};


pair<pattackweap, int> AttackGui::getEntry( const MapCoordinate& pos, int num )
{
   int counter = 0;
   
   AttackFieldList* afl;
   for ( int i = 0; i < 3; ++i ) {
      switch ( i ) {
         case 0: afl = &attackEngine->attackableVehicles; break;
         case 1: afl = &attackEngine->attackableBuildings; break;
         case 2: afl = &attackEngine->attackableObjects; break;
      }
      
      if ( afl->isMember( pos )) {
         pattackweap aw = &afl->getData(pos.x, pos.y);
         for ( int a = 0; a < aw->count; ++a ) {
            if ( counter == num )
               return make_pair( aw, a );
               
            ++counter;
         }
      }   
   }
   return make_pair( pattackweap(NULL), 0 );
}


bool AttackGui::available( const MapCoordinate& pos, int num )
{
   if ( num == -1 )
      return true;
      
   if ( getEntry(pos,num).first )
      return true;
   else
      return false;   
}

void AttackGui::execute( const MapCoordinate& pos, int num )
{
   if ( num != -1 ) {
   
      pair<pattackweap, int> p = getEntry(pos,num);
      if ( p.first ) {
         int res = pendingVehicleActions.attack->execute ( NULL, pos.x, pos.y, 2, 0, p.first->num[p.second] );
         if ( res < 0 )
            dispmessage2 ( -res, NULL );

      }
   }   
      setupWeapons( NULL );
      delete pendingVehicleActions.attack;
      actmap->cleartemps();
      NewGuiHost::popIconHandler();
      repaintMap();
      updateFieldInfo();
}

Surface& AttackGui::getImage( const MapCoordinate& pos, int num )
{
   if ( num == -1 )
      return IconRepository::getIcon("cancel.png");
      
   pair<pattackweap, int> p = getEntry(pos,num);
   switch ( p.first->typ[p.second] ) {
      case cwcruisemissileb: return IconRepository::getIcon("weap-cruisemissile.png");
      case cwbombb: return IconRepository::getIcon("weap-bomb.png");
      case cwairmissileb: return IconRepository::getIcon("weap-bigmissile.png");
      case cwgroundmissileb: return IconRepository::getIcon("weap-smallmissile.png");
      case cwtorpedob: return IconRepository::getIcon("weap-torpedo.png");
      case cwmachinegunb: return IconRepository::getIcon("weap-machinegun.png");
      case cwcannonb: return IconRepository::getIcon("weap-cannon.png");
      default: return IconRepository::getIcon("weap-laser.png");
   };
}

ASCString AttackGui::getName( const MapCoordinate& pos, int num )
{
   if ( num == -1 )
      return "cancel";

      Vehicle* attacker = attackEngine->getAttacker();

      tfight* battle = NULL;

      pair<pattackweap, int> p = getEntry(pos,num);
      
            
      if ( p.first->target == AttackWeap::vehicle )
         battle = new tunitattacksunit ( attacker, actmap->getField(pos)->vehicle, true, p.first->num[p.second] );
      else
      if ( p.first->target == AttackWeap::building )
         battle = new tunitattacksbuilding ( attacker, pos.x, pos.y, p.first->num[p.second] );
      else
      if ( p.first->target == AttackWeap::object )
         battle = new tunitattacksobject ( attacker, pos.x, pos.y, p.first->num[p.second] );


      int dd = battle->dv.damage;
      int ad = battle->av.damage;
      battle->calc ( );

      ASCString result;
      result.format( "%s; eff strength: %d; damage inflicted to enemy: %d, making a total of ~%d~; own damage will be +%d = %d", cwaffentypen[log2(p.first->typ[p.second])], battle->av.strength, battle->dv.damage-dd, battle->dv.damage, battle->av.damage-ad, battle->av.damage );
      return result;
}


void AttackGui::eval()
{
   MapCoordinate mc = actmap->player[actmap->actplayer].cursorPos;
   
   if ( !mc.valid() )
      return;
     
   if ( mc.x >= actmap->xsize || mc.y >= actmap->ysize )
      return;

   int num = 0;
   while ( getEntry( mc, num).first ) {
       GuiButton* b = host->getButton(num);
       b->registerFunc( this, mc, num );
       b->Show();
       ++num;
   }
   
   GuiButton* b = host->getButton(num);
   b->registerFunc( this, mc, -1 );
   b->Show();
   ++num;

   host->disableButtons(num);
}



AttackGui attackGui;









class Movement : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, int num );
      void execute( const MapCoordinate& pos, int num );
      Surface& getImage( const MapCoordinate& pos, int num )
      {
         return IconRepository::getIcon("movement.png");
      };
      ASCString getName( const MapCoordinate& pos, int num )
      {
         return "move unit";
      };
};

bool Movement::available( const MapCoordinate& pos, int num )
{
   if (moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing ) {
      Vehicle* eht = actmap->getField(pos)->vehicle;
      if ( eht )
         if ( eht->color == actmap->actplayer * 8)
            return VehicleMovement::avail ( eht );
   } else
      if ( pendingVehicleActions.actionType == vat_move ) {
         switch ( pendingVehicleActions.move->getStatus() ) {
            case 2:
               return pendingVehicleActions.move->reachableFields.isMember ( pos.x, pos.y );
            case 3:
               return pendingVehicleActions.move->path.rbegin()->x == pos.x && pendingVehicleActions.move->path.rbegin()->y == pos.y;
         } /* endswitch */
      }

   return false;
}

void Movement::execute( const MapCoordinate& pos, int num )
{
   if ( moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing ) {
      new VehicleMovement ( &getDefaultMapDisplay(), &pendingVehicleActions );

      int mode = 0;
      if (  skeypress( ct_lshift ) ||  skeypress ( ct_rshift ))
         mode |= VehicleMovement::DisableHeightChange;

      int res = pendingVehicleActions.move->execute ( actmap->getField(pos)->vehicle, -1, -1, 0, -1, mode );
      if ( res < 0 ) {
         dispmessage2 ( -res, NULL );
         delete pendingVehicleActions.action;
         return;
      }
      for ( int i = 0; i < pendingVehicleActions.move->reachableFields.getFieldNum(); i++ )
         pendingVehicleActions.move->reachableFields.getField( i ) ->a.temp = 1;

      // if ( !CGameOptions::Instance()->dontMarkFieldsNotAccessible_movement )
      for ( int j = 0; j < pendingVehicleActions.move->reachableFieldsIndirect.getFieldNum(); j++ )
         pendingVehicleActions.move->reachableFieldsIndirect.getField( j ) ->a.temp2 = 2;
      displaymap();

   } else {
      if ( !pendingVehicleActions.move )
         return;

      int ms = pendingVehicleActions.move->getStatus();
      if ( moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_move &&  (ms == 2 || ms == 3 )) {
         int res;
         res = pendingVehicleActions.move->execute ( NULL, pos.x, pos.y, pendingVehicleActions.move->getStatus(), -1, 0 );
         if ( res >= 0 && CGameOptions::Instance()->fastmove && ms == 2 ) {
            actmap->cleartemps(7);
            displaymap();
            res = pendingVehicleActions.move->execute ( NULL, pos.x, pos.y, pendingVehicleActions.move->getStatus(), -1, 0 );
         } else {
            if ( ms == 2 ) {
               actmap->cleartemps(7);
               for ( int i = 0; i < pendingVehicleActions.move->path.size(); i++ )
                  actmap->getField( pendingVehicleActions.move->path[i]) ->a.temp = 1;
               displaymap();
            } else {
               actmap->cleartemps(7);
               displaymap();
            }
         }


         if ( res < 0 ) {
            dispmessage2 ( -res, NULL );
            delete pendingVehicleActions.action;
            return;
         }

         if ( pendingVehicleActions.move->getStatus() == 1000 ) {
            delete pendingVehicleActions.move;
/*
            if ( CGameOptions::Instance()->smallguiiconopenaftermove ) {
               actgui->painticons();
               actgui->paintsmallicons ( CGameOptions::Instance()->mouse.smallguibutton, 0 );
            }
            */
         }
         updateFieldInfo();

      }
   }
}




class EndTurn : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, int num ) ;
      void execute( const MapCoordinate& pos, int num );
      Surface& getImage( const MapCoordinate& po, int nums )
      {
         return IconRepository::getIcon("endturn.png");
      };
      ASCString getName( const MapCoordinate& pos, int num )
      {
         return "end turn";
      };
};

bool EndTurn::available( const MapCoordinate& pos, int num )
{
   if (moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing)
      if (actmap->levelfinished == false)
         return true;
   return false;
}


void EndTurn::execute( const MapCoordinate& pos, int num )
{
   if ( !CGameOptions::Instance()->endturnquestion || (choice_dlg("do you really want to end your turn ?","~y~es","~n~o") == 1)) {

      static int autosave = 0;
      ASCString name = ASCString("autosave") + strrr( autosave ) + &savegameextension[1];

      savegame ( name );

      autosave = !autosave;
      next_turn();

      displaymap();
   }
}


class Attack : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, int num ) ;
      void execute( const MapCoordinate& pos, int num );
      Surface& getImage( const MapCoordinate& pos, int num )
      {
         return IconRepository::getIcon("attack.png");
      };
      ASCString getName( const MapCoordinate& pos, int num )
      {
         return "attack";
      };
};


bool Attack::available( const MapCoordinate& pos, int num )
{
   if (moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing ) {
      Vehicle* eht = actmap->getField(pos)->vehicle;
      if ( eht )
         if ( eht->color == actmap->actplayer * 8)
            return VehicleAttack::avail ( eht );
   }
   return false;
}

void Attack::execute(  const MapCoordinate& pos, int num )
{
   if ( moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing ) {
      new VehicleAttack ( &getDefaultMapDisplay(), &pendingVehicleActions );

      int res;
      res = pendingVehicleActions.attack->execute ( actmap->getField(pos)->vehicle, -1, -1, 0, 0, -1 );
      if ( res < 0 ) {
         dispmessage2 ( -res, NULL );
         delete pendingVehicleActions.action;
         return;
      }

      int i;
      for ( i = 0; i < pendingVehicleActions.attack->attackableVehicles.getFieldNum(); i++ )
         pendingVehicleActions.attack->attackableVehicles.getField( i ) ->a.temp = 1;
      for ( i = 0; i < pendingVehicleActions.attack->attackableBuildings.getFieldNum(); i++ )
         pendingVehicleActions.attack->attackableBuildings.getField( i ) ->a.temp = 1;
      for ( i = 0; i < pendingVehicleActions.attack->attackableObjects.getFieldNum(); i++ )
         pendingVehicleActions.attack->attackableObjects.getField( i ) ->a.temp = 1;

      displaymap();
      attackGui.setupWeapons( pendingVehicleActions.attack );
      NewGuiHost::pushIconHandler( &attackGui );
   }
}





class PowerOn : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, int num )
      {
         if (moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing )  {
            pfield fld = actmap->getField ( pos );
            if ( fld->vehicle )
               if ( fld->vehicle->color == actmap->actplayer*8  &&
                     ( fld->vehicle->typ->functions & cfgenerator))
                  if ( !fld->vehicle->getGeneratorStatus() )
                     return true;

         }
         return false;
      };
      
      void execute( const MapCoordinate& pos, int num )
      {
         Vehicle* veh = actmap->getField(pos)->vehicle;
         veh->setGeneratorStatus ( true );
         logtoreplayinfo ( rpl_setGeneratorStatus, veh->networkid, int(1) );
         updateFieldInfo();
      }
      
      Surface& getImage( const MapCoordinate& pos, int num )
      {
         return IconRepository::getIcon("poweron.png");
      };
      
      ASCString getName( const MapCoordinate& pos, int num )
      {
         return "enable power generation";
      };
};


class PowerOff : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, int num )
      {
         if (moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing )  {
            pfield fld = actmap->getField ( pos );
            if ( fld->vehicle )
               if ( fld->vehicle->color == actmap->actplayer*8  &&
                     ( fld->vehicle->typ->functions & cfgenerator))
                  if ( fld->vehicle->getGeneratorStatus() )
                     return true;

         }
         return false;
      };
      
      void execute( const MapCoordinate& pos, int num )
      {
         Vehicle* veh = actmap->getField(pos)->vehicle;
         veh->setGeneratorStatus ( false );
         logtoreplayinfo ( rpl_setGeneratorStatus, veh->networkid, int(0) );
         updateFieldInfo();
      }
      
      Surface& getImage( const MapCoordinate& pos, int num )
      {
         return IconRepository::getIcon("poweron.png");
      };
      
      ASCString getName( const MapCoordinate& pos, int num )
      {
         return "enable power generation";
      };
};








GuiIconHandler primaryGuiIcons;



} // namespace GuiFunctions


void registerGuiFunctions( GuiIconHandler& handler )
{
   handler.registerUserFunction( new GuiFunctions::Movement() );
   handler.registerUserFunction( new GuiFunctions::Attack() );
   handler.registerUserFunction( new GuiFunctions::PowerOn() );
   handler.registerUserFunction( new GuiFunctions::PowerOff() );
   handler.registerUserFunction( new GuiFunctions::EndTurn() );
}
