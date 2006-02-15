/*
    ParaGUI - crossplatform widgetset
    drawline - line drawing algo
 
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
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/src/draw/drawline.cpp,v $
    CVS/RCS Revision: $Revision: 1.1.2.1 $
    Status:           $State: Exp $
*/

#include "pgdraw.h"

void plotpixel(SDL_Surface* surface, Uint32 x, Uint32 y, const PG_Color& c, Uint8 width, int *pixelflag) {
	Uint32 xp, yp, xf, yf;
	static Uint32 oldx, oldy;

	//	width=1? Only need to draw 1 pixel
	if(width==1) {
		PG_Draw::SetPixel(x, y, c, surface);
	} else {

		//	calculate new pixel position (shifts are faster)
		xp=x-((width-1) >> 1);
		yp=y-((width-1) >> 1);

		//	beginning of line?
		if (*pixelflag==0) {

			*pixelflag=1;

			//	draw all pixels needed
			for (xf=0; xf<width; xf++) {
				for (yf=0; yf<width; yf++) {
					PG_Draw::SetPixel(xf+xp, yf+yp, c, surface);
				}
			}
		}
		//	not the offspring of line

		else {

			//	moved down a pixel?
			if (yp>oldy) {

				//	only need to draw the bottom pixels
				for (xf=0; xf<width; xf++) {
					PG_Draw::SetPixel(xf+xp, yp-1+(width-1), c, surface);
				}
			}

			//	moved right a pixel
			if (xp>oldx) {
				//	only need to draw the most right pixels
				for (yf=0; yf<width; yf++) {
					PG_Draw::SetPixel(xp-1+(width-1), yp+yf, c, surface);
				}
			}

			//	moved left a pixel
			if (xp<oldx) {

				//	only need to draw the most left pixels
				for (yf=0; yf<width; yf++) {
					PG_Draw::SetPixel(xp+1, yp+yf, c, surface);
				}
			}

			//	store actual pixel positions
			oldx=xp;
			oldy=yp;
		}
	}
}

void octant0(SDL_Surface* surface, Uint32 x0, Uint32 y0, Uint32 deltax, Uint32 deltay,
             int xdirection, const PG_Color& color, Uint8 width, int pixelflag) {
	int deltay2;
	int error;
	int deltay2deltax2;	/* delta y2 - deltax2 */

	/* setup intial error used in drawing loop */
	deltay2 = deltay << 1;
	deltay2deltax2 = deltay2 - (int)(deltax << 1);
	error = deltay2 - (int)deltax;

	/* draw the line */
	plotpixel(surface, x0, y0, color, width, &pixelflag);	/* draw first pixel */
	while(deltax--) {
		/* test to advance the y coordinate */
		if (error >= 0) {
			/* advance y coordinate and adjust the error term */
			y0++;
			error += deltay2deltax2;
		} /* end of the if */
		else {
			/* add to the error term */
			error += deltay2;
		} /* end of if else */

		x0 += xdirection; 		/* advance the x coordinate */
		plotpixel(surface, x0, y0, color, width, &pixelflag);
	} /* end of the while */

} /* end of the function */

void octant1(SDL_Surface* surface, Uint32 x0, Uint32 y0, Uint32 deltax, Uint32 deltay,
             int xdirection, const PG_Color& color, Uint8 width, int pixelflag) {
	int deltax2;
	int error;
	int deltax2deltay2;	/* delta x2 - deltay2 */

	/* setup intial error used in drawing loop */
	deltax2 = deltax << 1;
	deltax2deltay2 = deltax2 - (int)(deltay << 1);
	error = deltax2 - (int)deltay;

	/* draw the line */
	plotpixel(surface, x0, y0, color, width, &pixelflag);	/* draw first pixel */
	while(deltay--) {
		/* test to advance the y coordinate */
		if (error >= 0) {
			/* advance y coordinate and adjust the error term */
			x0 += xdirection;
			error += deltax2deltay2;
		} /* end of the if */
		else {
			/* add to the error term */
			error += deltax2;
		} /* end of if else */

		y0++; 		/* advance the x coordinate */
		plotpixel(surface, x0, y0, color, width, &pixelflag);

	} /* end of the while */

} /* end of the function */

void PG_Draw::DrawLine(SDL_Surface* surface, Uint32 x0, Uint32 y0, Uint32 x1, Uint32 y1, const PG_Color& color, Uint8 width) {
	int deltax;
	int deltay;
	int temp;
	int pixelflag;

	if (!width || !surface)
		return;

	pixelflag=0;

	/* swap y0 with y1 if y0 is greater than y1 */
	if (y0 > y1) {
		temp = y0;
		y0 = y1;
		y1 = temp;
		temp = x0;		/* switch the xs */
		x0 = x1;
		x1 = temp;

	} /* end of the if */

	/* handle four separate cases */
	deltax = x1 - x0;
	deltay = y1 - y0;
	if (deltax > 0) {
		if (deltax > deltay) {
			octant0(surface, x0, y0, deltax, deltay, 1, color, width, pixelflag);
		} /* end of if */
		else {
			octant1(surface, x0, y0, deltax, deltay, 1, color, width, pixelflag);
		} /* end if else */

	} /* end of the if */
	else {
		deltax = -deltax;		/* abs value of delta x */
		if (deltax > deltay) {
			octant0(surface, x0, y0, deltax, deltay, -1, color, width, pixelflag);
		} /* end of the if */
		else {
			/* not correct yet */
			octant1(surface, x0, y0, deltax, deltay, -1, color, width, pixelflag);
		} /* end of if-else */
	} /* end of if - else */

} /* end of the function */
