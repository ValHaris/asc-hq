//     $Id: spfst.cpp,v 1.18 2000-01-24 17:35:47 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.17  2000/01/24 08:16:50  steb
//     Changes to existing files to implement sound.  This is the first munge into
//     CVS.  It worked for me before the munge, but YMMV :)
//
//     Revision 1.16  2000/01/07 13:20:05  mbickel
//      DGA fullscreen mode now working
//
//     Revision 1.15  2000/01/06 11:19:14  mbickel
//      Worked on the Linux-port again...
//
//     Revision 1.14  2000/01/04 19:43:53  mbickel
//      Continued Linux port
//
//     Revision 1.13  2000/01/01 19:04:19  mbickel
//     /tmp/cvsVhJ4Z3
//
//     Revision 1.12  1999/12/30 20:30:40  mbickel
//      Improved Linux port again.
//
//     Revision 1.11  1999/12/29 12:50:48  mbickel
//      Removed a fatal error message in GUI.CPP
//      Made some modifications to allow platform dependant path delimitters
//
//     Revision 1.10  1999/12/28 21:03:22  mbickel
//      Continued Linux port
//      Added KDevelop project files
//
//     Revision 1.9  1999/12/27 13:00:11  mbickel
//      new vehicle function: each weapon can now be set to not attack certain
//                            vehicles
//
//     Revision 1.8  1999/12/14 20:24:00  mbickel
//      getfiletime now works on containerfiles too
//      improved BI3 map import tables
//      various bugfixes
//
//     Revision 1.7  1999/12/07 22:13:27  mbickel
//      Fixed various bugs
//      Extended BI3 map import tables
//
//     Revision 1.6  1999/11/25 22:00:12  mbickel
//      Added weapon information window
//      Added support for primary offscreen frame buffers to graphics engine
//      Restored file time handling for DOS version
//
//     Revision 1.5  1999/11/23 21:07:35  mbickel
//      Many small bugfixes
//
//     Revision 1.4  1999/11/22 18:27:57  mbickel
//      Restructured graphics engine:
//        VESA now only for DOS
//        BASEGFX should be platform independant
//        new interface for initialization
//      Rewrote all ASM code in C++, but it is still available for the Watcom
//        versions
//      Fixed bugs in RLE decompression, BI map importer and the view calculation
//
//     Revision 1.3  1999/11/16 17:04:14  mbickel
//     Made ASC compilable for DOS again :-)
//     Merged all the bug fixes in that I did last week
//
//     Revision 1.2  1999/11/16 03:42:32  tmwilson
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
#ifdef _DOS_
#include <conio.h>
#endif



#if (__WATCOM_CPLUSPLUS__ >= 1100 )
  #define __use_STL_for_ASC__
#endif

#ifndef __WATCOM_CPLUSPLUS__
  #define __use_STL_for_ASC__
#endif

#ifdef __use_STL_for_ASC__
  #include <utility>
  #include <map.h>
#endif


#include "basestrm.h"
#include "tpascal.inc"
#include "misc.h"
#include "keybp.h"
#include "basegfx.h"
#include "newfont.h"
#include "typen.h"
#include "spfst.h"
#include "mousehnd.h"
#include "spfldutl.h"
#include "dlg_box.h"
#include "timer.h"
#include "loaders.h"
#include "stack.h"
#include "loadpcx.h"

#ifndef karteneditor
  #include "missions.h"
#endif

#include "dialog.h"




// #define showtempnumber

#define show2threatvalue   

// #define showmovetemp
int showresources = 0;


   #define stepsizex 2  
   #define stepsizey 4  
   #define fielddx 4    /*  verschiebung der ro ecke des bodentypes gegenÅber der vehicle etc  */ 
   #define fielddy 4  


   struct {
          int mx,my;
          int fx,fy;
          int stat;
          int oldx, oldy;
       } doubleclickparams ;


tmoveparams moveparams;
          

   int lockdisplaymap = 0;

   tcursor            cursor;
   tview             view;
   pmap              actmap;

  TerrainTypeVector terrain;
  TerrainTypeVector& getterraintypevector ( void ) 
  {
     return terrain;
  }

  VehicleTypeVector vehicletypes; 
  VehicleTypeVector& getvehicletypevector ( void ) 
  {
     return vehicletypes;
  }

  BuildingTypeVector buildingtypes;
  BuildingTypeVector& getbuildingtypevector ( void ) 
  {
     return buildingtypes;
  }

  ObjectTypeVector objecttypes;
  ObjectTypeVector& getobjecttypevector ( void ) 
  {
     return objecttypes;
  }

  dynamic_array<ptechnology> technology; 



  int terraintypenum = 0;
  int vehicletypenum = 0;
  int buildingtypenum = 0;
  int technologynum = 0;
  int objecttypenum = 0;


   boolean godview, tempsvisible; 
   ffonts schriften; 
   tstreet streets; 
   int lasttick;   /*  fÅr paintvehicleinfo  */ 

   tpaintmapborder* mapborderpainter = NULL;


   pweapdist weapdist = NULL; 


#ifdef __use_STL_for_ASC__
  #ifdef __WATCOM_CPLUSPLUS__
   typedef less<int> lessint;
   map< int, pterraintype, lessint>  terrainmap;
   map< int, pobjecttype,  lessint>  objectmap;
   map< int, pvehicletype,  lessint>  vehiclemap;
   map< int, pbuildingtype, lessint>  buildingmap;
   map< int, ptechnology, lessint>  technologymap;
  #else
   map< int, pterraintype>  terrainmap;
   map< int, pobjecttype>  objectmap;
   map< int, pvehicletype>  vehiclemap;
   map< int, pbuildingtype>  buildingmap;
   map< int, ptechnology>  technologymap;
  #endif
#endif

pterraintype getterraintype_forid ( int id, int crccheck )
{
  #ifdef __use_STL_for_ASC__
   return terrainmap[id];
  #else
   for ( int i = 0; i < terraintypenum; i++ )
      if ( terrain[i]->id == id )
         return terrain[i];
   return NULL;
  #endif
}
pobjecttype getobjecttype_forid ( int id, int crccheck  )
{
  #ifdef __use_STL_for_ASC__
   return objectmap[id];
  #else
   for ( int i = 0; i < objecttypenum; i++ )
      if ( objecttypes[i]->id == id )
         return objecttypes[i];
   return NULL;
  #endif
}
pvehicletype getvehicletype_forid ( int id, int crccheck  )
{
  #ifdef __use_STL_for_ASC__
   return vehiclemap[id];
  #else
   for ( int i = 0; i < vehicletypenum; i++ )
      if ( vehicletypes[i]->id == id )
         return vehicletypes[i];
   return NULL;
  #endif
}
pbuildingtype getbuildingtype_forid ( int id, int crccheck  )
{
  #ifdef __use_STL_for_ASC__
   return buildingmap[id];
  #else
   for ( int i = 0; i < buildingtypenum; i++ )
      if ( buildingtypes[i]->id == id )
         return buildingtypes[i];
   return NULL;
  #endif
}
ptechnology gettechnology_forid ( int id, int crccheck  )
{
  #ifdef __use_STL_for_ASC__
   return technologymap[id];
  #else
   for ( int i = 0; i < technologynum; i++ )
      if ( technology[i]->id == id )
         return technology[i];
   return NULL;
  #endif
}


pterraintype getterraintype_forpos ( int pos, int crccheck  )
{
   if ( pos < terraintypenum )
      return terrain[pos];
   else
      return NULL;
}
pobjecttype getobjecttype_forpos ( int pos, int crccheck  )
{
   if ( pos < objecttypenum )
      return objecttypes[pos];
   else
      return NULL;
}
pvehicletype getvehicletype_forpos ( int pos, int crccheck  )
{
   if ( pos < vehicletypenum )
      return vehicletypes[pos];
   else
      return NULL;
}
pbuildingtype getbuildingtype_forpos ( int pos, int crccheck  )
{
   if ( pos < buildingtypenum )
      return buildingtypes[pos];
   else
      return NULL;
}
ptechnology gettechnology_forpos ( int pos, int crccheck  )
{
   if ( pos < technologynum )
      return technology[pos];
   else
      return NULL;
}

void addterraintype ( pterraintype bdt )
{
   if ( bdt ) {
      terrain[ terraintypenum++] = bdt;

      #ifdef __use_STL_for_ASC__
      terrainmap[bdt->id] = bdt;
      #endif
   }
}
void addobjecttype ( pobjecttype obj )
{
   if ( obj ) {
      objecttypes[ objecttypenum++] = obj;

      #ifdef __use_STL_for_ASC__
      objectmap[obj->id] = obj;
      #endif
   }
}
void addvehicletype ( pvehicletype vhcl )
{
   if ( vhcl ) {
      vehicletypes[ vehicletypenum++] = vhcl;

      #ifdef __use_STL_for_ASC__
      vehiclemap[vhcl->id] = vhcl;
      #endif
   }
}
void addbuildingtype ( pbuildingtype bld )
{
   if ( bld ) {
      buildingtypes[ buildingtypenum++] = bld;

      #ifdef __use_STL_for_ASC__
      buildingmap[bld->id] = bld;
      #endif
   }
}
void addtechnology ( ptechnology tech )
{
   if ( tech ) {
      technology[ technologynum++] = tech;

      #ifdef __use_STL_for_ASC__
      technologymap[tech->id] = tech;
      #endif
   }
}







#ifdef _NOASM_

int  rol ( int valuetorol, int rolwidth )
{
   int newvalue = valuetorol << rolwidth;
   newvalue |= valuetorol >> ( 32 - rolwidth );
   return newvalue;
}


void setvisibility ( word* visi, int valtoset, int actplayer )
{
   int newval = (valtoset ^ 3) << ( 2 * actplayer );
   int oneval = 3 << ( 2 * actplayer );

   int vis = *visi;
   vis |= oneval;
   vis ^= newval;
   *visi = vis;
}

void copyvfb2displaymemory_zoom ( void* parmbuf )
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

   copySurface2screen();
   agmp->directscreenaccess = tempdirectscreenaccess;
}

#endif


void         initmap( void )
{ 
   actmap->time.a.turn = 1;
   actmap->time.a.move = 0;

  #ifndef karteneditor
   for ( int j = 0; j < 8; j++ )
      quedevents[j] = 1;
   getnexteventtime();
  #endif
   
  #ifdef logging
   logtofile("initmap anfang");
       {
           for ( int jj = 0; jj < 8; jj++ ) {
           char tmpcbuf[200];
           sprintf(tmpcbuf,"humanplayername; address is %x", actmap->humanplayername[jj]);
           logtofile ( tmpcbuf );
           }
       }
  #endif
  
   actmap->levelfinished = false; 
   memset ( (void*) &moveparams, 0, sizeof ( moveparams ));
   actmap->firsteventpassed = NULL;
   actmap->network = NULL;
   int num = 0;
   int cols[72];
   memset ( cols, 0, sizeof ( cols ));
   int i;
   for ( i = 0; i < 8 ; i++) {
      if ( actmap->player[i].existent ) {
         num++;
         cols[ i * 8 ] = 1;
      } else
         cols[ i * 8 ] = 0;

      actmap->cursorpos.position[ i ].sx = 0;
      actmap->cursorpos.position[ i ].sy = 0;

   }
      
   #ifdef logging
   logtofile("initmap mitte");
       {
           for ( int jj = 0; jj < 8; jj++ ) {
           char tmpcbuf[200];
           sprintf(tmpcbuf,"humanplayername; address is %x", actmap->humanplayername[jj]);
           logtofile ( tmpcbuf );
           }
       }
  #endif     


   i = 0;                                                                        
   int sze = actmap->xsize * actmap->ysize;
   do {
      if ( actmap->field[i].vehicle ) 
         if ( cols[ actmap->field[i].vehicle->color] ) {
            actmap->cursorpos.position[ actmap->field[i].vehicle->color / 8 ].cx = actmap->field[i].vehicle->xpos;
            actmap->cursorpos.position[ actmap->field[i].vehicle->color / 8 ].cy = actmap->field[i].vehicle->ypos;
            num--;
            cols[ actmap->field[i].vehicle->color] = 0;
         }

      if ( actmap->field[i].building && actmap->field[i].building->color < 64 ) 
         if ( cols[ actmap->field[i].building->color] ) {
            actmap->cursorpos.position[ actmap->field[i].building->color / 8 ].cx = actmap->field[i].building->xpos;
            actmap->cursorpos.position[ actmap->field[i].building->color / 8 ].cy = actmap->field[i].building->ypos;
            num--;
            cols[ actmap->field[i].building->color] = 0;
         }
      i++;
   } while ( num   &&   i <= sze ); /* enddo */
   i = 0;
   while ( !actmap->player[i].existent )
      i++;


   for ( int n = 0; n< 8; n++ ) {
      actmap->bi_resource[n].a.energy = 0;
      actmap->bi_resource[n].a.material = 0;
      actmap->bi_resource[n].a.fuel = 0;
     #ifdef sgmain
      if ( actmap->resourcemode == 1 )
         for ( pbuilding bld = actmap->player[n].firstbuilding; bld ; bld = bld->next ) {
             actmap->bi_resource[n].a.energy += bld->actstorage.a.energy;
             bld->actstorage.a.energy = 0;

             actmap->bi_resource[n].a.fuel += bld->actstorage.a.fuel;
             bld->actstorage.a.fuel = 0;
   
         } /* endfor */
     #endif 
   }

   #ifndef karteneditor
   actmap->actplayer = -1;
   #else
   actmap->actplayer = 0;
   #endif

} 



boolean   weapexist( const pvehicle     eht)
{ 
  byte          b;

   if (eht->typ->weapons->count > 0) 
      for (b = 0; b < eht->typ->weapons->count ; b++) 
         if (eht->typ->weapons->weapon[b].shootable() ) 
            if (eht->typ->weapons->weapon[b].offensive() ) 
               if (eht->ammo[b] ) 
                  return true; 
               
    return false;
} 




int          getdirection(    int      x1,
                              int      y1,
                              int      x2,
                              int      y2)

{ 
  #ifdef HEXAGON 
    int a;
    int dx = (2 * x2 + (y2 & 1)) - (2 * x1 + (y1 & 1)); 
    int dy = y2 - y1; 

    if (dx < 0) 
       if (dy < 0) 
          a = 5; 
       else 
          a = 4; 
    else 
       if (dx > 0) 
          if (dy < 0) 
             a = 1; 
          else 
             a = 2; 
       else 
          if (dy < 0) 
             a = 0; 
          else 
             a = 3; 
    return a; 


  #else
    int      a; 
    int      dx, dy; 
  
     dx = (2 * x2 + (y2 & 1)) - (2 * x1 + (y1 & 1)); 
     dy = y2 - y1; 
     if (dx < 0) 
        if (dy < 0) 
           a = 7; 
        else 
           if (dy == 0) 
              a = 6; 
           else 
              a = 5; 
     else 
        if (dx > 0) 
           if (dy < 0) 
              a = 1; 
           else 
              if (dy == 0) 
                 a = 2; 
              else 
                 a = 3; 
        else 
           if (dy < 0) 
              a = 0; 
           else 
              a = 4; 
     return a; 
   #endif
} 



void tweapdist::loaddata(void)
{ 
   tnfilestream stream ( "weapons.dat", 1 );
   stream.readdata ( &data, sizeof( data ));
} 

/*
byte       tweapdist::getweapstrength2(word         typ,
                                       word         pos,
                                       byte         mindist,
                                       byte         maxdist)
{ 
  byte         translat[31]  = { 6, 255, 1, 3, 2, 4, 0, 5, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255}; 

  byte         newmaxdist; 
  byte         distdiff; 
  int      w; 
  word         v; 
  byte         tp; 

   tp = translat[ log2 (typ & cwweapon) ];
   distdiff = maxdist - mindist; 
   if (distdiff < 25) 
      newmaxdist = maxdist + 16 / (15 * (distdiff - 4) / 20 + 1); 
   else 
      newmaxdist = maxdist; 
   w = int(255) * int(pos - mindist * 16) / ((newmaxdist - mindist) * 16); 
   if (w < 0) 
      v = 0; 
   else 
      if (w > 0xFFFF )
         v = 0xFFFF ;
      else 
         v = w; 
   if ((tp < 0) || (tp >= 6) || (w < 0) || (w > 255)) {
      displaymessage("tweapdist.getweapstrength2: invalid values: \n tp = %d ; w = %d ",1, tp, w);
      if (tp >= 6)
         tp = 5;
      if (w > 255) 
         w = 255; 
      if (w < 0) 
         w = 0; 
   } 
   return data[tp][w];
} 

*/

char tweapdist::getweapstrength ( const SingleWeapon* weap, int dist, int attacker_height, int defender_height, int reldiff  )
{
  byte         translat[31]  = { 6, 255, 1, 3, 2, 4, 0, 5, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                                 255, 255, 255, 255, 255, 255}; 

   int typ = translat[ weap->getScalarWeaponType() ];



   if ( weap->maxdistance == 0 )
      return 0;

   if ( weap->minstrength == 0 )
      return 0;

   if ( reldiff == -1) {
      if ( dist < weap->mindistance || dist > weap->maxdistance ) {
         displaymessage("tweapdist::getweapstrength: invalid range: \n min = %d ; max = %d ; req = %d ",1, weap->mindistance, weap->maxdistance, dist);
         return 0;
      }
      reldiff = 255 * (dist - weap->mindistance) / ( weap->maxdistance - weap->mindistance) ;
   }

   int minstrength = 255 - 255 * weap->minstrength / weap->maxstrength;

   int relstrength = 255 - ( 255 - data[typ][reldiff] ) * minstrength / ( 255 - data[typ][255] );
                                 
   if ( attacker_height != -1 && defender_height!= -1 ) {
      int hd = getheightdelta ( log2 ( attacker_height ), log2 ( defender_height ));
      return relstrength * weap->efficiency[6+hd] / 100;
   } else
      return relstrength;
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

/*
byte       tweapdist::getweapstrength(word         typ,
                                      byte         pos,
                                      byte         mindist,
                                      byte         maxdist)
{ 
   return getweapstrength2(typ,pos * 16,mindist,maxdist);
} 
*/





int  resizemap( int top, int bottom, int left, int right )  // positive: larger
{ 
  if ( !top && !bottom && !left && !right )
     return 0;

  if ( -(top + bottom) > actmap->ysize )
     return 1;

  if ( -(left + right) > actmap->xsize )
     return 2;

  if ( bottom & 1 || top & 1 )
     return 3;

  int ox1, oy1, ox2, oy2;

  if ( top < 0 ) {
     for ( int x = 0; x < actmap->xsize; x++ )
        for ( int y = 0; y < -top; y++ )
           getfield(x,y)->deleteeverything();

     oy1 = -top;
  } else
     oy1 = 0;

  if ( bottom < 0 ) {
     for ( int x = 0; x < actmap->xsize; x++ )
        for ( int y = actmap->ysize+bottom; y < actmap->ysize; y++ )
           getfield(x,y)->deleteeverything();

     oy2 = actmap->ysize + bottom;
  } else
     oy2 = actmap->ysize;
   
  if ( left < 0 ) {
     for ( int x = 0; x < -left; x++ )
        for ( int y = 0; y < actmap->ysize; y++ )
           getfield(x,y)->deleteeverything();
     ox1 = -left;
  } else
     ox1 = 0;

  if ( right < 0 ) {
     for ( int x = actmap->xsize+right; x < actmap->xsize; x++ )
        for ( int y = 0; y < actmap->ysize; y++ )
           getfield(x,y)->deleteeverything();
     ox2 = actmap->xsize + right;
  } else
     ox2 = actmap->xsize;


  int newx = actmap->xsize + left + right;
  int newy = actmap->ysize + top + bottom;

  pfield newfield = new tfield [ newx * newy ];

  int x;
  for ( x = ox1; x < ox2; x++ )
     for ( int y = oy1; y < oy2; y++ ) {
        pfield org = getfield ( x, y );
        pfield dst = &newfield[ (x + left) + ( y + top ) * newx];
        *dst = *org;
     }

  tfield defaultfield;
  memset ( &defaultfield, 0, sizeof ( defaultfield ));
  defaultfield.typ = getterraintype_forid ( 30 )->weather[0];

  for ( x = 0; x < left; x++ )
     for ( int y = 0; y < newy; y++ )
        newfield[ x + y * newx ] = defaultfield;

  for ( x = actmap->xsize + left; x < actmap->xsize + left + right; x++ )
     for ( int y = 0; y < newy; y++ )
        newfield[ x + y * newx ] = defaultfield;


  int y;
  for ( y = 0; y < top; y++ )
     for ( int x = 0; x < newx; x++ )
        newfield[ x + y * newx ] = defaultfield;

  for ( y = actmap->ysize + top; y < actmap->ysize + top + bottom; y++ )
     for ( int x = 0; x < newx; x++ )
        newfield[ x + y * newx ] = defaultfield;

  calculateallobjects();
  for ( int p = 0; p < newx*newy; p++ )
     newfield[p].setparams();

  delete[] actmap->field;
  actmap->field = newfield;
  actmap->xsize = newx;
  actmap->ysize = newy;

  if (actmap->xpos + idisplaymap.getscreenxsize() > actmap->xsize) 
     actmap->xpos = actmap->xsize - idisplaymap.getscreenxsize() ; 
  if (actmap->ypos + idisplaymap.getscreenysize()  > actmap->ysize) 
     actmap->ypos = actmap->ysize - idisplaymap.getscreenysize() ; 


  return 0;

/*
  integer      ox, oy, x, y; 
  pfield        fld; 
  int      l, m; 

   ox = actmap->xsize; 
   oy = actmap->ysize; 
   for (x = 0; x <= ox - 1; x++) { 
      for (y = 0; y <= oy - 1; y++) { 
         if ((x >= xsize) || (y >= ysize)) { 
            fld = getfield(x,y); 
            if (fld->vehicle != NULL) 
               removevehicle(&fld->vehicle); 
            if (fld->building != NULL) 
               removebuilding(&fld->building); 
         } 
      } 
   } 
   for (x = 0; x <= ox - 1; x++) { 
      for (y = 0; y <= oy - 1; y++) { 
         l = y * ox + x; 
         m = y * ysize + x; 
//         actmap->field[m >> 10][m & 1023] = actmap->field[l >> 10][l & 1023]; 
      } 
   } 
   actmap->xsize = xsize; 
   actmap->ysize = ysize; 
   */
} 




#ifdef HEXAGON
  byte         ccmpheighchangemovedir[6]  = {0, 1, 5, 2, 4, 3 }; 
  
  void         getnextfielddir(int&       x,
                               int&       y,
                               byte       direc,
                               byte       sdir)
  { 
     int newdir = ccmpheighchangemovedir[direc] + sdir;
     if ( newdir >= 6 )
        newdir -= 6;

     getnextfield( x, y, newdir ); 
  } 

#else
  byte         ccmpheighchangemovedir[8]  = {0, 1, 7, 2, 6, 3, 5, 4}; 
  
  
  void         getnextfielddir(int&       x,
                               int&       y,
                               byte       direc,
                               byte       sdir)
  { 
     getnextfield( x, y, (ccmpheighchangemovedir[direc] + sdir) & 7); 
  } 

#endif




void         getnextfield(int&       x,
                          int&       y,
                          byte       direc)
{ 
   switch (direc) {
      
    #ifdef HEXAGON
      case 0: y-=2   ;                      /*  oben  */
              break;
      
      case 1: if ((y & 1) == 1) x+=1;        /*  rechts oben  */
              y-=1;
              break;
      
      case 2: if ((y & 1) == 1) x+=1;        /*  rechts unten  */
              y+=1;
              break;
      
      case 3: y+=2;                          /*  unten  */
              break;
      
      case 4: if ((y & 1) == 0) x-=1;        /*  links unten  */
              y+=1;
              break;
      
      case 5: if ((y & 1) == 0) x-=1;        /*  links oben  */
              y-=1;
              break;
              
    #else
    
      case 0: y-=2   ;                      /*  oben  */
              break;
      
      case 1: if ((y & 1) == 1) x+=1;        /*  rechts oben  */
              y-=1;
              break;
      
      case 2: x+=1;                        /*  rechts  */
              break;
      
      case 3: if ((y & 1) == 1) x+=1;        /*  rechts unten  */
              y+=1;
              break;
      
      case 4: y+=2;                          /*  unten  */
              break;
      
      case 5: if ((y & 1) == 0) x-=1;        /*  links unten  */
              y+=1;
              break;
      
      case 6: x-=1;                        /*  links  */
              break;
      
      case 7: if ((y & 1) == 0) x-=1;        /*  links oben  */
              y-=1;
              break;
   #endif
   } 
} 



byte      vehiclegeladen( pvehicle    eht)
{ 
  byte         a, b; 
  
   a = 0; 
   if ((eht == NULL) || (eht->typ == NULL)) { 
      return( 0 );
   } 
   if (eht->typ->loadcapacity == 0)  
      return( 0 );

   for (b = 0; b <= 31; b++) 
      if ( eht->loading[b] ) 
         a++; 
   return a; 
} 


byte         vehiclegeparkt(pbuilding    eht)
{ 
  byte         a, b; 

   a = 0; 
   if ((eht == NULL) || (eht->typ == NULL))  
      return ( 0 );

   if (eht->typ->loadcapacity == 0) 
      return ( 0 );

   for (b = 0; b <= 31; b++) 
      if (eht->loading[b] ) 
         a++; 
   return a; 
} 


boolean      vehicleplattfahrbar( const pvehicle     vehicle,
                                  const pfield        field)
{ 
   return false;
/*
   if (vehicle == NULL)  
      return ( false );
   if (field == NULL) 
      return ( false );
   if (field->vehicle == NULL) 
      return ( false );

   if ((vehicle->color != field->vehicle->color) && 
      (vehicle->height == chfahrend) && 
      (field->vehicle->height == chfahrend) && 
      (field->vehicle->functions & cftrooper) && 
      (vehicle->weight() >= fusstruppenplattfahrgewichtsfaktor * field->vehicle->weight()))
      return ( true ); 
   return ( false );
*/
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
   if ( uheight >= chtieffliegend) 
      return 2; 
   else {
        if ( uheight == chtiefgetaucht ) 
           if ( field->typ->art & cbwater3 )
              return 2; 
           else
              return -1;
        else
           if ( uheight == chgetaucht ) 
              if ( field->typ->art & ( cbwater3 | cbwater2 ))
                 return 2; 
              else
                 return -2;
           else {
              if ( vehicle->typ->terrainaccess->accessible ( field->bdt ) > 0 )
                 return 2;
              else
                 return -3;
            }
   }
}


byte         fieldaccessible( const pfield        field,
                            const pvehicle     vehicle,
                            int  uheight )
{ 
  if ( uheight == -1 )
     uheight = vehicle->height;

//   int c = ((field->visible >> (actmap->actplayer << 1)) & 3);
   int c = ((field->visible >> (vehicle->color / 4 )) & 3);
   if ( godview ) 
       c = visible_all; 

   if (field == NULL) 
     return 0;

   if (c == visible_not)
      return 0;

   if ( c == visible_all)
      if ( field->object && field->object->mine ) 
         if ((vehicle->height <= chfahrend)) 
            return 0;

   if ((field->vehicle == NULL) && (field->building == NULL)) { 
      if ( uheight != vehicle->height )
         return 0;
      else
         return terrainaccessible ( field, vehicle );
   } 
   else { 
      int m1 = vehicle->weight(); 
      int mx = vehicle->weight(); 
      int b = 1; 
      if ( vehicle->typ->loadcapacity > 0) 
         for (int c = 0; c <= 31; c++) 
            if ( vehicle->loading[c] ) { 
               b++;
               m1 += vehicle->loading[c]->weight();
               if ( vehicle->loading[c]->weight() > mx )
                  mx = vehicle->loading[c]->weight();
            } 


      if (field->building == NULL) {   
         if (field->vehicle->color == actmap->actplayer << 3) { 
            int ldbl = field->vehicle->vehicleloadable ( vehicle, uheight );
            if ( ldbl )
               return 2;
            else 
               if ( uheight != vehicle->height )
                  return 0;
               else
                  if ( terrainaccessible ( field, vehicle ))
                     return 1; 
                  else 
                     return 0; 
         } 
         else   ///////   keine eigene vehicle
            if ( uheight != vehicle->height )
               return 0;
            else
               if ( terrainaccessible ( field, vehicle ) )
                  if (vehicleplattfahrbar(vehicle,field)) 
                     return 2; 
                  else 
                     if ((attackpossible28(field->vehicle,vehicle) == false) || (getdiplomaticstatus(field->vehicle->color) == capeace)) 
                       if ( terrainaccessible ( field, vehicle ) )
                          return 1; 
                       else 
                          return 0; 
             
      } 
      else {   /*  gebÑude  */ 
        if ((field->bdt & cbbuildingentry) && field->building->vehicleloadable ( vehicle, uheight ))
           return 2; 
        else 
           if (vehicle->height >= chtieffliegend) 
              return 1;
           else 
              return 0;
      }
   } 
   return 0;
} 


void         generate_vehicle(pvehicletype fztyp,
                             byte         col,
                             pvehicle &   vehicle)
{ 
   if ( col > 8 )
      displaymessage ( "internal error !\ngenerate_vehicle :: col must be between 0 and 8 !", 1 );

   vehicle = new tvehicle;

  vehicle->color = col * 8 ;

   vehicle->typ = fztyp;

   actmap->chainunit ( vehicle );

   vehicle->damage = 0;
   vehicle->fuel = 0;
   vehicle->experience = 0; 
   vehicle->height = 1 << log2( fztyp->height ); 
   vehicle->attacked = false; 
   vehicle->direction = 0; 
   vehicle->connection = 0; 
   vehicle->energy = 0;
   vehicle->material = 0; 
//   vehicle->threats = 0; 
//   vehicle->order = 0; 
   vehicle->xpos = getxpos();
   vehicle->ypos = getypos();
   vehicle->generatoractive = 0;
   actmap->unitnetworkid++;
   vehicle->networkid = actmap->unitnetworkid; 

   
//   vehicle->completethreatvalue = 0; 
//   vehicle->completethreatvaluesurr = 0; 
   vehicle->name = NULL;
   vehicle->klasse = 0;

   vehicle->functions = vehicle->typ->functions & vehicle->typ->classbound[vehicle->klasse].vehiclefunctions;
   vehicle->armor = vehicle->typ->armor * vehicle->typ->classbound[vehicle->klasse].armor / 1024;

//   memset( &vehicle->threatvalue, 0, sizeof(vehicle->threatvalue)); 

   if ( vehicle->typ->weapons->count ) { 
      int m;
      for (m = 0; m < vehicle->typ->weapons->count ; m++) { 
         vehicle->weapstrength[m] = vehicle->typ->weapons->weapon[m].maxstrength * vehicle->typ->classbound[vehicle->klasse].weapstrength[vehicle->typ->weapons->weapon[m].getScalarWeaponType()] / 1024;
         vehicle->ammo[m] = 0;
      } 

      for ( int k = m + 1; k <= 7; k++) 
         vehicle->ammo[k] = 0; 
   } 
   
   if (vehicle->typ->height & chfahrend ) 
      vehicle->height = chfahrend; 
   else 
      if (vehicle->typ->height & chschwimmend )
         vehicle->height = chschwimmend;
     
   vehicle->movement = vehicle->typ->movement[log2(vehicle->height)];  
} 


void         generatevehicle_ka ( pvehicletype fztyp,
                                  byte         col,
                                  pvehicle &   vehicle)
{
   generate_vehicle ( fztyp, col, vehicle );
   vehicle->fuel = fztyp->tank;
   vehicle->material = fztyp->material;
   vehicle->energy = 0;
   vehicle->generatoractive = 0;

   for ( int m = 0; m < vehicle->typ->weapons->count ; m++) {
      vehicle->ammo[m] = fztyp->weapons->weapon[m].count;
      vehicle->weapstrength[m] = fztyp->weapons->weapon[m].maxstrength;
   }
   vehicle->armor = vehicle->typ->armor;
   vehicle->klasse = 255;
}



void  stu_height ( pvehicle vehicle )
{
   char l;
   pfield fld = getfield ( vehicle->xpos, vehicle->ypos );

   vehicle->height = chfahrend;

   for (l=chsatellit; l> chfahrend ;  ) {
      if (vehicle->typ->height & l )
         vehicle->height = l;
      l>>=1;
   } /* endfor */

   for (l=chtiefgetaucht; l<= chschwimmend ;  ) {
      if (vehicle->typ->height & l )
        if (vehicle->typ->terrainaccess->accessible (  fld->bdt ) > 0 )
           vehicle->height = l;
      l<<=1;
   }

   if (vehicle->typ->height & chfahrend) 
      if (vehicle->typ->terrainaccess->accessible ( fld->bdt ) > 0 )
         vehicle->height = chfahrend;

   vehicle->movement = vehicle->typ->movement[log2( vehicle->height )];
}


void         generatemap( const pwterraintype   bt,
                               int                xsize,
                               int                ysize)
{ 
   tfield    leerfield;

   erasemap();
   actmap = new tmap;
   memset( actmap, 0, sizeof( *actmap ));
   actmap->xsize = xsize; 
   actmap->ysize = ysize; 
   actmap->campaign = NULL;
   for (int k = 1; k < 8; k++) 
      actmap->player[k].stat = ps_computer;

   actmap->title = strdup( "random map" );
   actmap->codeword[0] = 0;

   actmap->field = new tfield[ xsize * ysize];
   if ( actmap->field== NULL)
      displaymessage ( "Could not generate map !! \nProbably out of enough memory !",2);

   leerfield.bdt.set ( 0 );
   leerfield.typ = bt; 
   leerfield.picture = NULL;
   leerfield.vehicle = NULL;
   leerfield.building = NULL; 
   leerfield.a.temp = 0; 
   leerfield.a.temp2 = 0; 
   leerfield.visible = 0; 
   leerfield.direction = 0; 
   leerfield.fuel = 0;
   leerfield.material = 0;
   leerfield.object = NULL;
   leerfield.resourceview = NULL;
   leerfield.connection = 0;
   leerfield.setparams ();
            
   {
      int l = 0; 
      for ( int j = 0; j < ysize ; j++) { 
         for ( int i = 0; i < xsize ; i++) { 
            memcpy( &actmap->field[l], &leerfield, sizeof (leerfield) );
            l++;
         } 
      } 
   }

   memset ( &actmap->alliances, 0, sizeof ( actmap->alliances ));
   int i;
   for ( i = 0; i < 7; i++ )
      for ( int j = 0; j < 7; j++ )
         actmap->alliances[i][j] = cawar;

   /*
   for ( int j = 0; j < 8; j++) 
      actmap->player[j].alliance = j;
   */
   
   checkplayernames ();


   for ( i = 0; i < gameparameternum; i++ )
      actmap->gameparameter[i] = gameparameterdefault[i];

   #ifdef HEXAGON
   actmap->resourcemode = 1;
   #else
   actmap->resourcemode = 0;
   #endif

} 






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


void         initspfst( int x , int y)
{
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
   /* godview = true; */

   if ( actmap ) {
      actmap->actplayer = 0;
      actmap->xsize = 0;
      actmap->ysize = 0;
      actmap->field = NULL;
      for (i = 0; i <= 7; i++) {
          actmap->player[i].existent = false; 
          actmap->player[i].firstvehicle = NULL; 
      }
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

word         getxpos(void)
{ 
   return cursor.posx + actmap->xpos; 
} 


word         getypos(void)
{ 
   return cursor.posy + actmap->ypos; 
} 


int beeline ( int x1, int y1, int x2, int y2 )
{
  #ifdef HEXAGON
  int num = 0;
  while ( x1 != x2  || y1 != y2 ) {
     num++;
     getnextfield ( x1, y1, getdirection ( x1, y1, x2, y2 ));
  }
  return minmalq*num;

  #else
  return luftlinie8( x1, y1, x2, y2 );
  #endif

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


int beeline ( const pvehicle a, const pvehicle b )
{
   return beeline ( a->xpos, a->ypos, b->xpos, b->ypos );
}


boolean      fieldvisiblenow( const pfield        pe, int player  )
{ 
  if ( pe ) { 
      int c = (pe->visible >> ( player * 2)) & 3; 
      if ( godview ) 
         c = visible_all; 
      if (c > visible_ago) { 
         if ( pe->vehicle ) { 
            if ((c == visible_all) || ((pe->vehicle->height >= chschwimmend) && (pe->vehicle->height <= chhochfliegend))) 
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


#include "movecurs.inc"      


pfield        getactfield(void)

{ 
  word         x, y; 
  int      l; 
  pfield        fld;

   x = actmap->xpos + cursor.posx; 
   y = actmap->ypos + cursor.posy; 
   l = y * actmap->xsize + x; 
   fld =   &actmap->field[l]; 
   return ( fld );
} 



pfield        getfield(int          x,
                     int          y)
{ 
  int      l; 

   if ((x < 0) || (y < 0) || (x >= actmap->xsize) || (y >= actmap->ysize)) 
      return NULL; 
   else {     
      l = y * actmap->xsize + x; 
      return (   &actmap->field[l] );
   } 
} 


#define compensatebuildingcoordinateorgx (a) (dx & (~a))
#define compensatebuildingcoordinatex ( + (dx & ~b) )


pfield        getbuildingfield( const pbuilding    bld,
                              shortint     x,
                              shortint     y)
{ 
  integer      x1, y1; 
  integer      orgx, orgy;
  shortint     dx; 

   orgx = bld->xpos - bld->typ->entry.x; 
   orgy = bld->ypos - bld->typ->entry.y; 

   dx = orgy & 1; 
   orgx += (dx & (~ bld->typ->entry.y));
   
   y1=orgy+y;  

   x1 = orgx + x - (dx & y1); 

   return getfield(x1,y1); 
} 



void         getbuildingfieldcoordinates( const pbuilding    bld,
                                         shortint     x,
                                         shortint     y,
                                         int     &    xx,
                                         int     &    yy)
{ 
  integer      orgx, orgy; 
  shortint     dx; 

   orgx = bld->xpos - bld->typ->entry.x; 
   orgy = bld->ypos - bld->typ->entry.y; 

   dx = orgy & 1; 

   orgx += (dx & (~ bld->typ->entry.y));
   
   yy=orgy+y;
   xx=orgx+x-(dx & yy);

}








pointer      getmineadress( byte         num )
{ 
   #ifdef HEXAGON
   return streets.mineposition[num-1].position; 
   #else
   return streets.mineposition[num].position; 
   #endif
} 



void         tcursor::init ( void )
{
   actpictwidth = 0;

   backgrnd = new int [  imagesize ( 0, 0, fieldxsize, fieldysize ) ];

   { 
      int w;
     #ifndef HEXAGON
      tnfilestream stream ( "curs3.raw", 1 ); 
     #else
      tnfilestream stream ( "curshex2.raw", 1 ); 
     #endif
      stream.readrlepict ( &cursor.markfield, false, &w);
   }

   { 
      int w;
     #ifndef HEXAGON
      tnfilestream stream ( "cursor3.raw", 1 ); 
     #else
      tnfilestream stream ( "curshex.raw", 1 ); 
     #endif
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

     //setinvisiblemouserectanglestk ( xp, yp, xp + fieldxsize, yp + fieldysize );
     if (agmp-> linearaddress != hgmp-> linearaddress ) {
        npush(*agmp);
        *agmp = *hgmp;
        getimage(xp, yp, xp + idisplaymap.getfieldsizex(), yp + idisplaymap.getfieldsizey(), backgrnd );
        npop (*agmp);
     } else {
        getimage(xp, yp, xp + idisplaymap.getfieldsizex(), yp + idisplaymap.getfieldsizey(), backgrnd );
     } /* endif */
     //getinvisiblemouserectanglestk (  );
}

void       tcursor::putbkgr ( void )
{
     int xp = idisplaymap.getfieldposx( posx, posy );
     int yp = idisplaymap.getfieldposy( posx, posy );

     //setinvisiblemouserectanglestk ( xp, yp, xp + fieldxsize, yp + fieldysize );
     collategraphicoperations cgo ( xp, yp, xp + fieldxsize, yp + fieldysize );
     if (agmp-> linearaddress != hgmp-> linearaddress ) {
        npush( *agmp );
        *agmp = *hgmp;
        putimage(xp, yp, backgrnd );
        npop ( *agmp );
     } else {
        putimage(xp, yp, backgrnd );
     } /* endif */
     //getinvisiblemouserectanglestk (  );

}

void       tcursor::putimg ( void )
{
     int xp = idisplaymap.getfieldposx( posx, posy );
     int yp = idisplaymap.getfieldposy( posx, posy );

     checksize();

     //setinvisiblemouserectanglestk ( xp, yp, xp + fieldxsize, yp + fieldysize );
     collategraphicoperations cgo ( xp, yp, xp + fieldxsize, yp + fieldysize );
     if (agmp-> linearaddress != hgmp-> linearaddress ) {
        npush( *agmp );
        *agmp = *hgmp;
        putrotspriteimage(xp, yp, picture,color );
        npop ( *agmp );
     } else {
        putrotspriteimage(xp, yp, picture,color );
     } /* endif */
     //getinvisiblemouserectanglestk (  );
}


void         tcursor::setcolor( byte col )
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
   if ( actmap->xsize == 0  || actmap->ysize == 0 )
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


int getmapposx ( void )  // return the screencoordinates of the upper left position of the displayed map
{
   return scrleftspace + cursorrightshift;
}

int getmapposy ( void )
{
   return scrtopspace  + cursordownshift;
}



byte         getdiplomaticstatus(byte         b)
{ 
   if ( b & 7 ) 
     displaymessage("getdiplomaticstatus: \n parameter has to be in [0,8,16,..,64]",2);

   if ( b/8 == actmap->actplayer )
      return capeace;

   char *d = &actmap->alliances[ b/8 ][ actmap->actplayer ] ;

   if ( *d == capeace || *d == canewsetwar1 || *d == cawarannounce )
      return capeace;
   else
      return cawar;
} 


byte        getdiplomaticstatus2(byte    b, byte    c)
{ 
   if ( (b & 7) || ( c & 7 ) ) 
      displaymessage("getdiplomaticstatus: \n parameters have to be in [0,8,16,..,64]",2);

   if ( b == c )
      return capeace;

   char *d = &actmap->alliances [ b/8][ c/8 ];

   if ( *d == capeace || *d == canewsetwar1 || *d == cawarannounce )
      return capeace;
   else
      return cawar;
} 


pattackweap  attackpossible( const pvehicle     angreifer,
                            integer      x,
                            integer      y)
{ 
  integer      d; 
  int          i; 
  byte         tm; 

  pattackweap atw = new tattackweap;
           
  memset(atw, 0, sizeof(*atw));


   if ((x < 0) || (y < 0) || (x >= actmap->xsize) || (y >= actmap->ysize))  
      return atw;
   if (angreifer == NULL) 
      return atw;
   if (angreifer->typ->weapons->count == 0) 
      return atw;

   pfield efield = getfield(x,y);
                       
   if ( efield->vehicle ) {
      if (getdiplomaticstatus2(efield->vehicle->color, angreifer->color) == cawar) 
         for (i = 0; i <= angreifer->typ->weapons->count - 1; i++) 
            if (angreifer->typ->weapons->weapon[i].shootable() ) 
               if (angreifer->typ->weapons->weapon[i].offensive()  ) 
                  if (!( angreifer->typ->weapons->weapon[i].targets_not_hittable & ( 1 << efield->vehicle->typ->movemalustyp )))
                     if ( efield->vehicle->height & angreifer->typ->weapons->weapon[i].targ )
                        if (fieldvisiblenow(efield, angreifer->color/8)) { 
                           d = beeline(angreifer->xpos,angreifer->ypos,x,y); 
                           if (d <= angreifer->typ->weapons->weapon[i].maxdistance) 
                              if (d >= angreifer->typ->weapons->weapon[i].mindistance) { 
                                 if (angreifer->height & angreifer->typ->weapons->weapon[i].sourceheight ) 
                                    if (angreifer->ammo[i] ) { 
                                       atw->strength[atw->count] = angreifer->weapstrength[i]; 
                                       atw->num[atw->count ] = i; 
                                       atw->typ[atw->count ] = 1 << angreifer->typ->weapons->weapon[i].getScalarWeaponType() ;
                                       atw->count++;
                                    } 
                              } 
                        } 
   } 
   else 
      if (efield->building != NULL) { 
         if (getdiplomaticstatus2(efield->building->color, angreifer->color) == cawar) 
            for (i = 0; i < angreifer->typ->weapons->count ; i++) 
               if (angreifer->typ->weapons->weapon[i].shootable() ) 
                  if (angreifer->typ->weapons->weapon[i].offensive() ) 
                     { 
                        tm = efield->building->typ->buildingheight;
                        if (tm & angreifer->typ->weapons->weapon[i].targ) {
                           if (fieldvisiblenow(efield, angreifer->color/8)) { 
                              d = beeline(angreifer->xpos,angreifer->ypos,x,y); 
                              if (d <= angreifer->typ->weapons->weapon[i].maxdistance) 
                                 if (d >= angreifer->typ->weapons->weapon[i].mindistance) { 
                                    if (angreifer->height & angreifer->typ->weapons->weapon[i].sourceheight) 
                                       if (angreifer->ammo[i] > 0) { 
                                          atw->strength[atw->count ] = angreifer->weapstrength[i]; 
                                          atw->typ[atw->count ] = 1 << angreifer->typ->weapons->weapon[i].getScalarWeaponType() ;
                                          atw->num[atw->count ] = i; 
                                          atw->count++;
                                       } 

                                 } 
                           } 
                        } 
                     } 
      } 
   if ( efield->object ) {
      int n = 0;
      for ( i = 0; i < efield->object->objnum; i++ )
         if ( efield->object->object[i]->typ->armor > 0 )
            n++;

      if ( n > 0 ) 
         if ((efield->vehicle == NULL) && ( efield->building == NULL)) {
            for (i = 0; i <= angreifer->typ->weapons->count - 1; i++) 
               if (angreifer->typ->weapons->weapon[i].shootable() ) 
                  if ( angreifer->typ->weapons->weapon[i].getScalarWeaponType() == cwcannonn ||  
                       angreifer->typ->weapons->weapon[i].getScalarWeaponType() == cwbombn ) { 
                     if (chfahrend & angreifer->typ->weapons->weapon[i].targ ) { 
                        if (fieldvisiblenow(efield, angreifer->color/8)) { 
                           d = beeline(angreifer->xpos,angreifer->ypos,x,y); 
                           if (d <= angreifer->typ->weapons->weapon[i].maxdistance) 
                              if (d >= angreifer->typ->weapons->weapon[i].mindistance) { 
                                 if (angreifer->height & angreifer->typ->weapons->weapon[i].sourceheight ) 
                                    if (angreifer->ammo[i] > 0) { 
                                       atw->strength[atw->count ] = angreifer->weapstrength[i];
                                       atw->num[atw->count ] = i;
                                       atw->typ[atw->count ] = 1 << angreifer->typ->weapons->weapon[i].getScalarWeaponType();
                                       atw->count++;
                                    } 
   
                              } 
                        } 
                     } 
                  } 
         } 
   }

   return atw;
} 


boolean      attackpossible2u( const pvehicle     angreifer,
                               const pvehicle     verteidiger)
{ 
   if (angreifer == NULL) 
     return false ;

   if (verteidiger == NULL) 
     return false ;

   if (angreifer->typ->weapons->count == 0) 
     return false ;

   if ( getdiplomaticstatus2 ( angreifer->color, verteidiger->color ) == cawar )
      for ( int i = 0; i < angreifer->typ->weapons->count ; i++) 
         if (angreifer->typ->weapons->weapon[i].shootable() ) 
            if (angreifer->typ->weapons->weapon[i].offensive() ) 
               if (verteidiger->height & angreifer->typ->weapons->weapon[i].targ ) 
                  if (angreifer->height & angreifer->typ->weapons->weapon[i].sourceheight ) 
                     if (!( angreifer->typ->weapons->weapon[i].targets_not_hittable & ( 1 << verteidiger->typ->movemalustyp )))
                        if (angreifer->ammo[i] > 0) 
                           return true;
                  
            
   return false; 
} 



boolean      attackpossible28( const pvehicle     angreifer,
                               const pvehicle     verteidiger)
{ 
   if (angreifer == NULL) 
     return false ;

   if (verteidiger == NULL) 
     return false ;

   if (angreifer->typ->weapons->count == 0) 
     return false ;

   if ( getdiplomaticstatus2 ( angreifer->color, verteidiger->color ) == cawar )
      for ( int i = 0; i < angreifer->typ->weapons->count ; i++) 
         if (angreifer->typ->weapons->weapon[i].shootable() ) 
            if (angreifer->typ->weapons->weapon[i].offensive() ) 
               if (verteidiger->height & angreifer->typ->weapons->weapon[i].targ ) 
                  if (minmalq <= angreifer->typ->weapons->weapon[i].maxdistance) 
                     if (minmalq >= angreifer->typ->weapons->weapon[i].mindistance) 
                        if (angreifer->height & angreifer->typ->weapons->weapon[i].sourceheight ) 
                           if (!( angreifer->typ->weapons->weapon[i].targets_not_hittable & ( 1 << verteidiger->typ->movemalustyp )))
                              if (angreifer->ammo[i] > 0) 
                                 return true;
                  
            
   return false; 
} 


boolean      attackpossible2n( const pvehicle     angreifer,
                               const pvehicle     verteidiger)
{ 
   if (angreifer == NULL) 
     return false ;

   if (verteidiger == NULL) 
     return false ;

   if (angreifer->typ->weapons->count == 0) 
     return false ;

   int dist = beeline ( angreifer, verteidiger );
   if ( getdiplomaticstatus2 ( angreifer->color, verteidiger->color ) == cawar )
      if ( !angreifer->attacked )
         if ( !angreifer->typ->wait || angreifer->movement == angreifer->typ->movement[log2(angreifer->height)] )
            for ( int i = 0; i < angreifer->typ->weapons->count ; i++) 
               if (angreifer->typ->weapons->weapon[i].shootable() ) 
                  if (angreifer->typ->weapons->weapon[i].offensive() ) 
                     if (verteidiger->height & angreifer->typ->weapons->weapon[i].targ ) 
                        if (dist <= angreifer->typ->weapons->weapon[i].maxdistance) 
                           if (dist >= angreifer->typ->weapons->weapon[i].mindistance) 
                              if (angreifer->height & angreifer->typ->weapons->weapon[i].sourceheight ) 
                                 if (!( angreifer->typ->weapons->weapon[i].targets_not_hittable & ( 1 << verteidiger->typ->movemalustyp )))
                                    if (angreifer->ammo[i] > 0) 
                                       return true;
                     
            
   return false; 
} 






pattackweap   attackpossible3u( const pvehicle     angreifer,
                                const pvehicle     verteidiger)
{ 
  boolean      s = 0; 
  byte         i; 

  pattackweap atw = new tattackweap;
           
  memset(atw, 0, sizeof(*atw));


   if ( !angreifer ) 
     return atw ;
   if ( !verteidiger ) 
     return atw ;
   if (angreifer->typ->weapons->count == 0) 
     return atw ;
   if ( getdiplomaticstatus2(angreifer->color, verteidiger->color ) == cawar )
      for (i = 0; i < angreifer->typ->weapons->count ; i++) 
         if (angreifer->typ->weapons->weapon[i].shootable() ) 
            if (angreifer->typ->weapons->weapon[i].offensive() ) 
               if (verteidiger->height & angreifer->typ->weapons->weapon[i].targ ) 
                  if (angreifer->height & angreifer->typ->weapons->weapon[i].sourceheight ) 
                     if (!( angreifer->typ->weapons->weapon[i].targets_not_hittable & ( 1 << verteidiger->typ->movemalustyp )))
                        if (angreifer->ammo[i] > 0) {
                           atw->strength[atw->count ] = angreifer->weapstrength[i];
                           atw->num[atw->count ] = i;
                           atw->typ[atw->count ] = 1 << angreifer->typ->weapons->weapon[i].getScalarWeaponType();
                           atw->count++;
                        }
               
   return atw; 
} 


pattackweap   attackpossible38( const pvehicle     angreifer,
                               const pvehicle     verteidiger)
{ 
  boolean      s = 0; 
  byte         i; 

  pattackweap atw = new tattackweap;
           
  memset(atw, 0, sizeof(*atw));


   if ( !angreifer ) 
     return atw ;
   if ( !verteidiger ) 
     return atw ;
   if (angreifer->typ->weapons->count == 0) 
     return atw ;
   if ( getdiplomaticstatus2(angreifer->color, verteidiger->color ) == cawar )
      for (i = 0; i < angreifer->typ->weapons->count ; i++) 
         if (angreifer->typ->weapons->weapon[i].shootable() ) 
            if (angreifer->typ->weapons->weapon[i].offensive() ) 
               if (verteidiger->height & angreifer->typ->weapons->weapon[i].targ ) { 
                  if (minmalq <= angreifer->typ->weapons->weapon[i].maxdistance) 
                     if (minmalq >= angreifer->typ->weapons->weapon[i].mindistance) { 
                        if (angreifer->height & angreifer->typ->weapons->weapon[i].sourceheight ) 
                           if (!( angreifer->typ->weapons->weapon[i].targets_not_hittable & ( 1 << verteidiger->typ->movemalustyp )))
                              if (angreifer->ammo[i] > 0) {
                                 atw->strength[atw->count ] = angreifer->weapstrength[i];
                                 atw->num[atw->count ] = i;
                                 atw->typ[atw->count ] = 1 << angreifer->typ->weapons->weapon[i].getScalarWeaponType();
                                 atw->count++;
                              }
                     } 
               } 
   return atw; 
} 

pattackweap   attackpossible3n( const pvehicle     angreifer,
                               const pvehicle     verteidiger)
{ 
  boolean      s = 0; 
  byte         i; 

  pattackweap atw = new tattackweap;
           
  memset(atw, 0, sizeof(*atw));


   if ( !angreifer ) 
     return atw ;
   if ( !verteidiger ) 
     return atw ;
   if (angreifer->typ->weapons->count == 0) 
     return atw ;

   int dist = beeline ( angreifer, verteidiger );
   if ( getdiplomaticstatus2(angreifer->color, verteidiger->color ) == cawar )
      for (i = 0; i < angreifer->typ->weapons->count ; i++) 
         if (angreifer->typ->weapons->weapon[i].shootable() ) 
            if (angreifer->typ->weapons->weapon[i].offensive() ) 
               if (verteidiger->height & angreifer->typ->weapons->weapon[i].targ ) { 
                  if (dist <= angreifer->typ->weapons->weapon[i].maxdistance) 
                     if (dist >= angreifer->typ->weapons->weapon[i].mindistance) { 
                        if (angreifer->height & angreifer->typ->weapons->weapon[i].sourceheight ) 
                           if (!( angreifer->typ->weapons->weapon[i].targets_not_hittable & ( 1 << verteidiger->typ->movemalustyp )))
                              if (angreifer->ammo[i] > 0) {
                                 atw->strength[atw->count ] = angreifer->weapstrength[i];
                                 atw->num[atw->count ] = i;
                                 atw->typ[atw->count ] = 1 << angreifer->typ->weapons->weapon[i].getScalarWeaponType();
                                 atw->count++;
                              }
                     } 
               } 
   return atw; 
} 


#ifdef HEXAGON

const int woodformnum = 28;
int woodform[ woodformnum ] = { 63,30,60,39,51,28,35,48,6,57,15,14,56,7,49,47,59,31,61,60,55, -1,-1,-1,-1,-1,-1,-1 };
int woodid = 81;

void smooth ( int what );


void calculateforest( void )
{
   {
      for ( int y = 0; y < actmap->ysize ; y++) 
         for ( int x = 0; x < actmap->xsize ; x++) {
            pfield fld = getfield(x,y); 
   
            if ( fld->object )
               for ( int i = 0; i < fld->object->objnum; i++ )
                  if ( fld->object->object[i]->typ->id == woodid ) 
                     fld->object->object[i]->dir = 0;
         }
   }
   smooth( 1 );
   return;

   int run = 0;
   int changed ;
   do {
      changed = 0;
      for ( int y = 0; y < actmap->ysize ; y++) 
         for ( int x = 0; x < actmap->xsize ; x++) { 
            pfield fld = getfield(x,y); 
   
            if ( fld->object )
               for ( int i = 0; i < fld->object->objnum; i++ )
                  if ( fld->object->object[i]->typ->id == woodid ) {
                     pobject oi2 = fld->object->object[i];
   
                     int c = 0; 
                     for ( int i = 0; i < sidenum; i++) { 
                        int a = x; 
                        int b = y; 
                        getnextfield( a, b, i ); 
                        pfield fld2 = getfield(a,b); 
                                    
                        if ( fld2 ) { 
                           pobject oi = fld2->checkforobject ( oi2->typ );
                           if ( oi )
                             if ( oi->dir <= 20  ||  run == 0 )
                                 c |=  1 << i ; 
                               
                            
                        } 
                     } 

                     int found = 0;
                     int dr;
                     for ( int j = 0; j < woodformnum; j++ )
                        if ( woodform[j] == c ) {
                           dr = j;
                           found = 1;
                        }

                     if ( !found )
                        dr = 21 + ticker % 7;

                     if ( oi2->dir != dr  && !(oi2->dir >= 21  && dr >= 21)) {
                        oi2->dir = dr;
                        fld->setparams();
                        changed = 1;
                     }
                  }
         } 
      run++;
   } while ( changed );
}
#endif






int isresourcenetobject ( pobjecttype obj )
{
   if ( obj->id == 3 || obj->id == 30 || obj->id == 4 )
      return 1;
   else
      return 0;
}




void         calculateobject(integer      x,
                              integer      y,
                              boolean      mof,
                              pobjecttype obj)  /* mof = modify other fields  */        
{ 
  #ifdef HEXAGON
  if ( obj->id == woodid ) {
     calculateforest();
     return;
  }
  #endif

  int       d, e; 

   if ( obj->no_autonet )
      return;

   pfield fld = getfield(x,y) ;
   pobject oi2 = fld-> checkforobject (  obj  );

   if ( oi2 ) 
     e = 1;
   else 
     e = 0;

   int c = 0; 
   for ( int i = 0; i < sidenum; i++) { 
      int a = x; 
      int b = y; 
      getnextfield( a, b, i ); 
      pfield fld2 = getfield(a,b); 
                  
      if ( fld2 ) {
         for ( int oj = -1; oj < obj->objectslinkablenum; oj++ ) {
            pobject oi;
            if ( oj == -1 )
               oi = fld2->checkforobject ( obj );
            else
               oi = fld2->checkforobject ( obj->objectslinkable[oj] );
            
            int bld = 0;
            if ( fld2->building ) {
               if ( obj->connectablewithbuildings()  &&  (fld2->bdt & cbbuildingentry) )
                  bld = 1;
               else
                  if ( isresourcenetobject ( obj ))
                     bld = 1;
   
            }
            if ( oi || bld ) { 
               c |=  e << i ; 
               if ( mof && oi ) {
                  d = i + sidenum/2; 
                  if (d >= sidenum ) 
                     d -= sidenum; 
   
                  if (e == 0)
                     oi->setdir ( oi->getdir() &  ~(1 << d));
                  else
                     oi->setdir ( oi->getdir() |  (1 << d));
   
               } 
            } 
         }
      } 
      else { 
           /*    if i and 1 = 0 then
         c:=c or (1 shl i);  */ 
      } 
   } 

   if ( e ) {
     oi2->setdir ( c );
     fld->setparams();
   }

} 




void         calculateallobjects(void)       /* prir tst */
{ 
   for ( int y = 0; y < actmap->ysize ; y++) 
      for ( int x = 0; x < actmap->xsize ; x++) { 
         pfield fld = getfield(x,y); 

         if ( fld->object )
            for ( int i = 0; i < fld->object->objnum; i++ )
               if ( !fld->object->object[i]->typ->no_autonet )
                  #ifdef HEXAGON
                  if ( fld->object->object[i]->typ->id != woodid )
                  #endif
                      calculateobject( x, y, false, fld->object->object[i]->typ ); 

         fld->setparams();
      } 

  #ifdef HEXAGON
    calculateforest();
  #endif
} 


void         tcursor::display(void)
{ 
      hide(); 
      oposx = posx;
      oposy = posy;
      show(); 
} 

  /* procedure mount2(p,q,r:pointer; w:word);
var
  a,b:word;
begin
   asm
      push es
      push di
      push ds
      push si
      
      lds si, q
      les di, p
      
      mov cx,w
      sub cx,4

      lodsw
      stosw
      inc ax
      mov a,ax

      lodsw
      stosw
      inc ax
      mov b,ax
      
      
      rep movsb
      
      lds si,r
      les di, p
      add di,4
      lodsw
      inc ax
      sub a,ax
      mov dx,ax
      
      lodsw
      inc ax
      mov bx,ax

      @xstart:
      mov cx,dx
      @y:
      lodsb
      cmp al,255
      jz @w1
      stosb
      jmp @w2
      @w1:
      inc di
      @w2:
      
      loop @y
      add di,a

      dec bx
      jnz @xstart
      
      
      pop si
      pop ds
      pop di
      pop es
   end;
end;  */ 






void         cleartemps(byte         b)
{ 
  word         x, y; 
  int      l; 

  if ((actmap->xsize == 0) || (actmap->ysize == 0))  
     return;

   l = 0; 
   for (x = 0; x < actmap->xsize ; x++) 
         for (y = 0; y <  actmap->ysize ; y++) {
               if (b & 1 ) 
                 actmap->field[l].a.temp = 0;
               if (b & 2 ) 
                 actmap->field[l].a.temp2 = 0;
               if (b & 4 ) 
                 actmap->field[l].temp3 = 0;
            
            l++;
         } 
    
   
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
               if ( actmap->gameparameter [ cgp_fahrspur ] > 0 )
                  if ( i->time + actmap->gameparameter [ cgp_fahrspur ] < actmap->time.a.turn )
                     getfield ( x, y ) -> removeobject ( fahrspurobject );

            i = actmap->field[l].checkforobject ( eisbrecherobject );
            if ( i ) 
               if ( actmap->gameparameter [ cgp_eis ] > 0 )
                  if ( i->time + actmap->gameparameter [ cgp_eis ] < actmap->time.a.turn )
                     getfield ( x, y ) -> removeobject ( eisbrecherobject );
            l++;
         } 
} 

/*
void         clearmovtemps2(void)
{ 
  word         x, y; 
  int      l; 
  pfield        pf; 

   l = 0; 
   if ((actmap->xsize == 0) || (actmap->ysize == 0)) 
      return;
   for (x = 0; x <= actmap->xsize - 1; x++) 
      for (y = 0; y <= actmap->ysize - 1; y++) { 
         pf = &actmap->field[l] ;
         pf->special &= (  ~(1 << (cbmovetempv + 1)));
         l++;
      } 
} 
*/





void         removevehicle(pvehicle*   vehicle)
{ 
  pvehicle     p1; 
  byte         b; 

   b = (*vehicle)->color; 
   if (b >= 8) 
      b >>= 3; 

   p1 = *vehicle; 
   if (p1->prev != NULL) { 
      if (p1->next != NULL) { 
         p1->prev->next = p1->next; 
         p1->next->prev = p1->prev; 
      } 
      else 
         p1->prev->next = NULL; 
   } 
   else { 
      if (p1->next != NULL) { 
         p1->next->prev = NULL; 
         actmap->player[b].firstvehicle = p1->next; 
      } 
      else {
         actmap->player[b].firstvehicle = NULL; 
         #ifndef karteneditor
           for ( int i = 0; i < 8; i++ )
              quedevents[i]++;
         #endif
      }
   } 

   for ( int i = 0; i < 32; i++ )
      if ( p1->loading[i] )
         removevehicle (  &p1->loading[i] );

   #ifndef karteneditor
   if (p1->connection > 0) 
      releaseevent(p1,NULL,cconnection_destroy);
   #endif

   pfield fld = getfield(p1->xpos,p1->ypos);
   if ( fld->vehicle  == p1 )
     fld->vehicle = NULL; 

   delete p1;
   *vehicle = NULL;
} 


void         removebuilding(pbuilding *  bld)
{ 
  pbuilding    p1; 
  byte         b; 
  integer      i, j; 

   pbuilding building = *bld; 
   b = building->color; 
   if (b >= 8) 
      b >>= 3; 

   p1 = building; 
   if (p1->prev != NULL) { 
      if (p1->next != NULL) 
         p1->next->prev = p1->prev; 
      p1->prev->next = p1->next; 
   } 
   else { 
      if (p1->next != NULL) { 
         p1->next->prev = NULL; 
         actmap->player[b].firstbuilding = p1->next; 
      } 
      else {
         actmap->player[b].firstbuilding = NULL; 
         #ifndef karteneditor
           for ( int i = 0; i < 8; i++ )
              quedevents[i]++;
         #endif
      }
   } 
   int set = building->unchainbuildingfromfield();
   if ( set ) 
      for (i = building->xpos - 6; i <= building->xpos + 6; i++) 
         for (j = building->ypos - 6; j <= building->ypos + 6; j++) 
            if ((i >= 0) && (i < actmap->xsize)) 
               if ((j >= 0) && (j < actmap->ysize)) { 
                  calculateobject(i,j,0   , streetobjectcontainer ); 
                  calculateobject(i,j,0   , railroadobject ); 
                  // calculateobject(i,j,true, powerlineobject ); 
                  // calculateobject(i,j,true, pipelineobject ); 
               } 

   for (i = 0; i <= 31; i++) 
      if (building->loading[i] ) 
         removevehicle(&building->loading[i]); 

   delete building ;

   #ifndef karteneditor
   if (building->connection > 0) 
      releaseevent(NULL,building,cconnection_destroy); 
   #endif

   *bld = NULL; 
} 



int  tbuilding :: chainbuildingtofield ( int x, int y )
{

   int orgx = x - typ->entry.x - (typ->entry.y & ~y & 1 );
   int orgy = y - typ->entry.y; 

   int dx = orgy & 1; 


   int a;
   for ( a = orgx; a <= orgx + 3; a++) 
      for ( int b = orgy; b <= orgy + 5; b++) 
         if ( typ->getpicture ( a - orgx, b - orgy )) {
            pfield f = getfield(a + compensatebuildingcoordinatex, b );
            if ( !f )
               return 1;

            if ( f->building )
               return 1;
         }

   for ( a = orgx; a <= orgx + 3; a++) 
      for ( int b = orgy; b <= orgy + 5; b++) 
         if ( typ->getpicture ( a - orgx, b - orgy ))
             { 
              pfield field = getfield(a + compensatebuildingcoordinatex, b );
               if ( field->object ) {
                  for ( int n = 0; n < field->object->objnum; n++ )
                     delete field->object->object[n];
                  field->object->objnum = 0;
               }

               if ( field->vehicle )
                  removevehicle ( &field->vehicle );

               field = getfield(a + compensatebuildingcoordinatex, b );
               field->building = this;

               // field->picture = gbde->typ->picture[compl][a - orgx][b - orgy]; 
               field->bdt &= ~( cbstreet | cbrailroad | cbpipeline | cbpowerline );
              } 
            
   xpos = x;
   ypos = y;

   for ( int i = 0; i < 32; i++ )
      if ( loading[i] )
         loading[i]->setnewposition ( x, y );


   pfield field = getbuildingfield( this, typ->entry.x, typ->entry.y ); 
   field->bdt |= cbbuildingentry ; 

   resetbuildingpicturepointers ( this );
   calculateallobjects(); 

   return 0;
}


int  tbuilding :: unchainbuildingfromfield ( void )
{
   int set = 0;
   for (int i = 0; i <= 3; i++) 
      for (int j = 0; j <= 5; j++) 
         if ( typ->getpicture ( i, j ) ) {
            pfield fld = getbuildingfield( this, i, j ); 
            if ( fld->building == this ) {
               set = 1;
               fld->building = NULL; 
               fld->picture = fld->typ->picture[0]; 
               if ( fld->vehicle ) 
                  removevehicle( &fld->vehicle ); 

               tterrainbits t1 = cbstreet | cbbuildingentry | cbrailroad | cbpowerline | cbpipeline;
               tterrainbits t2 = ~t1;

               fld->bdt &= t2; 
            }
         } 
   return set;
}


void         putbuilding(int          x,
                         int          y,
                         int          color,
                         pbuildingtype buildingtyp,
                         int          compl,
                         int          ignoreunits )
{ 
  integer      a, b; 
  pbuilding    gbde; 
  pfield        field; 
  integer      orgx, orgy; 
  byte         dx; 



   if ( color & 7 )
      displaymessage("putbuilding mu· eine farbe aus 0,8,16,24,.. Åbergeben werden !",2); 


//   recordaction4(cnetid_buildbuilding,1,color,x,y,buildingtyp->id); ##########

   orgx = x - buildingtyp->entry.x - (buildingtyp->entry.y & ~y & 1 );
   orgy = y - buildingtyp->entry.y; 

   dx = orgy & 1; 

   for (a = orgx; a <= orgx + 3; a++) 
      for (b = orgy; b <= orgy + 5; b++) 
         if (buildingtyp->getpicture ( a - orgx, b - orgy ) ) {
            field = getfield(a + compensatebuildingcoordinatex, b);
            if (field == NULL) 
               return ;
            else {
               if ( field->vehicle && (!ignoreunits ) ) 
                  return;
               if (field->building != NULL)
                  return;
            }
         } 


   gbde = new tbuilding;
   gbde->color = color; 

   actmap->chainbuilding ( gbde );
   
   gbde->xpos = x; 
   gbde->ypos = y; 
   gbde->typ = buildingtyp; 
   if (compl >= buildingtyp->construction_steps)
      compl = buildingtyp->construction_steps - 1;

   memset( &gbde->production, 0, sizeof (gbde->production ));
   memset( &gbde->productionbuyable, 0, sizeof (gbde->productionbuyable ));
   memset( &gbde->loading    , 0, sizeof (gbde->loading     ));
   memset( &gbde->munition  , 0, sizeof (gbde->munition   ));
   memset( &gbde->munitionsautoproduction  , 0, sizeof (gbde->munitionsautoproduction   ));

   gbde->damage = 0; 
   gbde->plus.a.energy = 0; 
   gbde->plus.a.material = 0; 
   gbde->plus.a.fuel = 0; 
   gbde->maxplus.a.energy = gbde->typ->maxplus.a.energy; 
   gbde->maxplus.a.material = gbde->typ->maxplus.a.material; 
   gbde->maxplus.a.fuel = gbde->typ->maxplus.a.fuel; 

   gbde->actstorage.a.fuel = 0; 
   gbde->actstorage.a.material = 0; 
   gbde->actstorage.a.energy = 0; 
   gbde->maxresearchpoints = 0; 
   gbde->researchpoints = 0; 
   gbde->name = NULL;
   gbde->netcontrol = 0;
   gbde->connection = 0; 
   gbde->visible = true; 
   gbde->completion = compl; 
   
   gbde->chainbuildingtofield ( x, y );

} 


void         putbuilding2(integer      x,
                          integer      y,
                          byte         color,
                          pbuildingtype buildingtyp)
{ 
  integer      a, b; 
  pbuilding    gbde; 
  pfield        field; 
  integer      orgx, orgy; 
  byte         dx; 


  if ( actmap->objectcrc ) 
     if ( !actmap->objectcrc->speedcrccheck->checkbuilding2 ( buildingtyp ))
        return;


   if ( color & 7 )
      displaymessage("putbuilding mu· eine farbe aus 0,8,16,24,.. Åbergeben werden !",2); 

   orgx = x - buildingtyp->entry.x - (buildingtyp->entry.y & ~y & 1 );
   orgy = y - buildingtyp->entry.y; 

   dx = orgy & 1; 

   /*    orgx:=orgx + (dx and (not buildingtyp^.entry.y));  */

   for (a = orgx; a <= orgx + 3; a++) 
      for (b = orgy; b <= orgy + 5; b++) 
         if (buildingtyp->getpicture ( a - orgx , b - orgy ) ) {
            field = getfield(a + compensatebuildingcoordinatex,b);
            if (field == NULL) 
               return;
            else {
               if (field->vehicle)
                  return;
            }
         } 
                          
   if (getfield(x,y)->building == NULL) { 
      gbde = new tbuilding;
      gbde->color = color; 

      actmap->chainbuilding ( gbde );
       
      
      gbde->xpos = x; 
      gbde->ypos = y; 
      gbde->typ = buildingtyp; 

      memset( &gbde->production, 0, sizeof (gbde->production ));
      memset( &gbde->loading    , 0, sizeof (gbde->loading     ));
      memset( &gbde->munition  , 0, sizeof (gbde->munition   ));
      memset( &gbde->munitionsautoproduction  , 0, sizeof (gbde->munitionsautoproduction   ));
      memset( &gbde->productionbuyable, 0, sizeof (gbde->productionbuyable ));

      {
         tresources maxplus;
         tresources actplus;
         tresources biplus;
         int maxresearch = 0;

         memset ( &maxplus, 0, sizeof ( maxplus ));
         memset ( &actplus, 0, sizeof ( actplus ));
         memset ( &biplus,  0, sizeof ( biplus ));
         pbuilding bld = actmap->player[ color/8].firstbuilding;
         while ( bld ) {
            if ( bld->typ == gbde->typ  && bld != gbde ) {

               if ( bld->maxplus.a.energy > maxplus.a.energy )
                  maxplus.a.energy = bld->maxplus.a.energy;

               if ( bld->maxplus.a.material > maxplus.a.material )
                  maxplus.a.material = bld->maxplus.a.material;

               if ( bld->maxplus.a.fuel > maxplus.a.fuel )
                  maxplus.a.fuel = bld->maxplus.a.fuel;


               if ( bld->bi_resourceplus.a.energy > biplus.a.energy )
                  biplus.a.energy = bld->bi_resourceplus.a.energy;

               if ( bld->bi_resourceplus.a.material > biplus.a.material )
                  biplus.a.material = bld->bi_resourceplus.a.material;

               if ( bld->bi_resourceplus.a.fuel > biplus.a.fuel )
                  biplus.a.fuel = bld->bi_resourceplus.a.fuel;


               if ( bld->plus.a.energy > actplus.a.energy )
                  actplus.a.energy = bld->plus.a.energy;

               if ( bld->plus.a.material > actplus.a.material )
                  actplus.a.material = bld->plus.a.material;

               if ( bld->plus.a.fuel > actplus.a.fuel )
                  actplus.a.fuel = bld->plus.a.fuel;


               if ( bld->maxresearchpoints > maxresearch )
                  maxresearch = bld->maxresearchpoints;

            }   
            bld = bld->next;
         }

         gbde->damage = 0; 
         if ( actmap->resourcemode == 1 ) {
            gbde->plus.a.energy = biplus.a.energy; 
            gbde->plus.a.material = biplus.a.material; 
            gbde->plus.a.fuel = biplus.a.fuel; 
         } else {
            gbde->plus.a.energy = maxplus.a.energy; 
            gbde->plus.a.material = maxplus.a.material; 
            gbde->plus.a.fuel = maxplus.a.fuel; 
         }
         gbde->maxplus.a.energy = maxplus.a.energy; 
         gbde->maxplus.a.material = maxplus.a.material; 
         gbde->maxplus.a.fuel = maxplus.a.fuel; 
         gbde->bi_resourceplus.a.energy = biplus.a.energy; 
         gbde->bi_resourceplus.a.material = biplus.a.material; 
         gbde->bi_resourceplus.a.fuel = biplus.a.fuel; 
         gbde->actstorage.a.fuel = 0; 
         gbde->actstorage.a.material = 0; 
         gbde->actstorage.a.energy = 0; 
         gbde->maxresearchpoints = maxresearch; 
         gbde->researchpoints = 0; 
         gbde->name = NULL;
         gbde->netcontrol = 0;
         gbde->connection = 0; 
         gbde->visible = true; 
         gbde->completion = 0;
      
      }
      gbde->chainbuildingtofield ( x, y ); 

   } 
   else { 
      gbde = getfield(x,y)->building; 
      if (gbde->completion < gbde->typ->construction_steps-1) {
         gbde->completion++;
         gbde->chainbuildingtofield ( x, y ); 
      } 
   } 
} 




void         resetbuildingpicturepointers ( pbuilding bld )
{ 
   if ( bld->visible )
      for (int x = 0; x < 4; x++) 
         for ( int y = 0; y < 6; y++ ) 
            if ( bld->getpicture ( x, y ) )
                getbuildingfield ( bld, x, y )->picture = bld->getpicture ( x, y );
} 

void         resetallbuildingpicturepointers ( void )
{
   for (int s = 0; s < 8; s++) {
      pbuilding bld = actmap->player[s].firstbuilding;
      while ( bld ) {
         resetbuildingpicturepointers ( bld );
         bld = bld->next;
      } /* endwhile */
   } /* endfor */
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


void  checkfieldsformouse ( void )
{
   int i,j,xp,yp,k;
   if (mouseparams.taste == 1 || doubleclickparams.stat == 1) {
      for (i=0;i<idisplaymap.getscreenxsize();i++ ) {
         for (j=0;j<idisplaymap.getscreenysize() ;j++ ) {
            if ( j & 1)
               xp = scrleftspace + fielddisthalfx + i * fielddistx + cursorrightshift;
            else
               xp = scrleftspace + i * fielddistx + cursorrightshift;
   
            yp = scrtopspace + j * fielddisty + cursordownshift;
   
            if ( mouseparams.x >= xp && mouseparams.x < xp+fieldxsize && mouseparams.y >= yp && mouseparams.y < yp + fieldysize) {
              #ifdef HEXAGON
               int xd = mouseparams.x - xp;
               int yd = mouseparams.y - yp;
               word* pw = (word*) icons.fieldshape;
               char* pc = (char*) icons.fieldshape;

               k = 0;

               pc+=4;
               if ( pw[0] >= xd  && pw[1] >= yd )
                  if ( pc[ yd * ( pw[0] + 1) + xd] != 255 )
                     k++;
              #else
               k=0;
               int y;
               for (y=0; y<10 ;y++ ) {
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
                  if ( cursor.posx != i || cursor.posy != j) {
                     if (mouseparams.taste & 1 ) {
                        doubleclickparams.mx = mouseparams.x;
                        doubleclickparams.fx = i;
                        doubleclickparams.my = mouseparams.y;
                        doubleclickparams.fx = j;
                        doubleclickparams.stat = 1;
   
                        mousevisible(false);
                        cursor.hide();
                        cursor.posx = i;
                        cursor.posy = j;
                        cursor.show();
                        mousevisible(true);
                     }
                  } else {
                     if (mouseparams.taste & 1) {
                        if (doubleclickparams.stat == 2) {
                           pfield fld = getactfield ();
                           if (fld->building || fld ->vehicle || fld->a.temp ) {
                              // setmouseongui();
                              doubleclickparams.stat = 3;
                           } else
                              doubleclickparams.stat = 0;
                        } else
                          if (doubleclickparams.stat == 0 || doubleclickparams.stat == 3 ) {
                             doubleclickparams.mx = mouseparams.x;
                             doubleclickparams.fx = i;
                             doubleclickparams.my = mouseparams.y;
                             doubleclickparams.fx = j;
                             doubleclickparams.stat = 1;
                          };
                     } else {
                        if (doubleclickparams.stat == 1)
                           doubleclickparams.stat = 2;
                    }
                  }
               } /* endif */
   
            } /* endif */
   
            
         } /* endfor */
      } /* endfor */
   }
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
      mz = gameoptions.mapzoom;
     #else
      mz = gameoptions.mapzoomeditor;
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
   gameoptions.mapzoom = newzoom;
  #else
   gameoptions.mapzoomeditor = newzoom;
  #endif
   gameoptions.changed = 1;
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


///////////////////////////////////////////////////////////////////////////////////////
///********************************************************************************////
///********************************************************************************////
///********************************************************************************////
///********************************************************************************////
//                               tgeneraldisplaymap FIXED ZOOM                     ////
///********************************************************************************////
///********************************************************************************////
///********************************************************************************////
///********************************************************************************////
///////////////////////////////////////////////////////////////////////////////////////




#ifndef FREEMAPZOOM

int tgeneraldisplaymap :: getfieldposx ( int x, int y )
{
     if ( y & 1 )   /*  gerade reihennummern  */
        return scrleftspace + fielddisthalfx + x * fielddistx + cursorrightshift;
     else 
        return scrleftspace + x * fielddistx + cursorrightshift;
}


int tgeneraldisplaymap :: getfieldposy ( int x, int y )
{
     return scrtopspace  + y * fielddisty + cursordownshift;
}

void tgeneraldisplaymap :: init ( int xs, int ys )
{
   if ( actmap )
      playerview = actmap->playerview;

   dispmapdata.numberoffieldsx = xs;
   dispmapdata.numberoffieldsy = ys;
   dispmapdata.orgnumberoffieldsx = xs;
   dispmapdata.orgnumberoffieldsy = ys;

   dispmapdata.disp.x1 = 0;
   dispmapdata.disp.y1 = 0;
   dispmapdata.disp.x2 = xs;
   dispmapdata.disp.y2 = ys;


  #ifndef HEXAGON
   viereck = new char* [ dispmapdata.numberoffieldsx + 10 ];
   for ( int i = 0; i < dispmapdata.numberoffieldsx + 10; i++ )
     viereck[i] = new char[ dispmapdata.numberoffieldsy + 10 ];
  #endif

  invmousewindow.x1 = scrleftspace;
  invmousewindow.y1 = scrtopspace;
  invmousewindow.x2 = scrleftspace + scrxsize;
  invmousewindow.y2 = scrtopspace + scrysize;

}


// #define debugmask


#ifdef debugmask
extern dacpalette256 pal;
#endif

void tdisplaymap :: setxsizes ( int _game_x, int _maped_x )
{
   game_x = _game_x;
   maped_x = _maped_x;
}

void tdisplaymap :: generate_map_mask ( int *sze )
{
    tvirtualdisplay vdp ( resolutionx, resolutiony, 255 );

    #ifdef debugmask
    initsvga( 0x103 );
    setvgapalette256 ( pal );
    #endif

    void* mask;
    #ifndef HEXAGON
    void* mask2;
    #endif

    {
       int size;
       #ifdef HEXAGON
       tnfilestream s ( "hxfield.raw", 1 );
       s.readrlepict ( &mask,  0, &size );
       #else
       tnfilestream s ( "oktfield.raw", 1 );
       s.readrlepict ( &mask,  0, &size );
       s.readrlepict ( &mask2,  0, &size );
       #endif
    }

    {
       int y;
       for ( y=0;y < dispmapdata.numberoffieldsy ;y++ ) {
          for ( int x=0;x < dispmapdata.numberoffieldsx ;x++ ) {
             int yp;
             int r;
             if (y & 1 )   /*  ungerade reihennummern  */
                r = vfbleftspace + fielddisthalfx + x * fielddistx;
             else 
                r = vfbleftspace + x * fielddistx;
   
             yp = vfbtopspace + y * fielddisty;
             #ifdef HEXAGON
             putspriteimage ( r, yp, mask );
             #else
             putspriteimage ( r + unitrightshift , yp + unitdownshift, mask );
             #endif
          } /* endfor */
       } /* endfor */

       #ifndef HEXAGON
       for ( y=1;y < dispmapdata.numberoffieldsy - 1 ;y++ ) {
          for ( int x=1;x < dispmapdata.numberoffieldsx ;x++ ) {
             int yp;
             int r;
             if (y & 1 )   /*  ungerade reihennummern  */
                r = vfbleftspace + fielddisthalfx + x * fielddistx;
             else 
                r = vfbleftspace + x * fielddistx;
   
             yp = vfbtopspace + y * fielddisty;
             putspriteimage( r + viereckrightshift , yp + viereckdownshift -1 , mask2 ); // I don't know why I have to move the image 1 pixel left and up ....
          } /* endfor */
       } /* endfor */
       #endif
    
       for ( y = 0; y < resolutiony; y++ ) {
          char* c = (char*) (agmp->linearaddress + y * agmp->bytesperscanline);
          for( int x = 0; x < resolutionx; x++, c++ )
             if ( *c != 255 )
                *c = 254;
       }
       if ( mapborderpainter )
          mapborderpainter->paintborder( vfbleftspace, vfbtopspace );

       for ( y = 0; y < resolutiony; y++ ) {
          char* c = (char*) (agmp->linearaddress + y * agmp->bytesperscanline);
          for( int x = 0; x < resolutionx; x++, c++ )
             if ( *c < 254 )
                *c = 255;
       }
      
    }


   dynamic_array<int> mbuf ( 10000 );
   int bufpos = 0;

    #ifdef debugmask
    wait();
    #endif


   int col = 255;
   int abspos = 0;
   int dist = 0;
   int mode = 0;
   int delta = 0;
   bufpos++;
   for (int y = 0; y < agmp->resolutiony; y++) {
      for (int x = 0; x < agmp->resolutionx; x++) {
         int c = getpixel ( x, y );

         if ( c != col ) 
            abspos++;

         if ( mode ) {       // letztes Pixel war col 
            if (c != col) {
               dist++;
            } else {
              #ifdef debugmask
               putpixel ( x, y, white ); 
              #endif

               mbuf[bufpos++] = delta + dist ;
               delta = 0;
               dist = 1;
               mode = 0;
               abspos++;
            }
         } else {
            if (c == col) {
              #ifdef debugmask
               putpixel ( x, y, yellow ); 
              #endif
               dist++;
            } else {
              #ifdef debugmask
               putpixel ( x, y, white );
              #endif
               mbuf[bufpos++] = delta + dist ;
               delta = 0;
               dist = 1;
               mode = 1;
               abspos++;
            }
         } /* endif */

      } /* endfor */

      if ( abspos==0 ) 
         dist = 0;
      else
         if ( bufpos ) {
               delta = -agmp->bytesperscanline;
              #ifdef debugmask
               putpixel ( x, y, blue );
              #endif
            }
   } /* endfor */
  
   mbuf[0] = (bufpos - 1) / 2;
   mbuf[1] = 0;

   int* secondbuf = new int [ bufpos + 10 ];
   for ( int i = 0; i < bufpos; i++ )
      secondbuf[i] = mbuf[i];

   displaybuffer.screenmaskbuf = secondbuf ;
   *sze = bufpos*4;

  #ifdef debugmask
   wait();
  #endif
}


void tdisplaymap :: init ( int xs, int ys )
{
   rgmp = *agmp;
   rgmp.directscreenaccess = 1;

   tgeneraldisplaymap :: init ( xs, ys );

 #ifndef HEXAGON
   displaybuffer.direcpictbuf = (int*) new char [ dispmapdata.orgnumberoffieldsx * dispmapdata.orgnumberoffieldsy * 4 ];
   displaybuffer.spacingbuf = (int*) new char [ 8 + dispmapdata.orgnumberoffieldsx * dispmapdata.orgnumberoffieldsy * ( 4 + 39 * 12 ) ];
 #endif
 

   vfbscanlinelength = hgmp->scanlinelength;

   vfbstartdif = ( vfbtopspace + vfbfirstpointydif ) * vfbscanlinelength + vfbleftspace + vfbfirstpointxdif;

   dispmapdata.vfbheight = ( dispmapdata.orgnumberoffieldsy + 1 ) * fielddisty + vfbtopspace + vfbbottomspace;

   if ( dispmapdata.vfbheight < hgmp->resolutiony )
      dispmapdata.vfbheight = hgmp->resolutiony;


   int i = (scrtopspace + scrfirstpointydif) * hgmp->scanlinelength + scrleftspace + scrfirstpointxdif;
   if (hgmp-> windowstatus != 100) {
      scrstartpage = i >> 16;
      scrstartdif = i & 0xFFFF;
   } else 
      scrstartdif = i;



   dispmapdata.vfbadress = new char [ vfbscanlinelength * dispmapdata.vfbheight ];
   dispmapdata.vfbwidth = vfbscanlinelength;
                                                          
   memset( dispmapdata.vfbadress , 0, vfbscanlinelength * dispmapdata.vfbheight  );

  #ifndef HEXAGON
   generatespfdspaces();
  #endif 

   doubleclickparams.stat = 0;

   resolutionx = hgmp->resolutionx;
   resolutiony = hgmp->resolutiony;

   memset ( &displaymovingunit, 0, sizeof ( displaymovingunit ));
}

void tdisplaymap :: setup_map_mask ( void )
{
   #ifndef HEXAGON

   int sze;

   if ( dispmapdata.numberoffieldsx == 10  && dispmapdata.numberoffieldsy == 20 ) {
      char* screenmaskfile = "bkgr4.scr";
      sze = 5444;
      displaybuffer.screenmaskbuf = new int  [ sze / 4 ];

      tnfilestream stream ( screenmaskfile , 1);
      stream.readdata ( displaybuffer.screenmaskbuf, sze );
   } else
      generate_map_mask ( &sze );

  #else
   int sze;
   generate_map_mask ( &sze );
  #endif

   int *ip = (int*) displaybuffer.screenmaskbuf;
   for (int j = 0; j < sze/4; j++) 
      if ( ip[j] < 0 )
         ip[j]+=vfbscanlinelength;
}               

void  tdisplaymap :: deletevehicle ( void )
{
   displaymovingunit.eht = NULL;
}   


void tgeneraldisplaymap :: putdirecpict ( int x1, int y1, const void* ptr )
{
   int i;
   char* c = (char*) ptr;

   if ( agmp->windowstatus == 100 ) {
      char* buf = (char*) (agmp->scanlinelength * y1 + x1 + agmp->linearaddress) + 19;

      for ( i=1; i<= 19 ;i++ ) {
        for ( int num = 0; num < i * 2; num ++ ) {
           *buf = *c;
           c++;
           buf++;
        }

//        buf += vfbscanlinelength - 2 * i - 1;
        buf += agmp->scanlinelength - 2 * i - 1;
      } /* endfor */
                            
      for (i=20; i > 0 ;i-- ) {
        for ( int num = 0; num < i * 2; num ++ ) {
           *buf = *c;
           c++;
           buf++;
        }

//        buf += vfbscanlinelength - 2 * i + 1;
        buf += agmp->scanlinelength - 2 * i + 1;
      } /* endfor */
      
   }

}


#ifndef HEXAGON
void tdisplaymap :: generatespfdptrs ( int xp, int yp ) {
   int x,y,l,dx;
   int *pi = displaybuffer.direcpictbuf;

   l = yp * actmap->xsize + xp;
   dx = actmap->xsize - dispmapdata.numberoffieldsx;
   for (y=0;y < dispmapdata.numberoffieldsy ;y++ ) {
      for (x=0;x < dispmapdata.numberoffieldsx ;x++ ) {
         *pi = (int) (actmap->field[l].typ->direcpict[actmap->field[l].direction] );
         pi++;                            
         l++;
      } /* endfor */
      l+=dx;
   } /* endfor */

}


void  tdisplaymap :: generatespfdspaces ( void ) {
   int i,x,y;
   int *pi;

   int num;

   pi = displaybuffer.spacingbuf;
   *pi = (int) dispmapdata.vfbadress + vfbtopspace * vfbscanlinelength + vfbleftspace + vfbfieldxdif;
   pi++;

   *pi = dispmapdata.numberoffieldsy * dispmapdata.numberoffieldsx;
   pi++;

   for (y=0; y < dispmapdata.numberoffieldsy; y++ ) {
      
      for (x=0; x < dispmapdata.numberoffieldsx ;x++ ) {
 
        *pi = 39;
        pi++;
        for (i=1; i<= 19 ;i++ ) {
          num = i * 2;
          *pi = num / 4;
          pi++;
          *pi = (num % 4) / 2;
          pi++;

          *pi = vfbscanlinelength - 2 * i - 1;
          pi++;
        } /* endfor */
                              
        for (i=20; i > 0 ;i-- ) {
          num = i * 2;
          *pi = num / 4;
          pi++;
          *pi = (num % 4) / 2;
          pi++;

          *pi = vfbscanlinelength - 2 * i + 1;
          pi++;
        } /* endfor */
        pi--;
        *pi = -38 * vfbscanlinelength + 38;
        pi++;

      } /* endfor */
      pi--;
      if (y & 1) {
         *pi = -(( dispmapdata.numberoffieldsx - 1 ) * 40 + 18 * vfbscanlinelength + 22 );
      } else {
         *pi = - ( dispmapdata.numberoffieldsx - 1 ) * 40 + 18 - 18 * vfbscanlinelength;
      } /* endif */
      pi++;

   } /* endfor */
}

#endif



void tgeneraldisplaymap :: clearvf ( void )
{
   int* i = (int*) dispmapdata.vfbadress;
   i--;
   memset( dispmapdata.vfbadress, 0, *i );
}




int  tgeneraldisplaymap :: getscreenxsize( int target )
{
   return dispmapdata.numberoffieldsx;
}

int  tdisplaymap :: getscreenxsize( int target )
{
   if ( target == 0 )
      return dispmapdata.numberoffieldsx;
   else
      if ( game_x  > maped_x )
         return game_x;
      else
         return  maped_x ;
}


int  tgeneraldisplaymap :: getscreenysize( int target )
{
   return dispmapdata.numberoffieldsy;
}


                       
void        tgeneraldisplaymap :: calcviereck(void)
{ 
 #ifndef HEXAGON
  int          i, j;
  pfield        pf; 

   for (i = 0; i <= dispmapdata.numberoffieldsx+4 ; i++)
      memset ( viereck[i], 0, dispmapdata.numberoffieldsy+4 );


   for (i = 1; i <= dispmapdata.numberoffieldsx+3 ; i++)
      for (j = 1; j <= dispmapdata.numberoffieldsy+3 ; j++) {
         pf = getfield(actmap->xpos + i - 2 ,actmap->ypos + j - 2 );
         if ( pf ) { 
            if ( ((pf->visible >> (playerview * 2)) & 3 ) == visible_not) {
               if (j & 1) {
                  viereck[i+1][j-1] |= (3 << 4);
                  viereck[i][j]     |= (3 << 2);
                  viereck[i+1][j]   |= (3 << 6);
                  viereck[i+1][j+1] |= (3 << 0);
               } else {
                  viereck[i][j-1]   |= (3 << 4);
                  viereck[i][j]     |= (3 << 2);
                  viereck[i+1][j]   |= (3 << 6);
                  viereck[i][j+1]   |= (3 << 0);
               }
            }
            if ( ((pf->visible >> (playerview * 2)) & 3 ) == visible_ago) {
               if (j & 1) {
                  viereck[i+1][j-1] |= (1 << 4);
                  viereck[i][j]     |= (1 << 2);
                  viereck[i+1][j]   |= (1 << 6);
                  viereck[i+1][j+1] |= (1 << 0);
               } else {
                  viereck[i][j-1]   |= (1 << 4);
                  viereck[i][j]     |= (1 << 2);
                  viereck[i+1][j]   |= (1 << 6);
                  viereck[i][j+1]   |= (1 << 0);
               }
            }


         } else {
            if (j & 1) {
               viereck[i+1][j-1] |= (3 << 4);
               viereck[i][j]     |= (3 << 2);
               viereck[i+1][j]   |= (3 << 6);
               viereck[i+1][j+1] |= (3 << 0);
            } else {
               viereck[i][j-1]   |= (3 << 4);
               viereck[i][j]     |= (3 << 2);
               viereck[i+1][j]   |= (3 << 6);
               viereck[i][j+1]   |= (3 << 0);
            }
         } 
      } 
 #endif
} 




void tdisplaymap :: gnt_terrain ( void )
{
   #ifndef HEXAGON
    playerview = actmap->playerview;
    generatespfdptrs ( actmap->xpos, actmap->ypos );
    calcviereck();
   #endif
}



void tgeneraldisplaymap :: pnt_terrain_rect ( void )
{
      for ( int y = dispmapdata.disp.y1;  y < dispmapdata.disp.y2; y++) {
         for ( int x = dispmapdata.disp.x1; x < dispmapdata.disp.x2; x++) {
            pfield fld = getfield ( actmap->xpos + x, actmap->ypos + y );

            int xp;
            if ( y & 1 )   /*  ungerade reihennummern  */
               xp = vfbleftspace + fielddisthalfx + x * fielddistx;
            else 
               xp = vfbleftspace  + x * fielddistx;

            int yp = vfbtopspace + y * fielddisty;
           #ifdef HEXAGON
            putspriteimage ( xp, yp, fld->typ->picture[0] );
           #else
            putdirecpict( xp , yp , fld->typ->direcpict [ fld->direction ] );
           #endif
         } 
      }
            
}



void tdisplaymap :: pnt_terrain ( void )
{
   gnt_terrain (  );

    agmp-> windowstatus = 100;
    agmp->scanlinelength = vfbscanlinelength;
    agmp->bytesperscanline = vfbscanlinelength;
    agmp-> linearaddress = (int) dispmapdata.vfbadress;
   #ifdef HEXAGON
    for (int y=dispmapdata.disp.y1; y < dispmapdata.disp.y2 ;y++ ) {
       for ( int x=dispmapdata.disp.x1 ; x < dispmapdata.disp.x2 ;x++ ) {
          pfield fld = getfield ( actmap->xpos + x, actmap->ypos + y );

          if ( !fld )
             displaymessage ( " spfst / tdisplaymap :: pnt_terrain / void field to display\n"
                              "   this is probably caused by trying to display a map that is smaller\n"
                              "   than the resolution. Please reduce the displayed resolution until we\n"
                              "   have rewritten these routines" , 2 );
          int b = (fld->visible >> (playerview << 1)) & 3;
          if ( godview ) 
              b = visible_all;

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
       } /* endfor */
    } /* endfor */
   #else

    dispspielfeld( displaybuffer.spacingbuf, displaybuffer.direcpictbuf ); 
   #endif

/*
    activefontsettings.font   = schriften.smallsystem;
    activefontsettings.color  = white;
    activefontsettings.background = 255;
    activefontsettings.length = 0;
    activefontsettings.justify = lefttext;
*/
}


void tgeneraldisplaymap :: pnt_terrain ( void )
{
   pnt_terrain_rect ( );
}


void tdisplaymap :: cp_buf ( void )
{
    copyvfb2displaymemory( displaybuffer.screenmaskbuf );
}


#else

///////////////////////////////////////////////////////////////////////////////////////
///********************************************************************************////
///********************************************************************************////
///********************************************************************************////
///********************************************************************************////
//                               tgeneraldisplaymap   FREEMAPZOOM                  ////
///********************************************************************************////
///********************************************************************************////
///********************************************************************************////
///********************************************************************************////
///////////////////////////////////////////////////////////////////////////////////////



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
      playerview = actmap->playerview;

   dispmapdata.numberoffieldsx = xs;
   dispmapdata.numberoffieldsy = ys;
}


void tgeneraldisplaymap :: _init ( int xs, int ys )
{
   if ( actmap )
      playerview = actmap->playerview;

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
     if ( y & 1 )   /*  gerade reihennummern  */
        return windowx1 + (fielddisthalfx + x * fielddistx ) * zoom / 100;
     else 
        return windowx1 + ( x * fielddistx ) * zoom / 100;
}


int tdisplaymap :: getfieldposy ( int x, int y )
{
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
   *agmp = vfb.parameters;

    for (int y=dispmapdata.disp.y1; y < dispmapdata.disp.y2; y++ ) {
       for ( int x=dispmapdata.disp.x1; x < dispmapdata.disp.x2; x++ ) {
          pfield fld = getfield ( actmap->xpos + x, actmap->ypos + y );
          if ( fld ) {
             int b = (fld->visible >> (playerview << 1)) & 3;
             if ( godview ) 
                 b = visible_all;
   
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

/*
       char* edi = (char*) (hgmp->linearaddress + windowx1 + windowy1 * hgmp->bytesperscanline );
       char* esi = (char*) (agmp->linearaddress + vfbstartdif );
   
       char* destlinestart = edi;
       char* esistart = esi;
   
       for ( int yp = 0; yp < window.ysize; yp++ ) {
          for ( int xp = 0; xp < window.xsize; xp++ ) {
             esi += copybufsteps[ xp ] + 1;
              *(edi++) = *esi;
          }
          
          edi = destlinestart + hgmp->bytesperscanline; 
          destlinestart = edi;
          esi = esistart + agmp->bytesperscanline * ( yp * 100 / zoom);
       }
*/

    }
}

#endif
///////////////////////////////////////////////////////////////////////////////////////
///********************************************************************************////
///********************************************************************************////
///********************************************************************************////
///********************************************************************************////
//                     tgeneraldisplaymap    ENDE                                  ////
///********************************************************************************////
///********************************************************************************////
///********************************************************************************////
///********************************************************************************////
///////////////////////////////////////////////////////////////////////////////////////





void tgeneraldisplaymap :: pnt_main ( void )
{
   activefontsettings.font = schriften.monogui;
   activefontsettings.background = 255;
   activefontsettings.color = white;
   activefontsettings.length = 0;
   activefontsettings.justify = lefttext;
   activefontsettings.height = 0;
   

   playerview = actmap->playerview;


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
               b = (fld->visible >> (playerview << 1)) & 3;
               if (godview) 
                  b = visible_all;
   
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
                      if ((b == visible_all) || godview) 
                           if (fld->object && fld->object->mine && hgt == 4 ) {
                              putrotspriteimage( r + unitrightshift , yp + unitdownshift ,getmineadress(fld->object->mine >> 4), (fld->object->mine & 6) >> 2);
                              #ifdef karteneditor
                              if  ( ( ( fld->object->mine >> 1 ) & 7 ) < 8 ) 
                                 bar ( r + unitrightshift + 5 , yp + unitdownshift +5, r + unitrightshift + 15 , yp + unitdownshift +10, 20 + ( ( ( fld->object->mine >> 1 ) & 7 ) << 3 ) );
                              #endif
                           }
   

                  /* display marked fields */
                      if ( hgt == 8 ) {
                          if ( ( fld->a.temp || fld->a.temp2 ) && tempsvisible ) 
                               putspriteimage(  r + unitrightshift , yp + unitdownshift ,cursor.markfield);
       



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
      /*viewbehinderungen: Visible_Ago,  Visible_NOT  zeichnen  ...            ˇ */
      /****************************************************************************/
      if (!godview) {
        #ifndef HEXAGON          
         for ( y=dispmapdata.disp.y1; y < dispmapdata.disp.y2; y++ ) {
            for ( x=dispmapdata.disp.x1; x < dispmapdata.disp.x2; x++ ) {
   
               if (y & 1 )   
                  r = vfbleftspace + fielddisthalfx + x * fielddistx;
               else 
                  r = vfbleftspace + x * fielddistx;
               yp = vfbtopspace + y * fielddisty;
   
               if ( viereck[x+2][y+2] )           // frÅher mal:   && (! (( x ==0) && ((y & 1) == 0)))   , aber da links genug Platz, mÅ·te es auch ohne klappen. Noch aus Vor-VFB-Zeiten
                  putspriteimage( r + viereckrightshift , yp + viereckdownshift , view.viereck[viereck[x+2][y+2]]);
            } 
         } 
        #endif



         for ( y=dispmapdata.disp.y1; y < dispmapdata.disp.y2; y++ ) {
            for ( x=dispmapdata.disp.x1; x < dispmapdata.disp.x2; x++ ) {
               fld = getfield ( actmap->xpos + x, actmap->ypos + y );
               if ( fld ) {
                  b = (fld->visible >> (playerview << 1)) & 3;
                  if (godview) 
                     b = visible_all;
      
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
                      putshadow( r + unitrightshift , yp + unitdownshift , view.nv8, &xlattables.a.dark2 );
      
                      if ( ( fld->a.temp || fld->a.temp2 ) && tempsvisible ) 
                          putspriteimage(  r + unitrightshift , yp + unitdownshift ,cursor.markfield);
   
                      #ifdef showtempnumber   
                      activefontsettings.color = white;
                      showtext2(strrr( fld->temp ), r + unitrightshift + 5, yp + unitdownshift + 5 );
                      activefontsettings.color = black;
                      showtext2(strrr( fld->temp2 ), r + unitrightshift + 5, yp + unitdownshift + 20 );
                      #endif   
                          
                  } else 
                    if (b == visible_not) {
                        putspriteimage( r + unitrightshift, yp + unitdownshift , view.nv8 );
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
         int b = (fld->visible >> (playerview << 1)) & 3;
         if (godview) 
            b = visible_all;

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
         int b = (fld->visible >> (playerview << 1)) & 3;
         if (godview) 
            b = visible_all;


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


   byte         ms;   /*  mousestatus  */
   boolean      bb;   /*  cursorstatus   */

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
      idisplaymap.playerview = actmap->playerview;

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


void  tdisplaymap :: movevehicle(integer      x1,
                                       integer      y1,
                                       integer      x2,
                                       integer      y2,
                                       pvehicle     eht)
{ 
   byte         dir; 

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
   
      int r1;
      if ( dir & 1 )
         r1 = 20;
      else
         r1 = 40;
   
      int r2 = 0;
   
   
     /************************************/
     /*     Schattenposition computen   */
     /************************************/
   
   
      if ( moveparams.movestatus == 12  && ( moveparams.oldheight > chfahrend  || moveparams.newheight > chfahrend )) {
   
         int actdist ;
         if ( dir & 1 )
            actdist = minmalq;
         else
            actdist = maxmalq;
   
         int compl = beeline ( moveparams.movesx, moveparams.movesy, getxpos(), getypos() ) / actdist;
         int went = beeline  ( moveparams.movesx, moveparams.movesy, x1+actmap->xpos, y1+actmap->ypos ) / actdist;
         int togo = beeline ( x2+actmap->xpos, y2+actmap->ypos, getxpos(), getypos() ) / actdist;
   
         int ht1 = 10 * ( log2 ( moveparams.oldheight ) - log2 ( chfahrend ) );
         int ht2 = 10 * ( log2 ( moveparams.newheight ) - log2 ( chfahrend ) );
         int htd = ht2- ht1;
   
         if ( moveparams.oldheight < moveparams.newheight ) {
            if ( moveparams.oldheight == chfahrend ) {
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
            if ( moveparams.newheight == chfahrend ) {
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
   
      int step = 1;
   
   
     /************************************/
     /*     eigentliche movement         */
     /************************************/
   
      int starttick = ticker;
      int sdx = dx;
      int sdy = dy;
   
      do {
         putimage ( xp1, yp1, picbuf );
   
         r2++;
   
   
         int tick =  gameoptions.movespeed - ( ticker - starttick );  
         if ( tick < 0 )
            tick = 0;
   
         displaymovingunit.xpos = x2;
         displaymovingunit.ypos = y2;
         displaymovingunit.dx = 0 ;// -dx;
         displaymovingunit.dy = 0 ;// -dy;
         displaymovingunit.hgt = h2 - ( h2 - h1 ) * tick / gameoptions.movespeed;
   
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
      
            idisplaymap.cp_buf (  );
         }

   
         dx = sdx * tick / gameoptions.movespeed;
         dy = sdy * tick / gameoptions.movespeed;

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


void checkobjectsforremoval ( void )
{
   for ( int y = 0; y < actmap->ysize; y++ )
      for ( int x = 0; x < actmap->xsize; x++ ) {
         pfield fld = getfield ( x, y );
         if ( fld->object )
            for ( int i = 0; i < fld->object->objnum; i++ )
               if ( fld->object->object[i]->typ->terrainaccess.accessible ( fld->bdt ) < 0 )
                  fld->removeobject ( fld->object->object[i]->typ );

      }

}

void  checkunitsforremoval ( void )
{
   int i;
   pvehicle eht, eht2;
   for (i=0;i<8 ;i++ ) {
      eht = actmap->player[i].firstvehicle;
      while (eht != NULL) {
          pfield field = getfield(eht->xpos,eht->ypos);
          eht2 = eht->next;
          if (field->vehicle == eht) {
             if ( eht->height <= chfahrend )
                if ( eht->typ->terrainaccess->accessible ( field->bdt ) < 0 )
                   removevehicle ( &eht );

             if ( getmaxwindspeedforunit( eht ) < actmap->weather.wind[getwindheightforunit ( eht )].speed*maxwindspeed )
                 removevehicle ( &eht );

          }
          eht = eht2;
      } /* endwhile */

   } /* endfor */
}


int  getwindheightforunit ( const pvehicle eht )
{
   if ( eht -> height == chfliegend )
      return 1;
   else
      if ( eht -> height == chhochfliegend )
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

      if ( (field->bdt & cbfestland) == 0)
         return eht->typ->maxwindspeedonwater * maxwindspeed;
   }
   return maxint;
}








void checkplayernames ( void )
{
  for ( int i = 0; i < 8; i++ ) {
     if ( !actmap->humanplayername[i] ) {
        actmap->humanplayername[i] = new char [10];
        strcpy ( actmap->humanplayername[i], "player " );
        strcat ( actmap->humanplayername[i], digit[i] );
     }
     if ( !actmap->computerplayername[i] ) {
        actmap->computerplayername[i] = new char [10];
        strcpy ( actmap->computerplayername[i], "ai " );
        strcat ( actmap->computerplayername[i], digit[i] );
     }

     if ( !actmap->player[i].name ) 
        if ( actmap->player[i].stat == 0 )
           actmap->player[i].name = actmap->humanplayername[i];
        else
           if ( actmap->player[i].stat == 1 )
              actmap->player[i].name = actmap->computerplayername[i];

     if ( actmap->player[i].stat == 2 )
        actmap->player[i].name = NULL;
  }
}


int getcrc ( const pvehicletype fzt )
{
    if ( !fzt )
       return -1;

    tvehicletype fz = *fzt;
    fz.name = NULL;
    fz.description = NULL;
    fz.infotext = NULL;
    for ( int i = 0; i < 8; i++ ) {
       fz.picture[i] = NULL;
       fz.threatvalue[i] = 0;
       fz.classnames[i] = NULL;

    }
    fz.generalthreatvalue = 0;
    fz.buildicon = NULL;
    int terr = fz.terrainaccess->getcrc();
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
    #ifdef HEXAGON
    o.oldpicture = NULL;
    #endif
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
          twterraintype b = *bdn->weather[i];
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

    tbuildingtype b = *bld;
    for ( int i = 0; i < maxbuildingpicnum; i++ )
       for ( int j = 0; j < 4; j++ )
          for ( int k = 0; k < 6; k++ )
             #ifdef HEXAGON
              for ( int w = 0; w < cwettertypennum; w++ )
                 b.w_picture[w][i][j][k] = NULL;
             #else
               b.picture[i][j][k] = NULL;
             #endif

    b.name = NULL;
    b.guibuildicon = NULL;
    b.terrain_access = NULL;
    
    return crc32buf ( &b, sizeof ( b ));
}


tsearchfields :: tsearchfields ( void )
{
   abbruch = 0;
}

void         tsearchfields::done(void)
{ 
} 


/*
void         tsearchfields::testfield(void)
{                         
   getfield(xp,yp)->temp = (char) dist;
} 

*/


void         tsearchfields::initsuche( int  sx, int  sy, word max, word min )
{ 
   maxdistance = max; 
   mindistance = min; 
   if (mindistance == 0) mindistance = 1; 
   if (maxdistance == 0) maxdistance = 1; 
   startx = sx; 
   starty = sy; 
} 


#ifdef HEXAGON

void         tsearchfields::startsuche(void)
{ 
  if ( abbruch )
     return;

  word         strecke; 
  shortint     step; 

   if (mindistance > maxdistance) 
      step = -1; 
   else 
      step = 1; 
   strecke = mindistance; 

   do { 
      dist = strecke; 

      xp = startx; 
      yp = starty - 2*strecke; 
      for ( int e = 0; e < 6; e++ ) {
         int dir = (e + 2) % sidenum;
         for ( int c = 0; c < strecke; c++) { 
            if ((xp >= 0) && (yp >= 0) && (xp < actmap->xsize) && (yp < actmap->ysize)) 
               testfield(); 
            getnextfield ( xp, yp, dir );
         }

         if ( abbruch ) 
            return;
      } 

      strecke += step; 

   }  while (!((strecke - step == maxdistance) || abbruch)); 
} 

#else

void         tsearchfields::startsuche(void)
{ 
  if ( abbruch )
     return;

  word         strecke; 
  int          a, c;
  shortint     step; 

   if (mindistance > maxdistance) 
      step = -1; 
   else 
      step = 1; 
   strecke = mindistance; 

   do { 
      dist = strecke; 
      a = startx - strecke; 

      xp = a; 
      yp = starty; 
      for (c = 1; c <= 2 * strecke; c++) { 
         yp--;
         if ((starty & 1) == (c & 1)) 
           xp++;
         if ((xp >= 0) && (yp >= 0) && (xp < actmap->xsize) && (yp < actmap->ysize)) 
            testfield(); 
         if (abbruch) return;
      } 

      xp = startx + strecke + (starty & 1); 
      yp = starty; 
      for (c = 0; c < 2 * strecke ; c++) { 
         if ((starty & 1) != (c & 1)) 
            xp--;
         if ((xp >= 0) && (yp >= 0) && (xp < actmap->xsize) && (yp < actmap->ysize)) 
            testfield(); 
         if (abbruch) 
            return;
         yp--;
      } 

      yp = starty + strecke * 2 - 1; 
      xp = startx - (yp & 1);
      for (c = 1; c <= 2 * strecke; c++) {   /*  ????????  */ 
         if ((xp >= 0) && (yp >= 0) && (xp < actmap->xsize) && (yp < actmap->ysize)) 
            testfield(); 
         if (abbruch) 
           return;
         if ((starty & 1) == (c & 1)) xp--;

         yp--;
      } 

      xp = startx; 
      yp = starty + 2 * strecke; 
      for (c = 0; c <= 2 * strecke - 1; c++) { 
         if ((xp >= 0) && (yp >= 0) && (xp < actmap->xsize) && (yp < actmap->ysize)) 
            testfield(); 
         if ((starty & 1) != (c & 1)) 
           xp++;
         yp--;
         if (abbruch) return;
      } 
      strecke += step; 
   }  while (!((strecke - step == maxdistance) || abbruch)); 
} 
#endif

  
tlockdispspfld :: tlockdispspfld ( void ) 
{ 
   lockdisplaymap ++; 
}

tlockdispspfld :: ~tlockdispspfld () 
{ 
   lockdisplaymap --; 
}




class tdisplaywholemap : public tgeneraldisplaymap {
          tvirtualdisplay* dsp;
       public:
          int getfieldposx ( int x, int y ) { return 0; }; 
          int getfieldposy ( int x, int y ) { return 0; };
          tdisplaywholemap ( void );
          virtual void init ( int xs, int ys );
          void cp_buf ( void );

          ~tdisplaywholemap ();

};

tdisplaywholemap :: tdisplaywholemap ( void )
{
   dsp = NULL;
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

extern dacpalette256 pal;

void tdisplaywholemap :: cp_buf ( void )
{
   writepcx ( getnextfilenumname ( "map", "pcx" ), vfbleftspace, vfbtopspace, vfbleftspace + getscreenxsize() * fielddistx + 20, vfbtopspace + (getscreenysize() + 1 ) * fielddisty, pal );
}


void writemaptopcx ( void )
{

   {
      int mb = 1024*1024;
      int bufsizex = actmap->xsize * fielddistx + 200 ;
      int bufsizey = actmap->ysize * fielddisty + 200 ;
      int bufsize = (bufsizex * bufsizey + mb - 1) / mb;
      if ( choice_dlg( "About %d MB of Buffer will be allocated !", "c~o~ntinue", "~c~ancel", bufsize) == 2)
         return;
   }

   displaymessage ( "writing image\nplease wait",0 );

   npush ( *agmp );
   npush ( actmap->xpos );
   npush ( actmap->ypos );
                      
   actmap->xpos = 0;
   actmap->ypos = 0;

   try {
      tdisplaywholemap wm;
      wm.init ( actmap->xsize, actmap->ysize );
      wm.pnt_terrain ( );
      wm.pnt_main ( );
      wm.cp_buf (  );
   } /* endtry */
   catch ( fatalgraphicserror ) {
      displaymessage("unable to generate image", 1 );
   } /* endcatch */

   npop ( actmap->ypos );
   npop ( actmap->xpos );
   npop ( *agmp );

   removemessage();
}




#ifdef HEXAGON




/* 
   These smoothing functions are straight conversions from Jîrg Richter's routines 
   he made for his Battle Isle Map editor 
   Many thanks for giving them to me !
*/


int SmoothTreesData0[] = {
     4, 7, 10,101,
     1,0x0001,243,
     1,0x0115,243,          // die die garnicht gehen 
    30,0x3F, 30,243,0x3F, 60,243,0x3F, 39,243,0x3F, 51,243,
       0x3F, 28,243,0x3F, 35,243,0x3F, 48,243,0x3F,  6,243,
       0x3F, 57,243,0x3F, 15,243,0x3F, 14,243,0x3F, 56,243,
       0x3F,  7,243,0x3F, 49,243,0x3F, 47,243,0x3F, 59,243,
       0x3F, 31,243,0x3F, 61,243,0x3F, 62,243,0x3F, 55,243,
       0x3F, 23,243,0x3F, 46,243,0x3F, 29,243,0x3F, 58,243,
       0x3F, 53,243,0x3F, 43,243,0x3F, 22,243,0x3F, 38,243,
       0x3F, 50,243,0x3F, 52,243,
     7,264,265,266,267,268,269,270
  };

int SmoothTreesData[] = {
     4, 7, 10,101,
     1,0x0001,243,
     1,0x0115,243,
    30,0x3F, 30,244,0x3F, 60,245,0x3F, 39,246,0x3F, 51,247,
       0x3F, 28,248,0x3F, 29,248,0x3F, 35,249,0x3F, 43,249,
       0x3F, 48,250,0x3F, 50,250,0x3F, 52,250,0x3F,  6,251,
       0x3F, 22,251,0x3F, 38,251,0x3F, 57,252,0x3F, 15,253,
       0x3F, 14,254,0x3F, 46,254,0x3F, 56,255,0x3F, 58,255,
       0x3F,  7,256,0x3F, 23,256,0x3F, 49,257,0x3F, 53,257,
       0x3F, 47,258,0x3F, 59,259,0x3F, 31,260,0x3F, 61,261,
       0x3F, 62,262,0x3F, 55,263,
     7,264,265,266,267,268,269,270
  };

int UnSmoothTreesData[] = {
     4, 7, 8, 9,
     1,0x011C,243,
     0,
     0,
     1,243
  };




int  SmoothBanksData [] = {
     4, 7, 16, 77,
     1, 0x0103, 95,    // was zu ersetzen ist         - blaues wasser }
     4, 0x010F, 95,    // was als 1 zu betrachten ist - wasser und strand }
        0x010E,  0,                                // - Hafen }
        0x0109,110,                                // - Steine und Schilf }
        0x0107,121,                                // - Schilf }
    20,0x3F,59, 98, 0x3F,51, 98, 0x3F,47, 99, 0x3F,39, 99, // durch was ersetzen }
       0x3F,31,100, 0x3F,30,100, 0x3F,61,101, 0x3F,60,101,
       0x3F,55,102, 0x3F,62,103, 0x3F,35,104, 0x3F,28,105,
       0x3F,56,106, 0x3F,48,106, 0x3F,49,106, 0x3F,57,106,
       0x3F,14,107, 0x3F,15,107, 0x3F, 7,107, 0x3F, 6,107,
     0                                            // wenn nicht gefunden }
  };

int  UnSmoothBanksData [] = {
     4, 7, 8, 9,
     1, 0x010C, 98,          // { alle str"nder ersetzen }
     0,
     0,
     1, 95                  // durch flaches wasser ersetzen }
  };


int  SmoothDarkBanksData [] = {
     4, 7, 16, 77,
     1,0x0103,385,                       // dunkels wasser }
     4,0x0103,385,                       // dunkles wasser }
       0x010A,373,
       0x0104,449,
       0x0104,463,
    20,0x3F,59,373, 0x3F,51,373, 0x3F,47,374, 0x3F,39,374,          // durch was ersetzen }
       0x3F,31,375, 0x3F,30,375, 0x3F,61,376, 0x3F,60,376,
       0x3F,55,377, 0x3F,62,378, 0x3F,35,379, 0x3F,28,380,
       0x3F,56,381, 0x3F,48,381, 0x3F,49,381, 0x3F,57,381,
       0x3F,14,382, 0x3F,15,382, 0x3F, 7,382, 0x3F, 6,382,
     0                                       // wenn nicht gefunden }
  };

int  UnSmoothDarkBanksData [] = {
     4, 7, 8, 9,
     1,0x010A,373,
     0,
     0,
     1,385
  };





int IsInSetOfWord( int Wert, int* A )
{
  int Pos = 0;
  int Anz1 = A[Pos];
  Pos++;
  int res = 0;
  int Anz2;
  while ( Anz1 > 0 ) {
    int W = A[Pos];
    Pos++;
    Anz2 = W & 0xff;

    if ( W & 0x100 ) {
       if (( Wert>= A[Pos]) && (Wert< A[Pos]+Anz2)) 
            res = 1;
          Pos++;
    } else {
       while ( Anz2 > 0) { 
         if ( Wert = A[Pos] )
            res = 1;
         Pos++;
         Anz2--;
       }
    }

    Anz1--;
  }

  return res;
}


int  GetNeighbourMask( int x, int y, int* Arr, pobjecttype o )
{
   int res = 0;
   for ( int d = 0; d < sidenum; d++ ) {
      int x1 = x;
      int y1 = y;
      getnextfield ( x1, y1, d );
      pfield fld = getfield ( x1, y1 );
      if ( fld ) {
         
         pobject obj = fld->checkforobject ( o );
         if ( obj )                               
            if ( obj->typ->picture[0] && (obj->typ->weather & 1) )
               if ( IsInSetOfWord ( obj->typ->picture[0][ obj->dir ].bi3pic, Arr ))
                  res += 1 << d;
         
         // if ( fld->checkforobject ( o ) )
         //    res += 1 << d;
      } else
         res += 1 << d;

   }
   return res;
}

int  GetNeighbourMask( int x, int y, int* Arr )
{
   int res = 0;
   for ( int d = 0; d < sidenum; d++ ) {
      int x1 = x;
      int y1 = y;
      getnextfield ( x1, y1, d );
      pfield fld = getfield ( x1, y1 );
      if ( fld ) {
         
         if ( IsInSetOfWord ( fld->typ->bi_picture[ fld->direction ], Arr ))
            res += 1 << d;
         
      } else
         res += 1 << d;

   }
   return res;
}


/*
      Res:= 0;
      for I:= Oben to LOben do begin
        GetNear(P, I, R);
        if ValidEck(R) then
          Res:= Res or
 (Byte(IsInSetOfWord(  TRawArrEck(   Mission.ACTN[R.Y, R.X] )[TerObj], Arr)) shl Ord(I))
        else
          Res:= Res or 1 shl Ord(I);
      end;
      GetNeighbourMask:= Res;
    end;
*/


int SearchAndGetInt( int Wert, int* Arr, int* Res )
{
   int Anz = Arr[0];
   int Pos = 1;
   while ( Anz> 0 ) {
     if (( Wert & Arr[Pos]) == Arr[Pos+1] ) {
        *Res = Arr[Pos+2];
        return  Anz > 0;
     }
     Pos += 3;
     Anz--;
   }
   return  Anz> 0;
}


int SmoothIt( pobjecttype TerObj, int* SmoothData )
{
  int P0 = SmoothData[0];
  int P1 = SmoothData[1];
  int P2 = SmoothData[2];
  int P3 = SmoothData[3];
  int Res = 0;
  for ( int Y = 0 ; Y < actmap->ysize; Y++ )
    for ( int X = 0; X < actmap->xsize; X++ ) {
        if ( TerObj ) {
           pobject obj = getfield ( X, Y )-> checkforobject ( TerObj );
           if ( obj  && obj->typ->picture[0] && (obj->typ->weather & 1)) {
              int Old = obj->dir; // bipicnum
                                  //    Old:= TRawArrEck(Mission.ACTN[Y, X])[TerObj];  // bisherige Form / oder Bildnummer ?
                                                   
              if ( IsInSetOfWord( obj->typ->picture[0][ obj->dir ].bi3pic, &SmoothData[P0] )) {    // Nur die "allesWald"-fielder werden gesmootht
                 int Mask = GetNeighbourMask( X, Y, &SmoothData[P1], TerObj );
                 if ( Mask < 63 ) {
                    int nw;
                    if ( !SearchAndGetInt(Mask, &SmoothData[P2], &nw) ) {  // Wenn kein passendes field gefunden wurde
                       if ( SmoothData[P3] == 0  ||  SmoothData[P3] == 1 )
                          nw = SmoothData[P3+ 1];
                       else 
                          nw = SmoothData[P3+ 1 ]; // + (ticker % SmoothData[P3] )
                    }                      
                    for ( int i = 0; i < TerObj->pictnum; i++ )
                       if ( TerObj->picture[0][ i ].bi3pic == nw )
                          obj->dir = i;
                 }
              }
              if ( Old != obj->dir )
                 Res = 1;
           }
        } else {
           pfield fld = getfield ( X, Y );
           pwterraintype old = fld->typ;
           int odir = fld->direction;
   
           if ( IsInSetOfWord( fld->typ->bi_picture[ fld->direction ], &SmoothData[P0] )) {    // Nur die "allesWald"-fielder werden gesmootht
              int Mask = GetNeighbourMask( X, Y, &SmoothData[P1] );
              if ( Mask < 63 ) {
                 int nw;
                 if ( !SearchAndGetInt(Mask, &SmoothData[P2], &nw) ) {  // Wenn kein passendes field gefunden wurde
                    if ( SmoothData[P3] == 0  ||  SmoothData[P3] == 1 )
                       nw = SmoothData[P3+ 1];
                    else 
                       nw = SmoothData[P3+ 1 ]; // + (ticker % SmoothData[P3] )
                 }
                 for ( int i = 0; i < terraintypenum; i++ ) {
                    pterraintype trrn = getterraintype_forpos( i );
                    if ( trrn )
                       for ( int j = 0; j < cwettertypennum; j++ )
                          if ( trrn->weather[j] )
                             for ( int k = 0; k < sidenum; k++ )
                                if ( trrn->weather[j]->picture[k] )
                                   if ( trrn->weather[j]->bi_picture[k] == nw ) {
                                      fld->typ = trrn->weather[j];
                                      fld->direction = k;
                                      fld->setparams();
                                   }
                 }
              }
           }
           if ( old != fld->typ  ||  odir != fld->direction )
              Res = 1;
           
        }
     }
  return Res;
}



void smooth ( int what )
{
  int ShowAgain = 0;
  if ( what & 2 ) {
    if ( SmoothIt( NULL, UnSmoothBanksData) ) 
       ShowAgain = 1;
    if ( SmoothIt( NULL, UnSmoothDarkBanksData) ) 
       ShowAgain = 1;
    if ( SmoothIt( NULL, SmoothBanksData) ) 
       ShowAgain = 1;
    if ( SmoothIt( NULL, SmoothDarkBanksData) ) 
       ShowAgain = 1;
  }

  if ( what & 1 ) {        
     pobjecttype obj = getobjecttype_forid ( woodid );
     if ( obj ) {
       int count = 0; 
       while ( SmoothIt ( obj, SmoothTreesData0 ) && count < 20 ) {
          ShowAgain = 1;
          count++;
       }
       if  ( SmoothIt ( obj, SmoothTreesData) )  
          ShowAgain = 1;
     }
  }
/*    while SmoothIt(1, SmoothDarkTreesData0) do ShowAgain:= true;
    if SmoothIt(1, SmoothDarkTreesData) then ShowAgain:= true;
    if ShowAgain then begin
      ShowAll;
      Repaint;
    end;    */
}


/*
procedure TMissView.Smooth;
  var
    ShowAgain: Boolean;
  begin
    ShowAgain:= false;
    if SmoothIt(0, SmoothBanksData) then ShowAgain:= true;
    if SmoothIt(0, SmoothDarkBanksData) then ShowAgain:= true;

    while SmoothIt(1, SmoothTreesData0) do ShowAgain:= true;
    if SmoothIt(1, SmoothTreesData) then ShowAgain:= true;
{    while SmoothIt(1, SmoothDarkTreesData0) do ShowAgain:= true;
    if SmoothIt(1, SmoothDarkTreesData) then ShowAgain:= true;}
    if ShowAgain then begin
      ShowAll;
      Repaint;
    end;
  end;

function IsInSetofWord(Wert: Word; var A: array of Word): Boolean;
  var
    Anz1: Word;
    Anz2: Word;
    W: Word;
    Pos: Word;
  begin
    Pos:= 0;
    Anz1:= A[Pos];
    Inc(Pos);
    IsInSetOfWord:= false;
    while Anz1> 0 do begin
      W:= A[Pos];
      Inc(Pos);
      Anz2:= Lo(W);
      case Hi(W) of
        0: begin
            while Anz2> 0 do begin
              if Wert= A[Pos] then IsInSetOfWord:= true;
              Inc(Pos);
              Dec(Anz2);
            end;
          end;
        1: begin
            if (Wert>= A[Pos]) and (Wert< A[Pos]+Anz2) then
              IsInSetOfWord:= true;
            Inc(Pos);
          end;
      end;
      Dec(Anz1);
    end;
  end;



****************
Smoothdaten
****************

{
  Aufbau eines Set of Ints

  1. word anzahl der bl"cke
  dann folgen die bl"cke

  aufbau eines blocks:
    1. word:
       hibyte- optionsnummer
         0- nur rawdaten   lobyte ist anzahl der folgenden raws
         1- ab hier        lobyte ist anzahl der ab hier
    weitere ist entweder raw oder startzahl
}


  SmoothBanksData: array[0..77] of Word= (
     4, 7, 16, 77,
     1, $0103, 95,    { was zu ersetzen ist         - blaues wasser }
     4, $010F, 95,    { was als 1 zu betrachten ist - wasser und strand }
        $010E,  0,                                { - Hafen }
        $0109,110,                                { - Steine und Schilf }
        $0107,121,                                { - Schilf }
    20,$3F,59, 98, $3F,51, 98, $3F,47, 99, $3F,39, 99, { durch was ersetzen }
       $3F,31,100, $3F,30,100, $3F,61,101, $3F,60,101,
       $3F,55,102, $3F,62,103, $3F,35,104, $3F,28,105,
       $3F,56,106, $3F,48,106, $3F,49,106, $3F,57,106,
       $3F,14,107, $3F,15,107, $3F, 7,107, $3F, 6,107,
     0                                            { wenn nicht gefunden }
  );

  UnSmoothBanksData: array[0..10] of Word= (
     4, 7, 8, 9,
     1, $010C, 98,          { alle str"nder ersetzen }
     0,
     0,
     1, 95                  { durch flaches wasser ersetzen }
  );


  SmoothDarkBanksData: array[0..77] of Word= (
     4, 7, 16, 77,
     1,$0103,385,                       { dunkels wasser }
     4,$0103,385,                       { dunkles wasser }
       $010A,373,
       $0104,449,
       $0104,463,
    20,$3F,59,373, $3F,51,373, $3F,47,374, $3F,39,374,          { durch was ersetzen }
       $3F,31,375, $3F,30,375, $3F,61,376, $3F,60,376,
       $3F,55,377, $3F,62,378, $3F,35,379, $3F,28,380,
       $3F,56,381, $3F,48,381, $3F,49,381, $3F,57,381,
       $3F,14,382, $3F,15,382, $3F, 7,382, $3F, 6,382,
     0                                       { wenn nicht gefunden }
  );

  UnSmoothDarkBanksData: array[0..10] of Word= (
     4, 7, 8, 9,
     1,$010A,373,
     0,
     0,
     1,385
  );

*/


#endif // Hexagon



void swapbuildings ( pbuilding building, pbuilding orgbuilding )
{
      npush ( orgbuilding->next );
      npush ( orgbuilding->prev );

      npush ( building->next );
      npush ( building->prev );

      tbuilding temp = *orgbuilding;
      *orgbuilding = *building;
      *building = temp;

      npop ( building->prev );
      npop ( building->next );

      npop ( orgbuilding->prev );
      npop ( orgbuilding->next );
}

/*
void swapvehicles ( pvehicle orgvehicle, pvehicle transport )
{
      npush ( orgvehicle->next );
      npush ( orgvehicle->prev );

      npush ( transport->next );
      npush ( transport->prev );

      tvehicle temp = *orgvehicle;
      *orgvehicle = *transport;
      *transport = temp;

      npop ( transport->prev );
      npop ( transport->next );

      npop ( orgvehicle->prev );
      npop ( orgvehicle->next );
}
*/





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
         if ( mouseparams.taste == gameoptions.mouse.scrollbutton ) {
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




//////  Functions, that belong to TYPEN.CPP , but need functions defined here ...





#ifndef sgmain
void tbuilding :: changecompletion ( int d ) {}
int  tbuilding :: getenergyplus(  int mode  ) { return 0; }
int  tbuilding :: getmaterialplus(  int mode  ) { return 0; }
int  tbuilding :: getfuelplus(  int mode  ) { return 0; }
void tbuilding :: produceenergy( void ) { }
void tbuilding :: producematerial( void ) {}
void tbuilding :: producefuel( void ) {}
void tbuilding :: execnetcontrol ( void ) {}
int  tbuilding :: processmining ( int res, int abbuchen ) { return 0; }
int  tbuilding :: put_energy ( int      need,    int resourcetype, int queryonly  )  { return 0; }
int  tbuilding :: get_energy ( int      need,    int resourcetype, int queryonly  )  { return 0; }
void tbuilding :: getresourceusage ( tresources* usage ) { usage->a.energy = 0;
                                                           usage->a.material =  0;
                                                           usage->a.fuel = 0;
                                                         }
int tvehicle::enablereactionfire( void ) { return 0; }
int tvehicle::disablereactionfire ( void ) { return 0; }

#else
void tbuilding :: changecompletion ( int d )
{
  completion += d;
  resetbuildingpicturepointers ( this );
}

#endif
   


/** Returns the SingleWeapon corresponding to the weaponNum for this
 *  vehicle.
 */
SingleWeapon *tvehicle::getWeapon( unsigned weaponNum ) {
  printf( "getWeapon(%u)\n", weaponNum );
  UnitWeapon *weapons=typ->weapons;
  return (weaponNum<=weapons->count)?weapons->weapon+weaponNum:NULL;;
}

int tvehicle :: buildingconstructable ( pbuildingtype building )
{
   if ( !building )
      return 0;

   int mf = 100;
   int ff = 100;

   if ( actmap->gameparameter ) {
      if ( actmap->gameparameter[cgp_forbid_building_construction] )
         return 0;

      if ( actmap->gameparameter[cgp_building_material_factor] )
         mf = actmap->gameparameter[cgp_building_material_factor];

      if ( actmap->gameparameter[cgp_building_fuel_factor] )
         ff = actmap->gameparameter[cgp_building_fuel_factor];
   }

   if ( building->produktionskosten.material * mf / 100 <= material   &&   building->produktionskosten.sprit * ff / 100 <= fuel ) {
      int found = 0;
      if ( functions & cfputbuilding )
         found = 1;
      if ( functions & cfspecificbuildingconstruction )
         for ( int i = 0; i < typ->buildingsbuildablenum; i++ )
            if ( typ->buildingsbuildable[i].from <= building->id &&
                 typ->buildingsbuildable[i].to   >= building->id )
                 found = 1;

      if ( found ) {
         if ( actmap->objectcrc ) {
            if ( actmap->objectcrc->speedcrccheck->checkbuilding2 ( building, 0 ))
               return true;
            else
               return false;
         } else
           return true;
      } else
         return false;
   } else
      return false;
}


int tvehicle :: searchstackforfreeweight ( pvehicle eht, int what )
{
   if ( eht == this ) {
      if ( what == 1 ) // material or fuel
         return maxint;  
      else
         return typ->loadcapacity - cargo();
        // return typ->maxweight() + typ->loadcapacity - weight();
   } else {
      int w1 = typ->maxweight() + typ->loadcapacity - weight();
      int w2 = -1;
      for ( int i = 0; i < 32; i++ ) 
         if ( loading[i] ) {
            int w3 = loading[i]->searchstackforfreeweight ( eht, what );
            if ( w3 >= 0 )
               w2 = w3;
         }

      if ( w2 != -1 ) 
         if ( w2 < w1 )
            return w2;
         else
            return w1;
      else
         return -1;
   }
}

int tvehicle :: freeweight ( int what )
{
   pfield fld = getfield ( xpos, ypos );
   if ( fld->vehicle ) 
        return fld->vehicle->searchstackforfreeweight ( this, what );
   else
      if ( fld->building ) {
         for ( int i = 0; i < 32; i++ )
            if ( fld->building->loading[i] ) {
               int w3 = fld->building->loading[i]->searchstackforfreeweight ( this, what );
               if ( w3 >= 0 )
                  return w3;
            }
      }

   return -2;
}

int tvehicle::getmaxfuelforweight ( void )
{
   pfield fld = getfield ( xpos, ypos );
   if ( fld->vehicle  &&  fld->vehicle != this ) {
      int fw = freeweight( 1 );
      if ( fw >= 0 ) {
         int maxf = fw * 1024 / fuelweight;
         if ( maxf > typ->tank || maxf < 0 )
            return typ->tank;
         else 
            return maxf;
      } else
         return typ->tank;
   } else
      return typ->tank;
}


int tvehicle::getmaxmaterialforweight ( void )
{
   pfield fld = getfield ( xpos, ypos );
   if ( fld->vehicle  &&  fld->vehicle != this ) {
      int fw = freeweight( 1 );
      if ( fw >= 0 ) {
         int maxm = fw * 1024 / materialweight;
         if ( maxm > typ->tank )
            return typ->material;
         else 
            return maxm;
      } else
         return typ->material;

  } else
      return typ->material;
}



int  tvehicle :: vehicleloadable ( pvehicle vehicle, int uheight )
{
   if ( uheight == -1 )
      uheight = vehicle->height;

   if ( uheight != vehicle->height  &&  uheight != height )
      return 0;

   if (( ( typ->loadcapability    & vehicle->height)   &&   
         (( typ->loadcapabilityreq & vehicle->typ->height) || !typ->loadcapabilityreq ) && 
         ((typ->loadcapabilitynot & vehicle->typ->height) == 0))
        || (vehicle->functions & cf_trooper )) {

      if ( typ->maxunitweight >= vehicle->weight() )
         if ( (cargo() + vehicle->weight() <= typ->loadcapacity) && 
              (vehiclegeladen( this ) + 1 < maxloadableunits)) {
              // && ( height <= chfahrend )) 

                 if ( getfield ( xpos, ypos )->vehicle != this )
                    return 2;
               #ifdef karteneditor
                  return 2;
               #else
                 if ( uheight != vehicle->height &&
                      height == uheight)  
                      return 2;
                 else
                    if (vehicle->height == chtieffliegend) 
                       if (vehicle->typ->steigung <= flugzeugtraegerrunwayverkuerzung) 
                          return 2; 
                       else 
                          return 0; 
                    else 
                       if (vehicle->height == chfahrend) { 
                           if ((height >= chschwimmend) && 
                               (height <= chfahrend)) 
                               return 2; 
                           else
                               return 0;
                       } else
                          if ( vehicle->height == height )
                             return 2;
               #endif
              }

   } 
   return 0;
}


void* tbuilding :: getpicture ( int x, int y )
{
//                      if ( bld->typ->id == 8 ) {          // Windkraftwerk

   pfield fld = getbuildingfield ( this, x, y );
   if ( fld ) {
      int w = fld->getweather();
   
      #ifdef HEXAGON
       if ( typ->w_picture[w][completion][x][y] )
          return typ->w_picture[w][completion][x][y];
       else
          return typ->w_picture[0][completion][x][y];
      #else
       return typ->picture[completion][x][y];
      #endif
   } else
      return NULL;
}


int tbuilding :: vehicleloadable ( pvehicle vehicle, int uheight )
{
   if ( uheight == -1 )
      uheight = vehicle->height;

      int hgt = uheight; // vehicle->height;

      if ( completion ==  typ->construction_steps - 1 ) {
         if ( (( typ->loadcapacity >= vehicle->size())               // the unit is physically able to get "through the door"
           && (vehiclegeparkt(this)+1 < maxloadableunits )
           && ( typ->loadcapability & vehicle->height )
           && ( typ->unitheightreq & vehicle->typ->height )
           && !( typ->unitheight_forbidden & vehicle->typ->height) ) 
                   ||  
             (( vehicle->functions & cf_trooper ) 
           && ( (uheight == typ->buildingheight)  || (typ->buildingheight >= chschwimmend && hgt == chfahrend) ))) { 
              
         #ifdef karteneditor
              return 2;
         #else
              if ( color == actmap->actplayer * 8) 
                 return 2;
              else 
                if ( !vehicle->attacked ) {
                   if ( color == (8 << 3) )      // neutral building can be conquered by any unit
                      return 2;
                   else
                      if ( (vehicle->functions & cf_conquer)  || ( damage >= mingebaeudeeroberungsbeschaedigung))
                         return 2;
                }    
         #endif
         }
      }
      
/*
&&
         (
         (( color == actmap->actplayer * 8)                              // ganz regulÑr: eigenes gebÑude
         
         || (( vehicle->functions & cftrooper )                // JEDES GebÑude mu· sich mit Fusstruppen erobern lassen
         && (( uheight == typ->height ) || (typ->height >= chschwimmend && hgt == chfahrend))  
         && ( !vehicle->attacked ))
         // && color != (8 << 3)) )
         //&& ( typ->loadcapability & hgt ))
         ||
         
         ( (( damage >= mingebaeudeeroberungsbeschaedigung) || ( vehicle->functions & cfconquer ))    // bei BeschÑdigung oder cfconquer jedes GebÑude mit fahrenden vehicle
         && (vehicle->height == chfahrend)
         // && ( color != (8 << 3))
         && ( !vehicle->attacked ) 
         && ( typ->loadcapability & hgt )
         && ( typ->height & vehicle->typ->height ))
         ||
         
         (( color == )                               // neutrale GebÑude lassen sich immer erobern
         // && (vehicle->height == chfahrend)
         && ( !vehicle->attacked ) 
         && ( typ->loadcapability & hgt )
         && ( typ->height & vehicle->typ->height ) ))
       )

          return 2; 
      else 
           return 0;
*/
   return 0;
}




void  tfield :: addobject( pobjecttype obj, int dir, int force )
{ 
   if ( !obj )
      return;

   pobject i = checkforobject ( obj );
   if ( !i ) {
     int buildable = obj->buildable ( this );
     #ifdef karteneditor
     if ( !buildable ) 
          if ( force )
             buildable = 1;
          else
             if (choice_dlg("object cannot be built here","~i~gnoe","~c~ancel") == 1) 
                buildable = 1;
     #endif

     if ( buildable ) {
         if ( !object )
            object = new tobjectcontainer;
         else
            if ( object->objnum == maxobjectonfieldnum-1 ) {
               displaymessage("can not add any more objects to this field", 1 );
               return;
            }

         object->object[ object->objnum ] = new tobject ( obj ) ;
         object->object[ object->objnum ]->time = actmap->time.a.turn;
         if ( dir != -1 )
            object->object[ object->objnum ]->dir = dir;
         else
            object->object[ object->objnum ]->dir = 0;
         object->objnum++;
   
         setparams();

         if ( dir == -1 )
            calculateobject( getx(), gety(), true, obj ); 

         sortobjects();
     }
   } else {
      if ( dir != -1 ) 
         i->dir |= dir;
      
      sortobjects();
   } 
} 


void tfield :: removeobject( pobjecttype obj )
{ 
   if ( building ) 
      return;

   #ifndef karteneditor
   if ( vehicle ) 
      if ( vehicle->color != actmap->actplayer << 3)
        return;
   #endif

   if ( !object )
      return;

   for ( int i = 0; i < object->objnum; )
      if ( object->object[i]->typ == obj ) {
         delete object->object[i];
         for ( int j = i+1; j < object->objnum; j++ )
            object->object[j-1] = object->object[j];

         object->objnum--;
      } else
         i++;

    setparams();

    calculateobject( getx(), gety(), true, obj );
} 

void tfield :: deleteeverything ( void )
{
   if ( vehicle )
      removevehicle ( &vehicle );

   if ( building )
      removebuilding ( &building );

   if ( object ) {
      for ( int i = 0; i < object->objnum; i++) {
          delete object->object[i];
          object->object[i] = NULL;
      }
      delete object;
      object = NULL;
   }
   setparams();
}



void tvehicle::convert ( int col )
{
  if ( col > 8)   
      displaymessage("convertvehicle: \n color mu· im bereich 0..8 sein ",2);

   int oldcol = color >> 3; 
   if ( !prev && !next ) { 
      actmap->player[oldcol].firstvehicle = NULL; 
     #ifndef karteneditor
      quedevents[oldcol]++;
     #endif
   } 
   else { 
      if ( prev ) 
         prev->next = next; 
      else 
         actmap->player[oldcol].firstvehicle = next; 
         
      if ( next ) 
         next->prev = prev; 
      
   } 

   color = col << 3; 

   pvehicle pe = actmap->player[ col ].firstvehicle; 
   actmap->player[ col ].firstvehicle = this; 
   prev = NULL; 
   next = pe;
   if ( pe )
      pe->prev = this;

   for ( int i = 0; i < 32; i++) 
      if ( loading[i] ) 
         loading[i]->convert( col );

   #ifndef karteneditor
      if ( connection & cconnection_conquer )
         releaseevent( this, NULL,cconnection_conquer) ;
      if ( connection & cconnection_lose ) 
         releaseevent( this, NULL,cconnection_lose); 
   #endif      
}

void tvehicle :: constructvehicle ( pvehicletype tnk, int x, int y )
{
   if ( vehicleconstructable( tnk, x, y )) {
      pvehicle v;
      generate_vehicle ( tnk, color/8, v );
      v->xpos = x;
      v->ypos = y;
      getfield ( x, y )->vehicle = v;
      material -= tnk->production.material;
      fuel -= tnk->production.energy;

      int refuel = 0;
      for ( int i = 0; i < typ->weapons->count; i++ )
         if ( typ->weapons->weapon[i].service()  )
            for ( int j = 0; j < typ->weapons->count ; j++) {
               if ( typ->weapons->weapon[j].canRefuel() )
                  refuel = 1;
               if ( functions & (cffuelref | cfmaterialref) )
                  refuel = 1; 
            }
      if ( refuel ) 
         verlademunition( v, this ,NULL, 3 ); 
      v->attacked = 1;
      

   }
}

int  tvehicle :: vehicleconstructable ( pvehicletype tnk, int x, int y )
{
   if ( tnk->terrainaccess->accessible ( getfield(x,y)->bdt ) > 0 ) 
      if ( tnk->production.material <= material &&
           tnk->production.energy   <= fuel )
           if ( beeline ( x, y, xpos, ypos ) <= maxmalq )
              if ( tnk->height & height )
                 return 1;

   
   return 0;
}


void tbuilding :: convert ( int col )
{ 
                    
   if (col > 8)
      displaymessage("convertbuilding: \n color mu· im bereich 0..8 sein ",2);

   int oldcol = color >> 3; 

   #ifdef sgmain
   if ( oldcol == 8 ) 
      if ( actmap->resourcemode == 1 ) {
          actmap->bi_resource[col].a.energy += actstorage.a.energy;
          actstorage.a.energy = 0;

          actmap->bi_resource[col].a.fuel += actstorage.a.fuel;
          actstorage.a.fuel = 0;
      }
   #endif 

   if ( !prev && !next ) { 
      actmap->player[oldcol].firstbuilding = NULL; 
      #ifndef karteneditor
        quedevents[oldcol]++;
      #endif
   } 
   else { 

      if ( prev ) 
         prev->next = next; 
      else 
         actmap->player[oldcol].firstbuilding = next; 

      if ( next ) 
         next->prev = prev; 
       
   } 
   color = col << 3; 

   pbuilding pe = actmap->player[col].firstbuilding; 
   actmap->player[ col ].firstbuilding = this; 
   prev = NULL; 
   next = pe;
   if ( pe )
      pe->prev = this;

   for ( int i = 0; i < 32; i++) 
      if ( loading[i] ) {
         loading[i]->convert ( col );
//         loading[i]->cmpchecked = 0; 
      } 

   #ifndef karteneditor
      if ( connection & cconnection_conquer ) 
         releaseevent(NULL,this,cconnection_conquer); 
      if ( connection & cconnection_lose ) 
         releaseevent(NULL,this,cconnection_lose); 
   #endif

} 

int tvehicle :: getstrongestweapon( int aheight, int distance)
{ 
   int str = 0; 
   int hw = 255;   /*  error-wert  ( keine waffe gefunden )  */ 
   for ( int i = 0; i < typ->weapons->count; i++) { 

      if (( ammo[i]) && 
          ( typ->weapons->weapon[i].mindistance <= distance) && 
          ( typ->weapons->weapon[i].maxdistance >= distance) &&
          ( typ->weapons->weapon[i].targ & aheight ) &&
          ( typ->weapons->weapon[i].sourceheight & height )) {
            int astr = weapstrength[i] * weapdist->getweapstrength( &typ->weapons->weapon[i], distance, height, aheight) / 256;
            if ( astr > str ) {
               str = astr;
               hw  = i;
            }
       }
   } 
   return hw; 
} 

int tbuildingtype :: gettank ( int resource )
{
   #ifdef HEXAGON
   if ( actmap && actmap->resourcemode == 1 )
      return _bi_maxstorage.resource[resource];
   else
   #endif
   return _tank.resource[resource];
}



int tobjecttype :: connectablewithbuildings ( void )
{
  if ( this == streetobjectcontainer || this == railroadobject  ||  this == pathobject || id == 1000 )
     return 1;
  else
     return 0;
}



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
