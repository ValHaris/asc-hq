/*! \file guiiconhandler.cpp
    \brief All the buttons of the user interface with which the unit actions are
      controlled.
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

const int guiIconSizeX = 49;
const int guiIconSizeY = 35;
const int guiIconSpace = 5;
const int guiIconColumnNum = 3;




GuiButton::GuiButton( PG_Widget *parent, const PG_Rect &r ) : PG_Button( parent, r, "", -1, "GuiButton"), func( NULL )
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
     func->execute( pos );
     return true;
  }
  return false;
}


void GuiButton::registerFunc( GuiFunction* f, const MapCoordinate& position )
{
   func = f;
   pos = position;
   SetIcon( f->getImage( position).getBaseSurface());
}

void GuiButton::unregisterFunc()
{
   func = NULL;
   SetIcon ( (SDL_Surface*) NULL );
}



void GuiIconHandler::eval()
{
   MapCoordinate mc = actmap->player[actmap->actplayer].cursorPos;
   
   if ( !mc.valid() )
      return;
     
   if ( mc.x >= actmap->xsize || mc.y >= actmap->ysize )
      return;

   int num = 0;
   for ( Functions::iterator i = functions.begin(); i != functions.end(); ++i ) {
      if ( (*i)->available(mc )) {
         GuiButton* b = host->getButton(num);
         b->registerFunc( *i, mc );
         b->Show();
         ++num;
      }
   }

   host->disableButtons(num);
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







NewGuiHost :: NewGuiHost (PG_Widget *parent, const PG_Rect &r )
         : Panel( parent, r, "GuiIcons", false ) , handler(NULL)
{
   updateFieldInfo.connect ( SigC::slot( *this, &NewGuiHost::eval ));
}

void NewGuiHost::eval()
{
   if ( handler )
      handler->eval();
}

void NewGuiHost::pushIconHandler( GuiIconHandler* iconHandler )
{
   handler = iconHandler;
   iconHandler->registerHost( this );
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

NewGuiHost::~NewGuiHost()
{
   if ( handler )
      handler->registerHost( NULL );
}

