/*! \file gameevent_dialogs.cpp
    \brief dialogs for the game event system
*/

/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2010  Martin Bickel  and  Marc Schellenberger

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

#include "global.h"
#include "buildingtype.h"
#include "vehicletype.h"
#include "newfont.h"
#include "typen.h"
// #include "basegfx.h"
#include "dlg_box.h"
#include "newfont.h"
#include "vehicletype.h"
#include "buildingtype.h"
#include "spfst.h"

#include "gameevents.h"
// #include "events.h"
#include "gameoptions.h"
#include "loadimage.h"
#include "errors.h"
#include "itemrepository.h"
#include "gameevent_dialogs.h"
#include "dialog.h"
#include "spfst-legacy.h"

#include "dialogs/fieldmarker.h"
#include "widgets/textrenderer.h"
#include "widgets/playerselector.h"
#include "dialogs/selectionwindow.h"
#include "dialogs/vehicletypeselector.h"
#include <pgpropertyeditor.h>
#include <pgpropertyfield_checkbox.h>

#ifdef karteneditor
# include "edmisc.h"
# include "edselfnt.h"
# include "maped-mainscreen.h"
 extern int  selectfield(int * cx ,int  * cy);

#else
int  selectfield(int * cx ,int  * cy)
{
  return 0;
}
void editpolygon (Poly_gon& poly) {};
Vehicle* selectUnitFromMap() { return NULL; };
#endif

// �S GetXYSel

bool chooseWeather( int& weather )
{
#ifdef karteneditor
   vector<ASCString> entries;

   for ( int w = 0; w < cwettertypennum; ++w )
      entries.push_back ( cwettertypen[ w ] );

   int value = chooseString ( "choose operation target", entries, weather );
   if ( value < 0 )
      return false;
   else
      weather = value;

#endif
  return true;
}

bool chooseTerrain( int& terrainID )
{
#ifdef karteneditor
   selectItemID( terrainID, terrainTypeRepository );
#endif
  return true;
}

bool chooseObject( int& objectID )
{
#ifdef karteneditor
   selectItemID( objectID, objectTypeRepository );
#endif
  return true;
}

bool chooseVehicleType( int& vehicleTypeID )
{
#ifdef karteneditor
   selectItemID( vehicleTypeID, vehicleTypeRepository );
#endif
  return true;
}


void         getxy_building(int *x,int *y)
{
   SelectBuildingFromMap::CoordinateList list;
   list.push_back ( MapCoordinate( *x, *y ));
   
   SelectBuildingFromMap sbfm( list, actmap );
   sbfm.Show();
   sbfm.RunModal();

   if ( list.empty() ) {
      *x = -1;
      *y = -1;
   } else {
      *x = list.begin()->x;
      *y = list.begin()->y;
   }
}

void selectFields( FieldAddressing::Fields& fields )
{
   SelectFromMap sbfm( fields, actmap );
   sbfm.Show();
   sbfm.RunModal();
}

class UnitListFactory: public SelectionItemFactory, public sigc::trackable  {
   public:
      typedef list<const VehicleType*> UnitList;
   private:
      const UnitList& unitList;
   protected:
      UnitList::const_iterator it;
      void itemSelected( const SelectionWidget* widget, bool mouse ) 
      {
      };
   public:
      UnitListFactory( const UnitList& units )  : unitList( units )
      {
         restart();
      };
      
      void restart()
      {
         it = unitList.begin();
      }
      
      SelectionWidget* spawnNextItem( PG_Widget* parent, const PG_Point& pos )
      {
         if ( it != unitList.end() ) {
            const VehicleType* v = *(it++);
            return new VehicleTypeBaseWidget( parent, pos, parent->Width() - 15, v, actmap->getCurrentPlayer() );
         } else
            return NULL;
      };
};



class UnitAvailabilityWindow : public ItemSelectorWindow {
   private:
      bool eventKeyDown(const SDL_KeyboardEvent* key)
      {
         if ( key->keysym.sym == SDLK_RETURN ) {
            QuitModal();
            return true;
         }
         return ItemSelectorWindow::eventKeyDown( key );
      };

   public:
      UnitAvailabilityWindow ( PG_Widget *parent, const PG_Rect &r , const ASCString& title, UnitListFactory* itemFactory ) 
   : ItemSelectorWindow( parent, r, title, itemFactory ) 
      {
      };
};      



void ShowNewTechnology::showTechnology( const Technology* tech, const TechnologyPresenter::Gadgets& newGadgetsAvailable )
{
   if ( tech ) {
      ASCString text = "#fontsize=18#Research completed#fontsize=12#\n\n";

      text = "Our scientists have mastered a new technology:\n\n#fontsize=18#";

      text += tech->name + "#fontsize=12#\n\n";

      if ( tech->relatedUnitID > 0 )
         text += "#vehicletype=" + ASCString::toString(tech->relatedUnitID) + "#\n\n";

      text += tech->infotext;

      if ( newGadgetsAvailable.units.size() ) {
         text += "\n#fontsize=15#\nNew units available for production:#fontsize=12#\n";
         
         for ( std::list<const VehicleType*>::const_iterator i = newGadgetsAvailable.units.begin(); i != newGadgetsAvailable.units.end(); ++i )
            text += "#vehicletype=" + ASCString::toString((*i)->id) + "#\n";
      }
      
      ViewFormattedText tr ("Research", text, PG_Rect(-1,-1, 400,250) );
      tr.Show();
      tr.RunModal();
   }
}



int selectunit ( int unitnetworkid )
{
  SelectUnitFromMap::CoordinateList list;

  Vehicle* v = actmap->getUnit ( unitnetworkid );
  if ( v )
      list.push_back ( v->getPosition() );

  SelectUnitFromMap sufm ( list, actmap );
  sufm.Show();
  sufm.RunModal();

  if ( list.empty() )
     return 0;
  
  MapField* fld = actmap->getField( *list.begin() );
  if ( fld && fld->vehicle )
     return fld->vehicle->networkid;
  else
     return 0;
}


class PlayerMultiSelect : public ASC_PG_Dialog {
    PlayerSelector* selector;
    int& players;

    bool apply() {
        players = selector->getSelectedPlayers();
        QuitModal();
        return true;
    }
public:
    PlayerMultiSelect (GameMap* gamemap, int& bitmappedPlayers) : ASC_PG_Dialog(NULL, PG_Rect(-1, -1, 300, 330), "Select Players") , players(bitmappedPlayers) {
        selector = new PlayerSelector(this, PG_Rect(10, 30, Width()-20, Height()-90), gamemap, true, 0, 3);
        selector->setSelection(bitmappedPlayers);
        AddStandardButton("OK")->sigClick.connect( sigc::hide( sigc::mem_fun( *this, &PlayerMultiSelect::apply )));
    }
};


void playerselall( int *playerbitmap)
{
    PlayerMultiSelect pms(actmap, *playerbitmap);
    pms.Show();
    pms.RunModal();
}



bool ReinforcementSelector::mark()
{
   MapCoordinate pos = actmap->getCursor();
   if  ( !accept(pos))
      return false;

   CoordinateList::iterator i = find( coordinateList.begin(), coordinateList.end(), pos );
   if ( i == coordinateList.end() )
      coordinateList.push_back ( pos );

   cut( pos );

   showFieldMarking( coordinateList );

   updateList();
   return true;
}

void ReinforcementSelector::cut( const MapCoordinate& pos )
{
   MapField* fld = actmap->getField( pos );
   if (!fld )
      return;

   cutPositions.push_back( pos );
   
   if ( fld->vehicle ) {
      MemoryStream stream ( &buf, tnstream::appending );
      stream.writeInt( Reinforcements::ReinfVehicle );
      fld->vehicle->write ( stream );
      objectNum++;
      delete fld->vehicle;
      fld->vehicle = NULL;
   } else
      if ( fld->building ) {
         MemoryStream stream ( &buf, tnstream::appending );
         stream.writeInt( Reinforcements::ReinfBuilding );
         fld->building->write ( stream );
         objectNum++;
         delete fld->building;
         fld->building = NULL;
      }
}


bool ReinforcementSelector::isOk()
{
   for ( CoordinateList::const_iterator i = coordinateList.begin(); i !=  coordinateList.end(); ++i  ) 
      if ( find( cutPositions.begin(), cutPositions.end(), *i ) == cutPositions.end() )
         cut( *i );
   return true;  
}


bool BitMapEditor::ok()
{
   reference = 0;
   propertyEditor->Apply();
   for ( int i = 0; i < bitCount; ++i ) 
      if ( values[i] )
         reference |= 1 << i;

   QuitModal();
   return true;
}

BitMapEditor::BitMapEditor( BitType& value, const ASCString& title, const vector<ASCString>& names ) : ASC_PG_Dialog(NULL, PG_Rect(-1,-1,300,500), title ), reference(value)
{
   propertyEditor = new ASC_PropertyEditor( this, PG_Rect( 10, GetTitlebarHeight(), Width() - 20, Height() - GetTitlebarHeight() - 50 ), "PropertyEditor", 70 );


   bitCount = names.size();
   int counter = 0;
   for ( vector<ASCString>::const_iterator i = names.begin(); i != names.end(); ++i ) {
      bool v = value & (1 << counter );
      values[counter] = v;
      new PG_PropertyField_Checkbox<bool>( propertyEditor, *i, &(values[counter]) );
      ++counter;
   }

   PG_Button* ok = new PG_Button( this, PG_Rect( Width() - 100, Height() - 40, 90, 30), "OK" );
   ok->sigClick.connect( sigc::hide( sigc::mem_fun( *this, &BitMapEditor::ok )));
}

