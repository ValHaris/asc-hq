/*! \file guiiconhandler.cpp
    \brief All system for controlling the buttons of the user interface with which the unit actions are controlled.
*/

/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2003  Martin Bickel  and  Marc Schellenberger

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

#include <stdio.h>
#include <cstring>
#include <stdlib.h>
#include <SDL_image.h>

#include "guiiconhandler.h"
#include "spfst.h"
#include "iconrepository.h"
#include "mapdisplay.h"
#include "sigc++/retype_return.h"
#include "asc-mainscreen.h"
#include "gameoptions.h"
#include "spfst-legacy.h"

const int guiIconSizeX = 49;
const int guiIconSizeY = 35;
const int guiIconSpace = 5;
const int guiIconColumnNum = 3;


const int smallGuiIconSizeX = 30;
const int smallGuiIconSizeY = 22;
const int smallGuiIconSpace = 2;
const float smallGuiIconSizeFactor = 0.6;



GuiButton::GuiButton( PG_Widget *parent, const PG_Rect &r ) : PG_Button( parent, r, "", -1, "GuiButton"), func( NULL ), id(-1)
{
  sigClick.connect ( SigC::slot( *this, &GuiButton::exec ));
  SetBackground( PRESSED, IconRepository::getIcon("empty-pressed.png").getBaseSurface() );
  SetBackground( HIGHLITED, IconRepository::getIcon("empty-high.png").getBaseSurface() );
  SetBackground( UNPRESSED, IconRepository::getIcon("empty.png").getBaseSurface() );
  SetBorderSize(0,0,0);
}

bool GuiButton::exec()
{
  if ( func ) {
     callFunc( pos, subject, id );
     return true;
  }
  return false;
}


void GuiButton::registerFunc( GuiFunction* f, const MapCoordinate& position, ContainerBase* subject, int id )
{
   this->id = id;
   this->subject = subject;
   func = f;
   pos = position;
   SetIcon( f->getImage( position, subject, id).getBaseSurface());
}

void GuiButton::unregisterFunc()
{
   func = NULL;
   id = 0;
   subject = NULL;
   pos = MapCoordinate(-1,-1);
   SetIcon ( (SDL_Surface*) NULL );
}

void GuiButton::eventMouseEnter()
{
   PG_Button::eventMouseEnter();
   showInfoText();
}

void GuiButton::eventMouseLeave()
{
   PG_Button::eventMouseLeave();
   MessagingHub::Instance().statusInformation("");
}

void GuiButton::callFunc( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   try {
      func->execute( pos, subject, id );
   } catch ( ActionResult res ) {
      errorMessage( res.getMessage() );
   }
}


bool GuiButton::checkForKey( const SDL_KeyboardEvent* key, int modifier )
{
   if ( func->available( pos, subject, id ))
      if ( func->checkForKey( key, modifier, id)) {
         callFunc( pos, subject, id );
         return true;
      }
   return false;
}

void GuiButton::showInfoText()
{
   if ( func )
      MessagingHub::Instance().statusInformation( func->getName(pos, subject, id));
}



SmallGuiButton::SmallGuiButton( PG_Widget *parent, const PG_Rect &r, GuiButton* guiButton, NewGuiHost* host ) : PG_Button( parent, r, "", -1, "GuiButton"), referenceButton( guiButton )
{
  sigClick.connect ( SigC::slot( *host, &NewGuiHost::clearSmallIcons ));
  sigClick.connect ( SigC::slot( *guiButton, &GuiButton::exec ));

  SetBackground( PRESSED, IconRepository::getIcon("empty-small-pressed.png").getBaseSurface() );
  SetBackground( HIGHLITED, IconRepository::getIcon("empty-small-high.png").getBaseSurface() );
  SetBackground( UNPRESSED, IconRepository::getIcon("empty-small.png").getBaseSurface() );
  SetBorderSize(0,0,0);
  SetDirtyUpdate(true);

  SetBehaviour( SIGNALONRELEASE );

  updateIcon();
}


void SmallGuiButton::updateIcon()
{
   SDL_Surface* icn = referenceButton->GetIcon( UNPRESSED );
   if ( icn ) {
      smallIcon = PG_Draw::ScaleSurface( icn, smallGuiIconSizeFactor, smallGuiIconSizeFactor );
      SetIcon( smallIcon, NULL, NULL, true );
   } else
      smallIcon = NULL;
}



void SmallGuiButton::press()
{
   SetPressed(true);
   Update();
}   

void SmallGuiButton::showInfoText()
{
   if ( referenceButton && referenceButton->func )
      MessagingHub::Instance().statusInformation( referenceButton->func->getName(referenceButton->pos, referenceButton->subject, referenceButton->id));
}


void SmallGuiButton::eventMouseEnter()
{
   PG_Button::eventMouseEnter();
   showInfoText();
}

void SmallGuiButton::eventMouseLeave()
{
   PG_Button::eventMouseLeave();
   MessagingHub::Instance().statusInformation("");
}


SmallGuiButton::~SmallGuiButton()
{
}



void GuiIconHandler::eval( const MapCoordinate& pos, ContainerBase* subject )
{
   int num = 0;
   for ( Functions::iterator i = functions.begin(); i != functions.end(); ++i ) {
      if ( (*i)->available(pos, subject, 0 )) {
         GuiButton* b = host->getButton(num);
         b->registerFunc( *i, pos, subject, 0 );
         b->Show();
         ++num;
      }
   }

   host->disableButtons(num);
}

/*
void GuiIconHandler::eval()
{
   MapCoordinate mc = actmap->getCursor();

   if ( !mc.valid() )
      return;

   if ( mc.x >= actmap->xsize || mc.y >= actmap->ysize )
      return;

   ContainerBase* subject = actmap->getField(mc)->getContainer();
   
   eval( mc, subject );
}
*/

bool GuiIconHandler::checkForKey( const SDL_KeyboardEvent* key, int modifier )
{
#if 0
   if ( !actmap->getCursor().valid())
      return false;

   ContainerBase* subject = actmap->getField(actmap->getCursor())->getContainer();
   
   for ( Functions::iterator i = functions.begin(); i != functions.end(); ++i )
      if ( (*i)->available(actmap->getCursor(), subject, 0 ))
         if ( (*i)->checkForKey( key, modifier, 0)) {
            (*i)->execute(actmap->getCursor(), subject, 0 );
            return true;
         }
#endif         
   return false;
}


void GuiIconHandler::registerUserFunction( GuiFunction* function )
{
   functions.push_back ( function );
}


GuiIconHandler::~GuiIconHandler()
{
   for ( Functions::iterator i = functions.begin(); i != functions.end(); ++i )
      delete *i;

}





NewGuiHost* NewGuiHost::theGuiHost = NULL;

NewGuiHost :: NewGuiHost (MainScreenWidget *parent, MapDisplayPG* mapDisplay, const PG_Rect &r )
         : DashboardPanel( parent, r, "GuiIcons", false ) , handler(NULL), enterKeyPressed(false), keyPressedButton(-1)
{
   this->mapDisplay = mapDisplay;
   mapDisplay->mouseButtonOnField.connect( SigC::slot( *this, &NewGuiHost::mapIconProcessing ));
   updateFieldInfo.connect ( SigC::slot( *this, &NewGuiHost::evalCursor ));
   theGuiHost = this;

   cursorMoved.connect( SigC::hide_return( SigC::slot( *this, &NewGuiHost::clearSmallIcons )) );

   
   PG_Application::GetApp()->sigKeyDown.connect( SigC::slot( *this, &NewGuiHost::eventKeyDown ));
   PG_Application::GetApp()->sigKeyUp.connect( SigC::slot( *this, &NewGuiHost::eventKeyUp ));
   SetTransparency(255);
   
   parent->lockOptionsChanged.connect( SigC::slot( *this, &NewGuiHost::lockOptionsChanged ));

   GameMap::sigMapDeletion.connect( SigC::slot( *this, &NewGuiHost::mapDeleted ));
}

void NewGuiHost::lockOptionsChanged( int options )
{
   if ( options & MainScreenWidget::LockOptions::MapActions )  
      // EnableReceiver(false, true);
      Hide();
   else  
      // EnableReceiver(true, true);
      Show();
}


class SmallButtonHolder : public SpecialInputWidget {
      bool locked;
   public:
      void Lock() { SetCapture(); locked = true; };
      void Unlock() { ReleaseCapture(); locked = false; };

      SmallButtonHolder (PG_Widget *parent, const PG_Rect &rect ) : SpecialInputWidget( parent, rect ), locked(false) {};
      bool eventMouseMotion (const SDL_MouseMotionEvent *motion) { return true; };
      bool eventMouseButtonDown (const SDL_MouseButtonEvent *button) { return true; };
      bool eventMouseButtonUp (const SDL_MouseButtonEvent *button) { Unlock(); return true; };
      
      bool ProcessEvent(const SDL_Event * event, bool bModal) { return SpecialInputWidget::ProcessEvent( event, bModal ); };
      bool ProcessEvent ( const SDL_Event *   event  )
      {
         /*
         PG_RectList* cl = GetChildList ();
		   for(PG_Widget* i = cl->first(); i != NULL; i = i->next()) {
			   if ( i->ProcessEvent( event, true ))
               return true;
		   }
         */
        
         bool result = false;
         
         if ( locked ) 
            ReleaseCapture();
         
         if ( SpecialInputWidget::ProcessEvent( event, true )) 
            result = true;
         
         if ( locked )
            SetCapture();
             
         if ( !result && event->type == SDL_MOUSEBUTTONUP )
            Unlock();
         
         return result;
      }

};



SmallButtonHolder* NewGuiHost :: smallButtonHolder = NULL;

Command* NewGuiHost :: pendingCommand = NULL;

void NewGuiHost::evalCursor()
{
   if ( !actmap )
      return;
   
   MapCoordinate mc = actmap->getCursor();

   if ( !mc.valid() )
      return;

   if ( mc.x >= actmap->xsize || mc.y >= actmap->ysize )
      return;

   ContainerBase* subject = actmap->getField(mc)->getContainer();
   
   eval( mc, subject );
}

void NewGuiHost::mapDeleted( GameMap& map )
{
   pendingCommand = NULL;
   /*
   while ( theGuiHost->iconHandlerStack.size() >= 1 )
      popIconHandler();
      */
}


void NewGuiHost::eval( const MapCoordinate& pos, ContainerBase* subject )
{
   if ( handler ) {
      BulkGraphicUpdates bgu(this);
      handler->eval( pos, subject );
   }
}


void NewGuiHost::pushIconHandler( GuiIconHandler* iconHandler )
{
   if ( !theGuiHost )
      return;

   if ( theGuiHost->handler )
      theGuiHost->iconHandlerStack.push_back( theGuiHost->handler );

   theGuiHost->handler = iconHandler;
   iconHandler->registerHost( theGuiHost );
   updateFieldInfo();
}

GuiIconHandler* NewGuiHost::getIconHandler(  )
{
   if ( theGuiHost )
      return theGuiHost->handler;
   else
      return NULL;
}


void NewGuiHost::popIconHandler( )
{
   if ( !theGuiHost )
      return;

   theGuiHost->clearSmallIcons();

   assert( theGuiHost->handler );

   theGuiHost->handler->registerHost( NULL );

   theGuiHost->handler = theGuiHost->iconHandlerStack.back();
   theGuiHost->iconHandlerStack.pop_back();
   updateFieldInfo();
}


GuiButton* NewGuiHost::getButton( int i )
{
   while ( i >= buttons.size() ) {
      int w = (Width() - 4 * guiIconSpace) / guiIconColumnNum;
      GuiButton* b = new GuiButton ( this, PG_Rect( guiIconSpace + i%3 * (w + guiIconSpace), 10 + guiIconSpace + i/3 * (guiIconSpace + guiIconSizeY), guiIconSizeX, guiIconSizeY));
      buttons.push_back ( b );
      b->Hide();
   }
   return buttons[i];
}


void NewGuiHost::disableButtons( int i )
{
   for ( int j = i; j < buttons.size(); ++j) {
      GuiButton* b = getButton(j);
      b->Hide();
      b->unregisterFunc();
   }
}



bool NewGuiHost::mapIconProcessing( const MapCoordinate& pos, const SPoint& mousePos, bool cursorChanged, int button, int prio )
{
   if ( prio > 1 )
      return false;
    
   if ( button != CGameOptions::Instance()->mouse.fieldmarkbutton )
      return false;

   

   // PG_Point p = mapDisplay->ScreenToClient( mousePos.x, mousePos.y );
   SPoint p = mousePos;

   tfield* fld = actmap->getField(pos);

   bool positionedUnderCursor = false;
   if ( ( fld->vehicle || fld->building) && fieldvisiblenow(fld) )
      positionedUnderCursor = true;
   
   if ( fld->a.temp ) {
      positionedUnderCursor = true;
      cursorChanged = false;
   }

   if ( positionedUnderCursor ) {
      p.x -= smallGuiIconSizeX/2;
      p.y -= smallGuiIconSizeY/2;
   } else {
      p.x += 2;
      p.y += 2;
   }

   showSmallIcons( mainScreenWidget, p, cursorChanged );
   return true;
}


SmallGuiButton* NewGuiHost::getSmallButton( int i )
{
   assert(smallButtonHolder);
   while ( i >= smallButtons.size() ) {
      PG_Rect r = PG_Rect( (smallGuiIconSizeX + smallGuiIconSpace) * smallButtons.size(), 0, smallGuiIconSizeX, smallGuiIconSizeY  );
      SmallGuiButton* b = new SmallGuiButton ( smallButtonHolder, r, getButton(i), this);
      smallButtons.push_back ( b );
      b->Hide();
   }

   return smallButtons[i];
}


bool   NewGuiHost::ProcessEvent (const SDL_Event *event, bool bModal)
{
   if ( smallButtonHolder && smallButtonHolder->ProcessEvent( event ))
      return true;
   
   if ( DashboardPanel::ProcessEvent( event, bModal ))
      return true;

   return false;
}



bool NewGuiHost::showSmallIcons( PG_Widget* parent, const SPoint& pos, bool cursorChanged )
{
   clearSmallIcons();

   BulkGraphicUpdates bgu;
  
   SmallGuiButton* firstSmallButton = NULL;
   
   int count = 0;
   if ( !cursorChanged || CGameOptions::Instance()->mouse.singleClickAction ) {
      for ( int j = 0; j < buttons.size(); ++j)
         if ( !getButton(j)->IsHidden() )
            ++count;
      
      if ( count ) {
         if ( !smallButtonHolder ) 
            smallButtonHolder = new SmallButtonHolder ( NULL, PG_Rect::null );
         

         smallButtonHolder->MoveWidget( PG_Rect( pos.x, pos.y, count * smallGuiIconSizeX + (count-1)*smallGuiIconSpace, smallGuiIconSizeY ), false );

         for ( int j = 0; j < buttons.size(); ++j) {
            GuiButton* b = getButton(j);
            if ( !b->IsHidden() ) {
               SmallGuiButton* sgi = getSmallButton( j );
               sgi->updateIcon();
               sgi->SetHidden(false);
               if ( j == 0  && sgi->IsMouseInside() )
                  firstSmallButton = sgi;
            } else
               getSmallButton( j )->SetHidden(true);
         }
      }
   }

   bgu.release();
   if ( smallButtonHolder && count ) {
      smallButtonHolder->BringToFront();
      smallButtonHolder->Show();
      smallButtonHolder->Lock();

      if ( firstSmallButton ) {
         firstSmallButton->press();
         firstSmallButton->showInfoText();
      }
   }
   
   return true;
}


bool NewGuiHost::setNewButtonPressed( int i )
{
   if ( keyPressedButton == i )
      return false;

   if ( keyPressedButton >= 0 ) {
      GuiButton* button = getButton( keyPressedButton );
      if ( button ) {
         button->SetPressed(false);
         button->SetToggle(false);
      }
   }

   if ( i < buttons.size() || i < 0 ) {
      keyPressedButton = i;

      if ( keyPressedButton >= 0 ) {
         GuiButton* button = getButton( keyPressedButton );
         if ( button ) {
            button->SetToggle(true);
            button->SetPressed(true);
            button->showInfoText();
         }
      }
      return true;
   }
   return false;
}

bool NewGuiHost::eventKeyDown(const SDL_KeyboardEvent* key)
{
   int mod = SDL_GetModState() & ~(KMOD_NUM | KMOD_CAPS | KMOD_MODE);
   if ( mod )
      return false;

   if ( !IsVisible() )
      return false;

   if ( key->keysym.sym == SDLK_RETURN   ) {
      if ( !enterKeyPressed ) {
         mapDisplay->keyboadCursorMovement( false );
         enterKeyPressed = true;
         setNewButtonPressed( 0 );
      }
      return true;
   }

   if ( enterKeyPressed ) {
      if ( key->keysym.sym == SDLK_RIGHT  || key->keysym.sym == SDLK_KP6 )
         return setNewButtonPressed( keyPressedButton + 1);

      if ( key->keysym.sym == SDLK_LEFT  || key->keysym.sym == SDLK_KP4 )
         if  ( keyPressedButton > 0 )
            return setNewButtonPressed( keyPressedButton - 1);

      if ( key->keysym.sym == SDLK_UP || key->keysym.sym == SDLK_KP8 )
         if ( keyPressedButton >= guiIconColumnNum )
            return setNewButtonPressed( keyPressedButton - guiIconColumnNum );

      if ( key->keysym.sym == SDLK_DOWN || key->keysym.sym == SDLK_KP2 )
         return setNewButtonPressed( keyPressedButton + guiIconColumnNum );
         
      if ( key->keysym.sym == SDLK_ESCAPE || key->keysym.sym == SDLK_END ) {
         enterKeyPressed = false;
         mapDisplay->keyboadCursorMovement( true );
         
         setNewButtonPressed( -1 );
         return true;
      }
         
   } else {
      int modifier = SDL_GetModState();
      for ( int j = 0; j < buttons.size(); ++j)
         if ( getButton(j)->ready() )
            if ( getButton(j)->checkForKey( key, modifier ))
               return true;

      if ( handler )
         if ( handler->checkForKey( key, modifier ))
            return true;

   }

   return false;
}

bool NewGuiHost::eventKeyUp(const SDL_KeyboardEvent* key)
{
   if ( key->keysym.sym == SDLK_RETURN  && enterKeyPressed ) {
      enterKeyPressed = false;
      mapDisplay->keyboadCursorMovement( true );

      GuiButton* button = getButton( keyPressedButton );
      if ( button )
         button->exec();

      setNewButtonPressed( -1 );
      return true;
   }

   return false;
}


#if 0

bool NewGuiHost::clearSmallIcons()
{
   bool bulk = PG_Application::GetBulkMode();

 //  if ( !bulk )
  //    PG_Application::SetBulkMode(true);

   bool redraw;
   PG_Rect redrawRect;
   if ( smallButtonHolder ) {
      redraw = true;
      redrawRect = *smallButtonHolder;
   } else
      redraw = false;
   
      delete smallButtonHolder;
      smallButtonHolder = NULL;

      if ( !bulk ) {
    //  PG_Application::SetBulkMode(false);
      // mapDisplay->UpdateRect( true );
      }

      return true;
}

#endif

bool NewGuiHost::clearSmallIcons()
{
   if ( smallButtonHolder && smallButtonHolder->IsVisible() ) {
      smallButtonHolder->Unlock();
      smallButtonHolder->Hide();
   }
   return true;
}


NewGuiHost::~NewGuiHost()
{
   if ( handler )
      handler->registerHost( NULL );

}

void resetActiveGuiAction( GameMap* map )
{
   if ( NewGuiHost::pendingCommand ) {
      delete NewGuiHost::pendingCommand;
      NewGuiHost::pendingCommand = NULL;
      map->cleartemps();
      repaintMap();
   }
}
