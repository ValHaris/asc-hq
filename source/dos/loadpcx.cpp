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

#include "../basestrm.h"
#include "../tpascal.inc"
#include "../basegfx.h"
#include "../misc.h"

typedef struct tpcxheader{
           char     manufacturer;
           char     version     ;
           char     encoding    ;
           char     bitsperpixel;
           word     xmin,ymin   ;
           word     xmax,ymax   ;
           word     hdpi,vdpi   ;
           char     colormap[48];
           char     reserved    ;
           char     nplanes     ;
           word     bytesperline;
           word     paletteinfo ;
           word     hscreensize ;
           word     vscreensize ;
           char     dummy[50]   ;
           int      size;
       }tpcxheader;


word  columncount;
extern "C" word  planenum;
extern "C" void* scratch;

extern "C" integer linelength;

integer spacelengthp;
integer spacelength;

extern "C" int  datatoread;
extern "C" int  dataread;

extern "C" int  scanlineret[3];
extern "C" size_t datalength;

const buffsize = 2000;

extern "C" void   decode_pcx256();
#pragma aux decode_pcx256 parm [ eax ] modify [ ebx ecx edx esi edi ]

extern "C" void   resetdata(int start);
#pragma aux resetdata modify [ eax ebx ecx edx ]


char loadpcxxy(char *name, char setpal, word x, word y)
{

/////////////
//variablen
////////////

  tpcxheader header;
  FILE       *fp;
  dacpalette256 pal;
  word       b,c;
  int        m;
  fpos_t     l;
  int       colors;

///////////
/// code
//////////

   m = y * agmp-> scanlinelength * agmp->byteperpix + x;

   fp = fopen(name, filereadmode );

   if (fp == NULL) {
      return(100);
   } /* endif */

      fread(&header,sizeof(header),1,fp);
       if (header.manufacturer != 10 || header.bitsperpixel != 8 || header.xmax-header.xmin > agmp-> scanlinelength ) {
         fclose(fp);
         return(11);
       }

   colors   = header.nplanes * header.bitsperpixel;
   planenum = header.nplanes;
   
   if (colors == 8) {
      fgetpos( fp, &l );
      b=0;
      fseek(fp, filesize( name ) - 769, SEEK_SET );
      fread( &b, 1, 1, fp );
      if (b != 12) {
         fclose( fp );
         return(10);
      } /* endif */
      fread( pal, 1, sizeof( pal ), fp );
      for (b=0;b <=255 ;b++ ) {
         for (c=0;c<=2 ;c++ ) {
            pal[b][c]>>= 2;
         } /* endfor */
      } /* endfor */
      if (setpal) {
         setvgapalette256(pal);
      } /* endif */
      fseek( fp,l, SEEK_SET );
   }

   linelength =  header.bytesperline;
   if (agmp->byteperpix > 1) {
      
      scanlineret[0] =agmp-> redfieldposition / 8;
      scanlineret[1] =  linelength * agmp->byteperpix - agmp->greenfieldposition / 8 + agmp-> redfieldposition / 8;
      scanlineret[2] =  linelength * agmp->byteperpix - agmp->bluefieldposition / 8 + agmp-> greenfieldposition / 8;
      scanlineret[3] = agmp-> scanlinelength  -
                       linelength* agmp->byteperpix + agmp->redfieldposition / 8 - agmp->bluefieldposition / 8;
                      
 

   } else {
      scanlineret[0]=0;
      scanlineret[1]=agmp-> scanlinelength - linelength* agmp->byteperpix;
   }

/*
   if (maxavail < buffsize) {
      buffsize=maxavail;
   }
*/

   datatoread=(header.ymax-header.ymin+1) * header.bytesperline * header.nplanes;
   dataread=datatoread;


   if ( agmp->windowstatus == 100 ) 
      resetdata( m );
   else {
      setvirtualpagepos( m >> 16 );
      resetdata( m & 0xffff );
   }


   scratch = new char [ buffsize*2 ];

   do {

      datalength = fread( scratch, 1, buffsize,fp) + 1;
      decode_pcx256();
 
   } while ( !feof(fp) && dataread > 0); /* enddo */
   int a = ftell ( fp );
   fclose(fp);
   delete[] scratch;
   return(0);
} /* endif */
  




char loadpcxxy( tnstream* stream, int x, int y, int setpalette )
{
   int read = 0;


   int m = y * agmp-> scanlinelength * agmp->byteperpix + x;

   tpcxheader header;

   stream->readdata ( &header, sizeof(header) );
   read += sizeof(header);

   if ( !header.size )
      return 12;

   int dataend = header.size;

   if ( header.manufacturer != 10 || 
        header.bitsperpixel != 8  || 
        header.xmax-header.xmin > agmp-> scanlinelength ) 
      return 11;

   int colors   = header.nplanes * header.bitsperpixel;
   if ( colors == 8 )
      dataend -= (sizeof ( dacpalette256 ) + 1);

   planenum = header.nplanes;
   
   linelength =  header.bytesperline;
   if (agmp->byteperpix > 1) {
      
      scanlineret[0] = agmp-> redfieldposition / 8;
      scanlineret[1] =  linelength * agmp->byteperpix - agmp->greenfieldposition / 8 + agmp-> redfieldposition / 8;
      scanlineret[2] =  linelength * agmp->byteperpix - agmp->bluefieldposition / 8 + agmp-> greenfieldposition / 8;
      scanlineret[3] = agmp-> scanlinelength  -
                       linelength* agmp->byteperpix + agmp->redfieldposition / 8 - agmp->bluefieldposition / 8;
                      
   } else {
      scanlineret[0]=0;
      scanlineret[1]=agmp-> scanlinelength - linelength* agmp->byteperpix;
   }


   datatoread=(header.ymax-header.ymin+1) * header.bytesperline * header.nplanes;
   dataread=datatoread;

   if ( agmp->windowstatus == 100 ) 
      resetdata( m );
   else {
      setvirtualpagepos( m >> 16 );
      resetdata( m & 0xffff );
   }

   scratch = new char [ buffsize ];

   do {
      if ( buffsize > dataend - read )
         datalength = dataend - read;
      else
         datalength = buffsize;

      stream->readdata ( scratch, datalength );
      read += datalength;

      datalength ++ ;
      decode_pcx256();
 
   } while ( header.size > read && dataread ); /* enddo */

   if ( setpalette && colors == 8 ) {
      while ( read < dataend ) {
         if ( buffsize > dataend - read )
            datalength = dataend - read;
         else
            datalength = buffsize;
         stream->readdata ( scratch, datalength );
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

   delete[] scratch;

   return 0;
} 

#define writedata2(a)  writedata ( &(a), sizeof(a) )
#define readdata2(a)   readdata  ( &(a), sizeof(a) )

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
   stream.seek ( 0 );
   header.size = fsize;
   stream.writedata2 ( header );
}

structure_size_tester sst2;

