/*
     This file is part of Advanced Strategic Command; http://www.asc-hq.de
     Copyright (C) 1994-2008  Martin Bickel  

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


#include "../sg.h"
#include "../ascstring.h"
#include "../vehicle.h"
#include "../gamemap.h"
#include "../spfst.h"
#include "../spfst-legacy.h"
               
#include "../itemrepository.h"
#include "../dlg_box.h"
#include "../paradialog.h"
#include "../gameeventsystem.h"
#include "../loaders.h"
#include "../accessconstraints.h"

#include <pgpropertyfield_integer.h>
#include <pgpropertyfield_intdropdown.h>
#include <pgpropertyfield_checkbox.h>
#include <pgpropertyfield_string.h>
#include <pgpropertyfield_button.h>
#include <pgpropertyeditor.h>
         
#include "common.h"
         
GameMap* getActiveMap()
{
   return actmap;
}
         
const ObjectType* getObjectType( int id )
{
   return objectTypeRepository.getObject_byID( id );   
}

const BuildingType* getBuildingType( int id )
{
   return buildingTypeRepository.getObject_byID( id );   
}

const VehicleType* getUnitType( int id )
{
   return vehicleTypeRepository.getObject_byID( id );   
}

const TerrainType* getTerrainType( int id )
{
   return terrainTypeRepository.getObject_byID( id );   
}


      bool PropertyDialog :: ok() {
         result = true;
         propertyEditor->Apply();
         QuitModal();
         return true;  
      }
      
      bool PropertyDialog :: cancel() {
         result = false;
         QuitModal();
         return true;  
      }
      
      PropertyDialog :: PropertyDialog( const ASCString& title ) : ASC_PG_Dialog( NULL, PG_Rect( -1, -1, 400, 500 ), title ), propertyEditor(NULL), result(false)
      {
         propertyEditor = new ASC_PropertyEditor( this, PG_Rect( 10, GetTitlebarHeight()+10, Width() - 20, Height() - GetTitlebarHeight() - 60 ), "PropertyEditor", 70 );
         
         StandardButtonDirection( Horizontal );
         AddStandardButton("OK")->sigClick.connect( SigC::slot( *this, &PropertyDialog::ok ));
         AddStandardButton("Cancel")->sigClick.connect( SigC::slot( *this, &PropertyDialog::cancel ));
      }
   
      void PropertyDialog :: addBool( const ASCString& name, bool defaultValue )
      {
         boolValues[name] = defaultValue ; 
         new PG_PropertyField_Checkbox<bool>( propertyEditor, name, &boolValues[name] );
      }
      
      void PropertyDialog :: addInteger( const ASCString& name, int defaultValue )
      {
         intValues[name] = defaultValue; 
         new PG_PropertyField_Integer<int>( propertyEditor, name, &intValues[name] );
      }
      
      void PropertyDialog :: addIntDropdown( const ASCString& name, const StringArray& names, int defaultValue )
      {
         intValues[name] = defaultValue; 
         new PG_PropertyField_IntDropDown<int,vector<ASCString>::const_iterator>( propertyEditor, name, &intValues[name], names.values.begin(), names.values.end() );
      }
      
      void PropertyDialog :: addString( const ASCString& name, const ASCString& defaultValue )
      {
         stringValues[name] = defaultValue ; 
         new PG_PropertyField_String<ASCString>( propertyEditor, name, &stringValues[name] );
      }
      
      std::string PropertyDialog :: getString( const ASCString& name )
      {
         return stringValues[name];  
      }
      
      int PropertyDialog :: getInteger( const ASCString& name )
      {
         return intValues[name];  
      }
      
      bool PropertyDialog :: getBool( const ASCString& name )
      {
         return boolValues[name];  
      }

      bool PropertyDialog :: run() {
         Show();
         RunModal();  
         Hide();
         return result;
      }
   
      
int selectString ( const ASCString& title, const StringArray& entries, int defaultEntry  )
{
   vector<ASCString> buttons;
   buttons.push_back("OK" );
   buttons.push_back("Cancel" );
   
   pair<int,int> r = new_chooseString ( title, entries.values, buttons, defaultEntry );
   if ( r.first == 1 )
      return -1;
   else
      return r.second;
}

GameMap* getLoadingMap()
{
   return eventLocalizationMap;  
}


void setLocalizedEventMessage( GameMap* eventLocalizationMap, int eventID, const ASCString& message )
{
   if ( !eventLocalizationMap )
      return;
   
   for ( GameMap::Events::const_iterator i = eventLocalizationMap->events.begin(); i != eventLocalizationMap->events.end(); ++i ) 
      if ( (*i)->id == eventID )
         (*i)->action->setLocalizationString( message );
}
      
void setLocalizedContainerName( GameMap* map, const MapCoordinate& pos, const std::string& name )
{
   if ( !map )
      return;
   
   MapField* fld = map->getField( pos );
   if ( !fld )
      return;
   
   ContainerBase* c = fld->getContainer();
   if ( !c )
      return;
   
   if ( !checkModificationConstraints( c ))
      return;
   
   c->setName( name );
}
      
      
      
MapCoordinate getCursorPosition( const GameMap* gamemap )
{
   return gamemap->getCursor(); 
}
      
