/*
     This file is part of Advanced Strategic Command; http://www.asc-hq.de
     Copyright (C) 1994-1999  Martin Bickel  and  Marc Schellenberger
 
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

#include <sstream>
#include <pgimage.h>
#include <pgpropertyeditor.h>
#include <pgpropertyfield_integer.h>
#include <pgpropertyfield_string.h>
#include <pgpropertyfield_checkbox.h>
#include <pgpropertyfield_button.h>
#include <pgpropertyfield_intdropdown.h>

#include "newmap.h"
#include "editmapparam.h"
#include "../gamemap.h"
#include "../paradialog.h"
#include "../itemrepository.h"
#include "../edselfnt.h"
#include "../edgen.h"


class NewMap: public ASC_PG_Dialog {
   private:
      PG_PropertyEditor* properties;
      GameMap* gamemap;
      bool create;
      int xsize,ysize;
      bool random;
      PG_Button* terrainButton;
      int terrainid;
      bool success;


      bool selectTerrain() {
         selectItemID( terrainid, terrainTypeRepository );
         updateButton();
         return true;
      }

      void updateButton()
      {
         TerrainType::Weather* w = terrainTypeRepository.getObject_byID( terrainid)->weather[0];
         if ( w )
            terrainButton->SetIcon( w->image.getBaseSurface() );
         else
            terrainButton->SetIcon( NULL );
      }
      
      
      bool ok()
      {
         if ( !properties->Apply() )
            return false;

        
         if ( ysize & 1 ) {
            warningMessage( "Height must be an even number!" );
            return false;
         }
         

         if ( create ) {
            TerrainType::Weather* w = terrainTypeRepository.getObject_byID( terrainid)->weather[0];
            if ( !w ) {
               warningMessage( "please choose a terrain!" );
               return false;
            }
            
            gamemap->allocateFields( xsize, ysize, w );

            if ( random)
               mapgenerator();
            
         }
         success = true;
         
         QuitModal();
         
         return true;
      }
      
      bool cancel()
      {
         if ( create ) {
            delete gamemap;
            gamemap = NULL;
         }
            
         QuitModal();
         return true;
      }
      
   public:
      NewMap( GameMap* map ) : ASC_PG_Dialog( NULL, PG_Rect( -1, -1, 500, 400 ), "Map Properties" ), terrainid( 2998 ), success(false)
      {
         if ( map ) {
            create = false;
            gamemap = map;
         } else {
            create = true;
            gamemap = new GameMap;
         }

         int ypos = 40;

         xsize = 10;
         ysize = 20;
         random = false;
         
         properties = new ASC_PropertyEditor( this, PG_Rect( 20 , ypos, Width()-20, Height() - ypos - 60  ));
         new PG_PropertyField_String<ASCString>( properties, "Title", &gamemap->maptitle );
         if ( create ) {
            new PG_PropertyField_Integer<int>( properties, "Width", &xsize );
            new PG_PropertyField_Integer<int>( properties, "Height", &ysize );
            new PG_PropertyField_Checkbox<bool>( properties, "Random", &ysize );
         }
         
         new PG_PropertyField_Checkbox<bool>( properties, "Campaign", &gamemap->campaign.avail );
         new PG_PropertyField_Integer<int>( properties, "Map ID", &gamemap->campaign.id );
         new PG_PropertyField_Checkbox<bool>( properties, "Direct Access", &gamemap->campaign.directaccess );
         (new PG_PropertyField_String<ASCString>( properties, "Password", &gamemap->codeWord ))->SetPassHidden('*');
         terrainButton = (new PG_PropertyField_Button( properties, "Terrain", "", 50 ))->GetButton();
         terrainButton->sigClick.connect( SigC::slot( *this, &NewMap::selectTerrain ));

         (new PG_PropertyField_Integer<int>( properties, "Wind Speed", &gamemap->weather.windSpeed ))->SetRange(0,255);

         static const char* directionNames[7] = { "South", "SouthWest", "NorthWest", "North", "NorthEast", "SouthEast", NULL };
         
         new PG_PropertyField_IntDropDown<int>( properties, "Wind Direction", &gamemap->weather.windDirection, directionNames );
         
         updateButton();
         
         ypos += 200;

         StandardButtonDirection ( ASC_PG_Dialog::Horizontal );

         AddStandardButton ( "Cancel" )->sigClick.connect( SigC::slot( *this, &NewMap::cancel ));
         AddStandardButton ( "OK" )->sigClick.connect( SigC::slot( *this, &NewMap::ok ));
         
      };

      GameMap* GetResult()
      {
         if ( !success )
            return NULL;

         GameMap* res = gamemap;
         gamemap = NULL;
         return res;
      }

      ~NewMap()
      {
         if ( create ) {
            delete gamemap;
            gamemap = NULL;
         }
      }
};




GameMap* createNewMap()
{
   NewMap nm(NULL);
   nm.Show();
   nm.RunModal();
   return nm.GetResult();
}

void editMap( GameMap* map )
{
   NewMap nm(map);
   nm.Show();
   nm.RunModal();
}
