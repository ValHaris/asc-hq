//     $Id: loadbi3.cpp,v 1.27 2000-10-12 19:00:21 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.26  2000/10/11 14:26:41  mbickel
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
//     Revision 1.25  2000/08/12 09:17:30  gulliver
//     *** empty log message ***
//
//     Revision 1.24  2000/08/06 11:39:10  mbickel
//      New map paramter: fuel globally available
//      Mapeditor can now filter buildings too
//      Fixed unfreed memory in fullscreen image loading
//      Fixed: wasted cpu cycles in building
//      map parameters can be specified when starting a map
//      map parameters are reported to all players in multiplayer games
//
//     Revision 1.23  2000/08/03 19:21:25  mbickel
//      Fixed: units had invalid height when produced in some buildings
//      Fixed: units could not enter building if unitheightreq==0
//      Started adding SDL_image support
//      Upgraded to SDL1.1.3 (for SDL_image)
//
//     Revision 1.22  2000/08/02 10:28:27  mbickel
//      Fixed: generator vehicle not working
//      Streams can now report their name
//      Field information shows units filename
//
//     Revision 1.21  2000/07/29 14:54:35  mbickel
//      plain text configuration file implemented
//
//     Revision 1.20  2000/07/16 14:20:03  mbickel
//      AI has now some primitive tactics implemented
//      Some clean up
//        moved weapon functions to attack.cpp
//      Mount doesn't modify PCX files any more.
//
//     Revision 1.19  2000/07/02 21:04:13  mbickel
//      Fixed crash in Replay
//      Fixed graphic errors in replay
//
//     Revision 1.18  2000/05/28 21:05:03  mbickel
//      Linux version of mapedit was unable to load buildings in BI3 map
//
//     Revision 1.17  2000/05/21 17:18:50  mbickel
//      Moved BI3 import tables to a separate file: objxlat.cpp
//
//     Revision 1.16  2000/05/11 20:12:05  mbickel
//      mapedit(lin) can now import BI3 maps
//
//     Revision 1.15  2000/04/27 16:25:24  mbickel
//      Attack functions cleanup
//      New vehicle categories
//      Rewrote resource production in ASC resource mode
//      Improved mine system: several mines on a single field allowed
//      Added unitctrl.* : Interface for vehicle functions
//        currently movement and height change included
//      Changed timer to SDL_GetTicks
//
//     Revision 1.14  2000/04/02 21:51:08  mbickel
//      Fixed bugs graphic set loading routines
//
//     Revision 1.13  2000/04/01 16:54:29  mbickel
//      Updated BI2PCX to use the new interchangeable graphic sets
//      Tagged ASC1-0-0
//
//     Revision 1.12  2000/04/01 11:38:39  mbickel
//      Updated the small editors
//      Added version numbering
//
//     Revision 1.11  2000/03/11 18:22:06  mbickel
//      Added support for multiple graphic sets
//
//     Revision 1.10  2000/01/04 19:43:52  mbickel
//      Continued Linux port
//
//     Revision 1.9  2000/01/02 19:47:08  mbickel
//      Continued Linux port
//      Fixed crash at program exit
//
//     Revision 1.8  1999/12/28 21:03:01  mbickel
//      Continued Linux port
//      Added KDevelop project files
//
//     Revision 1.7  1999/12/14 20:23:55  mbickel
//      getfiletime now works on containerfiles too
//      improved BI3 map import tables
//      various bugfixes
//
//     Revision 1.6  1999/12/07 22:13:20  mbickel
//      Fixed various bugs
//      Extended BI3 map import tables
//
//     Revision 1.5  1999/11/22 18:27:32  mbickel
//      Restructured graphics engine:
//        VESA now only for DOS
//        BASEGFX should be platform independant
//        new interface for initialization
//      Rewrote all ASM code in C++, but it is still available for the Watcom
//        versions
//      Fixed bugs in RLE decompression, BI map importer and the view calculation
//
//     Revision 1.4  1999/11/18 17:31:13  mbickel
//      Improved BI-map import translation tables
//      Moved macros to substitute Watcom specific routines into global.h
//
//     Revision 1.3  1999/11/16 17:04:08  mbickel
//     Made ASC compilable for DOS again :-)
//     Merged all the bug fixes in that I did last week
//
//     Revision 1.2  1999/11/16 03:41:54  tmwilson
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
#include <stdlib.h>
#include <string.h>

#include "loadbi3.h"
#include "newfont.h"
#include "basegfx.h"
#include "keybp.h"
#include "typen.h"
#include "buildingtype.h"
#include "vehicletype.h"
#include "misc.h"
#include "stack.h"
#include "palette.h"
#include "gameoptions.h"

#ifndef converter
 #include "dlg_box.h"
#endif

#ifndef HEXAGON
#error This file should only by used in the hexagonal version
#endif

#pragma pack(1)


class ActiveGraphicPictures {
   public:
     int activeId;
     int maxnum;
     int currentnum;
     void** bi3graphics;
     int* bi3graphmode;        // 0: no picture available ( should not happen ingame )
                               // 1: picture has BI size
                               // 2: picture has ASC size
                               // +0xff : picture is dummy picture
     int absoluteMaxPicSize;
     int setActive ( int id );

     ActiveGraphicPictures ( void ) {
        activeId = -1;
     };

     void alloc ( void );
     int picAvail ( int num );
     void* getPic ( int num );
     int getMode ( int num );
} activeGraphicPictures;


void ActiveGraphicPictures :: alloc ( void ) 
{
    for ( int i = 0; i < maxnum; i++ )
       bi3graphics[i] = asc_malloc ( absoluteMaxPicSize );
}

int ActiveGraphicPictures :: picAvail ( int num ) 
{
   if ( num < currentnum && bi3graphmode[num])
      return 1;
   else
      return 0;
}

void* ActiveGraphicPictures :: getPic ( int num ) 
{
   if ( picAvail ( num ))
      return bi3graphics[num];
   else
      return NULL;
}

int ActiveGraphicPictures :: getMode ( int num ) 
{
   if ( picAvail ( num ))
      return bi3graphmode[num];
   else
      return 0;
}



int paletteloaded = 0;

int marked = -1;
int bi3graphnum = 0;

int battleisleversion = -1;

int keeporiginalpalette = 0;


class timporterror : public ASCexception {
};



class GraphicSet {
         public:
           int id;
           int picnum;
           int singlepicsize;
           int maxPicSize;
           dynamic_array<void*> pic;
           dynamic_array<int>   picmode;
     };




dynamic_array<GraphicSet*> graphicSet;
int graphicSetNum = 0;


int ActiveGraphicPictures :: setActive ( int id )
{
   if ( id == activeId )
      return id;


   GraphicSet* gs = NULL;
   int found = 0;
   while ( !found ) {
      for ( int i = 0; i < graphicSetNum; i++ )
         if ( graphicSet[i]->id == id ) {
            found++;
            gs = graphicSet[i];
         }

     #ifndef converter
      if ( !found && id== 0 )
         displaymessage2 ( "fatal error: no default graphic set ( id=0 ) found !", 2);
     #endif

      if ( !found )
         id = 0;
   }
   if ( activeId != id ) {
      for ( int i = 0; i < gs->picnum; i++ ) {
         memcpy ( bi3graphics[i], gs->pic[i], getpicsize2 ( gs->pic[i] ));
         bi3graphmode[i] = gs->picmode[i];
      }
      activeId = id;
      currentnum = gs->picnum;
   }
   return id;
}


const int LIBFilesAnz  = 11;

struct  TLIBFiles {
      char*  Name;
      int FirstRecO;
      int RecSize;
      int DataInRecOfs;
      int DataSize;
      int Anzahl;
  }; ;
 TLIBFiles LIBFiles [ LIBFilesAnz ] = 
      {{"part000.lib",  0xC, 0x240,   0, 0x240, 1335},
      {"unit000.lib",  0x4, 0x240,   0, 0x240, 128},   // 128 f?r BI3   // 118 f?r BI2 Scenery
      {"layr000.lib",  0x4, 0x240,   0, 0x240, 24},
      {"layr001.lib",  0x4, 0x240,   0, 0x240, 24},
      {"layr002.lib",  0x4, 0x240,   0, 0x240, 24},
      {"layr003.lib",  0x4, 0x240,   0, 0x240, 24},
      {"layr004.lib",  0x4, 0x240,   0, 0x240, 24},
      {"layr005.lib",  0x4, 0x240,   0, 0x240, 24},
      {"layr006.lib",  0x4, 0x240,   0, 0x240, 24},
      {"curs000.lib",  0xC, 0x25E,0x1A, 0x240, 12},
      {"palt000.lib",  0xC, 0x300,   0, 0x300, 4 } ,
      } ;
      

int libs_to_load = 9;

void check_bi3_dir ( void )
{
   loadbi3graphics();
}

void checkbi3dir ( void )
{
   char temp[1000];

   if ( !CGameOptions::Instance()->bi3.dir.getName() ) {
      readgameoptions();
      /*
      if ( !gameoptions.bi3.dir.getName() ) {
         gameoptions.bi3.dir = new char [300];
         gameoptions.bi3.dir[0] = 0;
      }
      */
   }

   int notfound;
   do {
      notfound = 0;

      for ( int i = 0; i < libs_to_load ; i++ ) {
		  if ( CGameOptions::Instance()->bi3.dir.getName() )
            strcpy ( temp, CGameOptions::Instance()->bi3.dir.getName() );
         else
            temp[0] = 0;

         strcat ( temp, LIBFiles[i].Name );
         
         if ( !exist ( temp ) ) {
            if ( CGameOptions::Instance()->bi3.dir.getName() )
               strcpy ( temp, CGameOptions::Instance()->bi3.dir.getName() );
            else
               temp[0] = 0;

            strcat ( temp, "LIB" );
            strcat ( temp, pathdelimitterstring );
            strcat ( temp, LIBFiles[i].Name );
            if ( !exist ( temp ) ) {
               printf("Battle Isle file %s not found !\n", temp );
               notfound++;
            }
         }
      }
     if ( notfound ) {
         if ( !graphicinitialized ) {
            char bi3path[10000];
            printf("Enter Battle Isle directory:\n" );
            scanf ( "%s", bi3path );
            if ( bi3path[ strlen ( bi3path )-1 ] != pathdelimitter )
               strcat ( bi3path, pathdelimitterstring );

            CGameOptions::Instance()->bi3.dir.setName ( bi3path );
            CGameOptions::Instance()->setChanged();
         } else {
            closegraphics();
            printf("\nplease run ASC first to create a config file !\n");
            exit(0);
         }
     }
   } while ( notfound ); /* enddo */
   battleisleversion = 3;
}

tpixelxlattable bi2asc_color_translation_table;

class initloadbi3 {
      public:
         initloadbi3 ( void ) { bi2asc_color_translation_table[0] = 254; };
      } LoadBi3init;


int getGraphicSetIdFromFilename ( const char* filename )
{
      tnfilestream s ( filename, 1 );

      int magic;
      s.readdata2 ( magic );
      if ( magic == -1 ) {
         int id;
         s.readdata2 ( id );
         return id;
      } else
         return 0;
}

void loadbi3graphics( void )
{                
   if ( activeGraphicPictures.activeId >= 0 )
      return;

   #ifdef logging
   logtofile("loadbi3graphics");
   #endif

   if ( !keeporiginalpalette )
      if ( bi2asc_color_translation_table[0] == 254 ) 
         loadpalette();


   int highestPicNum = 0;
   bi3graphnum = maxint; 

   int absoluteMaxPicSize = 0;

   void* emptyfield;
   {
      int o;
      tnfilestream s ( "emptyfld.raw", 1 );
      s.readrlepict ( &emptyfield, false, &o );
   }
   int emptyfieldsize = getpicsize2 ( emptyfield );


   tfindfile ff ( "*.gfx" );
   char* filename = ff.getnextname();
   while ( filename ) {

      tnfilestream s ( filename, 1 );

      int magic;
      s.readdata2 ( magic );
      if ( magic == -1 ) {

         GraphicSet* gs = new GraphicSet;

         s.readdata2 ( gs->id );
         s.readdata2 ( gs->picnum );
         s.readdata2 ( gs->maxPicSize );

         if ( absoluteMaxPicSize < gs->maxPicSize )
            absoluteMaxPicSize = gs->maxPicSize;

         int* picmode = new int[gs->picnum];
         s.readdata ( picmode, gs->picnum * sizeof( int ) );
   
         for ( int i = 0; i < gs->picnum; i++ ) {
            if ( picmode[i] >= 1 ) {
               int o;
               void* p;
               s.readrlepict ( &p, false, &o );
               gs->pic[i] = p;
               gs->picmode[i] = picmode[i];
            } else {
               void* p = asc_malloc ( emptyfieldsize );
               memcpy ( p, emptyfield, emptyfieldsize );
               gs->pic[i] = p;
               gs->picmode[i] = 256 + 2;
            }
         }
         if ( gs->picnum > highestPicNum )
            highestPicNum = gs->picnum;

         delete[] picmode;

         graphicSet[graphicSetNum++] = gs;
      }

      filename = ff.getnextname();
   }
   
   for ( int i = 0; i < graphicSetNum; i++ ) {
       GraphicSet* gs = graphicSet[i];
       for ( int j = gs->picnum; j < highestPicNum; j++ ) {
           void* p = asc_malloc ( emptyfieldsize );
           memcpy ( p, emptyfield, emptyfieldsize );
           gs->pic[j] = p;
           gs->picmode[j] = 256 + 2;
       }
       gs->picnum = highestPicNum;
   }
   bi3graphnum = highestPicNum;


   activeGraphicPictures.bi3graphics = new void*[highestPicNum]; 
   activeGraphicPictures.bi3graphmode = new int[highestPicNum];
   activeGraphicPictures.maxnum = highestPicNum;
   activeGraphicPictures.absoluteMaxPicSize = absoluteMaxPicSize;
   activeGraphicPictures.alloc ( );
   activeGraphicPictures.setActive ( 0 );

  /*

   else {

   if ( picsfound < 1673 ) {
      checkbi3dir();
      for ( int lib = 0; lib < libs_to_load; lib++ ) {
         char filename[260];
   
         strcpy ( filename, gameoptions.bi3.dir );
         strcat ( filename, LIBFiles[ lib ].Name );
         
         tfindfile ff ( filename );
         if ( !ff.getnextname() ) {
            strcpy ( filename, gameoptions.bi3.dir );
            strcat ( filename, "LIB\\" );
            strcat ( filename, LIBFiles[ lib ].Name );
            tfindfile ff2 ( filename );
            if ( !ff2.getnextname() ) 
               throw tfileerror ( filename );
         }
   
   
   
         tn_file_buf_stream stream ( filename, 1 );
         int p = 0;
         p = p * LIBFiles[ lib ].RecSize + LIBFiles[ lib ].FirstRecO + LIBFiles[ lib ].DataInRecOfs;
         stream.seekstream ( p );
      
         for ( int i = 0; i < LIBFiles[ lib ].Anzahl; i++ ) {
            if ( !bi3graphics[bi3graphnum] ) {
               void* pic = new char[ LIBFiles[ lib ].DataSize + 4 ];
               word* pw = (word*) pic;
               pw[0] = 23;
               pw[1] = 23;
               unsigned char* pc = (unsigned char*) &pw[2];
               stream.readdata ( pc, LIBFiles[ lib ].DataSize );
               for ( int m = 0; m < LIBFiles[ lib ].DataSize; m++ ) {
                  if ( lib == 1 ) {
                     if (  pc [ m ] == 0 )
                        pc [ m ] = 88;
                     if ( pc [ m ] == 143 )
                        pc [ m ] = 0;
                  }
                  if ( !keeporiginalpalette )
                     pc[m] = bi2asc_color_translation_table [ pc [ m ]];
                  else
                     if ( pc[m] == 0 )
                        pc[m] = 255;
               }
               bi3graphics[bi3graphnum] = pw;
               bi3graphmode[bi3graphnum] = 1;
            } else {
               int blackhole[1000];
               stream.readdata ( blackhole, LIBFiles[ lib ].DataSize );
            }
            bi3graphnum++;
         }
   
         while ( bi3graphnum % 10 ) 
            bi3graphnum++;
            
      }
   } 
   #ifdef logging
   logtofile("loading of bi3 graphics finished");
   #endif
  */

}

extern dacpalette256 pal;

class tgetbi3pict {
   int start ;
//   int marked ;

   int xn ;
   int xs ;
   int yn ;
   int ys ;
   int picsize ;
   static pfont fnt;

   public:
      tgetbi3pict ( void );
      void paint ( void );
      void run ( int* num );

};

pfont tgetbi3pict :: fnt = NULL;

tgetbi3pict :: tgetbi3pict ( void )
{
   start = 0;
//   marked = -1;

   xn = 10;
   xs = 50;
   yn = 8;
   ys = 50;
   picsize = 24;

   if ( !bi3graphnum )
      loadbi3graphics();

   if ( !paletteloaded ) {
     loadpalette();
     paletteloaded = 1;
   }
   setvgapalette256(pal); 

}


void tgetbi3pict :: paint ( void )
{
   for ( int y = 0; y < yn; y++ ) {
      showtext2 ( strrr ( start + y * xn ), 10, 20 + y * ys );
      for (int x = 0; x < xn; x++ ) {
         int num = start + y * xn + x;
         bar ( 50 + x * xs, 10 + y * ys, 80 + x * xs, 40 + y * ys, black );
         if ( marked == num ) 
            rectangle ( 50 + x * xs, 10 + y * ys, 80 + x * xs, 40 + y * ys, yellow );

         if ( num < bi3graphnum  &&  activeGraphicPictures.picAvail ( num ) ) {
            void* buf;
            loadbi3pict ( num, &buf );
            putspriteimage ( 50 + x * xs + 30/2 - picsize/2, 10 + y * ys + 30/2 - picsize/2, buf );
            delete buf;
         }

      } /* endfor */
   }
}

void tgetbi3pict :: run ( int* num )
{
   if ( !fnt ) {
      tnfilestream stream ( "monogui.fnt", 1 );
      fnt = loadfont  ( &stream );
   }

   npush ( activefontsettings );

   activefontsettings.color = white;
   activefontsettings.justify = lefttext;
   activefontsettings.height = 0;
   activefontsettings.font = fnt;
   activefontsettings.length = 40;
   activefontsettings.background = black;


   if ( marked >= 0 ) {
     while ( marked < start ) 
        start -= xn;
     while ( marked >= start + yn * xn ) 
        start += xn;
   }

   paint();
   tkey ch;
   do {
      ch = r_key ();
      int oldmark = marked;
      int oldstart = start;

      if ( ch == ct_up ) {
         marked -= xn;
         if ( marked < 0 )
            marked = 0;
      }
      if ( ch == ct_down ) {
         marked += xn;
         if ( marked >= bi3graphnum )
            marked = bi3graphnum-1;
      }
      if ( ch == ct_right ) {
         marked += 1;
         if ( marked >= bi3graphnum )
            marked = bi3graphnum-1;
      }
      if ( ch == ct_left ) {
         marked -= 1;
         if ( marked < 0 )
            marked = 0;
      }
      if ( marked >= 0 ) {
        while ( marked < start ) 
           start -= xn;
        while ( marked >= start + yn * xn ) 
           start += xn;
      }
      if ( marked != oldmark || oldstart != start )
         paint();

   } while ( ch != ct_enter || marked < 0 ); /* enddo */

//   *picture = bi3graphics[marked];
   *num = marked;

   npop ( activefontsettings );

}

void getbi3pict ( int* num, void** picture )
{
   tgetbi3pict gbi3p;
   gbi3p.run ( num );
   loadbi3pict_double ( *num, picture );
}


void getbi3pict_double ( int* num, void** picture )
{
   tgetbi3pict gbi3p;
   gbi3p.run ( num );

   loadbi3pict_double ( *num, picture );

}        
         
int  loadbi3pict_double ( int num, void** pict, int interpolate, int reference )
{
   if ( !bi3graphnum )
      loadbi3graphics();

   if ( ! activeGraphicPictures.picAvail ( num ) ) {
      *pict = NULL;
      return -1;
   } 

   if ( (activeGraphicPictures.getMode ( num ) & 0xff ) == 1 ) {

      if( !interpolate ) {
     
         void* buf = new char [ imagesize ( 100, 100, 99+fieldxsize, 99+fieldysize )];
      
         char* src = (char*) activeGraphicPictures.getPic ( num );
         char* dst = (char*) buf;
         dst[0] = fieldxsize-1;
         dst[1] = 0;
         dst[2] = fieldysize-1;
         dst[3] = 0;
         dst+=4;
      
         src+=4;
      
         for ( int y = 0; y < fieldysize; y++ )
           for ( int x = 0; x < fieldxsize; x++ ) 
              dst[y * fieldxsize+x] = src[ (y/2)*(fieldxsize/2)+(x/2) ];
           
         *pict = buf;
         return 0;
      
   /*
         TrueColorImage* zimg = zoomimage ( bi3graphics[num], fieldxsize, fieldysize, pal, 1 );
         void* pic = convertimage ( zimg, pal ) ;
         delete zimg;
     */
     //    *pict = pic;
   
      } else {
   
         TrueColorImage* zimg = zoomimage ( activeGraphicPictures.getPic( num ), fieldxsize, fieldysize, pal, 1 );
         void* pic = convertimage ( zimg, pal ) ;
         delete zimg;
         *pict = pic;
         return 0;
      }
   } else
      if ( (activeGraphicPictures.getMode( num ) & 0xff ) == 2 ) {
         if ( reference == 1) {
            *pict = activeGraphicPictures.getPic ( num );
            return 1;
         } else {
            int sz = getpicsize2 ( activeGraphicPictures.getPic ( num ) );
            void* buf = asc_malloc ( sz );
            memcpy ( buf, activeGraphicPictures.getPic ( num ), sz );
            *pict = buf;
            return 0;
         } /* endif */
      } else {
        *pict = NULL;
        return -1;
      }
}

void loadbi3pict ( int num, void** pict )
{
   if ( !bi3graphnum )
      loadbi3graphics();

   if ( !activeGraphicPictures.picAvail( num ) ) {
      *pict = NULL;
      return;
   } 

   if ( (activeGraphicPictures.getMode( num ) & 0xff ) == 1 ) {
      char* buf = new char [ imagesize ( 100, 100, 99+fieldxsize/2, 99+fieldysize/2 )];
   
      char* src = (char*) activeGraphicPictures.getPic( num );
      char* dst = (char*) buf;
      dst[0] = fieldxsize/2-1;
      dst[1] = 0;
      dst[2] = fieldysize/2-1;
      dst[3] = 0;
      dst+=4;
   
      src+=4;
   
      for ( int y = 0; y < fieldysize/2; y++ )
        for ( int x = 0; x < fieldxsize/2; x++ ) 
           dst[y * fieldxsize/2 + x] = src[ y*fieldxsize/2 + x ];
   
      *pict = buf;
   } else 
      if ( (activeGraphicPictures.getMode( num ) & 0xff ) == 2 ) {
         void* buf2 = halfpict ( activeGraphicPictures.getPic( num ) );
         int sz = getpicsize2 ( buf2 );
         void* buf = asc_malloc ( sz );
         memcpy ( buf, buf2, sz );
         *pict = buf;
      } else
        *pict = NULL;
}


const char* getbi3path ( void )
{
	return CGameOptions::Instance()->bi3.dir.getName();
}


#include "objxlat.cpp"

/*

These tables are now in a separate file: objxlat.cpp , to be modifyable by non-programmers

const int terraintranslatenum = 17;
const int terraintranslate[terraintranslatenum][2] = {{ 574 , 526 } , { 575 , 1233 }, {577, 1244 }, 
                                                      { 581 ,1260 } , { 573 , 1226 }, {572, 1221 }, 
                                                      { 576 ,1238 } , { 578 , 1245 }, {579, 1249 }, 
                                                      { 580 ,1253 } , { 242 , 1135 }, {463,  449 },
                                                      { 464,  450 } , { 465,  451  }, {466,  452 },
                                                      { 237, 1110 } , { 233, 1094 }};

const int terraincombixlatnum = 2;
struct terraincombixlat {
           int bigraph;
           int terrainid;
           int terrainweather;
           int objectid;
       };
const terraincombixlat terraincombixlat[terraincombixlatnum] = {{ 222, 1011, 0, 1 }, 
                                                                { 223, 1012, 0, 1 }};

const int objecttranslatenum = 35;
const int objecttranslate[objecttranslatenum][5] = {{ 1264, 1470, 1500, -1, -1 }, 
                                                    { 1265, 1470, -1, 1560, -1 }, 
                                                    { 1266, 1470, -1, -1, 1530 }, 
                                                    { 1267, -1, 1500, 1560, -1 }, 
                                                    { 1268, -1, 1500, -1, 1530 }, 
                                                    { 1269, -1, -1, 1560, 1530 }, 
                                                    { 1270, 1470, 1500, 1560, -1 }, 
                                                    { 1271, 1470, 1500, -1, 1530 }, 
                                                    { 1272, 1470, -1, 1560, 1530 }, 
                                                    { 1273, -1, 1500, 1560, 1530 }, 
                                                    { 1274, 1470, 1500, 1560, 1530 },
                                                    { 90  , 1470,  -1 , -1, -1   }, 
                                                    { 91  , -1  , -1, 1560, -1   }, 
                                                    { 92  , -1, 1500, -1  , -1   }, 
                                                    { 93  , -1  , -1  , -1  , 1530 },
                                                    { 459, 1190, -1, -1, -1 },
                                                    { 460, 1191, -1, -1, -1 },
                                                    { 1329, 1296, -1, -1, -1 },
                                                    { 1330, 1300, -1, -1, -1 },
                                                    { 1331, 1304, -1, -1, -1 },
                                                    {  234, 1098, -1, -1, -1 },
                                                    {  235, 1101, -1, -1, -1 },
                                                    {  236, 1102, -1, -1, -1 },
                                                    { 1283, 1284, -1, -1, -1 },
                                                    { 1334, 1325, -1, -1, -1 },
                                                    { 1333, 1320, -1, -1, -1 },
                                                    {  238, 1113, -1, -1, -1 },
                                                    {  239, 1118, -1, -1, -1 },
                                                    {  240, 1125, -1, -1, -1 },
                                                    {  241, 1131, -1, -1, -1 },
                                                    {  345, 1152, -1, -1, -1 },
                                                    { 1332, 1310, -1, -1, -1 },
                                                    {  347,  340, -1, -1, -1 },
                                                    {  348,  342, -1, -1, -1 },
                                                    {  461, 1203, -1, -1, -1 }
                                                    };

*/

const int fuelfactor = 120;
const int materialfactor = 390;
const int energyfactor = 390;

int getterraintranslatenum ( void )
{
   return terraintranslatenum;
}

int getobjectcontainertranslatenum ( void )
{
   return objecttranslatenum;
}

const int* getterraintranslate ( int pos )
{
   return terraintranslate[pos];
}

const int* getobjectcontainertranslate ( int pos )
{
   return objecttranslate[pos];
}


#ifndef converter

#include "spfst.h"
#include "loaders.h"
#include "dialog.h"

// backwards due to byte order
/*
      int HeadID = 'NSSM';
      int ACTNID = 'NTCA';
      int SHOPID = 'POHS';
      int MAPID  = '\0PAM';
      int MISSID = 'SSIM';
*/
      char* HeadID = "MSSN";
      char* ACTNID = "ACTN";
      char* SHOPID = "SHOP";
      char* MAPID  = "MAP\0";
      char* MISSID = "MISS";


#pragma pack(1)

class tloadBImap {
     
       struct THeader {
         char ID[4];                            // { 'MSSN' }
         int MissPos;
         int MapPos ;
         int ShopPos;
         int ACTNPos;
         word DFNum ;
         int DFPos[32] ;                     // { Pos nach DFXX }
         int DFLength[32];                   // { +4B von DFXX }
       };

       struct TACTN {
         char ID[4];
         word XSize, YSize;
       };

       typedef word TStdProdRec[64];
       typedef char TAllianzen[6];


       struct TMISSPart {
           char ID[4];
           word Stuff1 [3];
           word NextMiss;
           char StartWeather;
           char WhoPlays;           // { jeder Mensch/Computer ein Bit }
           char PlayType;           // { jeder Computer ein Bit }
           char Recs2Win;           // { Wieviel Gewinnrecords aus map erf llt werden m ssen, damit man gewinnt (f r jeden player gleich) }
           char Stuff2[3];
           char Landscape;
           TAllianzen Allianz;      // { Wer mit wem }
           word Stuff3[65];
           TStdProdRec StdProd;
           word Stuff4[24];
       };
          
       struct TMAPHead {
              char ID[4];
       };
          
       struct TFileMap { 
              word What1;
              word Round, Move;
              char Player, What2;
              word Stuff2;
              word Zero1;
              word Nr1;
              word Nr2;
              word Nr3;
              word Zero2[11];
       };
          
       struct TSHOPHead {
              char ID[4];
              word Num;
       };
          
       typedef int TVehContent[8];
       typedef TVehContent *PVehContent;
          
          union TShopContent {
             word XY[4][4];
             word All[16];
             TVehContent Veh;
          };
       typedef TShopContent *PShopContent;
         
       typedef word TProduceRec[4];

       struct TFileShop {
         word ID;
         word Pos1;
         union  {
            struct {
               word ShopType;
               word Stuff06;  // { Zero }
               word Name;
               word Pos2;
               word E, M;
               char EP, MP ;
               TShopContent Content;
               word ShopType2;
               TProduceRec Produce;
               word Stuff60 ; // { Zero }
               word Prior;
               word Owner;
               word Stuff66[7];  // { Zero }
            } a;
            struct {
                char Zero2[6];
                word W0A;
                char Zero3[6];
                word W48;
                word W4A;
                char Zero4[2];
                word W4E;
            } b;
            char raw[76];
         };
       };

       dynamic_array<char*> names;
       int namenum;

     protected:
        int xoffset, yoffset;
        pwterraintype defaultterraintype;
     public:
       void LoadFromFile( char* path, char* AFileName, pwterraintype trrn );
       tloadBImap ( void ) { 
          xoffset = 0;
          yoffset = 0;
       };
       virtual ~tloadBImap() {};
     private:

       tn_file_buf_stream* MissFile;
       THeader Header;

       struct TTextItem {
           int a;
           int b;
           char name[1000];
       };

       dynamic_array<TTextItem> ShopNameList;
       void LoadTXTFile ( char* filename );
       int TPWMtextfile;

       void ReadMISSPart(void);
       void ReadACTNPart(void);
       void ReadSHOPPart(void);
       void ReadShopNames( FILE* fp );
       pvehicle getunit ( int typ, int col );
       pvehicletype getvehicletype ( int typ );
       char* GetStr ( int a, int b );
       int convcol ( int c );

       TMISSPart OrgMissRec;
       void GetTXTName ( const char* path, const char* filename, char* buf );

      protected:
        char* missing;
        virtual void preparemap ( int x, int y  ) = 0;
        virtual pfield getfield ( int x, int y );

};

class ImportBiMap : public tloadBImap {
         protected:
           virtual void preparemap ( int x, int y  );
};

class InsertBiMap : public tloadBImap {
         protected:
           virtual void preparemap ( int x, int y  );
           virtual pfield getfield ( int x, int y );
         public:
           InsertBiMap ( int x, int y ) {
             xoffset = x; 
             yoffset = y;
           };
};


pfield tloadBImap :: getfield ( int x, int y )
{
   return ::getfield ( x, y );
}

pfield InsertBiMap :: getfield ( int x, int y )
{
   return ::getfield ( xoffset + x, yoffset + y );
}


void  tloadBImap ::  ReadMISSPart(void)
{ 

  MissFile->seek ( Header.MissPos );

  MissFile->readdata2 ( OrgMissRec ); 
  if ( strncmp ( OrgMissRec.ID, MISSID, 4)  ) {
     strcat ( missing, "\nFatal error: No Battle Isle mission; invalid MissID\n"  );
     throw timporterror (); 
  }

/*
  int I; 
  StdProd = OrgMissRec.StdProd; 
  for (I = 0; I <= 63; I++) 
      StdProd[I] = StdProd[I] & 0x03; 

  PlayerType = OrgMissRec.PlayType & 0x3F; 
  Allianz = OrgMissRec.Allianz; 
  for (I = 0; I <= 5; I++) 
     Allianz[I] = Allianz[I] & 0x3F; 
  Recs2Win = OrgMissRec.Recs2Win; 
  StartWeather = OrgMissRec.StartWeather; 
  if ( StartWeather < 0  ||  StartWeather > 4 )
     StartWeather = 0; 
  NextMiss = OrgMissRec.NextMiss; 
  Landscape = OrgMissRec.Landscape; 
*/
} 

int tloadBImap :: convcol ( int c )
{
   int cl = log2 ( c );
   if ( cl == 6 )
      return 8;
   else
     if ( cl < 2 )
        cl = 1 - cl;
   return cl;
}

  
const int bi3unitnum = 74;
int translateunits[ bi3unitnum ][2] = { {1201,26}, {1270,26}, {1202,13}, {1200,6}, {1203,-1}, 
                                        {1204,16}, {1205,17}, {1206,4}, {1207,65}, {1208,-1},
                                        {1209,7},  {1210,66}, {1211,2}, {1212,3},  {1213,17}, 
                                        {1214,51}, {1215,17}, {1216,2}, {1217,10}, {1218,11},
                                        {1219,16}, {1220,61}, {1221,49},{1222,56}, {-1,-1 },
                                        {1223,25}, {1224,72}, {1225,57},{1226,58}, {1227,7},
                                        {1228,59}, {1241,9},  {1242,22},{1244,19}, {1243,29},
                                        {1245,55}, {1246,19}, {1271,21},{1247,52}, {1248,31}, 
                                        {1251,15}, {1252,15}, {1256,39},{1257,42}, {1258,47}, 
                                        {1259,14}, {1260,36}, {1261,34},{1262,47}, {1263,37},
                                        {1264,38}, {1265,5},  {1229,18},{-1, -1,}, {1249,-1}, 
                                        {1230,1},  {1231,66}, {1266, 5},{1232,64}, {1233,-1},
                                        {1234,4},  {1235,4},  {1267,35},{1250,29} };


pvehicletype  tloadBImap :: getvehicletype ( int tp )
{
   for ( int j = 0; j < vehicletypenum; j++ ) {
      pvehicletype tnk = getvehicletype_forpos ( j );
      if ( tnk )
         if ( tnk->bipicture > 0 ) 
            if ( tnk->bipicture == 1340 + tp * 2 ) 
                  return tnk;
   }


   if ( tp < bi3unitnum )
      for ( int i = 0; i < 2; i++ ) 
         if ( translateunits[tp][i] > 0 ) {
            pvehicletype tnk = getvehicletype_forid ( translateunits[tp][i] );
            if ( tnk )
               return tnk;
         }
   return NULL;
}

pvehicle tloadBImap :: getunit ( int tp, int col )
{
   pvehicletype vt = getvehicletype ( tp );
   if ( vt ) {
      pvehicle eht = new Vehicle ( vt, actmap, col );
      eht->fillMagically();
      return eht;
   }

   return NULL;
}



void ImportBiMap :: preparemap ( int x, int y  )
{
    generatemap ( defaultterraintype, x, y ); 
    actmap->setgameparameter( cgp_movefrominvalidfields, 1);
    actmap->setgameparameter( cgp_forbid_building_construction, 1);
}

void InsertBiMap :: preparemap ( int x, int y  )
{
   if ( yoffset & 1 ) {
      yoffset--;
      strcat ( missing, "y position for map insertion must be even; current position decreased\n");
   }
   int xp = xoffset;
   int yp = yoffset;
   if ( xp + x > actmap->xsize ) {
      int dx = xp + x - actmap->xsize;
      strcat ( missing, "map had to be resized in X direction\n");
      int r = resizemap ( 0, 0, 0, dx );
      if ( r ) {
         strcat ( missing, "Resizing failed !!\n");
         throw timporterror();
      }
   }
   if ( yp + y > actmap->ysize ) {
      int dy = yp + y - actmap->ysize;
      strcat ( missing, "map had to be resized in Y direction\n");
      int r = resizemap ( 0, dy, 0, 0 );
      if ( r ) {
         strcat ( missing, "Resizing failed !!\n");
         throw timporterror();
      }
   }

   for ( int b = 0; b < y; b++ )
      for ( int a = 0; a < x; a++ )
         getfield ( a, b ) -> deleteeverything();
}


void        tloadBImap ::   ReadACTNPart(void)
{ 
    struct {
       int X, Y;
    } Size;

    TACTN        ACTNHead; 
    Word         Line[64]; 
  
    MissFile->seek ( Header.ACTNPos );
    MissFile->readdata2( ACTNHead ); 
    if ( strncmp ( ACTNHead.ID ,ACTNID,4 )) {
       strcat ( missing, "\nFatal error: No Battle Isle mission; invalid ACTNID\n"  );
       throw timporterror (); 
    }

    Size.X = ACTNHead.XSize;
    Size.Y = ACTNHead.YSize;
    if ( Size.Y & 1 )
       Size.Y++;

    preparemap ( Size.X / 2, Size.Y * 2 );

    /*  Terrain  */ 
    int Y, X;

    int missnum = 0;
    dynamic_array<int> miss;


    for (Y = 0; Y < Size.Y ; Y++) { 
      MissFile->readdata( Line, Size.X * 2); 
      for (X = 0; X < Size.X ; X++) {
         for ( int tr = 0; tr < terraintranslatenum; tr++ )
            if ( Line[X] == terraintranslate[tr][0] )
               Line[X] = terraintranslate[tr][1];
         int found = 0;
         pfield fld = getfield ( X / 2, Y * 2 + (X & 1) );
         fld->tempw = Line[X];
         fld->temp3 = 0;

         for ( int i = 0; i < terraintypenum; i++ ) {
            pterraintype trrn = getterraintype_forpos ( i );
            if ( trrn )
               for ( int j = 0; j < cwettertypennum; j++ )
                  if ( trrn->weather[j] )
                     if ( trrn->weather[j]->bi_picture[0] == Line[X] ) {
                        fld->typ = trrn->weather[j];
                        fld->setparams();
                        found = 1;
                     }
         }
         if ( !found ) 
            for ( int j = 0; j < terraincombixlatnum; j++ )
               if ( Line[X] == terraincombixlat[j].bigraph ) {
                  pterraintype trrn = getterraintype_forid ( terraincombixlat[j].terrainid );
                  if ( trrn ) {
                     fld->typ = trrn->weather[terraincombixlat[j].terrainweather];
                     pobjecttype obj = NULL;
                     if ( terraincombixlat[j].objectid > 0 )
                        obj = getobjecttype_forid ( terraincombixlat[j].objectid );
                     if ( obj )
                        fld->addobject ( obj, -1, 1 );
                     fld->setparams();
                     found = 1;
                  }

               }

         if ( !found ) {
            int fnd = 0;
            for ( int k = 0; k < missnum; k++ )
               if ( miss[k] == Line[X] )
                  fnd = 1;

            if ( !fnd ) 
               miss[missnum++] = Line[X];
            
         }
      }
    } 
    if ( missnum ) {
       strcat ( missing, "The following terrain fields could not be found: " );
       for ( int k = 0; k < missnum; k++ ) {
          strcat ( missing, strrr ( miss[k] ));
          strcat ( missing, ", ");
       }
       strcat ( missing, "\n\n");
    }



    missnum = 0;

    //  Objekte  
    for (Y = 0; Y < Size.Y ; Y++) { 
      MissFile->readdata( Line, Size.X * 2 ); 
      
      for (X = 0; X < Size.X ; X++) {
         int found = 0;
         int newx = X / 2;
         int newy = Y * 2 + (X & 1);

         if ( Line[X] != 0xffff )
            getfield ( newx, newy )->tempw = Line[X];


         int xl = 0;
         int xlt[5];
         xlt[0] = Line[X];
         for ( int b = 0; b < objecttranslatenum; b++ )
             if ( objecttranslate[b][0] == Line[X] ) 
                for ( int c = 1; c < 5; c++ )
                   if ( objecttranslate[b][c] != -1 )
                      xlt[xl++] = objecttranslate[b][c];
         if ( xl == 0 ) {
            for ( int c = 0; c < terraintranslatenum; c++ )
                if ( terraintranslate[c][0] == Line[X] )
                   xlt[xl++] = terraintranslate[c][1];

            if ( xl == 0 )
               xl = 1;
         }

         for ( int m = 0; m < xl; m++ ) {
            int found_without_force = 0;
            for ( int pass = 0; pass < 2 && !found_without_force; pass++ ) 
               for ( int i = 0; i < objecttypenum; i++ ) {
                  pobjecttype obj = getobjecttype_forpos ( i );
                  if ( obj ) 
                     for ( int ww = 0; ww < cwettertypennum; ww++ )
                        if ( obj->weather & ( 1 << ww ))
                           for ( int j = 0; j < obj->pictnum; j++ )
                              if ( obj->picture[ww][j].bi3pic == xlt[m]  && !(found & 2)  && !(activeGraphicPictures.getMode(xlt[m]) & 256) ) {
                                 pfield fld = getfield ( newx, newy );
                                 if ( pass == 1 || obj->terrainaccess.accessible ( fld->bdt )) {
                                    fld -> addobject ( obj, 0, 1 );
                                    found |= 1;
                                    if ( pass == 0 )
                                      found_without_force = 1;
                                 }
                              }
               }
         }

         if ( !found  && Line[X] != 0xffff ) {
            getfield ( newx, newy )->temp3 = 1;

            int fnd = 0;
            for ( int k = 0; k < missnum; k++ )
               if ( miss[k] == Line[X] )
                  fnd = 1;

            if ( !fnd ) 
               if ( Line[X] < 44  || Line[X] > 88 )
                  miss[missnum++] = Line[X];
            
         }
      }
      
    } 
/*
    if ( missnum ) {
       strcat ( missing, "The following objects could not be found: " );
       for ( int k = 0; k < missnum; k++ ) {
          strcat ( missing, strrr ( miss[k] ));
          strcat ( missing, ", ");
       }
       strcat ( missing, "\n\n");
    }
*/

    for (Y = 0; Y < Size.Y ; Y++) { 
      MissFile->readdata ( Line, Size.X * 2 ); 
      for (X = 0; X < Size.X ; X++) {
         int tp = Line[X] & 255;
         int cl = convcol(Line[X] >> 8);
         pvehicle eht = getunit ( tp, cl );
         if ( eht ) {
            pfield fld = getfield ( X / 2, Y * 2 + (X & 1) );
            fld->vehicle = eht;
            fld->vehicle->xpos = xoffset + X / 2;
            fld->vehicle->ypos = yoffset + Y * 2 + (X & 1);
            stu_height ( fld->vehicle );
         }
      }
    } 
} 
 
struct blds {
  pbuildingtype bld;
  int pictnum;
  int terrainmatch;
} ;
   
void       tloadBImap :: ReadSHOPPart( void )
{ 
   TSHOPHead    SHOPHead; 
   TFileShop    FileShop; 
//   PVehContent  PVC; 
 
 int firstmissingbuilding = 1;
 
  MissFile->seek ( Header.ShopPos );
   MissFile->readdata2 ( SHOPHead ); 
   if ( strncmp ( SHOPHead.ID, SHOPID, 4 )) {
      strcat ( missing, "\nFatal error: No Battle Isle mission; invalid ShopID\n"  );
      throw timporterror (); 
   }

   int ShopNum = 0; 
   int VehContNum = 0; 
   int AINum = 0; 
   for ( int I = 0; I < SHOPHead.Num ; I++ ) { 
      MissFile->readdata2 ( FileShop ); 
      if ( FileShop.ID == 1) {   //  wenn kein ki punkt  
        if (FileShop.a.ShopType == 32) {   //  Vehicle  
           int IsVehCont = false; 
           for ( int J = 0; J < 8; J++ ) 
              if ( FileShop.a.Content.Veh[J] != 0xFFFF ) 
                 IsVehCont = true; 
           if ( IsVehCont ) {
              int Y = FileShop.Pos1 / 64;
              int X = FileShop.Pos1 % 64;
              pfield fld = getfield ( X / 2, Y * 2 + (X & 1) );
              int nm = 0;
              if ( fld->vehicle ) 
                 for ( int j = 0; j < 8; j++ ) 
                    if ( FileShop.a.Content.Veh[j] >= 0 ) {
                       pvehicle eht = getunit ( FileShop.a.Content.Veh[j], fld->vehicle->color/8 );
                       if ( eht ) {
                          eht->xpos = xoffset + X / 2;
                          eht->ypos = yoffset + Y * 2 + (X & 1);
                          fld->vehicle->loading[nm++] = eht;
                       }
                    }

           } 
        }
       
        else { 
           int Y = FileShop.Pos1 / 64;
           int X = FileShop.Pos1 % 64;
           int newx = X / 2;
           int newy = Y * 2 + (X & 1);
            
           pfield fld = getfield ( newx, newy );

           dynamic_array<blds> bldlist;
           int bldlistnum = 0;

           for ( int i = 0; i < buildingtypenum; i++ ) {
               pbuildingtype bld  = getbuildingtype_forpos ( i );
               if ( bld )
                  for ( int w = 0; w < cwettertypennum; w++ ) 
                     for ( int p = 0; p < maxbuildingpicnum; p++ ) 
                        if ( bld->w_picture[w][p][ bld->entry.x ] [ bld->entry.y ] )
                           if ( bld->bi_picture[w][p][ bld->entry.x ] [ bld->entry.y ] == fld->tempw ) {
                              bldlist[ bldlistnum ].bld = bld;
                              int cnt = 0;
                              int f = 0;
                              for ( int m = 0; m < 4; m++ )
                                 for ( int n = 0; n < 6; n++ )
                                    if ( bld->getpicture( m , n ) ) {
                                       int xpos;
                                       int ypos;
                                       bld->getfieldcoordinates ( newx, newy, m, n, &xpos, &ypos );
                                       pfield fld2 = getfield ( xpos, ypos );
                                       if ( bld->terrain_access->accessible ( fld2->bdt ) > 0 )
                                          f++;

                                       cnt++;
                                    }
                              bldlist[ bldlistnum ].pictnum = cnt;
                              bldlist[ bldlistnum ].terrainmatch = f;

                              bldlistnum++;

                           }
           }

           int found = 0;
           if ( bldlistnum ) {
              for ( int j = 0; j < bldlistnum-1;  )
                 if ( ( bldlist[ j ].terrainmatch < bldlist[ j+1 ].terrainmatch ) || 
                      ( bldlist[ j ].terrainmatch == bldlist[ j+1 ].terrainmatch && bldlist[ j ].pictnum < bldlist[ j+1 ].pictnum) ) {
                    blds tmp = bldlist[ j ];
                    bldlist[ j ] = bldlist[ j+1 ];
                    bldlist[ j+1 ] = tmp;
                    if ( j > 0 )
                       j--;
                 } else
                   j++;

              int actpos = 0;
              while ( !found && actpos < bldlistnum ) {
                  pbuildingtype bld = bldlist[actpos].bld;
                  for ( int w = 0; w < cwettertypennum; w++ ) 
                     for ( int p = 0; p < maxbuildingpicnum; p++ )  
                         if ( !found ) {
                            int match = 1;
                            for ( int m = 0; m < 4; m++ )
                               for ( int n = 0; n < 6; n++ )
                                  if ( bld->w_picture[w][p][ m ] [ n ] )
                                     if ( bld->getpicture( m , n ) ) {
                                        int x;
                                        int y;
                                        bld->getfieldcoordinates ( newx, newy, m, n, &x, &y );
                                        pfield fld2 = getfield ( xoffset + x, yoffset + y );
                                        if ( fld2->tempw != bld->bi_picture[w][p][ m ] [ n ] )
                                           match = 0;
                                     }
                            if ( match )
                               found = 1;
                         }
                  actpos++;
              }
              actpos--;

              if ( found ) {
                 putbuilding ( xoffset + newx, yoffset + newy, 0, bldlist[actpos].bld, bldlist[actpos].bld->construction_steps - 1, 1 );
                 if ( fld->building ) {
                    
                    for ( int m = 0; m < 4; m++ )
                       for ( int n = 0; n < 6; n++ )
                          if ( fld->building->getpicture( m , n ) ) {
                             fld->building->getField ( m, n )->temp3 = 0;
                          }

                       /*
                             if ( field->object )
                                for ( int o = 0; o < field->object->objnum; o++ )
                                   field->removeobject ( field->object->object[o]->typ );
                          }
                     */
                     found = 255;
                 } else
                     found = 254;
              } 
           } 


           if ( fld->building ) {
              if ( fld->building->name )
                 delete fld->building->name;

              if ( battleisleversion == 3 )
                 fld->building->name = GetStr( ShopNum + 2,16);   /*  bi3 macht das so  */ 
              /*
              else
                 ArrShop.NameStr = GetStr(FileShop.Name, 16);    /*  bi2 wahrscheinlich so  */ 
              int newcol = convcol ( FileShop.a.Owner );
              if ( newcol != fld->building->color/8 )
                 fld->building->convert ( newcol );

              int unitnum = 0;
              for ( int j = 0; j < 16; j++ ) {
                 pvehicle eht = getunit ( FileShop.a.Content.All[j], fld->building->color/8 );
                 if ( eht )
                    fld->building->loading[unitnum++] = eht;
              }

              int prodnum = 0;
              for ( int k= 0; k < 4; k++ ) {
                 pvehicletype vt = getvehicletype ( FileShop.a.Produce[k] );
                 if ( vt ) {
                    int fnd = 0;
                    for ( int l = 0; l < prodnum; l++ )
                       if ( fld->building->production[l] == vt )
                          fnd++;

                    if ( !fnd )
                       if ( fld->building->typ->vehicleloadable( vt ))
                          fld->building->production [ prodnum++ ] = vt;
                 }
              }
              for ( int l = 0; l < 64; l++ )
                 if ( OrgMissRec.StdProd[l] & 3 ) {
                    pvehicletype vt = getvehicletype ( l );
                    if ( vt ) {
                       int fnd = 0;
                       for ( int l = 0; l < prodnum; l++ )
                          if ( fld->building->production[l] == vt )
                             fnd++;
                       if ( !fnd )
                          if ( prodnum < 32 )
                             if ( fld->building->typ->vehicleloadable( vt ))
                                 fld->building->production [ prodnum++ ] = vt;
                    }
                 }
              fld->building->bi_resourceplus.energy = energyfactor * FileShop.a.EP;
              fld->building->bi_resourceplus.material = materialfactor * FileShop.a.MP;
              fld->building->bi_resourceplus.fuel = fuelfactor * FileShop.a.EP;

              fld->building->actstorage.energy = energyfactor * FileShop.a.E;
              fld->building->actstorage.material = materialfactor * FileShop.a.M;
              fld->building->actstorage.fuel = fuelfactor * FileShop.a.E;

           } else {
              if ( found == 254 ) {
                 char tmp[100];
                 int obj = fld->tempw;
                 sprintf( tmp, "The building at position %d / %d using pic #%d could not be set\n", newx, newy, obj );
                 strcat ( missing, tmp );
              } else {
                 if ( firstmissingbuilding ) {
                    strcat ( missing, "The buildings at the following coordinates could not be found:\n " );
                    firstmissingbuilding = 0;
                 }
                 char tmp[100];
                 int obj = fld->tempw;
                 sprintf( tmp, "%d / %d using pic #%d\n", newx, newy, obj );
                 strcat ( missing, tmp );
              }
           }
           ShopNum++;

        }
      } 
      
   } 
   if ( TPWMtextfile ) 
      strcat ( missing, "\nThe names of the buildings could not be read because the text file is TPWM encoded. Please decode it first if you want to keep the names of the shops !\n");
   

   int missnum = 0;
   dynamic_array<int> miss;

   for ( int y = 0; y < actmap->ysize; y++ )
      for ( int x = 0; x < actmap->xsize; x++ ) 
         if ( getfield(x,y)->temp3 ) {
            int m = getfield(x,y)->tempw;
            if ( m > 0 && m != 0xffff ) {
               int fnd = 0;
               for ( int k = 0; k < missnum; k++ )
                  if ( miss[k] == m )
                     fnd = 1;
   
               if ( !fnd ) 
                  miss[missnum++] = m;
               
            }
         }
      
     
   if ( missnum ) {
      strcat ( missing, "The following objects could not be found: " );
      for ( int k = 0; k < missnum; k++ ) {
         strcat ( missing, strrr ( miss[k] ));
         strcat ( missing, ", ");
      }
      strcat ( missing, "\n\n");
   }


} 


char* tloadBImap :: GetStr( int a, int b )
{
   if ( TPWMtextfile )
      return NULL;
   else
      for ( int i = 0; i <= ShopNameList.getlength(); i++ )
         if ( ShopNameList[i].a == a && ShopNameList[i].b == b )
            return strdup ( ShopNameList[i].name );

   return NULL;
}

/* TPTC: TOCONV.PAS(303): Warning: Nested function, tok=SplitName 
    
Boolean      SplitName(char *       S,
                       char *       Name,
                       Word *       R1,
                       Word *       R2)
      { 
        Integer      Code; 
      
        Result = false; 
        strcpy(S,Trim(S)); 
        if (strcmp(S,"") != 0) { 
          if (S[0] != '#') return;
          val(copy(S,2,4),R1,&Code); 
          if (Code != 0) return;
          if (S[5] != ':') return;
          val(copy(S,7,4),R2,&Code); 
          if (Code != 0) return;
          if (S[10] != '{') return;
          if (S[strlen(S)-1] != '}') return;
          strcpy(S,copy(S,12,strlen(S) - 12)); 
  //           Delete(S, 1, 11);
  //        Delete(S, Length(S), 1); 
          strcpy(Name,S); 
          sbld(Result,"%b",true); 
        } 
      } 

*/    


void    tloadBImap :: ReadShopNames( FILE* fp )
{ 
   char buf[ 1000];
   int c;      
   do {
      do {
         c = fgetc ( fp );
      } while ( c != '#' && c!= EOF );
      if ( c == '#' ) {
         int i;
         for ( i = 0; i < 4; i++ ) {
            c = fgetc ( fp );
            if ( c < '0' || c > '9' )
               return;

            buf[i] = c;
         }
         buf[4] = 0;
         int a1 = atoi ( buf );
         if ( fgetc ( fp ) != ':' )
            return;

         for ( i = 0; i < 4; i++ ) {
            c = fgetc ( fp );
            if ( c < '0' || c > '9' )
               return;

            buf[i] = c;
         }
         buf[4] = 0;
         int a2 = atoi ( buf );

         if ( fgetc ( fp ) != '{' )
            return;

         i = 0;
         do {
            c = fgetc ( fp );
            buf[i++] = c;
         } while ( c != '}' && c != EOF ); /* enddo */

         if ( c == '}' ) {
            buf[i-1] = 0;
            int pos = ShopNameList.getlength()+1;
            ShopNameList[pos].a = a1;
            ShopNameList[pos].b = a2;
            strcpy ( ShopNameList[pos].name , buf );
         }
      }
   } while ( c != EOF );
} 


void tloadBImap :: GetTXTName ( const char* path, const char* filename, char* buf )
{
    strcpy ( buf, path );
    strcat ( buf, "ger" );
    strcat ( buf, pathdelimitterstring );
    strcat ( buf, filename );
    strcpy ( &buf[ strlen ( buf ) - 3], "txt" );
    if ( exist ( buf )) 
       return;

    strcpy ( buf, path );
    strcat ( buf, "eng" );
    strcat ( buf, pathdelimitterstring );
    strcat ( buf, filename );
    strcpy ( &buf[ strlen ( buf ) - 3], "txt" );
    if ( exist ( buf )) 
       return;

    buf[0] = 0;
    return;
}
  
void tloadBImap :: LoadTXTFile ( char* filename )
{
   FILE* fp = fopen ( filename, "r" );
   if ( !fp )
      return;


   char buf[1000];
   fread ( buf, 1, 4, fp );
   if ( strncmp ( buf, "TPWM",4) == 0  ) {
      TPWMtextfile = 1;
      fclose ( fp );
      return;
   }

   TPWMtextfile = 0;
   fseek ( fp, 0, SEEK_SET );

   ReadShopNames( fp );

   fclose ( fp );

   /*
   LevelDescription:= GetStr(0, 0);
      LevelName:= GetStr(1, 24);

      P:= Pos('|', LevelDescription);
      if (Length(LevelDescription)> 0) and (P<> 0) then begin
        SavePlayerNames:= true;
        S:= Copy(LevelDescription, P, Length(LevelDescription)-P+1);
        Delete(LevelDescription, P, Length(LevelDescription)-P+1);
        Delete(S, 1, 1);
        for I:= 0 to 6 do begin
          P:= Pos('|', S);
          if P= 0 then P:= Length(S)+ 1;
          FPlayers[I].Name:= Copy(S, 1, P-1);
          Delete(S, 1, P);
          P:= Pos('|', S);
          if P= 0 then P:= Length(S)+ 1;
          try
            FPlayers[I].Color:= StrToInt(Copy(S, 1, P-1));
          except
            FPlayers[I].Color:= 0;
          end;
          Delete(S, 1, P);
        end;
      end else
        SavePlayerNames:= false;
   */
}

void tloadBImap :: LoadFromFile( char* path, char* AFileName, pwterraintype trrn )
{
   defaultterraintype = trrn;
   char temp[4000];
   missing = temp;
   missing[0] = 0;
   namenum = 0;
//   PShopNameItem PSNI;

    char TXTName[1000];
    try {
       GetTXTName( path, AFileName, TXTName );
   
       LoadTXTFile(TXTName);
   
       char completefilename[1000];
       strcpy ( completefilename, path );
       strcat ( completefilename, "mis" );
       strcat ( completefilename, pathdelimitterstring );
       strcat ( completefilename, AFileName );
       tn_file_buf_stream stream ( completefilename, 1 );
       MissFile = &stream;
       MissFile->readdata2( Header );
       if ( strncmp ( Header.ID, HeadID, 4 )) {
          strcat ( missing, "\nFatal error: No Battle Isle mission; invalid HeadID\n"  );
          throw timporterror (); 
       }
       ReadMISSPart();
       ReadACTNPart();
       ReadSHOPPart();
       /*
       ReadMAPPart();
       LoadDFs();
       UnPackDF(AktDFNum);
       */
   
       if ( actmap->title )
          delete actmap->title;
       actmap->title = GetStr ( 1, 24 );
       if ( !actmap->title )
          actmap->title = strdup ( "imported BI map");

    } /* endtry */
    catch ( tfileerror err ) {
       strcat ( missing, "\nA fatal error occured while accessing the file " );
       strcat ( missing, err.filename );
       strcat ( missing, "\n" );
    } /* endcatch */
    catch ( ASCexception ) {
       strcat ( missing, "\nA fatal error occured" );
    } /* endcatch */

    calculateallobjects();
    if ( missing[0] ) {
      tviewanytext vat;
      vat.init ( "warning", missing );
      vat.run();
      vat.done();
    }

}


void importbattleislemap ( char* path, char* filename, pwterraintype trrn  )
{
    activateGraphicSet ( 1 );
    ImportBiMap lbim;  
    lbim.LoadFromFile ( path, filename, trrn );
    actmap->_resourcemode = 1;
    actmap->cleartemps ( 7 );
}


void insertbattleislemap ( int x, int y, char* path, char* filename  )
{
    activateGraphicSet ( 1 );
    InsertBiMap lbim ( x, y );  
    lbim.LoadFromFile ( path, filename, NULL );
    actmap->_resourcemode = 1;
    actmap->cleartemps ( 7 );
}


#endif

int activateGraphicSet ( int id  )
{
  return    activeGraphicPictures.setActive ( id );
}

#pragma pack()
