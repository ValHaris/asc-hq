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

}

void 
xorline(int  x1,
     int  y1,
     int  x2,
     int  y2,
     Uint8 actcol)
{


}



void 
rectangle(int x1,
	  int y1,
	  int x2,
	  int y2,
	  Uint8 color)
{

}

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

}


void putinterlacedrotimage ( int x1, int y1, void* ptr, int rotation )
{


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

}

int getpixel8 ( int x1, int y1 )
{
    Uint8* buf = (Uint8*) (agmp->scanlinelength * y1 + x1 * agmp->byteperpix + agmp->linearaddress);
    return *buf;
}


void putpixel(int x1, int y1, int color)
{

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



void copySurface2screen( void )
{

}

void copySurface2screen( int x1, int y1, int x2, int y2 )
{

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
   

      } else {

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

      } else {

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

      } else {

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

      }
   }

}

void putrotspriteimage90(int x1, int y1, void *pic, int rotationvalue)
{


}

void putrotspriteimage180(int x1, int y1, void *pic, int rotationvalue)
{

}

void putrotspriteimage270(int x1, int y1, void *pic, int rotationvalue)
{
}




void putimageprt ( int x1, int y1, int x2, int y2, void *texture, int dx, int dy )
{

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


