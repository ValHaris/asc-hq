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

#include <conio.h>
#include <stdio.h>


#include "..\newfont.h"
#include "..\vesa.h"
#include "krkr.h"
#include "..\loadpcx.h"




tgraphmodeparameters vgmp;
toldfont                font;
toldfont                fontcompr; 
toldcharacter           charac;
int                  kerntable[256][256];
int                  kernsum[256];
int                  kernchartable[256];

struct tpcxheader{
           byte     manufacturer;
           byte     version     ;
           byte     encoding    ;
           byte     bitsperpixel;
           word     xmin,ymin   ;
           word     xmax,ymax   ;
           word     hdpi,vdpi   ;
           byte     colormap[48];
           byte     reserved    ;
           byte     nplanes     ;
           word     bytesperline;
           word     paletteinfo ;
           word     hscreensize ;
           word     vscreensize ;
           byte     dummy[54]   ;
       } header;


void showtext ( char* txt, int x, int y, toldfont* fnt )
{
   while ( *txt ) {
      if ( fnt->character[*txt].size ) {
         int xx ;
         char* c = fnt->character[*txt].memposition + 2;
         for ( int yy = 0; yy < fnt->height ; yy++ ) {
            for ( xx = 0; xx < fnt->character[*txt].width ; xx++ ) {
               if ( fnt->character[*txt].memposition[ 2 + xx + yy * fnt->character[*txt].width ] )
                  putpixel ( x + xx, y + yy, x ) ;
               putpixel ( x + xx, y + yy + 100, *(c++) ) ;
            }
         } /* endfor */
         x+=xx - kerntable[txt[0]][txt[1]] + 3;
      }
      txt++;
   }
}


int abs ( int a )
{
   if ( a > 0 )
      return a;
   else 
      return -a;
}

int min ( int a, int b, int c, int d, int e )
{
   if ( a <= b && a <= c && a <= d && a <= e)
      return a;
   else
     if ( b <= a && b <= c && b <= d && b <= e)
         return b;
      else
         if ( c <= a && c <= b && c <= d && c <= e)
            return c;
         else  
            if ( d <= a && d <= b && d <= c && d <= e)
                 return d;
              else  
                 if ( e <= a && e <= b && e <= c && e <= d)
                      return e;
                 else {
                     printf( " fehler bei min: %i, %i, %i, %i, %i \a", a, b, c, d, e );
                     return 1000;
                 }
}

int max ( int a, int b, int c, int d, int e )
{
   if ( a >= b && a >= c && a >= d && a >= e)
      return a;
   else
     if ( b >= a && b >= c && b >= d && b >= e)
         return b;
      else
         if ( c >= a && c >= b && c >= d && c >= e)
            return c;
         else  
            if ( d >= a && d >= b && d >= c && d >= e)
                 return d;
              else  
                 if ( e >= a && e >= b && e >= c && e >= d)
                      return e;
                 else {
                     printf( " fehler bei max: %i, %i, %i, %i, %i \a", a, b, c, d, e );
                     return 1000;
                 }
}

void main( void )
{
      memset ( &font, 0, sizeof( font ));
      memset ( kerntable, 0, sizeof( kerntable ));
      memset ( kernsum, 0, sizeof( kernsum ));

      settxt50mode();
      int maxxsize, maxysize;
      int background;
      char filename[15];

      filename[0] = 0;

      printf("\n\n   filename \n");
      stredit2 ( filename, 8, -1, -1 );

      tfile          datfile;
      fileselect("*.pcx", _A_NORMAL, datfile);

      FILE* fp = fopen(datfile.name, "rb");
      fread(&header,sizeof(header),1,fp);
      fclose(fp);

      maxxsize  = header.xmax-header.xmin+1;
      maxysize  = header.ymax-header.ymin+1;

      printf("\n\n   fontname \n");
      char fontname[256];
      stredit2 ( fontname, 60, -1, -1 );
      strcpy ( &font.name[1], fontname );
      font.name[0] = strlen ( fontname );

      printf ("\n\n caps\n");
      yn_switch ("YES", "NO", 1,0, font.caps);

      printf ("\n\n color\n");
      yn_switch ("YES", "NO", 1,0, font.color);

      vgmp.resolutionx = maxxsize;
      vgmp.resolutiony = maxysize;
      vgmp.windowstatus = 100;
      vgmp.scanlinelength = maxxsize;
      vgmp.scanlinenumber = maxysize;
      vgmp.bytesperscanline = maxxsize;
      vgmp.byteperpix = 1;
      vgmp.linearaddress = (int) malloc ( maxxsize * maxysize );
      if ( vgmp.linearaddress == 0 ) {
         printf("could not allocate memory \n");
         return;
      }
      memset ( (void*) vgmp.linearaddress, 0, maxxsize * maxysize );

      initsvga ( 0x101 );
//      *agmp = vgmp;

      loadpcxxy ( datfile.name, 1,0,0 );
      background = getpixel ( 0,0 );

      int y1,y2;
      int a,b,x,y;

      y=0;
      b=0;
      for ( y1 = 0; b == 0; y1++ ) 
         for ( x = 0; x < maxxsize ; x++ ) {
            if ( getpixel ( x,y1 ) != background )
               b = 1;
         } /* endfor */
      y1--;

      b=0;
      for ( y2 = maxysize-1; b == 0; y2-- ) 
         for ( x = 0; x < maxxsize ; x++ ) {
            if ( getpixel ( x,y2 ) != background )
               b = 1;
         } /* endfor */
      y2++;


      int h = font.height = y2-y1+1;
       

      char* buffer = (char*) malloc ( 65536 );
      charac.memposition  =  buffer;
      char* buf;
      memset( buffer,0, 65536 );

      x = 0;
      int c   = 0    ;
      do {


         b = 0;
         do {
            for (y=y1;y<=y2 ;y++ ) {
               if ( getpixel ( x, y ) != background )
                  b = 1;
            } /* endfor */

            if ( b == 0)
               x++;
         } while ( b == 0 || x == (maxxsize-10) ); /* enddo */

         if ( x < (maxxsize-10) ) {
            buf = buffer;
            charac.width = 0;
            do {
               b = 0;
               for (y=y1;y<=y2 ;y++ ) {
                  a = getpixel ( x, y );
                  /*
                  if ( a == background )
                     *buf = 0;
                  else
                     *buf = a;
                  */
                  *buf = a;

                  buf++;
   
                  if ( a != background )
                     b = 1;
               } /* endfor */
   
               if ( b ) {
                  charac.width++;
                  x++;
               }
   
            } while ( b != 0 ); /* enddo */
            
//            *agmp = *hgmp;
            bar ( 100, 100, 200, 200, background );
            buf = buffer;
   
   
   
            for ( int xx = 0; xx < charac.width ; xx++ ) {
                for ( y = 0; y < h; y++ ) {
                  putpixel ( 100 + xx, 100 + y, *buf );
                  buf++;
               } /* endfor */
            }
   
            if ( c >= 'A' && c < 'Z' || c >= 'a' && c < 'z' || c >= '1' && c < '9')
               c++;
            else
               if ( c == 27 )
                  c = 27;
               else
                  c = getch();
            buf = buffer;
            font.character[ c ] = charac;
            font.character[ c ].size = font.height* charac.width ;
            font.character[ c ].memposition = (char*) malloc ( font.character[ c ].size + 2 );
            for ( xx = 0; xx < charac.width ; xx++ ) {
               for ( y = 0; y < h; y++ ) {
                  if ( font.color ) 
                     font.character [ c ].memposition [ 2 + xx + y * charac.width ] = *buf ;
                  else
                     font.character [ c ].memposition [ 2 + xx + y * charac.width ] = *buf != background ;
                  buf++;
               } /* endfor */
            }
            word* pw = (word*) font.character [ c ].memposition;
            *pw = charac.width;
            
            buf = buffer;
            if ( c == 27 )
               font.character[ c ].size = 0;


   
//            *agmp = vgmp;
         }
      } while ( x < maxxsize  ); /* enddo */

      char *crt = (char*) 0xb8000;

      settextmode ( 3 );
      int* szel1 = ( int* ) malloc (  4 * font.height  );
      int* szel2 = ( int* ) malloc (  4 * font.height  );


      for ( int c1 = 0; c1 < 256 ; c1++ ) 
         if ( font.character [ c1 ].size ) {
            for ( int a = 0; a < font.height ; a++ ) {
               int c = 1;
               for (int b = font.character[ c1 ].width-1 ; b >=0 && c; b-- ) {
                  if ( font.character[ c1 ].memposition [ 2 + a * font.character[ c1 ].width + b ] )
                     c = 0;
               } /* endfor */
               szel1[ a ] = font.character[ c1 ].width-2 - b;
            } /* endfor */



            for ( int c2 = 0; c2 < 256; c2++ ) 
               if ( font.character [ c2 ].size ) {
                  crt[0] = c1;
                  crt[2] = c2;

                  for ( a = 0; a < font.height ; a++ ) {
                     c = 1;
                     for ( b = 0 ; b < font.character[ c2 ].width && c; b++ ) {
                        if ( font.character[ c2 ].memposition [ 2 + a * font.character[ c2 ].width + b ] )
                           c = 0;
                     } /* endfor */
                     szel2[ a ] = b;
                  } /* endfor */


                  int xdif1;
                  int xdif2;
                  int xdif3;
                  int xdif4;
                  int mdif;

                  int dif;
                  
                  int mindif = 1000;
                  for ( a = 0; a < font.height ; a++ ) {
                     if ( a > 0 ) {
                        xdif1 = szel1[ a   ] + szel2[ a-1 ] + 1;
                        xdif2 = szel1[ a-1 ] + szel2[ a   ] + 1;
                     } else   {
                        xdif1 = 1000;
                        xdif2 = 1000;
                     }

                     if ( a+1 < font.height ) {
                        xdif3 = szel1[ a   ] + szel2[ a+1 ] + 1;
                        xdif4 = szel1[ a+1 ] + szel2[ a   ] + 1;
                     } else   {
                        xdif3 = 1000;
                        xdif4 = 1000;
                     }

                     dif = szel1[ a ] + szel2[ a ];
                     mdif = min ( dif, xdif1, xdif2, xdif3, xdif4 );
                     if ( mdif < mindif )
                        mindif = mdif;
                  }
                  mindif++;

                  kerntable [ c1 ] [ c2 ] = mindif;
                  kernsum[ c1 ] += mindif;
                  kernsum[ c2 ] += mindif;

               } /* endif */
      } /* endfor */

      for ( int i = 0; i < 256; i++ ) 
         kernchartable[i] = i;

      for ( i = 0; i < 255 ; ) {
         if ( kernsum[i] < kernsum[i+1] ) {
            int j = kernsum[i];
            kernsum[i] = kernsum[i+1];
            kernsum[i+1] = j;
            j = kernchartable[i];
            kernchartable[i] = kernchartable[i+1];
            kernchartable[i+1] = j;

            if ( i > 2 )
               i-=2;
            else
               i=0;
         } else
            i++;
      } /* endfor */

      for ( i = 0; i < 256; i++ ) 
          font.kernchartable[ i ] = 100;

      for ( i = 0; i < 100; i++ ) 
          font.kernchartable[ kernchartable[i] ] = i;

      for ( i = 0; i < 256; i++ ) 
         for ( int j = 0; j < 256; j++ )
            font.kerning[ font.kernchartable[ i ] ] [ font.kernchartable[ j ] ] = 2 - kerntable [i][j];

      for ( i = 0; i <= 100; i++ ) {
         font.kerning[ i ] [ 100 ] = 0;
         font.kerning[ 100 ] [ i ] = 0;
      }


      fontcompr = font;

      if ( font.color == 0 ) 
         for ( i = 0; i < 256; i++ ) {
            if ( font.character[i].size ) {
               fontcompr.character[i].memposition = (char*) malloc ( fontcompr.character[i].size / 8 + 3 );
               memset ( fontcompr.character[i].memposition , 0 , fontcompr.character[i].size/8+3 );
               word* pw = (word*) fontcompr.character[i].memposition;
               *pw = fontcompr.character[i].size;
               for (int j=0; j < font.character[i].size; j++) 
                  if ( font.character[i].memposition[ 2 + j] )
                    fontcompr.character[i].memposition[ 2 + j / 8 ] |= 1 << ( j % 8 ); 
            }
         }
      else {
         fontcompr = font;
         for ( i = 0; i < 256; i++ ) 
            if ( font.character[i].size ) {
               word* pw = (word*) fontcompr.character[i].memposition;
               *pw = fontcompr.character[i].size;
            }
      }

      strcat ( filename, ".fnt");

      fp = fopen ( filename, "wb" );
      if ( fp == NULL) {
         printf("error writing file \n");
         return;
      }
      strcpy ( fontcompr.id, fontid );
      fontcompr.id[48] = 6;
      fwrite ( &fontcompr, sizeof ( fontcompr ), 1, fp );
      for ( i = 0; i < 256; i++ ) {
         if ( fontcompr.character[i].size ) {
            fontcompr.character[i].diskposition = ftell ( fp );
            if ( font.color == 0 ) 
               fwrite( fontcompr.character[i].memposition, 1, fontcompr.character[i].size/8 + 3, fp );
            else
               fwrite( fontcompr.character[i].memposition, 1, fontcompr.character[i].size + 2, fp );
         }
      }
      fseek ( fp, 0, SEEK_SET );
      fwrite ( &fontcompr, sizeof ( fontcompr ), 1, fp );
      fclose ( fp );

      initsvga ( 0x101 );

      setvgapalette256( *activepalette256 );

      showtext ( "WAVprobetexta-3NautilusNarhwal-Klasse", 200, 200, &font );

      activefontsettings.length = 0;
      activefontsettings.background = 0;
      activefontsettings.color = 14;
      activefontsettings.justify = lefttext;
/*      activefontsettings.font = &font;
      showtext2 ( "WAVprobetexta", 100, 50);*/

      c = getch();
      settextmode ( 3 );

}



