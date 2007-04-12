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


#include "choosetech.h"

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

class TechWidget: public SelectionWidget  {
      const Technology* tech;
      static Surface clippingSurface;
      Surface& getClippingSurface() { return clippingSurface; };
      int actplayer;
      static const int widgetHeight = 60;
      bool info();
   public:
      TechWidget( PG_Widget* parent, const PG_Point& pos, int width, const Technology* technology, int player = 0 );
      ASCString getName() const;
      const Technology* getTechnology() const { return tech; };
   protected:
      void display( SDL_Surface * surface, const PG_Rect & src, const PG_Rect & dst );
};

TechWidget :: TechWidget( PG_Widget* parent, const PG_Point& pos, int width, const Technology* technology, int player ) : SelectionWidget( parent, PG_Rect( pos.x, pos.y, width, widgetHeight )), tech( technology ), actplayer(player)
{

   int lineheight = 20;
   
   int xoffs = 20;
   if ( tech->relatedUnitID > 0 )
      xoffs += 40;

   int www = width - xoffs - 20 - 2 * lineheight;

   new PG_Label( this, PG_Rect( xoffs, 10, www, 25 ), tech->name );
   new PG_Label( this, PG_Rect( xoffs, 30, www, 25 ), ASCString::toString( tech->researchpoints) + " RP" );

   if ( tech->relatedUnitID > 0 ) {
      PG_Button* b = new PG_Button( this, PG_Rect( width - 2 * lineheight - 10, Height()/2-lineheight, 2*lineheight, 2*lineheight ));
      b->SetIcon( IconRepository::getIcon( "blue-i.png").getBaseSurface() );
      b->sigClick.connect( SigC::slot( *this, &TechWidget::info ));
   }
   
   SetTransparency( 255 );
};

bool TechWidget::info()
{
   const Vehicletype* vt = vehicleTypeRepository.getObject_byID( tech->relatedUnitID );
   if ( vt )
      unitInfoDialog( vt );
   return true;
}



ASCString TechWidget::getName() const
{
   return tech->name;
};

void TechWidget::display( SDL_Surface * surface, const PG_Rect & src, const PG_Rect & dst )
{
   Vehicletype* vt = NULL;
   if ( tech->relatedUnitID > 0 )
      if ( (vt = vehicleTypeRepository.getObject_byID( tech->relatedUnitID ))) {
         if ( !getClippingSurface().valid() )
            getClippingSurface() = Surface::createSurface( fieldsizex + 10, fieldsizey + 10, 32, 0 );
   
         getClippingSurface().Fill(0);
   
         vt->paint( getClippingSurface(), SPoint(5,5), actplayer, 0 );
         PG_Draw::BlitSurface( getClippingSurface().getBaseSurface(), src, surface, dst);
      }
}

Surface TechWidget::clippingSurface;





   class TechnologySelectionItemFactory: public SelectionItemFactory, public SigC::Object  {
         Player& player;
         bool allTechs;
      public:
         typedef vector<const Technology*> Container;

      protected:
         Container::iterator it;
         Container items;

      public:
         TechnologySelectionItemFactory( Player& player );


         bool showAllTechs( bool all) { allTechs = all; return true; };
         
         void restart();
         SigC::Signal1<void,const Technology*> techSelected;
   
         SelectionWidget* spawnNextItem( PG_Widget* parent, const PG_Point& pos );
      
         void itemSelected( const SelectionWidget* widget, bool mouse );
   };



   TechnologySelectionItemFactory :: TechnologySelectionItemFactory( Player& theplayer ) : player(theplayer), allTechs(false)
   {
      restart();
   };

   bool techComp ( const Technology* t1, const Technology* t2 )
   {
      return (t1->relatedUnitID > 0 && t2->relatedUnitID < 0 ) ||
             (t1->techlevel < t2->techlevel) ||
             (t1->techlevel == t2->techlevel && t1->name < t2->name );
   }
   
   void TechnologySelectionItemFactory::restart()
   {
      items.clear();
      
      for (int i = 0; i < technologyRepository.getNum(); i++) {
         const Technology* tech = technologyRepository.getObject_byPos( i );
         if ( tech ) {
            if ( allTechs ) {
               if ( tech->eventually_available( player.research, NULL ))
                  items.push_back( tech );
            } else {
               ResearchAvailabilityStatus a = player.research.techAvailable ( tech );
               if ( a == Available )
                  items.push_back ( tech );
            }
         }
      }
      
      sort( items.begin(), items.end(), techComp );
      it = items.begin();
   };


   SelectionWidget* TechnologySelectionItemFactory::spawnNextItem( PG_Widget* parent, const PG_Point& pos )
   {
      if ( it != items.end() ) {
         const Technology* v = *(it++);
         
         return new TechWidget( parent, pos, parent->Width() - 15, v, player.getPosition() );
      } else
         return NULL;
   };


   void TechnologySelectionItemFactory::itemSelected( const SelectionWidget* widget, bool mouse )
   {
      if ( !widget )
         return;

      const TechWidget* tw = dynamic_cast<const TechWidget*>(widget);
      assert( tw );

      techSelected(tw->getTechnology());
   }



class ChooseTech : public ASC_PG_Dialog
{
   ItemSelectorWidget* itemSelector;
   TechnologySelectionItemFactory* factory;
   PG_MultiLineEdit* techList;
   Player& player;

   PG_Label* pointsLabel;
   PG_Label* availLabel;
   const Technology* goal;

   bool changeTechView( bool all )
   {
      factory->showAllTechs ( all );
      itemSelector->reLoad( true );
      return true;
      
   }
   
   void techSelected( const Technology* tech )
   {
      ASCString s;
      list<const Technology*> techs;
      if ( !tech->eventually_available( player.research , &techs )) {
         warning("Inconsistency in ChooseTech::techSelected");
         return;
      }

      assert( techs.begin() != techs.end() );

      int points = 0;
      for ( list<const Technology*>::iterator i = techs.begin(); i != techs.end(); ++i ) {
         s += (*i)->name + "\n";
         points += (*i)->researchpoints;
      }
      techList->SetText( s );

      pointsLabel->SetText( ASCString("Sum: ") + ASCString::toString(points) + " Points" );

      goal = player.research.goal = tech;
      player.research.activetechnology = *techs.begin();
   };

   protected:
      bool handleButtonClick(PG_Button* button) 
      {
         if ( goal )
            return ASC_PG_Dialog::handleButtonClick( button );
         else
            return false;
      };

      bool ok()
      {
         if ( goal || !itemSelector->getItemNum() ) {
            QuitModal();
            return true;
         } else
            return false;
      }

      bool showPrerequisites()
      {
         if ( goal ) {
            ASCString msg = "Prerequisites for Technology " + goal->name + ":\n";
            msg += goal->techDependency.showDebug( player.research );
            ViewFormattedText vft("Technology Prerequisites", msg, PG_Rect( -1, -1, 500, 400 ));
            vft.Show();
            vft.RunModal();
         }
         return true;
      }

   
   public:
      ChooseTech( Player& my_player ) : ASC_PG_Dialog( NULL, PG_Rect( -1, -1, 770, 600), "Choose Technology" ) , factory(NULL), player( my_player ), goal(NULL)
      {
         factory = new TechnologySelectionItemFactory( player );
         factory->techSelected.connect( SigC::slot( *this, &ChooseTech::techSelected ));
         itemSelector = new ItemSelectorWidget( this, PG_Rect( 10, 40, 400, Height() - 80 ), factory );

         (new PG_CheckButton( this, PG_Rect( 10, Height() - 40, 300, 25 ), "Show All Technologies"))->sigClick.connect( SigC::slot( *this, &ChooseTech::changeTechView));
         techList = new PG_MultiLineEdit( this, PG_Rect ( 450, 40, 300, 200 ));
         techList->SetEditable(false);
         pointsLabel = new PG_Label( this, PG_Rect( 450, 250, 300, 25 ));
         availLabel = new PG_Label( this, PG_Rect( 450, 280, 300, 25 ), "Accumulated research points: " + ASCString::toString( player.research.progress) );

         (new PG_Button( this, PG_Rect( 450, 320, 300, 20), "List Prerequisites" ))->sigClick.connect( SigC::slot( *this, &ChooseTech::showPrerequisites ));

         AddStandardButton("~O~K")->sigClick.connect( SigC::slot( *this, &ChooseTech::ok ));
      };
};




void chooseTechnology( Player& player )
{
#if 0
   ItemSelectorWindow isw( NULL, );
   isw.Show();
   isw.RunModal();
#else
   ChooseTech ct( player);
   ct.Show();
   ct.RunModal();
#endif
}



