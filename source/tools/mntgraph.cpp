#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <map>

#include "../basegfx.h"
#include "../basestrm.h"
#include "../typen.h"
#include "../misc.h"
#include "../loadpcx.h"
#include "../palette.h"
#include "../sgstream.h"
#include "../basegfx.h"


int interpol;

const int p_count = 4000;
int maxnum = 0;

void* pics[p_count];
int picmode[p_count];

void* mask[2];

int doublesize;
int blank;
int lockpalette;



// the layout of the pcx file
   int colnum = 10;
   int x0 = 30;
   int xd = ( 400 - 2 * x0 ) / colnum ;


   int y0 = 10;
   int yd = fieldsizey/2 + 5;



typedef map<int, TrueColorImage*> TCI;
TCI tci;

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
   if ( pos > maxnum )
      maxnum = pos;

   doublesize = getpixel ( 0, 0) != getpixel( 1,0 );

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

   // putmask ( xp, yp, mask[doublesize], 0 );
   getimage ( xp, yp, xp + fsx-1, yp + fsy-1, buf );

   if ( doublesize ) {
      tci[pos] = convertimage2tc ( buf, *activepalette256 );
   } else
      tci[pos] = zoomimage ( buf, fieldxsize, fieldysize, pal, 1 );

   if ( !lockpalette ) {
      TrueColorImage* img = zoomimage ( buf, fsx, fsy, *activepalette256, 0 );
      asc_free ( buf );
      buf = convertimage ( img, pal );
   } else
     if ( lockpalette == 2 ) {
        bi2asc_color_translation_table[255] = 255;
        bi2asc_color_translation_table[0] = 0;
        void* nb = xlatpict ( &bi2asc_color_translation_table, buf );
        asc_free ( buf );
        buf = asc_malloc ( getpicsize2 ( nb ));
        memcpy ( buf, nb, getpicsize2 ( nb ));
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
   initFileIO( NULL );
   addSearchPath ( "." );
   opencontainer ( "*.con");
   {
      int i ;
      tnfilestream s ( "smallhex.raw", tnstream::reading );
      s.readrlepict ( &mask[0], false, & i );
   }
   {
      int i ;
      tnfilestream s ( "largehex.raw", tnstream::reading );
      s.readrlepict ( &mask[1], false, & i );
   }

   for ( int m = 0; m < p_count; m++ )
      picmode[m] = 0;


   {
      tvirtualdisplay vdp ( 1000, 10000, 255 );

      loadpalette();

      FILE* fp = fopen ( argv[1], filereadmodetext );
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
            // writepcx ( "testimg.pcx", 0,0, 999, 10000 , pal);
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
         tnfilestream s ( "newgraph.gfx", tnstream::writing );
         int magic = -1;
         s.writedata2 ( magic );

         int id = 1;
         printf ("\n    ID :  \n ( 0 = original ASC graphics; 1 = BI3 graphics; >=2 : additional graphic sets)\n    ");
         scanf ( "%d", &id );
         // num_ed ( id , 0, 65534);


         maxnum++;

         int maxpicsize = 0;
         for ( int i = 0; i < maxnum; i++ )
            if ( picmode[i] >= 1 )
               if ( maxpicsize < getpicsize2 ( pics[i] ) )
                  maxpicsize = getpicsize2 ( pics[i] );

         s.writedata2 ( id );
         s.writedata2 ( maxnum );
         s.writedata2 ( maxpicsize );
         s.writedata ( picmode, maxnum * sizeof ( int ) );
         for ( int j = 0; j < maxnum; j++ )
            if ( picmode[j] >= 1 )
               s.writedata ( pics[j], getpicsize2 ( pics[j] ) );
      }
   }
   {
      int maxy = 0;
      int maxx = 0;
      tvirtualdisplay vdp ( 1000, 30000, 255, 32 );
      for ( TCI::iterator ti = tci.begin(); ti != tci.end(); ti++ ) {
         int x = ti->first % colnum;
         int y = ti->first / colnum;
         int xp = x0 + x * xd;
         int yp = y0 + y * yd;
         putimage ( x, y, ti->second );

         if ( yp > maxy )
            maxy = yp;

         if ( xp > maxx )
            maxx = xp;
      }
      writepcx ( "images.pcx", 0, 0, maxx+100, maxy+100, activepalette );
   }
   return 0;
}
