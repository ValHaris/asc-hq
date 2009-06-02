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

#include <boost/regex.hpp>

#include "../paradialog.h"
#include "fileselector.h"
#include "../textfile_evaluation.h"
#include "../spfst.h"
#include "../spfst-legacy.h"
#include "../graphicset.h"
#include "../widgets/dropdownselector.h"
#include "exchangegraphics.h"
#include "../widgets/textrenderer.h"
#include "../fieldimageloader.h"
#include "../itemrepository.h"

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

      void setnewgfx ( int id, const ASCString& filename)
      {
         if ( orgTerrainGFX.find( id ) == orgTerrainGFX.end() )
            orgTerrainGFX[id] = GraphicSetManager::Instance().getPic( id );

         GraphicSetManager::Instance().setPic( id, loadASCFieldImage ( filename ) );
         newGFX[id] = filename;
      }

      void setnewimage( TerrainType::Weather* trr, const ASCString& filename )
      {
         if ( orgTerrainSurf.find( trr ) == orgTerrainSurf.end() )
            orgTerrainSurf[trr] = trr->image;

         trr->image = loadASCFieldImage ( filename );
         newSurfaces[getIdentifier(trr)] = filename;
      }

      void setnewimage( ObjectType* obj, const ASCString& filename )
      {
         obj->weatherPicture[0].images = loadASCFieldImageArray(filename, obj->weatherPicture[0].images.size() );
         obj->displayMethod = 0;
         /*
         if ( orgTerrainSurf.find( obj ) == orgTerrainSurf.end() )
            orgTerrainSurf[obj] = trr->image;

         trr->image = loadASCFieldImage ( filename );
         newSurfaces[getIdentifier(trr)] = filename;
         */
      }


      bool apply()
      { 
         tfield* fld = actmap->getField( actmap->getCursor() );
         if ( fld ) {
            try {
               if ( selectedType->GetSelectedItemIndex() == 0 ) {
                  if ( fld->typ->bi_pict >= 0 ) {
                     setnewgfx( fld->typ->bi_pict, filename->GetText() );
                  } else {
                     setnewimage( fld->typ, filename->GetText() );
                  }
               }
               if ( selectedType->GetSelectedItemIndex() == 1 ) {
                  if ( fld->objects.size() )
                     setnewimage( objectTypeRepository.getObject_byID( fld->objects[0].typ->id ), filename->GetText() );
               }
            }
            catch ( ... ) {
               warningMessage( "operation failed");
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
                  if ( orgTerrainGFX.find(fld->typ->bi_pict) != orgTerrainGFX.end() ) {
                     GraphicSetManager::Instance().setPic( fld->typ->bi_pict, orgTerrainGFX[fld->typ->bi_pict] );
                     eraseElement( newGFX, fld->typ->bi_pict );
                  }
               } else {
                  if ( orgTerrainSurf.find(fld->typ) != orgTerrainSurf.end() ) {
                     fld->typ->image = orgTerrainSurf[fld->typ];
                     eraseElement( newSurfaces, getIdentifier(fld->typ) );
                  }
               }
            }
         }
         repaintMap();
         return true;
      }

      bool readFile()
      {
         ASCString filename = selectFile("*.txt", true );
         if ( filename.empty() )
            return false;

         try {
            tnfilestream stream ( filename, tnstream::reading );
            bool finished = false;
            while ( !finished ) {

               ASCString line;
               finished = !stream.readTextString( line );

               boost::smatch what;

               static boost::regex gfx( "^GFX+(\\d+) -> (\\S+)");
               if( boost::regex_match( line, what, gfx)) {
                  ASCString ids ( what[1] );
                  int id = atoi( ids.c_str() );

                  ASCString name ( what[2] );
                  setnewgfx( id, name );
               }

               static boost::regex trr( "^(T: \\S+) -> (\\S+)");
               if( boost::regex_match( line, what, trr)) {
                  ASCString file ( what[1] );

                  ASCString name ( what[2] );

                  for ( int i = 0; i < terrainTypeRepository.getNum(); ++i ) {
                     TerrainType* t = terrainTypeRepository.getObject_byPos(i);
                     for ( int w = 0; w < cwettertypennum; ++w)
                        if ( t->weather[w] )
                           if ( getIdentifier(t->weather[w]) == file )
                              setnewimage( t->weather[w], name );
                  }
               }
            }
         }
         catch(...) {
            errorMessage("an error occured");
         }
         repaintMap();
         return true;
      }

      bool snow()
      {
         tfield* fld = actmap->getField( actmap->getCursor() );
         if ( fld ) {
            if ( selectedType->GetSelectedItemIndex() == 0 ) {
               if ( fld->typ->bi_pict < 0 )
                  snowify( fld->typ->image );
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

            if ( newGFX.find( fld->typ->bi_pict ) != newGFX.end() || newSurfaces.find( getIdentifier(fld->typ) ) != newSurfaces.end() )
               s += " (R) ";

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
      ExchangeGraphics() : ASC_PG_Dialog( NULL, PG_Rect( PG_Application::GetScreenWidth() - dlg_width, -1, dlg_width, 450 ), "Exchange Graphics"), filename(NULL), imageNum(NULL),terrain(NULL), object(NULL), selectedType(NULL)
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
         (new PG_Button( this, PG_Rect( 10, 320, 180, 30), "Load from file"))->sigClick.connect( SigC::slot( *this, &ExchangeGraphics::readFile ));
         (new PG_Button( this, PG_Rect( 10, 360, 180, 30), "Close"))->sigClick.connect( SigC::slot( *this, &ExchangeGraphics::close ));
         (new PG_Button( this, PG_Rect( 10, 400, 180, 30), "Snow"))->sigClick.connect( SigC::slot( *this, &ExchangeGraphics::snow ));

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
