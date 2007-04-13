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
    Update Date:      $Date: 2007-04-13 16:15:56 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pglayout.h,v $
    CVS/RCS Revision: $Revision: 1.2 $
    Status:           $State: Exp $
*/

#ifndef PG_LAYOUT_H
#define PG_LAYOUT_H

#include "pgwidget.h"
#include "pgapplication.h"
#include "pglabel.h"
#include "pgscrollbar.h"

/** \file pglayout.h
	Header file for the PG_Layout namespace.
	This include file defines the PG_Layout namespace and the PG_XMLTag class.
*/

/**
	@author Ales Teska
	@short XML layout loading functions.
 
	This namespace contains various functions responsible for
	loading XML widget layouts.
*/

class PG_XMLTag {
public:

	PG_XMLTag(const char* n, const char** a);
	~PG_XMLTag();

	const char* name;
	const char**atts;
};

namespace PG_Layout {

/**
	Load a XML layout.
	@param parent	Pointer to the widget which will be the parent for newly created widgets
						(from the XML layout). If NULL all new widgets will be created as toplevel
						widgets
	@param filename	name of the XML layoutfile
	@param WorkCallback progress callback function (OBSOLETE)
	@param UserSpace ???
	@return true on success
*/
DECLSPEC bool Load(PG_Widget* parent, const std::string& filename, void (* WorkCallback)(int now, int max), void *UserSpace);

DECLSPEC int GetParamInt(const char **Source, char *What);
DECLSPEC PG_ScrollBar::ScrollDirection GetParamScrollDirection(const char **Source, char *What);
DECLSPEC char* GetParamStr(const char **Source, char *What);
DECLSPEC void GetParamRect(const char **Source, char *What, PG_Rect& Rect, PG_Widget* parent = NULL);
DECLSPEC PG_Label::TextAlign GetParamAlign(const char **Source, char *What);
DECLSPEC PG_Draw::BkMode GetParamIMode(const char **Source, char *What);
DECLSPEC int GetParamGrad(const char **Source, char *What, PG_Gradient *grad);

};

#ifndef DOXYGEN_SKIP
extern void (* PG_LayoutWidgetParams)(PG_Widget *Widget, const char **atts);
extern void (* PG_LayoutProcessingInstruction)(const char *target,  const char *data, const std::string& FileName, void *UserSpace);
#endif // DOXYGEN_SKIP

#endif
