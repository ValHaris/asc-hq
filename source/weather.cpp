//     $Id: weather.cpp,v 1.4 2000-10-12 22:24:03 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.3  1999/11/22 18:28:08  mbickel
//      Restructured graphics engine:
//        VESA now only for DOS
//        BASEGFX should be platform independant
//        new interface for initialization
//      Rewrote all ASM code in C++, but it is still available for the Watcom
//        versions
//      Fixed bugs in RLE decompression, BI map importer and the view calculation
//
//     Revision 1.2  1999/11/16 03:42:51  tmwilson
//     	Added CVS keywords to most of the files.
//     	Started porting the code to Linux (ifdef'ing the DOS specific stuff)
//     	Wrote replacement routines for kbhit/getch for Linux
//     	Cleaned up parts of the code that gcc barfed on (char vs unsigned char)
//     	Added autoconf/automake capabilities
//     	Added files used by 'automake --gnu'
//
//
/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-1999  Martin Bickel  and  Marc Schellenberger

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


#include <math.h>
#include <stdio.h>
#include "tpascal.inc"
#include "misc.h"
#include "basegfx.h"
#include "newfont.h"
#include "keybp.h"
#include "typen.h"
#include "loaders.h"
#include "spfst.h"
#include "weather.h"
#include "dlg_box.h"

static const double pi = 3.141592654;


int          tfillpolygon::checkline( tpunkt a, tpunkt b, tpunkt d, tpunkt e )
{
   double r,s,t;

   b.x -= a.x;
   b.y -= a.y;

   e.x -= d.x;
   e.y -= d.y;

   t = b.x * e.y - b.y * e.x;
   if (t != 0) {
     double m = ( a.x * b.y - a.y * b.x + d.y * b.x - d.x * b.y );
     r = m / -t;

     double n = ( a.y * e.x - a.x * e.y + d.x * e.y - d.y * e.x );
     s = n / t;
     if ( s>=0 && s <= 1  &&  r>=0 && r <= 1)
        return 0;

   } /* endif */
   return 1;
}


int          tfillpolygon::checkpolygon   ( ppolygon poly )
{
    if ( poly->vertexnum > 1  && poly->vertexnum < 3 )
       return 1;
    else { 
       int i,j,k;

       for ( i = 0; i < poly->vertexnum; i++ )
          for ( j = 0; j < poly->vertexnum; j++ )
             if ( i != j )
               if ( poly->vertex[i].x == poly->vertex[j].x  &&
                    poly->vertex[i].y == poly->vertex[j].y )
                  return 0;
   
       k = poly->vertexnum -1;
       for (i=0; i < k ; i++) {
          for (j = i +2; j < k ; j++)
              if (checkline ( poly->vertex[i], poly->vertex[i+1], poly->vertex[j], poly->vertex[j+1] ) == 0)
                  return 0;
          if (i > 0 && j == k)
            if (checkline ( poly->vertex[i], poly->vertex[i+1], poly->vertex[j], poly->vertex[0] ) == 0)
               return 0;
       } /* endfor */
       return 1;
    }
}



double       tfillpolygon::getsegmentdir   ( int dx, int dy )
{
   double alpha;

   if (dx) {
      alpha = atan ( (double) dy / (double) dx );
      if ( dy < 0 ) {
         if ( dx < 0)
            alpha = -pi + alpha;
      } else
        if ( dx < 0)
           alpha = pi - alpha;

   } else {
      if (dy > 0)
         alpha = pi/2;
      else
         alpha = -pi/2;
   } /* endif */

   return alpha;
}

int          tfillpolygon::getpolydir     ( ppolygon a)
{
    int p ;
    double alpha, beta, gamma;
    int   r = 0;
    for (p=0; p< a->vertexnum; p++) {

       if (p == 0)
          alpha = getsegmentdir( a->vertex[p].x - a->vertex[a->vertexnum-1].x , a->vertex[p].y - a->vertex[a->vertexnum-1].y );
       else
          alpha = getsegmentdir( a->vertex[p].x - a->vertex[p-1].x, a->vertex[p].y - a->vertex[p-1].y );

       if (p+1 == a->vertexnum)
          beta =  getsegmentdir( a->vertex[0].x - a->vertex[p].x , a->vertex[0].y - a->vertex[p].y  );
       else
          beta =  getsegmentdir( a->vertex[p+1].x - a->vertex[p].x,  a->vertex[p+1].y - a->vertex[p].y );

       gamma = beta - alpha ;
       if (gamma > pi)
          gamma = pi - gamma;
       if (gamma < -pi)
          gamma = -pi - gamma;
       r+= (int) ( gamma * 1000 );
    }
    return r;
}


void         tfillpolygon::sortpolygon   ( ppolygon a )
{
   if ( a->vertexnum >= 3 )
      if ( getpolydir(a) < 0 ) {
         int    i;
         tpunkt p;
         for (i=1;i<= (a->vertexnum-1)/2 ;i++ ) {
            p = a->vertex[i];
            a->vertex[i] = a->vertex[a->vertexnum-i];
            a->vertex[a->vertexnum-i] = p;
         } /* endfor */
      }
}


void         tfillpolygon::painttriangle  ( tpunkt p[] )
{ 
   int x,y,dx1,dx2,dy1,dy2,i;

   for (i=0; i<2 ; ) {
      if ( p[i+1].y < p[i].y || ( p[i+1].y == p[i].y && p[i+1].x < p[i].x )) {
         x        = p[i].x;
         p[i].x   = p[i+1].x;
         p[i+1].x = x;
         y        = p[i].y;
         p[i].y   = p[i+1].y;
         p[i+1].y = y;
         i        = 0;
       } else
         i++;

   } /* endfor */


   if ( p[0].y < p[1].y ) {
      dx1 = p[1].x - p[0].x;
      dx2 = p[2].x - p[0].x;
      dy1 = p[1].y - p[0].y;
      dy2 = p[2].y - p[0].y;

      for (y=p[0].y; y<=p[1].y ; y++)
          painthorizline ( p[0].x + dx1 * (y - p[0].y) / dy1 , p[0].x + dx2 * (y - p[0].y) / dy2, y );

      if (p[1].y < p[2].y) {
         dx1 = p[2].x - p[1].x;
         dy1 = p[2].y - p[1].y;
   
         for ( ; y<=p[2].y ; y++)
             painthorizline ( p[1].x + dx1 * (y - p[1].y) / dy1 , p[0].x + dx2 * (y - p[0].y) / dy2, y );
      }
   } else {
      if ( p[1].y < p[2].y ) {

         dx1 = p[2].x - p[1].x;
         dx2 = p[2].x - p[0].x;
         dy1 = p[2].y - p[0].y;
   
         for (y=p[0].y; y<=p[2].y ; y++)
             painthorizline ( p[1].x + dx1 * (y - p[0].y) / dy1 , p[0].x + dx2 * (y - p[0].y) / dy1, y );
      } else
        painthorizline ( p[0].x, p[2].x, p[0].y );
   }

} 


void   tfillpolygon::painthorizline ( int x1, int x2, int y )
{
   int dx;
   if (x1 > x2)
      dx = -1;
   else
      if (x1 < x2)
         dx = 1;
      else {
         setpoint ( x2, y );
         return;
      }

   int x= x1;

   do {
      setpoint ( x, y );
      x += dx;
   } while ( x != x2 ); /* enddo */
   setpoint ( x, y );
}

void   tfillpolygon::setpoint ( int x, int y )
{
   putpixel ( x, y, 14 );
}


void   tfillpolygon :: paintline      ( int x1, int y1, int x2, int y2 )
{
	float           m, b;
	int             w;
	float           yy1, yy2, xx1, xx2;



    if ( x1 == x2) {
        for (w=y1;w<=y2 ;w++ ) 
           setpoint ( x1, w );
    } else {
       if ( y1 == y2) {
          for (w=x1;w<=x2 ;w++ ) 
               setpoint( w, y1 );
       } else {
        	yy1 = y1;
        	yy2 = y2;
        	xx1 = x1;
        	xx2 = x2;
        	m = (yy2 - yy1) / (xx2 - xx1);
        	b = y1 - m * x1;
        	if ((m <= 1) && (m >= -1)) {
        		if (x2 < x1) {
        			w = x2;
        			x2 = x1;
        			x1 = w;
        			w = y2;
        			y2 = y1;
        			y1 = w;
        		}
        		for (w = x1; w <= x2; w++) 
        			setpoint(w, (int) (m * w + b) );
        		
        	} else {
        		if (y2 < y1) {
        			w = x2;
        			x2 = x1;
        			x1 = w;
        			w = y2;
        			y2 = y1;
        			y1 = w;
        		}
        		for (w = y1; w <= y2; w++) {
        			setpoint( (int) ((w - b) / m), w );
        		}
        
        	}
         } /* endif */
    }
}



typedef int int3[3];
typedef double double2[2];

int   tfillpolygon::paint_polygon   ( ppolygon poly )
{

   if (checkpolygon ( poly )) {
      if ( poly->vertexnum == 1 ) {
         setpoint ( poly->vertex[0].x, poly->vertex[0].y );
         return 1;
       }
       if ( poly->vertexnum == 2 ) {
         paintline ( poly->vertex[0].x, poly->vertex[0].y, poly->vertex[1].x, poly->vertex[1].y  );
         return 1;
       }
      
    
       sortpolygon ( poly );
    
    
       double2*  floatpoints = new double2 [ poly->vertexnum+1 ];
       int i;
       for (i=0; i<poly->vertexnum ; i++ ) {
          floatpoints[i+1][0] = (double) poly->vertex[i].x;
          floatpoints[i+1][1] = (double) poly->vertex[i].y;
       } /* endfor */
    
       int3* triangles;
       triangles = new int3  [ poly->vertexnum-1 ];
    
       int contours[1];
       contours[0] = poly->vertexnum;
    
       triangulate_polygon ( 1, contours, floatpoints, triangles );
    
       tpunkt tripos[3];
    
       for (i=0; i<poly->vertexnum-2 ;i++ ) {
          tripos[0] = poly->vertex[triangles[i][0]-1];
          tripos[1] = poly->vertex[triangles[i][1]-1];
          tripos[2] = poly->vertex[triangles[i][2]-1];
          painttriangle ( tripos );
       } /* endfor */
    
       delete  ( triangles );
       delete  ( floatpoints );
       return 1;
   } else
     return 0;


}









void tfillpolygonweather::setpointabs    ( int x,  int y  )
{
   pfield ffield = getfield ( x , y );
   if (ffield)
     if (ffield->typ->terraintype->weather[ weather ] ) {
        ffield->typ = ffield->typ->terraintype->weather[ weather ];
        ffield->setparams();
     } else {
        ffield->typ = ffield->typ->terraintype->weather[ 0 ];
        ffield->setparams();
     }


}



void tfillpolygonweather::initweather ( int wt, int it )
{
  weather = wt;
  intensity = it;
}



void       tfillpolygonbdt::initbdt ( int id )
{
   bdt = getterraintype_forid ( id );
}



void       tfillpolygonsquarecoord::setpoint    ( int x,  int y  )
{
    if ( (x & 1) == (y & 1) ) 
       setpointabs ( x / 2, y);
}

int    tfillpolygonsquarecoord :: paint_polygon   ( ppolygon poly1 )
{
   tpolygon poly;
   poly.vertexnum = poly1->vertexnum;
   poly.vertex    = new tpunkt [ poly.vertexnum ];

   for ( int i=0; i< poly.vertexnum ; i++ ) {
      poly.vertex[i].x = poly1->vertex[i].x * 2 + (poly1->vertex[i].y & 1);  // auf quadratische Koordinaten umrechenen
      poly.vertex[i].y = poly1->vertex[i].y;
   } /* endfor */


   int result = tfillpolygon:: paint_polygon ( &poly );

   delete[] poly.vertex;
   return result;
}


void       tfillpolygonbdt::setpointabs    ( int x,  int y  )
{
    pfield ffield = getfield ( x , y );
    if (ffield) {
      int i, j=0;
      for (i=0; i < cwettertypennum; i++)
         if (ffield->typ == ffield->typ->terraintype->weather[i])
            j = i;
      if (bdt->weather[j] == NULL)
         j = 0;
 
      ffield->typ = bdt->weather[ j ];
      ffield->setparams();
    }
}





/* Read in the list of vertices from infile */
ppolygon read_segments2(char* filename)
{       
  FILE *infile;
  int ccount;
    int i;
  int ncontours, npoints, first, last;
  ppolygon poly;

  if ((infile = fopen(filename, "r")) == NULL)
    {
      perror(filename);
      return NULL;
    }

  ncontours = 1;

  /* For every contour, read in all the points for the contour. The */
  /* outer-most contour is read in first (points specified in */
  /* anti-clockwise order). Next, the inner contours are input in */
  /* clockwise order */

  ccount = 0;
  i = 1;
  
  poly = new (tpolygon);

  while (ccount < ncontours)
    {
      int j;
      double x,y;

      fscanf(infile, "%d", &npoints);

      poly->vertexnum = npoints;
      poly->vertex = new tpunkt [ npoints ];

      first = i;
      last = first + npoints - 1;
      for (j = 0; j < npoints; j++, i++)
	{
	  fscanf(infile, "%lf%lf", &x, &y);
          poly->vertex[j].x = (int) x*10;
          poly->vertex[j].y = (int) y*10;
	}

      ccount++;
    }

  return poly;
}





/*
main( void )
{
   initsvga( 0x101 );
   ppolygon pol = read_segments2 ( "data_1" );

   tpunkt triang[3] = { {10,10},{20,20},{30,10} };

   tfillpolygon poly;


   if ( poly.paintpolygon ( pol )) {
      getche();
      settextmode( 3 );
   } else {
      settextmode( 3 );
      printf( " \nInvalid polygon: crossing lines !\n");
   }

}

*/



void setweatherpoly ( int weather, int intensity, ppolygon poly1 )
{
   tfillpolygonweather fillpoly;
   fillpoly.initweather ( weather, intensity );
   if (fillpoly.paint_polygon ( poly1 ) == 0)
      displaymessage("Invalid Polygon !",1 );
}

void setweatherall ( int weather, int intensity )
{
   int x,y;
   tfillpolygonweather fillpoly;

   fillpoly.initweather ( weather, intensity );

   for (y=0; y < actmap->ysize; y++)
     for (x=0; x < actmap->xsize; x++) 
        fillpoly.setspecificpoint ( x, y );

}

void setweatherfields ( int weather, int intensity, int fieldnum, int* coordinates )
{
   tfillpolygonweather fillpoly;

   fillpoly.initweather ( weather, intensity );

   int i;
   for (i=0; i< fieldnum ; i++ ) 
        fillpoly.setspecificpoint ( *(coordinates++), *(coordinates++) );

}










void setbdtpoly ( int id, ppolygon poly1 )
{
   tfillpolygonbdt fillpoly;

   fillpoly.initbdt ( id );
   if (fillpoly.paint_polygon ( poly1 ) == 0)
      displaymessage("Invalid Polygon !",1 );
}

void setbdtall ( int id )
{
   int x,y;
   tfillpolygonbdt fillpoly;

   fillpoly.initbdt ( id );

   for (y=0; y < actmap->ysize; y++)
     for (x=0; x < actmap->xsize; x++) 
        fillpoly.setspecificpoint ( x, y );

}


void setbdtfields ( int id, int fieldnum, int* coordinates )
{
   tfillpolygonbdt fillpoly;

   fillpoly.initbdt ( id );

   int i;
   for (i=0; i< fieldnum ; i++ ) 
        fillpoly.setspecificpoint ( *(coordinates++), *(coordinates++) );

}


void displaywindspeed( void )
{
   displaymessage2("wind speed is %d; %d; %d ; effective 0 speed is %d = %s fields.", actmap->weather.wind[0].speed, actmap->weather.wind[1].speed, actmap->weather.wind[2].speed, actmap->weather.wind[0].speed * maxwindspeed / 256, strrrd8d ( actmap->weather.wind[0].speed * maxwindspeed / 256 ));
}

