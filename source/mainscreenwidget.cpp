
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "global.h"

#include <stdio.h>
#include <cstring>
#include <stdlib.h>
#include <new>
#include <cstdlib>
#include <ctype.h>
#include <algorithm>
#include <memory>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include "paradialog.h"

#include "mainscreenwidget.h"
#include "basegfx.h"
#include "misc.h"
#include "iconrepository.h"
#include "graphics/blitter.h"
#include "graphics/drawing.h"
#include "overviewmappanel.h"
#include "spfst.h"
#include "itemrepository.h"
#include "mapdisplay.h"




MainScreenWidget::MainScreenWidget( PG_Application& application )
              : PG_Widget(NULL, PG_Rect ( 0, 0, app.GetScreen()->w, app.GetScreen()->h ), false),
              app ( application ) , lastMessageTime(0), lastMouseScrollTime(0), messageLine(NULL)
{
}


void MainScreenWidget::setup( bool messageLine )
{
   displayLogMessage ( 5, "MainScreenWidget: initializing panels:\n");

   dataLoaderTicker();

   displayLogMessage ( 7, "  Mapdisplay ");
   mapDisplay = new MapDisplayPG( this, PG_Rect(15,30,Width() - 200, Height() - 73));
   mapDisplay->SetID( ASC_PG_App::mapDisplayID );
   dataLoaderTicker();

   SetID( ASC_PG_App::mainScreenID );

   displayLogMessage ( 5, "done\nMainScreenWidget completed\n");
   dataLoaderTicker();

   repaintDisplay.connect ( SigC::bind( SigC::slot( *this, &MainScreenWidget::Update ), true ));
   
   buildBackgroundImage( messageLine );
   dataLoaderTicker();

   PG_Application::GetApp()->sigAppIdle.connect( SigC::slot( *this, &MainScreenWidget::idleHandler ));

   mapChanged.connect( SigC::slot( OverviewMapHolder::clearmap ));
   dataLoaderTicker();
   
   MessagingHub::Instance().statusInformation.connect( SigC::slot( *this, &MainScreenWidget::displayMessage ));
   MessagingHub::Instance().messageWindowFactory.connect( SigC::slot( *this, &MainScreenWidget::createStatusWindow ));
}


void MainScreenWidget :: activateMapLayer( const ASCString& name, bool active )
{
   if ( mapDisplay )
      mapDisplay->activateMapLayer( name , active );
}

void MainScreenWidget :: toggleMapLayer( const ASCString& name )
{
   if ( mapDisplay )
      mapDisplay->toggleMapLayer( name );
}


bool MainScreenWidget :: idleHandler( )
{
   if ( ticker > lastMessageTime + 300 ) {
      displayMessage( "" );
      lastMessageTime = 0xfffffff;
   }

   mouseScrollChecker();
   return true;
}

void MainScreenWidget :: mouseScrollChecker()
{
   if ( getPGApplication().isFullscreen() && IsMouseInside() ) {

      if ( ticker > lastMouseScrollTime + 30 ) {
         int x,y;
         SDL_GetMouseState( &x, &y);
   
         if ( y <= 0 ) {
            if ( x <= 0 ) {
               mapDisplay->scrollMap( 7 );
               return;
            }

            if ( x >= PG_Application::GetScreenWidth() - 1 ) {
               mapDisplay->scrollMap( 1 );
               return;
            } 

            mapDisplay->scrollMap(0);
            return;
         }

         if ( y >= PG_Application::GetScreenHeight() - 1 ) {
            if ( x <= 0 ) {
               mapDisplay->scrollMap( 5 );
               return;
            }

            if ( x >= PG_Application::GetScreenWidth() - 1 ) {
               mapDisplay->scrollMap( 3 );
               return;
            }

            mapDisplay->scrollMap(4);
            return;
         }

         if ( x >= PG_Application::GetScreenWidth() - 1 ) {
            mapDisplay->scrollMap( 2 );
            return;
         }

         if ( x <= 0 ) {
            mapDisplay->scrollMap( 6 );
            return;
         }
      }
   }
}



void MainScreenWidget::buildBackgroundImage( bool messageLine )
{
   if ( !backgroundImage.valid() ) {
      backgroundImage = Surface::createSurface( Width(), Height(), 32 );
      
      Surface& source = IconRepository::getIcon( getBackgroundImageFilename() );
      
      MegaBlitter< gamemapPixelSize, gamemapPixelSize, ColorTransform_None,ColorMerger_PlainOverwrite,SourcePixelSelector_DirectZoom> blitter;
      blitter.setSize( source.w(), source.h(), Width(), Height(), false );
      
      dataLoaderTicker();
      blitter.blit( source, backgroundImage, SPoint(0,0) );

      dataLoaderTicker();
            
      assert( mapDisplay );
      PG_Rect r = *mapDisplay;
      
      const int borderWidth = 5;
      for ( int i = 0; i <= borderWidth; ++i ) 
         rectangle<4> ( backgroundImage, SPoint(r.x-i, r.y-i), r.w+2*i, r.h+2*i, ColorMerger_Brightness<4>( 0.6 ), ColorMerger_Brightness<4>( 1.5 ));

      int x1 = r.x + 1;
      int y1 = r.y + 1;
      int x2 = r.x + r.Width() - 1;
      int y2 = r.y + r.Height() -1;

      blitRects[0] = PG_Rect(  0, 0, Width(), y1 ); 
      blitRects[1] = PG_Rect(  0, y1, x1,     y2 ); 
      blitRects[2] = PG_Rect( x2, y1, Width() - x2, y2 ); 
      blitRects[3] = PG_Rect( 0, y2, Width(), Height() - y2 );
      
      if ( messageLine ) {
      
         Surface& msgstart = IconRepository::getIcon("msgline1.png");
         Surface& msgmid   = IconRepository::getIcon("msgline2.png");
         Surface& msgend   = IconRepository::getIcon("msgline3.png");
   
         dataLoaderTicker();
         
         int mx1 = x1 - borderWidth;
         int mx2 = x2 + borderWidth;
         int my1 = y2 + 10;
         
         int msglength = mx2 - mx1;
         int midlength = msglength - msgend.w();
   
         backgroundImage.Blit( msgstart, SPoint( mx1, my1 ));
   
         int x = msgstart.w();
         while ( x + msgmid.w() < midlength ) {
            backgroundImage.Blit( msgmid, SPoint( mx1 + x, my1 ));
            x += msgmid.w();
         }
         dataLoaderTicker();
         backgroundImage.Blit( msgmid, SPoint( mx1 + msglength - msgend.w() - msgmid.w(), my1 ));
         
         dataLoaderTicker();
         backgroundImage.Blit( msgend, SPoint( mx1 + msglength - msgend.w(), my1) );
   
         this->messageLine = new PG_Label ( this, PG_Rect( mx1 + 20, my1 + 9, msglength - 30, msgend.h() - 18) );
         this->messageLine->SetFontSize(11);
         dataLoaderTicker();
      }
         
   }
}

void MainScreenWidget::displayMessage( const ASCString& message )
{
   if ( messageLine ) {
      messageLine->SetText( message );
      lastMessageTime = ticker;
   }   
}



void MainScreenWidget::eventBlit (SDL_Surface *surface, const PG_Rect &src, const PG_Rect &dst) 
{
   SDL_Rect dstrect;
   Surface s = Surface::Wrap( PG_Application::GetScreen() );
   dstrect.x = blitRects[0].x;
   dstrect.y = blitRects[0].y;
   s.Blit( backgroundImage, blitRects[0], dstrect );
   
   dstrect.x = blitRects[1].x;
   dstrect.y = blitRects[1].y;
   s.Blit( backgroundImage, blitRects[1], dstrect );
   
   dstrect.x = blitRects[2].x;
   dstrect.y = blitRects[2].y;
   s.Blit( backgroundImage, blitRects[2], dstrect );
   
   dstrect.x = blitRects[3].x;
   dstrect.y = blitRects[3].y;
   s.Blit( backgroundImage, blitRects[3], dstrect );
}

StatusMessageWindowHolder MainScreenWidget::createStatusWindow( const ASCString& msg )
{
   return StatusMessageWindowHolder( new PG_StatusWindowData( msg ));
}

void MainScreenWidget::spawnOverviewMapPanel ( const ASCString& panelName )
{
   assert( mapDisplay);
   OverviewMapPanel* smp = new OverviewMapPanel( this, PG_Rect(Width()-170, 0, 170, 160), mapDisplay, panelName );
   smp->Show();
   mapChanged.connect( SigC::slot( OverviewMapHolder::clearmap ));
}


