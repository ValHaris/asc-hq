/*! \file edselfnt.cpp
    \brief Selecting units, buildings, objects, weather etc. in the mapeditor
*/

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
#include "edselfnt.h"
#include "edglobal.h"
#include "mapdisplay.h"
#include "itemrepository.h"




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

#define vect vector

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

                       virtual bool isavailable ( T item ) = 0;
                       virtual void displaysingleitem ( T item, int x, int y ) = 0;
                       virtual void _displaysingleitem ( T item, int x, int y );
                       virtual void displayItem ( int itemx, int itemy, int picx, int picy );
                       virtual void displayItem ( int itemx, int itemy );
                       virtual int getxposforitempos ( int itemx );
                       virtual int getyposforitempos ( int itemy );
                       virtual int getitemsizex ( void ) = 0;
                       virtual int getitemsizey ( void ) = 0;
                       virtual ASCString getItemName ( T item ) = 0;
                       virtual int getxgap ( void ) { return 5; };
                       virtual int getygap ( void ) { return 5; };
                       virtual void _showiteminfos ( T item, int x1, int y1, int x2, int y2 );
                       virtual void showiteminfos ( T item, int x1, int y1, int x2, int y2 ) = 0;
                       void showiteminfos ( T item );
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
   itemsavail.clear();

   for ( int i = 0; i < v.size(); i++ )
      if ( isavailable ( v[i] ) )
         itemsavail.push_back (  v[i] );
     
  int num = itemsavail.size() + 1; 
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
   collategraphicoperations cgo ( x1, y1, x2, y2 );
   showiteminfos ( item, x1, y1, x2, y2 );
}


template<class T> void SelectAnything<T> :: showactiteminfos ( int x1, int y1, int x2, int y2 )
{
   int pos = actitemx + actitemy * maxx;
   if ( pos < itemsavail.size() )
      _showiteminfos ( itemsavail[ pos ], x1, y1, x2, y2 );
   else
      _showiteminfos ( NULL, x1, y1, x2, y2 );

}

template<class T> void SelectAnything<T> :: showactiteminfos ( void )
{
   int pos = actitemx + actitemy * maxx;
   if ( pos < itemsavail.size() )
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
   collategraphicoperations cgo ( x, y, x + getitemsizex(), y + getitemsizey() );
   displaysingleitem ( item, x, y );
}


template<class T> void SelectAnything<T> :: displayItem ( int itemx, int itemy, int picx, int picy )
{
   int pos = itemx + itemy * maxx;
   if ( pos < itemsavail.size() )
      _displaysingleitem ( itemsavail[ pos ], picx, picy );
   else
      _displaysingleitem ( NULL, picx, picy );

   if ( itemx == actitemx  &&  itemy == actitemy ) 
      rectangle ( picx, picy, picx + getitemsizex(), picy + getitemsizey(), white );
   
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


template<class T> void SelectAnything<T> :: displayItem ( int itemx, int itemy )
{
   displayItem ( itemx, itemy, getxposforitempos ( itemx ), getyposforitempos ( itemy ) );
}

template<class T> void SelectAnything<T> ::  display ( void ) 
{
   for ( int y = winstarty; y < winstarty + winsizey; y++ ) 
      for ( int x = 0; x < maxx; x++ )
         displayItem ( x, y );
      
}


template<class T> void SelectAnything<T> :: setnewselection ( T item )
{
   int num = itemsavail.size(); 
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
   int num = itemsavail.size() ; 
   if ( num <= 0 )
      return NULL;

   pushallmouseprocs();
   if ( mouseparams.pictpointer != icons.mousepointer ) 
       setnewmousepointer ( icons.mousepointer, 0,0 );

   addmouseproc ( &selfntmousescrollproc );

   bar ( position.x1, position.y1-getiteminfoheight(), position.x2, position.y2, black );

   if ( previtem ) 
      for ( int i = 0; i < itemsavail.size(); i++ )
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
         int prntkey;
         tkey ch;
         getkeysyms ( &ch, &prntkey );
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
         if ((prntkey > ' ') && (prntkey < 256)) {   /* spedsearc */
            int key = tolower ( prntkey );

            int ax = actitemx;
            int ay = actitemy;
            bool finished  = false;
            do {
               ax++;
               while ( ax >= maxx ) {
                  ax -= maxx;
                  ay ++;
               }
               if ( ay >= maxy )
                  ay = 0;

               if ( ax == actitemx && ay == actitemy ) {
                  finished = true;
               } else {
                  int pos = ax + ay * maxx;
                  if ( pos < itemsavail.size() )
                     if ( itemsavail[pos] ) {
                        ASCString s = getItemName( itemsavail[pos] );
                        if ( !s.empty() )
                           if ( tolower (s[0]) == key ) {
                              actitemx = ax;
                              actitemy = ay;
                              finished = true;
                           }
                     }
               }
            } while ( !finished );
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
            displayItem ( oldx, oldy );
            displayItem ( actitemx, actitemy );
         }
      
      if ( oldx != actitemx  || oldy != actitemy ) 
         showactiteminfos();

      releasetimeslice();
   } while ( !finished );

   removemouseproc ( &selfntmousescrollproc );
   popallmouseprocs();


   if ( finished == 1 ) {
      int pos = actitemx + actitemy * maxx;
      if ( pos < itemsavail.size() )
         return itemsavail[ pos ];
      else
         return previtem;
   } else
      return previtem;
}


class SelectVehicleType : public SelectAnything< pvehicletype > {
                    protected:
                       virtual bool isavailable ( pvehicletype item );
                       virtual void displaysingleitem ( pvehicletype item, int x, int y );
                       virtual int getitemsizex ( void ) { return fieldsizex; } ;
                       virtual int getitemsizey ( void ) { return fieldsizey; } ;
                       virtual void showiteminfos ( pvehicletype item, int x1, int y1, int x2, int y2 );
                       virtual ASCString getItemName ( pvehicletype item );
};


bool SelectVehicleType :: isavailable ( pvehicletype item )
{
   /*
   if ( farbwahl == 8 ) {
      displaymessage("no neutral units allowed on map !\nswitching to red player!", 1 );
      farbwahl = 0;
   }
   */

   return !ItemFiltrationSystem::isFiltered( ItemFiltrationSystem::Vehicle, item->id );
}


void SelectVehicleType :: displaysingleitem ( pvehicletype item, int x, int y )
{
   bar ( x, y, x + getitemsizex(), y + getitemsizey(), black );
   if ( item )
      // putrotspriteimage ( x, y, item->picture[0], farbwahl*8 );
      item->paint( getActiveSurface(), SPoint(x,y), farbwahl, 0 );
}

ASCString SelectVehicleType :: getItemName ( pvehicletype item )
{
   if ( !item->name.empty()  )
      return item->name;
   else
      if ( !item->description.empty() )
         return item->description;
      else
         return "-NONE-";
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

      showtext4 ( "Name: %s", x1 + 20 + getitemsizex(), y1 + 10, getItemName (item).c_str() );

      showtext4 ( "ID: %d", x1 + 20 + getitemsizex(), y1 + 30, item->id );
      npop ( activefontsettings );
   }
}





class SelectTerrainType : public SelectAnything< pterraintype > {
                    protected:
                       virtual bool isavailable ( pterraintype item );
                       virtual void displaysingleitem ( pterraintype item, int x, int y );
                       virtual int getitemsizex ( void ) { return fieldsizex; } ;
                       virtual int getitemsizey ( void ) { return fieldsizey; } ;
                       virtual void showiteminfos ( pterraintype item, int x1, int y1, int x2, int y2 );
                       virtual ASCString getItemName ( pterraintype item );
};


bool SelectTerrainType :: isavailable ( pterraintype item )
{
   return !ItemFiltrationSystem::isFiltered( ItemFiltrationSystem::Terrain, item->id );
}

void SelectTerrainType :: displaysingleitem ( pterraintype item, int x, int y )
{
   bar ( x, y, x + getitemsizex(), y + getitemsizey(), black );
   if ( auswahlw >= cwettertypennum || auswahlw < 0 )
      auswahlw = 0;

   if ( item )
      if ( item->weather[auswahlw] )
         item->weather[auswahlw]->paint ( SPoint(x, y) );
      else
         item->weather[0]->paint ( SPoint(x, y) );
}

ASCString SelectTerrainType :: getItemName ( pterraintype item )
{
   if ( !item->name.empty() )
      return item->name;
   else
      return "-NONE-";
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

      showtext4 ( "Name: %s", x1 + 20 + getitemsizex(), y1 + 10, getItemName(item).c_str() );

      showtext4 ( "ID: %d", x1 + 20 + getitemsizex(), y1 + 30, item->id );
      npop ( activefontsettings );
   }
}





class SelectObjectType : public SelectAnything< pobjecttype > {
                    protected:
                       virtual bool isavailable ( pobjecttype item );
                       virtual void displaysingleitem ( pobjecttype item, int x, int y );
                       virtual int getitemsizex ( void ) { return fieldsizex; } ;
                       virtual int getitemsizey ( void ) { return fieldsizey; } ;
                       virtual void showiteminfos ( pobjecttype item, int x1, int y1, int x2, int y2 );
                       virtual ASCString getItemName ( pobjecttype item );
};

bool SelectObjectType :: isavailable ( pobjecttype item )
{
  return !ItemFiltrationSystem::isFiltered( ItemFiltrationSystem::Object, item->id );
}


void SelectObjectType :: displaysingleitem ( pobjecttype item, int x, int y )
{
   bar ( x, y, x + getitemsizex(), y + getitemsizey(), black );
   if ( item )
      item->display ( getActiveSurface(), SPoint(x, y) );
}

ASCString SelectObjectType :: getItemName ( pobjecttype item )
{
   if ( !item->name.empty() )
      return item->name;
   else
      return "-NONE-";
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

      showtext4 ( "Name: %s", x1 + 20 + getitemsizex(), y1 + 10, getItemName(item).c_str() );

      showtext4 ( "ID: %d", x1 + 20 + getitemsizex(), y1 + 30, item->id );
      npop ( activefontsettings );
   }
}





class SelectBuildingType : public SelectAnything< pbuildingtype > {
                       int buildingfieldsdisplayedx, buildingfieldsdisplayedy;
                    protected:
                       virtual bool isavailable ( pbuildingtype item );
                       virtual void displaysingleitem ( pbuildingtype item, int x, int y );
                       virtual int getitemsizex ( void ) { return fieldsizex+(buildingfieldsdisplayedx-1)*fielddistx+fielddisthalfx; } ;
                       virtual int getitemsizey ( void ) { return fieldsizey+(buildingfieldsdisplayedy-1)*fielddisty; } ;
                       virtual void showiteminfos ( pbuildingtype item, int x1, int y1, int x2, int y2 );
                       virtual ASCString getItemName ( pbuildingtype item );
                    public:
                       SelectBuildingType( void ) { buildingfieldsdisplayedx = 4; buildingfieldsdisplayedy = 6; };
};

bool SelectBuildingType :: isavailable ( pbuildingtype item )
{
  return !ItemFiltrationSystem::isFiltered( ItemFiltrationSystem::Building, item->id );
}

ASCString SelectBuildingType :: getItemName ( pbuildingtype item )
{
   if ( item->name.empty() )
      return "-NONE-";
   else
      return item->name;
}


void SelectBuildingType :: displaysingleitem ( pbuildingtype item, int x, int y )
{
   bar ( x, y, x + getitemsizex(), y + getitemsizey(), black );
   if ( item )
      item->paint( getActiveSurface(), SPoint(x,y), farbwahl );
}

void SelectBuildingType :: showiteminfos ( pbuildingtype item, int x1, int y1, int x2, int y2 )
{ 
   rectangle ( x1, y1, x2, y2, lightgray );
   bar ( x1+1, y1+1, x2-1, y2-1, black );
   if ( item ) {
      item->paintSingleField( getActiveSurface(), SPoint(x1 + 10, y1 + (y2 - y1 - fieldsizey )/2), item->entry , farbwahl );
      npush ( activefontsettings );
      activefontsettings.font = schriften.smallarial;
      activefontsettings.background  = 255;
      activefontsettings.justify = lefttext;
      activefontsettings.color = lightgray;
      activefontsettings.height = 0;
      activefontsettings.length = x2 - x1 - 20 - fieldsizex;

      showtext4 ( "Name: %s", x1 + 20 + fieldsizex, y1 + 10, getItemName(item).c_str() );

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
vector< pweathertype > weathervector;


class tcolortype {
         public:
            int col;
            int getcolor ( void ) { return 20 + 8 * col; };
       };
typedef tcolortype* pcolortype;
vector< pcolortype > colorvector;


class tminetype {
         public:
            int type;
            char* name;
            void paint ( int x, int y ) { 
            Mine::paint( MineTypes(type+1), farbwahl, getActiveSurface(), SPoint(x,y) ); 
            };
       };
typedef tminetype* pminetype;
vector< pminetype > minevector;



class SelectColor : public SelectAnything< pcolortype > {
                    protected:
                       virtual bool isavailable ( pcolortype item ) { return 1; };
                       virtual void displaysingleitem ( pcolortype item, int x, int y );
                       virtual int getitemsizex ( void ) { return fieldsizex; } ;
                       virtual int getitemsizey ( void ) { return fieldsizey; } ;
                       virtual void showiteminfos ( pcolortype item, int x1, int y1, int x2, int y2 );
                       virtual int getiteminfoheight( void ) { return 20; };
                       virtual ASCString getItemName ( pcolortype item );
};

ASCString SelectColor :: getItemName ( pcolortype item )
{
  return strrr ( item->col );
}


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
      showtext2 ( getItemName(item).c_str(), x, y + (getitemsizey() - activefontsettings.font->height) / 2 );
      npop ( activefontsettings );
   } else
      bar ( x, y, x + getitemsizex(), y + getitemsizey(), black );
}

void SelectColor :: showiteminfos ( pcolortype item, int x1, int y1, int x2, int y2 )
{ 
    rectangle ( x1, y1, x2, y2, lightgray );
    if ( item )
       bar ( x1+1, y1+1, x2-1 , y2-1, item->getcolor() );
    else
       bar ( x1+1, y1+1, x2-1 , y2-1, 0 );
}





class SelectWeather : public SelectAnything< pweathertype > {
                    protected:
                       virtual bool isavailable ( pweathertype item ) { return 1; };
                       virtual void displaysingleitem ( pweathertype item, int x, int y );
                       virtual int getitemsizex ( void ) { return 120; } ;
                       virtual int getitemsizey ( void ) { return 25; } ;
                       virtual void showiteminfos ( pweathertype item, int x1, int y1, int x2, int y2 );
                       virtual ASCString getItemName ( pweathertype  item ) ;
};

ASCString SelectWeather :: getItemName ( pweathertype  item )
{
  return item->name;
}

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
      showtext2 ( getItemName( item ).c_str(), x+5, y + (getitemsizey() - activefontsettings.font->height) / 2 );
      npop ( activefontsettings );
   }
}

void SelectWeather :: showiteminfos ( pweathertype item, int x1, int y1, int x2, int y2 )
{ 
   rectangle ( x1, y1, x2, y2, lightgray );
   bar ( x1+1, y1+1, x2-1, y2-1, black );
   if ( item && auswahl ) 
      if ( auswahl->weather[ item->num ] ) {
         auswahl->weather[item->num]->paint ( SPoint(x1 + 10, (y1 + y2 - fieldsizey )/2) );
      } else {

         auswahl->weather[0]->paint ( SPoint(x1 + 10, (y1 + y2 - fieldsizey )/2) );

         npush ( activefontsettings );
         activefontsettings.font = schriften.smallarial;
         activefontsettings.background  = 255;
         activefontsettings.justify = lefttext;
         activefontsettings.color = lightgray;
         activefontsettings.height = 0;
         activefontsettings.length = x2 - x1 - 20 - fieldsizex;
   
         showtext4 ( "the terrain '%s'", x1 + 20 + fieldsizex, y1 + 10, auswahl->name.c_str() );
         showtext2 ( "does not have this weather", x1 + 20 + fieldsizex, y1 + 30 );
   
         npop ( activefontsettings );
      }
   
}



class SelectMine : public SelectAnything< pminetype > {
                    protected:
                       virtual bool isavailable ( pminetype item ) { return 1; };
                       virtual void displaysingleitem ( pminetype item, int x, int y );
                       virtual int getitemsizex ( void ) { return fieldsizex; } ;
                       virtual int getitemsizey ( void ) { return fieldsizey; } ;
                       virtual void showiteminfos ( pminetype item, int x1, int y1, int x2, int y2 );
                       virtual ASCString getItemName ( pminetype  item );
};

void SelectMine :: displaysingleitem ( pminetype item, int x, int y )
{
   bar ( x, y, x + getitemsizex(), y + getitemsizey(), black );
   if ( item ) 
      item->paint ( x, y );
}

ASCString SelectMine :: getItemName ( pminetype  item )
{
   if ( item->name && item->name[0] )
      return item->name;
   else
      return "-NONE-";
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

      showtext4 ( "Name: %s", x1 + 20 + getitemsizex(), y1 + 10, getItemName(item).c_str() );

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
                     selectterraintype->init( terrainTypeRepository.getVector() );
                  }
                  return selectterraintype;
             };

             SelectBuildingType*   getbuildingselector ( void ) {
                  if ( !selectbuildingtype ) {
                     selectbuildingtype = new SelectBuildingType;
                     selectbuildingtype->init( buildingTypeRepository.getVector() );
                  }
                  return selectbuildingtype;
             };

             SelectVehicleType*   getvehicleselector ( void ) {
                  if ( !selectvehicletype ) {
                     selectvehicletype = new SelectVehicleType;
                     selectvehicletype->init( vehicleTypeRepository.getVector() );
                  }
                  return selectvehicletype;
             };

             SelectObjectType*   getobjectselector ( void ) {
                  if ( !selectobjecttype ) {
                     selectobjecttype = new SelectObjectType;
                     selectobjecttype->init( objectTypeRepository.getVector() );
                  }
                  return selectobjecttype;
             };

             SelectWeather*   getweatherselector ( void ) {
                  if ( !selectweather ) {
                     for ( int i = 0; i < cwettertypennum; i++ ) {
                        pweathertype wt = new tweathertype;
                        wt->num = i;
                        wt->name = strdup ( cwettertypen[i] );
                        weathervector.push_back ( wt );
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
                        minevector.push_back( mt );
                        mt->name = strdup ( MineNames[ i ] );
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
                        colorvector.push_back( ct );
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
      showtext2 ( selector[num].name, x1, y1 );

      y1 += activefontsettings.font->height;


      selector[num].selector->showactiteminfos ( x1, y1, x2, y2 );
      bar ( x2, y1, x2 + keywidth, y2, backgroundcol );

      selectionypos += selector[num].selector->getiteminfoheight() + freespace;
      if (y2 + freespace < agmp->resolutiony )
         bar ( x1, y2+1, x2 + keywidth, y2 + freespace, black );
      activefontsettings.background  = 255;
      activefontsettings.color = black;
      activefontsettings.length = keywidth;
      showtext2 ( selector[num].keyname, x2, y1 + ( y2 - y1 - activefontsettings.font->height ) / 2);
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
         case 0: if ( skeypress ( ct_lstrg ) || skeypress ( ct_rstrg ) || skeypress ( ct_lshift ) || skeypress ( ct_rshift ))
                    selterraintype ( ct_invvalue );
                 else
                    execaction(act_switchmaps );
            break;
         case 1: selvehicletype ( ct_invvalue );
            break;
         case 2: selcolor ( ct_invvalue );
            break;
         case 3: selbuilding ( ct_invvalue );
            break;
         case 4: if ( skeypress ( ct_lstrg ) || skeypress ( ct_rstrg ) || skeypress ( ct_lshift ) || skeypress ( ct_rshift ))
                    selobject ( ct_invvalue );
                 else
                    execaction(act_switchmaps );
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

pvehicletype selvehicletype(tkey ench )
{
   auswahlf = selectitemcontainer.getvehicleselector()->selectitem( auswahlf, ench );
   lastselectiontype = cselunit;
   showallchoices();
   return auswahlf;
}

void selcolor( tkey ench )
{
   int oldsel = farbwahl;
   farbwahl = selectitemcontainer.getcolorselector()->selectitem( colorvector[farbwahl], ench )->col;
   if ( farbwahl > 8 )
      farbwahl = oldsel;
       
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
   selectitemcontainer.getvehicleselector()->init( vehicleTypeRepository.getVector() );
}

void resetbuildingselector ( void )
{
   selectitemcontainer.getbuildingselector()->init( buildingTypeRepository.getVector() );
}

void resetterrainselector ( void )
{
   selectitemcontainer.getterrainselector()->init( terrainTypeRepository.getVector() );
}

void resetobjectselector ( void )
{
   selectitemcontainer.getobjectselector()->init( objectTypeRepository.getVector() );
}


void setnewterrainselection ( pterraintype t )
{
   selectitemcontainer.getterrainselector()->setnewselection( t );
}

void setnewobjectselection  ( pobjecttype o )
{
   selectitemcontainer.getobjectselector()->setnewselection( o );
}

void setnewbuildingselection ( pbuildingtype v )
{
   selectitemcontainer.getbuildingselector()->setnewselection ( v );
   selectitemcontainer.getcolorselector()->setnewselection( colorvector[farbwahl] );
//   showallchoices();
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

      if ( !item->name.empty() )
         showtext4 ( "Name: %s", x1 + 20 + getitemsizex(), y1 + 10, item->name.c_str() );
      else
         if ( !item->description.empty()  )
            showtext4 ( "Name: %s", x1 + 20 + getitemsizex(), y1 + 10, item->description.c_str() );
         else
            showtext4 ( "Name: NONE", x1 + 20 + getitemsizex(), y1 + 10 );

      showtext4 ( "weight: %d", x1 + 20 + getitemsizex(), y1 + 30, item->maxsize() );
      npop ( activefontsettings );
   }
}



class SelectVehicleTypeForContainerCargo : public SelectCargoVehicleType {
         ContainerBase* container;
      public:
         SelectVehicleTypeForContainerCargo ( ContainerBase* _container ) { container = _container; };
         bool isavailable ( pvehicletype item );
     };

bool SelectVehicleTypeForContainerCargo :: isavailable ( pvehicletype item )
{
   bool result = false;
   if ( SelectVehicleType::isavailable ( item ) ) {
      Vehicle* unit = new Vehicle ( item, actmap, container->getOwner() );
      if ( container->vehicleFit ( unit ))
         result = true;
      delete unit;
   }
   return result;
}

class SelectVehicleTypeForBuildingProduction : public SelectCargoVehicleType {
         Building* building;
      public:
         SelectVehicleTypeForBuildingProduction ( Building* _building ) { building = _building; };
         bool isavailable ( pvehicletype item ) {  
            for ( int i = 0; i < 32; i++ )
               if ( building->production[i] == item )
                  return 0;
            return building->typ->vehicleFit ( item )
                   && SelectVehicleType::isavailable ( item )
                   &&  ( building->vehicleUnloadable(item) || (building->typ->special & cgproduceAllUnitsB ));
         };
     };

//* õS Fahrzeuge fr Unit-Beloading


void selcargo( ContainerBase* container )
{
   SelectVehicleTypeForContainerCargo svtftc ( container );
   svtftc.init( vehicleTypeRepository.getVector() );
   pvehicletype newcargo = svtftc.selectitem ( NULL );

   if ( newcargo ) {
      Vehicle* unit = new Vehicle ( newcargo, actmap, container->getOwner() );
      unit->fillMagically();
      unit->setnewposition ( container->getPosition() );
      unit->tank.material = 0;
      unit->tank.fuel = 0;
      if ( container->vehicleFit ( unit )) {
         unit->tank.material = unit->typ->tank.material;
         unit->tank.fuel = unit->typ->tank.fuel;

         if ( !container->vehicleFit ( unit )) {
            unit->tank.material = 0;
            unit->tank.fuel = 0;
            displaymessage("Warning:\nThe unit you just set could not be loaded with full material and fuel\nPlease set these values manually",1);
         }
         int p = 0;
         while ( container->loading[p] && p < 32 )
            p++;
         if ( p < 32 )
            container->loading[p] = unit;

      } else {
        delete unit;
        displaymessage("The unit could not be loaded !",1);
      }
   }
}



//* õS Fahrzeuge fr Geb„ude-Production

void selbuildingproduction( Building* bld )
{  
   SelectVehicleTypeForBuildingProduction svtfbc ( bld );
   svtfbc.init( vehicleTypeRepository.getVector() );
   pvehicletype newcargo = svtfbc.selectitem ( NULL );
   if ( newcargo ) {
      int p = 0;
      while ( bld->production[p] )
        p++;
      bld->production[p] = newcargo;
   }
}



