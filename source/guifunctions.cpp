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
#include "sg.h"
#include "graphics/blitter.h"

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



class UnitInfo : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, int num ) ;
      void execute( const MapCoordinate& pos, int num ) { execUserAction_ev(ua_vehicleinfo);};
      Surface& getImage( const MapCoordinate& po, int nums )
      {
         return IconRepository::getIcon("unitinfo.png");
      };
      ASCString getName( const MapCoordinate& pos, int num )
      {
         return "unit info";
      };
};

bool UnitInfo::available( const MapCoordinate& pos, int num )
{
   pfield fld = actmap->getField(pos);
   if ( fld && fld->vehicle )
      if (moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing)
         if ( fld->vehicle != NULL)
            if (fieldvisiblenow( fld ))
               return true;
   return false;
}






class ObjectBuildingGui : public GuiIconHandler, public GuiFunction {
      Vehicle* veh;
   protected:
      enum Mode { Build, Remove };

      bool available( const MapCoordinate& pos, int num );
      void execute( const MapCoordinate& pos, int num );
      Surface& getImage( const MapCoordinate& pos, int num );
      ASCString getName( const MapCoordinate& pos, int num );
      bool checkObject( pfield fld, pobjecttype objtype, Mode mode );

      void search ( const MapCoordinate& pos, int& num, int pass );

      void addButton( int &num, const MapCoordinate& mc, int id );

   public:
      ObjectBuildingGui() : veh( NULL ) {};
      bool init( Vehicle* vehicle );
      void eval();

};


bool ObjectBuildingGui::init( Vehicle* vehicle )
{
   veh = vehicle;
   int num;

   for ( int i = 0; i< 6; ++i)
      search ( getNeighbouringFieldCoordinate(veh->getPosition(), i), num, 0 );

   return num > 0;
}


bool ObjectBuildingGui::available( const MapCoordinate& pos, int num )
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

void ObjectBuildingGui::execute( const MapCoordinate& pos, int num )
{
   if ( num ) {
      pfield fld = actmap->getField(pos);
      pobjecttype obj = objectTypeRepository.getObject_byID( abs(num) );

      if ( !fld->checkforobject ( obj ) ) {
         assert(num>0);
         fld-> addobject ( obj );
         logtoreplayinfo ( rpl_buildobj2, pos.x, pos.y, obj->id, veh->networkid );
      } else {
         assert(num<0);
         fld->removeobject ( obj );
         logtoreplayinfo ( rpl_remobj2, pos.x, pos.y, obj->id, veh->networkid );
      }

      veh->getResource( num > 0 ? obj->buildcost : obj->removecost, false );

      veh->decreaseMovement ( num > 0 ? obj->build_movecost : obj->remove_movecost );
   }
   moveparams.movestatus = 0;
   actmap->cleartemps();
   NewGuiHost::popIconHandler();
   repaintMap();
   updateFieldInfo();
}

Surface& ObjectBuildingGui::getImage( const MapCoordinate& pos, int num )
{
   if ( num == 0 )
      return IconRepository::getIcon("cancel.png");

   pobjecttype objtype = objectTypeRepository.getObject_byID( abs(num) );

   if ( !objtype )
      return IconRepository::getIcon("cancel.png");

   static map<int,Surface> removeIconRepository;

   if ( removeIconRepository.find( num ) != removeIconRepository.end() )
      return removeIconRepository[num];


   const Surface& cancelIcon = IconRepository::getIcon("cancel.png");
   Surface s = Surface::createSurface( cancelIcon.w(), cancelIcon.h(), 32, 0 );

   Surface& o = objtype->getPicture();
   if ( o.GetPixelFormat().BitsPerPixel() == 32 ) {
      MegaBlitter<4,4,ColorTransform_None, ColorMerger_AlphaOverwrite, SourcePixelSelector_Zoom> blitter;
      blitter.setSize( o.w(), o.h(), s.w(), s.h() );
      blitter.blit( o, s, SPoint((s.w() - blitter.getZoom() * o.w())/2, (s.h() - blitter.getZoom() * o.h())/2));
   } else {
      MegaBlitter<1,4,ColorTransform_None, ColorMerger_AlphaOverwrite, SourcePixelSelector_Zoom> blitter;
      blitter.setSize( o.w(), o.h(), s.w(), s.h() );
      blitter.blit( o, s, SPoint((s.w() - blitter.getZoom() * o.w())/2, (s.h() - blitter.getZoom() * o.h())/2));
   }



/*

   Surface o = objtype->getPicture().Duplicate();
   float ratio = min( float( cancelIcon.w()) / o.w(), float(cancelIcon.h()) / o.h() );
   o.strech( int( ratio * o.w()), int( ratio * o.h() ));
   Surface s = Surface::createSurface( cancelIcon.w(), cancelIcon.h(), 32, 0 );
   s.Blit( o, SPoint((s.w() - o.w())/2, (s.h() - o.h())/2));
*/

   if ( num < 0 ) {
      MegaBlitter<4,4,ColorTransform_None, ColorMerger_AlphaOverwrite> blitter;
      Surface& removegui = IconRepository::getIcon( "cancel-addon.png" );
      blitter.blit( removegui, s,  SPoint((s.w() - removegui.w())/2, (s.h() - removegui.h())/2));
   }

   removeIconRepository[num] = s;
   return removeIconRepository[num];
}


ASCString ObjectBuildingGui::getName( const MapCoordinate& pos, int num )
{
   if ( num == -1 )
      return "cancel";

   pobjecttype objtype = objectTypeRepository.getObject_byID( abs(num) );
   if ( !objtype )
      return "";

   ASCString result;
   if ( num < 0 ) {
      result.format( "Build %s (%d Material, %d Fuel, %d Movepoints)", objtype->name.c_str(), objtype->buildcost.material, objtype->buildcost.fuel, objtype->build_movecost );
      num = -num;
   } else
      result.format( "Remove %s (%d Material, %d Fuel, %d Movepoints)", objtype->name.c_str(), objtype->removecost.material, objtype->removecost.fuel, objtype->remove_movecost );

   return result;
}


bool ObjectBuildingGui::checkObject( pfield fld, pobjecttype objtype, Mode mode )
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


void ObjectBuildingGui::addButton( int &num, const MapCoordinate& mc, int id )
{
    GuiButton* b = host->getButton(num);
    b->registerFunc( this, mc, id );
    b->Show();
    ++num;
}

void ObjectBuildingGui::search ( const MapCoordinate& pos, int& num, int pass )
{
   pfield fld =  actmap->getField(pos);
   if ( fld->building || fld->vehicle || !fieldvisiblenow(fld) )
      return;

   for ( int i = 0; i < veh->typ->objectsBuildable.size(); i++ )
     for ( int j = veh->typ->objectsBuildable[i].from; j <= veh->typ->objectsBuildable[i].to; j++ )
       if ( checkObject( fld, actmap->getobjecttype_byid ( j ), Build ))
          if ( pass==1 )
            addButton(num, pos, j);
          else
            fld->a.temp = 1;


   for ( int i = 0; i < veh->typ->objectGroupsBuildable.size(); i++ )
     for ( int j = veh->typ->objectGroupsBuildable[i].from; j <= veh->typ->objectGroupsBuildable[i].to; j++ )
       for ( int k = 0; k < objectTypeRepository.getNum(); k++ ) {
          pobjecttype objtype = objectTypeRepository.getObject_byPos ( k );
          if ( objtype->groupID == j )
             if ( checkObject( fld, objtype, Build ))
               if ( pass==1 )
                  addButton(num, pos, objtype->id);
               else
                  fld->a.temp = 1;
       }

   for ( int i = 0; i < veh->typ->objectsRemovable.size(); i++ )
     for ( int j = veh->typ->objectsRemovable[i].from; j <= veh->typ->objectsRemovable[i].to; j++ )
       if ( checkObject( fld, actmap->getobjecttype_byid ( j ), Remove ))
          if ( pass==1 )
            addButton(num, pos, -j);
          else
            fld->a.temp = 1;


   for ( int i = 0; i < veh->typ->objectGroupsRemovable.size(); i++ )
     for ( int j = veh->typ->objectGroupsRemovable[i].from; j <= veh->typ->objectGroupsRemovable[i].to; j++ )
       for ( int k = 0; k < objectTypeRepository.getNum(); k++ ) {
          pobjecttype objtype = objectTypeRepository.getObject_byPos ( k );
          if ( objtype->groupID == j )
             if ( checkObject( fld, objtype, Remove ))
                if ( pass==1 )
                   addButton(num, pos, -objtype->id);
                else
                   fld->a.temp = 1;
       }

}


void ObjectBuildingGui::eval()
{
   MapCoordinate mc = actmap->player[actmap->actplayer].cursorPos;

   if ( !mc.valid() )
      return;

   int num = 0;
   if ( mc.x < actmap->xsize || mc.y < actmap->ysize )
      if ( veh )
         if ( beeline ( mc, veh->getPosition() ) == 10 )
            search( mc, num, 1 );

   GuiButton* b = host->getButton(num);
   b->registerFunc( this, mc, 0 );
   b->Show();
   ++num;

   host->disableButtons(num);
}



ObjectBuildingGui objectBuildingGui;


class BuildObject : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, int num ) ;
      void execute( const MapCoordinate& pos, int num );
      Surface& getImage( const MapCoordinate& pos, int num )
      {
         return IconRepository::getIcon("buildobjects.png");
      };
      ASCString getName( const MapCoordinate& pos, int num )
      {
         return "Object construction";
      };
};


bool BuildObject::available( const MapCoordinate& pos, int num )
{
   pfield fld = actmap->getField(pos);
   if ( fld && fld->vehicle )
      if (fld->vehicle->color == actmap->actplayer * 8)
         if ( fld->vehicle->typ->objectsBuildable.size() || fld->vehicle->typ->objectsRemovable.size() || fld->vehicle->typ->objectGroupsBuildable.size() || fld->vehicle->typ->objectGroupsRemovable.size())
            if (moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing)
               if ( !fld->vehicle->attacked )
                  return true;
   return false;
}

void BuildObject::execute(  const MapCoordinate& pos, int num )
{
   if ( pendingVehicleActions.actionType == vat_nothing ) {

      pfield fld = actmap->getField(pos);
      if ( fld->vehicle )
         if ( objectBuildingGui.init( fld->vehicle )) {
            moveparams.movestatus = 72;
            NewGuiHost::pushIconHandler( &objectBuildingGui );
            repaintMap();
            updateFieldInfo();
         }
   }
}







GuiIconHandler primaryGuiIcons;



} // namespace GuiFunctions


void registerGuiFunctions( GuiIconHandler& handler )
{
   handler.registerUserFunction( new GuiFunctions::Movement() );
   handler.registerUserFunction( new GuiFunctions::Attack() );
   handler.registerUserFunction( new GuiFunctions::PowerOn() );
   handler.registerUserFunction( new GuiFunctions::PowerOff() );
   handler.registerUserFunction( new GuiFunctions::EndTurn() );
   handler.registerUserFunction( new GuiFunctions::UnitInfo() );
   handler.registerUserFunction( new GuiFunctions::BuildObject() );
}
