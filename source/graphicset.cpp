/***************************************************************************
                          graphicset.cpp  -  description
                             -------------------
    begin                : Sun Jan 28 2001
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/*! \file graphicset.cpp
    \brief A system that provides a set of images for vehicles, buildings, etc.
*/


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "basestrm.h"
#include "misc.h"
#include "graphicset.h"
#include "basegfx.h"
#include "errors.h"
#include "typen.h"
#include "sgstream.h"

#include "loadpcx.h"

int bi3graphnum = 0;

int keeporiginalpalette = 0;



ActiveGraphicPictures activeGraphicPictures;

const ActiveGraphicPictures* getActiveGraphicSet()
{
  return &activeGraphicPictures;
}


void ActiveGraphicPictures :: alloc ( void )
{
    for ( int i = 0; i < maxnum; i++ )
       bi3graphics[i] = asc_malloc ( absoluteMaxPicSize );
}

int ActiveGraphicPictures :: picAvail ( int num ) const
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

int ActiveGraphicPictures :: getMode ( int num ) const
{
   if ( picAvail ( num ))
      return bi3graphmode[num];
   else
      return 0;
}



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

      if ( !found && id== 0 )
         fatalError ( "fatal error: no default graphic set ( id=0 ) found !" );

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


int getGraphicSetIdFromFilename ( const char* filename )
{
    tnfilestream stream ( filename, tnstream::reading );

    int magic = stream.readInt();
    if ( magic == -1 ) {
       return stream.readInt();
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

   loadpalette();

   int highestPicNum = 0;
   bi3graphnum = maxint;

   int absoluteMaxPicSize = 0;

   void* emptyfield;
   {
      int o;
      tnfilestream s ( "emptyfld.raw", tnstream::reading );
      s.readrlepict ( &emptyfield, false, &o );
   }
   int emptyfieldsize = getpicsize2 ( emptyfield );

   #ifdef genimg
   void* mask;
   {
      int i ;
      tnfilestream s ( "largehex.raw", tnstream::reading );
      s.readrlepict ( &mask, false, & i );
   }
   #endif


   tfindfile ff ( "*.gfx" );
   string filename = ff.getnextname();
   while ( !filename.empty() ) {

      tnfilestream s ( filename.c_str(), tnstream::reading );

      int magic = s.readInt();
      if ( magic == -1 ) {

         GraphicSet* gs = new GraphicSet;

         gs->id = s.readInt();
         gs->picnum = s.readInt();
         gs->maxPicSize = s.readInt();

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
#ifdef genimg
               if ( gs->id == 1 ) {
                  if ( picmode[i] < 256 ) {
                     tvirtualdisplay vdp ( 100, 100, 255 );
                     /*
                     putspriteimage ( 8, 8, p );
                     putspriteimage ( 8, 12, p );
                     putspriteimage ( 12, 8, p );
                     putspriteimage ( 12, 12, p );
                     */
                     putspriteimage ( 10, 10, p );

                     putmask ( 10, 10, mask, 0 );

                     ASCString fn = "15/";
                     fn+=strrr(i);
                     fn+=".pcx";
                     writepcx ( fn.c_str(), 10, 10, 10+fieldsizex-1, 10+fieldsizey-1, pal );
                  }
               }
#endif
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

int activateGraphicSet ( int id  )
{
  return    activeGraphicPictures.setActive ( id );
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
