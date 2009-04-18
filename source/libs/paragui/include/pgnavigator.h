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
    Update Date:      $Date: 2009-04-18 13:48:39 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pgnavigator.h,v $
    CVS/RCS Revision: $Revision: 1.3 $
    Status:           $State: Exp $
*/

/** \file pgnavigator.h
	Header file for the PG_Navigator class.
*/

#ifndef PG_NAVIGATOR_H
#define PG_NAVIGATOR_H

#include "pgwidget.h"
#include <vector>

/**
 * @author Alexander Pipelka
 *
 * @short A class for widget navigation
 *
 */

class PG_Navigator : protected std::vector<PG_Widget*> {

public:

	void Add(PG_Widget* widget);

	void Remove(PG_Widget* widget);

	bool Action(PG_Widget::KeyAction action);

	PG_Widget* Goto(PG_Widget* widget);

	PG_Widget* GotoFirst();

	PG_Widget* GotoLast();

	PG_Widget* GotoNext();

	PG_Widget* GotoPrev();

	PG_Widget* FindLeft(PG_Widget* widget = NULL);

	PG_Widget* FindRight(PG_Widget* widget = NULL);

	PG_Widget* FindUp(PG_Widget* widget = NULL);

	PG_Widget* FindDown(PG_Widget* widget = NULL);

private:

	DLLLOCAL PG_Widget* FindWidget(PG_Widget* from, PG_Point ref, bool absx, bool absy, int xmode, int ymode);

	static PG_Widget* my_currentWidget;
};

#endif	// PG_NAVIGATOR_H
