/*! \file spfst.cpp
    \brief map accessing and usage routines used by ASC and the mapeditor
*/

//     $Id: spfst.cpp,v 1.81 2001-01-28 14:04:19 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
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
#include <string.h>
#include <utility>
#include <map>


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

#ifndef karteneditor
  #include "missions.h"
  #include "gamedlg.h"
#endif

#include "dialog.h"
#include "loadbi3.h"
#include "mapalgorithms.h"
#include "mapdisplay.h"



   int dataVersion = 0;

   tcursor            cursor;
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


   char tempsvisible;
   Schriften schriften; 

   map< int, pterraintype>  terrainmap;
   map< int, pobjecttype>  objectmap;
   map< int, pvehicletype>  vehiclemap;
   map< int, pbuildingtype>  buildingmap;
   map< int, ptechnology>  technologymap;

pterraintype getterraintype_forid ( int id, int crccheck )
{
   return terrainmap[id];
}
pobjecttype getobjecttype_forid ( int id, int crccheck  )
{
   return objectmap[id];
}
pvehicletype getvehicletype_forid ( int id, int crccheck  )
{
   return vehiclemap[id];
}
pbuildingtype getbuildingtype_forid ( int id, int crccheck  )
{
   return buildingmap[id];
}
ptechnology gettechnology_forid ( int id, int crccheck  )
{
   return technologymap[id];
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
      terrainmap[bdt->id] = bdt;
   }
}
void addobjecttype ( pobjecttype obj )
{
   if ( obj ) {
      objecttypes[ objecttypenum++] = obj;
      objectmap[obj->id] = obj;
   }
}
void addvehicletype ( pvehicletype vhcl )
{
   if ( vhcl ) {
      vehicletypes[ vehicletypenum++] = vhcl;
      vehiclemap[vhcl->id] = vhcl;
   }
}
void addbuildingtype ( pbuildingtype bld )
{
   if ( bld ) {
      buildingtypes[ buildingtypenum++] = bld;
      buildingmap[bld->id] = bld;
   }
}
void addtechnology ( ptechnology tech )
{
   if ( tech ) {
      technology[ technologynum++] = tech;
      technologymap[tech->id] = tech;
   }
}







#ifdef _NOASM_

int  rol ( int valuetorol, int rolwidth )
{
   int newvalue = valuetorol << rolwidth;
   newvalue |= valuetorol >> ( 32 - rolwidth );
   return newvalue;
}

#endif











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

   if ( c == visible_all)
      if ( field->minenum() ) 
         if (vehicle->height <= chfahrend && getdiplomaticstatus2 ( vehicle->color, field->mineowner()*8 ) == cawar )  
            return 0;
      

   if ( !field->vehicle && !field->building ) {
      return terrainaccessible ( field, vehicle, uheight );
   } else {
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


      if (field->vehicle) {
         if (field->vehicle->color == vehicle->color) { 
            int ldbl = field->vehicle->vehicleloadable ( vehicle, uheight );
            if ( ldbl )
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
      else {   /*  geb„ude  */ 
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




void         generatemap( TerrainType::Weather*   bt,
                               int                xsize,
                               int                ysize)
{ 
   tfield    leerfield;

   delete actmap;
   actmap = new tmap;
   actmap->playerView = 0;

   actmap->xsize = xsize;
   actmap->ysize = ysize; 
   actmap->campaign = NULL;
   for (int k = 1; k < 8; k++) 
      actmap->player[k].stat = ps_computer;

   actmap->title = strdup( "new map" );
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


   for ( i = 0; i < gameparameternum; i++ )
      actmap->setgameparameter(i, gameparameterdefault[i] );

   #ifdef HEXAGON
   actmap->_resourcemode = 1;
   #else
   actmap->_resourcemode = 0;
   #endif

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

      if ( actmap->player[player].ai && actmap->player[player].ai->isRunning() )
         if ( c < actmap->player[player].ai->getVision() )
            c = actmap->player[player].ai->getVision();

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


int fieldVisibility( const pfield pe, int player )
{
  if ( pe && player >= 0 ) {
      int c = (pe->visible >> ( player * 2)) & 3;
      #ifdef karteneditor
         c = visible_all;
      #endif

      if ( actmap->player[player].ai && actmap->player[player].ai->isRunning() )
         if ( c < actmap->player[player].ai->getVision() )
            c = actmap->player[player].ai->getVision();

      return c;
   } else
      return visible_not;
}


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

   char *d = &actmap->alliances[ b/8 ][ actmap->actplayer ] ;

   if ( *d == capeace || *d == canewsetwar1 || *d == cawarannounce )
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


   char *d = &actmap->alliances [ b/8][ c/8 ];

   if ( *d == capeace || *d == canewsetwar1 || *d == cawarannounce )
      return capeace;
   else
      return cawar;
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




void         calculateobject(int      x,
                              int      y,
                              bool      mof,
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
                  if ( i->time + actmap->getgameparameter ( cgp_fahrspur ) < actmap->time.a.turn )
                     getfield ( x, y ) -> removeobject ( fahrspurobject );

            i = actmap->field[l].checkforobject ( eisbrecherobject );
            if ( i ) 
               if ( actmap->getgameparameter ( cgp_eis ) > 0 )
                  if ( i->time + actmap->getgameparameter ( cgp_eis ) < actmap->time.a.turn )
                     getfield ( x, y ) -> removeobject ( eisbrecherobject );

            getfield ( x, y )->checkminetime ( actmap->time.a.turn );
            l++;
         } 
} 


void         removevehicle(pvehicle*   vehicle)
{ 
  pvehicle     p1; 
  int         b; 

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
         for ( int i = 0; i < 8; i++ )
            actmap->player[i].queuedEvents++;
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
  int         b; 
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
         for ( int i = 0; i < 8; i++ )
            actmap->player[i].queuedEvents++;
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
  int         dx; 



   if ( color & 7 )
      displaymessage("putbuilding muá eine farbe aus 0,8,16,24,.. ?bergeben werden !",2); 


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


   gbde = new Building ( actmap );
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
   gbde->plus.energy = 0;
   gbde->plus.material = 0;
   gbde->plus.fuel = 0;
   gbde->maxplus.energy = gbde->typ->maxplus.energy;
   gbde->maxplus.material = gbde->typ->maxplus.material;
   gbde->maxplus.fuel = gbde->typ->maxplus.fuel;

   gbde->actstorage.fuel = 0;
   gbde->actstorage.material = 0;
   gbde->actstorage.energy = 0;
   gbde->maxresearchpoints = 0; 
   gbde->researchpoints = 0; 
   gbde->netcontrol = 0;
   gbde->connection = 0; 
   gbde->visible = true; 
   gbde->completion = compl; 
   
   gbde->chainbuildingtofield ( x, y );

} 


void         putbuilding2(integer      x,
                          integer      y,
                          int         color,
                          pbuildingtype buildingtyp)
{ 
  integer      a, b; 
  pbuilding    gbde; 
  pfield        field; 
  integer      orgx, orgy; 
  int         dx; 

/*
  if ( actmap->objectcrc ) 
     if ( !actmap->objectcrc->speedcrccheck->checkbuilding2 ( buildingtyp ))
        return;
*/

   if ( color & 7 )
      displaymessage("putbuilding muá eine farbe aus 0,8,16,24,.. ?bergeben werden !",2); 

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
      gbde = new Building;
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
         Resources maxplus;
         Resources actplus;
         Resources biplus;
         int maxresearch = 0;

         pbuilding bld = actmap->player[ color/8].firstbuilding;
         while ( bld ) {
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
            bld = bld->next;
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





void         resetallbuildingpicturepointers ( void )
{
   for (int s = 0; s < 8; s++) {
      pbuilding bld = actmap->player[s].firstbuilding;
      while ( bld ) {
         bld->resetPicturePointers ();
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
             if ( eht )
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




#ifdef HEXAGON




/* 
   These smoothing functions are straight conversions from Joerg Richter's routines
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
         if ( Wert == A[Pos] )
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
           TerrainType::Weather* old = fld->typ;
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

      Building temp = *orgbuilding;
      *orgbuilding = *building;
      *building = temp;

      npop ( building->prev );
      npop ( building->next );

      npop ( orgbuilding->prev );
      npop ( orgbuilding->next );
}







//////  Functions, that belong to TYPEN.CPP , but need functions defined here ...









/** Returns the SingleWeapon corresponding to the weaponNum for this
 *  vehicle.
 */
SingleWeapon *Vehicle::getWeapon( unsigned weaponNum ) {
  // printf( "getWeapon(%u)\n", weaponNum );
  UnitWeapon *weapons=typ->weapons;
  return (weaponNum<=weapons->count)?weapons->weapon+weaponNum:NULL;;
}


int tmine :: attacksunit ( const pvehicle veh )
{
     if  (!( ( veh->functions & cfmineimmune ) || 
              ( veh->height > chfahrend ) ||
              ( getdiplomaticstatus2 ( veh->color, color*8 ) == capeace ) || 
              ( (veh->functions & cf_trooper) && (type != cmantipersonnelmine)) || 
              ( veh->height <= chgetaucht && type != cmmooredmine ) || 
              ( veh->height == chschwimmend && type != cmfloatmine ) ||
              ( veh->height == chfahrend && type != cmantipersonnelmine  && type != cmantitankmine )
            ))
         return 1;
     return 0;
}


void tfield :: checkminetime ( int time )
{
   if ( minenum() ) 
      for ( int i = minenum()-1; i >= 0; i-- ) {
         int lt = actmap->getgameparameter ( cgp_antipersonnelmine_lifetime + object->mine[i]->type - 1);
         if ( lt )
            if ( object->mine[i]->time + lt < time )
               removemine ( i );
      }
}


int tfield :: mineattacks ( const pvehicle veh )
{
   int mn = minenum();
   for ( int i = 0; i < mn; i++ ) 
      if ( object->mine[i]->attacksunit ( veh ))
         return 1+i;

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
     #else
     if ( !buildable )
          if ( force )
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



int isUnitNotFiltered ( int id )
{
   if ( unitSets.size() > 0 ) {
      for ( int i = 0; i < unitSets.size(); i++ )
         if ( unitSets[i]->isMember ( id ))
            if ( !unitSets[i]->active )
                return 0;
   }
   return 1;
}

int isBuildingNotFiltered ( int id )
{
   if ( unitSets.size() > 0 ) {
      for ( int i = 0; i < unitSets.size(); i++ )
         if ( unitSets[i]->isMember ( id ))
            if ( !unitSets[i]->active && unitSets[i]->filterBuildings)
                return 0;
   }
   return 1;
}

