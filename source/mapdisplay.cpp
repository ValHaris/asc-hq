/*! \file mapdisplay.cpp    
    \brief everything for displaying the map on the screen
*/


/***************************************************************************
                          mapdisplay.cpp  -  description
                             -------------------
    begin                : Wed Jan 24 2001
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "mapdisplay.h"
#include "vehicletype.h"
#include "buildingtype.h"
#include "spfst.h"
#include "typen.h"
#include "loaders.h"
#include "gameoptions.h"
#include "dialog.h"
#include "stack.h"
#include "loadbi3.h"
#include "mapalgorithms.h"
#include "loadpcx.h"
#include "graphicset.h"

#ifdef sgmain
 #include "controls.h"
 #include "gui.h"
 #include "dashboard.h"
#endif


// #define showtempnumber

#define show2threatvalue

// #define showmovetemp
int showresources = 0;


   #define stepsizex 2
   #define stepsizey 4
   #define fielddx 4    /*  verschiebung der ro ecke des bodentypes gegen?ber der vehicle etc  */
   #define fielddy 4

   int lockdisplaymap = 0;

   MapDisplay        defaultMapDisplay;
   tpaintmapborder* mapborderpainter = NULL;


int vfbscanlinelength;


#ifdef _NOASM_
 int vfbstartdif;
 int scrstartdif;
 int scrstartpage;
#else
 extern "C" int vfbstartdif;
 extern "C" int scrstartdif;
 extern "C" int scrstartpage;
#endif



#define cursordownshift 0

#ifdef HEXAGON
 #define streettopshift 0
 #define streetleftshift 0
 #define cursorrightshift -8
 #define unitdownshift 0
#ifdef FREEMAPZOOM
 #define unitrightshift 0
#else
 #define unitrightshift -1
#endif

#else
 #define streettopshift 5
 #define streetleftshift 5
 #define cursorrightshift -1
 #define unitdownshift 4
 #define unitrightshift 5

 #define viereckdownshift ( unitdownshift + 11 )
 #define viereckrightshift ( unitrightshift - 10  )
#endif


#define buildingdownshift  0
#define buildingrightshift 0

#define mapborderwidth 4

// #define scrleftspace 21
int scrleftspace = 21;

#define scrxsize 417
#define scrysize 417

#define vfbbottomspace 25

#ifdef HEXAGON
 #define scrtopspace 22
 #define vfbfieldxdif 19
 #define vfbfirstpointxdif ( vfbfieldxdif - 3 )
 #define vfbfirstpointydif 0
 #define vfbleftspace 80
 #define vfbtopspace 80
#else
 #define scrtopspace 19
 #define vfbfieldxdif 19
 #define vfbfirstpointxdif ( vfbfieldxdif - 4 )
 #define vfbfirstpointydif 5
 #define vfbleftspace 40
 #define vfbtopspace 40
#endif

#define scrfirstpointxdif 9
#define scrfirstpointydif 0


#ifdef _NOASM_


void copyvfb2displaymemory_zoom ( void* parmbuf, int x1, int y1, int x2, int y2 )
{
   int tempdirectscreenaccess = agmp->directscreenaccess;
   if ( hgmp->directscreenaccess != agmp->directscreenaccess )
      agmp->directscreenaccess = hgmp->directscreenaccess;

   int* parmi = (int*) parmbuf;

   char* esi = (char*) parmi[0];
   char* edi = (char*) parmi[1];

   int edx = parmi[3];
   int ecx;
   int* ebp;
   do {
      ebp = (int*) parmi[4];
      ecx =  parmi[2];
      do {

         esi += *ebp;

         *(edi++) = *(esi++);
         ebp++;

         ecx--;

      } while ( ecx ); /* enddo */

      esi += parmi[5];
      edi += parmi[6];
      ebp = (int*) parmi[7];

      edx--;
      esi += ebp[edx];

   } while ( edx ); /* enddo */

   copySurface2screen( x1, y1, x2, y2 );
   agmp->directscreenaccess = tempdirectscreenaccess;
}

void copyvfb2displaymemory_zoom ( void* parmbuf )
{
   copyvfb2displaymemory_zoom ( parmbuf, idisplaymap.invmousewindow.x1, idisplaymap.invmousewindow.y1, idisplaymap.invmousewindow.x2, idisplaymap.invmousewindow.y2 );
}

#endif



void         initMapDisplay( )
{
   int x = -1;
   int y = -1;
   int game_x;
   int maped_x;
   if ( x == -1) {
      game_x = ( agmp->resolutionx - 225 ) / fielddistx;
      maped_x = ( agmp->resolutionx - 290 ) / fielddistx;
  #ifdef sgmain
      x = game_x;
    #ifndef FREEMAPZOOM
      int xw = ( x - 1) * fielddistx + fieldsizex + 2 * mapborderwidth + fielddisthalfx;
      int sw = (agmp->resolutionx - 640) + 433 - 16;
      scrleftspace += 2 + ( sw - xw ) / 2;
    #endif

  #else
      x = maped_x;
  #endif

   }



   if ( y == -1)
      y = ( agmp->resolutiony - 75 ) / fielddisty;

   if ( y & 1 )
      y--;

   int          i;

   if ( actmap ) {
      actmap->actplayer = 0;
      actmap->xsize = 0;
      actmap->ysize = 0;
      actmap->field = NULL;
   }

   tempsvisible = true;
   cursor.color = 0;
   // checkfieldmousestatus = 0;

  #ifdef FREEMAPZOOM
   #ifdef sgmain
    idisplaymap.init( scrleftspace, 21, agmp->resolutionx - 215, agmp->resolutiony - 55 );
   #else
    idisplaymap.init( scrleftspace, 21, agmp->resolutionx - 295, agmp->resolutiony - 40 );
   #endif
  #else
   idisplaymap.init( x, y  );
   idisplaymap.setxsizes ( game_x, maped_x );
  #endif
}


int getmapposx ( void )  // return the screencoordinates of the upper left position of the displayed map
{
   return scrleftspace + cursorrightshift;
}

int getmapposy ( void )
{
   return scrtopspace  + cursordownshift;
}


int   getfieldundermouse ( int* xf, int* yf )
{
   for ( int i=0;i<idisplaymap.getscreenxsize();i++ ) {
      for ( int j=0;j<idisplaymap.getscreenysize() ;j++ ) {
         int k;

         int xp = idisplaymap.getfieldposx( i, j );
         int yp = idisplaymap.getfieldposy( i, j );

         if ( mouseparams.x >= xp && mouseparams.x < xp+idisplaymap.getfieldsizex() && mouseparams.y >= yp && mouseparams.y < yp + idisplaymap.getfieldsizey() ) {
            k = 0;

           #ifdef HEXAGON

            // putspriteimage ( xp, yp, icons.fieldshape );

            int xd = mouseparams.x - xp;
            int yd = mouseparams.y - yp;


           #ifdef FREEMAPZOOM
            static int actpictwidth = -1;
            static void* picture = NULL;

            int actwidth = idisplaymap.getfieldsizex();
            if ( actpictwidth != actwidth ) {
               if ( picture )
                  delete[] picture ;

               TrueColorImage* zimg = zoomimage ( icons.fieldshape, idisplaymap.getfieldsizex(), idisplaymap.getfieldsizey(), pal, 0 );
               picture = convertimage ( zimg, pal ) ;
               delete zimg;

               int h;
               getpicsize ( picture, actpictwidth, h );
            }

           #else
            void* picture = icons.fieldshape;
           #endif

            int pix = getpixelfromimage ( picture, xd, yd );
            if ( pix >= 0 && pix < 255 )
               k++;


           /*
            if ( pw[0] >= xd  && pw[1] >= yd )
               for ( int xxx = 0; xxx <= pw[0]; xxx++ )
                  for ( int yyy = 0; yyy <= pw[1]; yyy++ )
                     if ( pc[ yyy * ( pw[0] + 1) + xxx] != 255 )
                        putpixel ( xp + xxx, yp + yyy, 255 );
           */

           #else
            int y;
            for ( y=0; y<10 ;y++ ) {
               if (mouseparams.x >  xp + 10 - y   &&   mouseparams.x <= xp + 20 + y   &&   mouseparams.y == yp + y)
                  k++;
            } /* endfor */

            for (y=10;y<20 ;y++ ) {
               if (mouseparams.x >= xp  &&   mouseparams.x <= xp + 30   &&   mouseparams.y == yp + y)
                  k++;
            } /* endfor */

            for (y=10; y>0 ;y-- ) {
               if (mouseparams.x >  xp + 10 - y   &&   mouseparams.x <= xp + 20 + y   &&   mouseparams.y == yp + 30 - y)
                  k++;
            } /* endfor */

           #endif

            if ( k ) {
               *xf = i;
               *yf = j;
               return 1;
            }
         } /* endif */


      } /* endfor */
   } /* endfor */
   #ifdef FREEMAPZOOM
   if ( mouseinrect ( &idisplaymap.invmousewindow  )) {
      int stepx =   fielddistx * zoomlevel.getzoomlevel() / 100 ;
      int stepy = 2*fielddisty * zoomlevel.getzoomlevel() / 100 ;
      int move = 0;
      if ( mouseparams.x > idisplaymap.invmousewindow.x2 - stepx )
         if (actmap->xpos + idisplaymap.getscreenxsize() < actmap->xsize)
            move += 1;

      if ( mouseparams.y > idisplaymap.invmousewindow.y2 - stepy )
         if (actmap->ypos + idisplaymap.getscreenysize() < actmap->ysize)
            move += 2;

      if ( move ) {

         int bb = cursor.an;
         if (bb)
            cursor.hide();

         if ( move & 1 )
            actmap->xpos++;
         else
            stepx = 0;

         if ( move & 2 )
            actmap->ypos+=2;
         else
            stepy = 0;

         setmouseposition ( mouseparams.x - stepx, mouseparams.y - stepy );

         displaymap();

         if (bb)
            cursor.show();

         return getfieldundermouse ( xf, yf );
      }
   }
   #endif

   return 0;
}


tdisplaymap idisplaymap;

#ifdef FREEMAPZOOM

ZoomLevel zoomlevel;

ZoomLevel :: ZoomLevel ( void )
{
   zoom = 75;
   queried = 0;
}

int ZoomLevel :: getmaxzoom ( void )
{
   return 100;
}

int ZoomLevel :: getminzoom ( void )
{
   return 33;
}

int ZoomLevel :: getzoomlevel ( void )
{
   if ( !queried ) {
      int mz;
     #ifdef sgmain
      mz = CGameOptions::Instance()->mapzoom;
     #else
      mz = CGameOptions::Instance()->mapzoomeditor;
     #endif
      if ( mz >= getminzoom()  &&
           mz <= getmaxzoom() )
          zoom = mz;
      queried = 1;
   }
   return zoom;
}

void ZoomLevel :: setzoomlevel ( int newzoom )
{
   zoom = newzoom;
   idisplaymap.setnewsize ( zoom );
  #ifdef sgmain
   CGameOptions::Instance()->mapzoom = newzoom;
  #else
   CGameOptions::Instance()->mapzoomeditor = newzoom;
  #endif
   CGameOptions::Instance()->setChanged();
}

#endif


void tgeneraldisplaymapbase :: setmouseinvisible ( void )
{
   setinvisiblemouserectanglestk ( invmousewindow );
}

void tgeneraldisplaymapbase :: restoremouse ( void )
{
   getinvisiblemouserectanglestk (  );
}

tgeneraldisplaymapbase :: tgeneraldisplaymapbase ( void )
{
   invmousewindow.x1 = 0;
   invmousewindow.y1 = 0;
   invmousewindow.x2 = hgmp->resolutionx;
   invmousewindow.y2 = hgmp->resolutiony;
}






int tgeneraldisplaymap :: getfieldsizex ( void )
{
   return fieldsizex * zoom / 100;
}

int tgeneraldisplaymap :: getfieldsizey ( void )
{
   return fieldsizey * zoom / 100;
}


int tgeneraldisplaymap :: getfielddistx ( void )
{
   return fielddistx * zoom / 100;
}

int tgeneraldisplaymap :: getfielddisty ( void )
{
   return fielddisty * zoom / 100;
}



void tgeneraldisplaymap :: init ( int xs, int ys )
{
   if ( actmap )
      playerview = actmap->playerView;

   dispmapdata.numberoffieldsx = xs;
   dispmapdata.numberoffieldsy = ys;
}


void tgeneraldisplaymap :: _init ( int xs, int ys )
{
   if ( actmap )
      playerview = actmap->playerView;

   window.xsize = xs;
   window.ysize = ys;

   setnewsize ( zoomlevel.getzoomlevel() );
}


void tgeneraldisplaymap :: setnewsize ( int _zoom )
{
   zoom = _zoom;
   dispmapdata.numberoffieldsx = 1 + (window.xsize - (fieldsizex + fielddisthalfx) * zoom / 100 ) * 100 / zoom / fielddistx;
   dispmapdata.numberoffieldsy = 2 + 2*((window.ysize - (fieldsizey + fielddisty) * zoom / 100 ) * 100 / zoom / (fielddisty*2));

   dispmapdata.disp.x1 = -1;
   dispmapdata.disp.y1 = -2;
   dispmapdata.disp.x2 = dispmapdata.numberoffieldsx + 2;
   dispmapdata.disp.y2 = dispmapdata.numberoffieldsy + 4;

   if ( dispmapdata.numberoffieldsy & 1 )
      dispmapdata.numberoffieldsy--;
}


int tdisplaymap :: getfieldposx ( int x, int y )
{
     if ( x < 0 )
        x = 0;

     if ( y & 1 )   /*  gerade reihennummern  */
        return windowx1 + (fielddisthalfx + x * fielddistx ) * zoom / 100;
     else
        return windowx1 + ( x * fielddistx ) * zoom / 100;
}


int tdisplaymap :: getfieldposy ( int x, int y )
{
     if ( y < 0 )
        y = 0;
     return windowy1  + (y * fielddisty) * zoom / 100;
}


tdisplaymap :: tdisplaymap ( void )
{
   copybufsteps = NULL;
}

void tdisplaymap :: init ( int x1, int y1, int x2, int y2 )
{
   invmousewindow.x1 = x1;
   invmousewindow.y1 = y1;
   invmousewindow.x2 = x2;
   invmousewindow.y2 = y2;

   rgmp = *agmp;
   rgmp.directscreenaccess = 1;

   tgeneraldisplaymap :: _init ( x2-x1, y2-y1 );


   dispmapdata.vfbwidth = window.xsize * 100/zoomlevel.getminzoom() + 5 * fielddistx;
   vfbscanlinelength = dispmapdata.vfbwidth;

   dispmapdata.vfbheight = window.ysize * 100/zoomlevel.getminzoom() + 10 * fielddisty;

   windowx1 = x1;
   windowy1 = y1;
   calcdisplaycache();

   if ( mapborderpainter )
      mapborderpainter->setrectangleborderpos ( x1-1, y1-1, x2+1, y2+1 );

   vfb.address = new char [ dispmapdata.vfbwidth * dispmapdata.vfbheight ];

   memset( vfb.address , 0, dispmapdata.vfbwidth * dispmapdata.vfbheight  );
   memset ( &displaymovingunit, 0, sizeof ( displaymovingunit ));

   vfbstartdif = vfbtopspace * vfbscanlinelength + vfbleftspace;

   vfb.parameters.resolutionx = dispmapdata.vfbwidth;
   vfb.parameters.resolutiony = dispmapdata.vfbheight;
   vfb.parameters.windowstatus = 100;
   vfb.parameters.scanlinelength= dispmapdata.vfbwidth;
   vfb.parameters.scanlinenumber = dispmapdata.vfbheight;
   vfb.parameters.bytesperscanline = dispmapdata.vfbwidth;
   vfb.parameters.byteperpix = 1;
   vfb.parameters.linearaddress = (int) vfb.address;
   vfb.parameters.directscreenaccess = 1;

}



void  tdisplaymap :: deletevehicle ( void )
{
   displaymovingunit.eht = NULL;
}


void tgeneraldisplaymap :: putdirecpict ( int x1, int y1, const void* ptr )
{
  int i, num;

   char* c = (char*) ptr;

   if ( agmp->windowstatus == 100 ) {
      char* buf = (char*) (agmp->scanlinelength * y1 + x1 + agmp->linearaddress) + 19;

      for (i=1; i<= 19 ;i++ ) {
        for (num = 0; num < i * 2; num ++ ) {
           *buf = *c;
           c++;
           buf++;
        }

//        buf += vfbscanlinelength - 2 * i - 1;
        buf += agmp->scanlinelength - 2 * i - 1;
      } /* endfor */

      for (i=20; i > 0 ;i-- ) {
        for (num = 0; num < i * 2; num ++ ) {
           *buf = *c;
           c++;
           buf++;
        }

//        buf += vfbscanlinelength - 2 * i + 1;
        buf += agmp->scanlinelength - 2 * i + 1;
      } /* endfor */

   }

}



int  tgeneraldisplaymap :: getscreenxsize( int target )
{
   if ( actmap && dispmapdata.numberoffieldsx >= actmap->xsize && actmap->xsize > 0 )
      return actmap->xsize ;
   else
      return dispmapdata.numberoffieldsx;
}

int  tgeneraldisplaymap :: getscreenysize( int target )
{
      if ( actmap && dispmapdata.numberoffieldsy >= actmap->ysize && actmap->ysize > 0 )
         return actmap->ysize;
      else
         return dispmapdata.numberoffieldsy;
}


void tgeneraldisplaymap :: pnt_terrain ( void )
{
    if ( playerview < 0 )
       displaymessage("tgeneraldisplaymap :: pnt_terrain ; playerview < 0", 2 );

   *agmp = vfb.parameters;

    for (int y=dispmapdata.disp.y1; y < dispmapdata.disp.y2; y++ ) {
       for ( int x=dispmapdata.disp.x1; x < dispmapdata.disp.x2; x++ ) {
          pfield fld = getfield ( actmap->xpos + x, actmap->ypos + y );
          if ( fld ) {
             int b = fieldVisibility ( fld, playerview );

             if ( b != visible_not ) {
                int yp;
                int r;
                if (y & 1 )   /*  ungerade reihennummern  */
                   r = vfbleftspace + fielddisthalfx + x * fielddistx;
                else
                   r = vfbleftspace + x * fielddistx;

                yp = vfbtopspace + y * fielddisty;
                putspriteimage ( r, yp, fld->typ->picture[0] );
             }
          } else {
             int yp;
             int r;
             if (y & 1 )   /*  ungerade reihennummern  */
                r = vfbleftspace + fielddisthalfx + x * fielddistx;
             else
                r = vfbleftspace + x * fielddistx;

             yp = vfbtopspace + y * fielddisty;
             putspriteimage ( r, yp, icons.mapbackground );
          }

       } /* endfor */
    } /* endfor */
}

void tdisplaymap :: setnewsize ( int _zoom )
{
   tgeneraldisplaymap :: setnewsize ( _zoom );
   calcdisplaycache();
}


void tdisplaymap :: calcdisplaycache( void )
{
   int sz;
   if ( window.xsize > window.ysize )
      sz = window.xsize ;
   else
      sz = window.ysize ;

   if ( !copybufsteps ) {
      copybufsteps = new int [ sz ];
      copybufstepwidth = new int [ sz ];
   }

   int last = 0;
   for ( int xp = 0; xp < sz; xp++ ) {
       int pos = xp * 100 / zoom ;
       int n = pos - last - 1;
       if ( n < 0 )
          n = 0;
       copybufsteps[ xp ] = n;
       copybufstepwidth[ xp ] = n * dispmapdata.vfbwidth;
       last = pos;
       if ( xp < window.xsize )
          vfbwidthused = last;
   }
}

void tdisplaymap :: cp_buf ( void )
{

    if ( hgmp->windowstatus != 100 ) {
      #ifndef _NOASM_
       int linaddress = windowx1 + windowy1 * hgmp->bytesperscanline;
       int page = hgmp->actsetpage;

       char* edi = (char*) (hgmp->linearaddress + ( linaddress & 0xffff) );
       char* esi = (char*) (agmp->linearaddress + vfbstartdif );

       int   linlinestart = linaddress;
       char* esistart = esi;

       for ( int yp = 0; yp < window.ysize; yp++ ) {
          for ( int xp = 0; xp < window.xsize; xp++ ) {
             if ( linaddress >> 16 != page ) {
                page = linaddress >> 16;
                setvirtualpagepos ( page );
                edi = (char*) (hgmp->linearaddress + ( linaddress & 0xffff) );
             }
             esi += copybufsteps[ xp ] + 1;
             *edi = *esi;
             edi++;
             linaddress++;
          }

          linaddress = linlinestart + hgmp->bytesperscanline;
          linlinestart = linaddress;

          edi = (char*) (hgmp->linearaddress + ( linaddress & 0xffff) );

          esi = esistart + agmp->bytesperscanline * ( yp * 100 / zoom);
       }
      #endif
    } else {


       struct {
          int src;
          int dst;
          int x;
          int y;
          int* steps;
          int srcdif;
          int dstdif;
          int* vfbsteps;
       } parm;

       parm.dst = hgmp->linearaddress + windowx1 + windowy1 * hgmp->bytesperscanline ;
       parm.src = agmp->linearaddress + vfbstartdif;
       parm.x = window.xsize;
       parm.y = window.ysize;
       parm.steps = copybufsteps;
       parm.srcdif = agmp->bytesperscanline - vfbwidthused - 1;
       parm.dstdif = hgmp->bytesperscanline - window.xsize;
       parm.vfbsteps = copybufstepwidth;

       copyvfb2displaymemory_zoom ( &parm.src );

    }
}


void tdisplaymap :: cp_buf ( int x1, int y1, int x2, int y2 )
{

       struct {
          int src;
          int dst;
          int x;
          int y;
          int* steps;
          int srcdif;
          int dstdif;
          int* vfbsteps;
       } parm;

       parm.dst = hgmp->linearaddress + windowx1 + windowy1 * hgmp->bytesperscanline ;
       parm.src = agmp->linearaddress + vfbstartdif;
       parm.x = window.xsize;
       parm.y = window.ysize;
       parm.steps = copybufsteps;
       parm.srcdif = agmp->bytesperscanline - vfbwidthused - 1;
       parm.dstdif = hgmp->bytesperscanline - window.xsize;
       parm.vfbsteps = copybufstepwidth;

      #ifdef _NOASM_
       copyvfb2displaymemory_zoom ( &parm.src, getfieldposx ( x1-1, y1-1), getfieldposy( x1-1, y1-1 ), getfieldposx ( x2, y2) + getfieldsizex(), getfieldposy( x2, y2 ) + getfieldsizex());
      #else
       copyvfb2displaymemory_zoom ( &parm.src );
      #endif
}




void tgeneraldisplaymap :: pnt_main ( void )
{

   activefontsettings.font = schriften.monogui;
   activefontsettings.background = 255;
   activefontsettings.color = white;
   activefontsettings.length = 0;
   activefontsettings.justify = lefttext;
   activefontsettings.height = 0;


   playerview = actmap->playerView;

    if ( playerview < 0 )
       displaymessage("tgeneraldisplaymap :: pnt_main ; playerview < 0", 2 );

   int b;
   pfield fld;

   int yp;
   int r;

   int x, y;

   /*****************************************************************/
   /*****************************************************************/

   for (int hgt = 0; hgt < 9 ;hgt++ ) {
      int binaryheight = 0;
      if ( hgt > 0 )
         binaryheight = 1 << ( hgt-1);

      for ( y=dispmapdata.disp.y1; y < dispmapdata.disp.y2; y++ ) {
         for ( x=dispmapdata.disp.x1; x < dispmapdata.disp.x2; x++ ) {
            fld = getfield ( actmap->xpos + x, actmap->ypos + y );
            if ( fld ) {
               b = fieldVisibility ( fld, playerview );

               if (y & 1 )   /*  ungerade reihennummern  */
                  r = vfbleftspace + fielddisthalfx + x * fielddistx;
               else
                  r = vfbleftspace + x * fielddistx;

               yp = vfbtopspace + y * fielddisty;


               if (b > visible_ago ) {



                  /* display buildings */

                      if ( fld->building  &&  (log2(fld->building->typ->buildingheight)+1 == hgt ) )
                           if ((b == visible_all) || (fld->building->typ->buildingheight >= chschwimmend) || ( fld->building->color == playerview*8 ))
                              if (fld->building->visible)
                                 if ( fld->building->typ->buildingheight < chschwimmend )
                                    putpicturemix ( r + buildingrightshift, yp + buildingdownshift ,fld->picture,fld->building->color, (char*) colormixbuf);
                                 else {
                                    if ( fld->building->typ->buildingheight >= chtieffliegend ) {
                                       int d = 6 * ( log2 ( fld->building->typ->buildingheight ) - log2 ( chfahrend ));
                                       putshadow ( r + buildingrightshift + d, yp + buildingdownshift + d,fld->picture, &xlattables.a.dark3);
                                    }
                                    putrotspriteimage( r + buildingrightshift, yp + buildingdownshift ,fld->picture,fld->building->color);
                                 }


                  /* display units */
                      if ( fld->vehicle  &&  (fld->vehicle->height == binaryheight))
                         if ( ( fld->vehicle->color == playerview * 8 ) || (b == visible_all) || ((fld->vehicle->height >= chschwimmend) && (fld->vehicle->height <= chhochfliegend)))
                            fld->vehicle->putimage ( r + unitrightshift , yp + unitdownshift );

                  /* display streets, railroads and pipelines */
                      if ( !fld->building || !fld->building->visible )
                           if ( fld->object )
                              for ( int n = 0; n < fld->object->objnum; n++ )  {
                                 int h = fld->object->object[n]->typ->height;
                                 if (  h >= hgt*30 && h < 30 + hgt*30 )
                                    fld->object->object[n]->display ( r - streetleftshift , yp - streettopshift, fld->getweather() );
                              }


                  /* display mines */
                      if ( b == visible_all )
                           if (fld->minenum() && hgt == 2 ) {
                              if ( fld->object->mine[0]->type != cmmooredmine )
                                 putspriteimage( r + unitrightshift , yp + unitdownshift ,getmineadress(fld->object->mine[0]->type) );
                              else
                                 putpicturemix ( r + unitrightshift , yp + unitdownshift ,getmineadress(fld->object->mine[0]->type, 1 ) ,  0, (char*) colormixbuf );
                              #ifdef karteneditor
                              bar ( r + unitrightshift + 5 , yp + unitdownshift +5, r + unitrightshift + 15 , yp + unitdownshift +10, 20 + fld->mineowner() * 8 );
                              #endif
                           }


                  /* display marked fields */
                      if ( hgt == 8 ) {

                          if ( fld->a.temp && tempsvisible )
                             putspriteimage(  r + unitrightshift , yp + unitdownshift ,cursor.markfield);
                          else
                             if ( fld->a.temp2 && tempsvisible )
                                putspriteimage(  r + unitrightshift , yp + unitdownshift , xlatpict ( &xlattables.a.dark2 , cursor.markfield));


                          #ifdef showtempnumber
                          activefontsettings.color = white;
                          showtext2(strrr( fld->temp ), r + unitrightshift + 5, yp + unitdownshift + 5 );
                          activefontsettings.color = black;
                          showtext2(strrr( fld->temp2 ), r + unitrightshift + 5, yp + unitdownshift + 20 );
                          #endif

                      /* display resources */
                          #ifndef karteneditor
                          if ( fld->resourceview && (fld->resourceview->visible & ( 1 << playerview) ) ){
                             if ( showresources == 1 ) {
                                showtext2( strrr ( fld->resourceview->materialvisible[playerview] ) , r + unitrightshift + 10 , yp + unitdownshift );
                                showtext2( strrr ( fld->resourceview->fuelvisible[playerview] )     , r + unitrightshift + 10 , yp + unitdownshift+ 10 );
                             } else
                                if ( showresources == 2 ) {
                                   if ( fld->resourceview->materialvisible[playerview] )
                                      bar ( r + unitrightshift + 10 , yp + unitdownshift -2, r + unitrightshift + 10 + fld->resourceview->materialvisible[playerview] / 10, yp + unitdownshift +2, 23 );

                                   if ( fld->resourceview->fuelvisible[playerview] )
                                      bar ( r + unitrightshift + 10 , yp + unitdownshift +10 -2, r + unitrightshift + 10 + fld->resourceview->fuelvisible[playerview] / 10, yp + unitdownshift +10 +2 , 191 );
                                }
                          }
                          #else
                          if ( showresources == 1 ) {
                             showtext2( strrr ( fld->material ) , r + unitrightshift + 10 , yp + unitdownshift );
                             showtext2( strrr ( fld->fuel )     , r + unitrightshift + 10 , yp + unitdownshift+ 10 );
                          }
                          else if ( showresources == 2 ) {
                             if ( fld->material )
                                bar ( r + unitrightshift + 10 , yp + unitdownshift -2, r + unitrightshift + 10 + fld->material / 10, yp + unitdownshift +2, 23 );
                             if ( fld->fuel )
                                bar ( r + unitrightshift + 10 , yp + unitdownshift +10 -2, r + unitrightshift + 10 + fld->fuel / 10, yp + unitdownshift +10 +2 , 191 );
                          }
                         #endif
                      }

               } else {
                  if (b == visible_ago) {
                       if ( fld->building )
                           if ((b == visible_all) || (fld->building->typ->buildingheight >= chschwimmend) || ( fld->building->color == playerview*8 ))
                               putrotspriteimage(r + buildingrightshift, yp + buildingdownshift, fld->picture, fld->building->color);

                  }
               }

            }
         }
      }
      displayadditionalunits ( hgt );

   }
      /****************************************************************************/
      /*viewbehinderungen: Visible_Ago,  Visible_NOT  zeichnen  ...            ÿ */
      /****************************************************************************/
     #ifndef HEXAGON
      for ( y=dispmapdata.disp.y1; y < dispmapdata.disp.y2; y++ ) {
         for ( x=dispmapdata.disp.x1; x < dispmapdata.disp.x2; x++ ) {

            if (y & 1 )
               r = vfbleftspace + fielddisthalfx + x * fielddistx;
            else
               r = vfbleftspace + x * fielddistx;
            yp = vfbtopspace + y * fielddisty;

            if ( viereck[x+2][y+2] )           // fr?her mal:   && (! (( x ==0) && ((y & 1) == 0)))   , aber da links genug Platz, m?áte es auch ohne klappen. Noch aus Vor-VFB-Zeiten
               putspriteimage( r + viereckrightshift , yp + viereckdownshift , view.viereck[viereck[x+2][y+2]]);
         }
      }
     #endif



      for ( y=dispmapdata.disp.y1; y < dispmapdata.disp.y2; y++ ) {
         for ( x=dispmapdata.disp.x1; x < dispmapdata.disp.x2; x++ ) {
            fld = getfield ( actmap->xpos + x, actmap->ypos + y );
            if ( fld ) {
               b = fieldVisibility (fld, playerview );

               if (y & 1 )   /*  ungerade reihennummern  */
                  r = vfbleftspace + fielddisthalfx + x * fielddistx;
               else
                  r = vfbleftspace + x * fielddistx;
               yp = vfbtopspace + y * fielddisty;

               if (b == visible_ago) {
                 #ifdef HEXAGON
                  for (int hgt = 0; hgt < 9 ;hgt++ ) {
                     int binaryheight = 0;
                     if ( hgt > 0 )
                        binaryheight = 1 << ( hgt-1);

                      /* display objects */
                      if ( !fld->building )
                           if ( fld->object )
                              for ( int n = 0; n < fld->object->objnum; n++ )
                                 if ( fld->object->object[n]->typ->visibleago ) {
                                    int h = fld->object->object[n]->typ->height;
                                    if (  h >= hgt*30 && h < 30 + hgt*30 )
                                       fld->object->object[n]->display ( r - streetleftshift , yp - streettopshift );
                                 }
                   }
                  #endif


                   // putspriteimage( r + unitrightshift , yp + unitdownshift , view.va8);
                   putshadow( r + unitrightshift , yp + unitdownshift , icons.view.nv8, &xlattables.a.dark2 );

                   if ( fld->a.temp && tempsvisible )
                      putspriteimage(  r + unitrightshift , yp + unitdownshift ,cursor.markfield);
                   else
                      if ( fld->a.temp2 && tempsvisible )
                         putspriteimage(  r + unitrightshift , yp + unitdownshift , xlatpict ( &xlattables.a.dark2 , cursor.markfield));

                   #ifdef showtempnumber
                   activefontsettings.color = white;
                   showtext2(strrr( fld->temp ), r + unitrightshift + 5, yp + unitdownshift + 5 );
                   activefontsettings.color = black;
                   showtext2(strrr( fld->temp2 ), r + unitrightshift + 5, yp + unitdownshift + 20 );
                   #endif

               } else
                 if (b == visible_not) {
                     putspriteimage( r + unitrightshift, yp + unitdownshift , icons.view.nv8 );
                     if ( ( fld->a.temp || fld->a.temp2 ) && tempsvisible )
                           putspriteimage(  r + unitrightshift , yp + unitdownshift ,cursor.markfield);

                   #ifdef showtempnumber
                   activefontsettings.color = white;
                   showtext2(strrr( fld->temp ), r + unitrightshift + 5, yp + unitdownshift + 5 );
                   activefontsettings.color = black;
                   showtext2(strrr( fld->temp2 ), r + unitrightshift + 5, yp + unitdownshift + 20 );
                   #endif

                 }
               /*
                       activefontsettings.color = white;
                       activefontsettings.font = schriften.guifont;
                       activefontsettings.length = 0;
                       activefontsettings.background = 255;
                       showtext2(strrr( fld->view[1].view ), r + 10 + 5, yp + 10 + 5 );
                       showtext2(strrr( fld->view[0].jamming ), r + 10 + 5, yp + 10 + 15 );
                */
            }
         }
      }



}

void tgeneraldisplaymap :: displayadditionalunits ( int height )
{
}

void tdisplaymap :: displayadditionalunits ( int height )
{
   int r;
   if ( height == 4 ) {

      if ( displaymovingunit.eht  &&  displaymovingunit.eht->height <= chfahrend ) {

         pfield fld = getfield ( displaymovingunit.xpos, displaymovingunit.ypos);
         int b = fieldVisibility ( fld, playerview );

         if ( displaymovingunit.hgt == 0 ) {
               if (displaymovingunit.ypos & 1 )   /*  ungerade reihennummern  */
                  r = vfbleftspace  + fielddisthalfx + (displaymovingunit.xpos - actmap->xpos) * fielddistx;
               else
                  r = vfbleftspace + (displaymovingunit.xpos - actmap->xpos) * fielddistx;
               int yp = vfbtopspace + (displaymovingunit.ypos - actmap->ypos) * fielddisty;

               if ( r >= 0 && yp >= 0 )
                  if ( displaymovingunit.eht->height >= chschwimmend )
                     putrotspriteimage( r + unitrightshift + displaymovingunit.dx , yp + unitdownshift + displaymovingunit.dy, displaymovingunit.eht->typ->picture[displaymovingunit.eht->direction], displaymovingunit.eht->color);
                  else
                     if ( b == visible_all )
                        putpicturemix ( r + unitrightshift + displaymovingunit.dx , yp + unitdownshift + displaymovingunit.dy, displaymovingunit.eht->typ->picture[displaymovingunit.eht->direction], displaymovingunit.eht->color, (char*) colormixbuf);
         }
      }
   }
   if ( height == 5 ) {
      if ( displaymovingunit.eht && displaymovingunit.eht->height > chfahrend) {
         pfield fld = getfield ( displaymovingunit.xpos, displaymovingunit.ypos);
         int b = fieldVisibility (fld, playerview );

          if (displaymovingunit.ypos & 1 )   /*  ungerade reihennummern  */
             r = vfbleftspace  + fielddisthalfx + (displaymovingunit.xpos - actmap->xpos) * fielddistx;
          else
             r = vfbleftspace + (displaymovingunit.xpos - actmap->xpos) * fielddistx;
          int yp = vfbtopspace + (displaymovingunit.ypos - actmap->ypos) * fielddisty;

         if ( r >= 0 && yp >= 0 ) {
            if ( (b == visible_all) || ( displaymovingunit.eht->height <= chhochfliegend ) || (displaymovingunit.eht->color / 8 == playerview )) {
               if ( displaymovingunit.hgt > 0 ) {
                  int d = 6 * displaymovingunit.hgt / 10 ;
                  putshadow ( r + unitrightshift + d + displaymovingunit.dx, yp + unitdownshift + d + displaymovingunit.dy, displaymovingunit.eht->typ->picture[displaymovingunit.eht->direction] , &xlattables.a.dark3);
               }
               putrotspriteimage( r + unitrightshift + displaymovingunit.dx , yp + unitdownshift + displaymovingunit.dy, displaymovingunit.eht->typ->picture[displaymovingunit.eht->direction], displaymovingunit.eht->color);
            }
         }
      }

   }
}


void         displaymap(  )
{
   if ( !actmap )
      return;

   if ( !actmap->xsize  ||  !actmap->ysize   || lockdisplaymap )
      return;

   activateGraphicSet ( actmap->graphicset );

   int         ms;   /*  mousestatus  */
   char      bb;   /*  cursorstatus   */

   ms = getmousestatus();
   bb = cursor.an;
   if (bb)
      cursor.hide();

   if (ms == 2)
      idisplaymap.setmouseinvisible();

   npush ( *agmp );

   if ( mapborderpainter )
      if ( mapborderpainter->getlastpaintmode () < 1)
          mapborderpainter->paint();

   if ( actmap )
      idisplaymap.playerview = actmap->playerView;

   #ifdef logging
   logtofile("spfst / displaymap ; vor pnt_terrain");
   #endif
   idisplaymap.pnt_terrain (  );

   #ifdef logging
   logtofile("spfst / displaymap ; vor pnt_main");
   #endif
   idisplaymap.pnt_main (  );

   #ifdef logging
   logtofile("spfst / displaymap ; vor cp_buf");
   #endif
   idisplaymap.cp_buf (  );

   npop ( *agmp );




   if (ms == 2)
      idisplaymap.restoremouse();

   if (bb)
      cursor.show();


}



struct tfieldlist {
                      int num;
                      int x[6];
                      int y[6];
                      int minx, maxx;
                      int miny, maxy;
                      // int minxp, maxxp;
                      // int minyp, maxyp;
                      int visible;
                   };

typedef tfieldlist* pfieldlist;


void adddirpts ( int x, int y, pfieldlist lst, int dir )
{
  int xt = x;
  int yt = y;
  getnextfield ( xt, yt, dir );
  lst->x [ lst->num ] = xt;
  lst->y [ lst->num ] = yt;
  lst->num++;
}

int pointvisible ( int x, int y )
{
   if ( (x >= 0) &&
        (x <  idisplaymap.getscreenxsize()) &&
        (y >= 0) &&
        (y < idisplaymap.getscreenysize()))
          return 1;
   else
          return 0;

}


pfieldlist generatelst ( int x1, int y1, int x2, int y2 )
{
  pfieldlist list = new ( tfieldlist );
  list->num = 2;
  list->x[0] = x1;
  list->y[0] = y1;
  list->x[1] = x2;
  list->y[1] = y2;
  list->visible = 0;


  int dir = getdirection(x1,y1,x2,y2);

 #ifdef HEXAGON
  switch ( dir ) {

     case 0: adddirpts ( x1, y1, list, 5 );
             adddirpts ( x1, y1, list, 1 );
             adddirpts ( x1, y1, list, 2 );
             break;

     case 1: adddirpts ( x1, y1, list, 0 );
             adddirpts ( x1, y1, list, 2 );
             adddirpts ( x2, y2, list, 2 );
             break;

     case 2: adddirpts ( x1, y1, list, 1 );
             adddirpts ( x1, y1, list, 3 );
             adddirpts ( x2, y2, list, 2 );
             break;

     case 3: adddirpts ( x2, y2, list, 2 );
             adddirpts ( x1, y1, list, 4 );
             break;

     case 4: adddirpts ( x1, y1, list, 3 );
             adddirpts ( x1, y1, list, 5 );
             adddirpts ( x1, y1, list, 2 );
             break;

     case 5: adddirpts ( x1, y1, list, 0 );
             adddirpts ( x1, y1, list, 4 );
             adddirpts ( x1, y1, list, 2 );
             break;

  } /* endswitch */
 #else
  switch ( dir ) {

     case 0: adddirpts ( x1, y1, list, 7 );
             adddirpts ( x1, y1, list, 1 );
             adddirpts ( x1, y1, list, 2 );
             adddirpts ( x1, y1, list, 3 );
             break;

     case 1: adddirpts ( x1, y1, list, 2 );
             adddirpts ( x1, y1, list, 3 );
             break;

     case 2: adddirpts ( x1, y1, list, 1 );
             adddirpts ( x1, y1, list, 3 );
             adddirpts ( x2, y2, list, 3 );
             adddirpts ( x2, y2, list, 4 );
             break;

     case 3: adddirpts ( x2, y2, list, 3 );
             break;

     case 4: adddirpts ( x1, y1, list, 3 );
             adddirpts ( x1, y1, list, 5 );
             adddirpts ( x2, y2, list, 2 );
             adddirpts ( x2, y2, list, 3 );
             break;

     case 5: adddirpts ( x1, y1, list, 3 );
             adddirpts ( x1, y1, list, 4 );
             break;

     case 6: adddirpts ( x1, y1, list, 3 );
             adddirpts ( x1, y1, list, 4 );
             adddirpts ( x2, y2, list, 1 );
             adddirpts ( x2, y2, list, 3 );
             break;

     case 7: adddirpts ( x1, y1, list, 3 );
             break;

  } /* endswitch */
 #endif

  list->minx = 0xffff;
  list->miny = 0xffff;
  list->maxx = 0;
  list->maxy = 0;

  for (int i = 0; i < list->num; i++) {

     if ( list->x[ i ] < list->minx )
        list->minx = list->x[ i ];

     if ( list->y[ i ] < list->miny )
        list->miny = list->y[ i ];

     if ( list->x[ i ] > list->maxx )
        list->maxx = list->x[ i ];

     if ( list->y[ i ] > list->maxy )
        list->maxy = list->y[ i ];

  } /* endfor */

  if ( pointvisible ( list->minx, list->miny ) )
     list->visible = 1;
  if ( pointvisible ( list->minx, list->maxy ) )
     list->visible = 1;
  if ( pointvisible ( list->maxx, list->miny ) )
     list->visible = 1;
  if ( pointvisible ( list->maxx, list->maxy ) )
     list->visible = 1;

  return list;
}


void  tdisplaymap :: resetmovement ( void )
{
   memset ( &displaymovingunit, 0, sizeof ( displaymovingunit ));
}


void  tdisplaymap :: movevehicle( int x1,int y1, int x2, int y2, pvehicle eht, int height1, int height2, int fieldnum, int totalmove )
{
   int         dir;

   int ms = getmousestatus();

   if ( ms == 2)
      setinvisiblemouserectanglestk ( scrleftspace, scrtopspace, scrleftspace + scrxsize, scrtopspace + scrysize );


   dir = getdirection(x1,y1,x2,y2);
   eht->direction = dir;


   displaymovingunit.eht = NULL;

   idisplaymap.pnt_terrain (  );
   idisplaymap.pnt_main (  );



   npush ( dispmapdata );
   x1 -= actmap->xpos;
   y1 -= actmap->ypos;
   x2 -= actmap->xpos;
   y2 -= actmap->ypos;

   int xp1,xp2, yp1, yp2;

   pfieldlist touchedfields = generatelst ( x1, y1, x2, y2 );

   if ( touchedfields->visible ) {

      xp1 = vfbleftspace + touchedfields->minx * fielddistx;
      if ( xp1 < 0 )
         xp1 = 0;

      xp2 = vfbleftspace + touchedfields->maxx * fielddistx + fielddisthalfx + fieldxsize;
      if ( xp2 > vfbscanlinelength )
         xp2 = vfbscanlinelength;


      yp1 = vfbtopspace + touchedfields->miny * fielddisty;
      if ( yp1 < 0 )
         yp1 = 0;
      yp2 = vfbtopspace + touchedfields->maxy * fielddisty + fieldysize;
      if ( yp2 > dispmapdata.vfbheight )
         yp2 = dispmapdata.vfbheight;

      char* picbuf = new char [ imagesize ( xp1, yp1, xp2, yp2 ) ];
      getimage ( xp1, yp1, xp2, yp2, picbuf );


      int dx;
      int dy;

     #ifdef HEXAGON
      switch ( dir ) {
      case 0: dx = 0;
              dy = -fielddisty*2;
              break;
      case 1: dx = fielddisthalfx;
              dy = -fielddisty;
              break;
      case 2: dx = fielddisthalfx;
              dy = fielddisty;
              break;
      case 3: dx = 0;
              dy = 2*fielddisty;
              break;
      case 4: dx = -fielddisthalfx;
              dy = fielddisty;
              break;
      case 5: dx = -fielddisthalfx;
              dy = -fielddisty;
              break;
      } /* endswitch */
     #else
      switch ( dir ) {
      case 0: dx = 0;
              dy = -fielddisty*2;
              break;
      case 1: dx = fielddisthalfx;
              dy = -fielddisty;
              break;
      case 2: dx = fielddistx;
              dy = 0;
              break;
      case 3: dx = fielddisthalfx;
              dy = fielddisty;
              break;
      case 4: dx = 0;
              dy = 2*fielddisty;
              break;
      case 5: dx = -fielddisthalfx;
              dy = fielddisty;
              break;
      case 6: dx = -fielddistx;
              dy = 0;
              break;
      case 7: dx = -fielddisthalfx;
              dy = -fielddisty;
              break;
      } /* endswitch */
     #endif

      displaymovingunit.eht = eht;

      int h1;
      int h2;

     /*
      int r1;
      if ( dir & 1 )
         r1 = 20;
      else
         r1 = 40; */

      int r2 = 0;


     /************************************/
     /*     Schattenposition computen   */
     /************************************/


      if ( height1 != height2  && ( height1 > chfahrend  || height2 > chfahrend )) {

         int compl = totalmove;
         int went = fieldnum;
         // int togo = totalmove - fieldnum;

         int ht1 = 10 * ( log2 ( height1 ) - log2 ( chfahrend ) );
         int ht2 = 10 * ( log2 ( height2 ) - log2 ( chfahrend ) );
         int htd = ht2- ht1;

         if ( height1 < height2 ) {
            if ( height1 == chfahrend ) {
               int takeoff = compl * 2 / 3 ;
               int ascend = compl - takeoff;
               if ( went >=  takeoff ) {
                  h1 = ht1 + htd * ( went-takeoff ) / ascend;
                  h2 = ht1 + htd * ( went-takeoff + 1 ) / ascend;
               } else {
                  h1 = ht1;
                  h2 = ht1;
               }
            } else {
               h1 = ht1 + htd * went / compl;
               h2 = ht1 + htd * (went + 1) / compl;
            }
         } else {
            if ( height2 == chfahrend ) {
               int landpos = (compl+2) / 3;

               if ( went <  landpos ) {
                  h1 = ht1 + htd * went / landpos;
                  h2 = ht1 + htd * (went + 1 ) / landpos;
               } else {
                  h1 = ht2;
                  h2 = ht2;
               }
            } else {
               h1 = ht1 + htd * went / compl;
               h2 = ht1 + htd * (went + 1) / compl;
            }
         }

      } else
         if ( eht->height > chfahrend ) {
            h1 = 10 * ( log2 ( eht->height ) - log2 ( chfahrend ) );
            h2 = h1;
         } else {
            h1 = 0;
            h2 = 0;
         }

      // int step = 1;


     /************************************/
     /*     eigentliche movement         */
     /************************************/

      int starttick = ticker;
      int sdx = dx;
      int sdy = dy;

      do {
         putimage ( xp1, yp1, picbuf );

         r2++;


         int tick =  CGameOptions::Instance()->movespeed - ( ticker - starttick );
         if ( tick < 0 )
            tick = 0;

         displaymovingunit.xpos = x2;
         displaymovingunit.ypos = y2;
         displaymovingunit.dx = 0 ;// -dx;
         displaymovingunit.dy = 0 ;// -dy;
         displaymovingunit.hgt =
				h2 - ( h2 - h1 ) * tick /
				( CGameOptions::Instance()->movespeed);

         int r;
         if ( displaymovingunit.ypos & 1 )   /*  ungerade reihennummern  */
            r = vfbleftspace + fielddisthalfx + x2 * fielddistx + unitrightshift - dx;
         else
            r = vfbleftspace + x2 * fielddistx + unitrightshift - dx;


         int yp = vfbtopspace + y2 * fielddisty + unitdownshift - dy;


         if ( r >= 0  &&  yp >= 0 &&  yp+unitsizey <= dispmapdata.vfbheight && r+unitsizex <= dispmapdata.vfbwidth ) {
            if ( displaymovingunit.hgt ) {
               int d = 6 * displaymovingunit.hgt / 10 ;
               putshadow ( r + d , yp + d , displaymovingunit.eht->typ->picture[displaymovingunit.eht->direction] , &xlattables.a.dark3);
            }
            if( eht->height >= chschwimmend )
               putrotspriteimage( r , yp , displaymovingunit.eht->typ->picture[displaymovingunit.eht->direction], displaymovingunit.eht->color);
            else
               putpicturemix ( r , yp , displaymovingunit.eht->typ->picture[displaymovingunit.eht->direction], displaymovingunit.eht->color, (char*) colormixbuf);

            idisplaymap.cp_buf ( touchedfields->minx, touchedfields->miny, touchedfields->maxx, touchedfields->maxy );
         }


         dx = sdx * tick / CGameOptions::Instance()->movespeed;
         dy = sdy * tick / CGameOptions::Instance()->movespeed;

         /*
         if ( dx > 0 )
            dx-=step;
         else
            if ( dx < 0 )
               dx+=step;
         if ( dy > 0 )
            dy-=step;
         else
            if ( dy < 0 )
               dy+=step;
         */
      } while ( dx!=0 || dy!=0  ); /* enddo */

      displaymovingunit.xpos = x2 + actmap->xpos;
      displaymovingunit.ypos = y2 + actmap->ypos;

      delete touchedfields;
      delete[] picbuf;

   }
   npop ( dispmapdata );
   *agmp = *hgmp;

   if ( ms == 2)
      getinvisiblemouserectanglestk (  );
}


tlockdispspfld :: tlockdispspfld ( void )
{
   lockdisplaymap ++;
}

tlockdispspfld :: ~tlockdispspfld ()
{
   lockdisplaymap --;
}





tdisplaywholemap :: ~tdisplaywholemap ( )
{
   if ( dsp ) {
      delete dsp;
      dsp = NULL;
   }
}

void tdisplaywholemap :: init ( int xs, int ys )
{
   tgeneraldisplaymap :: init ( xs, ys );

   dispmapdata.disp.x1 = 0;
   dispmapdata.disp.y1 = 0;
   dispmapdata.disp.x2 = xs;
   dispmapdata.disp.y2 = ys;

   int bufsizex = xs * fielddistx + 200 ;
   int bufsizey = ys * fielddisty + 200 ;

   dsp = new tvirtualdisplay ( bufsizex, bufsizey, 0 );

   vfb.parameters = *agmp;
   vfb.parameters.directscreenaccess = 1;
   dispmapdata.vfbadress = (void*) agmp->linearaddress;

}

int tdisplaywholemap :: getWidth( )
{
  return getscreenxsize() * fielddistx + 20 + 1;
}

int tdisplaywholemap :: getHeight( )
{
  return (getscreenysize() + 1 ) * fielddisty + 1;
}

void tdisplaywholemap :: cp_buf ( void )
{
   writepcx ( filename.c_str(), vfbleftspace, vfbtopspace, vfbleftspace + getWidth()-1, vfbtopspace + getHeight()-1, pal );

}


void writemaptopcx ( bool confirmation, string filename, int* width, int* height )
{

   if ( confirmation ) {
      int mb = 1024*1024;
      int bufsizex = actmap->xsize * fielddistx + 200 ;
      int bufsizey = actmap->ysize * fielddisty + 200 ;
      int bufsize = (bufsizex * bufsizey + mb - 1) / mb;
      if ( choice_dlg( "About %d MB of Buffer will be allocated !", "c~o~ntinue", "~c~ancel", bufsize) == 2)
         return;

      displaymessage ( "writing image\nplease wait",0 );
   }


   npush ( *agmp );
   npush ( actmap->xpos );
   npush ( actmap->ypos );

   actmap->xpos = 0;
   actmap->ypos = 0;

   try {
      if ( filename.empty() )
         filename = getnextfilenumname ( "map", "pcx" );

      tdisplaywholemap wm ( filename );
      wm.init ( actmap->xsize, actmap->ysize );
      wm.pnt_terrain ( );
      wm.pnt_main ( );
      wm.cp_buf (  );
      if ( width )
         *width = wm.getWidth();

      if ( height )
         *height = wm.getHeight();
   } /* endtry */
   catch ( fatalgraphicserror ) {
      displaymessage("unable to generate image", 1 );
   } /* endcatch */

   npop ( actmap->ypos );
   npop ( actmap->xpos );
   npop ( *agmp );

   if ( confirmation )
      removemessage();
}


int mousecurs = 8;
int mousescrollspeed = 6;
int lastmousemapscrolltick = 0;

const int mousehotspots[9][2] = { { 8, 0 }, { 15, 0 }, { 15, 8 }, { 15, 15 },
                                  { 8, 15 }, { 0, 15 }, { 0, 8 }, { 0, 0 },
                                  { 0, 0 } };


void checkformousescrolling ( void )
{
   if ( isfullscreen() )
      if ( !mouseparams.x  ||  !mouseparams.y   ||  mouseparams.x >= hgmp->resolutionx-1  ||   mouseparams.y >= hgmp->resolutiony-1 ) {
         if ( mouseparams.taste == CGameOptions::Instance()->mouse.scrollbutton ) {
            if ( lastmousemapscrolltick + mousescrollspeed < ticker ) {

               int newx = actmap->xpos;
               int newy = actmap->ypos;
               switch ( mousecurs ) {
                  case 0: newy-=2;
                     break;
                  case 1: newy-=2;
                          newx+=1;
                     break;
                  case 2: newx+=1;
                     break;
                  case 3: newy+=2;
                          newx+=1;
                     break;
                  case 4: newy+=2;
                     break;
                  case 5: newy+=2;
                          newx-=1;
                     break;
                  case 6: newx-=1;
                     break;
                  case 7: newy-=2;
                          newx-=1;
                     break;
               } /* endswitch */

               if ( newx < 0 )
                  newx = 0;
               if ( newy < 0 )
                  newy = 0;
               if ( newx > actmap->xsize - idisplaymap.getscreenxsize() )
                  newx = actmap->xsize - idisplaymap.getscreenxsize();
               if ( newy > actmap->ysize - idisplaymap.getscreenysize() )
                  newy = actmap->ysize - idisplaymap.getscreenysize();

               if ( newx != actmap->xpos  || newy != actmap->ypos ) {
                  cursor.hide();
                  int cursx = cursor.posx + (actmap->xpos - newx );
                  int cursy = cursor.posy + (actmap->ypos - newy );
                  if ( cursx < 0 )
                     cursx = 0;
                  if ( cursx >= idisplaymap.getscreenxsize())
                     cursx = idisplaymap.getscreenxsize() -1 ;
                  if ( cursy < 0 )
                     cursy = 0;
                  if ( cursy >= idisplaymap.getscreenysize())
                     cursy = idisplaymap.getscreenysize() -1 ;

                  cursor.posx = cursx;
                  cursor.posy = cursy;

                  actmap->xpos = newx;
                  actmap->ypos = newy;
                  displaymap();
                  lastmousemapscrolltick = ticker;

                  cursor.show();
               }
            }
         }
      }
}



tmousescrollproc mousescrollproc;


void tmousescrollproc :: mouseaction ( void )
{
   int pntnum = -1;

   void* newpnt = NULL;

   if ( mouseparams.x1 == 0 ) {
      if ( mouseparams.y1 == 0 )
         pntnum = 7;
      else
        if ( mouseparams.y1 + mouseparams.ysize >= hgmp->resolutiony  )
           pntnum = 5;
        else
           pntnum = 6;
   } else {
      if ( mouseparams.x1 + mouseparams.xsize >= hgmp->resolutionx ) {
         if ( mouseparams.y1 == 0 )
             pntnum = 1;
         else
           if ( mouseparams.y1 + mouseparams.ysize >= hgmp->resolutiony  )
             pntnum = 3;
           else
             pntnum = 2;
      } else {
         if ( mouseparams.y1 == 0 )
             pntnum = 0;
         else
           if ( mouseparams.y1 + mouseparams.ysize >= hgmp->resolutiony  )
             pntnum = 4;
           else
             pntnum = 8;
      }
   }
   if ( pntnum < 8 )
      newpnt = icons.pfeil2[pntnum];
   else
      newpnt = icons.mousepointer;

   mousecurs = pntnum;

   if ( mouseparams.pictpointer != newpnt )
      setnewmousepointer ( newpnt, mousehotspots[pntnum][0], mousehotspots[pntnum][1] );
}


/*
class MapDisplay {
         public:
           void displayMovingUnit ( int x1, int y1, int x2, int y2, pvehicle veh );

    };
*/

int  MapDisplay :: displayMovingUnit ( int x1,int y1, int x2, int y2, pvehicle vehicle, int height1, int height2, int fieldnum, int totalmove )
{
   if ( actmap->playerView < 0 )
      return 0;

   if ( height2 == -1 )
      height2 = height1;

   pfield fld1 = getfield ( x1, y1 );
   int view1 = fieldVisibility ( fld1, actmap->playerView );

   pfield fld2 = getfield ( x2, y2 );
   int view2 = fieldVisibility ( fld2, actmap->playerView );

   if (  view1 >= visible_now  ||  view2 >= visible_now )
      if ( ((vehicle->height >= chschwimmend) && (vehicle->height <= chhochfliegend)) || (( view1 == visible_all) && ( view2 == visible_all )) || ( actmap->actplayer == actmap->playerView ))
         idisplaymap.movevehicle( x1, y1, x2, y2, vehicle, height1, height2, fieldnum, totalmove );


   int result = 0;
   if (view1 >= visible_now )
      result +=1;

   if ( view2 >= visible_now )
      result +=2;

   return result;
}

void MapDisplay :: deleteVehicle ( pvehicle vehicle )
{
   idisplaymap.deletevehicle();
}

void MapDisplay :: displayMap ( void )
{
   displaymap();
}

void MapDisplay :: displayPosition ( int x, int y )
{
   displaymap();
}


void MapDisplay :: resetMovement ( void )
{
   idisplaymap.resetmovement();
}

void MapDisplay :: startAction ( void )
{
   int bb = cursor.an;

   if ( bb )
      cursor.hide();
   cursorstat[cursorstatnum++] = bb;
}

void MapDisplay :: stopAction ( void )
{
   int b;
   b = cursorstat[--cursorstatnum];
   if(  cursorstatnum < 0 )
      displaymessage ( "void MapDisplay :: stopAction / underflow !", 2 );

   if ( b )
      cursor.show();

}


#ifdef sgmain
tbackgroundpict backgroundpict;

tbackgroundpict :: tbackgroundpict ( void )
{
   background = NULL;
   inited = 0;
   run = 0;
   lastpaintmode = -1;
}

void tbackgroundpict :: paintrectangleborder ( void )
{
   if ( rectangleborder.initialized ) {
         int width = 2;
         for ( int i = 0; i < 3; i++ )
            for ( int j = 0; j < width; j++ ) {
               int p = i * width + j;
               int x;
               int y;
               for ( x = rectangleborder.x1 - p; x < rectangleborder.x2 + p; x++ )
                  putpixel ( x, rectangleborder.y1 - p, xlattables.a.dark2[ getpixel ( x, rectangleborder.y1-p )] );
               for ( y = rectangleborder.y1 - p; y < rectangleborder.y2 + p; y++ )
                  putpixel ( rectangleborder.x2 + p, y, xlattables.light[ getpixel ( rectangleborder.x2 + p, y )] );
               for ( x = rectangleborder.x2 + p; x > rectangleborder.x1 - p; x-- )
                  putpixel ( x, rectangleborder.y2 + p, xlattables.light[ getpixel ( x, rectangleborder.y2+p )] );
               for ( y = rectangleborder.y2 + p; y > rectangleborder.y1 - p; y-- )
                  putpixel ( rectangleborder.x1 - p, y, xlattables.a.dark2[ getpixel ( rectangleborder.x1 - p, y )] );

            }
      }
}


void tbackgroundpict :: init ( int reinit )
{
   if ( !inited || reinit ) {
     /*
     int borderx1 = getmapposx ( );
     int bordery1 = getmapposy ( );
     */
     int borderx1 = 0;
     int bordery1 = 0;

     int bordery2 = bordery1 + (idisplaymap.getscreenysize() - 1) * fielddisty + fieldsizey - 1;
     int borderx2 = borderx1 + (idisplaymap.getscreenxsize() - 1 ) * fielddistx + fieldsizex + fielddisthalfx - 1;

    #ifdef HEXAGON
     int height, width;

     getpicsize ( borderpicture[2], width, height );

     borderpos[0].x = borderx1 - mapborderwidth;
     borderpos[0].y = bordery1 - mapborderwidth;

     borderpos[2].x = borderx2 + mapborderwidth - ( width -1 );
     borderpos[2].y = bordery1 - mapborderwidth;


     getpicsize ( borderpicture[6], width, height );

     borderpos[1].x = borderx1 - mapborderwidth + 28;
     borderpos[1].y = bordery1 - mapborderwidth;

     borderpos[6].x = borderx1 - mapborderwidth + 28 + fielddisthalfx;
     borderpos[6].y = bordery2 + mapborderwidth - ( height-1);


     getpicsize ( borderpicture[4], width, height );

     borderpos[3].x = borderx1 - mapborderwidth;
     borderpos[3].y = bordery1 + fielddisty;

     borderpos[4].x = borderx2 + mapborderwidth - ( width - 1 );
     borderpos[4].y = bordery1 + 2 * fielddisty;


     getpicsize ( borderpicture[5], width, height );

     borderpos[5].x = borderx1 - mapborderwidth;
     borderpos[5].y = bordery2 + mapborderwidth - ( height - 1 );


     getpicsize ( borderpicture[7], width, height );

     borderpos[7].x = borderx2 + mapborderwidth - ( width- 1);
     borderpos[7].y = bordery2 + mapborderwidth - ( height - 1 );
    #endif
     inited = 1;

   }
}



void tbackgroundpict :: load ( void )
{
   int w;
   {
      tnfilestream stream ("amatur.raw", tnstream::reading);
      for ( int i = 0; i< 7; i++ )
         stream.readrlepict ( &dashboard[i], false, &w );
     #ifdef FREEMAPZOOM
      #ifdef sgmain
      ::dashboard.zoom.pic = dashboard[6];
      #endif
     #endif
   }
   #ifdef HEXAGON
   {
      tnfilestream stream ("hxborder.raw", tnstream::reading);
      for ( int i = 0; i < 8; i++ )
         stream.readrlepict ( &borderpicture[i], false, &w );
   }
   #endif
}


void tbackgroundpict :: paintborder ( int dx, int dy )
{
   paintborder ( dx, dy , 0 );
}

void tbackgroundpict :: paintborder ( int dx, int dy, int reinit )
{
   if ( lastpaintmode < 1 ) {
      #ifdef FREEMAPZOOM
       paintrectangleborder (  );
      #else

      if ( !inited || reinit )
           init( reinit );
       putspriteimage ( borderpos[0].x + dx, borderpos[0].y + dy,  borderpicture[0] );
       putspriteimage ( borderpos[2].x + dx, borderpos[2].y + dy,  borderpicture[2] );

       for ( int x = 0; x < idisplaymap.getscreenxsize() -1; x++ ) {
          putspriteimage ( borderpos[1].x + dx + x * fielddistx, borderpos[1].y + dy,  borderpicture[1] );
          putspriteimage ( borderpos[6].x + dx + x * fielddistx, borderpos[6].y + dy,  borderpicture[6] );
       }

       for ( int y = 0; y < idisplaymap.getscreenysize() -2; y+=2 ) {
          putspriteimage ( borderpos[3].x + dx, borderpos[3].y + y * fielddisty + dy,  borderpicture[3] );
          putspriteimage ( borderpos[4].x + dx, borderpos[4].y + y * fielddisty + dy,  borderpicture[4] );
       }

       putspriteimage ( borderpos[5].x + dx, borderpos[5].y + dy,  borderpicture[5] );
       putspriteimage ( borderpos[7].x + dx, borderpos[7].y + dy,  borderpicture[7] );
      #endif
      lastpaintmode = 1;
   }
}

void  tbackgroundpict :: paint ( int resavebackground )
{
  collategraphicoperations cgo;
  init();

  #ifndef HEXAGON
  if (hgmp->resolutionx == 640) {
     if ( actmap->xsize && !lockdisplaymap ) {
        tnfilestream stream ( "bkgr4.pcx", 1 );
        loadpcxxy( &stream, 0, 0 );
        lastpaintmode = 1;
     } else {
        tnfilestream stream ( "bkgr42.pcx", 1 );
        loadpcxxy( &stream, 0, 0 );
        lastpaintmode = 0;
     }
  } else
  #endif
  {
     if ( !background ) {

        char filename[100];
        sprintf( filename, "%d%d.pcx", hgmp->resolutionx, hgmp->resolutiony );
        if ( exist ( filename )) {
           tnfilestream stream ( filename, tnstream::reading );
           loadpcxxy( &stream ,0,0);

           background = new char[ imagesize ( 0, 0, agmp->resolutionx, agmp->resolutiony  )];
        } else {
           displaymessage2("generating background picture; please wait ..." );
           if ( !asc_paletteloaded )
              loadpalette();
           int x = hgmp->resolutionx;
           int y = hgmp->resolutiony;
           {
              tvirtualdisplay vdp ( agmp->resolutionx, agmp->resolutiony );
              tnfilestream stream ( "640480.pcx", tnstream::reading );
              loadpcxxy( &stream ,0,0);
              char* pic = new char[ imagesize ( 0, 0, 639, 479 )];
              getimage ( 0, 0, 639, 479, pic );

              TrueColorImage* img = zoomimage ( pic, x, y, pal, 1, 0  );
              delete pic;
              background = convertimage ( img, pal );
           }
           putimage ( 0, 0, background );
           writepcx ( filename, 0, 0, agmp->resolutionx-1, agmp->resolutiony-1, pal );

           /*
           bar ( 0, 0, agmp->resolutionx-1, agmp->resolutiony-1, greenbackgroundcol );
           found = 0;
           */
        }

        putimage ( agmp->resolutionx - ( 640 - 450), 15,  dashboard[0] );
        putimage ( agmp->resolutionx - ( 640 - 450), 211, dashboard[1] );
        putimage ( agmp->resolutionx - ( 640 - 450), agmp->resolutiony - ( 480 - 433),  dashboard[2] );
        int l_width, l_height;
        int m_width, m_height;
        int r_width, r_height;
        getpicsize ( dashboard[3], l_width, l_height );
        getpicsize ( dashboard[4], m_width, m_height );
        getpicsize ( dashboard[5], r_width, r_height );

        int lpos = 14;
        putimage ( lpos,  agmp->resolutiony - ( 480 - 442),  dashboard[3] );
        putimage ( lpos + l_width,  agmp->resolutiony - ( 480 - 442),  dashboard[4] );

        int rpos = agmp->resolutionx - ( 640 - 433) - r_width + 1;
        lpos += l_width + m_width;

        putimage ( rpos,  agmp->resolutiony - ( 480 - 442),  dashboard[5] );

        while ( rpos > lpos ) {
           rpos -= m_width;
           putimage ( rpos,  agmp->resolutiony - ( 480 - 442),  dashboard[4] );
        }

        getimage ( 0, 0, agmp->resolutionx-1, agmp->resolutiony-1, background );

     } else
        putimage ( 0, 0, background );

    lastpaintmode = 0;

    #ifdef HEXAGON
     if ( actmap && actmap->xsize && !lockdisplaymap )
        paintborder( getmapposx ( ), getmapposy ( ) );
    #endif
  }
  if ( resavebackground  ||  !run ) {
     gui.savebackground ( );
  }
  run++;
  ::dashboard.repainthard = 1;

}

int tbackgroundpict :: getlastpaintmode ( void )
{
   return lastpaintmode;
}

#endif


