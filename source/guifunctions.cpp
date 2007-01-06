/*! \file guifunctions.cpp
    \brief All the buttons of the user interface with which the unit actions are controlled.
*/

/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2007  Martin Bickel  and  Marc Schellenberger

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
#include "guifunctions-interface.h"
#include "unitctrl.h"
#include "controls.h"
#include "dialog.h"
#include "gameoptions.h"
#include "iconrepository.h"
#include "replay.h"
#include "attack.h"
#include "graphics/blitter.h"
#include "viewcalculation.h"
#include "spfst.h"
// #include "gamedlg.h"
#include "dialogs/cargodialog.h"
#include "dialogs/ammotransferdialog.h"
#include "mapdisplay.h"
#include "sg.h"
#include "loaders.h"
#include "itemrepository.h"
#include "turncontrol.h"
#include "dialogs/buildingtypeselector.h"

#include "actions/jumpdrive.h"


namespace GuiFunctions
{



class AttackGui : public GuiIconHandler, public GuiFunction {
     VehicleAttack* attackEngine;

     pair<pattackweap, int> getEntry( const MapCoordinate& pos, int num );

   protected:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num );
      void execute( const MapCoordinate& pos, ContainerBase* subject, int num );
      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num );
      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num );
      bool checkForKey( const SDL_KeyboardEvent* key, int modifier );

   public:
      AttackGui() : attackEngine( NULL ) {};
      void setupWeapons( VehicleAttack* va ) { attackEngine = va; };
      void eval( const MapCoordinate& mc, ContainerBase* subject );

};

bool AttackGui :: checkForKey( const SDL_KeyboardEvent* key, int modifier )
{
   if ( key->keysym.sym == SDLK_ESCAPE || key->keysym.unicode == 'c' ) {
      // execute( actmap->getCursor(), actmap->getField( actmap->getCursor())->getContainer() , -1 );
      return true;
   }
   return false;
}


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


bool AttackGui::available( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   if ( num == -1 )
      return true;
      
   if ( getEntry(pos,num).first )
      return true;
   else
      return false;
}

void AttackGui::execute( const MapCoordinate& pos, ContainerBase* subject, int num )
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

Surface& AttackGui::getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   if ( num == -1 )
      return IconRepository::getIcon("cancel.png");
      
   pair<pattackweap, int> p = getEntry(pos,num);
   switch ( p.first->typ[p.second] ) {
      case cwcruisemissileb: return IconRepository::getIcon("weap-cruisemissile.png");
      case cwbombb: return IconRepository::getIcon("weap-bomb.png");
      case cwlargemissileb: return IconRepository::getIcon("weap-bigmissile.png");
      case cwsmallmissileb: return IconRepository::getIcon("weap-smallmissile.png");
      case cwtorpedob: return IconRepository::getIcon("weap-torpedo.png");
      case cwmachinegunb: return IconRepository::getIcon("weap-machinegun.png");
      case cwcannonb: return IconRepository::getIcon("weap-cannon.png");
      default: return IconRepository::getIcon("weap-laser.png");
   };
}

ASCString AttackGui::getName( const MapCoordinate& pos, ContainerBase* subject, int num )
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


void AttackGui::eval( const MapCoordinate& mc, ContainerBase* subject )
{
   
   int num = 0;
   while ( getEntry( mc, num).first ) {
       GuiButton* b = host->getButton(num);
       b->registerFunc( this, mc, subject, num );
       b->Show();
       ++num;
   }

   GuiButton* b = host->getButton(num);
   b->registerFunc( this, mc, subject, -1 );
   b->Show();
   ++num;

   host->disableButtons(num);
}



AttackGui attackGui;



bool Cancel::available( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   return moveparams.movestatus || pendingVehicleActions.action;
};

void Cancel::execute( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   if ( moveparams.movestatus || pendingVehicleActions.action ) {
      moveparams.movestatus = 0;
      if ( pendingVehicleActions.action )
         delete pendingVehicleActions.action;

      actmap->cleartemps(7);
      updateFieldInfo();
      displaymap();
   }
};

bool Cancel::checkForKey( const SDL_KeyboardEvent* key, int modifier )
{
   return ( key->keysym.sym == SDLK_ESCAPE || key->keysym.unicode == 'c' );
};


Surface& Cancel::getImage( const MapCoordinate& po, ContainerBase* subject, int nums )
{
   return IconRepository::getIcon("cancel.png");
};

ASCString Cancel::getName( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   return "~c~ancel";
};



bool Movement::checkForKey( const SDL_KeyboardEvent* key, int modifier )
{
   return ( key->keysym.sym == SDLK_SPACE );
};

Surface& Movement::getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   return IconRepository::getIcon("movement.png");
};

ASCString Movement::getName( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   return "move unit (~space~)";
};


bool Movement::available( const MapCoordinate& pos, ContainerBase* subject, int num )
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

void Movement::execute( const MapCoordinate& pos, ContainerBase* subject, int num )
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
      updateFieldInfo();
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
            MapDisplayPG::CursorHiding ch;
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








class Ascend : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num );
      void execute( const MapCoordinate& pos, ContainerBase* subject, int num );
      bool checkForKey( const SDL_KeyboardEvent* key, int modifier )
      {
         return ( key->keysym.unicode == 's' );
      };
      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return IconRepository::getIcon("ascend-airplane.png");
      };
      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "a~s~cend";
      };
};

bool Ascend::available( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   if ( moveparams.movestatus == 0 && !pendingVehicleActions.action ) {
      Vehicle* eht = actmap->getField(pos)->vehicle;
      if ( !eht )
         return false;
      if (eht->color == actmap->actplayer * 8)
         return IncreaseVehicleHeight::avail ( eht );
   } else
      if ( pendingVehicleActions.actionType == vat_ascent ) {
         switch ( pendingVehicleActions.ascent->getStatus() ) {
           case 2: return pendingVehicleActions.ascent->reachableFields.isMember ( pos.x, pos.y );
           case 3: return pendingVehicleActions.ascent->path.rbegin()->x == pos.x && pendingVehicleActions.ascent->path.rbegin()->y == pos.y;
         } /* endswitch */
      }
   return false;
}

void Ascend::execute( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   if ( moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing ) {
      new IncreaseVehicleHeight ( &getDefaultMapDisplay(), &pendingVehicleActions );

      bool simpleMode = false;
      if (  skeypress( ct_lshift ) ||  skeypress ( ct_rshift ))
         simpleMode = true;

      int res = pendingVehicleActions.ascent->execute ( actmap->getField(pos)->vehicle, -1, -1, 0, actmap->getField(pos)->vehicle->height << 1, simpleMode );
      if ( res < 0 ) {
         dispmessage2 ( -res, NULL );
         delete pendingVehicleActions.action;
         return;
      }

      if ( res == 1000 )
         delete pendingVehicleActions.action;
      else {
         for ( int i = 0; i < pendingVehicleActions.ascent->reachableFields.getFieldNum(); i++ )
            pendingVehicleActions.ascent->reachableFields.getField( i ) ->a.temp = 1;
         displaymap();
      }

   } else
     if ( moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_ascent &&  (pendingVehicleActions.ascent->getStatus() == 2 || pendingVehicleActions.ascent->getStatus() == 3 )) {
        int xdst = pos.x;
        int ydst = pos.y;
        int res = pendingVehicleActions.ascent->execute ( NULL, xdst, ydst, pendingVehicleActions.ascent->getStatus(), -1, 0 );
        if ( res >= 0 && CGameOptions::Instance()->fastmove ) {
           actmap->cleartemps(7);
           displaymap();
           // if the status is 1000 at this position, the unit has been shot down by reactionfire before initiating the height change
           if ( res < 1000 )
              res = pendingVehicleActions.ascent->execute ( NULL, xdst, ydst, pendingVehicleActions.ascent->getStatus(), -1, 0 );
        } else {
           actmap->cleartemps(7);
           if ( res < 1000 )
              for ( int i = 0; i < pendingVehicleActions.ascent->path.size(); i++ )
                 actmap->getField( pendingVehicleActions.ascent->path[i]) ->a.temp = 1;
           displaymap();
        }

        if ( res < 0 ) {
           dispmessage2 ( -res, NULL );
           delete pendingVehicleActions.action;
           updateFieldInfo();
           return;
        }

        if ( pendingVehicleActions.ascent->getStatus() == 1000 ) {
           delete pendingVehicleActions.ascent;

        }
     }

   updateFieldInfo();
}





class Descend : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num );
      void execute( const MapCoordinate& pos, ContainerBase* subject, int num );
      bool checkForKey( const SDL_KeyboardEvent* key, int modifier )
      {
         return ( key->keysym.unicode == 'd' );
      };
      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return IconRepository::getIcon("descent-airplane.png");
      };
      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "~d~escend";
      };
};

bool Descend::available( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   if ( moveparams.movestatus == 0 && !pendingVehicleActions.action ) {
      Vehicle* eht = actmap->getField(pos)->vehicle;
      if ( !eht )
         return false;

      if (eht->color == actmap->actplayer * 8)
         return DecreaseVehicleHeight::avail ( eht );
   } else
      if ( pendingVehicleActions.actionType == vat_descent ) {
         switch ( pendingVehicleActions.descent->getStatus() ) {
           case 2: return pendingVehicleActions.descent->reachableFields.isMember ( pos.x, pos.y );
           case 3: return pendingVehicleActions.descent->path.rbegin()->x == pos.x && pendingVehicleActions.descent->path.rbegin()->y == pos.y;
         } /* endswitch */
      }
   return false;
}

void Descend::execute( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   if ( moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing ) {
      new DecreaseVehicleHeight ( &getDefaultMapDisplay(), &pendingVehicleActions );


      bool simpleMode = false;
      if (  skeypress( ct_lshift ) ||  skeypress ( ct_rshift ))
         simpleMode = true;

      int res = pendingVehicleActions.descent->execute ( actmap->getField(pos)->vehicle, -1, -1, 0, actmap->getField(pos)->vehicle->height >> 1, simpleMode );
      if ( res < 0 ) {
         dispmessage2 ( -res, NULL );
         delete pendingVehicleActions.action;
         return;
      }

      if ( res == 1000 )
         delete pendingVehicleActions.action;
      else {
         for ( int i = 0; i < pendingVehicleActions.descent->reachableFields.getFieldNum(); i++ )
            pendingVehicleActions.descent->reachableFields.getField( i ) ->a.temp = 1;
         displaymap();
      }

   } else
     if ( moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_descent &&  (pendingVehicleActions.descent->getStatus() == 2 || pendingVehicleActions.descent->getStatus() == 3 )) {
        int res = pendingVehicleActions.descent->execute ( NULL, pos.x, pos.y, pendingVehicleActions.descent->getStatus(), -1, 0 );
        if ( res >= 0 && CGameOptions::Instance()->fastmove ) {
           actmap->cleartemps(7);
           displaymap();
           // if the status is 1000 at this position, the unit has been shot down by reactionfire before initiating the height change
           if ( res < 1000 )
              res = pendingVehicleActions.descent->execute ( NULL, pos.x, pos.y, pendingVehicleActions.descent->getStatus(), -1, 0 );
        } else {
           actmap->cleartemps(7);
           if ( res < 1000 )
              for ( int i = 0; i < pendingVehicleActions.descent->path.size(); i++ )
                 actmap->getField( pendingVehicleActions.descent->path[i]) ->a.temp = 1;
           displaymap();
        }


        if ( res < 0 ) {
           dispmessage2 ( -res, NULL );
           delete pendingVehicleActions.action;
           updateFieldInfo();
           return;
        }

        if ( pendingVehicleActions.descent->getStatus() == 1000 ) {
           delete pendingVehicleActions.descent;

/*           if ( CGameOptions::Instance()->smallguiiconopenaftermove ) {
              actgui->painticons();
              actgui->paintsmallicons ( CGameOptions::Instance()->mouse.smallguibutton, 0 );
           }
           */
        }
     }
   updateFieldInfo();
}









class EndTurn : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num ) ;
      void execute( const MapCoordinate& pos, ContainerBase* subject, int num );
      bool checkForKey( const SDL_KeyboardEvent* key, int modifier )
      {
         return ( key->keysym.unicode == 'e' );
      };
      Surface& getImage( const MapCoordinate& po, ContainerBase* subject, int nums )
      {
         return IconRepository::getIcon("endturn.png");
      };
      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "~e~nd turn";
      };
};

bool EndTurn::available( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   if (moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing)
      if (actmap->levelfinished == false)
         return true;
   return false;
}


void EndTurn::execute( const MapCoordinate& pos, ContainerBase* subject, int num )
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
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num ) ;
      void execute( const MapCoordinate& pos, ContainerBase* subject, int num );
      bool checkForKey( const SDL_KeyboardEvent* key, int modifier )
      {
         return ( key->keysym.unicode == 'a' );
      };
      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return IconRepository::getIcon("attack.png");
      };
      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "~a~ttack";
      };
};


bool Attack::available( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   if (moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing ) {
      Vehicle* eht = actmap->getField(pos)->vehicle;
      if ( eht )
         if ( eht->color == actmap->actplayer * 8)
            return VehicleAttack::avail ( eht );
   }
   return false;
}

void Attack::execute(  const MapCoordinate& pos, ContainerBase* subject, int num )
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
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         if (moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing )  {
            tfield* fld = actmap->getField ( pos );
            if ( fld->vehicle )
               if ( fld->vehicle->color == actmap->actplayer*8  &&
                     ( fld->vehicle->typ->hasFunction( ContainerBaseType::MatterConverter  )))
                  if ( !fld->vehicle->getGeneratorStatus() )
                     return true;

         }
         return false;
      };

      bool checkForKey( const SDL_KeyboardEvent* key, int modifier )
      {
         return ( key->keysym.unicode == 'p' );
      };
      void execute( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         Vehicle* veh = actmap->getField(pos)->vehicle;
         veh->setGeneratorStatus ( true );
         logtoreplayinfo ( rpl_setGeneratorStatus, veh->networkid, int(1) );
         updateFieldInfo();
      }

      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return IconRepository::getIcon("poweron.png");
      };

      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "enable ~p~ower generation";
      };
};


class PowerOff : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         if (moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing )  {
            tfield* fld = actmap->getField ( pos );
            if ( fld->vehicle )
               if ( fld->vehicle->color == actmap->actplayer*8  &&
                    ( fld->vehicle->typ->hasFunction( ContainerBaseType::MatterConverter)))
                  if ( fld->vehicle->getGeneratorStatus() )
                     return true;

         }
         return false;
      };
      
      bool checkForKey( const SDL_KeyboardEvent* key, int modifier )
      {
         return ( key->keysym.unicode == 'p' );
      };
      void execute( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         Vehicle* veh = actmap->getField(pos)->vehicle;
         veh->setGeneratorStatus ( false );
         logtoreplayinfo ( rpl_setGeneratorStatus, veh->networkid, int(0) );
         updateFieldInfo();
      }

      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return IconRepository::getIcon("poweroff.png");
      };

      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "disable ~p~ower generation";
      };
};



class UnitInfo : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num ) ;
      void execute( const MapCoordinate& pos, ContainerBase* subject, int num ) { execUserAction_ev(ua_vehicleinfo);};
      Surface& getImage( const MapCoordinate& po, ContainerBase* subject, int nums )
      {
         return IconRepository::getIcon("unitinfo.png");
      };
      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "unit ~i~nfo";
      };
      bool checkForKey( const SDL_KeyboardEvent* key, int modifier )
      {
         return ( key->keysym.unicode == 'i' );
      };
};

bool UnitInfo::available( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   tfield* fld = actmap->getField(pos);
   if ( fld && fld->vehicle )
      if (moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing)
         if ( fld->vehicle != NULL)
            if (fieldvisiblenow( fld ))
               return true;
   return false;
}






class DestructBuilding : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num ) ;
      void execute( const MapCoordinate& pos, ContainerBase* subject, int num );
      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return IconRepository::getIcon("destructbuilding.png");
      };
      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "destruct building";
      };
};


bool DestructBuilding::available( const MapCoordinate& pos, ContainerBase* subject, int num )
{
    tfield* fld = actmap->getField(pos);
    if (moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing) {
       if ( fld->vehicle )
          if ( fld->vehicle->attacked == false && !fld->vehicle->hasMoved() )
             if (fld->vehicle->color == actmap->actplayer * 8)
                if ( fld->vehicle->typ->hasFunction( ContainerBaseType::ConstructBuildings  ) || !fld->vehicle->typ->buildingsBuildable.empty() )
                  if ( fld->vehicle->getTank().fuel >= destruct_building_fuel_usage * fld->vehicle->typ->fuelConsumption )
                     return true;
    }
    else
       if (moveparams.movestatus == 115) {
          if (fld->a.temp == 20)
             return true;
       }

    return false;
}

void DestructBuilding::execute(  const MapCoordinate& pos, ContainerBase* subject, int num )
{
   if (moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing) {
      destructbuildinglevel1( pos.x, pos.y );
      displaymap();
   }
   else
      if (moveparams.movestatus == 115) {
         destructbuildinglevel2( pos.x, pos.y );
         updateFieldInfo();
         displaymap();
      }
}


/*
class SearchForMineralResources : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         tfield* fld = actmap->getField(pos);
         if (fld->vehicle != NULL)
            if (fld->vehicle->color == actmap->actplayer * 8)
               if ( (fld->vehicle->typ->functions &  cfmanualdigger) && !(fld->vehicle->typ->functions &  cfautodigger) )
                  if (moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing)
                     if ( actmap->_resourcemode == 0 ) 
                        return fld->vehicle->searchForMineralResourcesAvailable();
         return false;
      };

      void execute( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
          actmap->getField(pos)->vehicle->searchForMineralResources( ) ;

          MapDisplayPG* mapDisplay = dynamic_cast<MapDisplayPG*>( ASC_PG_App::GetWidgetById( ASC_PG_App::mapDisplayID ));
          if ( mapDisplay )
             mapDisplay->activateMapLayer("resources", true);
          
          updateFieldInfo();
          repaintMap();
      }

      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return IconRepository::getIcon("dig.png");
      };

      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "search for mineral resources";
      };
};
*/

class OpenContainer : public GuiFunction
{
     static int containeractive;
   public:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
        tfield* fld = actmap->getField(pos);
        if ( fieldvisiblenow ( fld ) && fld->getContainer() ) {
           if ( !containeractive && !moveparams.movestatus && pendingVehicleActions.actionType == vat_nothing && !pendingVehicleActions.action ) {
              Player& player = fld->getContainer()->getMap()->player[fld->getContainer()->getOwner()];
              if ( fld->building && ( player.diplomacy.isAllied( actmap->actplayer) || actmap->getNeutralPlayerNum() == fld->building->getOwner() )) {
                 if ( fld->building->getCompletion() == fld->building->typ->construction_steps-1 )
                    return true;
              }  else {
                  if ( fld->vehicle && fld->vehicle->typ->maxLoadableUnits  &&  player.diplomacy.isAllied( actmap->actplayer)  )
                    return true;
              }
           }
        }
        return false;
      };

      void execute( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
          tfield* fld = actmap->getField(pos);
          
          cargoDialog( fld->getContainer() );
          
          updateFieldInfo();
          repaintMap();
      }

      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return IconRepository::getIcon("container.png");
      };

      bool checkForKey( const SDL_KeyboardEvent* key, int modifier )
      {
         return ( key->keysym.unicode == 'l' );
      };
      
      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "view ~l~oaded units";
      };
};

int OpenContainer::containeractive = 0;




class EnableReactionfire : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         Vehicle* eht = actmap->getField(pos)->vehicle;
         if ( eht )
            if ( eht->color == actmap->actplayer * 8)
               if ( eht->reactionfire.getStatus() == Vehicle::ReactionFire::off )
                  if ( moveparams.movestatus == 0  && pendingVehicleActions.actionType == vat_nothing)
                     if ( eht->weapexist() )
                        for ( int i = 0; i < eht->typ->weapons.count; ++i )
                           if ( eht->typ->weapons.weapon[i].offensive() && eht->typ->weapons.weapon[i].reactionFireShots )
                              return true;

         return false;
      };
      bool checkForKey( const SDL_KeyboardEvent* key, int modifier )
      {
         return ( key->keysym.unicode == 'x' );
      };

      void execute( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         int res = actmap->getField(pos)->vehicle->reactionfire.enable();
         if ( res < 0 )
            dispmessage2 ( -res, NULL );
         updateFieldInfo();
      }

      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return IconRepository::getIcon("enable-reactionfire.png");
      };

      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "enable reaction fire (~x~)";
      };
};

class DisableReactionfire : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         Vehicle* eht = actmap->getField(pos)->vehicle;
         if ( eht )
            if ( eht->color == actmap->actplayer * 8)
               if ( eht->reactionfire.getStatus() != Vehicle::ReactionFire::off )
                  if ( moveparams.movestatus == 0  && pendingVehicleActions.actionType == vat_nothing)
                     // for ( int i = 0; i < eht->typ->weapons.count; ++i )
                        // if ( eht->typ->weapons.weapon[i].offensive() && eht->typ->weapons.weapon[i].reactionFireShots )
                           return true;

         return false;
      };

      bool checkForKey( const SDL_KeyboardEvent* key, int modifier )
      {
         return ( key->keysym.unicode == 'x' );
      };
      void execute( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         actmap->getField(pos)->vehicle->reactionfire.disable();
         updateFieldInfo();
      }

      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return IconRepository::getIcon("disable-reactionfire.png");
      };

      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "disable reaction fire (~x~)";
      };
};


class JumpDriveIcon : public GuiFunction, public SigC::Object
{
   void fieldMarker( GameMap* gamemap, const MapCoordinate& pos )
   {
      gamemap->getField(pos)->a.temp = 1;
   };
   
   public:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         JumpDrive jd;
         if ( moveparams.movestatus == 0  && pendingVehicleActions.actionType == vat_nothing) {
         
            Vehicle* eht = actmap->getField(pos)->vehicle;
            if ( eht )
               if ( eht->getOwner() == actmap->actplayer )
                  return jd.available(eht);
         } else {
            if ( moveparams.movestatus == 140 ) {
               if ( jd.available(moveparams.vehicletomove) )
                  if ( jd.fieldReachable( moveparams.vehicletomove, pos ))
                     return true;  
            }
         }

         return false;
      };

      bool checkForKey( const SDL_KeyboardEvent* key, int modifier )
      {
         return ( key->keysym.unicode == 'j' );
      };

      void execute( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         if ( moveparams.movestatus == 0 ) {
            Vehicle* eht = actmap->getField(pos)->vehicle;
            if ( !eht )
               return;
            
            JumpDrive jd;
            jd.fieldAvailable.connect (SigC::slot( *this, &JumpDriveIcon::fieldMarker ));
            if ( jd.getFields( eht )) {
               repaintMap();
               moveparams.movestatus = 140;
               moveparams.vehicletomove = eht;
            }
         } else 
            if ( moveparams.movestatus == 140 ) {
               JumpDrive jd;
               jd.jump ( moveparams.vehicletomove, pos );
               actmap->cleartemps(7);
               repaintMap();
               mapChanged( actmap );
               moveparams.movestatus = 0;
               moveparams.vehicletomove = NULL;
            }
      }

      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return IconRepository::getIcon("jumpdrive.png");
      };

      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "activate jump drive (~j~)";
      };
};



/*
class ExternalLoading : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         if (moveparams.movestatus == 130)
            if ( actmap->getField(pos)->a.temp == 123 )
               return true;
         return false;
      };

      void execute( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         moveparams.movestatus++;
      }

      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return IconRepository::getIcon("disable-reactionfire.png");
      };

      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "disable reaction fire";
      };
};
*/



class RepairUnit : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         tfield* fld = actmap->getField(pos);
         if (moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing) {
            if ( fld->vehicle )
               if (fld->vehicle->color == actmap->actplayer * 8)
                  if ( VehicleService::avail ( fld->vehicle ))
                     if ( VehicleService::getServices( fld->vehicle) & (1 << VehicleService::srv_repair ))
                        return true;
         } else
            if ( pendingVehicleActions.actionType == vat_service && pendingVehicleActions.service->guimode == 1 ) {
               if ( fld->vehicle ) {
                  // if ( pendingVehicleActions.service->getServices ( fld->vehicle) & ( 1 << VehicleService::srv_repair) ) {
                  VehicleService::TargetContainer::iterator i = pendingVehicleActions.service->dest.find(fld->vehicle->networkid);
                  if ( i != pendingVehicleActions.service->dest.end() )
                     for ( int j = 0; j < i->second.service.size(); j++ )
                        if ( i->second.service[j].type == VehicleService::srv_repair )
                           return true;
               }
            }

         return false;
      };

      bool checkForKey( const SDL_KeyboardEvent* key, int modifier )
      {
         return ( key->keysym.unicode == 'r' );
      };
      void execute( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         if ( pendingVehicleActions.actionType == vat_nothing ) {
            VehicleService* vs = new VehicleService ( &getDefaultMapDisplay(), &pendingVehicleActions );
            vs->guimode = 1;
            int res = vs->execute ( actmap->getField(pos)->vehicle, -1, -1, 0, -1, -1 );
            if ( res < 0 ) {
               dispmessage2 ( -res );
               delete vs;
               return;
            }
            int fieldCount = 0;
            for ( VehicleService::TargetContainer::iterator i = pendingVehicleActions.service->dest.begin(); i != pendingVehicleActions.service->dest.end(); i++ ) {
               tfield* fld = getfield ( i->second.dest->xpos, i->second.dest->ypos );
               if ( fld != actmap->getField(pos) )
                  for ( int j = 0; j < i->second.service.size(); j++ )
                     if ( i->second.service[j].type == VehicleService::srv_repair ) {
                        fieldCount++;
                        fld->a.temp = 1;
                     }
            }
            if ( !fieldCount ) {
               delete vs;
               dispmessage2 ( 211 );
            } else
               repaintMap();
         } else {
            for ( VehicleService::TargetContainer::iterator i = pendingVehicleActions.service->dest.begin(); i != pendingVehicleActions.service->dest.end(); i++ ) {
               tfield* fld = actmap->getField(pos);
               if ( i->second.dest == fld->vehicle )
                  // for ( vector<VehicleService::Service>::iterator j = i->second->service.begin(); j != i->second->service.end(); j++ )
                  for ( int j = 0; j < i->second.service.size(); j++ )
                     if ( i->second.service[j].type == VehicleService::srv_repair )
                        pendingVehicleActions.service->execute ( NULL, fld->vehicle->networkid, -1, 2, j, i->second.service[j].minAmount );
            }

            delete pendingVehicleActions.service;
            actmap->cleartemps(7);
            repaintMap();
            updateFieldInfo();
         }
      }

      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return IconRepository::getIcon("repair.png");
      };

      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "~r~epair a unit";
      };
};


class RefuelUnit : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         tfield* fld = actmap->getField(pos);
         if (moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing) {
            if ( fld && fld->getContainer() )
               if (fld->getContainer()->getOwner() == actmap->actplayer )
                  if ( NewVehicleService::avail ( fld->getContainer() ) )
                     return true;
         } else
            if ( pendingVehicleActions.actionType == vat_newservice ) { // && pendingVehicleActions.service->guimode == 2
               if ( fld && fld->getContainer() ) {
                  if ( pendingVehicleActions.newservice->targetAvail( fld->getContainer() ))
                     return true;
               }
            }

         return false;
      };
      bool checkForKey( const SDL_KeyboardEvent* key, int modifier )
      {
         return ( key->keysym.unicode == 'f' );
      };

      void execute( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         if ( pendingVehicleActions.actionType == vat_nothing ) {
            NewVehicleService* vs = new NewVehicleService ( &getDefaultMapDisplay(), &pendingVehicleActions );
            // pendingVehicleActions.service->guimode = 2;
            int res = vs->executeContainer ( actmap->getField(pos)->getContainer(), pos.x, pos.y, 0, -1, -1 );
            if ( res < 0 ) {
               dispmessage2 ( -res );
               delete vs;
               return;
            }
            int fieldCount = 0;
            MapCoordinate srcPos = pendingVehicleActions.newservice->getContainer()->getPosition();
            for ( NewVehicleService::Targets::iterator i = pendingVehicleActions.newservice->targets.begin(); i != pendingVehicleActions.newservice->targets.end(); i++ ) {
               MapCoordinate targetPos = (*i)->getPosition();
               if ( targetPos != srcPos ) {
                  tfield* fld = actmap->getField ( targetPos );
                  fieldCount++;
                  fld->a.temp = 1;
               }
            }
            if ( !fieldCount ) {
               delete vs;
               dispmessage2 ( 211 );
            } else
               displaymap();

            updateFieldInfo();
         } else {
            tfield* fld = actmap->getField( pos );
            if ( fld && fld->getContainer() )
               if ( pendingVehicleActions.newservice && pendingVehicleActions.newservice->targetAvail( fld->getContainer() ) )
                  pendingVehicleActions.newservice->fillEverything( fld->getContainer() );

            delete pendingVehicleActions.newservice;
            actmap->cleartemps(7);
            displaymap();
            updateFieldInfo();
         }
      }

      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return IconRepository::getIcon("refuel.png");
      };

      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "re~f~uel a unit";
      };
};


class RefuelUnitDialog : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         tfield* fld = actmap->getField(pos);
         if ( pendingVehicleActions.newservice && fld->a.temp && fld->getContainer() ) // && pendingVehicleActions.service->guimode == 2
            return true;

         return false;
      };
      bool checkForKey( const SDL_KeyboardEvent* key, int modifier )
      {
         return ( key->keysym.unicode == 'f' && (modifier & KMOD_SHIFT)  );
      };

      void execute( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         ammoTransferWindow( pendingVehicleActions.newservice->getContainer(), actmap->getField(pos)->getContainer() );
         delete pendingVehicleActions.newservice;
         actmap->cleartemps ( 7 );
         displaymap();
         updateFieldInfo();
      }

      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return IconRepository::getIcon("refuel-dialog.png");
      };

      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "re~F~uel dialog ";
      };
};





#if 0
class ViewMap : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         if ( moveparams.movestatus == 0  && pendingVehicleActions.actionType == vat_nothing)
            return true;
         return false;
      };

      void execute( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
          // showmap ();
          displaymap();
      }

      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return IconRepository::getIcon("worldmap.png");
      };

      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "view survey map";
      };
};
#endif


class PutMine : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         tfield* fld = actmap->getField(pos);
         if ( moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing)
            if ( fld->vehicle )
               if (fld->vehicle->color == actmap->actplayer * 8)
                  if (fld->vehicle->typ->hasFunction( ContainerBaseType::PlaceMines ) )
                     if ( !fld->vehicle->attacked )
                        return true;
         return false;
      };
      bool checkForKey( const SDL_KeyboardEvent* key, int modifier )
      {
         return ( key->keysym.unicode == 'm' );
      };

      void execute( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         putMine( pos, 0, 0);
         updateFieldInfo();
         displaymap();
      }

      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return IconRepository::getIcon("putmine.png");
      };

      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "put / remove ~m~ines";
      };
};



class PutGroundMine : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         if (moveparams.movestatus == 90) {
            tfield* fld = actmap->getField(pos);
            if ( (fld->bdt & (getTerrainBitType( cbwater ).flip())).any() )
               if ( fld->a.temp & 1)
                  if ( fld->mines.empty() || fld->mineowner() == actmap->actplayer )
                     return true;
         }
         return false;
      };
};

class PutAntiTankMine : public PutGroundMine
{
   public:
      void execute( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         putMine(pos, cmantitankmine, 1 );
         displaymap();
      }

      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return IconRepository::getIcon("putantitankmine.png");
      };

      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "put anti-tank mine";
      };
};

class PutAntiPersonalMine : public PutGroundMine
{
   public:
      void execute( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         putMine(pos, cmantipersonnelmine, 1);
         displaymap();
      }

      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return IconRepository::getIcon("putantipersonalmine.png");
      };

      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "put anti-personal mine";
      };
};




class PutAntiShipMine : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         if (moveparams.movestatus == 90) {
            tfield* fld = actmap->getField(pos);
            if ( (fld->bdt & getTerrainBitType(cbwater0)).any()
                 || (fld->bdt & getTerrainBitType(cbwater1)).any()
                 || (fld->bdt & getTerrainBitType(cbwater2)).any()
                 || (fld->bdt & getTerrainBitType(cbwater3)).any() )
               if (fld->a.temp & 1 )
                  if ( fld->mines.empty() || fld->mineowner() == actmap->actplayer )
                     return true;
         }
         return false;
      };

      void execute( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         putMine( pos, cmfloatmine, 1);
         displaymap();
      }

      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return IconRepository::getIcon("putantishipmine.png");
      };

      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "put anti-ship mine";
      };
};

class PutAntiSubMine : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         if (moveparams.movestatus == 90) {
            tfield* fld = actmap->getField(pos);
            if ( (fld->typ->art & getTerrainBitType(cbwater2)).any()
                 || (fld->typ->art & getTerrainBitType(cbwater3)).any() )
               if (fld->a.temp & 1 )
                  if ( fld->mines.empty() || fld->mineowner() == actmap->actplayer )
                     return true;
         }
         return false;
      };

      void execute( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         putMine( pos, cmmooredmine,1 );
         displaymap();
      }

      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return IconRepository::getIcon("putantisubmine.png");
      };

      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "put anti-submarine mine";
      };
};


class RemoveMine : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         if (moveparams.movestatus == 90) {
            tfield* fld = actmap->getField(pos);
            if ( fld->a.temp )
               return !fld->mines.empty();
         }
         return false;
      };

      void execute( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         putMine( pos, 0, -1);
         displaymap();
      }

      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return IconRepository::getIcon("removemine.png");
      };

      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "remove mine";
      };
};






class ObjectBuildingGui : public GuiIconHandler, public GuiFunction {
      Vehicle* veh;
      bool buttonDone( std::map<int,bool>& map, int id ) { return map.find( id ) != map.end(); };
   protected:
      enum Mode { Build, Remove };

      bool available( const MapCoordinate& pos, ContainerBase* subject, int num );
      void execute( const MapCoordinate& pos, ContainerBase* subject, int num );
      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num );
      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num );
      bool checkObject( tfield* fld, ObjectType* objtype, Mode mode );

      void search ( const MapCoordinate& pos, int& num, int pass );

      void addButton( int &num, const MapCoordinate& mc, ContainerBase* subject, int id );

   public:
      ObjectBuildingGui() : veh( NULL ) {};
      bool init( Vehicle* vehicle );
      void eval( const MapCoordinate& mc, ContainerBase* subject );

};


bool ObjectBuildingGui::init( Vehicle* vehicle )
{
   veh = vehicle;
   int num = 0;

   for ( int i = 0; i< 6; ++i)
      search ( getNeighbouringFieldCoordinate(veh->getPosition(), i), num, 0 );

   return num > 0;
}


bool ObjectBuildingGui::available( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   return true;
   /*
   if ( num == 0 )
      return true;

checkObject

   if ( getEntry(pos,num).first )
      return true;
   else
      return false;
      */
}

void ObjectBuildingGui::execute( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   if ( num ) {
      tfield* fld = actmap->getField(pos);
      ObjectType* obj = objectTypeRepository.getObject_byID( abs(num) );

      if ( !fld->checkforobject ( obj ) ) {
         assert(num>0);
         fld-> addobject ( obj );
         logtoreplayinfo ( rpl_buildobj2, pos.x, pos.y, obj->id, veh->networkid );
      } else {
         assert(num<0);
         fld->removeobject ( obj );
         logtoreplayinfo ( rpl_remobj2, pos.x, pos.y, obj->id, veh->networkid );
      }

      if ( obj->basicjamming_plus || obj->viewbonus_plus || obj->viewbonus_abs != -1 || obj->basicjamming_abs != -1 )
         computeview( actmap );

      veh->getResource( num > 0 ? obj->buildcost : obj->removecost, false );

      veh->decreaseMovement ( num > 0 ? obj->build_movecost : obj->remove_movecost );
   }
   moveparams.movestatus = 0;
   actmap->cleartemps();
   NewGuiHost::popIconHandler();
   repaintMap();
   updateFieldInfo();
}


Surface buildGuiIcon( const Surface& image, bool remove = false )
{
   const Surface& cancelIcon = IconRepository::getIcon("empty-pressed.png" ); //  "cancel.png"
   Surface s = Surface::createSurface( cancelIcon.w(), cancelIcon.h(), 32, 0 );

   const Surface& o = image;
   if ( o.GetPixelFormat().BitsPerPixel() == 32 ) {
      MegaBlitter<4,4,ColorTransform_None, ColorMerger_AlphaOverwrite, SourcePixelSelector_DirectZoom> blitter;
      blitter.setSize( o.w(), o.h(), s.w(), s.h() );
      blitter.blit( o, s, SPoint(int((s.w() - blitter.getZoomX() * o.w())/2), int((s.h() - blitter.getZoomY() * o.h())/2)));
   } else {
      MegaBlitter<1,4,ColorTransform_None, ColorMerger_AlphaOverwrite, SourcePixelSelector_DirectZoom> blitter;
      blitter.setSize( o.w(), o.h(), s.w(), s.h() );
      blitter.blit( o, s, SPoint(int((s.w() - blitter.getZoomX() * o.w())/2), int((s.h() - blitter.getZoomY() * o.h())/2)));
   }

   if ( remove ) {
      MegaBlitter<4,4,ColorTransform_None, ColorMerger_AlphaOverwrite> blitter;
      Surface& removegui = IconRepository::getIcon( "cancel-addon.png" );
      blitter.blit( removegui, s,  SPoint((s.w() - removegui.w())/2, (s.h() - removegui.h())/2));
   }
   return s;
}

Surface& ObjectBuildingGui::getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   if ( num == 0 )
      return IconRepository::getIcon("cancel.png");

   ObjectType* objtype = objectTypeRepository.getObject_byID( abs(num) );

   if ( !objtype )
      return IconRepository::getIcon("cancel.png");

   static map<int,Surface> removeIconRepository;

   if ( removeIconRepository.find( num ) != removeIconRepository.end() )
      return removeIconRepository[num];

   removeIconRepository[num] = buildGuiIcon( objtype->getPicture(), num < 0 );
   return removeIconRepository[num];
}


ASCString ObjectBuildingGui::getName( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   if ( num == 0 )
      return "cancel (~ESC~)";

   ObjectType* objtype = objectTypeRepository.getObject_byID( abs(num) );
   if ( !objtype )
      return "";

   ASCString result;
   if ( num >= 0 ) {
      result.format( "Build %s (%d Material, %d Fuel, %d Movepoints)", objtype->name.c_str(), objtype->buildcost.material, objtype->buildcost.fuel, objtype->build_movecost );
      // num = -num;
   } else
      result.format( "Remove %s (%d Material, %d Fuel, %d Movepoints)", objtype->name.c_str(), objtype->removecost.material, objtype->removecost.fuel, objtype->remove_movecost );

   return result;
}


bool ObjectBuildingGui::checkObject( tfield* fld, ObjectType* objtype, Mode mode )
{
    if ( !objtype || !fld )
       return false;

    if ( mode == Build ) {
       if ( objtype->getFieldModification(fld->getweather()).terrainaccess.accessible( fld->bdt ) > 0
            &&  !fld->checkforobject ( objtype )
            && objtype->techDependency.available(actmap->player[veh->getOwner()].research) ){
//            && !getheightdelta ( log2( actvehicle->height), log2(objtype->getEffectiveHeight())) ) {

          if ( veh->getResource(objtype->buildcost, true) == objtype->buildcost && veh->getMovement() >= objtype->build_movecost )
             return true;
       }
    } else {
       if ( fld->checkforobject ( objtype ) ) {
//          &&  !getheightdelta ( log2( actvehicle->height), log2(objtype->getEffectiveHeight())) ) {
          Resources r = objtype->removecost;
          for ( int i = 0; i <3; ++i )
             if ( r.resource(i) < 0 )
                r.resource(i) = 0;

          if ( veh->getResource(r, true) == r && veh->getMovement() >= objtype->remove_movecost )
             return true;
       }
    }
    return false;
}


void ObjectBuildingGui::addButton( int &num, const MapCoordinate& mc, ContainerBase* subject, int id )
{
    GuiButton* b = host->getButton(num);
    b->registerFunc( this, mc, subject, id );
    b->Show();
    ++num;
}

void ObjectBuildingGui::search ( const MapCoordinate& pos, int& num, int pass )
{
   tfield* fld =  actmap->getField(pos);
   if ( !fld )
      return;

   if ( fld->building || fld->vehicle || !fieldvisiblenow(fld) )
      return;

   std::map<int,bool> buttons;
   
   for ( int i = 0; i < veh->typ->objectsBuildable.size(); i++ )
     for ( int j = veh->typ->objectsBuildable[i].from; j <= veh->typ->objectsBuildable[i].to; j++ )
       if ( checkObject( fld, actmap->getobjecttype_byid ( j ), Build ))
          if ( pass==1 ) {
            if ( !buttonDone( buttons, j ))
               addButton(num, pos, veh, j);
            buttons[j] = true;
          } else {
            ++num;
            fld->a.temp = 1;
          }


   for ( int i = 0; i < veh->typ->objectGroupsBuildable.size(); i++ )
     for ( int j = veh->typ->objectGroupsBuildable[i].from; j <= veh->typ->objectGroupsBuildable[i].to; j++ )
       for ( int k = 0; k < objectTypeRepository.getNum(); k++ ) {
          ObjectType* objtype = objectTypeRepository.getObject_byPos ( k );
          if ( objtype->groupID == j )
             if ( checkObject( fld, objtype, Build ))
                if ( pass==1 ) {
                  if ( !buttonDone( buttons, objtype->id ))
                     addButton(num, pos, veh, objtype->id);
                  buttons[objtype->id] = true;
                } else {
                  ++num;
                  fld->a.temp = 1;
                }
       }

   for ( int i = 0; i < veh->typ->objectsRemovable.size(); i++ )
     for ( int j = veh->typ->objectsRemovable[i].from; j <= veh->typ->objectsRemovable[i].to; j++ )
       if ( checkObject( fld, actmap->getobjecttype_byid ( j ), Remove ))
          if ( pass==1 ) {
            if ( !buttonDone( buttons, -j ))
               addButton(num, pos, veh, -j);
            buttons[-j] = true;
          } else {
            ++num;
            fld->a.temp = 1;
          }


   for ( int i = 0; i < veh->typ->objectGroupsRemovable.size(); i++ )
     for ( int j = veh->typ->objectGroupsRemovable[i].from; j <= veh->typ->objectGroupsRemovable[i].to; j++ )
       for ( int k = 0; k < objectTypeRepository.getNum(); k++ ) {
          ObjectType* objtype = objectTypeRepository.getObject_byPos ( k );
          if ( objtype->groupID == j )
             if ( checkObject( fld, objtype, Remove ))
                if ( pass==1 ) {
                  if ( !buttonDone( buttons, -objtype->id ))
                     addButton(num, pos, veh, -objtype->id);
                  buttons[-objtype->id] = true;
                } else {
                  ++num;
                  fld->a.temp = 1;
                }
       }

}


void ObjectBuildingGui::eval( const MapCoordinate& mc, ContainerBase* subject )
{
   int num = 0;
   if ( mc.x < actmap->xsize || mc.y < actmap->ysize )
      if ( veh )
         if ( beeline ( mc, veh->getPosition() ) == 10 )
            search( mc, num, 1 );

   GuiButton* b = host->getButton(num);
   b->registerFunc( this, mc, subject, 0 );
   b->Show();
   ++num;

   host->disableButtons(num);
}



ObjectBuildingGui objectBuildingGui;


class BuildObject : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num ) ;
      void execute( const MapCoordinate& pos, ContainerBase* subject, int num );
      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return IconRepository::getIcon("buildobjects.png");
      };
      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "object ~c~onstruction";
      };
      bool checkForKey( const SDL_KeyboardEvent* key, int modifier )
      {
         return ( key->keysym.unicode == 'c' );
      };
};


bool BuildObject::available( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   tfield* fld = actmap->getField(pos);
   if ( fld && fld->vehicle )
      if (fld->vehicle->color == actmap->actplayer * 8)
         if ( fld->vehicle->typ->objectsBuildable.size() || fld->vehicle->typ->objectsRemovable.size() || fld->vehicle->typ->objectGroupsBuildable.size() || fld->vehicle->typ->objectGroupsRemovable.size())
            if (moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing)
               if ( !fld->vehicle->attacked )
                  return true;
   return false;
}

void BuildObject::execute(  const MapCoordinate& pos, ContainerBase* subject, int num )
{
   if ( pendingVehicleActions.actionType == vat_nothing ) {

      tfield* fld = actmap->getField(pos);
      if ( fld->vehicle )
         if ( objectBuildingGui.init( fld->vehicle )) {
            moveparams.movestatus = 72;
            NewGuiHost::pushIconHandler( &objectBuildingGui );
            repaintMap();
            updateFieldInfo();
         }
   }
}










class VehicleBuildingGui : public GuiIconHandler, public GuiFunction {
      Vehicle* veh;
   protected:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int id  );
      void execute( const MapCoordinate& pos, ContainerBase* subject, int id  );
      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int id  );
      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int id  );

      void search ( const MapCoordinate& pos, int& num, int pass );

      void addButton( int &num, const MapCoordinate& mc, ContainerBase* subject, int id );

   public:
      VehicleBuildingGui() : veh( NULL ) {};
      bool init( Vehicle* vehicle );
      void eval( const MapCoordinate& mc , ContainerBase* subject );

};


bool VehicleBuildingGui::init( Vehicle* vehicle )
{
   veh = vehicle;
   int num = 0;

   for ( int i = 0; i< 6; ++i)
      search ( getNeighbouringFieldCoordinate(veh->getPosition(), i), num, 0 );

   return num > 0;
}


bool VehicleBuildingGui::available( const MapCoordinate& pos, ContainerBase* subject, int id  )
{
   return true;
}

void VehicleBuildingGui::execute( const MapCoordinate& pos, ContainerBase* subject, int id  )
{
   if ( id ) {
      Vehicletype* vt = vehicleTypeRepository.getObject_byID( id );
      Vehicle* v = veh->constructvehicle ( vt, pos.x, pos.y );
      logtoreplayinfo ( rpl_buildtnk4, pos.x, pos.y, vt->id, veh->getOwner(), veh->getPosition().x, veh->getPosition().y, int(v->height) );
      evaluateviewcalculation( actmap );
   }
   moveparams.movestatus = 0;
   actmap->cleartemps();
   NewGuiHost::popIconHandler();
   repaintMap();
   updateFieldInfo();
}

Surface& VehicleBuildingGui::getImage( const MapCoordinate& pos, ContainerBase* subject, int id )
{
   if ( id == 0 )
      return IconRepository::getIcon("cancel.png");

   Vehicletype* vehtype = vehicleTypeRepository.getObject_byID( id );

   if ( !vehtype )
      return IconRepository::getIcon("cancel.png");

   static map<int,Surface> removeIconRepository;

   if ( removeIconRepository.find( id ) != removeIconRepository.end() )
      return removeIconRepository[id];

   removeIconRepository[id] = buildGuiIcon( vehtype->getImage() );

   return removeIconRepository[id];
}


ASCString VehicleBuildingGui::getName( const MapCoordinate& pos, ContainerBase* subject, int id )
{
   if ( id == 0 )
      return "cancel (~ESC~)";

   Vehicletype* vehtype = vehicleTypeRepository.getObject_byID( id );
   if ( !vehtype )
      return "";

   ASCString result;
   result.format( "Build %s (%d Material, %d Fuel)", vehtype->name.c_str(), vehtype->productionCost.material, vehtype->productionCost.fuel );

   return result;
}



void VehicleBuildingGui::addButton( int &num, const MapCoordinate& mc, ContainerBase* subject, int id )
{
    GuiButton* b = host->getButton(num);
    b->registerFunc( this, mc, subject, id );
    b->Show();
    ++num;
}

void VehicleBuildingGui::search ( const MapCoordinate& pos, int& num, int pass )
{
   tfield* fld =  actmap->getField(pos);
   if ( fld->building || fld->vehicle || !fieldvisiblenow(fld) )
      return;

     for ( int i = 0; i < veh->typ->vehiclesBuildable.size(); i++ )
       for ( int j = veh->typ->vehiclesBuildable[i].from; j <= veh->typ->vehiclesBuildable[i].to; j++ )
         if ( actmap->getgameparameter(cgp_forbid_unitunit_construction) == 0 || actmap->unitProduction.check(j) ) {
            Vehicletype* v = actmap->getvehicletype_byid ( j );
            if ( v && veh->vehicleconstructable ( v, pos.x, pos.y )) {
               if ( pass==1 )
                  addButton(num, pos, veh, v->id);
               else {
                  ++num;
                  fld->a.temp = 1;
               }   
            }
         }
}


void VehicleBuildingGui::eval( const MapCoordinate& mc , ContainerBase* subject )
{
   int num = 0;
   if ( mc.x < actmap->xsize || mc.y < actmap->ysize )
      if ( veh )
         if ( beeline ( mc, veh->getPosition() ) == 10 )
            search( mc, num, 1 );

   GuiButton* b = host->getButton(num);
   b->registerFunc( this, mc, subject, 0 );
   b->Show();
   ++num;

   host->disableButtons(num);
}



VehicleBuildingGui vehicleBuildingGui;


class BuildVehicle : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num ) ;
      void execute( const MapCoordinate& pos, ContainerBase* subject, int num );
      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return IconRepository::getIcon("constructunit.png");
      };
      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "Unit construction";
      };
};


bool BuildVehicle::available( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   tfield* fld = actmap->getField(pos);
   if ( fld && fld->vehicle )
      if (fld->vehicle->color == actmap->actplayer * 8)
         if ( fld->vehicle->typ->vehiclesBuildable.size() )
            if (moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing)
               if ( !fld->vehicle->attacked )
                  return true;
   return false;
}

void BuildVehicle::execute(  const MapCoordinate& pos, ContainerBase* subject, int num )
{
   if ( pendingVehicleActions.actionType == vat_nothing ) {
      tfield* fld = actmap->getField(pos);
      if ( fld->vehicle )
         if ( vehicleBuildingGui.init( fld->vehicle )) {
            moveparams.movestatus = 73;
            NewGuiHost::pushIconHandler( &vehicleBuildingGui );
            repaintMap();
            updateFieldInfo();
         }
   }
}












class BuildingConstruction : public GuiIconHandler, public GuiFunction {
      Vehicle* veh;
      int bldid;
      map<MapCoordinate,int> entryPos;
   protected:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int id  );
      void execute( const MapCoordinate& pos, ContainerBase* subject, int id  );
      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int id  );
      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int id  );

      void search ( const MapCoordinate& pos, int& num, int pass );

      void addButton( int &num, const MapCoordinate& mc, ContainerBase* subject, int id );

   public:
      BuildingConstruction() : veh( NULL ), bldid(-1) {};
      bool init( Vehicle* vehicle );
      bool setup();
      void eval( const MapCoordinate& mc, ContainerBase* subject );
      bool setBuilding( const MapCoordinate& pos, ContainerBase* subject, int id );

};


bool BuildingConstruction::init( Vehicle* vehicle )
{
   entryPos.clear();
   veh = vehicle;
   bldid = -1;
   int num = 0;

   if ( moveparams.movestatus == 0 )
      for ( int i = 0; i < buildingTypeRepository.getNum(); i++)
         if ( veh->buildingconstructable ( buildingTypeRepository.getObject_byPos( i ) ))
            ++num;

   return num > 0;
}


bool BuildingConstruction::setup()
{
   int num = 0;

   if ( moveparams.movestatus == 110 )
      for ( int i = 0; i < buildingTypeRepository.getNum(); i++)
         if ( veh->buildingconstructable ( buildingTypeRepository.getObject_byPos( i ) ))
            addButton( num, veh->getPosition(), veh, buildingTypeRepository.getObject_byPos( i )->id );


   GuiButton* b = host->getButton(num);
   b->registerFunc( this, veh->getPosition(), veh, -1 );
   b->Show();
   ++num;

   host->disableButtons(num);
   return true;
}


bool BuildingConstruction::available( const MapCoordinate& pos, ContainerBase* subject, int id  )
{
   if ( id < 0 )
      return true;

   if ( moveparams.movestatus == 110 )
      return true;

   if ( moveparams.movestatus == 111 )
      return (actmap->getField(pos)->a.temp == 20) && (id == bldid);

   if ( moveparams.movestatus == 112 )
      return ( entryPos[pos] == bldid ) && (id == bldid);

   return false;
}

bool BuildingConstruction::setBuilding( const MapCoordinate& pos, ContainerBase* subject, int id )
{
   entryPos.clear();
   bldid = id;
   actmap->cleartemps(7);

   int num = 0;
   for ( int i = 0; i< 6; ++i)
      search ( getNeighbouringFieldCoordinate(veh->getPosition(), i), num, 0 );

   if ( num ) {
      moveparams.movestatus = 111;
      repaintMap();
      eval( pos, subject );
      return true;
   } else {
      dispmessage2( 301, "" );
      return false;
   }

}

void BuildingConstruction::execute( const MapCoordinate& pos, ContainerBase* subject, int id  )
{
   bool close = false;
   if ( id <= 0 )
      close  = true;

   BuildingType* bld = buildingTypeRepository.getObject_byID( bldid );


   if (moveparams.movestatus == 110 && !close ) {
      if ( !setBuilding( pos, subject, id ))
         close = true;
   } else

     if (moveparams.movestatus == 111 && !close ) {
         entryPos.clear();
         bool b = true;

         if ( actmap->getField(pos)->a.temp == 20) {

            actmap->cleartemps(7);
            for ( int y1 = 0; y1 <= 5; y1++)
               for ( int x1 = 0; x1 <= 3; x1++)
                  if ( bld->fieldExists ( BuildingType::LocalCoordinate(x1, y1) ) ) {
                     MapCoordinate mc = bld->getFieldCoordinate( pos,  BuildingType::LocalCoordinate(x1,y1));
                     tfield* fld = actmap->getField( mc );
                     if ( fld ) {
                         if ( fld->vehicle || (fld->building && fld->building->getCompletion() == fld->building->typ->construction_steps-1 ))
                            b = false;

                         if ( b ) {
                           if ( x1 == bld->entry.x  && y1 == bld->entry.y ) {
                              fld->a.temp = 23;
                              entryPos[pos] = bldid;
                           } else {
                              fld->a.temp = 22;
                           }
                         } else
                            displaymessage("severe error: \n inconsistency between level1 and level2 of putbuilding",2);
                     } else
                       b = false;
                  }
            if ( b ) {
               moveparams.movestatus = 112;
               repaintMap();
               eval( pos, subject );
            }
         }
     } else
        if (moveparams.movestatus == 112 && !close ) {
            actmap->cleartemps(7);
            entryPos.clear();
            putbuilding2( pos, veh->color, bld);

            logtoreplayinfo ( rpl_putbuilding2, pos.x, pos.y, bldid, (int) veh->color, veh->networkid );

            int mf = actmap->getgameparameter ( cgp_building_material_factor );
            int ff = actmap->getgameparameter ( cgp_building_fuel_factor );
            if ( mf <= 0 )
               mf = 100;

            if ( ff <= 0 )
               ff = 100;

            Resources cost = Resources ( 0, bld->productionCost.material * mf / 100, bld->productionCost.fuel * ff / 100);
            if ( veh->getResource( cost, false ) < cost )
               displaymessage("not enough resources!",1);

            close = true;
            veh->setMovement ( 0 );
            veh->attacked = true;
            computeview( actmap );
         }

   if ( close ) {
      moveparams.movestatus = 0;
      actmap->cleartemps();
      NewGuiHost::popIconHandler();
      repaintMap();
      updateFieldInfo();
   }
}

Surface generate_gui_build_icon ( BuildingType* bld )
{
   Surface s = Surface::createSurface(500,500,32, 0);

   int minx = 1000;
   int miny = 1000;
   int maxx = 0;
   int maxy = 0;

    for (int y = 0; y <= 5; y++)
       for (int x = 0; x <= 3; x++)
          if (bld->fieldExists( BuildingType::LocalCoordinate(x,y) ) ) {
             int xp = fielddistx * x  + fielddisthalfx * ( y & 1);
             int yp = fielddisty * y ;
             if ( xp < minx )
                minx = xp;
             if ( yp < miny )
                miny = yp;
             if ( xp > maxx )
                maxx = xp;
             if ( yp > maxy )
                maxy = yp;

             // bld->paintSingleField( s, SPoint(xp,yp), BuildingType::LocalCoordinate(x,y) );
             s.Blit( bld->getPicture( BuildingType::LocalCoordinate(x,y)), SPoint(xp,yp) );
          }
   maxx += fieldxsize;
   maxy += fieldysize;

   /*
   Surface sc = Surface::Wrap( PG_Application::GetScreen() );
   sc.Blit( s, SPoint(0,0 ));
   PG_Application::UpdateRect( PG_Application::GetScreen(), 0,0,0,0);
   */
   
   Surface s2 = Surface::createSurface(maxx-minx+1,maxy-miny+1,32,0);
   // s2.Blit( s, SDLmm::SRect(SPoint(minx,miny), SPoint(maxx,maxy) ), SPoint(0,0));

   MegaBlitter<4,4,ColorTransform_None,ColorMerger_AlphaOverwrite,SourcePixelSelector_Rectangle> blitter;
   blitter.setSrcRectangle( SPoint( minx, miny ), maxx-minx, maxy-miny );
   blitter.blit ( s, s2, SPoint(0,0) );

   /*
   
   MegaBlitter<4,4,ColorTransform_None,ColorMerger_AlphaOverwrite,SourcePixelSelector_Zoom> blitter;

   blitter.setSize( s2.w(), s2.h(), s3.w(), s3.h() );
   blitter.initSource ( s2 );
   blitter.blit ( s2, s3, SPoint((s3.w() - blitter.getWidth())/2, (s3.h() - blitter.getHeight())/2) );
   */
   
   return s2;
}


Surface& BuildingConstruction::getImage( const MapCoordinate& pos, ContainerBase* subject, int id )
{
   if ( id <= 0 )
      return IconRepository::getIcon("cancel.png");

   BuildingType* bld = buildingTypeRepository.getObject_byID( id );

   if ( !bld )
      return IconRepository::getIcon("cancel.png");

   static map<int,Surface> removeIconRepository;

   if ( removeIconRepository.find( id ) != removeIconRepository.end() )
      return removeIconRepository[id];

   removeIconRepository[id] = buildGuiIcon( generate_gui_build_icon( bld) );

   return removeIconRepository[id];
}


ASCString BuildingConstruction::getName( const MapCoordinate& pos, ContainerBase* subject, int id )
{
   if ( id <= 0 )
      return "cancel";

   BuildingType* bld = buildingTypeRepository.getObject_byID( id );
   if ( !bld )
      return "";

   ASCString result;
   result.format( "Build %s (%d Material, %d Fuel)", bld->name.c_str(), bld->productionCost.material, bld->productionCost.fuel );

   return result;
}



void BuildingConstruction::addButton( int &num, const MapCoordinate& mc, ContainerBase* subject, int id )
{
    GuiButton* b = host->getButton(num);
    b->registerFunc( this, mc, subject, id );
    b->Show();
    ++num;
}

void BuildingConstruction::search ( const MapCoordinate& pos, int& num, int pass )
{
   tfield* entryfield =  actmap->getField(pos);
   BuildingType* bld = buildingTypeRepository.getObject_byID( bldid );

   if ( bld ) {
      bool b = true;
      for ( int y1 = 0; y1 <= 5; y1++)
         for ( int x1 = 0; x1 <= 3; x1++)
            if ( bld->fieldExists ( BuildingType::LocalCoordinate(x1, y1)) ) {
               tfield* fld = actmap->getField ( bld->getFieldCoordinate( pos, BuildingType::LocalCoordinate(x1,y1) ));
               if (fld) {
                  if (fld->vehicle != NULL)
                     b = false;
                  if ( bld->buildingheight <= chfahrend )
                     if ( bld->terrainaccess.accessible ( fld->bdt ) <= 0 )
                        b = false;
                  if (fld->building != NULL) {
                     if (fld->building->typ != bld)
                        b = false;
                     if (fld->building->getCompletion() == fld->building->typ->construction_steps - 1)
                        b = false;
                     if ( (entryfield->bdt & getTerrainBitType(cbbuildingentry) ).none() )
                        b = false;
                  }
                  if (entryfield->building != fld->building)
                     b = false;
               } else
                  b = false;
            }
      if (b) {
         ++num;
         entryfield->a.temp = 20;
         entryPos[pos] = bldid;
      }
   }

/*
               if ( pass==1 )
                  addButton(num, pos, v->id);
               else
                  fld->a.temp = 1;
                  */
}


void BuildingConstruction::eval( const MapCoordinate& mc, ContainerBase* subject )
{
   if (moveparams.movestatus == 112 || moveparams.movestatus ==111 ) {
      int num = 0;

      if ( entryPos[mc] == bldid )
         addButton( num, mc, veh, bldid );

      GuiButton* b = host->getButton(num);
      b->registerFunc( this, mc, subject, 0 );
      b->Show();
      ++num;

      host->disableButtons(num);
   }
}



BuildingConstruction buildingConstruction;


class ConstructBuilding : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num ) ;
      void execute( const MapCoordinate& pos, ContainerBase* subject, int num );
      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return IconRepository::getIcon("constructbuilding.png");
      };
      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "Building construction";
      };
};


bool ConstructBuilding::available( const MapCoordinate& pos, ContainerBase* subject, int num )
{

   if ( actmap->getgameparameter(cgp_forbid_building_construction) )
      return false;

    tfield* fld = actmap->getField(pos);
    if (moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing) {
       if ( fld->vehicle )
          if ( fld->vehicle->attacked == false && !fld->vehicle->hasMoved() )
             if (fld->vehicle->color == actmap->actplayer * 8)
                if (fld->vehicle->typ->hasFunction( ContainerBaseType::ConstructBuildings  ))
                  return true;
    }
    else
       if (moveparams.movestatus == 111) {
          if (fld->a.temp == 20)
             return true;
       }
       else
          if (moveparams.movestatus == 112)
             if (fld->a.temp == 23)
                return true;

   return false;
}


class BuildingConstructionSelection : public BuildingTypeSelectionItemFactory {
   private:
      const BuildingType* selectedType;
   protected:
      void BuildingTypeSelected( const BuildingType* type ) { selectedType = type;  };
   public:
      BuildingConstructionSelection ( Resources plantResources, const Container& types, int player ) : BuildingTypeSelectionItemFactory(plantResources, types, player ), selectedType(NULL) {};
      const BuildingType* getSelectedType() { return selectedType; };
};

void ConstructBuilding::execute(  const MapCoordinate& pos, ContainerBase* subject, int num )
{
   if ( pendingVehicleActions.actionType == vat_nothing ) {
      tfield* fld = actmap->getField(pos);
      if ( fld->vehicle ) {
         if ( fld->vehicle->attacked || (fld->vehicle->typ->wait && fld->vehicle->hasMoved() )) {
            dispmessage2(302,"");
            return;
         }

         BuildingTypeSelectionItemFactory::Container buildings;
         
         for ( int i = 0; i < buildingTypeRepository.getNum(); i++)
            if ( fld->vehicle->buildingconstructable ( buildingTypeRepository.getObject_byPos( i ), false ))
               buildings.push_back ( buildingTypeRepository.getObject_byPos(i) );

         if ( buildings.empty() ) {
            dispmessage2( 303, NULL );
            return;
         }

         BuildingConstructionSelection* bcs = new BuildingConstructionSelection ( fld->vehicle->getResource( Resources(maxint, maxint, maxint), true ), buildings, fld->vehicle->getOwner() );
         ItemSelectorWindow isw ( NULL, PG_Rect( -1, -1, 550, 650), "Select Building", bcs);
         isw.Show();
         isw.RunModal();
         isw.Hide();
         
         if ( buildingConstruction.init( fld->vehicle ) && bcs->getSelectedType() && fld->vehicle->buildingconstructable( bcs->getSelectedType() )) {
            moveparams.movestatus = 111;
            NewGuiHost::pushIconHandler( &buildingConstruction );
            buildingConstruction.setup();
            if ( !buildingConstruction.setBuilding( pos, subject, bcs->getSelectedType()->id )) {
               NewGuiHost::popIconHandler();
               moveparams.movestatus = 0;
            }
               
            updateFieldInfo();
         }
         
      }
   }
}






class ReplayPlay : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         if ( runreplay.status == 1 )
            return true;

         return false;
      };

      void execute( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
        runreplay.status = 2;
        updateFieldInfo();
      }

      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return IconRepository::getIcon("replay-play.png");
      };

      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "start re~p~lay";
      };

      bool checkForKey( const SDL_KeyboardEvent* key, int modifier )
      {
         return ( key->keysym.unicode == 'p' );
      };
      
};


class ReplayPause : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         if ( runreplay.status == 2 )
            return true;

         return false;
      };

      void execute( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
        runreplay.status = 1;
        updateFieldInfo();
      }

      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return IconRepository::getIcon("replay-pause.png");
      };

      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "~p~ause replay";
      };
      
      bool checkForKey( const SDL_KeyboardEvent* key, int modifier )
      {
         return ( key->keysym.unicode == 'p' );
      };
      
};



class ReplayFaster : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         if ( runreplay.status == 2 )
            if ( CGameOptions::Instance()->replayspeed > 0 )
              return true;

         return false;
      };

      void execute( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         if ( CGameOptions::Instance()->replayspeed > 20 )
            CGameOptions::Instance()->replayspeed -= 20;
         else
            CGameOptions::Instance()->replayspeed = 0;

         CGameOptions::Instance()->setChanged ( 1 );
         displaymessage2 ( "delay set to %d / 100 sec", CGameOptions::Instance()->replayspeed );
         updateFieldInfo();
      }

      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return IconRepository::getIcon("replay-faster.png");
      };

      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "increase replay speed (~+~)";
      };

      bool checkForKey( const SDL_KeyboardEvent* key, int modifier )
      {
         return ( key->keysym.unicode == '+' );
      };
      
};


class ReplaySlower : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         if ( runreplay.status == 2 )
            return true;

         return false;
      };

      void execute( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         CGameOptions::Instance()->replayspeed += 20;
         CGameOptions::Instance()->setChanged ( 1 );
         displaymessage2 ( "delay set to %d / 100 sec", CGameOptions::Instance()->replayspeed );
         updateFieldInfo();
      }

      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return IconRepository::getIcon("replay-slow.png");
      };

      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "decrease replay speed (~-~)";
      };

      bool checkForKey( const SDL_KeyboardEvent* key, int modifier )
      {
         return ( key->keysym.unicode == '-' );
      };
      
};


class ReplayRewind : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         if ( runreplay.status == 1  ||  runreplay.status == 10 )
              return true;

         return false;
      };

      void execute( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         runreplay.status = 101;
         updateFieldInfo();
      }

      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return IconRepository::getIcon("replay-back.png");
      };

      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "~r~estart replay";
      };

      bool checkForKey( const SDL_KeyboardEvent* key, int modifier )
      {
         return ( key->keysym.unicode == 'r' );
      };
      
};


class ReplayExit : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         if ( runreplay.status == 1 || runreplay.status == 10 || runreplay.status == 11 )
              return true;

         return false;
      };

      void execute( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         runreplay.status = 100;
         updateFieldInfo();
      }

      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return IconRepository::getIcon("replay-exit.png");
      };

      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "e~x~it replay";
      };

      bool checkForKey( const SDL_KeyboardEvent* key, int modifier )
      {
         return ( key->keysym.unicode == 'x' );
      };
      
};



GuiIconHandler primaryGuiIcons;






} // namespace GuiFunctions


void registerCargoGuiFunctions( GuiIconHandler& handler )
{
   // handler.registerUserFunction( new GuiFunctions::Movement() );
}

void registerReplayGuiFunctions( GuiIconHandler& handler )
{
   handler.registerUserFunction( new GuiFunctions::ReplayPlay() );
   handler.registerUserFunction( new GuiFunctions::ReplayPause() );
   handler.registerUserFunction( new GuiFunctions::ReplayFaster() );
   handler.registerUserFunction( new GuiFunctions::ReplaySlower() );
   handler.registerUserFunction( new GuiFunctions::ReplayRewind() );
   handler.registerUserFunction( new GuiFunctions::ReplayExit() );
}

void registerGuiFunctions( GuiIconHandler& handler )
{
   handler.registerUserFunction( new GuiFunctions::Movement() );
   handler.registerUserFunction( new GuiFunctions::Attack() );
   handler.registerUserFunction( new GuiFunctions::JumpDriveIcon() );
   handler.registerUserFunction( new GuiFunctions::PowerOn() );
   handler.registerUserFunction( new GuiFunctions::PowerOff() );
   handler.registerUserFunction( new GuiFunctions::UnitInfo() );
   handler.registerUserFunction( new GuiFunctions::BuildObject() );
   handler.registerUserFunction( new GuiFunctions::BuildVehicle() );
   handler.registerUserFunction( new GuiFunctions::ConstructBuilding() );
   handler.registerUserFunction( new GuiFunctions::DestructBuilding() );
//   handler.registerUserFunction( new GuiFunctions::SearchForMineralResources() );
   handler.registerUserFunction( new GuiFunctions::OpenContainer() );
   handler.registerUserFunction( new GuiFunctions::EnableReactionfire() );
   handler.registerUserFunction( new GuiFunctions::DisableReactionfire() );
   handler.registerUserFunction( new GuiFunctions::Ascend );
   handler.registerUserFunction( new GuiFunctions::Descend );
   handler.registerUserFunction( new GuiFunctions::RepairUnit );
   handler.registerUserFunction( new GuiFunctions::RefuelUnit );
   handler.registerUserFunction( new GuiFunctions::RefuelUnitDialog );
   // handler.registerUserFunction( new GuiFunctions::ViewMap );
   handler.registerUserFunction( new GuiFunctions::PutMine );
   handler.registerUserFunction( new GuiFunctions::PutAntiTankMine );
   handler.registerUserFunction( new GuiFunctions::PutAntiPersonalMine );
   handler.registerUserFunction( new GuiFunctions::PutAntiShipMine );
   handler.registerUserFunction( new GuiFunctions::PutAntiSubMine );
   handler.registerUserFunction( new GuiFunctions::RemoveMine );
   
   handler.registerUserFunction( new GuiFunctions::EndTurn() );
   handler.registerUserFunction( new GuiFunctions::Cancel() );
   
}

