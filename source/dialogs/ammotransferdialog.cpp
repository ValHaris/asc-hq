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

#include <sigc++/sigc++.h>

#include "ammotransferdialog.h"

#include "../unitctrl.h"
#include "../containercontrols.h"

#include "../actions/servicing.h"


class TransferWidget : public PG_Widget {
   private:
      Transferrable* trans;
      PG_Slider* slider;

      bool updatePos( long a = 0 )
      {
         if ( slider )
            slider->SetPosition( trans->getAmount( trans->getDstContainer() ));
         return true;
      }

      bool updateRange()
      {
         int min = trans->getMin( trans->getDstContainer(), false );
         
         int max;
         if ( trans->getMax( trans->getDstContainer(), true ) * 3 < trans->getMax( trans->getDstContainer(), false ))
            max = trans->getMax( trans->getDstContainer(), true );
         else
            max = trans->getMax( trans->getDstContainer(), false );

         if ( slider )
            slider->SetRange( min, max );
         
         updatePos();
         return true;
      }
      
      
   public:
      TransferWidget ( PG_Widget* parent, const PG_Rect& pos, Transferrable* transferrable, TransferHandler& handler ) : PG_Widget( parent,pos ), trans( transferrable ), slider(NULL)
      {
         if ( transferrable->isExchangable() ) {
            slider = new PG_Slider( this, PG_Rect( 0, 30, pos.w, 15 ),  PG_ScrollBar::HORIZONTAL );
   
            updateRange();
   
            slider->sigSlide.connect( SigC::slot( *transferrable, &Transferrable::setDestAmount ));
            slider->sigSlideEnd.connect( SigC::slot( *this, &TransferWidget::updatePos));

            handler.updateRanges.connect( SigC::slot( *this, &TransferWidget::updateRange));
         }

         
         PG_Rect labels = PG_Rect( 0, 0, pos.w, 20 );
         PG_Label* l = new PG_Label ( this, labels, transferrable->getName() );
         l->SetAlignment( PG_Label::CENTER );
         
         l = new PG_Label ( this, labels );
         l->SetAlignment( PG_Label::LEFT );
         transferrable->sigSourceAmount.connect( SigC::slot( *l, &PG_Label::SetText ));
         
         l = new PG_Label ( this, labels );
         l->SetAlignment( PG_Label::RIGHT );
         transferrable->sigDestAmount.connect( SigC::slot( *l, &PG_Label::SetText ));
      };
};


class AmmoTransferWindow : public PG_Window {
   private:
      ContainerBase* first;
      ContainerBase* second;
      TransferHandler handler;

      bool ok()
      {
         handler.commit();
         QuitModal();
         return true;
      }
      
   public:
      AmmoTransferWindow ( ContainerBase* source, ContainerBase* destination, PG_Widget* parent );

      bool somethingToTransfer() { return handler.getTransfers().size(); };
      
      bool eventKeyDown(const SDL_KeyboardEvent* key)
      {
         if ( key->keysym.sym == SDLK_ESCAPE )  {
            QuitModal();
            return true;
         }
         return false;
      }
      
};


AmmoTransferWindow :: AmmoTransferWindow ( ContainerBase* source, ContainerBase* destination, PG_Widget* parent ) : PG_Window( NULL, PG_Rect( 30, 30, 400, 400 ), "Transfer" ), first (source), second( destination ), handler( source, destination )
{
   int ypos = 30;
   int border = 10;

   const int singleTransferHeight = 60;

   int expectedHeight = handler.getTransfers().size() * singleTransferHeight;
   if ( handler.ammoProductionPossible() )
      expectedHeight  += 30;

   int newHeight = min( PG_Application::GetScreen()->h - 60, expectedHeight + 80 );
   SizeWidget( w, newHeight );

   PG_ScrollWidget* area = new PG_ScrollWidget ( this, PG_Rect( border, ypos, w - border, h - 80 ));
   area->SetTransparency( 255 );
         
   ypos = 0;
   if ( handler.ammoProductionPossible() ) {
      PG_CheckButton* production = new PG_CheckButton( area, PG_Rect( border, ypos, area->w - 20, 20 ), "allow ammo production" );
      production->SetPressed(  );
      production->sigClick.connect( SigC::slot( handler, &TransferHandler::allowAmmoProduction ));
      ypos += 30;
   }
   
   for ( TransferHandler::Transfers::iterator i = handler.getTransfers().begin(); i != handler.getTransfers().end(); ++i ) {
      new TransferWidget( area, PG_Rect( 0, ypos, area->w - 20, 50 ), *i, handler );
      ypos += singleTransferHeight;
   }

   int buttonWidth = 150;
   PG_Button* b = new PG_Button( this, PG_Rect( w - buttonWidth - border, h - 30 - border, buttonWidth, 30), "OK" );
   b->sigClick.connect( SigC::slot( *this, &AmmoTransferWindow::ok ));
   
   for ( TransferHandler::Transfers::iterator i = handler.getTransfers().begin(); i != handler.getTransfers().end(); ++i ) 
      (*i)->showAll();
}

void ammoTransferWindow ( ContainerBase* source, ContainerBase* destination )
{
   AmmoTransferWindow atw( source, destination, NULL );
   if ( atw.somethingToTransfer() ) {
      atw.Show();
      atw.RunModal();
   }
}

void ammoTransferWindow ( VehicleService* serviceAction, ContainerBase* destination )
{

}
