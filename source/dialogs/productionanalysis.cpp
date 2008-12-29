/***************************************************************************
                          cargodialog.cpp  -  description
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


#include "../containerbasetype.h"
#include "../vehicletype.h"
#include "../player.h"
#include "../gamemap.h"
#include "../itemrepository.h"

#include "productionanalysis.h"
#include "vehicletypeselector.h"
#include "../widgets/textrenderer.h"

class AvailableUnitWindow : public ItemSelectorWindow {
   private:
      const Vehicletype* selectedVehicleType;
      
      virtual void itemSelected( const SelectionWidget* sw) {
         const VehicleTypeBaseWidget* vtcw = dynamic_cast<const VehicleTypeBaseWidget*>(sw);
         assert( vtcw );
         selectedVehicleType = vtcw->getVehicletype ();
         if ( selectedVehicleType )
            QuitModal();
      };
   public:
      AvailableUnitWindow ( PG_Widget *parent, const PG_Rect &r , const ASCString& title, SelectionItemFactory* itemFactory ) : ItemSelectorWindow( parent, r, title, itemFactory ), selectedVehicleType(NULL) {};
      const Vehicletype* getSelected() { return selectedVehicleType; };
};      




int evaluateProduction( const ContainerBaseType* potentialFactory, const Vehicletype* vt, GameMap* gamemap )
{
   if ( potentialFactory->vehicleFit( vt ) && potentialFactory->hasFunction( ContainerBaseType::InternalVehicleProduction)) 
      return 1;
   else
      return 0;
}

int evaluateProduction( const Vehicletype* potentialFactory, const Vehicletype* vt, GameMap* gamemap )
{
   const ContainerBaseType* cbt = potentialFactory;
   int res = evaluateProduction( cbt, vt, gamemap ); 

   // if ( potentialFactory->hasFunction( ContainerBaseType::ExternalVehicleProduction)) 
      for ( vector<IntRange>::const_iterator i = potentialFactory->vehiclesBuildable.begin(); i != potentialFactory->vehiclesBuildable.end(); ++i )
         if( vt->id >= i->from && vt->id <= i->to ) 
            return res | 2;
   return res;
}


ASCString getProductionString( const ContainerBaseType* potentialFactory, const Vehicletype* vt, GameMap* gamemap )
{
   ASCString s;
   int res = evaluateProduction( potentialFactory, vt, gamemap);
   if ( res ) {
      s = potentialFactory->getName() + " (" + ASCString::toString(potentialFactory->id) + ") ";
      if ( res & 1 )
         s += " internally";
      if ( res & 2 )
         s += " externally";
      s += "\n";
   }
   return s;
}



ASCString getInstances( const ContainerBaseType* evaluatedFactory, const Vehicletype* unitsToProduce, GameMap* gamemap, bool lineAvail )
{
   ASCString instances;
   if ( lineAvail ) {
      int count = 0;
      ASCString units = evaluatedFactory->getName() + ": ";
         
      for ( Player::VehicleList::const_iterator j = gamemap->getCurrentPlayer().vehicleList.begin(); j != gamemap->getCurrentPlayer().vehicleList.end(); ++j )
         if ( (*j)->typ == evaluatedFactory ) 
            for ( Vehicle::Production::const_iterator i = (*j)->getProduction().begin(); i != (*j)->getProduction().end(); ++i)
               if ( *i == unitsToProduce ) {
                  units += (*j)->getPosition().toString();
                  ++count;
               }
         
      if ( count )
         instances += units + "\n";

   }
   {
      int count = 0;
      ASCString units = evaluatedFactory->getName() + ": ";
         
      for ( Player::BuildingList::const_iterator j = gamemap->getCurrentPlayer().buildingList.begin(); j != gamemap->getCurrentPlayer().buildingList.end(); ++j )
         if ( (*j)->typ == evaluatedFactory ) {
            if ( lineAvail ) {
               for ( ContainerBase::Production::const_iterator k = (*j)->getProduction().begin(); k != (*j)->getProduction().end(); ++k )
                  if ( *k == unitsToProduce ) {
                     units += (*j)->getPosition().toString();
                     ++count;
                  }
            } else {
               if ( unitsToProduce->techDependency.available( gamemap->getCurrentPlayer().research ) ) {
                  bool found = false;
                  for ( ContainerBase::Production::const_iterator k = (*j)->getProduction().begin(); k != (*j)->getProduction().end(); ++k )
                     if ( *k == unitsToProduce ) 
                        found = true;
   
                  if ( !found ) {
                     units += (*j)->getPosition().toString();
                     ++count;
                  }
               }
            }
         }
      if ( count )
         instances += units + "\n";
   }

   return instances;
}

template<typename T>
void checkType( T* t, const Vehicletype* evaluatedUnitType, ASCString& instances, ASCString& lineAddable, ASCString& types, GameMap* gamemap, bool checkResearch )
{
   if ( evaluateProduction( t, evaluatedUnitType, gamemap)) {
      instances += getInstances( t, evaluatedUnitType, gamemap, true );               
      lineAddable += getInstances( t, evaluatedUnitType, gamemap, false );
      if ( !checkResearch || t->techDependency.available( gamemap->getCurrentPlayer().research )) 
         types += getProductionString( t, evaluatedUnitType, gamemap );
   }
}

bool vehicleOwned( const Vehicletype* vt, GameMap* gamemap )
{
   for ( Player::VehicleList::const_iterator j = gamemap->getCurrentPlayer().vehicleList.begin(); j != gamemap->getCurrentPlayer().vehicleList.end(); ++j )
      if ( (*j)->typ == vt )
         return true;
   return false;
}


void unitProductionAnalysis( GameMap* gamemap, bool checkResearch )
{
   VehicleTypeSelectionItemFactory::Container c;

   for ( int i = 0; i < vehicleTypeRepository.getNum(); ++i ) {
      Vehicletype* p = vehicleTypeRepository.getObject_byPos(i);
      if ( p ) {
         if ( vehicleOwned( p, gamemap ) || !checkResearch || p->techDependency.available( gamemap->getCurrentPlayer().research )  )
            c.push_back(p);
      }
   }

   AvailableUnitWindow auw( NULL, PG_Rect( -1, -1, 400, 400), "Select Vehicle Type", new VehicleTypeSelectionItemFactory(c, gamemap->getCurrentPlayer() ));
   auw.Show();
   auw.RunModal();
   auw.Hide();


   if ( auw.getSelected() ) {
      ASCString types, instances, lineAddable;

      for ( int i = 0; i < vehicleTypeRepository.getNum(); ++i ) 
         checkType( vehicleTypeRepository.getObject_byPos(i), auw.getSelected(), instances, lineAddable, types, gamemap, checkResearch );
      
      for ( int i = 0; i < buildingTypeRepository.getNum(); ++i ) 
         checkType( buildingTypeRepository.getObject_byPos(i), auw.getSelected(), instances, lineAddable, types, gamemap, checkResearch );

      if ( types.empty() )
         types = "-none-\n";

      if ( instances.empty() )
         instances = "-nowhere-\n";

      if ( lineAddable.empty() )
         lineAddable = "-nowhere-\n";

      types = "\n#fontsize=18#Vehicle and Building types#fontsize=10#\n(provided that the unit has been researched)\n#fontsize=12#" + types;
      instances = "\n#fontsize=18#Production available #fontsize=12#\n" + instances;
      lineAddable = "\n#fontsize=18#Production can be added #fontsize=12#\n" + lineAddable;

      ViewFormattedText vft("Unit Production Analysis", instances + lineAddable + types, PG_Rect( -1, -1, 500, 400 ));
      vft.Show();
      vft.RunModal();
   }
}

