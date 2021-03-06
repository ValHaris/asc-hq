/*! \file basegfx.cpp
    \brief Platform indepedant graphic functions.
*/

/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2010  Martin Bickel  and  Marc Schellenberger

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

#include <cstring>
#include <cstdlib>
#include <math.h>
#include "global.h"
#include "basegfx.h"
#include "misc.h"
#include "newfont.h"

#ifdef _DOS_
 #include "dos/vesa.h"
#else
 #include "sdl/graphics.h"
#endif

tgraphmodeparameters *agmp = (tgraphmodeparameters *) & activegraphmodeparameters;
tgraphmodeparameters *hgmp = (tgraphmodeparameters *) & hardwaregraphmodeparameters;

int xlatbuffersize = 66000;

void generategrayxlattable( ppixelxlattable tab, Uint8 offset, Uint8 size, dacpalette256* pal )
{
	for ( int b = 0; b <= 255; b++) {
//		(*tab)[b] = (Uint8) (offset + size - 1 - ((*activepalette256)[b][0] + (*activepalette256)[b][1] + (*activepalette256)[b][2]) * size / 192);
		(*tab)[b] = (Uint8) (offset + size - 1 - ( 0.299 * (*pal)[b][0] + 0.587 * (*pal)[b][1] + 0.114 * (*pal)[b][2]) * size / 64 );
	}
}

void getpicsize( void* hd2, int &width, int &height)
{
   trleheader* hd = (trleheader*) hd2;
   if (hd->id == 16973) {
      width = hd->x;
      height = hd->y;
   } else {
     width = hd->id + 1;
     height = hd->size + 1;
   }
}

int getpicsize2( void* hd2 )
{
   trleheader* hd = (trleheader*) hd2;
   int width; 
   int height;
   if (hd->id == 16973) 
      return hd->size + sizeof(*hd);
   else {
     width = hd->id + 1;
     height = hd->size + 1;
     return height*width+4;
   }
}




void 
rahmen(bool invers,
       int x1,
       int y1,
       int x2,
       int y2)
{
	collategraphicoperations cgo ( x1, y1, x2, y2 );
	int col	=	(invers == false)	?	white	:	darkgray;

	line(x1, y1, x1, y2, col);
	line(x1, y1, x2, y1, col);
	
	col	=		(invers == true)	?	white	:	darkgray;
	line(x2, y1, x2, y2, col);
	line(x1, y2, x2, y2, col);
}

void tdrawline :: start ( int x1, int y1, int x2, int y2 )
{ 
   int      i, deltax, deltay, numpixels, d, dinc1, dinc2, x, xinc1, xinc2, y, yinc1, yinc2; 


    /*  calculate deltax and deltay for initialisation  */ 
    
  deltax = x2 - x1; 
  if ( deltax < 0 )
     deltax = -deltax;

  deltay = y2 - y1; 
  if ( deltay < 0 )
     deltay = -deltay;

    /*  initialize all vars based on which is the independent variable  */ 
  if (deltax >= deltay) 
    { 

        /*  x is independent variable  */ 
      numpixels = deltax + 1; 
      d = (2 * deltay) - deltax; 
      dinc1 = deltay << 1; 
      dinc2 = (deltay - deltax) << 1; 
      xinc1 = 1; 
      xinc2 = 1; 
      yinc1 = 0; 
      yinc2 = 1; 
    } 
  else 
    { 

        /*  y is independent variable  */ 
      numpixels = deltay + 1; 
      d = (2 * deltax) - deltay; 
      dinc1 = deltax << 1; 
      dinc2 = (deltax - deltay) << 1; 
      xinc1 = 0; 
      xinc2 = 1; 
      yinc1 = 1; 
      yinc2 = 1; 
    } 

    /*  make sure x and y move in the right directions  */ 
  if (x1 > x2) 
    { 
      xinc1 = -xinc1; 
      xinc2 = -xinc2; 
    } 
  if (y1 > y2) 
    { 
      yinc1 = -yinc1; 
      yinc2 = -yinc2; 
    } 

    /*  start drawing at <x1, y1>  */ 
  x = x1; 
  y = y1; 

    /*  draw the pixels  */ 
  for (i = 1; i <= numpixels; i++) 
    { 
      putpix( x, y ); 


      if (d < 0) 
        { 
          d = d + dinc1; 
          x = x + xinc1; 
          y = y + yinc1; 
        } 
      else 
        { 
          d = d + dinc2; 
          x = x + xinc2; 
          y = y + yinc2; 
        } 
    } 
} 


void 
line(int  x1,
     int  y1,
     int  x2,
     int  y2,
     Uint8 actcol)
{
   collategraphicoperations cgs ( x1, y1, x2, y2 );
	float           m, b;
	int             w;
	float           yy1, yy2, xx1, xx2;



    if ( x1 == x2) {
        for (w=y1;w<=y2 ;w++ ) 
           putpixel(x1, w, actcol );

    } else {
       if ( y1 == y2) {
          for (w=x1;w<=x2 ;w++ ) 
               putpixel(w, y1, actcol );
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
        			putpixel(w, (int) (m * w + b), actcol);
        		
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
        			putpixel((int) ((w - b) / m), w, actcol);
        		}
        
        	}
         } /* endif */
     }

}

void 
xorline(int  x1,
     int  y1,
     int  x2,
     int  y2,
     Uint8 actcol)
{
   collategraphicoperations cgs ( x1, y1, x2, y2 );

	float           m, b;
	int             w;
	float           yy1, yy2, xx1, xx2;

    if ( x1 == x2) {
        for (w=y1;w<=y2 ;w++ ) {
		putpixel( x1, w, getpixel ( x1, w ) ^ actcol);
        } /* endfor */
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
		for (w = x1; w <= x2; w++) {
			putpixel(w, (int) (m * w + b), getpixel( w, (int) (m * w + b) ) ^ actcol);
		}
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
			putpixel((int) ((w - b) / m), w, getpixel ( (int) ((w - b) / m), w ) ^ actcol);             
		}

	}
     }

}



void 
rectangle(int x1,
	  int y1,
	  int x2,
	  int y2,
	  Uint8 color)
{
   collategraphicoperations cgs ( x1, y1, x2, y2 );

	line(x1, y1, x1, y2, color);
	line(x1, y1, x2, y1, color);
	line(x2, y1, x2, y2, color);
	line(x1, y2, x2, y2, color);

}


void xorrectangle(int x1,
	     int y1,
	     int x2,
	     int y2,
	     Uint8 color)
{
   collategraphicoperations cgs ( x1, y1, x2, y2 );

          xorline(x1,y1,x1,y2,color); 
          xorline(x1,y1,x2,y1,color);
          xorline(x2,y1,x2,y2,color); 
          xorline(x1,y2,x2,y2,color);

}

/*
void         xorrectangle( tmouserect r, byte         color)
{
   xorrectangle ( r.x1, r.y1, r.x2, r.y2, color );
}

  */


void* halfpict ( void* vbuf )
{
   Uint8* buf = (Uint8*) vbuf;

   Uint16* wp = (Uint16*) xlatbuffer;
   Uint8* dest = (Uint8*) xlatbuffer;

   trleheader*   hd = (trleheader*) vbuf; 

   if ( hd->id == 16973 ) { 
      wp[0] = hd->x / 2;
      wp[1] = hd->y / 2;

      if ( ( wp[0] + 1 ) * ( wp[1] + 1 ) + 4 >= xlatbuffersize )
         throw fatalgraphicserror ( "halfpict : picture larger than buffer ! " );

      dest += 4;

      buf += sizeof ( *hd );


      int linecount = 0;
      int rowcount = 0;

      for ( int c = 0; c < hd->size; c++ ) {
         if ( *buf == hd->rle ) {
            for ( int i = buf[1]; i > 0; i-- ) {
               if ( !(linecount & 1)  &&  !(rowcount & 1)) {
                  *dest = buf[2];
                  dest++;
               }
               rowcount++;
               if ( rowcount > hd->x ) {
                  rowcount = 0;
                  linecount++;
               }
            }

            buf += 3;
            c += 2;

         } else {
            if ( !(linecount & 1)  &&  !(rowcount & 1)) {
               *dest = *buf;
               dest++;
            }
            buf++;
            rowcount++;
            if ( rowcount > hd->x ) {
               rowcount = 0;
               linecount++;
            }
         }
         if ( (PointerSizedInt)dest - (PointerSizedInt)xlatbuffer > xlatbuffersize )
            throw fatalgraphicserror ( "halfpict : picture larger than buffer ! " );

      }
   } else {
      int linecount = 0;
      int rowcount = 0;

      Uint16* wp2 = (Uint16*) vbuf;

      wp[0] = wp2[0] / 2;
      wp[1] = wp2[1] / 2;

      if ( ( wp[0] + 1 ) * ( wp[1] + 1 ) + 4 >= xlatbuffersize )
         throw fatalgraphicserror ( "halfpict : picture larger than buffer ! " );

      dest += 4;
      buf += 4;

      for ( int c = (wp2[0] + 1) * (wp2[1] + 1); c > 0; c-- ) {
         if ( !(linecount & 1)  &&  !(rowcount & 1)) {
            *dest = *buf;
            dest++;
         }
         buf++;
         rowcount++;
         if ( rowcount > wp2[0] ) {
            rowcount = 0;
            linecount++;
         }
      }

   }
   return xlatbuffer;
} 

#if 0
void putshadow ( int x1, int y1, void* ptr, ppixelxlattable xl )
{
   Uint16* w = (Uint16*) ptr;
   Uint8* c = (Uint8*) ptr + 4;
   int spacelength = agmp->scanlinelength - *w - 1;

   collategraphicoperations cgo ( x1, y1, x1 + w[0], y1+w[1] );
   if ( agmp->windowstatus == 100 ) {
      Uint8* buf = (Uint8*) (agmp->scanlinelength * y1 + x1 + agmp->linearaddress);
      for ( int y = w[1] + 1; y > 0; y-- ) {
         for ( int x = w[0]+1; x > 0; x-- ) {
            if ( *c != 255 )
               *buf = (*xl)[*buf];
            buf++;
            c++;
         }
         buf+=spacelength;
      }
   }

}
#endif
void putpicturemix ( int x1, int y1, void* ptr, int rotation, Uint8* mixbuf )
{
   Uint16* w = (Uint16*) ptr;
   Uint8* c = (Uint8*) ptr + 4;
   int spacelength = agmp->scanlinelength - *w - 1;

   collategraphicoperations cgo ( x1, y1, x1 + w[0], y1+w[1] );
   if ( agmp->windowstatus == 100 ) {
      Uint8* buf = (Uint8*) (agmp->scanlinelength * y1 + x1 + agmp->linearaddress);
      for ( int y = w[1] + 1; y > 0; y-- ) {
         for ( int x = w[0]+1; x > 0; x-- ) {
            if ( *c != 255 ) {
               int o = *buf << 8;
               if ( *c >= 16  && *c < 24 )
                  *buf = mixbuf[o + *c + rotation];
               else
                  *buf = mixbuf[o + *c];
            }
            buf++;
            c++;
         }
         buf+=spacelength;
      }
   }

}


void putinterlacedrotimage ( int x1, int y1, void* ptr, int rotation )
{
   Uint16* w = (Uint16*) ptr;
   Uint8* c = (Uint8*) ptr + 4;
   int spacelength = agmp->scanlinelength - *w - 1;

   collategraphicoperations cgo ( x1, y1, x1 + w[0], y1+w[1] );
   if ( agmp->windowstatus == 100 ) {
      Uint8* buf = (Uint8*) (agmp->scanlinelength * y1 + x1 + agmp->linearaddress);
      for ( int y = w[1] + 1; y > 0; y-- ) {
         for ( int x = w[0]+1; x > 0; x-- ) {
            if ( *c != 255 ) {
               if ( ((PointerSizedInt)(buf+y)) & 1 ) {
                  if ( *c >= 16  && *c < 24 )
                     *buf = *c + rotation;
                  else
                     *buf = *c;
               }
            }
            buf++;
            c++;
         }
         buf+=spacelength;
      }
   }

}


void rotatepict90 ( void* s, void* d )
{
   Uint16* sw = (Uint16*) s;
   Uint8* sc = (Uint8*) s + 4;
   
   Uint16* dw = (Uint16*) d;
   Uint8* dc = (Uint8*) d + 4;

   dw[0] = sw[1];
   dw[1] = sw[0];

   int dl = dw[0]+1;
//   int dh = dw[1]+1;

   int sl = sw[0]+1;
   int sh = sw[1]+1;

   for (int y = 0; y <= dw[1]; y++) 
     for (int x = 0; x <= dw[0]; x++) 
         dc[ y * dl + x] = sc[ ( sh - x - 1 ) * sl + y];
}
  union tpix {
  struct { Uint8 r,g,b,a; } s;
  int all;
};


typedef tpix timage[ 100 ][ 100 ];

int getimagepixel ( void* image, int x, int y )
{
   int xs, ys;
   getpicsize ( image, xs, ys );


   y += ys/2;
   x += xs/2;
   if ( x < 0  || x >= xs || y < 0 || y >= ys )
      return -1;
   else {
      Uint8* pc = (Uint8*) image;
      return pc[ 4 + y * xs + x];
   }
}

const float pi = 3.14159265;

Uint8* rotatepict ( void* image, int organgle )
{
   int fieldxsize, fieldysize;
   getpicsize(image, fieldxsize, fieldysize );

   float angle = ((float)organgle) / 360 * 2 * pi + pi;

   Uint8* dst = new Uint8[ imagesize ( 0, 0, fieldxsize, fieldysize ) ];
   Uint16* wp = (Uint16*) dst;
   wp[0] = fieldxsize-1;
   wp[1] = fieldysize-1;

   Uint8* pnt  = dst + 4;

   for ( int y = 0; y < fieldysize; y++ ) {
      for ( int x = 0; x < fieldxsize; x++ ) {
         int dx = x - fieldxsize/2 ;
         int dy = fieldysize/2 - y;
         double nx = 0;
         double ny = 0;
         if ( organgle != 0 && organgle != -180 && organgle != 180) {
            float wnk ;
            if ( dx  )
               wnk = atan2 ( double(dy), double(dx) );
            else
               if ( dy > 0 )
                  wnk = pi/2;
               else
                  wnk = -pi/2;

            wnk -= angle;
            float radius = sqrt ( double(dx * dx + dy * dy ));

            nx = radius * cos ( wnk );
            ny = radius * sin ( wnk );
         } else
            if ( organgle == 0 ) {
               nx = -dx;
               ny = -dy;
            } else
               if ( organgle == 180 || organgle == -180) {
                  nx = dx;
                  ny = dy;
               }


         int newpix = getimagepixel ( image, (int)-nx, (int)ny );
         if ( newpix == -1 )
            *pnt = 255;
         else
            *pnt = newpix;

         pnt++;
      }
   }

   return dst;
}


Uint8* rotatepict_grw ( void* image, int organgle )
{
   int fieldxsize, fieldysize;
   getpicsize(image, fieldxsize, fieldysize );

   float angle = ((float)organgle) / 360 * 2 * pi + pi;

   int d = int(sqrt(double(fieldxsize*fieldxsize + fieldysize*fieldysize )));

   Uint8* dst = new Uint8[ imagesize ( 0, 0, d,d ) ];
   Uint16* wp = (Uint16*) dst;
   wp[0] = d-1;
   wp[1] = d-1;

   Uint8* pnt  = dst + 4;

   for ( int y = 0; y < d; y++ ) {
      for ( int x = 0; x < d; x++ ) {
         int dx = x - d/2 ;
         int dy = d/2 - y;
         float nx = 0;
         float ny = 0;
         if ( organgle != 0 && organgle != -180 && organgle != 180) {
            float wnk ;
            if ( dx  )
               wnk = atan2 ( double(dy), double(dx) );
            else
               if ( dy > 0 )
                  wnk = pi/2;
               else
                  wnk = -pi/2;

            wnk -= angle;
            float radius = sqrt ( double(dx * dx + dy * dy ));

            nx = radius * cos ( wnk );
            ny = radius * sin ( wnk );
         } else
            if ( organgle == 0 ) {
               nx = -dx;
               ny = -dy;
            } else
               if ( organgle == 180 || organgle == -180) {
                  nx = dx;
                  ny = dy;
               }


         int newpix = getimagepixel ( image, (int)-nx, (int)ny );
         if ( newpix == -1 )
            *pnt = 255;
         else
            *pnt = newpix;

         pnt++;
      }
   }

   return dst;
}


void flippict ( void* s, void* d, int dir )
{
   Uint16* sw = (Uint16*) s;
   Uint8* sc = (Uint8*) s + 4;
   
   Uint16* dw = (Uint16*) d;
   Uint8* dc = (Uint8*) d + 4;

   dw[1] = sw[1];
   dw[0] = sw[0];

   int sl = sw[0]+1;
   int sh = sw[1]+1;

   if ( dir == 1 ) {
      for (int y = 0; y <= dw[1]; y++) 
        for (int x = 0; x <= dw[0]; x++) 
            dc[ y * sl + x] = sc[ y * sl + ( sl - 1 - x ) ];
   } else {
      for (int y = 0; y <= dw[1]; y++) 
        for (int x = 0; x <= dw[0]; x++) 
            dc[ y * sl + x] = sc[ ( sh - 1 - y)  * sl + x ];
   }
}




void putpixel8 ( int x1, int y1, int color )
{
    collategraphicoperations cgo ( x1, y1, x1, y1 );
    Uint8* buf = (Uint8*) (agmp->scanlinelength * y1 + x1 * agmp->byteperpix + agmp->linearaddress);
    *buf = color;
}

int getpixel8 ( int x1, int y1 )
{
    Uint8* buf = (Uint8*) (agmp->scanlinelength * y1 + x1 * agmp->byteperpix + agmp->linearaddress);
    return *buf;
}


void putpixel(int x1, int y1, int color)
{
   collategraphicoperations cgo ( x1, y1, x1, y1 );
   if ( agmp->byteperpix == 1 )
      putpixel8 ( x1, y1, color );
   else {
      if ( agmp->windowstatus == 100 ) {
         Uint8* pc = (Uint8*) ( agmp->linearaddress + x1 * agmp->byteperpix + y1 * agmp->scanlinelength );
         int alpha = color >> 24;
         if ( alpha == 0 ) {
            pc[ agmp->redfieldposition/8 ] = color & 0xff;
            pc[ agmp->greenfieldposition/8 ] = (color >> 8) & 0xff;
            pc[ agmp->bluefieldposition/8 ] = (color >> 16) & 0xff;
         } else {
            pc[ agmp->redfieldposition/8 ] = pc[ agmp->redfieldposition/8 ] * alpha / alphabase + (color & 0xff) * (alphabase - alpha ) / alphabase;
            pc[ agmp->greenfieldposition/8 ] = pc[ agmp->greenfieldposition/8 ] * alpha / alphabase + ((color >> 8 ) & 0xff) * (alphabase - alpha ) / alphabase;
            pc[ agmp->bluefieldposition/8 ] = pc[ agmp->bluefieldposition/8 ] * alpha / alphabase + ((color >> 16) & 0xff) * (alphabase - alpha ) / alphabase;
         }
      } else {
        /*
         int pos = x1 * agmp->byteperpix + y1 * agmp->scanlinelength;
         int page = pos >> 16;
         if ( hgmp->actsetpage != page )
            setvirtualpagepos ( page );

         Uint8* pc = (Uint8*) ( agmp->linearaddress + (pos & 0xffff) );

         int alpha = color >> 24;
         if ( alpha == 0 ) {
            pc[ agmp->redfieldposition/8 ] = color & 0xff;
            pc[ agmp->greenfieldposition/8 ] = (color >> 8) & 0xff;
            pc[ agmp->bluefieldposition/8 ] = (color >> 16) & 0xff;
         } else {
            pc[ agmp->redfieldposition/8 ] = pc[ agmp->redfieldposition/8 ] * alpha / alphabase + (color & 0xff) * (alphabase - alpha ) / alphabase;
            pc[ agmp->greenfieldposition/8 ] = pc[ agmp->greenfieldposition/8 ] * alpha / alphabase + ((color >> 8 ) & 0xff) * (alphabase - alpha ) / alphabase;
            pc[ agmp->bluefieldposition/8 ] = pc[ agmp->bluefieldposition/8 ] * alpha / alphabase + ((color >> 16) & 0xff) * (alphabase - alpha ) / alphabase;
         }
        */ 
      }
   }

}

int getpixel(int x1, int y1)
{
   if ( agmp->byteperpix == 1 )
      return getpixel8( x1, y1 );
   else {
      if ( agmp->windowstatus == 100 ) {
         Uint8* pc = (Uint8*) ( agmp->linearaddress + x1 * agmp->byteperpix + y1 * agmp->scanlinelength );
         trgbpixel pix;
         pix.channel.r = pc[ agmp->redfieldposition/8 ];
         pix.channel.g = pc[ agmp->greenfieldposition/8 ];
         pix.channel.b = pc[ agmp->bluefieldposition/8 ];
         pix.channel.a = 0;
         return pix.rgb;
      } else {
        return -1;
      }
   }
}




TrueColorImage :: TrueColorImage ( int x, int y )
{
   pix = new trgbpixel[x*y];
   xsize = x;
   ysize = y;
}

TrueColorImage :: ~TrueColorImage (  )
{
   if ( pix ) {
      delete[] pix;
      pix = NULL;
   }
}


int getpixelfromimage ( void* buf, int x, int y )
{
   Uint16* wp = (Uint16*) buf;
   if ( x > wp[0]  ||  y > wp[1] || x < 0 || y < 0 )
      return -1;

   Uint8* pc = (Uint8*) buf;
   return pc[4 + x + y * (wp[0]+1) ];
}

void TrueColorImage :: setpix ( int x, int y, int r, int g, int b, int alpha )
{
   trgbpixel* p = & pix[ x + y * xsize ];
   p->channel.r = r;
   p->channel.g = g;
   p->channel.b = b;
   p->channel.a = alpha;
}

void TrueColorImage :: setpix ( int x, int y, const trgbpixel& _pix )
{
   pix[ x + y * xsize ] = _pix;
}


trgbpixel TrueColorImage :: getpix ( int x, int y )
{
   return pix[ x + y * xsize ];
}


int TrueColorImage :: getxsize( void )
{
   return xsize;
}

int TrueColorImage :: getysize( void )
{
   return ysize;
}



#define f2i(x) (int(x))
#define f2i2(x) (int(x+0.5))


int newpalgenerated = 0;
dacpalette256 ppal;






TrueColorImage* smoothimage ( TrueColorImage* src )
{
  TrueColorImage* dst = new TrueColorImage ( src->getxsize(), src->getysize() );
  for ( int y = 0; y < src->getysize(); y++ )
     for ( int x = 0; x < src->getxsize(); x++ ) {
        if ( src->getpix ( x, y ).channel.a < alphabase / 2 ) {
           int cnt = 0;
           int r = 0;
           int g = 0;
           int b = 0;
           for ( int a = -1; a <= 1; a++ )
              for ( int bb = -1; bb <= 1; bb++ ) {
                  int nx = x+a;
                  int ny = y+bb;
                  if ( nx >= 0 && ny >= 0 && nx < src->getxsize() && ny < src->getysize()) {
                     trgbpixel p = src->getpix( nx, ny );
                     if ( p.channel.a < alphabase/2 ) {
                        cnt++;
                        r += p.channel.r;
                        g += p.channel.g;
                        b += p.channel.b;
                     }
                  }
              }

           dst->setpix ( x, y, r/cnt, g/cnt, b/cnt, 0 );
        } else {
           dst->setpix ( x, y, 0, 0, 0, alphabase );
        }
     }
  return dst;
}

#define sqr(a) (a)*(a)
#define cub(a) abs ((a)*(a)*(a))


#ifndef minimal
 #include "basestrm.h"
#endif


int
imagesize(int x1, int y1, int x2, int y2)
{
	return ((x2 - x1 + 1) * (y2 - y1 + 1) + 4);
}

#if 0
Uint8* convertimage ( TrueColorImage* img, dacpalette256 pal )
{
  #ifndef minimal
   if ( truecolor2pal_table[0] == 255 ) {
      tfindfile ff ( "tc2pal.dat" );
      if ( !ff.getnextname().empty() ) {
         tnfilestream stream ( "tc2pal.dat", tnstream::reading );
         stream.readdata ( truecolor2pal_table, sizeof ( truecolor2pal_table ));
      } else {
         for ( int r = 0; r < 64; r++ )
            for ( int g = 0; g < 64; g++ )
               for ( int b = 0; b < 64; b++ ) {
                     int sml = r  + ( g << 6) + ( b << 12 );
      
                     int diff = 0xFFFFFFF;
                     int pix1 = 0;
            
                     for ( int k=0;k<256 ;k++ ) {
                        int actdif = sqr( pal[k][0]  - r ) + sqr( pal[k][1]  - g ) + sqr( pal[k][2]  - b );

                        if (actdif < diff) {
                           diff = actdif;
                           pix1 = k;
                        }
                     } 
                    truecolor2pal_table[sml] = pix1;
               }
       /*
         tnfilestream stream ( "tc2pal.dat", 2 );
         stream.writedata ( truecolor2pal_table, sizeof ( truecolor2pal_table ));
       */
      }
   }
  #endif
   int size = imagesize ( 1, 1, img->getxsize(), img->getysize() );
   Uint8* newimg = new Uint8[ size ] ;
   Uint8* start = newimg;
   Uint16* wp = (Uint16*) newimg;
   wp[0] = img->getxsize()-1;
   wp[1] = img->getysize()-1;

   newimg+= 4;

   for ( int y = 0; y <= wp[1]; y++ )
      for ( int x = 0; x <= wp[0]; x++ ) {
         trgbpixel p  = img->getpix ( x, y );


         if ( p.channel.a <= alphabase*2/3 ) {
            int sml = ( p.channel.r >> 2) + (( p.channel.g >> 2) << 6) + (( p.channel.b >> 2) << 12);

            *newimg = truecolor2pal_table[sml];

         } else {
           *newimg = 255;
         }
         newimg++;
      }

   if ( newimg - start > size ) 
      printf("\a");
   return (Uint8*)wp;
}
#endif

fatalgraphicserror :: fatalgraphicserror ( const char* strng ) {
   strcpy ( st, strng );
}

fatalgraphicserror :: fatalgraphicserror ( void ) {
   st[0] = 0;
}
                 
void putmask ( int x1, int y1, void* vbuf, int newtransparence )
{
      // int linecount = 0;
      // int rowcount = 0;

      Uint16* wp = (Uint16*) vbuf;

      Uint8* basemembuf = (Uint8*) (agmp->linearaddress + y1 * agmp->bytesperscanline + x1 * agmp->byteperpix );
      Uint8* img = (Uint8*) vbuf + 4;

      for ( int y = 0; y <= wp[1]; y++ ) {
         Uint8* membuf = basemembuf + y * agmp->bytesperscanline;

         for ( int x = 0; x <= wp[0]; x++ ) {
            if ( *img != newtransparence )
               *membuf = *img;
            membuf += agmp->byteperpix;
            img++;
         }
      }
}

int graphicinitialized = 0;



void* uncompress_rlepict ( void* pict )
{
   trleheader* hd = (trleheader*) pict;
   if (hd->id == 16973) {

      int w, h;
      getpicsize( pict, w, h );
      w++;
      h++;
      void* newbuf = malloc ( w * h + 4 );
      Uint16* wp = (Uint16*) newbuf;
      Uint8* dest = (Uint8*) newbuf;
      wp[0] = w-1;
      wp[1] = h-1;
      dest +=4;
   
      Uint8* buf = (Uint8*) pict;
      buf += sizeof ( *hd );
   
      for ( int c = 0; c < hd->size; c++ ) {
         if ( *buf == hd->rle ) {
            for ( int i = buf[1]; i > 0; i-- ) {
               *dest = buf[2];
               dest++;
            }
            buf += 3;
            c += 2;
   
         } else {
            *dest = *buf;
            dest++;
            
            buf++;
         }
      }
      return newbuf;
   } else
      return NULL;
}



void ellipse ( int x1, int y1, int x2, int y2, int color, float tolerance )
{
   collategraphicoperations cgs ( x1, y1, x2, y2 );

   int midx = (x1 + x2) / 2;
   int midy = (y1 + y2) / 2;
   float xr = x2 - x1;
   float yr = y2 - y1;

   tolerance = tolerance / (xr+yr) *  80;

   xr= (xr/2)*(xr/2);
   yr= (yr/2)*(yr/2);

   for ( int y = y1; y <= y2; y++ )
      for ( int x = x1; x <= x2; x++ ) {
         float dx = x - midx;
         float dy = y - midy;
         float tmp = dx*dx/xr + dy*dy/yr;
//         float tmp = dx*dx*yr + dy*dy*xr;
         if (  tmp <= 1 + tolerance && tmp >= 1 - tolerance )
            putpixel ( x, y, color );
      }

}



// Uint8 truecolor2pal_table[262144];


dacpalette256* activepalette256;


class tinitgfxengine {
        public:
          tinitgfxengine ( void );
       } initgfx;

tinitgfxengine::tinitgfxengine ( void )
{
#ifdef use_truecolor2pal
   memset ( truecolor2pal_table, 255, sizeof ( truecolor2pal_table ));
#endif
   hgmp = (tgraphmodeparameters *) & hardwaregraphmodeparameters;
   agmp = (tgraphmodeparameters *) & activegraphmodeparameters;
   activepalette256 = (dacpalette256*) &activepalette;
   xlatbuffer = malloc ( xlatbuffersize );
}


void tvirtualdisplay :: init ( int x, int y, int color, int depth )
{
   agmp = (tgraphmodeparameters *) & activegraphmodeparameters;
   oldparams = *agmp ;
   Uint8* cbuf = (Uint8*) malloc ( x * y * depth/8 );
   if ( !cbuf )
      throw fatalgraphicserror ( "could not allocate memory !");

   buf = cbuf;
   for ( int i = 0; i < (x*y*depth/8); i++ )
      cbuf[i] = color;

   agmp->resolutionx   = x   ;
   agmp->resolutiony   = y   ;
   agmp->windowstatus  = 100 ;
   agmp->scanlinelength  = x*depth/8 ;
   agmp->scanlinenumber  = y ;
   agmp->bytesperscanline  = x * depth/8;
   agmp->byteperpix   = depth/8    ;
   agmp->linearaddress = (PointerSizedInt) buf ;
   agmp->bitperpix          = depth;
   agmp->directscreenaccess = 1;
   if ( depth ==24 || depth==32 ) {
      agmp->redmasksize = 8;
      agmp->greenmasksize = 8;
      agmp->bluemasksize = 8;
      agmp->redfieldposition = 0;
      agmp->greenfieldposition = 8;
      agmp->bluefieldposition = 16;
      surface = agmp->surface = new Surface (  Surface::CreateSurface( buf, x, y, depth, 0xff, 0xff00, 0xff0000) );
   } else {
      surface = agmp->surface = new Surface ( Surface::CreateSurface( buf, x, y, depth, agmp->scanlinelength ) );
      agmp->surface->assignDefaultPalette();
   }

}

Surface& tvirtualdisplay :: getSurface()
{
   return *surface;
}

tvirtualdisplay :: tvirtualdisplay ( int x, int y )
{
   init ( x, y, 0, 8  );
}

tvirtualdisplay :: tvirtualdisplay ( int x, int y, int color, int depth )
{
   init ( x, y, color, depth );
}


tvirtualdisplay :: ~tvirtualdisplay ( )
{
   delete agmp->surface;
   free (  buf ) ;
   *agmp = oldparams;
}


collategraphicoperations :: collategraphicoperations ( void )
{
   #ifndef _DOS_
   status = 1;
   x1 = -1; y1 = -1;
   x2 = -1; y2 = -1;
   olddirectscreenaccess = agmp->directscreenaccess;
   agmp->directscreenaccess = 1;
   #endif
}

collategraphicoperations :: collategraphicoperations ( int _x1, int _y1, int _x2, int _y2 )
{
   #ifndef _DOS_
   status = 1;
   x1 = _x1; y1 = _y1;
   x2 = _x2; y2 = _y2;
   olddirectscreenaccess = agmp->directscreenaccess;
   agmp->directscreenaccess = 1;
   #endif
}


void collategraphicoperations :: on ( void )
{
   #ifndef _DOS_
   agmp->directscreenaccess = 1;
   status = 1;
   #endif
}

void collategraphicoperations :: off ( void )
{
   #ifndef _DOS_
   status = 0;
   agmp->directscreenaccess = 0;
   if (agmp->directscreenaccess == 0)
      copy2screen( x1, y1, x2, y2 );

   #endif
}

collategraphicoperations :: ~collategraphicoperations (  )
{
   #ifndef _DOS_
   if ( status ) {
      agmp->directscreenaccess = olddirectscreenaccess;
      if (agmp->directscreenaccess == 0)
         copy2screen( x1, y1, x2, y2 );
   }
   #endif
}

void copySurface2screen( void )
{
   if (agmp->directscreenaccess == 0)
      copy2screen( -1, -1, -1, -1 );
}

void copySurface2screen( int x1, int y1, int x2, int y2 )
{
   if (agmp->directscreenaccess == 0)
      copy2screen( x1, y1, x2, y2 );
}




tgraphmodeparameters activegraphmodeparameters;
tgraphmodeparameters hardwaregraphmodeparameters;
 
int dpmscapabilities;
int actdpmsmode;
 
dacpalette256  activepalette;
// int       palette16[256][4];
void*     xlatbuffer;
 // dacpalette256  *activepalette256;






void bar(int x1, int y1, int x2, int y2, Uint8 color)
{
   collategraphicoperations cgo ( x1, y1, x2, y2 );
   if ( agmp->windowstatus == 100 ) {
      int spacelength = agmp->scanlinelength - (x2-x1) - 1;
      Uint8* buf = (Uint8*) (agmp->scanlinelength * y1 + x1 * agmp->byteperpix + agmp->linearaddress);
      for ( int y = y1; y <= y2; y++ ) {
         for ( int x = x1; x <= x2; x++ ) 
            *(buf++) = color;

         buf+=spacelength;
      }
   }

}


void getimage(int x1, int y1, int x2, int y2, void *buffer)
{
   if ( agmp->windowstatus == 100 ) {
      int spacelength = agmp->scanlinelength - (x2-x1) - 1;
      Uint8* buf = (Uint8*) (agmp->scanlinelength * y1 + x1 * agmp->byteperpix + agmp->linearaddress);

      Uint8* cb = (Uint8*) buffer;
      Uint16* wb = (Uint16*) buffer;
   
      wb[0] = x2 - x1;
      wb[1] = y2 - y1;
      cb += 4;

      for ( int y = y1; y <= y2; y++ ) {
         for ( int x = x1; x <= x2; x++ )
            *(cb++) = *(buf++);

         buf+=spacelength;
      }
   }


}

void putimage ( int x1, int y1, void* img )
{
   Uint8* src = (Uint8*) img;

   if ( agmp->windowstatus == 100 ) {
      Uint8* buf = (Uint8*) (agmp->scanlinelength * y1 + x1 * agmp->byteperpix + agmp->linearaddress);

      trleheader*   hd = (trleheader*) img;
   
      if ( hd->id == 16973 ) { 
   
         collategraphicoperations cgo ( x1, y1, x1+hd->x, y1+hd->y );

         int spacelength = agmp->scanlinelength - hd->x - 1;

         src  += sizeof ( *hd );

         int x = 0;
         for ( int c = 0; c < hd->size; c++ ) {
            if ( *src == hd->rle ) {
               x += src[1];
               for ( int i = src[1]; i > 0; i-- )
                  *(buf++) = src[2];
   
               src += 3;
               c+=2;

            } else {
               *(buf++) = *(src++);
               x++;
            }

            if ( x > hd->x ) {
               buf += spacelength;
               x = 0;
            }
         }
      } else {
         Uint16* w = (Uint16*) img;
         collategraphicoperations cgo ( x1, y1, x1+w[0], y1+w[1] );
         int spacelength = agmp->scanlinelength - *w - 1;
         src += 4;
         for ( int y = w[1] + 1; y > 0; y-- ) {
            for ( int x = w[0]+1; x > 0; x-- ) 
               *(buf++) = *(src++);
            
            buf+=spacelength;
         }
      }
   }

}


void putimage ( int x1, int y1, TrueColorImage* tci )
{
  for ( int y = 0; y < tci->getysize(); y++ )
     for ( int x = 0; x < tci->getxsize(); x++ ) {
        trgbpixel t = tci->getpix ( x,y );
        putpixel ( x1 + x, y1 + y, t.channel.r + 256*t.channel.g + 256*256*t.channel.b );
     }
}

void putimage_noalpha ( int x1, int y1, TrueColorImage* tci )
{
  for ( int y = 0; y < tci->getysize(); y++ )
     for ( int x = 0; x < tci->getxsize(); x++ ) {
        trgbpixel t = tci->getpix ( x,y );
        if (t.channel.a > alphabase * 2 / 3 )
           putpixel ( x1 + x, y1 + y, TCalpha );
        else
           putpixel ( x1 + x, y1 + y, t.channel.r + 256*t.channel.g + 256*256*t.channel.b );
     }
}

bool trgbpixel::isTransparent()
{
   return ( channel.r == 0xfe && channel.g == 0xfe && channel.b == 0xfe ) ;
}

TrueColorImage* getimage ( int x1, int y1, int x2, int y2 )
{
  TrueColorImage* tci = new TrueColorImage ( x2-x1+1, y2-y1+1 );
  for ( int y = 0; y < tci->getysize(); y++ )
     for ( int x = 0; x < tci->getxsize(); x++ ) {
        trgbpixel t;
        t.rgb = getpixel ( x1 + x, y1 + y );
        if ( t.isTransparent() )
           t.channel.a = alphabase;
        tci->setpix ( x, y, t );
     }
  return tci;
}


void putxlatfilter ( int x1, int y1, void* pic, Uint8* xlattables )
{
   Uint8* src = (Uint8*) pic;

   if ( agmp->windowstatus == 100 ) {
      Uint8* buf = (Uint8*) (agmp->scanlinelength * y1 + x1 * agmp->byteperpix + agmp->linearaddress);
      trleheader*   hd = (trleheader*) pic;

      if ( hd->id == 16973 ) {
         collategraphicoperations cgo ( x1, y1, x1+hd->x, y1+hd->y );

         int spacelength = agmp->scanlinelength - hd->x - 1;
         src  += sizeof ( *hd );

         int x = 0;
         for ( int c = 0; c < hd->size; c++ ) {
            if ( *src == hd->rle ) {
               x += src[1];
               for ( int i = src[1]; i > 0; i-- ) {
                  if ( src[2] != 255 )
                     *buf = xlattables[ src[2] * 256 + *buf ];
                  buf++;
               }

               src += 3;
               c+=2;

            } else {
               if ( *src != 255 )
                  *buf = xlattables[ *(src++) * 256 + *buf ];
               else
                  src++;
               buf++;
               x++;
            }

            if ( x > hd->x ) {
               buf += spacelength;
               x = 0;
            }
         }
      } else {
         Uint16* w = (Uint16*) pic;
         collategraphicoperations cgo ( x1, y1, x1+w[0], y1+w[1] );
         int spacelength = agmp->scanlinelength - *w - 1;

         src += 4;
         for ( int y = w[1] + 1; y > 0; y-- ) {
            for ( int x = w[0]+1; x > 0; x-- ) {
               if ( *src != 255 )
                  *buf = xlattables[ *(src++) * 256 + *buf ];
               else
                  src++;
               buf++;
            }

            buf+=spacelength;
         }
      }
   }

}


void putspriteimage ( int x1, int y1, void* pic )
{
   Uint8* src = (Uint8*) pic;

   if ( agmp->windowstatus == 100 ) {
      trleheader*   hd = (trleheader*) pic;
      Uint8* buf = (Uint8*) (agmp->scanlinelength * y1 + x1 * agmp->byteperpix + agmp->linearaddress);
      if ( hd->id == 16973 ) { 
         collategraphicoperations cgo ( x1, y1, x1+hd->x, y1+hd->y );
         int spacelength = agmp->scanlinelength - hd->x - 1;
         src  += sizeof ( *hd );

         int x = 0;
         for ( int c = 0; c < hd->size; c++ ) {
            if ( *src == hd->rle ) {
               x += src[1];
               if ( src[2] != 255 ) {
                  for ( int i = src[1]; i > 0; i-- )
                     *(buf++) = src[2];
               } else
                  buf += src[1];
               src += 3;
               c+=2;
   
            } else {
               if ( *src != 255 )
                  *buf = *src;
               buf++;
               src++;
               x++;
            }

            if ( x > hd->x ) {
               buf += spacelength;
               x = 0;
            }
         }
      } else {
         Uint16* w = (Uint16*) pic;
         collategraphicoperations cgo ( x1, y1, x1+w[0], y1+w[1] );
         int spacelength = agmp->scanlinelength - *w - 1;
         src += 4;
         for ( int y = w[1] + 1; y > 0; y-- ) {
            for ( int x = w[0]+1; x > 0; x-- ) {
            	Uint8 d = *(src++);
               if ( d != 255 )
                  *buf = d;
               buf++;
            }
            
            buf+=spacelength;
         }
      }
   }

}

void putrotspriteimage(int x1, int y1, void *pic, int rotationvalue)
{
   Uint8* src = (Uint8*) pic;

   if ( agmp->windowstatus == 100 ) {
      Uint8* buf = (Uint8*) (agmp->scanlinelength * y1 + x1 * agmp->byteperpix + agmp->linearaddress);
      trleheader*   hd = (trleheader*) pic;
   
      if ( hd->id == 16973 ) { 
         collategraphicoperations cgo ( x1, y1, x1+hd->x, y1+hd->y );

         int spacelength = agmp->scanlinelength - hd->x - 1;
         src  += sizeof ( *hd );

         int x = 0;
         for ( int c = 0; c < hd->size; c++ ) {
            if ( *src == hd->rle ) {
               x += src[1];
               Uint8 d = src[2];
               if ( d != 255 ) {
                  if ( d >= 16 && d < 24 )
                     d += rotationvalue;
                  for ( int i = src[1]; i > 0; i-- )
                     *(buf++) = d;
               } else
                  buf += src[1];
   
               src += 3;
               c+=2;
   
            } else {
            	Uint8 d = *(src++);
               if ( d != 255 ) {
                  if ( d >= 16 && d < 24 )
                     d += rotationvalue;
                  *(buf++) = d;
               } else
                 buf++;

               x++;
            }

            if ( x > hd->x ) {
               buf += spacelength;
               x = 0;
            }
         }
      } else {
         Uint16* w = (Uint16*) pic;
         collategraphicoperations cgo ( x1, y1, x1+w[0], y1+w[1] );
         int spacelength = agmp->scanlinelength - *w - 1;
         src += 4;
         for ( int y = w[1] + 1; y > 0; y-- ) {
            for ( int x = w[0]+1; x > 0; x-- ) {
            	Uint8 d = *(src++);
               if ( d != 255 ) {
                  if ( d >= 16 && d < 24 )
                     d += rotationvalue;
   
                  *buf = d;
               }
               buf++;
            }
            
            buf+=spacelength;
         }
      }
   }

}

void putrotspriteimage90(int x1, int y1, void *pic, int rotationvalue)
{
   Uint16* w = (Uint16*) pic;
   // Uint8* c = (Uint8*) pic + 4;
   int spacelength = agmp->scanlinelength - w[1] - 1;
   collategraphicoperations cgo ( x1, y1, x1+w[1], y1+w[0] );

   if ( agmp->windowstatus == 100 ) {
      Uint8* buf = (Uint8*) (agmp->scanlinelength * y1 + x1 * agmp->byteperpix + agmp->linearaddress);
      for ( int y = 0; y <= w[0] ; y++ ) {
         for ( int x = 0; x <= w[1]; x++ ) {
            int d = getpixelfromimage ( pic, y, w[1] - x );
            if ( d != 255 && d != -1) {
               if ( d >= 16 && d < 24 )
                  d += rotationvalue;

               *buf = d;
            }
            buf++;
         }
         
         buf+=spacelength;
      }
   }

}

void putrotspriteimage180(int x1, int y1, void *pic, int rotationvalue)
{
   Uint16* w = (Uint16*) pic;
   collategraphicoperations cgo ( x1, y1, x1+w[0], y1+w[1] );
   // Uint8* c = (Uint8*) pic + 4;
   int spacelength = agmp->scanlinelength - *w - 1;

   if ( agmp->windowstatus == 100 ) {
      Uint8* buf = (Uint8*) (agmp->scanlinelength * y1 + x1 * agmp->byteperpix + agmp->linearaddress);
      for ( int y = 0; y <= w[1] ; y++ ) {
         for ( int x = 0; x <= w[0]; x++ ) {
            int d = getpixelfromimage ( pic, w[0] - x, w[1] - y );
            if ( d != 255 && d != -1) {
               if ( d >= 16 && d < 24 )
                  d += rotationvalue;

               *buf = d;
            }
            buf++;
         }
         
         buf+=spacelength;
      }
   }

}

void putrotspriteimage270(int x1, int y1, void *pic, int rotationvalue)
{
   Uint16* w = (Uint16*) pic;
   collategraphicoperations cgo ( x1, y1, x1+w[0], y1+w[1] );
   // Uint8* c = (Uint8*) pic + 4;
   int spacelength = agmp->scanlinelength - *w - 1;

   if ( agmp->windowstatus == 100 ) {
      Uint8* buf = (Uint8*) (agmp->scanlinelength * y1 + x1 * agmp->byteperpix + agmp->linearaddress);
      for ( int y = 0; y <= w[1] ; y++ ) {
         for ( int x = 0; x <= w[0]; x++ ) {
            int d = getpixelfromimage ( pic, w[1] - y, x );
            if ( d != 255 && d != -1) {
               if ( d >= 16 && d < 24 )
                  d += rotationvalue;

               *buf = d;
            }
            buf++;
         }
         
         buf+=spacelength;
      }
   }

}

void puttexture ( int x1, int y1, int x2, int y2, void *texture )
{
   collategraphicoperations cgo ( x1, y1, x2, y2 );
   Uint8* c = (Uint8*) texture;
   int spacelength = agmp->scanlinelength - (x2 - x1) - 1;

   if ( agmp->windowstatus == 100 ) {
      int offset = agmp->scanlinelength * y1 + x1;
      Uint8* buf = (Uint8*) agmp->linearaddress;
      for ( int y = y1 ; y <= y2; y++ ) {
         for ( int x = x1; x <= x2 ; x++ ) {
            buf[offset] = c[offset];
            offset++;
         }
         
         offset+=spacelength;
      }
   }

}


void putspritetexture ( int x1, int y1, int x2, int y2, void *texture )
{
   collategraphicoperations cgo ( x1, y1, x2, y2 );
   Uint8* c = (Uint8*) texture;
   int spacelength = agmp->scanlinelength - (x2 - x1) - 1;

   if ( agmp->windowstatus == 100 ) {
      int offset = agmp->scanlinelength * y1 + x1;
      Uint8* buf = (Uint8*) agmp->linearaddress;
      for ( int y = y1 ; y <= y2; y++ ) {
         for ( int x = x1; x <= x2 ; x++ ) {
        	 Uint8 d = c[offset];
            if ( d != 255 )
               buf[offset] = d;
            offset++;
         }
         
         offset+=spacelength;
      }
   }

}

void putimageprt ( int x1, int y1, int x2, int y2, void *texture, int dx, int dy )
{
   collategraphicoperations cgo ( x1, y1, x2, y2 );
   int spacelength = agmp->scanlinelength - (x2-x1) - 1;

   if ( agmp->windowstatus == 100 ) {
      Uint8* buf = (Uint8*) (agmp->scanlinelength * y1 + x1 * agmp->byteperpix + agmp->linearaddress);
      for ( int y = y1; y <= y2 ; y++ ) {
         for ( int x = x1; x <= x2; x++ ) {
            int p = getpixelfromimage ( texture, x - x1 + dx, y - y1 + dy );
            if ( p != -1 ) 
               *buf = p;

            buf++;
         }
         buf+=spacelength;
      }
      /*
      for ( int y = 0; y <= w[1] ; y++ ) {
         for ( int x = 0; x <= w[0]; x++ ) {
            int p = getpixelfromimage ( texture, x + dx, y + dy );
            if ( p != -1 )
               *buf = p;

            buf++;
         }
         buf+=spacelength;
      } */
   }

}

void copybuf2displaymemory(int size, void *buf)
{
   collategraphicoperations cgo ;
   memcpy ( (void*) agmp->linearaddress, buf, size );
}

#if 0
void* xlatpict ( ppixelxlattable xl, void* vbuf )
{

   Uint8* buf = (Uint8*) vbuf;

   Uint16* wp = (Uint16*) xlatbuffer;
   Uint8* dest = (Uint8*) xlatbuffer;

   trleheader*   hd = (trleheader*) vbuf; 

   if ( hd->id == 16973 ) {
      memcpy ( xlatbuffer, vbuf, sizeof ( *hd ));

      trleheader* desthead = (trleheader*) xlatbuffer; 


      dest += sizeof ( *hd );
      buf  += sizeof ( *hd );

      for ( int c = 0; c < hd->size; c++ ) 
         if ( *buf == hd->rle ) {
            *(dest++) = *(buf++);
            *(dest++) = *(buf++);
            *(dest++) = (*xl)[ *(buf++) ];

            c += 2;

         } else {
            int newpix = (*xl)[ *(buf++) ];
            if ( newpix == hd->rle ) {
               *(dest++) = hd->rle;
               *(dest++) = 1;
               *(dest++) = hd->rle;
               desthead->size += 2;
            } else
               *(dest++) = newpix;
         }
      
   } else {
      Uint16* wp2 = (Uint16*) vbuf;

      wp[0] = wp2[0];
      wp[1] = wp2[1];
/*
      if ( ( wp[0] + 1 ) * ( wp[1] + 1 ) + 4 >= xlatbuffersize )
         throw fatalgraphicserror ( "halfpict : picture larger than buffer ! " );
*/
      dest += 4;
      buf += 4;

      for ( int c = (wp2[0] + 1) * (wp2[1] + 1); c > 0; c-- ) {
         *dest = (*xl)[ *buf ];
         dest++;
         buf++;
      }

   }
   return xlatbuffer;
}
#endif

int loga2 ( int a )
{
   int l = 0;
   if ( a ) 
      while ( ! (a & 1)) {
         a >>= 1;
         l++;
      }
   return l;
}



void showtext ( const char* text, int x, int y, int textcol )
{
   if ( !activefontsettings.font )
     return;

   if ( !text )
     return;
 
   Uint8* fb = (Uint8*)(x * agmp->byteperpix + y * agmp->scanlinelength + agmp->linearaddress);
   int fontheight;
   int extraheight = 0;
   if ( activefontsettings.height == 0 )
      fontheight = activefontsettings.font->height;
   else
      if ( activefontsettings.height > activefontsettings.font->height ) {
         fontheight = activefontsettings.font->height;
         extraheight = activefontsettings.height - activefontsettings.font->height;
      } else 
         fontheight = activefontsettings.height;

    const Uint8* t = (Uint8*) text;
    int length = 0;

    Uint8* characterpointer[1024];
    int    characterwidth[1024];
    int    characterdist[1024];
    int ps = 0;
    while ( *t ) {
       if ( activefontsettings.font->character[int(*t)].width ) {
          characterwidth[ps] = activefontsettings.font->character[int(*t)].width;
          characterpointer[ps] = activefontsettings.font->character[int(*t)].memposition + 2;
          if ( t[1] )
             characterdist[ps] = activefontsettings.font->kerning[int(t[1])][int(t[0])] + 2;
          else
             characterdist[ps] = 0;
          length +=activefontsettings.font->character[int(*t)].width;
          length += characterdist[ps];
          ps++;
       }
       t++;
    }
    int leftextralength = 0;
    int rightextralength = 0;

    if ( !activefontsettings.length )
       leftextralength = rightextralength = 0;
    else {
       if ( activefontsettings.justify == 0 ) {
          leftextralength = 0;
          if ( activefontsettings.length > length )
             rightextralength = activefontsettings.length - length;
          else {
             rightextralength = 0;
             length = activefontsettings.length;
          }
       } else
       if ( activefontsettings.justify == 1 ) {
          if ( activefontsettings.length > length ) {
             leftextralength = (activefontsettings.length - length+2)/2;
             rightextralength = activefontsettings.length - length - leftextralength;
          } else {
             rightextralength = 0;
             leftextralength = 0;
             length = activefontsettings.length;
          }
       } else
       if ( activefontsettings.justify == 2 ) {
          rightextralength = 0;
          if ( activefontsettings.length > length )
             leftextralength = activefontsettings.length - length;
          else {
             leftextralength = 0;
             length = activefontsettings.length;
          }
       } 
       
    }
    collategraphicoperations cgo ( x, y, x +length + leftextralength + rightextralength, y + fontheight + extraheight );

    int suppressbkgr = 0;
    int spacelength = agmp->scanlinelength - (length + leftextralength + rightextralength);
    if ( activefontsettings.background == 255 ) {
       spacelength += leftextralength + rightextralength;
       fb += leftextralength;
       leftextralength = rightextralength = 0;
       extraheight = 0;
    }
    for ( int yl = 0; yl < fontheight; yl++ ) {
       if ( leftextralength ) 
          for ( int i = 0; i < leftextralength; i++ )
             *(fb++) = activefontsettings.background;

       int x = 0;
       ps = 0;
       while ( x < length ) {
          int cx;
          for ( cx = characterwidth[ps]; cx > 0 && x < length; cx--) {
             int pix = *(characterpointer[ps]++);
             if ( pix ) {
                if ( textcol != -1 )
                   *fb = textcol;
                else
                   *fb = pix;
             } else
                if ( activefontsettings.background != 255 ) {
                   if ( suppressbkgr )
                      suppressbkgr--;
                   else
                     *fb = activefontsettings.background;
                }
             fb++;
             x++;
          } /* endfor */
          if ( x < length ) {
             if ( characterdist[ps] > 0 ) {
                for (int i = characterdist[ps]; i && x < length; i-- ) {
                   if ( activefontsettings.background != 255 )
                     *fb = activefontsettings.background;
                   fb++;
                   x++;
                } /* endfor */
             } else {
                fb += characterdist[ps];
                x += characterdist[ps];
                suppressbkgr = -characterdist[ps];
             }
          } else {
             characterpointer[ps] += cx;
          }
          ps++;
       }

       if ( rightextralength )
          for ( int i = 0; i < rightextralength; i++ )
             *(fb++) = activefontsettings.background;

       fb += spacelength;
    }
    if ( extraheight )
       for ( int yl = extraheight; yl > 0; yl-- ) {
          for ( int x = length + leftextralength + rightextralength; x > 0; x-- )
             *(fb++) = activefontsettings.background;
          fb += spacelength;
       }

}

void showtext2 ( const ASCString& text, int x, int y )
{
   showtext ( text.c_str(), x, y, activefontsettings.color );
}

void showtext2c ( const ASCString& text, int x, int y )
{           
   showtext ( text.c_str(), x, y, -1 );
}

#ifdef use_truecolor2pal
void* convertSurface ( SDLmm::Surface& s, bool paletteTranslation )
{
  s.Lock();

  Uint8* buf = new Uint8[ imagesize ( 1, 1, s.w(), s.h())];
  Uint16* wp = (Uint16*) buf;
  wp[0] = s.w()-1;
  wp[1] = s.h()-1;
  SDLmm::PixelFormat fmt = s.GetPixelFormat();
  if ( fmt.BytesPerPixel() == 1 && !paletteTranslation ) {
     for ( int y = 0; y < s.h(); y++ )
        for ( int x = 0; x < s.w(); x++ )
           buf[4 + s.w()*y+x] = s.GetPixel ( x, y );

  } else {

     for ( int y = 0; y < s.h(); y++ )
        for ( int x = 0; x < s.w(); x++ ) {
           Uint8 red, green, blue, alpha;
           fmt.GetRGBA ( s.GetPixel ( x, y ), red, green, blue, alpha);

           if ( alpha < 128 )
              buf[4 + s.w()*y+x] = 255;
           else
              buf[4 + s.w()*y+x] = truecolor2pal_table[ (red >> 2) +
                                                        (( green >> 2) << 6) +
                                                        (( blue >> 2) << 12) ];
        }
  }

  s.Unlock();
  return buf;
}
#endif

SPoint getPixelRotationLocation( SPoint pos, int width, int height, int degrees )
{
   const float pi = 3.14159265;
   double angle = double(-degrees) / 360 * 2 * pi;

   double dx = pos.x - width/2 ;
   double dy = height/2 - pos.y;
   float nx, ny;
   if ( degrees == 0 ) {
      nx = dx;
      ny = dy;
   } else
      if ( degrees == 180 || degrees == -180) {
         nx = -dx;
         ny = -dy;
      } else {
         float wnk ;
         if ( dy  )
            wnk = atan2 ( -dx, dy );
         else
            if ( dx < 0 )
               wnk = pi/2;
            else
               wnk = -pi/2;

         wnk += angle;
         float radius = sqrt ( dx * dx + dy * dy );

         nx = -radius * sin ( wnk );
         ny = radius * cos ( wnk );
      }

   return SPoint( int( width/2 + nx), int ( -ny + height/2));
}


