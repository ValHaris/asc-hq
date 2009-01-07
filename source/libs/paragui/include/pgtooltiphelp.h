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
    Update Date:      $Date: 2009-01-07 18:45:15 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pgtooltiphelp.h,v $
    CVS/RCS Revision: $Revision: 1.3 $
    Status:           $State: Exp $
*/

/** \file pgwidget.h
 Header file for the PG_Widget class.
*/

#ifndef PG_TOOLTIPHELP_H
#define PG_TOOLTIPHELP_H

#include "pgmessageobject.h"
#include "pgtimerobject.h"
#include "pgpoint.h"

class PG_Widget;
class PG_LineEdit;


/**
 @author Martin Bickel
 
 @short ToolTip Help for widgets
 
 Displays ToolTip help that opens when a mouse cursor hovers over a widget. 
 It behaves similar to a PG_Widget, but it is not derived from it
        
 The ToolTipHelp will delete itself when the parent widget is deleted. 
 It is also safe to delete the ToolTipHelp object manually prior to deleting the widget. 
 
 @ToDo Query the cursor size and position the help so it doesn't overlap with large cursors
*/
class DECLSPEC PG_ToolTipHelp: public SigC::Object {
private:

class Ticker: public PG_TimerObject {
		volatile Uint32 ticker;
		Uint32 eventTimer(Uint32 interval) {
			++ticker;
			return interval;
		};
	public:
		Ticker( int interval ) : ticker(0) {
			SetTimer( interval );
		};
		Uint32 getTicker() {
			return ticker;
		};
	};

	static Ticker* ticker;

	void startTimer();

protected:
	PG_Widget* parentWidget;
	PG_TimerObject::ID id;
	Uint32 lastTick;

	enum { off, counting, shown } status;

	std::string my_text;
	std::string labelStyle;

	int my_delay;

	static PG_LineEdit* toolTipLabel;

	bool onParentEnter( PG_Pointer dummy );
	bool onParentLeave( PG_Pointer dummy );
	bool onParentDelete( const PG_MessageObject* object );
	bool onMouseMotion( const SDL_MouseMotionEvent *motion );
	bool onIdle();


public:
	/**
	Create a ToolTipHelp for the given widget
	     
	It automatically enables SigIdle calls for PG_Application

	       @param parent The widget for which the ToolTip Help shall be shown
	@param text The help text 
	       @param delay The delay in 1/10 s after which the help appears when the mouse has stopped moving 
	@param style The theme style for the Help. Default: Widget Type = ToolTipHelp . Object Name = LineEdit 
	*/
	PG_ToolTipHelp( PG_Widget* parent, const std::string& text, int delay = 10, const std::string &style="ToolTipHelp" );


	/**
	Changes the help text 
	*/
	void SetText( const std::string& text );

	/**
	Show the help to be shown
	       
	@param pos The screen coordinates of the upper left corner 
	*/
	void ShowHelp( const PG_Point& pos );

	/**
	Hides the ToolTip Help
	*/
	void HideHelp( );
};



#endif
