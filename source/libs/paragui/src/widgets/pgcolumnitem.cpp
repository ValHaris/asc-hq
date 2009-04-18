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
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/src/widgets/pgcolumnitem.cpp,v $
    CVS/RCS Revision: $Revision: 1.3 $
    Status:           $State: Exp $
*/

#include "pgcolumnitem.h"

PG_ColumnItem::PG_ColumnItem(PG_Widget* parent, Uint32 columns, Uint16 height, void* userdata, const std::string& style) : PG_ListBoxItem(parent, height, PG_NULLSTR, NULL, NULL, style) {
	SetUserData(userdata);
	my_columncount = columns;
	static PG_String YEmpty;

	my_columnwidth.reserve(columns);
	my_columntext.reserve(columns);

	// fill our vectors with the default values
	for(Uint32 i=0; i<my_columncount; i++) {
		my_columnwidth.push_back(Width()/my_columncount);
		my_columntext.push_back(YEmpty);
	}

}

PG_ColumnItem::~PG_ColumnItem() {
	my_columnwidth.clear();
	my_columntext.clear();
}

void PG_ColumnItem::SetColumnWidth(Uint32 column, Uint32 width) {
	my_columnwidth[column] = width;
}

void PG_ColumnItem::SetColumnText(Uint32 column, const std::string& text) {
	my_columntext[column] = text;
	Update();
}

void PG_ColumnItem::eventBlit(SDL_Surface* srf, const PG_Rect& src, const PG_Rect& dst) {
	int xshift = 0;

	if((dst.my_width == 0) || (dst.my_height == 0)) {
		return;
	}

	PG_ListBoxItem::eventBlit(srf, src, dst);

	if(my_srfIcon != NULL) {
		xshift = my_srfIcon->w;
	}

	for(Uint32 i=0; i<my_columncount; i++) {

		if(my_columntext[i].empty()) {
			xshift += my_columnwidth[i];
			continue;
		}

		Uint16 w, h;
		GetTextSize(w, h, my_columntext[i]);

		int cw = my_columnwidth[i];
		if(xshift + cw > my_width) {
			cw -=  ((xshift + cw) - my_width);
		}
		DrawText(xshift, (my_height - h)/2, my_columntext[i], PG_Rect(xshift, 0, cw-5, my_height));
		xshift += my_columnwidth[i];
	}

}

int PG_ColumnItem::GetColumnWidth(Uint32 column) {
	return my_columnwidth[column];
}

const PG_String& PG_ColumnItem::GetColumnText(Uint32 column) {
	return my_columntext[column];
}

int PG_ColumnItem::GetColumnCount() {
	return my_columncount;
}
