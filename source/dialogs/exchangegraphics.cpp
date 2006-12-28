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


#include "../paradialog.h"
#include "fileselector.h"
#include "../textfile_evaluation.h"
#include "../spfst.h"
#include "../graphicset.h"
#include "../widgets/dropdownselector.h"
#include "exchangegraphics.h"
#include "../widgets/textrenderer.h"
#include "../fieldimageloader.h"

template<typename T, typename U>
void eraseElement( T& t, const U& u)
{
   if ( t.find( u ) != t.end() )
      t.erase( t.find(u) );
}


class ExchangeGraphics: public ASC_PG_Dialog 
{
      static const int dlg_width = 200;

      typedef map<int, Surface> OrgTerrainGFX;
      static OrgTerrainGFX orgTerrainGFX;

      typedef map<const LoadableItemType*, Surface> OrgTerrainSurf;
      static OrgTerrainSurf orgTerrainSurf;

      typedef map<int,ASCString> NewGFX;
      static NewGFX newGFX;

      typedef map<ASCString,ASCString> NewSurfaces;
      static NewSurfaces newSurfaces;

      PG_LineEdit* filename;
      PG_LineEdit* imageNum;
      PG_Label* terrain;
      PG_Label* object;

      DropDownSelector* selectedType;

      bool fileSelect()
      {
         ASCString fn = selectFile("*.png;*.pcx", true );
         if ( !fn.empty() )
            filename->SetText( fn );
         return true;
      }

      ASCString getIdentifier( const TerrainType::Weather* w)
      {
         return "T: " + w->terraintype->filename;
      }

      ASCString getIdentifier( const ObjectType* o)
      {
         return "O: " + o->filename;
      }


      bool apply()
      { 
         tfield* fld = actmap->getField( actmap->getCursor() );
         if ( fld ) {
            try {
               if ( selectedType->GetSelectedItemIndex() == 0 ) {
                  if ( fld->typ->bi_pict >= 0 ) {
                     if ( orgTerrainGFX.find( fld->typ->bi_pict ) == orgTerrainGFX.end() )
                        orgTerrainGFX[fld->typ->bi_pict] = GraphicSetManager::Instance().getPic( fld->typ->bi_pict );

                     GraphicSetManager::Instance().setPic( fld->typ->bi_pict, loadASCFieldImage ( filename->GetText() ) );

                     newGFX[fld->typ->bi_pict] = filename->GetText();
                  } else {
                     if ( orgTerrainSurf.find( fld->typ ) == orgTerrainSurf.end() )
                        orgTerrainSurf[fld->typ] = fld->typ->image;

                     fld->typ->image = loadASCFieldImage ( filename->GetText() );

                     newSurfaces[getIdentifier(fld->typ)] = filename->GetText();
                  }
               }
            }
            catch ( ... ) {
               warning( "operation failed");
            }
         }
         repaintMap();

         return true;
      }

      bool close()
      {
         Hide();
         return true;
      }

      bool restore()
      {
         tfield* fld = actmap->getField( actmap->getCursor() );
         if ( fld ) {
            if ( selectedType->GetSelectedItemIndex() == 0 ) {
               if ( fld->typ->bi_pict >= 0 ) {
                  GraphicSetManager::Instance().setPic( fld->typ->bi_pict, orgTerrainGFX[fld->typ->bi_pict] );
                  eraseElement( newGFX, fld->typ->bi_pict );
               } else {
                  fld->typ->image = orgTerrainSurf[fld->typ];
                  eraseElement( newSurfaces, getIdentifier(fld->typ) );
               }
            }
         }
         repaintMap();
         return true;
      }

      bool summary()
      {
         ASCString s;
         for ( NewGFX::iterator i = newGFX.begin(); i != newGFX.end(); ++i )
            s += "GFX" + ASCString::toString(i->first) + " -> " + i->second + "\n";

         for ( NewSurfaces::iterator i = newSurfaces.begin(); i != newSurfaces.end(); ++i )
            s += i->first + " -> " + i->second + "\n";


         ViewFormattedText vft("Graphics replacement summary", s, PG_Rect( -1, -1, 500, 400 ));
         vft.Show();
         vft.RunModal();
         return true;
      }

      void newCursorPos()
      {
         tfield* fld = actmap->getField( actmap->getCursor() );
         if ( !fld )
            return;

         if ( terrain ) {
            ASCString s = "T: ID=" + ASCString::toString( fld->typ->terraintype->id );
            if ( fld->typ->bi_pict >= 0 )
               s += " GFX=" + ASCString::toString( fld->typ->bi_pict );
            terrain->SetText( s );
         }

         if ( object ) {
            if ( fld->objects.size() ) {
               ASCString s = "O: ID=" + ASCString::toString( fld->objects.front().typ->id );
               if ( fld->objects.front().typ->weatherPicture[0].bi3pic[0] >= 0 )
                  s += " GFX=" + ASCString::toString( fld->objects.front().typ->weatherPicture[0].bi3pic[0] );
                  
               object->SetText( s );
            } else
               object->SetText("-");
         }

      }


   public:
      ExchangeGraphics() : ASC_PG_Dialog( NULL, PG_Rect( PG_Application::GetScreenWidth() - dlg_width, -1, dlg_width, 370 ), "Exchange Graphics"), filename(NULL), imageNum(NULL),terrain(NULL), object(NULL), selectedType(NULL)
      {

         selectedType = new DropDownSelector( this, PG_Rect( 10, 30, 180, 25 ));
         selectedType->AddItem( "Terrain" );
         selectedType->AddItem( "Object" );
         selectedType->SelectItem( 0 );

         (new PG_Button( this, PG_Rect( 10, 100, 180, 30), "Select Filename"))->sigClick.connect( SigC::slot( *this, &ExchangeGraphics::fileSelect ));

         terrain = new PG_Label( this, PG_Rect( 10, 140, 180, 25));
         object  = new PG_Label( this, PG_Rect( 10, 170, 180, 25));


         (new PG_Button( this, PG_Rect( 10, 200, 180, 30), "Apply"))->sigClick.connect( SigC::slot( *this, &ExchangeGraphics::apply ));
         (new PG_Button( this, PG_Rect( 10, 240, 180, 30), "Restore Original"))->sigClick.connect( SigC::slot( *this, &ExchangeGraphics::restore ));
         (new PG_Button( this, PG_Rect( 10, 280, 180, 30), "Replacement Summary"))->sigClick.connect( SigC::slot( *this, &ExchangeGraphics::summary ));
         (new PG_Button( this, PG_Rect( 10, 320, 180, 30), "Close"))->sigClick.connect( SigC::slot( *this, &ExchangeGraphics::close ));

         cursorMoved.connect( SigC::slot( *this, &ExchangeGraphics::newCursorPos ));
         updateFieldInfo.connect( SigC::slot( *this, &ExchangeGraphics::newCursorPos ));
   
         newCursorPos();

         filename = new PG_LineEdit( this, PG_Rect( 10, 65, 180, 30 ));

      }
};

ExchangeGraphics::OrgTerrainGFX  ExchangeGraphics::orgTerrainGFX;
ExchangeGraphics::OrgTerrainSurf ExchangeGraphics::orgTerrainSurf;
ExchangeGraphics::NewGFX ExchangeGraphics::newGFX;
ExchangeGraphics::NewSurfaces ExchangeGraphics::newSurfaces;



void exchangeGraphics()
{
   static ExchangeGraphics* eg = NULL;
   if  ( !eg )
      eg = new ExchangeGraphics();
   eg->Show();
}
