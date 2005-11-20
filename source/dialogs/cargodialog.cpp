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

#include "cargodialog.h"
#include "../containerbase.h"
#include "../paradialog.h"
#include "../messaginghub.h"
#include "../iconrepository.h"
#include "../vehicle.h"


class HighLightingManager {
      int marked;
   public:
      HighLightingManager() : marked(-1) {};
      int getMark() { return marked; };
      SigC::Signal2<void,int,int> markChanged;
      void setNew(int pos ) {
         int old = marked;
         marked = pos;
         markChanged(old,pos);
      };
};

typedef vector<Vehicle*> StorageVector;

class StoringPosition : public PG_Widget {
      static Surface clippingSurface;
      HighLightingManager& highlight;
      StorageVector& storage;
      int num;
      bool regular;
   protected:
      void markChanged(int old, int mark)
      {
         if ( num == old || num == mark )
            Update();
      }   

      bool eventMouseButtonDown (const SDL_MouseButtonEvent *button)      
      {
         if ( button->type == SDL_MOUSEBUTTONDOWN && button->button == SDL_BUTTON_LEFT ) {
            highlight.setNew( num );
            return true;
         }   
         return false;
      }
            
   public:
   
      static const int spWidth = 64;
      static const int spHeight = 64;
      
      StoringPosition( PG_Widget *parent, const PG_Point &pos, HighLightingManager& highLightingManager, StorageVector& storageVector, int number, bool regularPosition  ) 
         : PG_Widget ( parent, PG_Rect( pos.x, pos.y, spWidth, spHeight)), highlight( highLightingManager ), storage( storageVector), num(number), regular(regularPosition)
      {
         if ( !clippingSurface.valid() )
            clippingSurface = Surface::createSurface( spWidth + 10, spHeight + 10, 32, 0 );

         highlight.markChanged.connect( SigC::slot( *this, &StoringPosition::markChanged ));
      }
      
      
      
      
      void eventBlit (SDL_Surface *surface, const PG_Rect &src, const PG_Rect &dst)     
      {
         clippingSurface.Fill(0);   

         ASCString background = "hexfield-bld-";
         background += regular ? "1" : "2";
         if (  num == highlight.getMark() )
            background += "h";
         background += ".png";
            
         Surface& icon = IconRepository::getIcon( background );

         MegaBlitter<4,4,ColorTransform_None, ColorMerger_AlphaOverwrite> blitter;
         blitter.blit( icon, clippingSurface, SPoint(0,0));
                  
         if ( num < storage.size() && storage[num] ) {
            int ypos;
            if ( num == highlight.getMark() )
               ypos = 0;
            else 
               ypos = 1;
                  
            storage[num]->typ->paint( clippingSurface, SPoint(0,ypos), storage[num]->getOwner() );
         }   

         PG_Draw::BlitSurface( clippingSurface.getBaseSurface(), src, PG_Application::GetScreen(), dst);
      }
};

Surface StoringPosition::clippingSurface;





const int subWindowNum = 11;
static const char* subWindowName[subWindowNum] = { "ammotransfer", "ammoproduction", "info", "cargoinfo", "conventionalpower", "mining", "netcontrol", "research", "resourceinfo", "solarpower", "windpower" };

class SubWinButton : public PG_Button {
       public:
          static const int buttonwidth = 47;
          static const int buttonheight = 23;
          
          SubWinButton( PG_Widget *parent, const SPoint& pos, int subWindow ) : PG_Button( parent, PG_Rect( pos.x, pos.y, buttonwidth, buttonheight ), "", -1, "SubWinButton")
          {
            SetBackground( PRESSED, IconRepository::getIcon("cargo-buttonpressed.png").getBaseSurface() );
            SetBackground( HIGHLITED, IconRepository::getIcon("cargo-buttonhighlighted.png").getBaseSurface() );
            SetBackground( UNPRESSED, IconRepository::getIcon("cargo-buttonunpressed.png").getBaseSurface() );
            SetBorderSize(0,0,0);
            SetIcon( IconRepository::getIcon(ASCString("cargo-") + subWindowName[subWindow] + ".png" ).getBaseSurface() );
          };
};


class CargoDialog;

class SubWindow {
   public:
      virtual bool available( CargoDialog* cd ) = 0;
      virtual void registerSubwindow( CargoDialog* cd ) {};
};




class CargoDialog : public Panel {
        ContainerBase* container;
        bool setupOK;
        int unitColumnCount;
        
        vector<StoringPosition*> storingPositionVector;
        
        HighLightingManager unitHighLight;

        deallocating_vector<SubWindow*> subwindows;

        StorageVector loadedUnits;
        
        void moveSelection( int delta )
        {
           int newpos = unitHighLight.getMark() + delta;
           
           if ( newpos < 0 )
               newpos = 0;
               
           if ( newpos >= storingPositionVector.size() )
               newpos = storingPositionVector.size() -1 ;
               
           if ( newpos != unitHighLight.getMark() )
               unitHighLight.setNew( newpos );
        }
        
        bool eventKeyDown(const SDL_KeyboardEvent* key)
        {
           if ( key->keysym.sym == SDLK_ESCAPE ) {
              QuitModal();
              return true;
           }
            if ( key->keysym.sym == SDLK_RIGHT )  {
               moveSelection(1);
               return true;
            }
            if ( key->keysym.sym == SDLK_LEFT )  {
               moveSelection(-1);
               return true;
            }
            if ( key->keysym.sym == SDLK_UP )  {
               moveSelection(-unitColumnCount);
               return true;
            }
            if ( key->keysym.sym == SDLK_DOWN )  {
               moveSelection(unitColumnCount);
               return true;
            }
           return false;
        };



         void registerSpecialDisplay( const ASCString& name )
         {
            SpecialDisplayWidget* sdw = dynamic_cast<SpecialDisplayWidget*>( FindChild( name, true ) );
            if ( sdw )
               sdw->display.connect( SigC::slot( *this, &CargoDialog::painter ));
         };

         void registerSpecialInput( const ASCString& name )
         {
            SpecialInputWidget* siw = dynamic_cast<SpecialInputWidget*>( FindChild( name, true ) );
            if ( siw )
               siw->sigMouseButtonDown.connect( SigC::slot( *this, &CargoDialog::onClick ));
         };

         bool onClick ( PG_MessageObject* obj, const SDL_MouseButtonEvent* event ) {
            PG_Widget* w = dynamic_cast<PG_Widget*>(obj);
            if ( w ) {
               // click( w->GetName() );
               return true;
            } 
            return false;
         };

         void painter ( const PG_Rect &src, const ASCString& name, const PG_Rect &dst)
         {
            Surface screen = Surface::Wrap( PG_Application::GetScreen() );

            if ( name == "unitpad_unitsymbol" ) {
               // if ( vt )
               //   vt->paint( screen, SPoint( dst.x, dst.y ), veh ? veh->getOwner() : 0 );
            
            }
         };

         bool activate_i( int pane ) {
            activate( subWindowName[pane] );
            return true;
         }
            
         void activate( const ASCString& pane ) {
            PG_Application::SetBulkMode();
            for ( int i = 0; i < subWindowNum; ++i )
                if ( ASCString( subWindowName[i]) != pane )
                   hide( subWindowName[i] );
                   
            for ( int i = 0; i < subWindowNum; ++i )
                if ( ASCString( subWindowName[i]) == pane )
                   show( subWindowName[i] );
            PG_Application::SetBulkMode(false);
            Update();
         };
         
         void updateResourceDisplay()
         {
            setLabelText( "energyavail", container->getResource(maxint, 0, true ) );
            setLabelText( "materialavail", container->getResource(maxint, 1, true ) );
            setLabelText( "fuelavail", container->getResource(maxint, 2, true ) );
         }

        
        
         void checkStoringPosition( int oldpos, int newpos )
         {
            PG_ScrollWidget* unitScrollArea = dynamic_cast<PG_ScrollWidget*>(FindChild( "UnitScrollArea", true ));
            if ( unitScrollArea )
               if ( newpos < storingPositionVector.size() && newpos >= 0 )
                  unitScrollArea->ScrollToWidget( storingPositionVector[newpos] );
         }   
         
         Vehicle* getMarkedUnit()
         {
            int pos = unitHighLight.getMark();
            if ( pos < 0 || pos >= loadedUnits.size() )
               return NULL;
            else
               return loadedUnits[pos];
         }
         
     public:
        
        
        CargoDialog (PG_Widget *parent, ContainerBase* cb ) 
           : Panel( parent, PG_Rect::null, "cargodialog", false ), container(cb), setupOK(false), unitColumnCount(0)  {
               sigClose.connect( SigC::slot( *this, &CargoDialog::QuitModal ));

               try {
                  if ( !setup() )
                     return;
               }
               catch ( ParsingError err ) {
                  errorMessage( err.getMessage() );
                  return;
               }
               catch ( ... ) {
                  errorMessage( "unknown exception" );
                  return;
               }

               unitHighLight.markChanged.connect( SigC::slot( *this, &CargoDialog::checkStoringPosition ));
               
               loadedUnits = cb->cargo;
               /*               
               for ( int i = 0; i< 32; ++i )
                  if ( cb->loading[i] )
                     loadedUnits.push_back (  cb->loading[i] );
                     */
                     
               PG_Widget* unitScrollArea = FindChild( "UnitScrollArea", true );
               if ( unitScrollArea ) {
                  int x = 0;
                  int y = 0;
                  int posNum = cb->baseType->maxLoadableUnits;
                  if ( loadedUnits.size() > posNum )
                     posNum = loadedUnits.size();
                     
                  for ( int i = 0; i < posNum; ++i ) {
                     storingPositionVector.push_back( new StoringPosition( unitScrollArea, PG_Point( x, y), unitHighLight, loadedUnits, i, cb->baseType->maxLoadableUnits >= loadedUnits.size() ));
                     x += StoringPosition::spWidth;
                     if ( x + StoringPosition::spWidth >= unitScrollArea->Width() - 20 ) {
                        if ( !unitColumnCount )
                           unitColumnCount = i + 1;
                        x = 0;
                        y += StoringPosition::spHeight;
                     }   
                  }
               }
               
               // setLabelText( "unitpad_unitcategory", cmovemalitypes[ vt->movemalustyp ] );
               /*
               registerSpecialDisplay( "unitpad_unitsymbol");
               registerSpecialDisplay( "unitpad_weapon_diagram");
               registerSpecialDisplay( "unitpad_transport_transporterlevel");
               registerSpecialDisplay( "unitpad_transport_unitlevel");
               registerSpecialDisplay( "unitpad_transport_leveldisplay");
               */
               
               updateResourceDisplay();
               
              activate_i(0);
              Show();
              setupOK = true;
               
           };
           
      int RunModal()
      {
         if ( setupOK )
            return Panel::RunModal();
         return 0;   
      }     

      void userHandler( const ASCString& label, PropertyReadingContainer& pc, PG_Widget* parent, WidgetParameters widgetParams ) 
      {
         if ( label == "ButtonPanel" ) {
            int x = 0;
            for ( int i = 0; i < subWindowNum; ++i ) {
               SubWinButton* button = new SubWinButton( parent, SPoint( x, 0 ), i);
               button->sigClick.connect( SigC::bind( SigC::slot( *this, &CargoDialog::activate_i  ), i));
               x += SubWinButton::buttonwidth;
            }   
            
         /*
            int yoffset = 0;
            for ( int i = 0; i < vt->heightChangeMethodNum; ++i ) {
               int srcLevelCount = 0;
               for ( int j = 0; j < 8; ++j )
                  if ( vt->height & vt->heightChangeMethod[i].startHeight & (1 << j)) 
                     ++srcLevelCount;

               pc.openBracket( "LineWidget" );
               PG_Rect r = parseRect( pc, parent);
               r.y += yoffset;
               r.my_height *= (srcLevelCount-1) / 3 + 1;
               widgetParams.runTextIO( pc );

               SpecialInputWidget* sw = new SpecialInputWidget ( parent, r );
               parsePanelASCTXT( pc, sw, widgetParams );
               pc.closeBracket();
               yoffset += sw->Height();



               int counter = 0;
               for ( int j = 0; j < 8; ++j )
                  if ( vt->height & vt->heightChangeMethod[i].startHeight & (1 << j))  {
                     ASCString filename = "height-a" + ASCString::toString(j) + ".png";
                     int xoffs = 3 + IconRepository::getIcon(filename).w() * (counter % 3 );
                     int yoffs = 2 + IconRepository::getIcon(filename).h() * (counter / 3 );
                     new PG_Image( sw, PG_Point( xoffs, yoffs ), IconRepository::getIcon(filename).getBaseSurface(), false );
                     ++counter;
                  }

               ASCString delta = ASCString::toString( vt->heightChangeMethod[i].heightDelta );
               if ( vt->heightChangeMethod[i].heightDelta > 0 )
                  delta = "+" + delta;
               setLabelText( "unitpad_move_changeheight_change", delta, sw );
               
               setLabelText( "unitpad_move_changeheight_movepoints", vt->heightChangeMethod[i].moveCost, sw );
               setLabelText( "unitpad_move_changeheight_distance", vt->heightChangeMethod[i].dist, sw );
            }
            */
         }
      }; 
        
};








void cargoDialog( ContainerBase* cb )
{
   CargoDialog cd ( NULL, cb );
   cd.Show();
   cd.RunModal();
}



