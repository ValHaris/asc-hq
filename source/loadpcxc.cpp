//     $Id: loadpcxc.cpp,v 1.1 1999-11-22 18:27:40 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.2  1999/11/16 03:42:02  tmwilson
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

#include <stdio.h>
#include <string.h>

#include "basestrm.h"
#include "tpascal.inc"
#include "basegfx.h"
#include "misc.h"

typedef struct tpcxheader{
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
           byte     dummy[50]   ;
           int      size;            // patch to be able to read pcx files without seeking 
       }tpcxheader;


char loadpcxxy( pnstream stream, int x, int y, int setpalette )
{
   int read = 0;


   int m = y * agmp-> scanlinelength * agmp->byteperpix + x;

   tpcxheader header;

   stream->readdata ( &header, sizeof(header) );
   read += sizeof(header);

   if ( !header.size )
      return 12;

   int width = (header.xmax - header.xmin + 1 );
   int pixels = width * (header.ymax - header.ymin + 1) * header.nplanes;

   if ( header.manufacturer != 10 || 
        header.bitsperpixel != 8  || 
        header.xmax-header.xmin > agmp-> scanlinelength ) 
      return 11;

   int colors   = header.nplanes * header.bitsperpixel;

   int scanlineret[4];

   if (agmp->byteperpix > 1) {
      
      scanlineret[0] = agmp-> redfieldposition / 8;
      scanlineret[1] = header.bytesperline * agmp->byteperpix - agmp->greenfieldposition / 8 + agmp-> redfieldposition / 8;
      scanlineret[2] = header.bytesperline * agmp->byteperpix - agmp->bluefieldposition / 8 + agmp-> greenfieldposition / 8;
      scanlineret[3] = -(agmp-> scanlinelength  -
                       header.bytesperline* agmp->byteperpix + agmp->redfieldposition / 8 - agmp->bluefieldposition / 8);
                      
   } else {
      scanlineret[0]=0;
      scanlineret[1]=agmp-> scanlinelength - header.bytesperline * agmp->byteperpix;
   }

   int totalbytes = header.nplanes * header.bytesperline;


   int ttlbytes = 0;
   int xpos = 0;
   int planenum = 0;

   int byteperpix = agmp->byteperpix;


   char* dest = (char*) (agmp->linearaddress + x * agmp->byteperpix + y * agmp->bytesperscanline + scanlineret[0]);
   while ( pixels ) {
      char a;
      stream->readdata2 ( a );   // if you really want speed, use my asm code :-)
      read++;

      int count;
      if ( a >= 192 ) {
         count = a ^192;
         stream->readdata2 ( a );
         read++;
      } else
         count = 1;

      while ( count ) {
         if ( xpos >= header.bytesperline ) {
            planenum++;
            dest -= scanlineret[planenum];
            if ( planenum >= header.nplanes ) {
               planenum = 0;
               ttlbytes = 0;
            }
            xpos = 0;
         }
         if ( xpos < width )
            *dest = a;
         dest+= byteperpix;
         xpos++;
         ttlbytes++;
         count--;
         pixels--;
      } /* endwhile */

   } /* endwhile */


   if ( setpalette && colors == 8 ) {
      if ( header.size ) {
         int dataend = header.size - sizeof ( dacpalette256 ) - 1;
         while ( read < dataend ) {
            char scratch[100];
            int datalength;
            if ( 100 > dataend - read )
               datalength = dataend - read;
            else
               datalength = 100;
            stream->readdata ( scratch, datalength );
            read+= datalength;;
         }
      }    

      char c;
      stream->readdata ( &c, 1 );
      if ( c == 12 ) {
         dacpalette256 pal;
         stream->readdata ( pal, sizeof ( pal ) );
         for ( int i = 0; i < 3; i++ )
            for ( int j = 0; j < 256; j++ )
               pal[j][i] >>= 2;
         setvgapalette256 ( pal );
      }
   }

   return 0;
} 


char loadpcxxy (char *name, boolean setpal, word x, word y)
{
   tnfilestream s ( name, 1 );
   return loadpcxxy ( &s, x, y, setpal );
}


void writepcx ( char* name, int x1, int y1, int x2, int y2, dacpalette256 pal )
{
   tpcxheader header;
   memset ( &header, 0, sizeof (header ));
   header.manufacturer = 10;
   header.version = 5;
   header.encoding = 1;
   header.bitsperpixel = 8;
   header.xmin = 0;
   header.xmax = x2 - x1 ;
   header.ymin = 0;
   header.ymax = y2 - y1 ;
   header.hdpi = 0;
   header.vdpi = 0;
   if ( agmp->byteperpix == 1 )
      header.nplanes = 1;
   else
      header.nplanes = 3;

//   header.bytesperline = ((header.xmax - header.xmin) + 1) & 0xFFFE;
   header.bytesperline = x2 - x1 + 1 ;
   if ( header.bytesperline & 1 )
      header.bytesperline++;
   header.paletteinfo = 1;

   int fsize = 0;

   tn_file_buf_stream stream ( name, 2 );
   stream.writedata2 ( header );
   fsize += sizeof ( header );

   for ( int y = y1; y <= y2; y++ ) 
      for ( int plane = 0; plane < header.nplanes; plane++ ) {
         int lastbyte = -1;
         int count = 0;
         for ( int x = x1; x < x1 + header.bytesperline; x++ ) {
            char c = (getpixel( x, y ) >> ( plane * 8 )) & 0xff;
   
            if ( (lastbyte == c && count < 63) || lastbyte == -1 ) {
               count ++;
               lastbyte = c;
            }
            else {
               if ( count > 1 || lastbyte >= 192 ) {
                  char d = 192 + count;
                  stream.writedata2 ( d );
                  fsize += sizeof ( d );
               }
               char lstbyte = lastbyte;
               stream.writedata2 ( lstbyte );
               fsize += sizeof ( lstbyte );
               count = 1;
               lastbyte = c;
            }
            
         }
         if ( count > 1 || lastbyte >= 192 ) {
            char d = 192 + count;
            stream.writedata2 ( d );
            fsize += sizeof ( d );
         }
         char lstbyte = lastbyte;
         stream.writedata2 ( lstbyte );
         fsize += sizeof ( lstbyte );
   
      }

   if ( header.nplanes == 1 ) {
      char d = 12;
      stream.writedata2 ( d );
      fsize += sizeof ( d );
   
      dacpalette256 pal2;
   
      for ( int i = 0; i < 3; i++ )
          for ( int j = 0; j < 256; j++ )
              pal2[j][i] = pal[j][i] << 2;
   
      stream.writedata ( &pal2, 768 );
      fsize += 768 ;
   }
   stream.seekstream ( 0 );
   header.size = fsize;
   stream.writedata2 ( header );
}

structure_size_tester sst2;

