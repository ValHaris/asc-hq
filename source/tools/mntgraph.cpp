#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>

#include "..\vesa.h"
#include "..\basestrm.h"
#include "..\typen.h"
#include "..\misc.h"
#include "..\loadpcx.h"
#include "..\palette.h"
#include "..\sgstream.h"


int interpol;

const int pcount = 2000;

void* pics[pcount];
int picmode[pcount];

void* mask[2];

int doublesize;
int blank;
int lockpalette;



void* doublepict ( void* vbuf, int interpolate )
{
   if ( !interpolate ) {
      void* xlatbuffer = asc_malloc ( 10000 );
   
      int width, height;
      getpicsize ( vbuf, width, height );
   
   
      char* buf = (char*) vbuf;
   
      word* wp = (word*) xlatbuffer;
      char* dest = (char*) xlatbuffer;
   
      wp[0] = width * 2 - 1;
      wp[1] = height * 2 - 1;
   
     dest += 4;
   
      for ( int y = 0; y < height*2; y++) 
         for ( int x = 0; x < width*2; x++ ) {
            *dest = getpixelfromimage ( vbuf, x/2, y/2 );
            dest++;
         } /* endfor */
   
      return xlatbuffer;
   } else {
      TrueColorImage* zimg = zoomimage ( vbuf, fieldxsize, fieldysize, pal, 1 );
      void* pic = convertimage ( zimg, pal ) ;
      delete zimg;
      return pic;
   }
} 



void getpic ( int pos, int offset )
{
   doublesize = getpixel ( 0, 0) != getpixel( 1,0 );
   int colnum = 10;
   int x0 = 30;
   int xd = ( 400 - 2 * x0 ) / colnum ;
   
   
   int y0 = 10;
   int yd = fieldsizey/2 + 5;

   int np = pos - offset;

   int x = np % colnum;
   int y = np / colnum;

   int xp = x0 + x * xd;
   int yp = y0 + y * yd;


   int fsx;
   int fsy;
   if ( doublesize ) {
      xp *= 2;
      yp *= 2;
      fsx = fieldsizex;
      fsy = fieldsizex;
   } else {
      fsx = fieldsizex/2;
      fsy = fieldsizey/2;
   }


   void* buf = asc_malloc ( 10000 );

   putmask ( xp, yp, mask[doublesize], 0 );
   getimage ( xp, yp, xp + fsx-1, yp + fsy-1, buf );

   if ( !lockpalette ) {
      TrueColorImage* img = zoomimage ( buf, fsx, fsy, *activepalette256, 0 );
      asc_free ( buf );
      buf = convertimage ( img, pal );
   }

   if ( doublesize ) 
      pics[pos] = buf;
   else {
   
      void* buf2 = doublepict ( buf, interpol );
      asc_free ( buf );
   
   
      pics[pos] = buf2;
   }
   picmode[pos] = 2;

   if ( blank )
      picmode[pos] += 256;
}

int main(int argc, char *argv[] )
{
   {
      int i ;
      tnfilestream s ( "smallhex.raw", 1);
      s.readrlepict ( &mask[0], false, & i );
   }
   {
      int i ;
      tnfilestream s ( "largehex.raw", 1);
      s.readrlepict ( &mask[1], false, & i );
   }

   for ( int m = 0; m < 2000; m++ )
      picmode[m] = 0;


   tvirtualdisplay vdp ( 1000, 10000, 255 );

   loadpalette();

   FILE* fp = fopen ( argv[1], "rt" );
   char buf[10000];
   char* res = fgets ( buf, 10000, fp );


   while ( res ) {
      printf("\n");
      fflush ( stdout );

      buf[ strlen(buf)-1 ] = 0;
      if ( buf[0] && buf[0]!= ';' ) {
         char* filename = strtok ( buf, ";");
         interpol = atoi ( strtok ( NULL, ";" ));
         int picoffset = atoi ( strtok ( NULL, ";" ));
         blank = atoi ( strtok ( NULL, ";" ));
         lockpalette = atoi ( strtok ( NULL, ";" ));
         char* piclist = strtok ( NULL, ";" );

         loadpcxxy ( filename, 1, 0, 0 );
         char* pic = strtok ( piclist, "," );
         while ( pic ) {
            int from, to;
            if ( strchr ( pic, '-' )) {
               char* a = strchr ( pic, '-' );
               *a = 0;
               from = atoi ( pic );
               to = atoi ( ++a );
            } else 
               from = to = atoi ( pic );

            for ( int i = from; i <= to; i++ )
               getpic ( i, picoffset );

            pic = strtok ( NULL, "," );
            printf(".");
           fflush ( stdout );

         }


      }

      res = fgets ( buf, 10000, fp );
   }

   {
      tnfilestream s ( "newgraph.dta", 2 );
      s.writedata2 ( pcount );
      s.writedata2 ( picmode );
      for ( int i = 0; i < pcount; i++ ) 
         if ( picmode[i] >= 1 )
            s.writedata ( pics[i], getpicsize2 ( pics[i] ) );
   }
   return 0;
}
