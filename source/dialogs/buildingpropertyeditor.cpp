
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

#include "buildingpropertyeditor.h"


class BuildingPropertyEditor : public ASC_PG_Dialog {
   private:
      PG_PropertyEditor* propertyEditor;
      Building* bld;
      int owner;
      ASCString playerNames[8];
      const char* cplayerNames[9];
      int experienceOffensive;
      int experienceDefensive;
      Resources tank;

      bool ok()
      {
         if ( propertyEditor->Apply() ) {
            mapsaved = false;

            if ( owner != bld->getOwner() ) {
                bld->convert(owner, true);
            }

            quitModalLoop(0);

            bld->researchpoints = min(bld->researchpoints, bld->maxresearchpoints);

            return true;
         } else
            return false;
      }

   public:
      BuildingPropertyEditor( PG_Widget* parent, Building* bld) : ASC_PG_Dialog( parent, PG_Rect( 50, 50, 500, 550 ), "Edit Unit Properties"), bld(bld)
      {
         propertyEditor = new ASC_PropertyEditor( this, PG_Rect( 10, GetTitlebarHeight(), Width() - 20, Height() - GetTitlebarHeight() - 50 ), "PropertyEditor", 70 );

         new PG_PropertyField_String<ASCString>( propertyEditor, "Name", &bld->name );
         new PG_PropertyField_Integer<int>( propertyEditor , "Damage", &bld->damage );
         new PG_PropertyField_Checkbox( propertyEditor , "Visible", &bld->visible );

         (new PG_PropertyField_Integer<int>( propertyEditor , "Energy stored", &bld->actstorage.energy ))->SetRange(0, bld->getStorageCapacity().energy);
         (new PG_PropertyField_Integer<int>( propertyEditor , "Material stored", &bld->actstorage.material ))->SetRange(0, bld->getStorageCapacity().material);
         (new PG_PropertyField_Integer<int>( propertyEditor , "Fuel stored", &bld->actstorage.fuel ))->SetRange(0, bld->getStorageCapacity().fuel);

         new PG_PropertyField_Integer<int>( propertyEditor , "BI Energy plus", &bld->bi_resourceplus.energy );
         new PG_PropertyField_Integer<int>( propertyEditor , "BI Material plus", &bld->bi_resourceplus.material );
         new PG_PropertyField_Integer<int>( propertyEditor , "BI Fuel plus", &bld->bi_resourceplus.fuel );

         if ( bld->typ->hasFunction( ContainerBaseType::MatterConverter  ) ||
                 bld->typ->hasFunction( ContainerBaseType::SolarPowerPlant  ) ||
                 bld->typ->hasFunction( ContainerBaseType::WindPowerPlant  ) ||
                 bld->typ->hasFunction( ContainerBaseType::MiningStation  ) ||
                 bld->typ->hasFunction( ContainerBaseType::ResourceSink  )) {

             (new PG_PropertyField_Integer<int>( propertyEditor , "Energy max plus", &bld->maxplus.energy ))->SetRange(0, bld->typ->maxplus.energy);
             (new PG_PropertyField_Integer<int>( propertyEditor , "Material max plus", &bld->maxplus.material ))->SetRange(0, bld->typ->maxplus.material);
             (new PG_PropertyField_Integer<int>( propertyEditor , "Fuel max plus", &bld->maxplus.fuel ))->SetRange(0, bld->typ->maxplus.fuel);

         }

         if ( bld->typ->hasFunction(ContainerBaseType::Research)) {
             (new PG_PropertyField_Integer<int>( propertyEditor , "Research output (points)", &bld->researchpoints ))->SetRange(0, bld->maxresearchpoints);
             (new PG_PropertyField_Integer<int>( propertyEditor , "Max. Research output (points)", &bld->maxresearchpoints ))->SetRange(0, bld->typ->maxresearchpoints);
         }


         for ( int i = 0; i < 8; ++i) {
             playerNames[i] = ASCString("(") + ASCString::toString(i) + ") " + bld->getMap()->player[i].getName();
             cplayerNames[i] = playerNames[i].c_str();
         }
         cplayerNames[8] = NULL;
         owner = bld->getOwner();
         new PG_PropertyField_IntDropDown<int>( propertyEditor, "Owner", &owner, cplayerNames  );

         for ( int w =0; w < waffenanzahl; w++)
             (new PG_PropertyField_Integer<int>( propertyEditor , "Ammo for " + ASCString(cwaffentypen[w]), &bld->ammo[w] ))
                     ->SetRange( 0, 10000 );

         PG_Button* ok = new PG_Button( this, PG_Rect( Width() - 100, Height() - 40, 90, 30), "OK" );
         ok->sigClick.connect( sigc::hide( sigc::mem_fun( *this, &BuildingPropertyEditor::ok )));
      };

};


void changeBuildingPropertyDialog(Building* bld)
{
    if ( !bld )
        return;

    BuildingPropertyEditor propEd ( NULL, bld);
    propEd.Show();
    propEd.RunModal();
}
