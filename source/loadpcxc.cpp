/*! \file loadpcxc.cpp
    \brief Loading and storing images in PCX format, written in C

    There is also an optimized assembler version for the DOS version around
    which is called #loadpcx.cpp , but not used any more.
*/

//     $Id: loadpcxc.cpp,v 1.17 2001-08-09 22:08:22 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.16  2001/08/02 15:33:01  mbickel
//      Completed text based file formats
//
//     Revision 1.15  2001/07/27 21:13:35  mbickel
//      Added text based file formats
//      Terraintype and Objecttype restructured
//
//     Revision 1.14  2001/02/28 14:10:05  mbickel
//      Added some small editors to linux makefiles
//      Added even more dirty hacks to basegfx: some more truecolor functions
//
//     Revision 1.13  2001/02/18 15:37:15  mbickel
//      Some cleanup and documentation
//      Restructured: vehicle and building classes into separate files
//         tmap, tfield and helper classes into separate file (gamemap.h)
//      basestrm : stream mode now specified by enum instead of int
//
//     Revision 1.12  2001/02/11 11:39:39  mbickel
//      Some cleanup and documentation
//
//     Revision 1.11  2001/01/28 14:04:14  mbickel
//      Some restructuring, documentation and cleanup
//      The resource network functions are now it their own files, the dashboard
//       as well
//      Updated the TODO list
//
//     Revision 1.10  2001/01/25 23:45:01  mbickel
//      Moved map displaying routins to own file (mapdisplay.cpp)
//      Wrote program to create pcx images from map files (map2pcx.cpp)
//      Fixed bug in repair function: too much resource consumption
//      AI improvements and bug fixes
//      The BI3 map import function now evaluates the player status (human/
//       computer)
//
//     Revision 1.9  2000/12/23 13:19:47  mbickel
//      Made ASC compileable with Borland C++ Builder
//
//     Revision 1.8  2000/11/09 17:48:47  mbickel
//      The size of a stream can now be queried
//      PCX loader (in C) can now load unpatched images provided they are not
//        compressed
//
//     Revision 1.7  2000/10/11 14:26:43  mbickel
//      Modernized the internal structure of ASC:
//       - vehicles and buildings now derived from a common base class
//       - new resource class
//       - reorganized exceptions (errors.h)
//      Split some files:
//        typen -> typen, vehicletype, buildingtype, basecontainer
//        controls -> controls, viewcalculation
//        spfst -> spfst, mapalgorithm
//      bzlib is now statically linked and sources integrated
//
//     Revision 1.6  2000/08/12 15:03:24  mbickel
//      Fixed bug in unit movement
//      ASC compiles and runs under Linux again...
//
//     Revision 1.5  2000/08/03 19:21:26  mbickel
//      Fixed: units had invalid height when produced in some buildings
//      Fixed: units could not enter building if unitheightreq==0
//      Started adding SDL_image support
//      Upgraded to SDL1.1.3 (for SDL_image)
//
//     Revision 1.4  2000/05/23 21:09:26  mbickel
//      Removed boolean data type
//
//     Revision 1.3  1999/12/28 21:03:07  mbickel
//      Continued Linux port
//      Added KDevelop project files
//
//     Revision 1.2  1999/12/27 13:00:06  mbickel
//      new vehicle function: each weapon can now be set to not attack certain
//                            vehicles
//
//     Revision 1.1  1999/11/22 18:27:40  mbickel
//      Restructured graphics engine:
//        VESA now only for DOS
//        BASEGFX should be platform independant
//        new interface for initialization
//      Rewrote all ASM code in C++, but it is still available for the Watcom
//        versions
//      Fixed bugs in RLE decompression, BI map importer and the view calculation
//
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

#pragma pack(1)

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
           int      size;            // patch to be able to read pcx files without seeking 
       }tpcxheader;

#pragma pack()


int pcxGetColorDepth ( const ASCString& filename, int* width, int* height )
{
   tnfilestream stream ( filename.c_str(), tnstream::reading );

   tpcxheader header;

   stream.readdata ( &header, sizeof(header) );

   if ( width )
      *width = header.xmax - header.xmin + 1 ;

   if ( height )
      *height = header.ymax - header.ymin + 1;


   return header.nplanes * header.bitsperpixel;
}


char loadpcxxy( pnstream stream, int x, int y, bool setpalette, int* xsize, int* ysize )
{
   int read = 0;

   // int m = y * agmp-> scanlinelength * agmp->byteperpix + x;

   tpcxheader header;

   stream->readdata ( &header, sizeof(header) );
   read += sizeof(header);

   if ( !header.size ) {
      if ( stream->getSize() < 0 )
         return 12;
      else
         header.size = stream->getSize();
   }

   int width = header.xmax - header.xmin + 1 ;
   int height = header.ymax - header.ymin + 1;
   int pixels = width * height * header.nplanes;

   if ( xsize )
      *xsize = width;

   if ( ysize )
      *ysize = height;

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
      scanlineret[1]= -(agmp->scanlinelength - header.bytesperline * agmp->byteperpix);
   }

   // int totalbytes = header.nplanes * header.bytesperline;


   int ttlbytes = 0;
   int xpos = 0;
   int planenum = 0;

   int byteperpix = agmp->byteperpix;


   // some simple buffering
   char* buf = new char[header.size];
   // int bufdata =
   stream->readdata ( buf, header.size - sizeof( header ));
   int actpos = 0;


   char* dest = (char*) (agmp->linearaddress + x * agmp->byteperpix + y * agmp->bytesperscanline + scanlineret[0]);
   while ( pixels ) {
      char a;

      a = buf[actpos++];

      read++;

      int count;
      if ( a >= 192 ) {
         count = a ^192;

         a = buf[actpos++];

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
         if ( xpos < width ) {
            *dest = a;
            pixels--;
         }
         dest+= byteperpix;
         xpos++;
         ttlbytes++;
         count--;
      } /* endwhile */

   } /* endwhile */

   if ( setpalette && colors == 8 ) {
      if ( header.size ) {
         int dataend = header.size - sizeof ( dacpalette256 ) - 1 - sizeof ( header );
         actpos = dataend;
         /*
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
         */
      }    

      char c = buf[actpos++];
      if ( c == 12 ) {
         dacpalette256 pal;
         memcpy ( pal, &buf[actpos], sizeof ( pal ));
         // stream->readdata ( pal, sizeof ( pal ) );
         for ( int i = 0; i < 3; i++ )
            for ( int j = 0; j < 256; j++ )
               pal[j][i] >>= 2;
         setvgapalette256 ( pal );
      }
   }

   delete[] buf;
   return 0;
} 


char loadpcxxy ( const ASCString& name, bool setpal, int xpos, int ypos, int* xsize, int* ysize )
{
   tnfilestream s ( name, tnstream::reading );
   return loadpcxxy ( &s, xpos, ypos, setpal, xsize, ysize );
}


void writepcx ( const ASCString& name, int x1, int y1, int x2, int y2, dacpalette256 pal )
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

   tn_file_buf_stream stream ( name, tnstream::writing );
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

