/*! \file spfst.cpp
    \brief map accessing and usage routines used by ASC and the mapeditor
*/

//     $Id: spfst.cpp,v 1.118 2003-02-12 20:11:53 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.117  2002/12/17 22:02:17  mbickel
//      Enemy mines can now be crossed even if visible
//      submerged mines can not be placed on shallow water
//      new game parameter: objects destroyable by terrain
//
//     Revision 1.116  2002/12/15 23:54:46  mbickel
//      New: system sends mail if units die
//      Fixed: orbiting and submerged units could crash on water due to wind
//      Updated documentation
//
//     Revision 1.115  2002/11/24 10:54:19  mbickel
//      made field allocation more robust
//
//     Revision 1.114  2002/11/17 11:43:23  mbickel
//      Fixed replay errors when replaying the AI moves
//
//     Revision 1.113  2002/11/02 14:13:18  mbickel
//      New net handling for objects
//
//     Revision 1.112  2002/11/01 14:06:53  mbickel
//      Changing the height between floating and driving has no movecost any more
//      submarines evalute the field properties, not the terrain type properties
//
//     Revision 1.111  2002/10/09 16:58:46  mbickel
//      Fixed to GrafikSet loading
//      New item filter for mapeditor
//
//     Revision 1.110  2002/09/19 20:20:05  mbickel
//      Cleanup and various bug fixes
//
//     Revision 1.109  2002/03/19 20:38:56  mbickel
//      Some cleanup and documentation in dlg_box
//      Fixed some type assignment errors
//
//     Revision 1.108  2002/03/18 21:42:17  mbickel
//      Some cleanup and documentation in the Mine class
//      The number of mines is now displayed in the field information window
//
//     Revision 1.107  2002/03/03 22:19:33  mbickel
//      Updated documentation
//      Improved user interface
//      Fixed AI bug
//
//     Revision 1.106  2002/03/03 14:13:49  mbickel
//      Some documentation updates
//      Soundsystem update
//      AI bug fixed
//
//     Revision 1.105  2002/03/02 23:04:01  mbickel
//      Some cleanup of source code
//      Improved Paragui Integration
//      Updated documentation
//      Improved Sound System
//
//     Revision 1.104  2001/12/19 17:16:29  mbickel
//      Some include file cleanups
//
//     Revision 1.103  2001/12/14 10:20:05  mbickel
//      Cleanup and enhancements to configure.in
//      Removed last remains of octagonal version from source files
//
//     Revision 1.102  2001/11/04 22:52:35  mbickel
//      Fixed bug in wood net calculation
//      Fixed broken refuel dialog
//
//     Revision 1.101  2001/11/04 21:50:16  mbickel
//      Fixed error messages in AI
//      Fixed wrong displaying of objects in snow
//
//     Revision 1.100  2001/10/31 18:34:33  mbickel
//      Some adjustments and fixes for gcc 3.0.2
//
//     Revision 1.99  2001/10/21 20:18:39  mbickel
//      Fixed non-empty table problem with BI3 map import
//      Added ini parameter to specify wood behaviour
//
//     Revision 1.98  2001/10/16 19:58:20  mbickel
//      Added title screen for mapeditor
//      Updated source documentation
//
//     Revision 1.97  2001/10/11 10:41:06  mbickel
//      Restructured platform fileio handling
//      Added map archival information to mapeditor
//
//     Revision 1.96  2001/10/02 14:06:28  mbickel
//      Some cleanup and documentation
//      Bi3 import tables now stored in .asctxt files
//      Added ability to choose amoung different BI3 import tables
//      Added map transformation tables
//
//     Revision 1.95  2001/09/25 09:35:12  mbickel
//      Fixed disappearing building when resizing map
//
//     Revision 1.94  2001/09/24 17:22:12  mbickel
//      Fixed crash in end of turn
//      Improved documentation
//
//     Revision 1.93  2001/08/06 20:54:43  mbickel
//      Fixed lots of crashes related to the new text files
//      Fixed delayed events
//      Fixed crash in terrin change event
//      Fixed visibility of mines
//      Fixed crashes in event loader
//
//     Revision 1.92  2001/08/02 18:50:43  mbickel
//      Corrected Error handling in Text parsers
//      Improved version information
//
//     Revision 1.91  2001/07/28 21:09:08  mbickel
//      Prepared vehicletype structure for textIO
//
//     Revision 1.90  2001/07/28 11:19:12  mbickel
//      Updated weaponguide
//      moved item repository from spfst to itemrepository
//
//     Revision 1.89  2001/07/27 21:13:35  mbickel
//      Added text based file formats
//      Terraintype and Objecttype restructured
//
//     Revision 1.88  2001/07/18 16:05:47  mbickel
//      Fixed: infinitive loop in displaying "player exterminated" msg
//      Fixed: construction of units by units: wrong player
//      Fixed: loading bug of maps with mines
//      Fixed: invalid map parameter
//      Fixed bug in game param edit dialog
//      Fixed: cannot attack after declaring of war
//      New: ffading of sounds
//
//     Revision 1.87  2001/03/30 12:43:16  mbickel
//      Added 3D pathfinding
//      some cleanup and documentation
//      splitted the ai into several files, now located in the ai subdirectory
//      AI cares about airplane servicing and range constraints
//
//     Revision 1.86  2001/02/26 12:35:31  mbickel
//      Some major restructuing:
//       new message containers
//       events don't store pointers to units any more
//       tfield class overhauled
//
//     Revision 1.85  2001/02/18 15:37:19  mbickel
//      Some cleanup and documentation
//      Restructured: vehicle and building classes into separate files
//         tmap, tfield and helper classes into separate file (gamemap.h)
//      basestrm : stream mode now specified by enum instead of int
//
//     Revision 1.84  2001/02/11 11:39:43  mbickel
//      Some cleanup and documentation
//
//     Revision 1.83  2001/02/01 22:48:49  mbickel
//      rewrote the storing of units and buildings
//      Fixed bugs in bi3 map importing routines
//      Fixed bugs in AI
//      Fixed bugs in mapeditor
//
//     Revision 1.82  2001/01/28 23:00:41  mbickel
//      Made the small editors compilable with Watcom again
//
//     Revision 1.81  2001/01/28 14:04:19  mbickel
//      Some restructuring, documentation and cleanup
//      The resource network functions are now it their own files, the dashboard
//       as well
//      Updated the TODO list
//
//     Revision 1.80  2001/01/25 23:45:04  mbickel
//      Moved map displaying routins to own file (mapdisplay.cpp)
//      Wrote program to create pcx images from map files (map2pcx.cpp)
//      Fixed bug in repair function: too much resource consumption
//      AI improvements and bug fixes
//      The BI3 map import function now evaluates the player status (human/
//       computer)
//
//     Revision 1.79  2001/01/23 21:05:20  mbickel
//      Speed up of AI
//      Lot of bugfixes in AI
//      Moved Research to own files (research.*)
//      Rewrote storing of developed technologies
//      Some cleanup and documentation
//
//     Revision 1.78  2001/01/21 16:37:20  mbickel
//      Moved replay code to own file ( replay.cpp )
//      Fixed compile problems done by cleanup
//
//     Revision 1.77  2001/01/19 13:33:55  mbickel
//      The AI now uses hemming
//      Several bugfixes in Vehicle Actions
//      Moved all view calculation to viewcalculation.cpp
//      Mapeditor: improved keyboard support for item selection
//
//     Revision 1.76  2000/12/28 16:58:38  mbickel
//      Fixed bugs in AI
//      Some cleanup
//      Fixed crash in building construction
//
//     Revision 1.75  2000/12/27 22:23:16  mbickel
//      Fixed crash in loading message text
//      Removed many unused variables
//
//     Revision 1.74  2000/11/21 20:27:08  mbickel
//      Fixed crash in tsearchfields (used by object construction for example)
//      AI improvements
//      configure.in: added some debug output
//                    fixed broken check for libbz2
//
//     Revision 1.73  2000/11/15 19:28:34  mbickel
//      AI improvements
//
//     Revision 1.72  2000/11/14 20:36:42  mbickel
//      The AI can now use supply vehicles
//      Rewrote objecttype IO routines to make the structure independant of
//       the memory layout
//
//     Revision 1.71  2000/11/09 18:39:00  mbickel
//      Fix: mapeditor crashed at startup due to invalid map being generated
//
//     Revision 1.70  2000/11/08 19:31:14  mbickel
//      Rewrote IO for the tmap structure
//      Fixed crash when entering damaged building
//      Fixed crash in AI
//      Removed item CRCs
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
#include <cstring>
#include <utility>
#include <map>
#include <SDL_image.h>


#include "vehicletype.h"
#include "buildingtype.h"

#include "basestrm.h"
#include "tpascal.inc"
#include "misc.h"
#include "basegfx.h"
#include "newfont.h"
#include "typen.h"
#include "spfst.h"
#include "events.h"
#include "dlg_box.h"
#include "loaders.h"
#include "stack.h"
#include "loadpcx.h"
#include "attack.h"
#include "gameoptions.h"
#include "itemrepository.h"

#ifndef karteneditor
  #include "missions.h"
  #include "gamedlg.h"
#endif

#include "dialog.h"
#include "loadbi3.h"
#include "mapalgorithms.h"
#include "mapdisplay.h"
#include "vehicle.h"
#include "buildings.h"


   tcursor            cursor;
   pmap              actmap;

   Schriften schriften;


int  rol ( int valuetorol, int rolwidth )
{
   int newvalue = valuetorol << rolwidth;
   newvalue |= valuetorol >> ( 32 - rolwidth );
   return newvalue;
}







int          terrainaccessible ( const pfield        field, const pvehicle     vehicle, int uheight )
{
   int res  = terrainaccessible2 ( field, vehicle, uheight );
   if ( res < 0 )
      return 0;
   else
      return res;
}

int          terrainaccessible2 ( const pfield        field, const pvehicle     vehicle, int uheight )
{ 
   if ( uheight == -1 )
      uheight = vehicle->height;

   if ( !((uheight & vehicle->typ->height) || ((vehicle->functions & cfparatrooper) && (uheight & (chtieffliegend | chfliegend )))))
      return 0;


   if ( uheight >= chtieffliegend) 
      return 2;
   else {
        if ( uheight == chtiefgetaucht ) 
           if ( (field->bdt & getTerrainBitType(cbwater3) ).any() )
              return 2;
           else
              return -1;
        else
           if ( uheight == chgetaucht )
              if ( (field->bdt & ( getTerrainBitType(cbwater3) | getTerrainBitType(cbwater2 )) ).any() )
                 return 2; 
              else
                 return -2;
           else {
              if ( vehicle->typ->terrainaccess.accessible ( field->bdt ) > 0 )
                 return 2;
              else
                 return -3;
            }
   }
}


int         fieldaccessible( const pfield        field,
                            const pvehicle     vehicle,
                            int  uheight )
{
   if ( !field || !vehicle )
      return 0;

   if ( uheight == -1 )
      uheight = vehicle->height;

   int c = fieldVisibility ( field, vehicle->color/8 );

   if (field == NULL) 
     return 0;

   if (c == visible_not)
      return 0;

/*
   if ( c == visible_all)
      if ( field->mines.size() )
         for ( int i = 0; i < field->mines.size(); i++ )
            if ( field->getMine(i).attacksunit( vehicle ))
               return 0;
*/


   if ( !field->vehicle && !field->building ) {
      if ( vehicle->typ->height & uheight )
         return terrainaccessible ( field, vehicle, uheight );
      else
         return 0;
   } else {
   /*
      int m1 = vehicle->weight();
      int mx = vehicle->weight();
      int b = 1;
      for (int c = 0; c <= 31; c++)
         if ( vehicle->loading[c] ) {
            b++;
            m1 += vehicle->loading[c]->weight();
            if ( vehicle->loading[c]->weight() > mx )
               mx = vehicle->loading[c]->weight();
         }
     */

      if (field->vehicle) {
         if (field->vehicle->color == vehicle->color) {
            if ( field->vehicle->vehicleLoadable ( vehicle, uheight ) )
               return 2;
            else
               if ( terrainaccessible ( field, vehicle, uheight ))
                  return 1;
               else
                  return 0;
         }
         else   ///////   keine eigene vehicle
           if ( terrainaccessible ( field, vehicle, uheight ) )
              if (vehicleplattfahrbar(vehicle,field))
                 return 2;
              else
                 if ((attackpossible28(field->vehicle,vehicle) == false) || (getdiplomaticstatus(field->vehicle->color) == capeace))
                   if ( terrainaccessible ( field, vehicle, uheight ) )
                      return 1;
                   else
                      return 0;

      }
      else {   // building
        if ((field->bdt & getTerrainBitType(cbbuildingentry) ).any() && field->building->vehicleLoadable ( vehicle, uheight ))
           return 2;
        else
           if (uheight >= chtieffliegend)
              return 1;
           else
              return 0;
      }
   }
   return 0;
}




void         generatemap( TerrainType::Weather*   bt,
                               int                xsize,
                               int                ysize)
{ 
   delete actmap;
   actmap = new tmap;
   for (int k = 1; k < 8; k++)
      actmap->player[k].stat = Player::computer;

   actmap->maptitle = "new map";

   actmap->allocateFields(xsize, ysize);

   if ( actmap->field== NULL)
      displaymessage ( "Could not generate map !! \nProbably out of enough memory !",2);

   for ( int l = 0; l < xsize*ysize; l++ ) {
      actmap->field[l].typ = bt;
      actmap->field[l].setparams();
      actmap->field[l].setMap( actmap );
   }

   actmap->_resourcemode = 1;
   actmap->playerView = 0;
}




int getxpos(void)
{ 
   return cursor.posx + actmap->xpos; 
} 


int getypos(void)
{ 
   return cursor.posy + actmap->ypos; 
} 




/*

word         beeline(integer      x1,
                       integer      y1,
                       integer      x2,
                       integer      y2)
{ 
  word         ll; 
  integer      max, min; 

   ll = maxmalq; 
      asm
      shl x1,1
      mov dx,y1
      and dx,1
      add x1,dx
      
      shl x2,1
      mov dx,y2
      and dx,1
      add x2,dx
      
      mov ax,x1
      sub ax,x2
      cmp ax,0
      jg @pos1
      neg ax
      @pos1:
      
      mov cx,y1
      sub cx,y2
      cmp cx,0
      jg @pos2
      neg cx
      @pos2:
      
      cmp ax,cx
      jna @aa
      {      cmp ax,0
      ja @weiter
      
      mov dx,y1
      and dx,1
      and y2,1
      cmp dx,y2
      jz @weiter
      inc ax     
      jmp @weiter
      
      @aa:
      xchg ax,cx
      {  cmp cx,0
      ja @weiter
      
      mov dx,y1
      and dx,1
      and y2,1
      cmp dx,y2
      jz @weiter
      inc cx
      @weiter:
      sub ax,cx
      shr ax,1
      mul ll
      shl cx,3
      add ax,cx
      mov ll,ax
   end;
   beeline:=ll;  

        x1:=x1 shl 1;
   x2:=x2 shl 1;
   if abs(x1 - x2) > abs(y1-y2) then begin
   max:=abs(x1 -x2 );
   if max = 0 then
   if y1 and 1 <> y2 and 1 then inc(max);
   min:=abs(y1-y2);
   end
   else begin
   min:=abs(x1 -x2 );
   if min = 0 then
   if y1 and 1 <> y2 and 1 then inc(min);
   max:=abs(y1-y2);
   end;
   ll:=min shl 3 + (max - min) * maxmalq shr 1;

}

*/


#include "movecurs.inc"      


pfield        getactfield(void)
{
   return getfield ( actmap->xpos + cursor.posx, actmap->ypos + cursor.posy ); 
} 



pfield        getfield(int          x,
                     int          y)
{ 
   if ((x < 0) || (y < 0) || (x >= actmap->xsize) || (y >= actmap->ysize))
      return NULL; 
   else
      return (   &actmap->field[y * actmap->xsize + x] );
}







void* uncompressedMinePictures[4] = { NULL, NULL, NULL, NULL };


void*      getmineadress(  int num , int uncompressed )
{ 
   if ( !uncompressed ) 
      return icons.mine[num-1]; 
   else {
      int type = num-1;
      if ( !uncompressedMinePictures[type] ) 
         uncompressedMinePictures[type] = uncompress_rlepict ( icons.mine[num-1] );

      return uncompressedMinePictures[type];
   }
} 



void         tcursor::init ( void )
{
   actpictwidth = 0;

   backgrnd = new int [  imagesize ( 0, 0, fieldxsize, fieldysize ) ];

   {
      tnfilestream iconl ( "markedfield.pcx", tnstream::reading );
      SDLmm::Surface markedField ( IMG_Load_RW( SDL_RWFromStream ( &iconl ), true ));

      cursor.markfield = convertSurface ( markedField, false );
   }

   {
      int w;
      tnfilestream stream ( "curshex.raw", tnstream::reading );
      stream.readrlepict ( &cursor.orgpicture, false, &w);
      void* newpic = uncompress_rlepict ( cursor.orgpicture );
      if ( newpic ) {
         delete[] cursor.orgpicture ;
         cursor.orgpicture = newpic;
      }
   }

   int h,w;
   getpicsize ( orgpicture, w, h );
   actpictwidth = w;
   picture = new char [ getpicsize2 ( orgpicture )];
   memcpy ( picture, orgpicture, getpicsize2 ( orgpicture ));

   posx = 0;
   posy = 0;
   oposx = 0;
   oposy = 0;
}

void  tcursor :: checksize ( void )
{
   int actwidth = idisplaymap.getfieldsizex();
   if ( actpictwidth != actwidth ) {
      delete[] picture;

      TrueColorImage* zimg = zoomimage ( orgpicture, idisplaymap.getfieldsizex(), idisplaymap.getfieldsizey(), pal, 0 );
      picture = convertimage ( zimg, pal ) ;
      delete zimg;

      int h;
      getpicsize ( picture, actpictwidth, h );

   }
}


void       tcursor::reset ( void )
{
   if (an) hide();
   posx = 0; 
   posy = 0; 
   oposx = 0; 
   oposy = 0; 

}


void       tcursor::getimg ( void )
{
     int xp = idisplaymap.getfieldposx( posx, posy );
     int yp = idisplaymap.getfieldposy( posx, posy );

     if (agmp-> linearaddress != hgmp-> linearaddress ) {
        npush(*agmp);
        *agmp = *hgmp;
        getimage(xp, yp, xp + idisplaymap.getfieldsizex(), yp + idisplaymap.getfieldsizey(), backgrnd );
        npop (*agmp);
     } else {
        getimage(xp, yp, xp + idisplaymap.getfieldsizex(), yp + idisplaymap.getfieldsizey(), backgrnd );
     } /* endif */
}

void       tcursor::putbkgr ( void )
{
     int xp = idisplaymap.getfieldposx( posx, posy );
     int yp = idisplaymap.getfieldposy( posx, posy );

     collategraphicoperations cgo ( xp, yp, xp + fieldxsize, yp + fieldysize );
     if (agmp-> linearaddress != hgmp-> linearaddress ) {
        npush( *agmp );
        *agmp = *hgmp;
        putimage(xp, yp, backgrnd );
        npop ( *agmp );
     } else {
        putimage(xp, yp, backgrnd );
     } /* endif */
}

void       tcursor::putimg ( void )
{
     int xp = idisplaymap.getfieldposx( posx, posy );
     int yp = idisplaymap.getfieldposy( posx, posy );

     checksize();

     collategraphicoperations cgo ( xp, yp, xp + fieldxsize, yp + fieldysize );
     if (agmp-> linearaddress != hgmp-> linearaddress ) {
        npush( *agmp );
        *agmp = *hgmp;
        putrotspriteimage(xp, yp, picture,color );
        npop ( *agmp );
     } else {
        putrotspriteimage(xp, yp, picture,color );
     } /* endif */
}


void         tcursor::setcolor( int col )
{ 
   color = col; 
} 


int        tcursor::checkposition ( int x, int y )
{
   if ( x >= actmap->xsize )
      x = actmap->xsize - 1;
   if ( y >= actmap->ysize )
      y = actmap->ysize - 1;

   int result = 0;
   int a = actmap->xpos; 
   int b = actmap->ypos; 
   int xss = idisplaymap.getscreenxsize();
   int yss = idisplaymap.getscreenysize();

   if ((x < a) || (x >= a + xss )) { 
      if (x >= xss / 2) 
         actmap->xpos = (x - xss / 2); 
      else 
         actmap->xpos = 0; 

      result++;
   } 

   if (y < b   ||   y >= b + yss  ) {
      if (y >= yss / 2) 
         actmap->ypos = (y - yss / 2); 
      else 
         actmap->ypos = 0; 
      if ( actmap->ypos & 1 )
         actmap->ypos--;

      result++;
   }

   if (actmap->xpos + xss > actmap->xsize) 
      actmap->xpos = actmap->xsize - xss ; 
   if (actmap->ypos + yss  > actmap->ysize) 
      actmap->ypos = actmap->ysize - yss ; 

   if ((actmap->xpos != a) || (actmap->ypos != b)) {
      displaymap(); 
      result++;
   }

   cursor.posx = x - actmap->xpos; 
   cursor.posy = y - actmap->ypos; 

   return result;
}


int tcursor::gotoxy(int x, int y, int disp)
{ 
   if ( x >= actmap->xsize )
      x = actmap->xsize-1;

   if ( y >= actmap->ysize )
      y = actmap->ysize-1;

   if ( x < 0 )
      x = 0;

   if ( y < 0 )
      y = 0;

   int res = 0;
   if ( disp || an )
      cursor.hide(); 

   if ( disp )
      res = checkposition ( x, y );
   else {
      cursor.posx = x - actmap->xpos; 
      cursor.posy = y - actmap->ypos; 
   }

   if ( disp )
      cursor.show(); 

   return res;
} 



void         tcursor::show(void)
{ 
   if ( !actmap || actmap->xsize == 0  || actmap->ysize == 0 )
      return;

   int ms = getmousestatus (); 
   if (ms == 2) 
       mousevisible(false); 

    
   int poschange = checkposition ( actmap->xpos + posx, actmap->ypos + posy );

   if (  an == false || poschange ) {
         oposx = posx;
         oposy = posy;

         getimg();
         putimg();
   } 
      
   an = true; 

   if (ms == 2)
      mousevisible(true);
}


void         tcursor::hide(void)
{
   int ms = getmousestatus();
   if (ms == 2) mousevisible(false);
   if ( an )
      putbkgr();
   an = false;
   if (ms == 2) mousevisible(true);
}



int         getdiplomaticstatus(int         b)
{
   if ( b & 7 )
     displaymessage("getdiplomaticstatus: \n parameter has to be in [0,8,16,..,64]",2);

   if ( b/8 == actmap->actplayer )
      return capeace;

   if ( b == 64 )  // neutral
      return capeace;

   char d = actmap->alliances[ b/8 ][ actmap->actplayer ] ;
   char e = actmap->alliances[ actmap->actplayer ][ b/8 ] ;

   if (  (d == capeace || d == canewsetwar1 || d == cawarannounce || d == capeace_with_shareview )
       &&(e == capeace || e == canewsetwar1 || e == cawarannounce || e == capeace_with_shareview))
      return capeace;
   else
      return cawar;
}


int        getdiplomaticstatus2(int    b, int    c)
{
   if ( (b & 7) || ( c & 7 ) )
      displaymessage("getdiplomaticstatus: \n parameters have to be in [0,8,16,..,64]",2);

   if ( b == c )
      return capeace;

   if ( b == 64 || c == 64 )  // neutral
      return capeace;


   char d = actmap->alliances [ b/8][ c/8 ];
   char e = actmap->alliances [ c/8][ b/8 ];

   if (  (d == capeace || d == canewsetwar1 || d == cawarannounce || d == capeace_with_shareview )
       &&(e == capeace || e == canewsetwar1 || e == cawarannounce || e == capeace_with_shareview))
      return capeace;
   else
      return cawar;
}










int isresourcenetobject ( pobjecttype obj )
{
   if ( obj->id == 3 || obj->id == 30 || obj->id == 4 )
      return 1;
   else
      return 0;
}






void         tcursor::display(void)
{ 
      hide(); 
      oposx = posx;
      oposy = posy;
      show(); 
} 


void         clearfahrspuren(void)
{ 
      if ((actmap->xsize == 0) || (actmap->ysize == 0)) 
        return;
      int l = 0; 
      for ( int y = 0; y < actmap->ysize ; y++) 
         for ( int x = 0; x < actmap->xsize ; x++) {
            pobject i = actmap->field[l].checkforobject ( fahrspurobject );
            if ( i ) 
               if ( actmap->getgameparameter ( cgp_fahrspur ) > 0 )
                  if ( i->time + actmap->getgameparameter ( cgp_fahrspur ) < actmap->time.turn() )
                     getfield ( x, y ) -> removeobject ( fahrspurobject );

            i = actmap->field[l].checkforobject ( eisbrecherobject );
            if ( i ) 
               if ( actmap->getgameparameter ( cgp_eis ) > 0 )
                  if ( i->time + actmap->getgameparameter ( cgp_eis ) < actmap->time.turn() )
                     getfield ( x, y ) -> removeobject ( eisbrecherobject );

            getfield ( x, y )->checkminetime ( actmap->time.turn() );
            l++;
         } 
} 



void         putbuilding( const MapCoordinate& entryPosition,
                         int          color,
                         pbuildingtype buildingtyp,
                         int          completion,
                         int          ignoreunits )
{ 
   if ( color & 7 )
      displaymessage("putbuilding muá eine farbe aus 0,8,16,24,.. ?bergeben werden !",2); 

   for ( int a = 0; a < 4; a++)
      for ( int b = 0; b < 6; b++ )
         if ( buildingtyp->getpicture ( BuildingType::LocalCoordinate( a, b ) ) ) {
            pfield field = actmap->getField( buildingtyp->getFieldCoordinate( entryPosition, BuildingType::LocalCoordinate(a,b) ));
            if (field == NULL) 
               return ;
            else {
               if ( field->vehicle && (!ignoreunits ) ) 
                  return;
               if (field->building != NULL)
                  return;
            }
         } 


   pbuilding gbde = new Building ( actmap , entryPosition, buildingtyp, color/8 );

   if (completion >= buildingtyp->construction_steps)
      completion = buildingtyp->construction_steps - 1;

   gbde->damage = 0;
   gbde->maxresearchpoints = 0;
   gbde->researchpoints = 0; 
   gbde->netcontrol = 0;
   gbde->connection = 0; 
   gbde->visible = true; 
   gbde->setCompletion ( completion );
}


void         putbuilding2( const MapCoordinate& entryPosition,
                           int         color,
                           pbuildingtype buildingtyp)
{ 
   if ( color & 7 )
      displaymessage("putbuilding muá eine farbe aus 0,8,16,24,.. ?bergeben werden !",2); 

   for ( int a = 0; a < 4; a++)
      for ( int b = 0; b < 6; b++ )
         if ( buildingtyp->getpicture ( BuildingType::LocalCoordinate( a, b ) ) ) {
            pfield field = actmap->getField( buildingtyp->getFieldCoordinate( entryPosition, BuildingType::LocalCoordinate(a,b) ));
            if (field == NULL)
               return ;
            else {
               if ( field->vehicle )
                  return;
            }
         }

   if ( !actmap->getField(entryPosition)->building ) {
      pbuilding gbde = new Building ( actmap, entryPosition, buildingtyp, color/8 );

      Resources maxplus;
      Resources actplus;
      Resources biplus;
      int maxresearch = 0;
      for ( tmap::Player::BuildingList::iterator i = actmap->player[color/8].buildingList.begin(); i != actmap->player[ color/8].buildingList.end(); i++ ) {
         pbuilding bld = *i;
         if ( bld->typ == gbde->typ  && bld != gbde ) {

            if ( bld->maxplus.energy > maxplus.energy )
               maxplus.energy = bld->maxplus.energy;

            if ( bld->maxplus.material > maxplus.material )
               maxplus.material = bld->maxplus.material;

            if ( bld->maxplus.fuel > maxplus.fuel )
               maxplus.fuel = bld->maxplus.fuel;


            if ( bld->bi_resourceplus.energy > biplus.energy )
               biplus.energy = bld->bi_resourceplus.energy;

            if ( bld->bi_resourceplus.material > biplus.material )
               biplus.material = bld->bi_resourceplus.material;

            if ( bld->bi_resourceplus.fuel > biplus.fuel )
               biplus.fuel = bld->bi_resourceplus.fuel;


            if ( bld->plus.energy > actplus.energy )
               actplus.energy = bld->plus.energy;

            if ( bld->plus.material > actplus.material )
               actplus.material = bld->plus.material;

            if ( bld->plus.fuel > actplus.fuel )
               actplus.fuel = bld->plus.fuel;


            if ( bld->maxresearchpoints > maxresearch )
               maxresearch = bld->maxresearchpoints;

         }
      }

      gbde->damage = 0;
      if ( actmap->_resourcemode == 1 ) {
         gbde->plus.energy = biplus.energy;
         gbde->plus.material = biplus.material;
         gbde->plus.fuel = biplus.fuel;
      } else {
         gbde->plus.energy = maxplus.energy;
         gbde->plus.material = maxplus.material;
         gbde->plus.fuel = maxplus.fuel;
      }
      gbde->maxplus.energy = maxplus.energy;
      gbde->maxplus.material = maxplus.material;
      gbde->maxplus.fuel = maxplus.fuel;
      gbde->bi_resourceplus.energy = biplus.energy;
      gbde->bi_resourceplus.material = biplus.material;
      gbde->bi_resourceplus.fuel = biplus.fuel;
      gbde->actstorage.fuel = 0;
      gbde->actstorage.material = 0;
      gbde->actstorage.energy = 0;
      gbde->maxresearchpoints = maxresearch;
      gbde->researchpoints = 0;
      gbde->netcontrol = 0;
      gbde->connection = 0;
      gbde->visible = true;
      gbde->setCompletion ( 0 );

   }
   else { 
      pbuilding gbde = actmap->getField(entryPosition)->building;
      if (gbde->getCompletion() < gbde->typ->construction_steps-1)
         gbde->setCompletion( gbde->getCompletion()+1 );

   } 
} 





void         resetallbuildingpicturepointers ( void )
{
   for (int s = 0; s < 9; s++)
      for ( tmap::Player::BuildingList::iterator i = actmap->player[s].buildingList.begin(); i != actmap->player[s].buildingList.end(); i++ )
         (*i)->resetPicturePointers ();
}


void     putstreets2  ( int      x1,
                        int      y1,
                        int      x2,
                        int      y2,
                        pobjecttype obj )
{

   int x = x1;
   int y = y1;

   pfield fld = getfield( x1, y1 );


   if ( !obj->buildable ( getfield(x2,y2)))
      return;

   if ( !obj->buildable ( fld ))
      return;
   
   getfield ( x1, y1 ) -> addobject ( obj );

   while ((x != x2) || (y != y2)) {
      int orgdir = getdirection ( x, y, x2, y2 );
      int dir = orgdir;
      int t = 0;
      int a, b;
      do {

         a = x;
         b = y;
         getnextfield ( a, b, dir );

         static int obstacle[8] = { -1, 1, -2, 2, -3, 3, -4, 4 };

         dir = orgdir + obstacle[t++];

      } while ( !obj->buildable ( getfield( a, b )) && (t < sidenum)  );

      if ( obj->buildable ( getfield( a, b ))) {
         getfield ( a, b ) -> addobject ( obj );
         x = a;
         y = b;
      } else 
         getnextfield ( x, y, orgdir );

   }

   calculateallobjects();
}



void checkobjectsforremoval ( void )
{
   for ( int y = 0; y < actmap->ysize; y++ )
      for ( int x = 0; x < actmap->xsize; x++ ) {
         pfield fld = getfield ( x, y );
         for ( tfield::ObjectContainer::iterator i = fld->objects.begin(); i != fld->objects.end();  )
            if ( i->typ->terrainaccess.accessible ( fld->bdt ) < 0 ) {
               fld->removeobject ( i->typ );
               i = fld->objects.begin();
            } else
               i++;
      }
}

void  checkunitsforremoval ( void )
{
   for ( int c=0; c<=8 ;c++ )
      for ( Player::VehicleList::iterator i = actmap->player[c].vehicleList.begin(); i != actmap->player[c].vehicleList.end();  ) {
          pvehicle eht = *i;
          pfield field = getfield(eht->xpos,eht->ypos);
          bool erase = false;

          ASCString reason;
          if (field->vehicle == eht) {
             if ( eht->height <= chfahrend )
                if ( eht->typ->terrainaccess.accessible ( field->bdt ) < 0 ) {
                   erase = true;
                   reason = "was swallowed by the ground";
                }
             if ( eht )
                if ( getmaxwindspeedforunit( eht ) < actmap->weather.wind[getwindheightforunit ( eht )].speed*maxwindspeed ) {
                   reason = "was blown away by the wind";
                   erase = true;
                }
          }
          if ( erase ) {
             ASCString ident = "The unit " + (*i)->getName() + " at position ("+strrr((*i)->getPosition().x)+"/"+strrr((*i)->getPosition().y)+") ";
             new Message ( ident+reason, actmap, 1<<(*i)->getOwner());
             Vehicle* pv = *i;
             actmap->player[c].vehicleList.erase ( i );
             delete pv;



             /* if the unit was a transport and had other units loaded, these units have been deleted as well.
                We don't know which elements of the container are still valid, so we start from the beginning again. */
             i = actmap->player[c].vehicleList.begin();
          } else
             i++;
      }
}


int  getwindheightforunit ( const pvehicle eht, int uheight )
{
   if ( uheight == -1 )
      uheight = eht->height;

   if ( uheight == chfliegend )
      return 1;
   else
      if ( uheight == chhochfliegend )
         return 2;
      else
         return 0;
}

int  getmaxwindspeedforunit ( const pvehicle eht )
{
   pfield field = getfield(eht->xpos,eht->ypos);
   if ( field->vehicle == eht) {
      if (eht->height >= chtieffliegend && eht->height <= chhochfliegend ) //    || ((eht->height == chfahrend) && ( field->typ->art & cbwater ))) ) 
         return eht->typ->movement[log2(eht->height)] * 256 ;

      if ( (field->bdt & getTerrainBitType(cbfestland)).none() && eht->height <= chfahrend && eht->height >= chschwimmend )
         return eht->typ->maxwindspeedonwater * maxwindspeed;
   }
   return maxint;
}




/*
int getcrc ( const pvehicletype fzt )
{
    if ( !fzt )
       return -1;

    Vehicletype fz = *fzt;
    fz.name = NULL;
    fz.description = NULL;
    fz.infotext = NULL;
    for ( int i = 0; i < 8; i++ ) {
       fz.picture[i] = NULL;
       fz.classnames[i] = NULL;
    }
    fz.buildicon = NULL;
    int terr = 0; // fz.terrainaccess->getcrc();
    fz.terrainaccess = NULL;
    
    int crcob = 0;
    if ( fz.objectsbuildablenum ) 
       crcob = crc32buf ( fz.objectsbuildableid, fz.objectsbuildablenum*4 );
    fz.objectsbuildableid = NULL;

    int crcbld = 0;
    if ( fz.buildingsbuildablenum ) 
       crcbld = crc32buf ( fz.buildingsbuildable, fz.buildingsbuildablenum*sizeof(tbuildrange) );
    fz.buildingsbuildable = NULL;


    int crcfz = 0;
    if ( fz.vehiclesbuildablenum ) 
       crcob = crc32buf ( fz.vehiclesbuildableid, fz.vehiclesbuildablenum*4 );
    fz.vehiclesbuildableid = NULL;

   int crctr = 0;
   if ( fz.terrainaccess ) 
      crctr = crc32buf ( fz.terrainaccess, sizeof ( *fz.terrainaccess ));
   
   int crcweap = crc32buf ( fz.weapons, sizeof ( *fz.weapons ));
   fz.weapons = NULL;

   memset ( &fz.oldattack, 0 , sizeof ( fz.oldattack ));
   
   for ( int j = 0; j < 8; j++ )
      fz.aiparam[j] = NULL;

   fz.terrainaccess = NULL;
   return crc32buf ( &fz, sizeof ( fz )) + crcob + crctr + crcfz + crcbld + terr + crcweap;
}

int getcrc ( const ptechnology tech )
{
  if ( !tech )
       return -1;

    ttechnology t = *tech;
    t.icon = NULL;
    t.infotext = NULL;
    t.name = NULL;
    for ( int i = 0; i < 6; i++ )
       if ( tech->requiretechnology )
          t.requiretechnologyid[i] = tech->requiretechnology[i]->id;
       else
          t.requiretechnologyid[i] = 0;

    t.pictfilename = NULL;
    
    return crc32buf ( &t, sizeof ( t )) ;
}

int getcrc ( const pobjecttype obj )
{
    if ( !obj )
       return -1;

    tobjecttype o = *obj;
    for ( int ww= 0; ww < cwettertypennum; ww++ )
       o.picture[ww] = NULL;

    int crc1;
    if ( o.movemalus_plus_count )
       crc1 = crc32buf ( o.movemalus_plus, o.movemalus_plus_count );
    else
       crc1 = 0;

    int crc2;
    if ( o.movemalus_abs_count )
       crc2 = crc32buf ( o.movemalus_abs, o.movemalus_abs_count );
    else
       crc2 = 0;

    o.movemalus_plus = NULL;
    o.movemalus_abs  = NULL;

    o.name = NULL;
    o.buildicon = NULL;
    o.removeicon = NULL;
    o.objectslinkable = NULL;
    o.oldpicture = NULL;
    o.dirlist = NULL;
    
    return crc32buf ( &o, sizeof ( o )) + crc1 + crc2;
}

int getcrc ( const pterraintype bdn )
{
    if ( !bdn )
       return -1;

    int crc = bdn->id;
    for ( int i = 0; i < cwettertypennum; i++ ) 
       if ( bdn->weather[i] ) {
          TerrainType::Weather b = *bdn->weather[i];
          for ( int j = 0; j < 8; j++) {
             b.picture[j] = NULL;
             b.direcpict[j]= NULL;
          }
          if ( b.movemaluscount )
             crc += crc32buf ( b.movemalus, b.movemaluscount );

          b.movemalus = NULL;
          b.terraintype = NULL;
          b.quickview = NULL;

          crc += crc32buf ( &b, sizeof ( b ));
      }
    
    return crc;
}


int getcrc ( const pbuildingtype bld )
{
    if ( !bld )
       return -1;

    Buildingtype b = *bld;
    for ( int i = 0; i < maxbuildingpicnum; i++ )
       for ( int j = 0; j < 4; j++ )
          for ( int k = 0; k < 6; k++ )
              for ( int w = 0; w < cwettertypennum; w++ )
		b.w_picture[w][i][j][k] = NULL;

    b.name = NULL;
    b.guibuildicon = NULL;
    b.terrain_access = NULL;
    
    return crc32buf ( &b, sizeof ( b ));
}
*/









//////  Functions, that belong to TYPEN.CPP , but need functions defined here ...













void tdrawline8 :: start ( int x1, int y1, int x2, int y2 )
{
   x1 += x1 + (y1 & 1);
   x2 += x2 + (y2 & 1);
   tdrawline::start ( x1, y1, x2, y2 );
}

void tdrawline8 :: putpix ( int x, int y )
{
       if ( (x & 1) == (y & 1) )
          putpix8( x/2, y );
}


void EllipseOnScreen :: paint ( void )
{
   if ( active )
      ellipse ( x1, y1, x2, y2, color, precision );
}



int getheightdelta ( int height1, int height2 )
{
   int ah = height1;
   int dh = height2;
   int hd = dh - ah;

   if ( ah >= 3 && dh <= 2 )
      hd++;
   if (dh >= 3 && ah <= 2 )
      hd--;

   return hd;
}

bool fieldvisiblenow( const pfield pe, int player )
{
  if ( player < 0 ) {
     #ifdef karteneditor
     return true;
     #else
     return false;
     #endif
  }

  if ( pe ) { 
      int c = (pe->visible >> ( player * 2)) & 3;
      #ifdef karteneditor
         c = visible_all;
      #endif

      if ( c < actmap->getInitialMapVisibility( player ) )
         c = actmap->getInitialMapVisibility( player );

      if (c > visible_ago) { 
         if ( pe->vehicle ) { 
            if ((c == visible_all) || (pe->vehicle->color / 8 == player ) || ((pe->vehicle->height >= chschwimmend) && (pe->vehicle->height <= chhochfliegend)))
               return true; 
         } 
         else 
            if (pe->building != NULL) { 
               if ((c == visible_all) || (pe->building->typ->buildingheight >= chschwimmend) || (pe->building->color == player*8)) 
                  return true; 
            } 
            else
               return true; 
      } 
   }
   return false; 
} 


VisibilityStates fieldVisibility( const pfield pe, int player )
{
  if ( pe && player >= 0 ) {
      VisibilityStates c = VisibilityStates((pe->visible >> ( player * 2)) & 3);
      #ifdef karteneditor
         c = visible_all;
      #endif

      if ( c < actmap->getInitialMapVisibility( player ) )
         c = actmap->getInitialMapVisibility( player );

      return c;
   } else
      return visible_not;
}

