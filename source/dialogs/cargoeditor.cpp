/***************************************************************************
                          cargowidget.cpp  -  description
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
#include "cargowidget.h"

#include "../clipboard.h"
#include "../edselfnt.h"
#include "../edmisc.h"



class CargoEditor : public PG_Window {
      ContainerBase* container;
      HighLightingManager highLightingManager;
      int unitColumnCount;
      CargoWidget* cargoWidget;
      PG_ProgressBar* bgw;
      static int stack;

      TemporaryContainerStorage tus;

      bool addUnit()
      {
         addCargo( container );
         cargoWidget->redrawAll();
         updateGraph();
         return true;
      }
      
      bool remove()
      {
         if ( cargoWidget->getMarkedUnit() ) {
            delete cargoWidget->getMarkedUnit();
            cargoWidget->redrawAll();
            updateGraph();
            return true;
         } else
            return false;
      }

      bool copyUnit()
      {
         if ( cargoWidget->getMarkedUnit() ) {
            ClipBoard::Instance().clear();
            ClipBoard::Instance().addUnit( cargoWidget->getMarkedUnit() );
            return true;
         } else
            return false;
      }
      
      bool pasteUnit()
      {
         Vehicle* veh = ClipBoard::Instance().pasteUnit();
         if ( !veh )
            return false;
         
         if ( container->vehicleFit( veh )) {
            container->addToCargo( veh );
            cargoWidget->redrawAll();
            updateGraph();
            return true;
         } else {
            delete veh;
            return false;
         }
      }

      bool editUnit()
      {
         changeunitvalues( cargoWidget->getMarkedUnit() );
         updateGraph();
         return true;
      }

      bool editUnitCargo()
      {
         cargoEditor( cargoWidget->getMarkedUnit() );
         updateGraph();
         return true;
      }
               
      bool ok()
      {
         QuitModal();
         return true;
      }

      bool cancel()
      {
         tus.restore();
         QuitModal();
         return true;
      }

      void updateGraph()
      {
         if ( container->baseType->maxLoadableWeight > 0 ) {
            bgw->SetProgress( float( container->cargoWeight()) / container->baseType->maxLoadableWeight * 100 );
            bgw->Update();
         }
      }
      
   public:
      CargoEditor( PG_Widget* parent, ContainerBase* my_container ) : PG_Window( parent, PG_Rect( 50 + stack * 20, 30 + stack * 20, 500, 400 ), "Cargo Editor" ), container( my_container ), tus( container, true)
      {
         bgw = new PG_ProgressBar( this, PG_Rect( 10, 35, Width() - 20, 15 ) );

         updateGraph();
         
         ++stack;
         cargoWidget = new CargoWidget( this, PG_Rect( 10, 60, Width()-20, Height() - 120 ), container, true);

         int buttonLine = Height() - 110 + 60;
         int buttonHeight = 30;
         PG_Button* add = new PG_Button( this, PG_Rect( 10, buttonLine, 50, buttonHeight), "+" );
         add->sigClick.connect( SigC::slot( *this, &CargoEditor::addUnit ));
         
         PG_Button* rem = new PG_Button( this, PG_Rect( 70, buttonLine, 50, buttonHeight), "-" );
         rem->sigClick.connect( SigC::slot( *this, &CargoEditor::remove ));
         
         PG_Button* copy = new PG_Button( this, PG_Rect( 130, buttonLine, 50, buttonHeight), "copy" );
         copy->sigClick.connect( SigC::slot( *this, &CargoEditor::copyUnit ));
         
         PG_Button* paste = new PG_Button( this, PG_Rect( 190, buttonLine, 50, buttonHeight), "paste" );
         paste->sigClick.connect( SigC::slot( *this, &CargoEditor::pasteUnit ));
         
         PG_Button* edit = new PG_Button( this, PG_Rect( 250, buttonLine, 50, buttonHeight), "edit" );
         edit->sigClick.connect( SigC::slot( *this, &CargoEditor::editUnit ));
         
         PG_Button* cargo = new PG_Button( this, PG_Rect( 310, buttonLine, 50, buttonHeight), "cargo" );
         cargo->sigClick.connect( SigC::slot( *this, &CargoEditor::editUnitCargo ));
         
         PG_Button* cancel = new PG_Button( this, PG_Rect( 370, buttonLine, 50, buttonHeight), "cancel" );
         cancel->sigClick.connect( SigC::slot( *this, &CargoEditor::cancel ));
         
         PG_Button* ok = new PG_Button( this, PG_Rect( 430, buttonLine, 50, buttonHeight), "ok" );
         ok->sigClick.connect( SigC::slot( *this, &CargoEditor::ok ));
      }

      bool eventKeyDown(const SDL_KeyboardEvent* key)
      {
         int mod = SDL_GetModState() & ~(KMOD_NUM | KMOD_CAPS | KMOD_MODE);

         if ( !mod  ) {
            if ( key->keysym.sym == SDLK_ESCAPE )
               return cancel();
               
            if ( key->keysym.sym == SDLK_RETURN )
               return ok();
               
            if ( key->keysym.unicode == 'c' )
               return editUnitCargo();
            
            if ( key->keysym.unicode == 'p' )
               return editUnit();
         
            if ( key->keysym.unicode == '+' )
               return addUnit();

            if ( key->keysym.unicode == '-' || key->keysym.sym == SDLK_DELETE )
               return remove();
            
         }
         
         if ( mod & KMOD_CTRL ) {
            if ( key->keysym.unicode == 'c' )
               return copyUnit();
            
            if ( key->keysym.unicode == 'v' )
               return pasteUnit();

         }
         return false;
      };
      
      ~CargoEditor()
      {
         --stack;
      }
};


int CargoEditor::stack = 0;

void cargoEditor( ContainerBase* container )
{
   if ( container && container->baseType->maxLoadableUnits ) {
      CargoEditor ce ( NULL, container );
      ce.Show();
      ce.RunModal();
   }
}

