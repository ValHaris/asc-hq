//     $Id: newfont.cpp,v 1.9 2000-10-11 14:26:44 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.8  2000/08/12 12:52:49  mbickel
//      Made DOS-Version compile and run again.
//
//     Revision 1.7  2000/01/04 19:43:53  mbickel
//      Continued Linux port
//
//     Revision 1.6  1999/12/29 17:38:18  mbickel
//      Continued Linux port
//
//     Revision 1.5  1999/12/28 21:03:13  mbickel
//      Continued Linux port
//      Added KDevelop project files
//
//     Revision 1.4  1999/11/25 22:00:06  mbickel
//      Added weapon information window
//      Added support for primary offscreen frame buffers to graphics engine
//      Restored file time handling for DOS version
//
//     Revision 1.3  1999/11/22 18:27:44  mbickel
//      Restructured graphics engine:
//        VESA now only for DOS
//        BASEGFX should be platform independant
//        new interface for initialization
//      Rewrote all ASM code in C++, but it is still available for the Watcom
//        versions
//      Fixed bugs in RLE decompression, BI map importer and the view calculation
//
//     Revision 1.2  1999/11/16 03:42:15  tmwilson
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

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>
#include "global.h"
#include "tpascal.inc"
#include "newfont.h"
#include "stack.h"
#include "basestrm.h"
#include "misc.h"
#include "basegfx.h"

#define blockread(a,b,c) fread(b,1,c,a)
#define seek(a,b) fseek(a,b,SEEK_SET)

const char* fontid = "I\'m a font file on your Disk. Leave me alone !\x01A\x0A0";

#define spacewidthkey 'n'

typedef dacpalette256 dacpal;

#ifdef _NOASM_
tfontsettings activefontsettings;

void expand(void* p1, void* q1, int size)
{
   char* src = (char*) p1;
   char* dst = (char*) q1;

   dst[0] = src[0];
   dst[1] = src[1];

   src+=2;
   dst+=2;

   while ( size ) {
      char al = *(src++);
      for ( int i = 0; i < 8; i++ )
         if ( size ) {
            if ( al & (1 << i ))
               dst[i] = 1;
            else
               dst[i] = 0;
   
            size--;
         }
      dst += 8;
   }
}


#endif


pfont        loadfont(char *       filename)
{                
   toldfont     *font1; 
   FILE         *fp;
   int          i; 
   char *p;
   void         *q;
   int      ll, ll2, completesize; 
   word*         pg;


   fp = fopen(filename, filereadmode );
   if ( !fp ) {
      printf("error opening file %s \n error code is %d \n",filename,errno);
      return(NULL);
   } /* endif */

   font1 = new ( toldfont ) ;


   fread( (void*) font1,1,sizeof(*font1),fp);
   if (ferror(fp) != 0) {
      printf("error rading file %s \n errno is %d  ; ferror is %d\n ",filename,errno,ferror(fp));
      return(NULL);
   } /* endif */

   font1->id[47] = 0;
             
   if ( strncmp(font1->id,fontid, 46) ) 
      return (NULL);

   completesize = 0; 

   font1->useems = false;

   if (font1->color) { 
     if ( font1->palette ) {
        fseek ( fp, sizeof(dacpalette256), SEEK_END );
        font1->palette = (dacpalette256*) new char [ sizeof ( dacpalette256 ) ];
        blockread(fp,*font1->palette,sizeof(dacpalette256));
     }
   } else  
      font1->palette = NULL; 
   
   for (i = 0; i <= 255; i++) { 
      if ( font1->character[i].size ) { 
         ll2 = ( font1->character[i].size / 8 + 1) * 8; 
         q = new char [ ll2+2 ];
         font1->character[i].memposition = (char*) q; 
        
         if (font1->color == false) { 
            ll = font1->character[i].size / 8 + 1;            
            p = new char [ ll + 2 ]; 
            seek(fp,font1->character[i].diskposition);
            blockread(fp,p,ll + 2);
            if (ferror(fp) != 0) {
               printf("error rading file %s \n errno is %d  ; ferror is %d\n ",filename,errno,ferror(fp));
               return(NULL);
            } /* endif */
            expand(p,q,font1->character[i].size);
            delete[] p; 
         } else {                                  
            seek(fp,font1->character[i].diskposition); 
            blockread(fp,q,font1->character[i].size + 2);
         } 
         pg = (word*) q;
         if (*pg != font1->character[i].size) {
            printf("invalid size of character %c\n",i);
         } /* endif */
         *pg = font1->character[i].width;
      } else {
        if (i == 32) {
           ll = font1->height * ( font1->character[spacewidthkey].width - 2 );
           p = new char [ ll+2 ];
           pg = (word*) p;
           memset(p,0,ll+2);
           *pg = font1->character[spacewidthkey].width - 2;
           font1->character[i].width = font1->character[spacewidthkey].width - 2;
           font1->character[i].size = ll;
           font1->character[i].memposition = (char*) p;
        } /* endif */
     }
   } 
   fclose(fp); 




   pfont font2   = new ( tfont );
   font2->name   = strdup ( font1->name );
   font2->color  = font1->color;
   font2->caps   = font1->caps ;
   font2->height = font1->height;
   font2->groundline = font1->groundline;
   font2->palette    = font1->palette;
   for (i = 0; i < 256 ;i++ ) {
      font2->character[i].width     =  font1->character[i].width;    
      font2->character[i].size      =  font1->character[i].size;
      font2->character[i].memposition =font1->character[i].memposition;
      for (int j = 0; j < 256 ; j++ ) {
          font2->kerning[j][i] = font1->kerning[font1->kernchartable[i]] [font1->kernchartable[j]];
          if ( font2->kerning[j][i] > 10  ||  font2->kerning[j][i] < - 10 )
             printf("fehler bei font %s ; kerning zwischen %c und %c \n",font2->name,i,j);
      }

   } /* endfor */

   delete font1 ;

   return font2;
} 



pfont        loadfont( pnstream stream )
{                
   toldfont     *font1; 
   char *p;
   void         *q;
   int      ll, ll2, completesize; 
   word*         pg;
   int i;

   font1 = new toldfont;

   stream->readdata ( font1, sizeof(*font1) );

   font1->id[47] = 0;
             
   if ( strncmp(font1->id,fontid, 46) ) 
      return (NULL);

   completesize = 0; 

   font1->useems = false;
   font1->palette = NULL; 

   for (i = 0; i <= 255; i++) { 
      if ( font1->character[i].size ) { 
         ll2 = ( font1->character[i].size / 8 + 1) * 8; 
         q = new char [ ll2+2 ];
         font1->character[i].memposition = (char*) q; 
        
         if (font1->color == false) { 
            ll = font1->character[i].size / 8 + 1;            
            p = new char [ ll + 2 ]; 
            stream->readdata ( p, ll + 2 );
            expand( p, q, font1->character[i].size );
            delete[] p; 
         } else 
            stream->readdata ( q, font1->character[i].size + 2 );
          
         pg = (word*) q;
         if (*pg != font1->character[i].size) {
            printf("invalid size of character %c\n",i);
         } /* endif */
         *pg = font1->character[i].width;
      } else {
        if (i == 32) {
           ll = font1->height * ( font1->character[spacewidthkey].width - 2 );
           p = new char [ ll+2 ];
           pg = (word*) p;
           memset(p,0,ll+2);
           *pg = font1->character[spacewidthkey].width - 2;
           font1->character[i].width = font1->character[spacewidthkey].width - 2;
           font1->character[i].size = ll;
           font1->character[i].memposition = (char*) p;
        } /* endif */
     }
   } 


  if (font1->color) { 
     if ( font1->palette ) {
        font1->palette = (dacpalette256*) new char [ sizeof ( dacpalette256 ) ];
        stream->readdata ( font1->palette, sizeof(dacpalette256));
     }
   } else  
      font1->palette = NULL; 
   


   pfont font2   = new ( tfont );
   font2->name   = strdup ( font1->name );
   font2->color  = font1->color;
   font2->caps   = font1->caps ;
   font2->height = font1->height;
   font2->groundline = font1->groundline;
   font2->palette    = font1->palette;
   for (i = 0; i < 256 ;i++ ) {
      font2->character[i].width     =  font1->character[i].width;    
      font2->character[i].size      =  font1->character[i].size;
      font2->character[i].memposition =font1->character[i].memposition;
      for (int j = 0; j < 256 ; j++ ) {
          font2->kerning[j][i] = font1->kerning[font1->kernchartable[i]] [font1->kernchartable[j]];
          if ( font2->kerning[j][i] > 10  ||  font2->kerning[j][i] < - 10 )
             printf("%c%c\n",i,j);
      }


   } /* endfor */


   delete font1 ;

   return font2;
} 



void shrinkfont ( pfont font, int diff )
{
   if ( font ) 
      for (int i = 0; i < 256 ;i++ ) 
         if (font->character[i].size ) 
            for (int j = 0; j < 256 ; j++ ) 
               if (font->character[j].size ) 
                  font->kerning[i][j] += diff;

}

int gettextwdth ( char* txt, pfont font )
{
   int i, result;

   result = 0;
   if (font == NULL)
      font = activefontsettings.font;
   if (font == NULL)
      return 0;

   i=0;
   while (txt[i] != 0) {
      if (font->character[txt[i]].size) {
         result +=2 + font->character[txt[i]].width;
         if ( txt[i+1] )                          //          if ((txt[i+1] != 0) && (txt[i+1] != ' ')) 
            result += font->kerning[txt[i+1]][txt[i]];
      } /* endif */
      i++;
   } /* endwhile */
   if (result > 2) {
      result -= 2;
   } else {
      result = 0;
   } /* endif */

   return result;
}


void         showtext3( const char *       txt,
                       word         xpos,
                       word         ypos)
{
   // collategraphicoperations cgo ( xpos, ypos, max ( activefontsettings.height, activefontsettings->font.height ),;

    char         *s, *s2, *s3;
    int         i, j,k;
    word         newx; 
    signed char  cnt;
    int         orgjus; 


   if (txt[0] == 0)
       return;
   npush( activefontsettings );
   s2 = new char [ 255 ];
   s  = new char [ 255 ];
   s3 = new char [ 255 ];
   s2[0] = 0;

   i = 0;
   cnt = 0; 
   do { 
      if (txt[i] != '~') {
         s2[cnt] = txt[i];
         cnt++;
         s2[cnt] = 0;
      } /* endif */
      i++;
   }  while ( txt[i] != 0 );

   cnt = -2;
   showtext2(s2,xpos,ypos); 
   i = 0;
   orgjus = activefontsettings.justify; 

   while ((txt[i] != '~') && (txt[i] != 0)) {
      i++;
   } /* endwhile */
   
   while (txt[i] != 0) {
      cnt+=2; 

      j = i + 1; 
      s[0] = 0;
      s3[0] = 0;
      k=0;
      activefontsettings.background = 255;

      while ((txt[j] != '~') && ( txt[j] != 0)) {
         s[k] = txt[j];
         k++;
         s[k] = 0;
         j++;
      } 

      if (s[0] != 0) {
         strncpy( s3, s2, k+i-cnt );
         s3[k+i-cnt]=0;
         if (orgjus == lefttext) { 
            newx = xpos + gettextwdth(s3, NULL) - gettextwdth(s,NULL);
         } 
         else 
            if (orgjus == centertext) { 
               newx = xpos + (activefontsettings.length - gettextwdth(s2, NULL)) / 2 + gettextwdth(s3,NULL) - gettextwdth(s,NULL) + 1;
            } 
            else 
               if (orgjus == righttext) { 
                  newx = xpos + 1 + (activefontsettings.length - gettextwdth(s2, NULL)) + gettextwdth(s3,NULL) - gettextwdth(s,NULL);
               } 
         activefontsettings.color = activefontsettings.markcolor; 
         activefontsettings.justify = lefttext; 
         showtext2(s,newx,ypos); 
      } 
      i = j + 1; 
      while ((txt[i] != '~') && (txt[i] != 0))
        i++;
   } 
   delete[] s3;
   delete[] s;
   delete[] s2;

   npop( activefontsettings );
} 






void         showtext3c( const char *       txt,
                       word         xpos,
                       word         ypos)
{
   // collategraphicoperations cgo;

    char         *s, *s2, *s3;
    int         i, j,k;
    word         newx; 
    signed char  cnt;
    int         orgjus; 


   npush( activefontsettings );
   s2 = new char [ 255 ];
   s  = new char [ 255 ];
   s3 = new char [ 255 ];
   s2[0] = 0;

   i = 0;
   cnt = 0; 
   while ( txt[i] ) { 
      if (txt[i] != '~') {
         s2[cnt] = txt[i];
         cnt++;
         s2[cnt] = 0;
      } /* endif */
      i++;
   } 

   cnt = -2;
   showtext2c(s2,xpos,ypos); 
   i = 0;
   orgjus = activefontsettings.justify; 

   while ((txt[i] != '~') && (txt[i] )) 
      i++;
   
   while ( txt[i] ) {
      cnt+=2; 

      j = i + 1; 
      s[0] = 0;
      s3[0] = 0;
      k=0;
      activefontsettings.background = 255;

      while ((txt[j] != '~') && ( txt[j] != 0)) {
         s[k] = txt[j];
         k++;
         s[k] = 0;
         j++;
      } 

      if (s[0] != 0) {
         strncpy( s3, s2, k+i-cnt );
         s3[k+i-cnt]=0;
         if (orgjus == lefttext) { 
            newx = xpos + gettextwdth(s3, NULL) - gettextwdth(s,NULL);
         } 
         else 
            if (orgjus == centertext) { 
               newx = xpos + (activefontsettings.length - gettextwdth(s2, NULL)) / 2 + gettextwdth(s3,NULL) - gettextwdth(s,NULL) + 1;
            } 
            else 
               if (orgjus == righttext) { 
                  newx = xpos + 1 + (activefontsettings.length - gettextwdth(s2, NULL)) + gettextwdth(s3,NULL) - gettextwdth(s,NULL);
               } 
         activefontsettings.font = activefontsettings.markfont; 
         activefontsettings.justify = lefttext; 
         showtext2c(s,newx,ypos); 
      } 
      i = j + 1; 
      while ((txt[i] != '~') && (txt[i] != 0))
        i++;
   } 
   delete[] s3;
   delete[] s;
   delete[] s2;

   npop( activefontsettings );
} 

void showtext4 ( const char* TextToOutput, int x1, int y1, ... )
{
   va_list paramlist;
   va_start ( paramlist, y1 );

   char tempbuf[1000];

   vsprintf( tempbuf, TextToOutput, paramlist );
   showtext2 ( tempbuf, x1, y1 );
}

void showtext4c ( const char* TextToOutput, int x1, int y1, ... )
{
   va_list paramlist;
   va_start ( paramlist, y1 );

   char tempbuf[1000];

   vsprintf( tempbuf, TextToOutput, paramlist );
   showtext2c ( tempbuf, x1, y1 );
}

class InitNewfont {
         public:
           InitNewfont ( void )
           {
              memset( &activefontsettings, 0, sizeof ( activefontsettings ));
           };
      } initnewfont;

char* int2string ( int i, char* buf )
{
   if ( i >= 0 ) {
      itoa ( i, buf, 10 );
   } else {
      buf[0] = '-';
      i = -i;
      itoa ( i, &buf[1], 10 );
   }
   if ( gettextwdth ( buf, NULL ) > activefontsettings.length   &&  activefontsettings.length) {
      char buf2[50];

      int pot  = (int) log10 ( i );
      int base = (int) pow ( 10, pot );
      int first = i / base;
      int rest = i - first * base;

      buf2[0] = digit[ first ][0];
      int p = 1;
      buf2[p] = 'E';
      buf2[p+1] = 0;
      strcat ( buf2, strrr ( pot ));

      while ( gettextwdth ( buf2, NULL ) < activefontsettings.length) {
         strcpy ( buf, buf2 );
         base /= 10;
         if ( p == 1 )
            buf2[p++] = '.';
         buf2[p++] = digit[ rest /  base ][0];
         rest = rest % base;

         buf2[p] = 'E';
         buf2[p+1] = 0;
         strcat ( buf2, strrr ( pot ));
      } /* endwhile */
   }
   return buf;

}
