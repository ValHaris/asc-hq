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

#include <stdio.h>           
#include <cstring>
#include <math.h>
#include <stdarg.h>
#include <ctime>

#include "buildingtype.h"
#include "vehicletype.h"
#include "typen.h"

#include "newfont.h"
#include "spfst.h"
#include "loaders.h"
#include "misc.h"
#include "controls.h"
#include "stack.h"
#include "dlg_box.h"
#include "dialog.h"
#include "attack.h"
#include "gamedlg.h"
#include "gameoptions.h"
#include "ai/ai.h"
#include "errors.h"
#include "viewcalculation.h"
#include "replay.h"
#include "resourcenet.h"
#include "itemrepository.h"
#include "strtmesg.h"
#include "messagedlg.h"
#include "gameevent_dialogs.h"
#include "cannedmessages.h"
#include "mapdisplay.h"
#include "player.h"

#include "dialogs/choosetech.h"

static TechnologyPresenter* techPresenter = NULL;

void setResearchPresenter( TechnologyPresenter* presenter )
{
   techPresenter = presenter;
}


static bool anyTechAvailable( const Player& player )
{
   for (int i = 0; i < technologyRepository.getNum(); i++) {
      const Technology* tech = technologyRepository.getObject_byPos( i );
      if ( tech ) {
         ResearchAvailabilityStatus a = player.research.techAvailable ( tech );
         if ( a == Available )
            return true;
      }
   }
   return false;
}


//! checks, which vehicle and building types are newly available. 
class NewGadgetDetection  {
      std::list<const Vehicletype*>  units;
      std::list<const BuildingType*>  buildings;
      Player& p;
   public:
      NewGadgetDetection( Player& player ) : p(player )
      {
         for ( int i=0; i < vehicleTypeRepository.getNum() ; i++ ) {
            const Vehicletype* v = vehicleTypeRepository.getObject_byPos ( i );
            if ( !v->techDependency.available( p.research ))
               units.push_back( v );
         }
         
         for ( int i=0; i < buildingTypeRepository.getNum() ; i++ ) {
            const BuildingType* b = buildingTypeRepository.getObject_byPos ( i );
            if ( !b->techDependency.available( p.research ))
               buildings.push_back( b );
         }
         
      }

     //! checks, which vehicle are now available that where not available when constructor was called. 
      void evaluate( TechnologyPresenter::Gadgets& storage )
      {
         for ( int i=0; i < vehicleTypeRepository.getNum() ; i++ ) {
            const Vehicletype* v = vehicleTypeRepository.getObject_byPos ( i );
            if ( !v->techDependency.available( p.research ))
               units.remove( v );
         }
         
         for ( int i=0; i < buildingTypeRepository.getNum() ; i++ ) {
            const BuildingType* b = buildingTypeRepository.getObject_byPos ( i );
            if ( !b->techDependency.available( p.research ))
               buildings.remove( b );
         }
         
         storage.buildings = buildings;
         storage.units = units;
      }
};




const Technology* getNextTechnologyTowardsGoal( Player& player )
{
   if ( !player.research.goal )
      return NULL;
   
   if ( player.research.techResearched( player.research.goal->id )) {
      player.research.goal = NULL;
      return NULL;
   }
   
   list<const Technology*> techs;
   if ( player.research.goal->eventually_available( player.research, &techs ))
      return *techs.begin();
   else
      return NULL;
}


void runResearch( Player& player, vector<const Technology*>* newTechs, vector<ASCString>* newTechAdapter )
{
   Research& research = player.research;
   
   if ( research.activetechnology && research.techResearched( research.activetechnology->id ) ) {
      /* the current research may suddenly be researched already if the map was modified by the ditor */
      research.progress = max( 0, research.progress - research.activetechnology->researchpoints ) ;
      research.activetechnology = NULL;
      if ( !anyTechAvailable( player ) ) 
         return;
   }
   
   while ( research.activetechnology  &&  (research.progress >= research.activetechnology->researchpoints)) {
      
      const Technology* newTech = research.activetechnology;
      research.activetechnology = NULL;
      
      if ( newTechs )
         newTechs->push_back( newTech );
      
      NewGadgetDetection ngd( player );
      
      vector<ASCString> techAdapters = research.addanytechnology( newTech );
      if( newTechAdapter )
         newTechAdapter->insert( newTechAdapter->end(), techAdapters.begin(), techAdapters.end() );

      if ( techPresenter ) {
         TechnologyPresenter::Gadgets newItemsAvailable;
         ngd.evaluate( newItemsAvailable );
         techPresenter->showTechnology( newTech, newItemsAvailable );
      }
      
      research.progress -= newTech->researchpoints;

      research.activetechnology = getNextTechnologyTowardsGoal( player );
   }
   
}


void checkForNewResearch( Player& player )
{
   Research& research = player.research;
   
   while ( research.activetechnology == NULL && research.progress && anyTechAvailable( player ) ) 
      if ( !chooseTechnology( player ))
         return;
}
      


