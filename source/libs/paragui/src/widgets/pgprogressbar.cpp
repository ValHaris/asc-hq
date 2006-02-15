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
    Update Date:      $Date: 2006-02-15 21:30:16 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/src/widgets/pgprogressbar.cpp,v $
    CVS/RCS Revision: $Revision: 1.1.2.1 $
    Status:           $State: Exp $
*/

#include "pgapplication.h"
#include "pgprogressbar.h"
#include "pgdraw.h"
#include "pgtheme.h"

PG_ProgressBar::PG_ProgressBar(PG_Widget* parent, const PG_Rect& r, const std::string& style) : PG_ThemeWidget(parent, r) {

	my_percentCurrent = 0;
	my_drawPercentage = true;

	my_colorText.r = 0xFF;
	my_colorText.g = 0xFF;
	my_colorText.b = 0xFF;

	// fill in default gradient
	my_pbGradient.colors[0].r = 0x00;
	my_pbGradient.colors[0].g = 0x00;
	my_pbGradient.colors[0].b = 0x00;

	my_pbGradient.colors[1].r = 0x00;
	my_pbGradient.colors[1].g = 0x00;
	my_pbGradient.colors[1].b = 0xFF;

	my_pbGradient.colors[2].r = 0x00;
	my_pbGradient.colors[2].g = 0x00;
	my_pbGradient.colors[2].b = 0x00;

	my_pbGradient.colors[3].r = 0x00;
	my_pbGradient.colors[3].g = 0x00;
	my_pbGradient.colors[3].b = 0xFF;

	my_pbBackground = NULL;
	my_pbBackmode = PG_Draw::TILE;
	my_pbBlend = 255;
	my_bordersize = 1;

	LoadThemeStyle(style);
}

PG_ProgressBar::~PG_ProgressBar() {}

void PG_ProgressBar::SetDrawPercentage(bool drawit) {
	if(drawit != my_drawPercentage) {
		my_drawPercentage = drawit;
		Update();
	}
}


void PG_ProgressBar::LoadThemeStyle(const std::string& widgettype) {
	PG_Theme* t = PG_Application::GetTheme();

	PG_ThemeWidget::LoadThemeStyle(widgettype, "Background");

	my_pbBackground = t->FindSurface(widgettype, "Indicator", "background");

	PG_Gradient* g = t->FindGradient(widgettype, "Indicator", "gradient");

	if(g) {
		my_pbGradient = *g;
	}

	t->GetProperty(widgettype, "Indicator", "backmode", my_pbBackmode);
	t->GetProperty(widgettype, "Indicator", "blend", my_pbBlend);
}

void PG_ProgressBar::eventBlit(SDL_Surface* srf, const PG_Rect& src, const PG_Rect& dst) {
	PG_Rect pr(
	    my_xpos + 1,
	    my_ypos + 2,
	    my_width - 2,
	    my_height - 4);

	PG_Rect my_src, my_dst;
	char *buf;

	// get length of progress indicator

	int l = (int)(((double)pr.my_width / 100.0) * my_percentCurrent);
	pr.my_width = l;

	// draw our superclass
	PG_ThemeWidget::eventBlit(srf, src, dst);

	// draw the gradient
	SDL_Surface* ind = PG_ThemeWidget::CreateThemedSurface(
	                       pr,
	                       &my_pbGradient,
	                       my_pbBackground,			// currently no background
	                       my_pbBackmode,	// backmode
	                       my_pbBlend);			// background blend

	GetClipRects(my_src, my_dst, pr);
	PG_Draw::BlitSurface(ind, my_src, PG_Application::GetScreen(), my_dst);

	PG_ThemeWidget::DeleteThemedSurface(ind);

	if(my_drawPercentage) {
		buf = new char[my_text.length() + 64];
		// draw the percentage
		if (my_text.length() == 0)
			sprintf(buf, "%i%%", (int)my_percentCurrent);
		else
			sprintf(buf, my_text.c_str(), (int)my_percentCurrent);

		Uint16 w, h;
		GetTextSize(w,h, buf);

		// !! textrectangle can be bigger than the widget
		int x = (my_width - w)/2;
		int y = (my_height - h)/2;

		DrawText(x, y, buf);

		delete[] buf;
	}
}

// Why a double? -Dave
void PG_ProgressBar::SetProgress(double p) {
	if(p < 0)
		p = 0;
	if(p >100)
		p = 100;
	if(my_percentCurrent != p) {
		my_percentCurrent = p;
		Update();
	}
}

/*
 * Local Variables:
 * c-basic-offset: 8
 * End:
 */
