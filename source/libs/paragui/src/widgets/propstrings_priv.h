/*
    ParaGUI - crossplatform widgetset
    Copyright (C) 2000,2001,2002,2003,2004  Alexander Pipelka
 
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
    Update Date:      $Date: 2006-03-19 19:56:01 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/src/widgets/propstrings_priv.h,v $
    CVS/RCS Revision: $Revision: 1.1.2.2 $
    Status:           $State: Exp $
*/

#ifndef PROPSTRINGS_PRIV_H
#define PROPSTRINGS_PRIV_H

#include <string>

// private static strings 
class PG_PropStr {
public:
	static const std::string Button;
	static const std::string Label;
	static const std::string ListBox;
	static const std::string ListBoxItem;
	static const std::string RadioButton;
	static const std::string Scrollbar;
	static const std::string ScrollbarDown;
	static const std::string ScrollbarDrag;
	static const std::string ScrollbarDragH;
	static const std::string ScrollbarDragV;
	static const std::string ScrollbarH;
	static const std::string ScrollbarLeft;
	static const std::string ScrollbarRight;
	static const std::string ScrollbarUp;
	static const std::string ScrollbarV;
	static const std::string ThemeWidget;

	static const std::string label;
	static const std::string alignment;
	static const std::string simplebackground;
	static const std::string nocache;
	static const std::string backgroundcolor;
	static const std::string background;
	static const std::string blend;
	static const std::string textcolor;
   static const std::string texthighlightcolor;
   static const std::string bordercolor0;
	static const std::string bordercolor1;
	static const std::string bordercolor0i;
	static const std::string bordercolor1i;
	static const std::string bordersize;
	static const std::string backmode;
	static const std::string gradient;
	static const std::string transparency;
	static const std::string width;
	static const std::string height;

	static const std::string iconup;
	static const std::string icondown;
	static const std::string iconover;
	static const std::string gradient0;
	static const std::string gradient1;
	static const std::string gradient2;
	static const std::string bordersize0;
	static const std::string bordersize1;
	static const std::string bordersize2;
	static const std::string transparency0;
	static const std::string transparency1;
	static const std::string transparency2;
	static const std::string background0;
	static const std::string background1;
	static const std::string background2;
	static const std::string backmode0;
	static const std::string backmode1;
	static const std::string backmode2;
	static const std::string blend0;
	static const std::string blend1;
	static const std::string blend2;
	static const std::string shift;
	static const std::string iconindent;
};

#endif // PROPSTRINGS_PRIV_H
