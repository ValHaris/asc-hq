/*
   ParaGUI - crossplatform widgetset
   Copyright (C) 2000,2001,2002  Alexander Pipelka
 
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
 
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
 
   You should have received a copy of the GNU Library General Public
   License along with this library; if not, write to the Free
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 
   Alexander Pipelka
   pipelka@teleweb.at
 
   Last Update:      $Author: mbickel $
   Update Date:      $Date: 2009-04-18 13:48:40 $
   Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/src/widgets/pgtooltiphelp.cpp,v $
   CVS/RCS Revision: $Revision: 1.3 $
   Status:           $State: Exp $
 */

#include <cstring>

#include "pgapplication.h"
#include "pgeventsupplier.h"
#include "pgwidget.h"
#include "pglineedit.h"
#include "pgtooltiphelp.h"



PG_LineEdit* PG_ToolTipHelp::toolTipLabel = NULL;
PG_ToolTipHelp::Ticker* PG_ToolTipHelp::ticker = NULL;


PG_ToolTipHelp :: PG_ToolTipHelp( PG_Widget* parent, const std::string& text, int delay, const std::string &style )
		: parentWidget(parent), lastTick(0), status(off), labelStyle(style), my_delay(delay) {
	if ( !parent )
		return;

	parent->sigMouseEnter.connect( SigC::slot( *this, &PG_ToolTipHelp::onParentEnter ), parent );
	parent->sigMouseLeave.connect( SigC::slot( *this, &PG_ToolTipHelp::onParentLeave ), parent );
	parent->sigMouseMotion.connect( SigC::slot( *this, &PG_ToolTipHelp::onMouseMotion ));
	PG_Application::GetApp()->sigAppIdle.connect( SigC::slot( *this, &PG_ToolTipHelp::onIdle ));
	PG_Application::GetApp()->EnableAppIdleCalls();

	parent->sigDelete.connect( SigC::slot( *this, &PG_ToolTipHelp::onParentDelete ));

	SetText( text );
}


void PG_ToolTipHelp :: SetText( const std::string& text ) {
	my_text = text;
}

bool PG_ToolTipHelp :: onIdle(  ) {
	if ( !ticker )
		return false;

	if ( status != counting )
		return false;

	if ( status == counting && !parentWidget->IsMouseInside() ) {
		status = off;
		return false;
	}

	if ( ticker->getTicker() > lastTick + 10 ) {
		if ( status < shown ) {
			int x, y;
			PG_Application::GetEventSupplier()->GetMouseState( x,y );
			ShowHelp( PG_Point(x+5,y+10) );
			status = shown;
		}
		return true;
	}
	return false;
}


bool PG_ToolTipHelp :: onParentEnter( void* dummy ) {
	if ( !ticker )
		ticker = new Ticker(100);

	status = counting;

	lastTick = ticker->getTicker();
	return true;
}

bool PG_ToolTipHelp :: onParentLeave( void* dummy ) {
	// if the ToolTipLabel is beneath the mouse cursor, we'll receive a onParentLeave notification that we'll ignore
	if ( toolTipLabel && toolTipLabel->IsMouseInside() )
		return false;

	HideHelp();
	status = off;
	return false;
}


bool PG_ToolTipHelp :: onParentDelete( const PG_MessageObject* object ) {
	if ( status != off)
		HideHelp();

	delete this;
	return true;
}

bool PG_ToolTipHelp :: onMouseMotion( const SDL_MouseMotionEvent *motion ) {
	if ( ticker )
		lastTick = ticker->getTicker();

	status = counting;

	HideHelp();
	return true;
}


void PG_ToolTipHelp :: ShowHelp( const PG_Point& pos ) {
	PG_Point mousePos = pos;

	/*
	if ( ! parentWidget->IsInside( mousePos ) ) 
	   mousePos = PG_Point( parentWidget->x + parentWidget->Width() / 2, parentWidget->y + parentWidget->Height() / 2 );
	*/

	if ( toolTipLabel )
		delete toolTipLabel;

	toolTipLabel = new PG_LineEdit( NULL, PG_Rect( mousePos.x, mousePos.y, 0, 0 ), labelStyle );
	toolTipLabel->SetText( my_text );
	toolTipLabel->SetEditable( false );

	Uint16 w;
	Uint16 h;
	toolTipLabel->GetTextSize( w, h );

	PG_Rect r = *toolTipLabel;
	r.w = w + 6;
	r.h = h + 4;
	if ( r.x + r.w > PG_Application::GetScreen()->w )
		r.x = PG_Application::GetScreen()->w - r.w;

	if ( r.y + r.h > PG_Application::GetScreen()->h )
		r.y = PG_Application::GetScreen()->h - r.h;

	toolTipLabel->MoveWidget( r, false );
	toolTipLabel->Show();
	toolTipLabel->sigMouseMotion.connect( SigC::slot( *this, &PG_ToolTipHelp::onMouseMotion ));
}

void PG_ToolTipHelp :: HideHelp( ) {
	if ( toolTipLabel ) {
		delete toolTipLabel;
		toolTipLabel = NULL;
	}
}

