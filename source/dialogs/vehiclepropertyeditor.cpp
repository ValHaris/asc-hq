
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

#include <pgpropertyeditor.h>
#include <pgpropertyfield_integer.h>
#include <pgpropertyfield_intdropdown.h>
#include <pgpropertyfield_string.h>
#include <pgpropertyfield_checkbox.h>

#include "../gamemap.h"
#include "../paradialog.h"
#include "../edmisc.h"

#include "vehiclepropertyeditor.h"

static const char* cdirnames[7] = { "north", "north-east", "south-east", "south", "south-west", "north-west", NULL };

class VehiclePropertyEditor : public ASC_PG_Dialog {
   private:
      PG_PropertyEditor* propertyEditor;
      Vehicle* veh;
      bool reactionFire;
      int owner;
      ASCString playerNames[8];
      const char* cplayerNames[9];
      int experienceOffensive;
      int experienceDefensive;
      Resources tank;


      const char* cheightNames[choehenstufennum+1];
      int heightMapping[choehenstufennum];
      int unitHeight;

      bool getReactionfireStatus() {
         return veh->reactionfire.getStatus() != Vehicle::ReactionFire::off;
      }

      bool ok()
      {
         if ( propertyEditor->Apply() ) {

            if ( reactionFire != getReactionfireStatus() ) {
                if ( reactionFire )
                    veh->reactionfire.enable();
                else
                    veh->reactionfire.disable();
                mapsaved = false;
            }

            if ( owner != veh->getOwner() ) {
                veh->convert(owner, true);
                mapsaved = false;
            }

            if ( heightMapping[unitHeight] != getFirstBit( veh->height )) {
                veh->height = 1 << heightMapping[unitHeight];
                veh->resetMovement();
                mapsaved = false;
            }

            if ( experienceOffensive != veh->getExperience_offensive() ) {
                veh->setExperience_offensive( experienceOffensive );
                mapsaved = false;
            }

            if ( experienceDefensive != veh->getExperience_defensive() ) {
                veh->setExperience_defensive( experienceDefensive );
                mapsaved = false;
            }

            if ( tank.material != veh->getTank().material ) {
                veh->putResource( tank.material-veh->getTank().material, 1, false, 1, veh->getOwner() );
                mapsaved = false;
            }

            if ( tank.fuel != veh->getTank().fuel ) {
                veh->putResource( tank.fuel-veh->getTank().fuel, 2, false, 1, veh->getOwner() );
                mapsaved = false;
            }

            quitModalLoop(0);

            return true;
         } else
            return false;
      }

   public:
      VehiclePropertyEditor( PG_Widget* parent, Vehicle* veh ) : ASC_PG_Dialog( parent, PG_Rect( 50, 50, 500, 550 ), "Edit Unit Properties")
      {
         this->veh = veh;
         propertyEditor = new ASC_PropertyEditor( this, PG_Rect( 10, GetTitlebarHeight(), Width() - 20, Height() - GetTitlebarHeight() - 50 ), "PropertyEditor", 70 );

         new PG_PropertyField_String<ASCString>( propertyEditor, "Name", &veh->name );
         new PG_PropertyField_Integer<int>( propertyEditor , "Damage", &veh->damage );

         experienceOffensive = veh->getExperience_offensive();
         (new PG_PropertyField_Integer<int>( propertyEditor , "Experience offensive", &experienceOffensive ))
                 ->SetRange(0,1000);

         experienceDefensive = veh->getExperience_defensive();
         (new PG_PropertyField_Integer<int>( propertyEditor , "Experience defensive", &experienceDefensive ))
                 ->SetRange(0,1000);

         reactionFire = getReactionfireStatus();
         new PG_PropertyField_Checkbox<bool>( propertyEditor, "ReactionFire", &reactionFire );

         tank = veh->getTank();

         if ( veh->getStorageCapacity().material )
            ( new PG_PropertyField_Integer<int>( propertyEditor , "Material", &tank.material ))
                 ->SetRange(0, veh->getStorageCapacity().material);

         if ( veh->getStorageCapacity().fuel )
            ( new PG_PropertyField_Integer<int>( propertyEditor , "Fuel", &tank.fuel ))
                 ->SetRange( 0, veh->getStorageCapacity().fuel);

         for ( int i = 0; i < 8; ++i) {
             playerNames[i] = ASCString("(") + ASCString::toString(i) + ") " + veh->getMap()->player[i].getName();
             cplayerNames[i] = playerNames[i].c_str();
         }
         cplayerNames[8] = NULL;
         owner = veh->getOwner();
         new PG_PropertyField_IntDropDown<int>( propertyEditor, "Owner", &owner, cplayerNames  );

         for ( int w =0; w < veh->typ->weapons.count; w++)
             (new PG_PropertyField_Integer<int>( propertyEditor , "Ammo Weapon " + ASCString::toString(w) + " " +  veh->typ->weapons.weapon[w].getName() , &veh->ammo[w] ))
                     ->SetRange( 0, veh->typ->weapons.weapon[w].count );


         int count = 0;
         for ( int h = 0; h < choehenstufennum; ++h ) {
             if ( veh->typ->height & (1 << h )) {
                 cheightNames[count] = choehenstufen[h];
                 heightMapping[count] = h;
                 if ( h == getFirstBit( veh->height )) {
                     unitHeight = count;
                 }
                 ++count;
             }
         }
         cheightNames[count] = NULL;
         new PG_PropertyField_IntDropDown<int>( propertyEditor, "Level of height", &unitHeight, cheightNames  );

         new PG_PropertyField_IntDropDown<Uint8>( propertyEditor, "Orientation", &veh->direction, cdirnames );

         PG_Button* ok = new PG_Button( this, PG_Rect( Width() - 100, Height() - 40, 90, 30), "OK" );
         ok->sigClick.connect( SigC::slot( *this, &VehiclePropertyEditor::ok ));
      };

};


void changeUnitPropertyDialog(Vehicle* ae)
{
    if ( !ae )
        return;

    VehiclePropertyEditor units ( NULL, ae );
    units.Show();
    units.RunModal();
}
