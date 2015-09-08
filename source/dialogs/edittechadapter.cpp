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


#include "edittechadapter.h"

#include "../player.h"
#include "../gamemap.h"

#include "../paradialog.h"
#include "../itemrepository.h"
#include "../iconrepository.h"
#include "../vehicletype.h"
#include "unitinfodialog.h"

#include "selectionwindow.h"

#include "pgmultilineedit.h"
#include "../widgets/textrenderer.h"

class TechAdapterWidget: public SelectionWidget
{
      ASCString name;
   public:
      TechAdapterWidget( PG_Widget* parent, const PG_Point& pos, int width, const ASCString& adapterName ) : SelectionWidget( parent, PG_Rect( pos.x, pos.y, width, 18 )), name(adapterName)
      {
         PG_Label* lbl1 = new PG_Label( this, PG_Rect( 5, 0, width - 10, Height() ), name );
         lbl1->SetFontSize( lbl1->GetFontSize() -1 );

         SetTransparency( 255 );
      };

      ASCString getName() const
      {
         return name;
      };

   protected:

      void display( SDL_Surface * surface, const PG_Rect & src, const PG_Rect & dst )
      {
      }
      ;
};




   class TechAdapterSelectionItemFactory: public SelectionItemFactory, public sigc::trackable  {
        
         ASCString selected;
         
         void add( set<ASCString>& list, const TechAdapterDependency& adapter );
         
      public:
         typedef set<ASCString> Container;

      protected:
         Container::iterator it;
         Container items;
         Container* adapterForPlayer;

      public:
         TechAdapterSelectionItemFactory(  );
         TechAdapterSelectionItemFactory( set<ASCString>& adapter );

         
         
         void restart();
         sigc::signal<void,const ASCString&> techSelected;
   
         SelectionWidget* spawnNextItem( PG_Widget* parent, const PG_Point& pos );
      
         void itemSelected( const SelectionWidget* widget, bool mouse );
         void itemMarked( const SelectionWidget* widget, bool mouse );
         
         ASCString getSelected() { return selected; };
   };



   TechAdapterSelectionItemFactory :: TechAdapterSelectionItemFactory( ) : adapterForPlayer(NULL)
   {
      restart();
   };

   TechAdapterSelectionItemFactory :: TechAdapterSelectionItemFactory( set<ASCString>& adapter ) : adapterForPlayer( &adapter )
   {
      restart();
   };

   
   void TechAdapterSelectionItemFactory::add( set<ASCString>& list, const TechAdapterDependency& adapter )
   {
      const TechAdapterDependency::RequiredTechAdapter& deps = adapter.listAdapters();
      for ( TechAdapterDependency::RequiredTechAdapter::const_iterator i = deps.begin(); i != deps.end(); ++i ) {
         list.insert( *i );
      }
   }
   
   
   void TechAdapterSelectionItemFactory::restart()
   {
      items.clear();
      
      if ( !adapterForPlayer ) {
         
         for ( int i = 0; i < vehicleTypeRepository.getNum(); ++i )
            add( items, vehicleTypeRepository.getObject_byPos(i)->techDependency );
      
         for ( int i = 0; i < buildingTypeRepository.getNum(); ++i )
            add( items, buildingTypeRepository.getObject_byPos(i)->techDependency );
         
         for ( int i = 0; i < objectTypeRepository.getNum(); ++i )
            add( items, objectTypeRepository.getObject_byPos(i)->techDependency );
         
      } else {
         items = *adapterForPlayer;
      }
         
      it = items.begin();
   };


   SelectionWidget* TechAdapterSelectionItemFactory::spawnNextItem( PG_Widget* parent, const PG_Point& pos )
   {
      if ( it != items.end() ) {
         ASCString v = *(it++);
         
         return new TechAdapterWidget( parent, pos, parent->Width() - 15, v );
      } else
         return NULL;
   };


   void TechAdapterSelectionItemFactory::itemMarked( const SelectionWidget* widget, bool mouse )
   {
      itemSelected(widget, mouse );
   }
   
   void TechAdapterSelectionItemFactory::itemSelected( const SelectionWidget* widget, bool mouse )
   {
      if ( !widget )
         return;

      const TechAdapterWidget* tw = dynamic_cast<const TechAdapterWidget*>(widget);
      assert( tw );

      selected = tw->getName();
      techSelected(tw->getName());
   }



class EditTechAdapter : public ASC_PG_Dialog
{
   ItemSelectorWidget* allItemSelector;
   ItemSelectorWidget* playerItemSelector;
   TechAdapterSelectionItemFactory* allAdapterfactory;
   TechAdapterSelectionItemFactory* playerFactory;
   
   set<ASCString> playerAdapter;
   
   TextRenderer* objectList;
   Player& player;

   bool containsAdapter( const ASCString& adapter, const TechAdapterDependency& dependency )
   {
      return find( dependency.listAdapters().begin(), dependency.listAdapters().end(), adapter ) != dependency.listAdapters().end();
   }
   
   void techAdapterSelected( const ASCString& adapter )
   {
      const ASCString big = "#fontsize=14#";
      const ASCString medium = "#fontsize=12#";
      const ASCString small = "#fontsize=10#";
      
      ASCString complete = big + "TechAdapter " + adapter + " is used by:" + small + "\n";
      ASCString local;
      
      for ( int i = 0; i < vehicleTypeRepository.getNum(); ++i )
         if ( containsAdapter( adapter, vehicleTypeRepository.getObject_byPos(i)->techDependency ))
            local += "#vehicletype=" + ASCString::toString(vehicleTypeRepository.getObject_byPos(i)->id) + "# " + vehicleTypeRepository.getObject_byPos(i)->getName() + " \n";
      
      if ( !local.empty() )
         complete += medium + "VehicleTypes" + small + "\n" + local;
      local.clear();
      
      for ( int i = 0; i < buildingTypeRepository.getNum(); ++i )
         if ( containsAdapter( adapter, buildingTypeRepository.getObject_byPos(i)->techDependency ))
            local += "  " + buildingTypeRepository.getObject_byPos(i)->getName() + "\n";
         
      if ( !local.empty() )
         complete += medium + "BuildingTypes" + small + "\n" + local;
      local.clear();
      
      for ( int i = 0; i < objectTypeRepository.getNum(); ++i )
         if ( containsAdapter( adapter, objectTypeRepository.getObject_byPos(i)->techDependency ))
            local += "  " + objectTypeRepository.getObject_byPos(i)->getName() + "\n";
      
      if ( !local.empty() )
         complete += medium + "ObjectTypes" + small + "\n" + local;
      
      objectList->SetText( complete );
   };

   protected:
      bool handleButtonClick(PG_Button* button) 
      {
            return false;
      };

      bool ok()
      {
         player.research.setPredefinedTechAdapter( playerAdapter );
         QuitModal();
         return true;
      }

      bool cancel()
      {
         QuitModal();
         return true;
      }
      
      bool add()
      {
         playerAdapter.insert( allAdapterfactory->getSelected() );
         playerItemSelector->reLoad( true );
         return true;  
      }
      
      bool remove()
      {
         set<ASCString>::iterator i = playerAdapter.find( playerFactory->getSelected());
         if ( i != playerAdapter.end() )
            playerAdapter.erase( playerFactory->getSelected() );
         playerItemSelector->reLoad( true );
              
         return true;  
      }
     
      bool eventKeyDown(const SDL_KeyboardEvent* key)
      {
         int mod = SDL_GetModState() & ~(KMOD_NUM | KMOD_CAPS | KMOD_MODE);

         if ( !mod  ) 
            if ( key->keysym.sym == SDLK_RETURN ) 
               return ok();
         
         return false;
      };

   
   public:
      EditTechAdapter( Player& my_player ) : ASC_PG_Dialog( NULL, PG_Rect( -1, -1, 770, 600), "Edit Tech Adapter" ) , allAdapterfactory(NULL), playerFactory(NULL), player( my_player )
      {
         playerAdapter.insert( player.research.getPredefinedTechAdapter().begin(), player.research.getPredefinedTechAdapter().end() );
         
         new PG_Label(this, PG_Rect( 10, 30, 300, 20), "All TechAdapter");
         PG_Rect left ( 10, 55, 300, Height() - 220 );
         
         new PG_Label(this, PG_Rect( 420, 30, 300, 20), "TechAdapter for " + player.getName() );
         PG_Rect right( 420, 55, 300, Height() - 220 );
         
         Emboss* eleft = new Emboss( this, left, true );
         Emboss* eright = new Emboss( this, right, true );
         
         
         allAdapterfactory = new TechAdapterSelectionItemFactory();
         allAdapterfactory->techSelected.connect( sigc::mem_fun( *this, &EditTechAdapter::techAdapterSelected ));
         allItemSelector = new ItemSelectorWidget( eleft, PG_Rect( 10, 10, eleft->Width() - 20, eleft->Height()-20 ), allAdapterfactory );

         playerFactory = new TechAdapterSelectionItemFactory( playerAdapter );
         playerFactory->techSelected.connect( sigc::mem_fun( *this, &EditTechAdapter::techAdapterSelected ));
         playerItemSelector = new ItemSelectorWidget( eright, PG_Rect( 10, 10, eright->Width() - 20, eright->Height()-20 ), playerFactory );
         
         
         (new PG_Button(this, PG_Rect( left.x + left.Width() + 10, left.y, right.x - left.x - left.Width() - 20,  30), "Add >>" ))->sigClick.connect( sigc::hide( sigc::mem_fun( *this, &EditTechAdapter::add)));
         (new PG_Button(this, PG_Rect( left.x + left.Width() + 10, left.y + 40, right.x - left.x - left.Width() - 20,  30), "Remove <<" ))->sigClick.connect( sigc::hide( sigc::mem_fun( *this, &EditTechAdapter::remove)));
         
         Emboss* deps = new Emboss(this, PG_Rect ( 10, Height() - 160, 330, 120 ), true);
         objectList = new TextRenderer( deps, PG_Rect(1,1,deps->Width() - 2, deps->Height() - 2) );

         AddStandardButton("~O~K")->sigClick.connect( sigc::hide( sigc::mem_fun( *this, &EditTechAdapter::ok )));
         AddStandardButton("~C~ancel")->sigClick.connect( sigc::hide( sigc::mem_fun( *this, &EditTechAdapter::cancel )));
      };
};




void editTechAdapter( Player& player )
{
   EditTechAdapter eta( player);
   eta.Show();
   eta.RunModal();
}



