//     $Id: edselfnt.cpp,v 1.18 2000-11-21 20:27:02 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.17  2000/10/26 18:55:29  mbickel
//      Fixed crash when editing the properties of a vehicle inside a building
//      Added mapeditorFullscreen switch to asc.ini
//
//     Revision 1.16  2000/10/18 14:14:09  mbickel
//      Rewrote Event handling; DOS and WIN32 may be currently broken, will be
//       fixed soon.
//
//     Revision 1.15  2000/10/12 19:00:21  mbickel
//      Fixed crash in building placement
//      Replaced multi-character character constants by strings (there where
//        problems with the byte order)
//      Building ID and name are now correctly displayed in mapeditor
//
//     Revision 1.14  2000/10/11 14:26:34  mbickel
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
//     Revision 1.13  2000/08/15 16:22:55  mbickel
//      Fixed: crash in mapedit when selecting a unit as buildingcargo
//
//     Revision 1.12  2000/08/06 13:14:16  mbickel
//      Fixed crashes in mapeditor
//
//     Revision 1.11  2000/08/06 11:39:06  mbickel
//      New map paramter: fuel globally available
//      Mapeditor can now filter buildings too
//      Fixed unfreed memory in fullscreen image loading
//      Fixed: wasted cpu cycles in building
//      map parameters can be specified when starting a map
//      map parameters are reported to all players in multiplayer games
//
//     Revision 1.10  2000/08/04 15:11:07  mbickel
//      Moving transports costs movement for units inside
//      refuelled vehicles now have full movement in the same turn
//      terrain: negative attack / defensebonus allowed
//      new mapparameters that affect damaging and repairing of building
//
//     Revision 1.9  2000/08/03 19:21:22  mbickel
//      Fixed: units had invalid height when produced in some buildings
//      Fixed: units could not enter building if unitheightreq==0
//      Started adding SDL_image support
//      Upgraded to SDL1.1.3 (for SDL_image)
//
//     Revision 1.8  2000/06/28 19:26:16  mbickel
//      fixed bug in object generation by building removal
//      Added artint.cpp to makefiles
//      Some cleanup
//
//     Revision 1.7  2000/05/06 19:57:09  mbickel
//      Mapeditor/linux is now running
//
//     Revision 1.6  2000/05/02 16:20:54  mbickel
//      Fixed bug with several simultaneous vehicle actions running
//      Fixed graphic error at ammo transfer in buildings
//      Fixed ammo loss at ammo transfer
//      Movecost is now displayed for mines and repairs
//      Weapon info now shows unhittable units
//
//     Revision 1.5  2000/04/27 16:25:22  mbickel
//      Attack functions cleanup
//      New vehicle categories
//      Rewrote resource production in ASC resource mode
//      Improved mine system: several mines on a single field allowed
//      Added unitctrl.* : Interface for vehicle functions
//        currently movement and height change included
//      Changed timer to SDL_GetTicks
//
//     Revision 1.4  2000/03/29 09:58:45  mbickel
//      Improved memory handling for DOS version
//      Many small changes I can't remember ;-)
//
//     Revision 1.3  1999/12/27 12:59:59  mbickel
//      new vehicle function: each weapon can now be set to not attack certain
//                            vehicles
//
//     Revision 1.2  1999/11/16 03:41:40  tmwilson
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

#include "vehicletype.h"
#include "buildingtype.h"

#include "edmisc.h"
#include "flview.h"
#include "edselfnt.h"
#include "edglobal.h"







class tselfntmousescrollproc : public tsubmousehandler {
         public:
           void mouseaction ( void );
};

tselfntmousescrollproc selfntmousescrollproc;

int selfntmousecurs = 8;
void tselfntmousescrollproc :: mouseaction ( void )
{
   int pntnum = -1;

   void* newpnt = NULL;

   if ( mouseparams.y1 == 0 )
       pntnum = 0;
   else
     if ( mouseparams.y1 + mouseparams.ysize >= hgmp->resolutiony  )
       pntnum = 4;
     else
       pntnum = 8;

   if ( pntnum < 8 )
      newpnt = icons.pfeil2[pntnum];
   else
      newpnt = icons.mousepointer;

   selfntmousecurs = pntnum;

   if ( mouseparams.pictpointer != newpnt ) 
      setnewmousepointer ( newpnt, mousehotspots[pntnum][0], mousehotspots[pntnum][1] );
}


//* õS terrainSelect

// How must a typedef be used to work with templates?
// something like
//template<class T> typedef dynamic_array<T> vect<T> ;

#define vect dynamic_array  

class SelectAnythingBase {
            public:
                virtual void showactiteminfos ( int x1, int y1, int x2, int y2 ) = 0;
                virtual int getiteminfoheight( void ) { return 60; };
                virtual ~SelectAnythingBase ( ) {};
        };




template<class T> 
class SelectAnything : public SelectAnythingBase {
                    protected:
                       tmouserect position;
                       int maxx;  int maxy;
                       int winstartx;  int winstarty;
                       int winsizey;
                       int actitemx;   int actitemy;

                       virtual int isavailable ( T item ) = 0;
                       virtual void displaysingleitem ( T item, int x, int y ) = 0;
                       virtual void _displaysingleitem ( T item, int x, int y );
                       virtual void displaysingleitem ( int itemx, int itemy, int picx, int picy );
                       virtual void displaysingleitem ( int itemx, int itemy );
                       virtual int getxposforitempos ( int itemx );
                       virtual int getyposforitempos ( int itemy );
                       virtual int getitemsizex ( void ) = 0;
                       virtual int getitemsizey ( void ) = 0;
                       virtual int getxgap ( void ) { return 5; };
                       virtual int getygap ( void ) { return 5; };
                       virtual void _showiteminfos ( T item, int x1, int y1, int x2, int y2 );
                       virtual void showiteminfos ( T item, int x1, int y1, int x2, int y2 ) { };
                       virtual void showiteminfos ( T item );
                       virtual void showactiteminfos ( void );
                       vect<T> itemsavail;

                    public:
                       virtual void showactiteminfos ( int x1, int y1, int x2, int y2 );
                       T selectitem ( T previtem, tkey neutralkey = 0 );
                       void init ( vect<T> &v, int x1, int y1, int x2, int y2 ); 
                       void init ( vect<T> &v ); 
                       void display ( void );
                       void setnewselection ( T item );
                  };


template<class T> void SelectAnything<T> :: init ( vect<T> &v ) 
{
   init ( v, selfontxanf, selfontyanf, selfontxanf + selfontxsize, agmp->resolutiony - 20 );
}


template<class T> void SelectAnything<T> :: init ( vect<T> &v, int x1, int y1, int x2, int y2 ) 
{
   itemsavail.reset();

   for ( int i = 0; i <= v.getlength(); i++ )
      if ( isavailable ( v[i] ) )
         itemsavail[ itemsavail.getlength()+1 ] = v[i];
     
  int num = itemsavail.getlength() + 1; 
  if ( num <= 0 ) {
     displaymessage("no items available", 1 );
     // return;
  } 


  position.x1 = x1;
  if ( x1 + getitemsizex() > x2 ) {
     if ( x1 + getitemsizex() >= agmp->resolutionx ) 
        position.x1 = agmp->resolutionx - getitemsizex();
     position.x2 = position.x1 + getitemsizex();
  } else
     position.x2 = x2;
  position.y1 = y1 + getiteminfoheight();
  position.y2 = y2;

  maxx = (position.x2 - position.x1) / ( getitemsizex() + getxgap() );
  if ( maxx < 1 )
     maxx = 1;

  maxy = (num + maxx -1) / maxx;

  actitemx = 0;
  actitemy = 0;
  winstartx = 0;
  winstarty = 0;
  winsizey = ( position.y2 - position.y1 ) / ( getitemsizey() + getygap() );
  if ( winsizey > maxy )
     winsizey = maxy;
}


template<class T> void SelectAnything<T> :: _showiteminfos ( T item, int x1, int y1, int x2, int y2 )
{
   setinvisiblemouserectanglestk ( x1, y1, x2, y2 );
   showiteminfos ( item, x1, y1, x2, y2 );
   getinvisiblemouserectanglestk ();
}


template<class T> void SelectAnything<T> :: showactiteminfos ( int x1, int y1, int x2, int y2 )
{
   int pos = actitemx + actitemy * maxx;
   if ( pos <= itemsavail.getlength() )
      _showiteminfos ( itemsavail[ pos ], x1, y1, x2, y2 );
   else
      _showiteminfos ( NULL, x1, y1, x2, y2 );

}

template<class T> void SelectAnything<T> :: showactiteminfos ( void )
{
   int pos = actitemx + actitemy * maxx;
   if ( pos <= itemsavail.getlength() )
      showiteminfos ( itemsavail[ pos ] );
   else
      showiteminfos ( NULL );
}


template<class T> void SelectAnything<T> :: showiteminfos ( T item )
{
   _showiteminfos ( item, position.x1, position.y1 - getiteminfoheight(), position.x2, position.y1 - 5 );
}







template<class T> void SelectAnything<T> :: _displaysingleitem ( T item, int x, int y )
{
   setinvisiblemouserectanglestk ( x, y, x + getitemsizex(), y + getitemsizey() );
   displaysingleitem ( item, x, y );
   getinvisiblemouserectanglestk ();
}


template<class T> void SelectAnything<T> :: displaysingleitem ( int itemx, int itemy, int picx, int picy )
{
   setinvisiblemouserectanglestk ( picx, picy, picx + getitemsizex(), picy + getitemsizey() );

   int pos = itemx + itemy * maxx;
   if ( pos <= itemsavail.getlength() )
      displaysingleitem ( itemsavail[ pos ], picx, picy );
   else
      displaysingleitem ( NULL, picx, picy );

   if ( itemx == actitemx  &&  itemy == actitemy ) 
      rectangle ( picx, picy, picx + getitemsizex(), picy + getitemsizey(), white );
   
   getinvisiblemouserectanglestk ();
}

template<class T> int SelectAnything<T> :: getxposforitempos ( int itemx )
{
   int p = position.x1 + itemx * getitemsizex();
   if ( itemx > 0 )
      p +=( itemx ) * getxgap();
   return p;
}

template<class T> int SelectAnything<T> :: getyposforitempos ( int itemy )
{
   int p = position.y1 + (itemy - winstarty) * getitemsizey();
   if ( itemy-winstarty > 0 )
      p +=  ( itemy-winstarty ) * getygap();
   return p;
}


template<class T> void SelectAnything<T> :: displaysingleitem ( int itemx, int itemy )
{
   displaysingleitem ( itemx, itemy, getxposforitempos ( itemx ), getyposforitempos ( itemy ) );
}

template<class T> void SelectAnything<T> ::  display ( void ) 
{
   for ( int y = winstarty; y < winstarty + winsizey; y++ ) 
      for ( int x = 0; x < maxx; x++ ) 
         displaysingleitem ( x, y );
      
}


template<class T> void SelectAnything<T> :: setnewselection ( T item )
{
   int num = itemsavail.getlength() + 1; 
   if ( num <= 0 )
      return;

   if ( !item )
      return;

   int i = 0;
   int found = 0;
   while ( i < num && !found ) {
      if ( itemsavail[i] == item )
         found = 1;
      else
         i++;
   }
   if ( found ) {
      actitemx = i % maxx;
      actitemy = i / maxx;
   }

}



template<class T> T SelectAnything<T> :: selectitem( T previtem, tkey neutralkey )
{
   int num = itemsavail.getlength() + 1; 
   if ( num <= 0 )
      return NULL;

   pushallmouseprocs();
   if ( mouseparams.pictpointer != icons.mousepointer ) 
       setnewmousepointer ( icons.mousepointer, 0,0 );

   addmouseproc ( &selfntmousescrollproc );

   setinvisiblemouserectanglestk ( position.x1, position.y1-getiteminfoheight(), position.x2, position.y2 );
   bar ( position.x1, position.y1-getiteminfoheight(), position.x2, position.y2, black );
   getinvisiblemouserectanglestk ();


   if ( previtem ) 
      for ( int i = 0; i <= itemsavail.getlength(); i++ )
         if ( itemsavail[ i ] == previtem ) {
            actitemx = i % maxx;
            actitemy = i / maxx;
         }
   
   display();
   showactiteminfos();
   int finished = 0;
   int mousepressed = 0;
   int lastscroll = ticker;
   int mousescrollspeed = 8;
   do {
      int oldx = actitemx;
      int oldy = actitemy;
      int oldwiny = winstarty;
      if ( keypress() ) {
         tkey ch = r_key();
         switch ( ch ) {
         
               #ifdef NEWKEYB
               case ct_up:
               #endif
               case ct_8k:   actitemy--;
                             break;
                             
               #ifdef NEWKEYB
               case ct_left:
               #endif
               case ct_4k:   actitemx--;
                             break;
                             
               #ifdef NEWKEYB
               case ct_right:
               #endif
               case ct_6k:   actitemx++;
                             break;
                             
               #ifdef NEWKEYB
               case ct_down:
               #endif
               case ct_2k:   actitemy++;
                  break;
                  
               case ct_pos1: actitemy=0;
                  break;
               case ct_ende: actitemy = maxy-1;
                  break;
                  
               case ct_f2:
               case ct_space:
               case ct_enter: finished = 1;
                  break;
               case ct_esc: finished = 2;
                  break;
                  
               case ct_f3:
               case ct_f4:
               case ct_f5:
               case ct_f6:
               case ct_f7:
               case ct_f8:
               case ct_f9: if ( ch != neutralkey && neutralkey ) {
                              finished = 2;
                              ::ch = ch;
                           }
                  break;
         } 
      }

      if ( mouseparams.taste == 1 ) {
         for ( int x = 0; x < maxx; x++ )
            for ( int y = winstarty; y < winstarty + winsizey; y++ )
               if ( mouseinrect ( getxposforitempos ( x ), getyposforitempos ( y ),
                                  getxposforitempos ( x ) + getitemsizex(), getyposforitempos ( y ) + getitemsizey()) )
                  {
                     actitemx = x;
                     actitemy = y;
                     mousepressed = 1;
                  }
         if ( mouseparams.x >= position.x1 && mouseparams.x <= position.x2 ) {
            if ( mouseparams.y > position.y2 ) 
               if ( winstarty + winsizey < maxy -1 )
                  if ( ticker > lastscroll + mousescrollspeed ) {
                     winstarty ++;
                     lastscroll = ticker;
                     actitemy ++;
                  }
            if ( mouseparams.y < position.y1 && mouseparams.y > position.y1 - getiteminfoheight() ) 
               if ( winstarty > 0 )
                  if ( ticker > lastscroll + mousescrollspeed ) {
                     winstarty --;
                     lastscroll = ticker;
                     actitemy --;
                  }
            
         }
      } else
         if ( mousepressed == 1 )
            finished = 1;

      if ( selfntmousecurs != 8 ) {
         if ( selfntmousecurs == 0 )
            if ( winstarty > 0 )
               if ( ticker > lastscroll + mousescrollspeed ) {
                  winstarty --;
                  lastscroll = ticker;
                  actitemy --;
               }

         if ( selfntmousecurs == 4 )
            if ( winstarty + winsizey < maxy -1 )
               if ( ticker > lastscroll + mousescrollspeed ) {
                  winstarty ++;
                  lastscroll = ticker;
                  actitemy ++;
               }
      }

      while ( actitemx >= maxx ) {
         actitemx -= maxx;
         actitemy ++;
      }
      while ( actitemx < 0 ) {
         actitemx += maxx;
         actitemy --;
      }
      if ( actitemy < 0 )
         actitemy = 0;
      if ( actitemy >= maxy )
         actitemy = maxy -1;

      if ( winstarty > actitemy )
         winstarty = actitemy;

      if ( winstarty + winsizey <= actitemy )
         winstarty = actitemy - winsizey +1;

      if ( oldwiny != winstarty ) 
         display();
      else 
         if ( oldx != actitemx  || oldy != actitemy ) {
            displaysingleitem ( oldx, oldy );
            displaysingleitem ( actitemx, actitemy );
         }
      
      if ( oldx != actitemx  || oldy != actitemy ) 
         showactiteminfos();

   } while ( !finished );

   removemouseproc ( &selfntmousescrollproc );
   popallmouseprocs();


   if ( finished == 1 ) {
      int pos = actitemx + actitemy * maxx;
      if ( pos <= itemsavail.getlength() )
         return itemsavail[ pos ];
      else
         return previtem;
   } else
      return previtem;
}


class SelectVehicleType : public SelectAnything< pvehicletype > {
                    protected:
                       virtual int isavailable ( pvehicletype item );
                       virtual void displaysingleitem ( pvehicletype item, int x, int y );
                       virtual int getitemsizex ( void ) { return fieldsizex; } ;
                       virtual int getitemsizey ( void ) { return fieldsizey; } ;
                       virtual void showiteminfos ( pvehicletype item, int x1, int y1, int x2, int y2 );
};


int SelectVehicleType :: isavailable ( pvehicletype item )
{
   /*
   if ( farbwahl == 8 ) {
      displaymessage("no neutral units allowed on map !\nswitching to red player!", 1 );
      farbwahl = 0;
   }
   */

   return isUnitNotFiltered ( item->id );  
}


void SelectVehicleType :: displaysingleitem ( pvehicletype item, int x, int y )
{
   bar ( x, y, x + getitemsizex(), y + getitemsizey(), black );
   if ( item )
      putrotspriteimage ( x, y, item->picture[0], farbwahl*8 );
}

void SelectVehicleType :: showiteminfos ( pvehicletype item, int x1, int y1, int x2, int y2 )
{ 
   rectangle ( x1, y1, x2, y2, lightgray );
   bar ( x1+1, y1+1, x2-1, y2-1, black );
   if ( item ) {
      displaysingleitem ( item, x1 + 10, (y1 + y2 - getitemsizey() )/2 );
      npush ( activefontsettings );
      activefontsettings.font = schriften.smallarial;
      activefontsettings.background  = 255;
      activefontsettings.justify = lefttext;
      activefontsettings.color = lightgray;
      activefontsettings.height = 0;
      activefontsettings.length = x2 - x1 - 20 - getitemsizex();

      if ( item->name && item->name[0] )
         showtext4 ( "Name: %s", x1 + 20 + getitemsizex(), y1 + 10, item->name );
      else
         if ( item->description  &&  item->description[0] )
            showtext4 ( "Name: %s", x1 + 20 + getitemsizex(), y1 + 10, item->description );
         else
            showtext4 ( "Name: NONE", x1 + 20 + getitemsizex(), y1 + 10 );

      showtext4 ( "ID: %d", x1 + 20 + getitemsizex(), y1 + 30, item->id );
      npop ( activefontsettings );
   }
}





class SelectTerrainType : public SelectAnything< pterraintype > {
                    protected:
                       virtual int isavailable ( pterraintype item ) { return 1; };
                       virtual void displaysingleitem ( pterraintype item, int x, int y );
                       virtual int getitemsizex ( void ) { return fieldsizex; } ;
                       virtual int getitemsizey ( void ) { return fieldsizey; } ;
                       virtual void showiteminfos ( pterraintype item, int x1, int y1, int x2, int y2 );
};

void SelectTerrainType :: displaysingleitem ( pterraintype item, int x, int y )
{
   bar ( x, y, x + getitemsizex(), y + getitemsizey(), black );
   if ( auswahlw >= cwettertypennum || auswahlw < 0 ) 
      auswahlw = 0;

   if ( item )
      if ( item->weather[auswahlw] )   
         item->weather[auswahlw]->paint ( x, y );
      else
         item->weather[0]->paint ( x, y );
}

void SelectTerrainType :: showiteminfos ( pterraintype item, int x1, int y1, int x2, int y2 )
{ 
   rectangle ( x1, y1, x2, y2, lightgray );
   bar ( x1+1, y1+1, x2-1, y2-1, black );
   if ( item ) {
      displaysingleitem ( item, x1 + 10, (y1 + y2 - getitemsizey() )/2 );
      npush ( activefontsettings );
      activefontsettings.font = schriften.smallarial;
      activefontsettings.background  = 255;
      activefontsettings.justify = lefttext;
      activefontsettings.color = lightgray;
      activefontsettings.height = 0;
      activefontsettings.length = x2 - x1 - 20 - getitemsizex();

      if ( item->name && item->name[0] )
         showtext4 ( "Name: %s", x1 + 20 + getitemsizex(), y1 + 10, item->name );
      else
         showtext4 ( "Name: NONE", x1 + 20 + getitemsizex(), y1 + 10 );

      showtext4 ( "ID: %d", x1 + 20 + getitemsizex(), y1 + 30, item->id );
      npop ( activefontsettings );
   }
}





class SelectObjectType : public SelectAnything< pobjecttype > {
                    protected:
                       virtual int isavailable ( pobjecttype item ) { return 1; };
                       virtual void displaysingleitem ( pobjecttype item, int x, int y );
                       virtual int getitemsizex ( void ) { return fieldsizex; } ;
                       virtual int getitemsizey ( void ) { return fieldsizey; } ;
                       virtual void showiteminfos ( pobjecttype item, int x1, int y1, int x2, int y2 );
};

void SelectObjectType :: displaysingleitem ( pobjecttype item, int x, int y )
{
   bar ( x, y, x + getitemsizex(), y + getitemsizey(), black );
   if ( item )
      item->display ( x, y );
}

void SelectObjectType :: showiteminfos ( pobjecttype item, int x1, int y1, int x2, int y2 )
{ 
   rectangle ( x1, y1, x2, y2, lightgray );
   bar ( x1+1, y1+1, x2-1, y2-1, black );
   if ( item ) {
      displaysingleitem ( item, x1 + 10, (y1 + y2 - getitemsizey() )/2 );
      npush ( activefontsettings );
      activefontsettings.font = schriften.smallarial;
      activefontsettings.background  = 255;
      activefontsettings.justify = lefttext;
      activefontsettings.color = lightgray;
      activefontsettings.height = 0;
      activefontsettings.length = x2 - x1 - 20 - getitemsizex();

      if ( item->name && item->name[0] )
         showtext4 ( "Name: %s", x1 + 20 + getitemsizex(), y1 + 10, item->name );
      else
         showtext4 ( "Name: NONE", x1 + 20 + getitemsizex(), y1 + 10 );

      showtext4 ( "ID: %d", x1 + 20 + getitemsizex(), y1 + 30, item->id );
      npop ( activefontsettings );
   }
}





class SelectBuildingType : public SelectAnything< pbuildingtype > {
                       int buildingfieldsdisplayedx, buildingfieldsdisplayedy;
                    protected:
                       virtual int isavailable ( pbuildingtype item ); // { return 1; };
                       virtual void displaysingleitem ( pbuildingtype item, int x, int y );
                       virtual int getitemsizex ( void ) { return fieldsizex+(buildingfieldsdisplayedx-1)*fielddistx+fielddisthalfx; } ;
                       virtual int getitemsizey ( void ) { return fieldsizey+(buildingfieldsdisplayedy-1)*fielddisty; } ;
                       virtual void showiteminfos ( pbuildingtype item, int x1, int y1, int x2, int y2 );
                    public:
                       SelectBuildingType( void ) { buildingfieldsdisplayedx = 4; buildingfieldsdisplayedy = 6; };
};

int SelectBuildingType :: isavailable ( pbuildingtype item )
{
   return isBuildingNotFiltered ( item->id );
}


void SelectBuildingType :: displaysingleitem ( pbuildingtype item, int x, int y )
{
   bar ( x, y, x + getitemsizex(), y + getitemsizey(), black );
   if ( item )
      for ( int xp = 0; xp < buildingfieldsdisplayedx; xp++ )
         for ( int yp = 0; yp < buildingfieldsdisplayedy; yp++ )
            if ( item->getpicture ( xp, yp ) )
               putrotspriteimage ( x + xp * fielddistx + ( yp & 1 ) * fielddisthalfx, y + yp * fielddisty, item->getpicture( xp, yp ), farbwahl*8 );
}

void SelectBuildingType :: showiteminfos ( pbuildingtype item, int x1, int y1, int x2, int y2 )
{ 
   rectangle ( x1, y1, x2, y2, lightgray );
   bar ( x1+1, y1+1, x2-1, y2-1, black );
   if ( item ) {
      putrotspriteimage ( x1 + 10, y1 + (y2 - y1 - fieldsizey )/2, item->getpicture ( item->entry.x, item->entry.y ), farbwahl*8 );
      npush ( activefontsettings );
      activefontsettings.font = schriften.smallarial;
      activefontsettings.background  = 255;
      activefontsettings.justify = lefttext;
      activefontsettings.color = lightgray;
      activefontsettings.height = 0;
      activefontsettings.length = x2 - x1 - 20 - fieldsizex;

      if ( item->name && item->name[0] )
         showtext4 ( "Name: %s", x1 + 20 + fieldsizex, y1 + 10, item->name );
      else
         showtext4 ( "Name: NONE", x1 + 20 + fieldsizex, y1 + 10 );

      showtext4 ( "ID: %d", x1 + 20 + fieldsizex, y1 + 30, item->id );
      npop ( activefontsettings );
   }
}


class tweathertype {
         public:
            int num;
            char* name;
       };
typedef tweathertype* pweathertype;
dynamic_array< pweathertype > weathervector;


class tcolortype {
         public:
            int col;
            int getcolor ( void ) { return 20 + 8 * col; };
       };
typedef tcolortype* pcolortype;       
dynamic_array< pcolortype > colorvector;


class tminetype {
         public:
            int type;
            char* name;
            void paint ( int x, int y ) { putrotspriteimage ( x, y, getmineadress(type+1), farbwahl*8 ); };
       };
typedef tminetype* pminetype;       
dynamic_array< pminetype > minevector;



class SelectColor : public SelectAnything< pcolortype > {
                    protected:
                       virtual int isavailable ( pcolortype item ) { return 1; };
                       virtual void displaysingleitem ( pcolortype item, int x, int y );
                       virtual int getitemsizex ( void ) { return fieldsizex; } ;
                       virtual int getitemsizey ( void ) { return fieldsizey; } ;
                       virtual void showiteminfos ( pcolortype item, int x1, int y1, int x2, int y2 );
                       virtual int getiteminfoheight( void ) { return 20; };
};

void SelectColor :: displaysingleitem ( pcolortype item, int x, int y )
{
   if ( item ) {
      bar ( x, y, x + getitemsizex(), y + getitemsizey(), item->getcolor() );
      npush ( activefontsettings );
      activefontsettings.font = schriften.arial8;
      activefontsettings.background  = 255;
      activefontsettings.justify = centertext;
      activefontsettings.color = black;
      activefontsettings.height = 0;
      activefontsettings.length = getitemsizex();
      showtext2 ( strrr ( item->col ), x, y + (getitemsizey() - activefontsettings.font->height) / 2 );
      npop ( activefontsettings );
   } else
      bar ( x, y, x + getitemsizex(), y + getitemsizey(), black );
}

void SelectColor :: showiteminfos ( pcolortype item, int x1, int y1, int x2, int y2 )
{ 
    rectangle ( x1, y1, x2, y2, lightgray );
    bar ( x1+1, y1+1, x2-1 , y2-1, item->getcolor() );
}





class SelectWeather : public SelectAnything< pweathertype > {
                    protected:
                       virtual int isavailable ( pweathertype item ) { return 1; };
                       virtual void displaysingleitem ( pweathertype item, int x, int y );
                       virtual int getitemsizex ( void ) { return 120; } ;
                       virtual int getitemsizey ( void ) { return 25; } ;
                       virtual void showiteminfos ( pweathertype item, int x1, int y1, int x2, int y2 );
};

void SelectWeather :: displaysingleitem ( pweathertype item, int x, int y )
{
   bar ( x, y, x + getitemsizex(), y + getitemsizey(), black );
   if ( item ) {
      npush ( activefontsettings );
      activefontsettings.font = schriften.smallarial;
      activefontsettings.background  = 255;
      activefontsettings.justify = lefttext;
      activefontsettings.color = white;
      activefontsettings.height = 0;
      activefontsettings.length = getitemsizex() - 10;
      showtext2 ( item->name, x+5, y + (getitemsizey() - activefontsettings.font->height) / 2 );
      npop ( activefontsettings );
   }
}

void SelectWeather :: showiteminfos ( pweathertype item, int x1, int y1, int x2, int y2 )
{ 
   rectangle ( x1, y1, x2, y2, lightgray );
   bar ( x1+1, y1+1, x2-1, y2-1, black );
   if ( item && auswahl ) 
      if ( auswahl->weather[ item->num ] ) {
         auswahl->weather[item->num]->paint ( x1 + 10, (y1 + y2 - fieldsizey )/2 );
      } else {

         auswahl->weather[0]->paint ( x1 + 10, (y1 + y2 - fieldsizey )/2 );

         npush ( activefontsettings );
         activefontsettings.font = schriften.smallarial;
         activefontsettings.background  = 255;
         activefontsettings.justify = lefttext;
         activefontsettings.color = lightgray;
         activefontsettings.height = 0;
         activefontsettings.length = x2 - x1 - 20 - fieldsizex;
   
         showtext4 ( "the terrain '%s'", x1 + 20 + fieldsizex, y1 + 10, auswahl->name );
         showtext2 ( "does not have this weather", x1 + 20 + fieldsizex, y1 + 30 );
   
         npop ( activefontsettings );
      }
   
}



class SelectMine : public SelectAnything< pminetype > {
                    protected:
                       virtual int isavailable ( pminetype item ) { return 1; };
                       virtual void displaysingleitem ( pminetype item, int x, int y );
                       virtual int getitemsizex ( void ) { return fieldsizex; } ;
                       virtual int getitemsizey ( void ) { return fieldsizey; } ;
                       virtual void showiteminfos ( pminetype item, int x1, int y1, int x2, int y2 );
};

void SelectMine :: displaysingleitem ( pminetype item, int x, int y )
{
   bar ( x, y, x + getitemsizex(), y + getitemsizey(), black );
   if ( item ) 
      item->paint ( x, y );
}


void SelectMine :: showiteminfos ( pminetype item, int x1, int y1, int x2, int y2 )
{ 
   rectangle ( x1, y1, x2, y2, lightgray );
   bar ( x1+1, y1+1, x2-1, y2-1, black );
   if ( item ) {
      displaysingleitem ( item, x1 + 10, (y1 + y2 - getitemsizey() )/2 );
      npush ( activefontsettings );
      activefontsettings.font = schriften.smallarial;
      activefontsettings.background  = 255;
      activefontsettings.justify = lefttext;
      activefontsettings.color = lightgray;
      activefontsettings.height = 0;
      activefontsettings.length = x2 - x1 - 20 - getitemsizex();

      if ( item->name && item->name[0] )
         showtext4 ( "Name: %s", x1 + 20 + getitemsizex(), y1 + 10, item->name );
      else
         showtext4 ( "Name: NONE", x1 + 20 + getitemsizex(), y1 + 10 );

      npop ( activefontsettings );
   }
}





class SelectItemContainer {
             SelectTerrainType*   selectterraintype;
             SelectVehicleType*   selectvehicletype;
             SelectColor*         selectcolor;
             SelectObjectType*    selectobjecttype;
             SelectMine*          selectmine;
             SelectWeather*       selectweather;
             SelectBuildingType*  selectbuildingtype;

             struct {
                SelectAnythingBase*  selector;
                char*                name;
                char*                keyname;
                tmouserect           pos;
             } selector[10];
             int                  paintallselections_initialized;
             int                  selectornum;

             void addselector ( SelectAnythingBase* _selector, char* name, char* keyname ) {
                      selector[selectornum].selector = _selector;
                      selector[selectornum].name     = name;
                      selector[selectornum].keyname  = keyname;
                      selector[selectornum].pos.x1 = -1;
                      selector[selectornum].pos.x2 = -1;
                      selector[selectornum].pos.y1 = -1;
                      selector[selectornum].pos.y2 = -1;
                      selectornum++;
             };

             int                  selectionypos;
             void paintselections ( int num, int act );

         public:
             SelectItemContainer ( void ) {
                  selectterraintype = NULL;
                  selectvehicletype = NULL;
                  selectcolor = NULL;
                  selectobjecttype = NULL;
                  selectmine = NULL;
                  selectweather = NULL;
                  selectbuildingtype = NULL;
                  paintallselections_initialized = 0;
             };

             SelectTerrainType*   getterrainselector ( void ) {  
                  if ( !selectterraintype ) {
                     selectterraintype = new SelectTerrainType;
                     selectterraintype->init( getterraintypevector() );
                  }
                  return selectterraintype;
             };

             SelectBuildingType*   getbuildingselector ( void ) {  
                  if ( !selectbuildingtype ) {
                     selectbuildingtype = new SelectBuildingType;
                     selectbuildingtype->init( getbuildingtypevector() );
                  }
                  return selectbuildingtype;
             };

             SelectVehicleType*   getvehicleselector ( void ) {  
                  if ( !selectvehicletype ) {
                     selectvehicletype = new SelectVehicleType;
                     selectvehicletype->init( getvehicletypevector() );
                  }
                  return selectvehicletype;
             };

             SelectObjectType*   getobjectselector ( void ) {  
                  if ( !selectobjecttype ) {
                     selectobjecttype = new SelectObjectType;
                     selectobjecttype->init( getobjecttypevector() );
                  }
                  return selectobjecttype;
             };

             SelectWeather*   getweatherselector ( void ) {  
                  if ( !selectweather ) {
                     for ( int i = 0; i < cwettertypennum; i++ ) {
                        pweathertype wt = new tweathertype;
                        wt->num = i;
                        wt->name = strdup ( cwettertypen[i] );
                        weathervector[i] = wt;
                     }
                     selectweather = new SelectWeather;
                     selectweather->init( weathervector );
                  }
                  return selectweather;
             };

             SelectMine*   getmineselector ( void ) {  
                  if ( !selectmine ) {
                     for ( int i = 0; i < cminenum; i++ ) {
                        pminetype mt = new tminetype;
                        mt->type = i;
                        minevector[i] = mt;
                        mt->name = strdup ( cminentypen[ i ] );
                     }
                     selectmine = new SelectMine;
                     selectmine->init( minevector );
                  }
                  return selectmine;
             };

             SelectColor*   getcolorselector ( void ) {  
                  if ( !selectcolor ) {
                     for ( int i = 0; i < 9; i++ ) {
                        pcolortype ct = new tcolortype;
                        ct->col = i;
                        colorvector[i] = ct;
                     }
                     selectcolor = new SelectColor;
                     selectcolor->init( colorvector );
                  }
                  return selectcolor;
             };


             void paintallselections ( void );
             void checkformouse ( void );
      } selectitemcontainer;


void SelectItemContainer :: paintselections ( int num, int act )
{

   int backgroundcol;
   if ( num == act ) 
      backgroundcol = white;
   else 
      backgroundcol = lightgray;


   int freespace = 20;

   npush ( activefontsettings );
   activefontsettings.font = schriften.smallarial;
   activefontsettings.background  = backgroundcol;
   activefontsettings.justify = centertext;
   activefontsettings.color = black;
   activefontsettings.height = 0;
   activefontsettings.length = selfontxsize;

   int keywidth = 20;

   int x1 = selfontxanf;
   int y1 = selectionypos;
   int x2 = selfontxanf + selfontxsize - keywidth;
   int y2 = selectionypos + activefontsettings.font->height + selector[num].selector->getiteminfoheight();

   selector[num].pos.x1 = x1;
   selector[num].pos.x2 = x2 + keywidth;
   selector[num].pos.y1 = y1;
   selector[num].pos.y2 = y2;

   if ( y2 < agmp->resolutiony ) {
      setinvisiblemouserectanglestk ( x1, y1, selector[num].pos.x2, y2 + freespace );
                                                   
      showtext2 ( selector[num].name, x1, y1 );
   
      y1 += activefontsettings.font->height;
   
   
      selector[num].selector->showactiteminfos ( x1, y1, x2, y2 );
      bar ( x2, y1, x2 + keywidth, y2, backgroundcol ); 
                                 
      selectionypos += selector[num].selector->getiteminfoheight() + freespace;
      bar ( x1, y2+1, x2 + keywidth, y2 + freespace, black );
      activefontsettings.background  = 255;
      activefontsettings.color = black;
      activefontsettings.length = keywidth;
      showtext2 ( selector[num].keyname, x2, y1 + ( y2 - y1 - activefontsettings.font->height ) / 2);
   
      getinvisiblemouserectanglestk ();
   }
   npop ( activefontsettings );
}

void SelectItemContainer :: paintallselections ( void ) 
{
     if ( !paintallselections_initialized ) {
        selectornum = 0;
        addselector ( getterrainselector(),   "Terrain", "F3" );
        addselector ( getvehicleselector(),   "Vehicle", "F4" );
        addselector ( getcolorselector(),     "Color",   "F5" );
        addselector ( getbuildingselector(),  "Building","F6" );
        addselector ( getobjectselector(),    "Object",  "F7" );
        addselector ( getmineselector(),      "Mine",    "F8" );
        addselector ( getweatherselector(),   "Weather", "F9" );
     }
     selectionypos = selfontyanf;
     for ( int i = 0; i < selectornum; i++ )
        paintselections ( i, lastselectiontype - 1 );

     int x1 = selfontxanf;
     int x2 = selfontxanf + selfontxsize;

     if ( selectionypos < agmp->resolutiony-1 )
        bar ( x1, selectionypos , x2, agmp->resolutiony-1 , black );
}

void SelectItemContainer :: checkformouse ( void ) 
{
   if ( mouseparams.taste == 1 ) {
      int found = -1;
      for ( int i = 0; i < selectornum; i++ )
         if ( mouseinrect ( &selector[i].pos )) {
            found = i;
            while ( mouseparams.taste & 1 )
               releasetimeslice();
            break;   
         }

      switch ( found ) {
         case 0: selterraintype ( ct_invvalue );
            break;
         case 1: selvehicletype ( ct_invvalue );
            break;
         case 2: selcolor ( ct_invvalue );
            break;
         case 3: selbuilding ( ct_invvalue );
            break;
         case 4: selobject ( ct_invvalue );
            break;
         case 5: selmine ( ct_invvalue );
            break;
         case 6: selweather ( ct_invvalue );
            break;
      } /* endswitch */
   }
}


void selterraintype( tkey ench )
{  
   auswahl = selectitemcontainer.getterrainselector()->selectitem( auswahl, ench );
   lastselectiontype = cselbodentyp;
   showallchoices();
}

void selvehicletype(tkey ench )
{  
   auswahlf = selectitemcontainer.getvehicleselector()->selectitem( auswahlf, ench );
   lastselectiontype = cselunit;
   showallchoices();
}

void selcolor( tkey ench )
{  
   farbwahl = selectitemcontainer.getcolorselector()->selectitem( colorvector[farbwahl], ench )->col;
   if ( lastselectiontype != cselunit && lastselectiontype != cselbuilding && lastselectiontype != cselmine )
      lastselectiontype = cselunit;
   showallchoices();
}

void selobject( tkey ench )
{  
   actobject = selectitemcontainer.getobjectselector()->selectitem( actobject, ench );
   lastselectiontype = cselobject;
   showallchoices();
}

void selmine( tkey ench )
{  
   auswahlm = selectitemcontainer.getmineselector()->selectitem( minevector[auswahlm], ench )->type;
   lastselectiontype = cselmine;
   showallchoices();
}

void selweather( tkey ench )
{  
   auswahlw = selectitemcontainer.getweatherselector()->selectitem( weathervector[auswahlw], ench )->num;
   lastselectiontype = cselbodentyp;
   showallchoices();
}

void selbuilding ( tkey ench )
{
   auswahlb = selectitemcontainer.getbuildingselector()->selectitem( auswahlb, ench );
   lastselectiontype = cselbuilding;
   showallchoices();
}

  /* Nr
 1:terrain
 2:vehicletypeen
 3:Farben
 4:Buildings
 5:(Keine Auswahl) SPezielle Darstellung fr vehicle beim beladen (mit Gewicht)
 6:object
 7:Fahrzeuge fr BuildingCargo
 8:Fahrzeuge fr vehicleCargo
 9:Minen
 10: Weathersel
 */ 


void    showallchoices(void)
{
   static int shown = 0;
   if (shown )
      return;
   shown++;
   selectitemcontainer.paintallselections( );
   shown--;
}


void checkselfontbuttons(void)
{
   selectitemcontainer.checkformouse( );
}




void setnewvehicleselection ( pvehicletype v )
{
   selectitemcontainer.getvehicleselector()->setnewselection ( v );
   selectitemcontainer.getcolorselector()->setnewselection( colorvector[farbwahl] );
//   showallchoices();
}

void resetvehicleselector ( void )
{
   selectitemcontainer.getvehicleselector()->init( getvehicletypevector() );
}

void resetbuildingselector ( void )
{
   selectitemcontainer.getbuildingselector()->init( getbuildingtypevector() );
}


void setnewterrainselection ( pterraintype t )
{
   selectitemcontainer.getterrainselector()->setnewselection( t );
}

void setnewobjectselection  ( pobjecttype o )
{
   selectitemcontainer.getobjectselector()->setnewselection( o );
}


/////////////////////////////////////// CARGO //////////////////////////////////////


class SelectCargoVehicleType : public SelectVehicleType {
      public:
          void showiteminfos ( pvehicletype item, int x1, int y1, int x2, int y2 );
     };

void SelectCargoVehicleType :: showiteminfos ( pvehicletype item, int x1, int y1, int x2, int y2 )
{ 
   rectangle ( x1, y1, x2, y2, lightgray );
   bar ( x1+1, y1+1, x2-1, y2-1, black );
   if ( item ) {
      displaysingleitem ( item, x1 + 10, (y1 + y2 - getitemsizey() )/2 );
      npush ( activefontsettings );
      activefontsettings.font = schriften.smallarial;
      activefontsettings.background  = 255;
      activefontsettings.justify = lefttext;
      activefontsettings.color = lightgray;
      activefontsettings.height = 0;
      activefontsettings.length = x2 - x1 - 20 - getitemsizex();

      if ( item->name && item->name[0] )
         showtext4 ( "Name: %s", x1 + 20 + getitemsizex(), y1 + 10, item->name );
      else
         if ( item->description  &&  item->description[0] )
            showtext4 ( "Name: %s", x1 + 20 + getitemsizex(), y1 + 10, item->description );
         else
            showtext4 ( "Name: NONE", x1 + 20 + getitemsizex(), y1 + 10 );

      showtext4 ( "weight: %d", x1 + 20 + getitemsizex(), y1 + 30, item->maxsize() );
      npop ( activefontsettings );
   }
}



class SelectVehicleTypeForTransportCargo : public SelectCargoVehicleType {
         pvehicle transport;
      public:
         SelectVehicleTypeForTransportCargo ( pvehicle _transport ) { transport = _transport; };
         int isavailable ( pvehicletype item );
     };

int SelectVehicleTypeForTransportCargo :: isavailable ( pvehicletype item ) 
{
    if ( transport->freeweight() < item->maxsize() )
       return 0;
    else
       return transport->typ->vehicleloadable ( item ) && SelectVehicleType::isavailable ( item ); 
}

class SelectVehicleTypeForBuildingCargo : public SelectCargoVehicleType {
         pbuilding building;
      public:
         SelectVehicleTypeForBuildingCargo ( pbuilding _building ) { building = _building; };
         int isavailable ( pvehicletype item ) {    return building->typ->vehicleloadable ( item ) && SelectVehicleType::isavailable ( item ); };
     };

class SelectVehicleTypeForBuildingProduction : public SelectCargoVehicleType {
         pbuilding building;
      public:
         SelectVehicleTypeForBuildingProduction ( pbuilding _building ) { building = _building; };
         int isavailable ( pvehicletype item ) {  
            for ( int i = 0; i < 32; i++ )
               if ( building->production[i] == item )
                  return 0;
            return building->typ->vehicleloadable ( item ) && SelectVehicleType::isavailable ( item ); 
         };
     };

//* õS Fahrzeuge fr Unit-Beloading


void selunitcargo( pvehicle transport )
{  
   SelectVehicleTypeForTransportCargo svtftc ( transport );
   svtftc.init( getvehicletypevector() );
   pvehicletype newcargo = svtftc.selectitem ( NULL );

   if ( newcargo ) {
      pvehicle unit = new Vehicle ( newcargo, actmap, transport->color / 8 );
      unit->fillMagically();
      // generatevehicle_ka ( newcargo, transport->color / 8, unit );
    
      int match = 0;
      for ( int i = 0; i < 8; i++ )
        if ( unit->typ->height & ( 1 << i )) {
           unit->height = 1 << i;
           unit->tank.material = unit->typ->tank.material;
           unit->tank.fuel = unit->typ->tank.fuel;
           if ( transport->vehicleloadable ( unit )) {
              int p = 0;
              while ( transport->loading[p] )
                 p++;
              transport->loading[p] = unit;
              match = 1;
              break;
           } else {
              unit->tank.material = 0;
              unit->tank.fuel = 0;
              if ( transport->vehicleloadable ( unit )) {
                 int p = 0;
                 while ( transport->loading[p] )
                    p++;
                 transport->loading[p] = unit;
                 match = 1;
                 displaymessage("Warning:\nThe unit you just set could not be loaded with full material and fuel\nPlease set these values manually",1);
                 break;
              } 
           }
       }
      if ( !match ) {
        removevehicle ( &unit );
        displaymessage("The unit could not be loaded !",1);
      }
   }
}

//* õS Fahrzeuge fr Geb„ude-loading


void selbuildingcargo( pbuilding bld )
{  
   SelectVehicleTypeForBuildingCargo svtfbc ( bld );
   svtfbc.init( getvehicletypevector() );
   pvehicletype newcargo = svtfbc.selectitem ( NULL );
   if ( newcargo ) {
      pvehicle unit = new Vehicle ( newcargo, actmap, bld->color / 8 );
      MapCoordinate mc = bld->getEntry();
      unit->xpos = mc.x;
      unit->ypos = mc.y;
      unit->fillMagically();

      int match = 0;
      int poss = 0;
      if ( unit ) {
         for ( int i = 0; i < 8; i++ )
            if ( unit->typ->height & ( 1 << i )) {
               unit->height = 1 << i;
               if ( bld->vehicleloadable ( unit )) {
                  poss |= 1 << i;
                  match = 1;
               }
            }

          for ( int h2 = 0; h2<8; h2++ )
            if ( unit->typ->height & ( 1 << h2 ))
               if ( poss & ( 1 << h2 ))
                  if ( bld->typ->loadcapability & ( 1 << h2))
                     unit->height = 1 << h2;

          for ( int h1 = 0; h1<8; h1++ )
            if ( unit->typ->height & ( 1 << h1 ))
               if ( poss & ( 1 << h1 ))
                  if ( bld->typ->buildingheight & ( 1 << h1))
                     unit->height = 1 << h1;
       }

       if ( match ) {
          int p = 0;
          while ( bld->loading[p] )
            p++;
          bld->loading[p] = unit;
          unit->setMovement ( unit->typ->movement[log2( unit->height)] );
       } else {
           displaymessage("The unit could not be loaded !",1);
           removevehicle ( &unit );
       }

   }

}


//* õS Fahrzeuge fr Geb„ude-Production

void selbuildingproduction( pbuilding bld )
{  
   SelectVehicleTypeForBuildingProduction svtfbc ( bld );
   svtfbc.init( getvehicletypevector() );
   pvehicletype newcargo = svtfbc.selectitem ( NULL );
   if ( newcargo ) {
      int p = 0;
      while ( bld->production[p] )
        p++;
      bld->production[p] = newcargo;
   }
}



