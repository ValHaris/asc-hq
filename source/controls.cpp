/*! \file controls.cpp
   Controlling units (which is graudally moved to #vehicletype.cpp and #unitctrl.cpp );
   Resource networks
   Things that are run when starting and ending someones turn   
*/

//     $Id: controls.cpp,v 1.94 2001-01-28 17:18:58 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.93  2001/01/28 14:04:07  mbickel
//      Some restructuring, documentation and cleanup
//      The resource network functions are now it their own files, the dashboard
//       as well
//      Updated the TODO list
//
//     Revision 1.92  2001/01/25 23:44:52  mbickel
//      Moved map displaying routins to own file (mapdisplay.cpp)
//      Wrote program to create pcx images from map files (map2pcx.cpp)
//      Fixed bug in repair function: too much resource consumption
//      AI improvements and bug fixes
//      The BI3 map import function now evaluates the player status (human/
//       computer)
//
//     Revision 1.91  2001/01/23 21:05:12  mbickel
//      Speed up of AI
//      Lot of bugfixes in AI
//      Moved Research to own files (research.*)
//      Rewrote storing of developed technologies
//      Some cleanup and documentation
//
//     Revision 1.90  2001/01/21 16:37:14  mbickel
//      Moved replay code to own file ( replay.cpp )
//      Fixed compile problems done by cleanup
//
//     Revision 1.89  2001/01/19 13:33:47  mbickel
//      The AI now uses hemming
//      Several bugfixes in Vehicle Actions
//      Moved all view calculation to viewcalculation.cpp
//      Mapeditor: improved keyboard support for item selection
//
//     Revision 1.88  2000/12/23 13:19:43  mbickel
//      Made ASC compileable with Borland C++ Builder
//
//     Revision 1.87  2000/12/21 11:00:47  mbickel
//      Added some code documentation
//
//     Revision 1.86  2000/11/29 09:40:13  mbickel
//      The mapeditor has now two maps simultaneously active
//      Moved memorychecking functions to its own file: memorycheck.cpp
//      Rewrote password handling in ASC
//
//     Revision 1.85  2000/11/21 20:26:56  mbickel
//      Fixed crash in tsearchfields (used by object construction for example)
//      AI improvements
//      configure.in: added some debug output
//                    fixed broken check for libbz2
//
//     Revision 1.84  2000/11/11 11:05:15  mbickel
//      started AI service functions
//
//     Revision 1.83  2000/11/08 19:30:56  mbickel
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
#include <math.h>
#include <stdarg.h>

#include "buildingtype.h"
#include "vehicletype.h"
#include "typen.h"

#include "basegfx.h"
#include "newfont.h"
#include "spfst.h"
#include "loaders.h"
#include "misc.h"
#include "controls.h"
#include "network.h"
#include "events.h"
#include "loadpcx.h"
#include "stack.h"
#include "dlg_box.h"
#include "dialog.h"
#include "attack.h"
#include "building.h"
#include "gamedlg.h"
#include "missions.h"
#include "sg.h"
#include "weather.h"
#include "gameoptions.h"
#include "artint.h"
#include "errors.h"
#include "password_dialog.h"
#include "viewcalculation.h"
#include "replay.h"
#include "dashboard.h"
#include "resourcenet.h"

         int             windmovement[8];

         tmoveparams moveparams;


class InitControls {
  public:
   InitControls ( void ) 
   {
      memset ( (void*) &moveparams, 0, sizeof ( moveparams ));
   };
} initControls;



  class   tsearchexternaltransferfields : public tsearchfields {
                      public:
                                pbuilding            bld;
                                char                    numberoffields;
                                void                    searchtransferfields( pbuilding building );
                                virtual void            testfield ( void );
                                tsearchexternaltransferfields ( pmap _gamemap ) : tsearchfields ( _gamemap ) {};
                             };

void         tsearchexternaltransferfields :: testfield( void )
{ 

  if ((xp >= 0) && (yp >= 0) && (xp < actmap->xsize) && (yp < actmap->ysize)) {
     pfield fld  = getfield ( xp, yp );
     if ( fld && fld->vehicle )
        if ( fld->vehicle->height & bld->typ->externalloadheight ) {
           numberoffields++;
           fld->a.temp = 123;
        }
  }
} 


void tsearchexternaltransferfields :: searchtransferfields( pbuilding building )
{
   actmap->cleartemps( 7 );
   bld = building;
   int x;
   int y;
   numberoffields = 0;
   if ( bld->typ->special & cgexternalloadingb ) {
      bld->getFieldCoordinates ( bld->typ->entry.x, bld->typ->entry.y, x, y );
      initsearch( x, y, 1, 1 );
   
      startsearch();
   }
   if ( numberoffields )
      moveparams.movestatus = 130;
}

int searchexternaltransferfields ( pbuilding bld )
{
   tsearchexternaltransferfields setf ( actmap );
   setf.searchtransferfields ( bld );
   return setf.numberoffields;
}

          
// #define netdebug 

  class   tsearchputbuildingfields : public tsearchfields {
                      public:
                                pbuildingtype        bld;
                                char                    numberoffields;
                                pfield                   startfield;
                                void                    initputbuilding( word x, word y, pbuildingtype building );
                                virtual void            testfield ( void );
                                tsearchputbuildingfields ( pmap _gamemap ) : tsearchfields ( _gamemap ) {};
                             };

  class   tsearchdestructbuildingfields : public tsearchfields {
                      public:
                                char                    numberoffields;
                                pfield                   startfield;
                                void                    initdestructbuilding( int x, int y );
                                virtual void            testfield ( void );
                                tsearchdestructbuildingfields ( pmap _gamemap ) : tsearchfields ( _gamemap ) {};
                             };




void         tsearchputbuildingfields::initputbuilding( word x, word y, pbuildingtype building )
{ 
   pvehicle eht = getfield(x,y)->vehicle; 

   if ( eht->attacked || (eht->typ->wait && eht->hasMoved() )) {
      dispmessage2(302,""); 
      return;
   } 

   actmap->cleartemps(7); 
   initsearch(x,y,1,1);
   bld = building;
   numberoffields = 0;
   startfield = getfield(x,y);
   startsearch();
   if (numberoffields > 0) {
      moveparams.movestatus = 111;
      moveparams.vehicletomove = eht;
      moveparams.buildingtobuild = bld;
   }
   else {
      dispmessage2(301,"");
   }
}


void         tsearchputbuildingfields::testfield(void)
{
  int          x2, y2;
  int         x1, y1;
  pfield        fld;
  char      b;


   if ((xp >= 0) && (yp >= 0) && (xp < actmap->xsize) && (yp < actmap->ysize)) {
      startfield = getfield(xp,yp);
      b = true;
      for (y1 = 0; y1 <= 5; y1++)
         for (x1 = 0; x1 <= 3; x1++)
            if (bld->getpicture (x1, y1) ) {
               bld->getfieldcoordinates(xp, yp,x1,y1, &x2, &y2);
               fld = getfield(x2,y2);
               if (fld) {
                  if (fld->vehicle != NULL)
                     b = false;
                  if ( bld->terrain_access->accessible ( fld->typ->art ) <= 0 )
                     b = false;
                  if (fld->building != NULL) {
                     if (fld->building->typ != bld)
                        b = false;
                     if (fld->building->completion == fld->building->typ->construction_steps - 1)
                        b = false;
                     if ( ! (startfield->bdt & cbbuildingentry ) )
                        b = false;
                  }
                  if (startfield->building != fld->building)
                     b = false;
               } else
                  b = false;
            }
      if (b) {
         numberoffields++;
         getfield(xp,yp)->a.temp = 20;
      }

   }
}







void         putbuildinglevel1(void)
{

   tsearchputbuildingfields   spbf ( actmap );
   tkey                       taste;
   pvehicle                   eht;

   eht = getactfield()->vehicle;

   int num = 0;
   for ( int i = 0; i < buildingtypenum; i++)
      if ( eht->buildingconstructable ( getbuildingtype_forpos( i ) ))
         num++;

   if ( num == 0 ) {
      dispmessage2( 303, NULL );
      return;
   }

   mousevisible(false);

   actgui->restorebackground();
   npush ( actgui );
   selectbuildinggui.reset ( eht );
   selectbuildinggui.painticons();
   actgui = &selectbuildinggui;

   mousevisible(true);
   do {
     if ( keypress() ) {
        taste = r_key();
        selectbuildinggui.checkforkey(taste);
     }

     selectbuildinggui.checkformouse();

   } while ( (selectbuildinggui.selectedbuilding == NULL) && (selectbuildinggui.cancel == 0));

   selectbuildinggui.restorebackground();
   npop ( actgui );
   mousevisible(false);

   if ( selectbuildinggui.selectedbuilding )
      spbf.initputbuilding( getxpos(), getypos(), selectbuildinggui.selectedbuilding );

   mousevisible(true);
}




void         putbuildinglevel2( const pbuildingtype bld,
                               integer      xp,
                               integer      yp)
{
  int          x1, y1, x2, y2;
  pfield        fld;
  char      b = true;

   if (getfield(xp,yp)->a.temp == 20)
      if (moveparams.movestatus == 111) {
         actmap->cleartemps(7);
         for (y1 = 0; y1 <= 5; y1++)
            for (x1 = 0; x1 <= 3; x1++)
               if ( bld->getpicture ( x1, y1 ) ) {
                  bld->getfieldcoordinates(xp,yp,  x1, y1, &x2, &y2);

                  fld = getfield(x2,y2);
                  if ( fld ) {
                      if ( fld->vehicle || (fld->building && fld->building->completion == fld->building->typ->construction_steps-1 ))
                         b = false;

                        /* if fld^.typ^.art and bld^.terrain = 0 then
                      b:=false; */
                      if ( b )
                           if ( x1 == bld->entry.x  && y1 == bld->entry.y ) {
                              moveparams.movesx = x2;
                              moveparams.movesy = y2;
                              fld->a.temp = 23;
                           } else
                              fld->a.temp = 22;
                      else
                         displaymessage("severe error: \n inconsistency between level1 and level2 of putbuilding",2);
                  } else
                    b = false;
               }
         if ( b ) {
            moveparams.movestatus = 112;
         }
      }
}


void         putbuildinglevel3(integer      x,
                               integer      y)
{
  pbuildingtype bld;
  pvehicle     eht;

   if (getfield(x,y)->a.temp == 23)
      if (moveparams.movestatus == 112) {
         actmap->cleartemps(7);
         bld = moveparams.buildingtobuild;
         eht = moveparams.vehicletomove;
         putbuilding2(x,y,eht->color,bld);

         logtoreplayinfo ( rpl_putbuilding, (int) x, (int) y, (int) bld->id, (int) eht->color );

         int mf = actmap->getgameparameter ( cgp_building_material_factor );
         int ff = actmap->getgameparameter ( cgp_building_fuel_factor );
         if ( mf <= 0 )
            mf = 100;

         if ( ff <= 0 )
            ff = 100;

         if (eht->tank.material < bld->productionCost.material * mf / 100 ) {
            displaymessage("not enough material!",1);
            eht->tank.material = 0;
         }
         else
            eht->tank.material -= bld->productionCost.material * mf / 100;


         if (eht->tank.fuel < bld->productionCost.fuel * ff / 100) {
            displaymessage("not enough fuel!",1);
            eht->tank.fuel = 0;
         }
         else
            eht->tank.fuel -= bld->productionCost.fuel * ff / 100;

         moveparams.movestatus = 0;
         eht->setMovement ( 0 );
         eht->attacked = true;
         computeview( actmap );
      }
}








void         tsearchdestructbuildingfields::initdestructbuilding( int x, int y )
{
   pvehicle     eht = getfield(x,y)->vehicle;
   if (eht->attacked || (eht->typ->wait && eht->hasMoved() )) {
      dispmessage2(305,NULL);
      return;
   }
   actmap->cleartemps(7);
   initsearch(x,y,1,1);
   numberoffields = 0;
   startfield = getfield(x,y);
   startsearch();
   if (numberoffields > 0) {
      moveparams.movestatus = 115;
      moveparams.vehicletomove = eht;
   }
   else {
      dispmessage2(306,"");
   }
}


void         tsearchdestructbuildingfields::testfield(void)
{
   if ((xp >= 0) && (yp >= 0) && (xp < actmap->xsize) && (yp < actmap->ysize)) {
      startfield = getfield(xp,yp);
      if (startfield->building) {
         numberoffields++;
         startfield->a.temp = 20;
      }

   }
}





void         destructbuildinglevel1(int xp, int yp)
{
   tsearchdestructbuildingfields   sdbf ( actmap );
   sdbf.initdestructbuilding( xp, yp  );
}


void         destructbuildinglevel2( int xp, int yp)
{
   pfield fld = getfield(xp,yp);
   if (fld->a.temp == 20)
      if (moveparams.movestatus == 115) {
         actmap->cleartemps(7);
         pvehicle eht = moveparams.vehicletomove;
         pbuildingtype bld = fld->building->typ;



         pbuilding bb = fld->building;

         eht->tank.material += bld->productionCost.material * (100 - bb->damage) / destruct_building_material_get / 100;
         if ( eht->tank.material > eht->typ->tank.material )
            eht->tank.material = eht->typ->tank.material;

         eht->setMovement ( 0 );
         eht->attacked = 1;
         eht->tank.fuel -= destruct_building_fuel_usage * eht->typ->fuelConsumption;
         if ( bb->completion ) {
            bb->changecompletion ( -1 );
         } else {
            removebuilding ( &bb );
         }
         logtoreplayinfo ( rpl_removebuilding, xp, yp );
         computeview( actmap );
         displaymap();
         moveparams.movestatus = 0;
      }
}










void         tputmine::testfield(void)
{
   pfield        fld;

   if ((xp >= 0) && (yp >= 0) && (xp < actmap->xsize) && (yp < actmap->ysize)) {
      fld = getfield(xp,yp);
      if ( !fld->vehicle  &&  !fld->building ) {
         fld->a.temp = 0;
         if (fld->object && fld->object->mine ) {
            fld->a.temp += 2;
            numberoffields++;
         }
         if (mienenlegen && (!fld->minenum() || fld->mineowner() == player)) {
            fld->a.temp += 1;
            numberoffields++;
         }
      }
   }
}




void         tputmine::initpm(  char mt, const pvehicle eht )
{
  int         i;

   numberoffields = 0;
   mienenlegen = false;
   mienenraeumen = false;
   if (eht->typ->weapons->count > 0)
      for (i = 0; i <= eht->typ->weapons->count - 1; i++)
         if ((eht->typ->weapons->weapon[i].getScalarWeaponType() == cwminen) && eht->typ->weapons->weapon[i].shootable() ) {
            mienenraeumen = true;
            if (eht->ammo[i] > 0)
               mienenlegen = true;
         }
   player = eht->color / 8;
   mienentyp = mt;
   if (eht->getMovement() < mineputmovedecrease) {
      mienenlegen = false;
      mienenraeumen = false;
   }
   if (mienenlegen || mienenraeumen)
      initsearch( getxpos(),getypos(),1,1);
}


void         tputmine::run(void)
{
   if ((mienenlegen || mienenraeumen)) {
      startsearch();
      if (numberoffields > 0) {
         moveparams.movestatus = 90;
         moveparams.movesx = getxpos();
         moveparams.movesy = getypos();

      }
   }
}






void  legemine( int typ, int delta )
{
   if (moveparams.movestatus == 0) {
      tputmine ptm ( actmap );
      pvehicle eht = getactfield()->vehicle;
      moveparams.vehicletomove = eht;
      if (eht == NULL)
         return;
      if (eht->color != (actmap->actplayer << 3))
         return;
      ptm.initpm(typ,eht);
      ptm.run();
   }
   else
      if (moveparams.movestatus == 90) {
         pvehicle eht = moveparams.vehicletomove;
         pfield fld = getactfield();
         if ( fld->a.temp ) {

            if ( (fld->a.temp & 1) && ( delta > 0 )) {
               pvehicletype fzt = eht->typ;
               int  strength = 64;
               for ( int i = 0; i < fzt->weapons->count ; i++)
                  if ((fzt->weapons->weapon[i].getScalarWeaponType() == cwminen) && fzt->weapons->weapon[i].shootable() )
                     if ( fld-> putmine( actmap->actplayer, typ, cminestrength[typ-1] * strength / 64 )) {
                        eht->ammo[i]--;
                        eht->setMovement ( eht->getMovement() - mineputmovedecrease );
                        strength = eht->weapstrength[i];
                        int x = getxpos();
                        int y = getypos();
                        logtoreplayinfo ( rpl_putmine, x, y, (int) actmap->actplayer, (int) typ, (int) cminestrength[typ-1] * strength / 64 );
                        break;
                     }


            }

            if ( (fld->a.temp & 2) && ( delta < 0 )) {
               int x = getxpos();
               int y = getypos();
               pfield fld = getactfield();
               fld -> removemine( -1 );
               logtoreplayinfo ( rpl_removemine, x, y );
            }
            actmap->cleartemps(7);
            computeview( actmap );
            moveparams.movestatus = 0;
         }
      }
}







/*
void         refuelvehicle(int         b)
{
   pvehicle     actvehicle;

   if (moveparams.movestatus == 0) {

      trefuelvehicle rfe;
      rfe.initrefuelling(getxpos(),getypos(),b);
      rfe.startsearch();

   }
   else {
      if (moveparams.movestatus == 65) {
         if (getactfield()->a.temp > 0) {
            actvehicle = getfield(moveparams.movesx,moveparams.movesy)->vehicle;
            verlademunition(getactfield()->vehicle,actvehicle,NULL,3 - b);
            actmap->cleartemps(7);
            moveparams.movestatus = 0;
         }
      }
      else
         if (moveparams.movestatus == 66)
            if (getactfield()->a.temp > 0) {
               actvehicle = getfield(moveparams.movesx,moveparams.movesy)->vehicle;
               // actvehicle->repairunit( getactfield()->vehicle );
               actmap->cleartemps(7);
               moveparams.movestatus = 0;
            }
      dashboard.x = 0xffff;
   }

}
*/

typedef class tobjectcontainers_buildable_on_field *pobjectcontainers_buildable_on_field;
class tobjectcontainers_buildable_on_field {
       public:
          dynamic_array<pobjecttype> objects_buildable;
          dynamic_array<pobjecttype> objects_removable;
          int objects_buildable_num;
          int objects_removable_num;
          int x,y;
          tobjectcontainers_buildable_on_field ( int xp, int yp ) {
              objects_buildable_num = 0;
              objects_removable_num = 0;
              x = xp;
              y = yp;
          };
     };

typedef class tobjectcontainers_buildable *pobjectcontainers_buildable ;
class tobjectcontainers_buildable {
        int inited;
        BasicGuiHost* oldgui;
       public:
        dynamic_array<pobjectcontainers_buildable_on_field> field;
        int fieldnum;

        void init ( void )
        {
           if ( fieldnum && inited )
              reset();
           fieldnum = 0;
           inited = 1;
        };

        void start ( void )
        {
           init();
           oldgui = actgui;
           actgui = &selectobjectcontainergui;
        }


        tobjectcontainers_buildable ( void )
        {
           inited = 0;
           init ();
        };


        void reset ( void )
        {
           for ( int i = 0; i< fieldnum; i++ )
              delete field[i];

           fieldnum = 0;
           // actgui = oldgui;

        };

        ~tobjectcontainers_buildable (  )
        {
           reset ();
        };

};


tobjectcontainers_buildable objects_buildable;


int  object_constructable ( int x, int y, pobjecttype obj )
{
   for ( int i = 0; i < objects_buildable.fieldnum; i++ )
      if ( objects_buildable.field[i]->x == x   &&
           objects_buildable.field[i]->y == y ) {

         pobjectcontainers_buildable_on_field fld = objects_buildable.field[i];
         for ( int j = 0; j < fld->objects_buildable_num; j++ )
            if ( fld->objects_buildable[j] == obj )
               return 1;
      }

  return 0;
}

int  object_removeable ( int x, int y, pobjecttype obj )
{
   for ( int i = 0; i < objects_buildable.fieldnum; i++ )
      if ( objects_buildable.field[i]->x == x   &&
           objects_buildable.field[i]->y == y ) {

         pobjectcontainers_buildable_on_field fld = objects_buildable.field[i];
         for ( int j = 0; j < fld->objects_removable_num; j++ )
            if ( fld->objects_removable[j] == obj )
               return 1;
      }

  return 0;
}

void build_objects_reset( void )
{
   objects_buildable.reset();
   actmap->cleartemps(7);
   moveparams.movestatus = 0;
   displaymap();
   actgui->restorebackground();
   actgui = &gui;
}

   class tbuildstreet : public tsearchfields {
                public:
                       pvehicle         actvehicle;
                       word             numberoffields;

                       virtual void     testfield ( void );
                       void             initbs ( void );
                       void             run ( void );
                       tbuildstreet ( pmap _gamemap ) : tsearchfields ( _gamemap ) {};
                  };


void         tbuildstreet::initbs(void)
{
   startx = getxpos();
   starty = getypos();
   actvehicle = getfield(startx,starty)->vehicle;

   moveparams.movesx = startx;
   moveparams.movesy = starty;
   mindistance = 1;
   maxdistance = 1;
   numberoffields = 0;
}


void    getobjbuildcosts ( pobjecttype obj, pfield fld, Resources* resource, int* movecost )
{
   int bridgemultiple = 8;

#ifndef HEXAGON
   if ( fld->bdt & cbwater1 )
      bridgemultiple = brigde1buildcostincrease;
   else
      if ( fld->bdt & cbwater2 )
         bridgemultiple = brigde2buildcostincrease;
      else
         if ( fld->bdt & cbwater3 )
            bridgemultiple = brigde3buildcostincrease;
#endif


   int mvcost;

   if ( !fld->checkforobject ( obj ) ) {
      // int costmultiply = ( 8 + ( fld->movemalus[0] - 8 ) / ( objectbuildmovecost / 8 ) ) *  bridgemultiple / 8;
      *resource = obj->buildcost;

      #ifdef HEXAGON
       mvcost = obj->build_movecost;
      #else
       mvcost = obj->movecost;
      #endif

   }

   else {
      for ( int j = 0; j < resourceTypeNum; j++ )
         resource->resource(j) = obj->removecost.resource(j) * bridgemultiple / 8;

      #ifdef HEXAGON
       mvcost = obj->remove_movecost;
      #else
       mvcost = obj->movecost;
      #endif
   }

   *movecost =  ( 8 + ( fld->getmovemalus( 0 ) - 8 ) / ( objectbuildmovecost / 8 ) ) * mvcost  / 8  *  bridgemultiple / 8;
}

void         tbuildstreet::testfield(void)
{

   if ((xp >= 0) && (yp >= 0) && (xp < actmap->xsize) && (yp < actmap->ysize)) {
      pfield fld = getfield(xp,yp);


      pobjectcontainers_buildable_on_field obj = new tobjectcontainers_buildable_on_field ( xp, yp );

      objects_buildable.field[ objects_buildable.fieldnum++ ] = obj;

      if ( !fld->vehicle ) {
         if ( !fld->building ) {

            for ( int i = 0; i < actvehicle->typ->objectsbuildablenum; i++ ) {
               pobjecttype objtype = getobjecttype_forid ( actvehicle->typ->objectsbuildableid[i] );
               if ( objtype )
                   if ( objtype->terrainaccess.accessible( fld->bdt ) > 0 || fld->checkforobject ( objtype ) ) {
                      int movecost;
                      Resources cost;
                      getobjbuildcosts ( objtype, fld, &cost, &movecost );
                      if ( actvehicle->tank >= cost && actvehicle->getMovement() >= movecost ) {

                              if ( !fld->checkforobject ( objtype ) ) {
                                 obj->objects_buildable[ obj->objects_buildable_num++ ] = objtype;
                              } else
                                 obj->objects_removable[ obj->objects_removable_num++ ] = objtype;

                              fld->a.temp = 1;
                              numberoffields++;
                      }
                   }
            }

         }
      }
   }
}


void         tbuildstreet::run(void)
{
   startsearch();
   if (numberoffields > 0)
      moveparams.movestatus = 72;

}



class SearchVehicleConstructionFields : public tsearchfields {
                       pvehicle         actvehicle;
                public:
                       word             numberoffields;

                       virtual void     testfield ( void );
                       void             initbs ( void );
                       void             run ( void );
                       SearchVehicleConstructionFields ( pmap _gamemap ) : tsearchfields ( _gamemap ) {};
                  };


void         SearchVehicleConstructionFields::initbs(void)
{
   startx = getxpos();
   starty = getypos();
   actvehicle = getfield(startx,starty)->vehicle;

   moveparams.movesx = startx;
   moveparams.movesy = starty;
   mindistance = 1;
   maxdistance = 1;
   numberoffields = 0;
}


void         SearchVehicleConstructionFields::testfield(void)
{

   if ((xp >= 0) && (yp >= 0) && (xp < actmap->xsize) && (yp < actmap->ysize)) {
      pfield fld = getfield(xp,yp);

      if ( !fld->vehicle ) {
         if ( !fld->building ) {

            for ( int i = 0; i < actvehicle->typ->vehiclesbuildablenum; i++ ) {
               pvehicletype v = getvehicletype_forid (  actvehicle->typ->vehiclesbuildableid[i] );
               if ( v )
                   if ( actvehicle->vehicleconstructable ( v, xp, yp )) {
                      fld->a.temp = 1;
                      numberoffields++;
                   }
            }

         }
      }
   }
}


void         SearchVehicleConstructionFields::run(void)
{
   startsearch();
   if (numberoffields > 0)
      moveparams.movestatus = 120;

}




void         setspec( pobjecttype obj )
{

   if (moveparams.movestatus == 0) {

      tbuildstreet buildstreet ( actmap );

      objects_buildable.init();

      buildstreet.initbs();
      buildstreet.run();

      if ( moveparams.movestatus == 72 ) {
           actgui->restorebackground();
           selectobjectcontainergui.reset ( );
           selectobjectcontainergui.painticons();
           actgui = &selectobjectcontainergui;

      }

   }
   else {
      if (moveparams.movestatus == 72) {
         pvehicle eht = getfield(moveparams.movesx,moveparams.movesy)->vehicle;
         pfield fld = getactfield();

         /*
         int stat = 0;

         if ( actmap->objectcrc ) {
            if ( actmap->objectcrc->speedcrccheck->checkobj2 ( obj ))
               stat = 1;
         } else
            stat = 1;

         if ( stat ) {
         */
            int movecost;
            Resources cost;
            getobjbuildcosts ( obj, fld, &cost, &movecost );

            int x = getxpos();
            int y = getypos();
            if ( !fld->checkforobject ( obj ) ) {
               getactfield() -> addobject ( obj );
               logtoreplayinfo ( rpl_buildobj, x, y, obj->id );
            } else {
               getactfield() -> removeobject ( obj );
               logtoreplayinfo ( rpl_remobj, x, y, obj->id );
            }

            eht->tank -= cost;
            eht->setMovement ( eht->getMovement() - movecost );

         // }

         build_objects_reset();
      }
      dashboard.x = 0xffff;

   }
}


void build_vehicles_reset( void )
{
   objects_buildable.reset();
   actmap->cleartemps(7);
   moveparams.movestatus = 0;
   displaymap();
   actgui->restorebackground();
   actgui = &gui;
}



void         constructvehicle( pvehicletype tnk )
{

   if (moveparams.movestatus == 0) {

      moveparams.vehicletomove = getactfield()->vehicle;

      SearchVehicleConstructionFields svcf ( actmap );

      svcf.initbs();
      svcf.run();

      if ( moveparams.movestatus == 120 ) {
           actgui->restorebackground();
           selectvehiclecontainergui.reset ( moveparams.vehicletomove );
           selectvehiclecontainergui.painticons();
           actgui = &selectvehiclecontainergui;
      }
   }
   else {
      if (moveparams.movestatus == 120 ) {
         pvehicle eht = moveparams.vehicletomove;
         // pfield fld = getactfield();

         /*
         int stat = 0;
         if ( actmap->objectcrc ) {
            if ( actmap->objectcrc->speedcrccheck->checkunit2 ( tnk ))
               stat = 1;
         } else
            stat = 1;

         if ( stat ) {
         */

            int x = getxpos();
            int y = getypos();
            eht->constructvehicle ( tnk, x, y );
            logtoreplayinfo ( rpl_buildtnk, x, y, tnk->id, moveparams.vehicletomove->color/8 );
            computeview( actmap );

         // }

         build_vehicles_reset();
      }
      dashboard.x = 0xffff;

   }
}





int windbeeline ( int x1, int y1, int x2, int y2 ) {
  #ifdef HEXAGON
   int dist = 0;
   while ( x1 != x2  || y1 != y2 ) {
      dist+= minmalq;
      int direc = getdirection ( x1, y1, x2, y2 );
      dist -= windmovement[direc];
      getnextfield ( x1, y1, direc );
   }
   return dist;
  #else
   int direc, dist = 0;
   int x = x1;
   int y = y1;
   int a,b,dx,dy;

   while ( x != x2 || y != y2 ) {
      dx = (2 * x2 + (y2 & 1)) - (2 * x + (y & 1));
      dy = y2 - y;
      if (dx < 0)
         a = 0;
      else
         if (dx == 0)
            a = 1;
         else
            a = 2;
      if (dy < 0)
         b = 0;
      else
         if (dy == 0)
            b = 1;
         else
            b = 2;

      direc = directions[b][a][0];
      if (direc & 1)
         dist += minmalq;
      else
         dist += maxmalq;


      getnextfield( x, y, direc);

      dist -= windmovement[direc];
   };

   return dist;
  #endif
}













treactionfirereplay :: treactionfirereplay ( void )
{
   num = 0;
   unit = NULL;
}

void treactionfirereplay :: init ( pvehicle eht, IntFieldList* fieldlist )
{
   if ( runreplay.status > 0 ) {
      preactionfire_replayinfo rpli;
      do {
         rpli = runreplay.getnextreplayinfo ( );
         if ( rpli )
            replay[num++] = rpli;

      } while ( rpli );

      unit = eht;
   }
}

int  treactionfirereplay :: checkfield ( int x, int y, pvehicle &eht, MapDisplayInterface* md )
{
   int attacks = 0;

   if ( eht == unit ) {
      for ( int i = 0; i < num; i++ ) {
         preactionfire_replayinfo rpli = replay[ i ];
         if ( eht && rpli->x2 == x  && rpli->y2 == y ) {

             pfield fld = getfield ( rpli->x1, rpli->y1 );
             pfield targ = getfield ( rpli->x2, rpli->y2 );

             npush ( targ->vehicle );
             targ->vehicle = eht;
             eht->xpos = x;
             eht->ypos = y;

             int attackvisible = fieldvisiblenow ( fld ) || fieldvisiblenow ( targ );


             if ( md && attackvisible ) {
               cursor.setcolor ( 8 );

               cursor.gotoxy ( rpli->x1, rpli->y1 );
               int t = ticker;
               while ( t + 15 > ticker )
                  releasetimeslice();

               cursor.gotoxy ( x, y );
               t = ticker;
               while ( t + 15 > ticker )
                  releasetimeslice();

               cursor.setcolor ( 0 );
               cursor.hide();
             }
             attacks++;

             tunitattacksunit battle ( fld->vehicle, targ->vehicle, 1, rpli->wpnum );
             battle.av.damage = rpli->ad1;
             battle.dv.damage = rpli->dd1;
             if ( md && attackvisible  )
                battle.calcdisplay ( rpli->ad2, rpli->dd2 );
             else {
                battle.calc ();
                battle.av.damage = rpli->ad2;
                battle.dv.damage = rpli->dd2;
             }

             int killed = 0;
             if ( battle.dv.damage >= 100 )
                killed = 1;

             battle.setresult ();
             dashboard.x = 0xffff;

             npop ( targ->vehicle );

             if ( killed )
                eht = NULL;
         }
      }
   }
   return attacks;
}

treactionfirereplay :: ~treactionfirereplay ( )
{
   for ( int i = 0; i < num; i++ )
      delete replay[i];
}



                typedef struct tunitlist* punitlist;
                struct tunitlist {
                        pvehicle eht;
                        punitlist next;
                     };

                punitlist unitlist[8];


int tsearchreactionfireingunits :: maxshootdist[8] = { -1, -1, -1, -1, -1, -1, -1, -1 };

tsearchreactionfireingunits :: tsearchreactionfireingunits ( void )
{
   for ( int i = 0; i < 8; i++ )
      unitlist[i] = NULL;
}

void tsearchreactionfireingunits :: init ( pvehicle vehicle, IntFieldList* fieldlist )
{
   int x1 = maxint;
   int y1 = maxint;
   int x2 = 0;
   int y2 = 0;
   int i, j, h;

   if ( maxshootdist[0] == -1 ) {
      for (i = 0; i < 8; i++ )
         maxshootdist[i] = 0;

      for (i = 0; i < vehicletypenum; i++ ) {
         pvehicletype fzt = getvehicletype_forpos ( i );
         if ( fzt )
            for (j = 0; j < fzt->weapons->count; j++ )
               if ( fzt->weapons->weapon[j].shootable() )
                  for (h = 0; h < 8; h++ )
                     if ( fzt->weapons->weapon[j].targ & ( 1 << h ) )
                        if ( fzt->weapons->weapon[j].maxdistance > maxshootdist[h] )
                           maxshootdist[h] = fzt->weapons->weapon[j].maxdistance;
      }
   }

   for ( i = 0; i < fieldlist->getFieldNum(); i++) {
      int xt, yt;
      fieldlist->getFieldCoordinates ( i, &xt, &yt );
      if ( xt > x2 )
         x2 = xt ;
      if ( yt > y2 )
         y2 = yt ;

      if ( xt < x1 )
         x1 = xt ;
      if ( yt < y1 )
         y1 = yt ;
   }
   int height = log2 ( vehicle->height );

   x1 -= maxshootdist[height];
   y1 -= maxshootdist[height];
   x2 += maxshootdist[height];
   y2 += maxshootdist[height];

   if ( x1 < 0 )
      x1 = 0;
   if ( y1 < 0 )
      y1 = 0;
   if ( x2 >= actmap->xsize )
      x2 = actmap->xsize -1;
   if ( y2 >= actmap->ysize )
      y2 = actmap->ysize -1;

   for ( int y = y1; y <= y2; y++ )
      for ( int x = x1; x <= x2; x++ ) {
         pvehicle eht = getfield ( x, y )->vehicle;
         if ( eht )
            if ( eht->color != vehicle->color )
               if ( eht->reactionfire.status >= tvehicle::ReactionFire::ready )
                  if ( eht->reactionfire.enemiesAttackable & ( 1 << ( vehicle->color / 8 )))
                     if ( getdiplomaticstatus ( eht->color ) == cawar )
                        if ( attackpossible2u ( eht, vehicle ) )
                           addunit ( eht );

      }
   for ( i = 0; i < 8; i++ )
      if ( fieldvisiblenow ( getfield ( vehicle->xpos, vehicle->ypos ), i )) {
         punitlist ul  = unitlist[i];
         while ( ul ) {
            punitlist next = ul->next;
            pattackweap atw = attackpossible ( ul->eht, vehicle->xpos, vehicle->ypos );
            if ( atw->count )
               removeunit ( ul->eht );

            delete atw;
            ul = next;
         } /* endwhile */
      }

}

void  tsearchreactionfireingunits :: addunit ( pvehicle eht )
{
   int c = eht->color / 8;
   punitlist ul = new tunitlist;
   ul->eht = eht;
   ul->next= unitlist[c];
   unitlist[c] = ul;
}


void tsearchreactionfireingunits :: removeunit ( pvehicle vehicle )
{
   int c = vehicle->color / 8;
   punitlist ul = unitlist[c];
   punitlist last = NULL;
   while ( ul  &&  ul->eht != vehicle ) {
      last = ul;
      ul = ul->next;
  }
  if ( ul  &&  ul->eht == vehicle ) {
     if ( last )
        last->next = ul->next;
     else
        unitlist[c] = ul->next;

     delete ul;
  }
}


int  tsearchreactionfireingunits :: checkfield ( int x, int y, pvehicle &vehicle, MapDisplayInterface* md )
{

   int attacks = 0;
   int result = 0;

   pfield fld = getfield( x, y );
   npush ( fld->vehicle );
   fld->vehicle = vehicle;
   vehicle->xpos = x;
   vehicle->ypos = y;

   for ( int i = 0; i < 8; i++ ) {
      evaluatevisibilityfield ( actmap, fld, i, -1, actmap->getgameparameter ( cgp_initialMapVisibility ) );
      if ( fieldvisiblenow ( fld, i )) {
         punitlist ul  = unitlist[i];
         while ( ul  &&  !result ) {
            punitlist next = ul->next;
            pattackweap atw = attackpossible ( ul->eht, x, y );
            if ( atw->count && (ul->eht->reactionfire.enemiesAttackable & (1 << (vehicle->color / 8)))) {

               int ad1, ad2, dd1, dd2;

               int strength = 0;
               int num;
               for ( int j = 0; j < atw->count; j++ )
                  if ( atw->strength[j]  > strength ) {
                     strength = atw->strength[j];
                     num = j;
                  }

               if ( md ) {
                  displaymessage2 ( "attacking with weapon %d ", atw->num[num] );
                  cursor.setcolor ( 8 );

                  cursor.gotoxy ( ul->eht->xpos, ul->eht->ypos );
                  int t = ticker;
                  while ( t + 15 > ticker )
                     releasetimeslice();

                  cursor.gotoxy ( x, y );
                  t = ticker;
                  while ( t + 15 > ticker )
                     releasetimeslice();

                  cursor.setcolor ( 0 );
                  cursor.hide();
               }

               tunitattacksunit battle ( ul->eht, fld->vehicle, 0, atw->num[num] );

               ad1 = battle.av.damage;
               dd1 = battle.dv.damage;

               if ( md )
                  battle.calcdisplay ();
               else
                  battle.calc();

               ad2 = battle.av.damage;
               dd2 = battle.dv.damage;

               attacks++;

               if ( battle.dv.damage >= 100 )
                  result = 1;

               battle.setresult();

               logtoreplayinfo ( rpl_reactionfire, ul->eht->xpos, ul->eht->ypos, x, y, ad1, ad2, dd1, dd2, atw->num[num] );

               dashboard.x = 0xffff;
               ul->eht->reactionfire.enemiesAttackable &= 0xff ^ ( 1 <<  (vehicle->color / 8) );
               ul->eht->attacked = false;
               removeunit ( ul->eht );

            }
            delete atw;
            ul = next;
         } /* endwhile */
      }
   }
   npop ( fld->vehicle );

   if ( result )
      vehicle = NULL;

   return attacks;
}

tsearchreactionfireingunits :: ~tsearchreactionfireingunits()
{
   for ( int i = 0; i < 8; i++ ) {
      punitlist ul = unitlist[i];
      while ( ul ) {
         punitlist ul2 = ul->next;
         delete ul;
         ul = ul2;
      }
   }
}




int square ( int i )
{
   return i*i;
}

float square ( float i )
{
   return i*i;
}

void         calcmovemalus(int          x1,
                           int          y1,
                           int          x2,
                           int          y2,
                           pvehicle     vehicle,
                           int          direc,
                           int&         fuelcost,               // fuer Spritfuelconsumption
                           int&         movecost )             //  fuer movementdecrease
{
#ifdef HEXAGON
 static const  int         movemalus[2][6]  = {{ 8, 6, 3, 0, 3, 6 }, {0, 0, 0, 0, 0, 0 }};
#else
 static const  int         movemalus[2][8]  = {{2, 4, 2, 8, 0, 8, 2, 4}, {4, 2, 5, 2, 0, 2, 5, 2}};
#endif

  int          d;
  int           x, y;
  int         c;
  pfield        fld;
  pfield        fld2;
  int         mode;
  pattackweap  atw;


   if (direc == -1) {
      direc = getdirection ( x1, y1, x2, y2 );
   }
   else {
      if (x2 == -1 && y2 == -1) {
         x2 = x1;
         y2 = y1;
         getnextfield(x2,y2, direc);
      }
   }

  #ifndef HEXAGON
   mode = direc & 1;
  #else
   mode = 0;
  #endif


   if ( mode ) {
      fuelcost = minmalq;
      if (vehicle->height >= chtieffliegend)
         movecost = minmalq;
      else
         movecost = getfield(x2,y2)->getmovemalus( vehicle );
   } else {
      fuelcost = maxmalq;
      if (vehicle->height >= chtieffliegend)
         movecost = maxmalq;
      else
         movecost = getfield(x2,y2)->getmovemalus( vehicle ) * maxmalq / minmalq;
   }


   for (c = 0; c < sidenum; c++) {
      x = x2;
      y = y2;
      getnextfield( x,  y, c );
      fld = getfield ( x, y );
      if ( fld ) {
        d = (c - direc);

        if (d >= sidenum)
           d -= sidenum;

        if (d < 0)
           d += sidenum;

        fld = getfield(x,y);
        if ( fld->vehicle ) {
           if ( getdiplomaticstatus(fld->vehicle->color) == cawar ) {
              fld2 = getfield(x2,y2);

              npush( fld2->vehicle );
              npush( vehicle->xpos );
              npush( vehicle->ypos );
              vehicle->xpos = x2;
              vehicle->ypos = y2;
              fld2->vehicle = vehicle;
              atw = attackpossible(fld->vehicle,x2,y2);
              if (atw->count > 0)
                 movecost += movemalus[mode][d];
              npop( vehicle->ypos );
              npop( vehicle->xpos );
              npop( fld2->vehicle );

              delete atw ;
           }
        }
      }
   }

    /*******************************/
    /*    Wind calculation        ÿ */
    /*******************************/
   if (vehicle->height >= chtieffliegend && vehicle->height <= chhochfliegend && actmap->weather.wind[ getwindheightforunit ( vehicle ) ].speed  ) {
      movecost -=  windmovement[direc];
      fuelcost -=  windmovement[direc];
   }


}


void initwindmovement(  const pvehicle vehicle ) {
   int direc;
   twind wind = actmap->weather.wind[ getwindheightforunit ( vehicle ) ];
   for (direc = 0; direc < sidenum; direc++) {
     float relwindspeedx = 0, relwindspeedy = 0, abswindspeed;

     #ifdef HEXAGON
      abswindspeed = ( wind.speed * maxwindspeed * minmalq / 256 );
     #else
      if (direc & 1)
         abswindspeed = ( wind.speed * maxwindspeed * minmalq / 256);
      else
         abswindspeed = ( wind.speed * maxwindspeed * maxmalq / 256 );
     #endif

      float relwindspeed  =  abswindspeed / vehicle->typ->movement[log2(vehicle->height)];

      float pi = 3.14159265;

      relwindspeedx = 10 * relwindspeed * sin ( 2 * pi * wind.direction / sidenum );
      relwindspeedy = -10 * relwindspeed * cos ( 2 * pi * wind.direction / sidenum );



      float xtg = 0, ytg = 0;

      xtg = 120 * sin ( 2 * pi * direc / sidenum );
      ytg = -120 * cos ( 2 * pi * direc / sidenum );

      #ifndef HEXAGON
       if ( direc & 1 ) {
          xtg = xtg * minmalq / maxmalq;
          ytg = ytg * minmalq / maxmalq;
       }
      #endif

      int disttofly = (int)sqrt ( square ( xtg - relwindspeedx) + square ( ytg - relwindspeedy ) );

      #ifdef HEXAGON
       windmovement[direc] =  (120 - disttofly) / 10;
      #else
       if (direc & 1)
         windmovement[direc] =  (80 - disttofly) / 10;
       else
         windmovement[direc] =  (120 - disttofly) / 10;
      #endif

   }
}








void checkalliances_at_endofturn ( void )
{
   for ( int i = 0; i < 8; i++ ) {
      int act = actmap->actplayer;
      if ( i != act ) {

         if ( actmap->alliances[i] [act] == canewsetwar2 ) {
            int to = 0;
            for ( int j = 0; j < 8; j++ )
               if ( j != act )
                  to |= 1 << j;
                  // Message "sneak attack" an alle auáer dem Angreifer selbst.


            char txt[200];
            char* sp = getmessage( 10001 );
            sprintf ( txt, sp, actmap->player[act].getName().c_str(), actmap->player[i].getName().c_str() );
            sp = strdup ( txt );
            new tmessage ( sp, to );
         }

         if ( actmap->alliances[i] [act] == capeaceproposal  &&  actmap->alliances_at_beginofturn[i] != capeaceproposal ) {
            int to = 0;
            for ( int j = 0; j < 8; j++ )
               if ( j == i )
                  to |= 1 << j;

            char txt[200];
            char* sp = getmessage( 10003 );
            sprintf ( txt, sp, actmap->player[act].getName().c_str() );
            sp = strdup ( txt );
            new tmessage ( sp, to );
         }

         if ( actmap->alliances[i] [act] == cawarannounce ) {
            int to = 0;
            for ( int j = 0; j < 8; j++ )
               if ( j == i )
                  to |= 1 << j;

            char txt[200];
            char* sp = getmessage( 10002 );
            sprintf ( txt, sp, actmap->player[act].getName().c_str() );
            sp = strdup ( txt );
            new tmessage ( sp, to );
         }

      }
   }
}



void checkalliances_at_beginofturn ( void )
{
  int i;

   int act = actmap->actplayer ;
   for (i = 0; i < 8; i++ ) {
      if ( actmap->alliances[i][act] == cawarannounce || actmap->alliances[i][act] == canewsetwar2 ) {
         actmap->alliances[i][act] = cawar;
         actmap->alliances[act][i] = cawar;
         if ( actmap->shareview ) {
            actmap->shareview->mode[act][i] = sv_none;
            actmap->shareview->mode[i][act] = sv_none;
         }
      }
      if ( actmap->alliances[i][act] == capeaceproposal  &&  actmap->alliances[act][i] == capeaceproposal) {
         actmap->alliances[i][act] = capeace;
         actmap->alliances[act][i] = capeace;
      }
   }


   for ( i = 0; i < 8; i++ )
      actmap->alliances_at_beginofturn[i] = actmap->alliances[i][act];

}





void    tprotfzt::initbuffer( void )
{
   buf = new char[ vehicletypenum ];

   int i;
   for ( i=0; i < vehicletypenum ; i++ )
      buf[i] = actmap->player[ actmap->actplayer ].research.vehicletypeavailable ( getvehicletype_forpos ( i ) );
}



void    tprotfzt::evalbuffer( void )
{
   int i, num = 0;
   for ( i=0; i < vehicletypenum ;i++ ) {
      if (buf[i] == 0) {
          buf[i] = actmap->player[ actmap->actplayer ].research.vehicletypeavailable ( getvehicletype_forpos ( i ) );
          if ( buf[i] )
             num++;
      } else
          buf[i] = 0;

   }

   if ( num ) {
      tshownewtanks snt;
      snt.init ( buf );
      snt.run  ();
      snt.done ();
   }

   delete[] buf ;
}








void returnresourcenuseforpowerplant (  const pbuilding bld, int prod, Resources *usage, int percentagee_based_on_maxplus )
{
   Resources res;
   if ( percentagee_based_on_maxplus )
      res = bld->maxplus;
   else
      res = bld->plus;

   for ( int r = 0; r < 3; r++ )
      if ( res.resource(r) > 0 )
         usage->resource(r) = 0;
      else
         usage->resource(r) = -res.resource(r) * prod / 100;
}



void Building :: execnetcontrol ( void )
{
   if ( actmap->_resourcemode != 1 ) {
      for ( int i = 0; i < 3; i++ ) {
         if (  netcontrol & (cnet_moveenergyout << i )) {
            npush (  netcontrol );
            netcontrol |= (cnet_stopenergyinput << i );
            actstorage.resource(i) -= putResource ( actstorage.resource(i), i, 0 );
            npop (  netcontrol );
         } else
            if (  netcontrol & (cnet_storeenergy << i )) {
               npush (  netcontrol );
               netcontrol |= (cnet_stopenergyoutput << i );
               actstorage.resource(i) += getResource ( gettank(i) -  actstorage.resource(i), i, 0 );
               npop (  netcontrol );
            }
      }
   }
}

                    /*   modes: 0 = energy   ohne abbuchen
                                1 = material ohne abbuchen
                                2 = fuel     ohne abbuchen

                                  +4         mit abbuchen                         /
                                  +8         nur Tributzahlungen kassieren       /
                                 +16         plus zur?ckliefern                 <  diese Bits schlieáen sich gegenseitig aus
                                 +32         usage zur?ckliefern                 \
                                 +64         tank zur?ckliefern                   \
                                 */


int  Building :: putResource ( int      need,    int resourcetype, int queryonly, int scope  )
{
   PutResource putresource ( scope );
   return putresource.getresource ( xpos, ypos, resourcetype, need, queryonly, color/8, scope );
}


int  Building :: getResource ( int      need,    int resourcetype, int queryonly, int scope )
{
   GetResource gr ( scope );
   return gr.getresource ( xpos, ypos, resourcetype, need, queryonly, color/8, scope );
}



int  Building :: getresourceplus( int mode, Resources* gplus, int queryonly )
{
   int did_something = 0;

   if ( !queryonly && (mode & 1))
      mode -= 1;

   if ( actmap->_resourcemode != 1 ) {
      *gplus = Resources(0,0,0);

      if ( (typ->special & cgwindkraftwerkb ) && ( mode & 2 ) )
         for ( int r = 0; r < 3; r++ ) {
            int eplus =  maxplus.resource(r) * actmap->weather.wind[0].speed / 255;
            if ( ! (mode & 1 )) {
               int putable = putResource ( eplus + gplus->resource(r)*queryonly, r, 1 ) - gplus->resource(r)*queryonly;
               if ( putable < eplus )
                  eplus = putable;
            }
            gplus->resource(r) += eplus;
            if ( !queryonly && !work.wind_done ) {
               putResource ( eplus, r, 0 );
               work.wind_done = 1;
               did_something++;
            }
         }


      if ( (typ->special & cgsolarkraftwerkb) && ( mode & 4 ) ) {
         int sum = 0;
         int num = 0;
         for ( int x = 0; x < 4; x++ )
            for ( int y = 0; y < 6; y++)
               if ( getpicture ( x, y ) ) {
                  pfield fld = getField ( x, y );
                  int weather = 0;
                  while ( fld->typ != fld->typ->terraintype->weather[weather] )
                     weather++;

                  sum += csolarkraftwerkleistung[weather];
                  num ++;
               }

         Resources rplus;
         for ( int r = 0; r < 3; r++ ) {
            rplus.resource(r) = maxplus.resource(r) * sum / ( num * 1024 );
            if ( ! (mode & 1 )) {
               int putable = putResource ( rplus.resource(r) + gplus->resource(r)*queryonly, r, 1 ) - gplus->resource(r)*queryonly;
               if ( putable < rplus.resource(r) )
                  rplus.resource(r) = putable;
            }
            gplus->resource(r) += rplus.resource(r);
         }

         if ( !queryonly && !work.solar_done ) {
            for ( int s = 0; s < 3; s++ )
               putResource ( rplus.resource(s), s, 0 );
            work.solar_done = 1;
            did_something++;
         }
      }



      if ( (typ->special & cgconventionelpowerplantb) && ( mode & 8 ) ) {

         int perc = 100;
         Resources toproduce;
         if ( !queryonly )
            toproduce = work.resource_production.toproduce;
         else
            toproduce = plus;

         for ( int r = 0; r < 3; r++ )
            if ( plus.resource(r) > 0 ) {
               int p = plus.resource(r);
               if ( !(mode & 1))
                  p = putResource ( plus.resource(r) + p, 0, 1 );

               if ( perc > 100 * p / maxplus.resource(r) )
                  perc = 100 * p / maxplus.resource(r) ;
            }


         Resources usage;
         returnresourcenuseforpowerplant ( this, perc, &usage, 0 );
         int ena ;
         int maa ;
         int fua ;
         if ( !queryonly ) {
            ena = getResource ( usage.energy  , 0, 1 );
            maa = getResource ( usage.material, 1, 1 );
            fua = getResource ( usage.fuel    , 2, 1 );
         } else {
            ena = usage.energy;
            maa = usage.material;
            fua = usage.fuel;
         }


         // This calculation is done iteratively because the resourcenusage may be nonlinear
         if ( maa < usage.material ||  fua < usage.fuel  || ena < usage.energy  ) {
            int diff = perc / 2;
            while ( maa < usage.material ||  fua < usage.fuel  || ena < usage.energy  || diff > 1) {
               if ( maa < usage.material ||  fua < usage.fuel  || ena < usage.energy )
                  perc -= diff;
               else
                  perc += diff;

               if ( diff > 1 )
                  diff /=2;
               else
                  diff = 1;

               returnresourcenuseforpowerplant ( this, perc, &usage, 0 );
            }
         }

         if ( maa < usage.material ||  fua < usage.fuel  || ena < usage.energy  )
            displaymessage( "controls : int tbuilding :: getenergyplus( void ) : inconsistency in getting material or fuel for energyproduction", 2 );

         if ( !queryonly ) {
            work.resource_production.did_something_lastpass = 0;
            work.resource_production.finished = 1;
            for ( int r = 0; r < 3; r++ ) {
               getResource ( usage.resource(r)  , r, 0 );
               putResource ( toproduce.resource(r) * perc / 100, r, 0 );
               work.resource_production.toproduce.resource(r) -= toproduce.resource(r) * perc / 100;
               if ( toproduce.resource(r) * perc / 100  > 0 ) {
                  work.resource_production.did_something_lastpass = 1;
                  work.resource_production.did_something_atall = 1;
                  did_something++;
               }

               if ( work.resource_production.toproduce.resource(r) > 0 )
                  work.resource_production.finished = 0;
            }
         }


         for ( int s = 0; s < 3; s++ )
            if ( toproduce.resource(s) * perc / 100  > 0 )
               gplus->resource(s) += toproduce.resource(s) * perc / 100;
      }

      if ( (typ->special & cgminingstationb) && ( mode & 16 ) )
         if ( queryonly || work.mining.finished < 3 ) {
            int mp = 0;
            int fp = 0;
            if ( queryonly )
               initwork();
            mp = processmining ( 1, !queryonly );
            fp = processmining ( 2, !queryonly );
            gplus->material += mp;
            gplus->fuel     += fp;
            if ( mp || fp )
               did_something++;
         }


   } else {
      *gplus = bi_resourceplus;
      if ( !queryonly && !work.bimode_done ) {
         work.bimode_done = 1;
         for ( int r = 0; r < 3; r++ ) {
            putResource ( bi_resourceplus.resource(r), r, 0 );
            if ( bi_resourceplus.resource(r) > 0 )
               did_something++;
         }
      }
   }
   return did_something;
}



int  getminingstationeficency ( int dist )
{
  // f ( x ) = a / ( b * ( x + d ) ) - c

double a,b,c,d;

a          =     10426.400 ;
b          =     1.0710969 ;
c          =     568.88887 ;
d          =     6.1111109 ;

   double f = a / ( b * (dist + d)) - c;
  return (int)f;
}



tgetmininginfo :: tgetmininginfo ( pmap _gamemap ) : tsearchfields ( _gamemap )
{
   mininginfo = new tmininginfo;
   memset ( mininginfo, 0, sizeof ( *mininginfo ));
}

void tgetmininginfo :: testfield ( void )
{
  if ((xp >= 0) && (yp >= 0) && (xp < actmap->xsize) && (yp < actmap->ysize)) {
        pfield fld = getfield ( xp, yp );
        if ( mininginfo->efficiency[ dist ] == 0 )
           mininginfo->efficiency[ dist ] = getminingstationeficency ( dist );

        mininginfo->avail[dist].material += fld->material * resource_material_factor;
        mininginfo->avail[dist].fuel     += fld->fuel     * resource_fuel_factor;
        mininginfo->max[dist].material   += 255 * resource_material_factor;
        mininginfo->max[dist].fuel       += 255 * resource_fuel_factor;
  }
}


void tgetmininginfo :: run (  const pbuilding bld )
{
   initsearch ( bld->xpos, bld->ypos, maxminingrange, 0 );
   xp = bld->xpos;
   yp = bld->ypos;
   dist=0;
   testfield();
   startsearch();
}

class tprocessminingfields : public tsearchfields {
             int maxfuel;
             int maxmaterial;
             int materialtoget;
             int fueltoget;
             int worktodo;
             int materialgot;
             int fuelgot;
             int range;
             int abbuchen;
             int color;

          public:
             void testfield ( void );
             int  setup ( pbuilding bld, int& mm, int cm, int& mf, int cf, int abbuch, int resource );  // mm: maxmaterial, cm: capacity material
             tprocessminingfields ( pmap _gamemap ) : tsearchfields ( _gamemap ) {};
          };

void tprocessminingfields :: testfield ( void )
{
  if ((xp >= 0) && (yp >= 0) && (xp < actmap->xsize) && (yp < actmap->ysize)) {
     if ( worktodo == 0    ||  ( materialgot >= materialtoget && fuelgot >= fueltoget ) )
        abbruch = true;
     else {
        pfield fld = getfield ( xp, yp );
        int efz = getminingstationeficency ( dist /* beeline ( startx, starty, xp, yp ) */ );


        if ( materialtoget > materialgot ) {
           range = dist;
           int matavail = fld->material * resource_material_factor;
           int wtg = (materialtoget - materialgot) * 1024 / efz;
           if ( wtg > worktodo )
              wtg = worktodo;
           int mtg = (wtg * efz + 1023 ) / 1024;
           if ( mtg > matavail )
              mtg = matavail;

           materialgot += mtg;
           worktodo -= (mtg * 1024 + efz-1) / efz;
           matavail -= mtg;
           if ( matavail < 0 )
              matavail = 0;
           if ( worktodo < 0 )
              worktodo = 0;
           if( materialgot > materialtoget )
              materialgot = materialtoget;

           if ( abbuchen )
              fld->material =  matavail / resource_material_factor;

        }

        if ( fueltoget > fuelgot ) {
           range = dist;
           int fuelavail = fld->fuel * resource_fuel_factor;
           int wtg = (fueltoget - fuelgot) * 1024 / efz;
           if ( wtg > worktodo )
              wtg = worktodo;
           int ftg = (wtg * efz + 1023 ) / 1024;
           if ( ftg > fuelavail )
              ftg = fuelavail;

           fuelgot += ftg;
           worktodo -= (ftg * 1024 + efz - 1) / efz;
           fuelavail -= ftg;
           if ( fuelavail < 0 )
              fuelavail = 0;
           if ( worktodo < 0 )
              worktodo = 0;
           if( fuelgot > fueltoget )
              fuelgot = fueltoget;

           if ( abbuchen )
              fld->fuel =  fuelavail / resource_fuel_factor;
        }

        if ( abbuchen ) {
           if ( !fld->resourceview )
              fld->resourceview = new tresourceview;
           fld->resourceview->visible |= 1 << color;
           fld->resourceview->fuelvisible[color] = fld->fuel;
           fld->resourceview->materialvisible[color] = fld->material;
        }


     }
  }
}


int   tprocessminingfields :: setup ( pbuilding bld, int& mm, int cm, int& mf, int cf, int abbuch, int res )  // mm: maxmaterial, cm: capacity material
{
   if ( (bld->work.mining.finished & res) == res ) {
      mm =0;
      mf =0;
      return 0;
   }
   // int oldmm = mm;
   // int oldmf = mf;

   color = bld->color/8;
   abbuchen = abbuch;
   range = 0;

   maxmaterial = cm;
   maxfuel = cf;

   if ( bld->typ->efficiencymaterial )
      materialtoget = maxmaterial * 1024 / bld->typ->efficiencymaterial ;
   else
      materialtoget = 0;

   if ( bld->typ->efficiencyfuel )
      fueltoget     = maxfuel     * 1024 / bld->typ->efficiencyfuel ;
   else
      fueltoget = 0;

   worktodo = 0;
   if ( bld->typ->efficiencymaterial )
      worktodo += mm * 1024 / bld->typ->efficiencymaterial;
   if ( bld->typ->efficiencyfuel )
      worktodo += mf * 1024 / bld->typ->efficiencyfuel;


   if ( !abbuchen ) {
      for ( int r = 0; r < 3; r++ )
         if ( bld->plus.resource(r) < 0 )
            bld->work.mining.touse.resource(r) = -bld->plus.resource(r);
         else
            bld->work.mining.touse.resource(r) = 0;
   }

   int perc = 1000;
   int resourcesrequired = 0;
   if ( abbuch )
      for ( int r = 0; r < 3; r++ )
         if ( bld->work.mining.touse.resource(r) > 0 ) {
            resourcesrequired = 1;
            int g = bld->getResource ( bld->work.mining.touse.resource(r), r, 1 );
            if ( g * 1000 / (-bld->plus.resource(r)) < perc )
               perc = g * 1000 / (-bld->plus.resource(r));
         }

   if ( abbuch )
      if ( perc ) {
          bld->work.mining.did_something_atall = 1;
          bld->work.mining.did_something_lastpass = 1;
      } else
          bld->work.mining.did_something_lastpass = 0;


   int orgworktodo = worktodo;
   worktodo = worktodo * perc/1000;
   int workbeforestart = worktodo;

   materialgot = 0;
   fuelgot = 0;

   initsearch( bld->xpos, bld->ypos, maxminingrange, 0 );
   xp = bld->xpos;
   yp = bld->ypos;
   dist = 0;
   testfield();
   startsearch();

   mm = materialgot * bld->typ->efficiencymaterial / 1024;
   mf = fuelgot     * bld->typ->efficiencyfuel     / 1024;

   if ( abbuch ) {
      int perc = 1000 * ( workbeforestart - worktodo ) / orgworktodo;
      for ( int r = 0; r < 3; r++ )
         if ( bld->plus.resource(r) < 0 ) {
            int g = bld->getResource ( -bld->plus.resource(r) * perc / 1000, r, 0 );
            bld->work.mining.touse.resource(r) -= g;
         }
      if ( !resourcesrequired ) 
            bld->work.mining.finished+= res;
      

   }
   return range;
}

int  Building :: processmining ( int res, int abbuchen )
{

   int maxfuel = plus.fuel;
   int maxmaterial = plus.material;
  
   int capfuel = 0;
   int capmaterial = 0;

   if ( abbuchen ) {
      if ( res == 1 ) 
         capmaterial = putResource ( plus.material, 1, 1 );
      else 
         capfuel = putResource ( plus.fuel, 2, 1 );
   } else {
      capmaterial = plus.material;
      capfuel = plus.fuel;
   }

   tprocessminingfields pmf ( actmap );


   lastmineddist = pmf.setup ( this, maxmaterial, capmaterial, maxfuel, capfuel, abbuchen, res );

   if ( abbuchen ) {
      putResource ( maxmaterial, 1 , 0 );
      putResource ( maxfuel,     2 , 0 );
   }
   if ( res==1 )
      return maxmaterial;
   else
      return maxfuel;

}

void Building :: getresourceusage ( Resources* usage )
{
   returnresourcenuseforpowerplant ( this, 100, usage, 0 );
   if ( typ->special & cgresearchb ) {
      int material;
      int energy;
      returnresourcenuseforresearch ( this, researchpoints, &energy, &material );
      usage->material += material;
      usage->energy   += energy;
      usage->fuel  = 0;
   }
}



void doresearch ( int i )
{
   // typedef struct tresbuild* presbuild;

   struct  tresbuild {
               int        eff;
               pbuilding  bld;
               tresbuild  *next;
           };


   tresbuild* first = NULL;

   pbuilding bld = actmap->player[i].firstbuilding;
   while ( bld ) {
      if ( bld->typ->special & cgresearchb ) {
         int energy, material;
         returnresourcenuseforresearch ( bld, bld->researchpoints, &energy, &material );

         tresbuild* a = new tresbuild;
         if ( energy )
            a->eff = 16384 * bld->researchpoints / energy ;
         else
            a->eff = maxint;

         a->bld = bld;

         tresbuild* b  = first;
         tresbuild* bp = NULL;
         while ( b && ( b->eff > a->eff ) ) {
            bp = b;
            b = b->next;
         }

         a->next = b;
         if ( bp )
            bp->next = a;
         else
            first = a;

      }
      bld = bld->next;
   }

   tresbuild*  a = first;
   while ( a ) {
      bld = a->bld;
      int energy, material;
      returnresourcenuseforresearch ( bld, bld->researchpoints, &energy, &material );
      int ena = bld->getResource ( energy,   0, 1 );
      int maa = bld->getResource ( material, 1, 1 );

      int res = bld->researchpoints;
      if ( ena < energy  ||  maa < material ) {
         int diff = bld->researchpoints / 2;
         while ( ena < energy  ||  maa < material  || diff > 1) {
            if ( ena < energy  ||  maa < material ) 
               res -= diff;
            else
               res += diff;

            if ( diff > 1 )
               diff /=2;
            else
               diff = 1;

            returnresourcenuseforresearch ( bld, res, &energy, &material );
         }

         returnresourcenuseforresearch ( bld, res+1, &energy, &material );

         if ( ena >= energy  &&  maa >= material )
            res++;
         else
            returnresourcenuseforresearch ( bld, res, &energy, &material );
         
      }

      ena = bld->getResource ( energy,   0, 0 );
      maa = bld->getResource ( material, 1, 0 );

      if ( ena < energy || maa < material )
         displaymessage( "controls : doresearch : inconsistency in getting energy or material for building", 2 );

      actmap->player[i].research.progress += res;


      a = a->next;

   }

   tresbuild*  b;
   a = first;
   while ( a ) {
      b = a;
      a = a->next;
      delete b;
   }


}

int Building :: getmininginfo ( int res )
{
   return processmining ( res, 0 );
}


void Building :: initwork ( void )
{
   repairedThisTurn = 0;

   lastmaterialavail = -1;
   lastfuelavail = -1;
   lastenergyavail = -1;

   Resources nul;
   nul.energy = 0; nul.material = 0; nul.fuel = 0;

   work.mining.finished        = 3;
   work.mining.did_something_atall      = 0;

   work.mining.did_something_lastpass   = 0;
   work.mining.touse = nul;
   work.resource_production.finished        = 1;
   work.resource_production.did_something_atall   = 0;
   work.resource_production.did_something_lastpass   = 0;
   work.resource_production.toproduce = nul;
  /*
   work.research.finished        = 1;
   work.research.did_something   = 0;
   work.research.toresearch = 0;
  */
   work.wind_done = 1;
   work.solar_done = 1;
   work.bimode_done = 1;


   if ( completion == typ->construction_steps - 1 ) {
      if ( actmap->_resourcemode == 0 ) {
         if ( typ->special & cgwindkraftwerkb ) 
            work.wind_done = 0;

         if ( typ->special & cgsolarkraftwerkb ) 
            work.solar_done = 0;

         if ( typ->special & cgminingstationb ) {
            for ( int r = 0; r < 3; r++ )
               if ( plus.resource(r) < 0 )
                  work.mining.touse.resource(r) = -plus.resource(r);
               else
                  work.mining.touse.resource(r) = 0;

            work.mining.finished = 0;
         }

         if ( typ->special & cgconventionelpowerplantb ) {
            for ( int r = 0; r < 3; r++ )
               if ( plus.resource(r) > 0 )
                  work.resource_production.toproduce.resource(r) = plus.resource(r);
               else
                  work.resource_production.toproduce.resource(r) = 0;

            work.resource_production.finished = 0;
         }

      } else 
         work.bimode_done = 0;


   }
}

int Building :: worktodo ( void )
{
   if ( actmap->_resourcemode == 1 ) {
      return !work.bimode_done;
   }
   if ( !work.wind_done  || !work.solar_done )
      return 1;

   if ( work.mining.finished < 3 )
      return 1;

   if ( !work.resource_production.finished  )
      return 1;

   return 0;
}




int  Building :: processwork ( void )
{
   if ( (completion == typ->construction_steps - 1) ) {

      if ( actmap->_resourcemode == 1 ) {
         Resources plus;

         int res = getresourceplus  ( -2, &plus, 0 );

         execnetcontrol ();

         return res;
      } else {

         Resources plus;
         int res = getresourceplus  ( -2, &plus, 0 );

         execnetcontrol ();

         for ( int r = 0; r < 3; r++ )
            if (  actstorage.resource(r) >  gettank(r) ) {
               putResource ( gettank(r) -  actstorage.resource(r), r , 0 );
               actstorage.resource(r) =  gettank(r);
            }

         return res;
      }
 
   }
   return 0;
}      



void turnwrap ( void )
{

    actmap->actplayer = 0; 
    actmap->time.a.turn++;
    clearfahrspuren(); 

    for (int i = 0; i <= 7; i++) 
       if (actmap->player[i].existent) { 
   
          pvehicle eht = actmap->player[i].firstvehicle;
          while ( eht ) {
             eht->turnwrap();
             eht = eht->next;
          }


          {
             pbuilding actbuilding = actmap->player[i].firstbuilding; 
             while (actbuilding )  {
                actbuilding->initwork ();
                actbuilding = actbuilding->next;
             }
          }

          int pass = 0;
          int buildingwaiting = 0;
          int buildingnum;  
          do {
             pass++;
             buildingwaiting = 0;
             buildingnum = 0;

             pbuilding actbuilding = actmap->player[i].firstbuilding; 
             while ( actbuilding ) { 

                if ( actbuilding->worktodo() ) 
                   buildingwaiting += actbuilding->processwork();

                

                actbuilding = actbuilding->next; 
                buildingnum++;
             } /* end while */
   
          } while ( buildingwaiting && pass < 2*buildingnum ); /* enddo */

          doresearch( i );

       } 

}


void initchoosentechnology( void )
{
   actmap->player[actmap->actplayer].research.progress = 0;
 
   pdissectedunit du = actmap->player[ actmap->actplayer ].dissectedunit;
   pdissectedunit last = NULL;
   while ( du ) {
 
      if ( du->tech == actmap->player[actmap->actplayer].research.activetechnology ) {
         actmap->player[actmap->actplayer].research.progress += du->points;
 
         du = du->next;
         if ( last ) {
            delete last->next;
            last->next = du;
         } else {
            delete actmap->player[ actmap->actplayer ].dissectedunit;
            actmap->player[ actmap->actplayer ].dissectedunit = du;
         }
 
      } else
        du = du->next;
 
   }
}


void newTurnForHumanPlayer ( int forcepasswordchecking = 0 )
{
   checkalliances_at_beginofturn ();

   if ( actmap->player[actmap->actplayer].stat == ps_human ) {

      int humannum = 0;
      for ( int i = 0; i < 8; i++ )
         if ( actmap->player[i].existent )
            if ( actmap->player[i].stat == ps_human )
               humannum++;

      if ( humannum > 1  ||  forcepasswordchecking > 0 ) {
         tlockdispspfld ldsf;
         backgroundpict.paint();

         bool firstRound = actmap->time.a.turn == 1;
         if ( (!actmap->player[actmap->actplayer].passwordcrc.empty() && actmap->player[actmap->actplayer].passwordcrc != CGameOptions::Instance()->getDefaultPassword() ) // && actmap->player[actmap->actplayer].passwordcrc != encodepassword ( password )
            || firstRound || (actmap->network && actmap->network->globalparams.reaskpasswords) ) {
               if ( forcepasswordchecking < 0 ) {
                  delete actmap;
                  actmap = NULL;
                  throw NoMapLoaded();
               } else {
                  bool stat;
                  do {
                     stat = enterpassword ( actmap->player[actmap->actplayer].passwordcrc, firstRound );
                  } while ( actmap->player[actmap->actplayer].passwordcrc.empty() && stat && viewtextquery ( 910, "warning", "~e~nter password", "~c~ontinue without password" ) == 0 ); /* enddo */
               }
         } else
            displaymessage("next player is:\n%s",3,actmap->player[actmap->actplayer].getName().c_str() );
      }

      checkforreplay();

      if (actmap->player[actmap->actplayer].research.activetechnology == NULL)
         if ( actmap->player[actmap->actplayer].research.progress ) {
            int mx  = actmap->player[actmap->actplayer].research.progress;
            choosetechnology();
            initchoosentechnology();
            actmap->player[actmap->actplayer].research.progress += mx;
         }
      if (actmap->player[actmap->actplayer].research.activetechnology != NULL) {
         while ((actmap->player[actmap->actplayer].research.activetechnology != NULL) &&
                (actmap->player[actmap->actplayer].research.progress >= actmap->player[actmap->actplayer].research.activetechnology->researchpoints))
                {
                 int mx = actmap->player[actmap->actplayer].research.progress -
                      actmap->player[actmap->actplayer].research.activetechnology->researchpoints;
                 showtechnology(actmap->player[actmap->actplayer].research.activetechnology);

                 tprotfzt   pfzt;
                 pfzt.initbuffer ();

                 actmap->player[actmap->actplayer].research.addtechnology();

                 pfzt.evalbuffer ();


                 choosetechnology();
                 initchoosentechnology();

                 actmap->player[actmap->actplayer].research.progress += mx;
         }
      }

      if ( actmap->lastjournalchange.abstime )
         if ( (actmap->lastjournalchange.a.turn == actmap->time.a.turn ) ||
              (actmap->lastjournalchange.a.turn == actmap->time.a.turn-1  &&  actmap->lastjournalchange.a.move > actmap->actplayer ) )
                 viewjournal();

      dashboard.x = 0xffff;

      moveparams.movestatus = 0;


   }
   computeview( actmap );

   actmap->playerView = actmap->actplayer;

   if ( startreplaylate ) {
      actmap->replayinfo = new treplayinfo;
      startreplaylate = 0;
   }

   if ( actmap->replayinfo && actmap->player[ actmap->actplayer ].stat != ps_off ) {
      if ( actmap->replayinfo->actmemstream )
         displaymessage2( "actmemstream already open at begin of turn ",2 );
      // displaymessage("saving replay information",0 );

      if ( actmap->replayinfo->guidata[actmap->actplayer] ) {
         delete actmap->replayinfo->guidata[actmap->actplayer];
         actmap->replayinfo->guidata[actmap->actplayer] = NULL;
      }

      savereplay ( actmap->actplayer );

      actmap->replayinfo->guidata[actmap->actplayer] = new tmemorystreambuf;
      actmap->replayinfo->actmemstream = new tmemorystream ( actmap->replayinfo->guidata[actmap->actplayer], 2 );

      // removemessage ();

   }


   actmap->xpos = actmap->cursorpos.position[ actmap->actplayer ].sx;
   actmap->ypos = actmap->cursorpos.position[ actmap->actplayer ].sy;

   cursor.gotoxy ( actmap->cursorpos.position[ actmap->actplayer ].cx, actmap->cursorpos.position[ actmap->actplayer ].cy , 0);


   dashboard.x = 0xffff;
   transfer_all_outstanding_tribute();
}





void sendnetworkgametonextplayer ( int oldplayer, int newplayer )
{
/*
   int num = 0;
   int pl[8];

   for (int i = 0; i < 8; i++) {
      if ( actmap->player[i].existent )
         if ( actmap->player[i].firstvehicle || actmap->player[i].firstbuilding ) {
            pl[num] = i;
            num++;
          }
   }  endfor */


   tnetworkcomputer* compi = &actmap->network->computer[ actmap->network->player[ oldplayer ].compposition ];
   while ( compi->send.transfermethod == NULL  ||  compi->send.transfermethodid == 0 )
        setupnetwork( actmap->network, 2, oldplayer );

   displaymessage ( " starting data transfer ",0);

   try {
      compi->send.transfermethod->initconnection ( TN_SEND );
      compi->send.transfermethod->inittransfer ( &compi->send.data );

      tnetworkloaders nwl;
      nwl.savenwgame ( compi->send.transfermethod->stream );

      compi->send.transfermethod->closetransfer();
      compi->send.transfermethod->closeconnection();
   } /* endtry */
   catch ( tfileerror ) {
      displaymessage ( "error saving file", 1 );
   } /* endcatch */

   delete actmap;
   actmap = NULL;
   displaymessage( " data transfer finished",1);

   throw NoMapLoaded ();

}



void endTurn ( void )
{
   if ( actmap->replayinfo )
      if ( actmap->replayinfo->actmemstream ) {
         delete actmap->replayinfo->actmemstream;
         actmap->replayinfo->actmemstream = NULL;
      }

 /*
   if ( actmap->objectcrc )
      if ( actmap->objectcrc->speedcrccheck->getstatus ( )  ) {
         erasemap();
         throw NoMapLoaded();
      }
*/
     /* *********************  vehicle ********************  */

   mousevisible(false);
   if ( actmap->actplayer >= 0 ) {
      actmap->cursorpos.position[actmap->actplayer].cx = getxpos();
      actmap->cursorpos.position[actmap->actplayer].cy = getypos();
      actmap->cursorpos.position[actmap->actplayer].sx = actmap->xpos;
      actmap->cursorpos.position[actmap->actplayer].sy = actmap->ypos;

      pvehicle actvehicle = actmap->player[actmap->actplayer].firstvehicle;
      while ( actvehicle ) {
         pvehicle nxeht = actvehicle->next;


         /* fuel and movement */

         int j = 1;


         // Bei Žnderungen hier auch die Windanzeige dashboard.PAINTWIND aktualisieren !!!

         if (( actvehicle->height >= chtieffliegend )   &&  ( actvehicle->height <= chhochfliegend ) && ( getfield(actvehicle->xpos,actvehicle->ypos)->vehicle == actvehicle)) {
            if ( getmaxwindspeedforunit ( actvehicle ) < actmap->weather.wind[ getwindheightforunit ( actvehicle ) ].speed*maxwindspeed ){
               removevehicle ( &actvehicle );
               j = -1;
            } else {

               j = actvehicle->tank.fuel - actvehicle->typ->fuelConsumption * nowindplanefuelusage;

               if ( actvehicle->height <= chhochfliegend )
                  j -= ( actvehicle->getMovement() * 64 / actvehicle->typ->movement[log2(actvehicle->height)] ) 
                       * (actmap->weather.wind[ getwindheightforunit ( actvehicle ) ].speed * maxwindspeed / 256 ) * actvehicle->typ->fuelConsumption / ( minmalq * 64 );

              //          movement * 64        windspeed * maxwindspeed         fuelConsumption
              // j -=   ----------------- *  ----------------------------- *   -----------
              //          typ->movement                 256                       64 * 8
              //
              //

              //gek?rzt:
              //
              //             movement            windspeed * maxwindspeed
              // j -= --------------------- *  ----------------------------   * fuelConsumption
              //           typ->movement             256   *      8
              //
              //
              //
              // Falls eine vehicle sich nicht bewegt hat, bekommt sie soviel Sprit abgezogen, wie sie zum zur?cklegen der Strecke,
              // die der Wind pro Runde zur?ckgelegt hat, fuelConsumptionen w?rde.
              // Wenn die vehicle sich schon bewegt hat, dann wurde dieser Abzug schon beim movement vorgenommen, so daá er hier nur
              // noch fuer das ?briggebliebene movement stattfinden muá.
              //


               if (j < 0)
                  removevehicle(&actvehicle);
               else
                  actvehicle->tank.fuel = j;
            }
         }
         if (j >= 0)  {
               if ( actvehicle->reactionfire.getStatus()) {
                  if ( actvehicle->reactionfire.getStatus()< 3 )
                     actvehicle->reactionfire.status++;

                  if ( actvehicle->reactionfire.getStatus() == 3 )  {
                     //actvehicle->reactionfire = 0xff;
                     actvehicle->attacked = false;
                  } else {
                     //actvehicle->reactionfire = 0;
                     actvehicle->attacked = true;
                  }

                  // actvehicle->setMovement ( 0 );
                  actvehicle->resetMovement();
                  actvehicle->attacked = false;
               } else {
                  actvehicle->resetMovement();
                  actvehicle->attacked = false;
               }
            }
                     if ( actvehicle )
            actvehicle->endTurn();

         actvehicle = nxeht;
      }

      checkunitsforremoval ();
   }

     /* *********************  allianzen ********************  */

  checkalliances_at_endofturn ();

     /* *********************  messages ********************  */


  while ( actmap->unsentmessage ) {
     pmessagelist list = actmap->unsentmessage;

     pmessagelist nw = new tmessagelist ( &actmap->player[ actmap->actplayer ].sentmessage );
     nw->message = list->message;
     for ( int i = 0; i < 8; i++ )
        if ( nw->message->to & ( 1 << i )) {
           pmessagelist dst = new tmessagelist ( &actmap->player[ i ].unreadmessage );
           dst->message = nw->message;
        }

     actmap->unsentmessage = list->next;
     delete list;
  }

  if ( actmap->newjournal ) {
     int s = 0;
     if ( actmap->journal )
        s = strlen ( actmap->journal );

     char* n = new char[ strlen ( actmap->newjournal ) + s + 200 ];

     char tempstring[100];
     char tempstring2[50];
     sprintf( tempstring, "#color0# %s ; turn %d #color0##crt##crt#", actmap->player[actmap->actplayer].getName().c_str(), actmap->time.a.turn );
     sprintf( tempstring2, "#color%d#", getplayercolor ( actmap->actplayer ));

     if ( actmap->journal ) {
        strcpy ( n, actmap->journal );
        int fnd;
        int num = 0;
        do {
           fnd = 0;
           if ( n[s-1] == '\n' ) {
              fnd++;
              num++;
              s-=1;
           } else
             if ( s > 4 )
                if ( strnicmp ( &n[s-5], "#crt#", 5 ) == 0 ) {
                   fnd++;
                   num++;
                   s-=5;
                }

        } while ( fnd ); /* enddo */

        strcat ( n, tempstring2 );
        strcat ( n, actmap->newjournal );
        strcat ( n, tempstring );

        delete[] actmap->journal;
        actmap->journal = n;

     } else {
        strcpy ( n, tempstring2 );
        strcat ( n, actmap->newjournal );
        strcat ( n, tempstring );

        actmap->journal = n;
     }

     delete[] actmap->newjournal;
     actmap->newjournal = NULL;

     actmap->lastjournalchange.a.turn = actmap->time.a.turn;
     actmap->lastjournalchange.a.move = actmap->actplayer;

  }
}


void nextPlayer( void )
{
   int oldplayer = actmap->actplayer;
   if ( oldplayer >= 0)
      if ( !actmap->player[oldplayer].firstvehicle && !actmap->player[oldplayer].firstbuilding )
         actmap->player[oldplayer].existent = 0;

   int runde = 0;
   do {
      actmap->actplayer++;
      actmap->time.a.move = 0;
      if (actmap->actplayer > 7) {
         turnwrap();
         runde++;
      }
   }  while (!(actmap->player[actmap->actplayer].firstvehicle  || actmap->player[actmap->actplayer].firstbuilding  || (runde > 2)));

   if (runde > 2) {
      displaymessage("There are no players left any more !",2);
      delete actmap;
      actmap = NULL;
      throw NoMapLoaded ();
   }

   int newplayer = actmap->actplayer;
   actmap->playerView = actmap->actplayer;

   if ( actmap->network &&  oldplayer != actmap->actplayer && actmap->network->player[ newplayer ].compposition != actmap->network->player[ oldplayer ].compposition )
      sendnetworkgametonextplayer ( oldplayer, newplayer );
   else {
/*
      tlockdispspfld ldsf;

      int forcepwd;  // Wenn der aktuelle player gerade verloren hat, muá fuer den n„chsten player die Passwortabfrage kommen, auch wenn er nur noch der einzige player ist !
      if ( oldplayer >= 0  &&  !actmap->player[oldplayer].existent )
         forcepwd = 1;
      else
         forcepwd = 0;

      newturnforplayer( forcepwd );
*/
   }
}


void runai( int playerView )
{
   if ( CGameOptions::Instance()->runAI ) {
      actmap->playerView = playerView;

      if ( !actmap->player[ actmap->actplayer ].ai )
         actmap->player[ actmap->actplayer ].ai = new AI ( actmap, actmap->actplayer );

      actmap->player[ actmap->actplayer ].ai->run();

   } else {
      tlockdispspfld displock;
      checkalliances_at_beginofturn ();
      computeview( actmap );
      displaymessage("no AI available yet", 1 );
   }
}

void next_turn ( int playerView )
{
   int startTurn = actmap->time.a.turn;

   int pv;
   if ( playerView == -2 ) {
      if ( actmap->time.a.turn == 0 )  // the game has just been started
         pv = -1;
      else
         if ( actmap->player[actmap->actplayer].stat != ps_human )
            pv = -1;
         else
            pv = actmap->actplayer;
   } else
      pv = playerView;


   int bb = cursor.an;
   if (bb)
      cursor.hide();

   do {
     endTurn();
     nextPlayer();
     if ( actmap->player[actmap->actplayer].stat == ps_computer )
        runai( pv );

     if ( actmap->time.a.turn >= startTurn+2 ) {
        displaymessage("no human players found !", 1 );
        delete actmap;
        actmap = NULL;
        throw NoMapLoaded();
     }

   } while ( actmap->player[actmap->actplayer].stat != ps_human ); /* enddo */

   newTurnForHumanPlayer();

   if (bb)
     cursor.display();
}

void initNetworkGame ( void )
{
   while ( actmap->player[actmap->actplayer].stat != ps_human ) {

     if ( actmap->player[actmap->actplayer].stat == ps_computer )
        runai(-1);
     endTurn();
     nextPlayer();
   }

   newTurnForHumanPlayer( 0 );
}


void continuenetworkgame ( void )
{
   tlockdispspfld ldsf;

   tnetwork network;
/*
   int stat;
   int go = 0;
   do {
      stat = setupnetwork( &network, 1+8 );
      if ( stat == 1 ) {
         return;
      }
      if ( network.computer[0].receive.transfermethod == 0 )
         displaymessage("please choose a transfer method !",1 );
      else
         if ( network.computer[0].receive.transfermethodid != network.computer[0].receive.transfermethod->getid() )
            displaymessage("please setup transfer method !", 1 );
         else
            if ( !network.computer[0].receive.transfermethod->validateparams( &network.computer[0].receive.data, TN_RECEIVE ))
               displaymessage("please setup transfer method !", 1 );
            else
               go = 1;
   } while ( !go );

*/
   int stat;
   int go = 0;
   do {
      stat = network.computer[0].receive.transfermethod->setupforreceiving ( &network.computer[0].receive.data );
      if ( stat == 0 )
         return;

      if ( network.computer[0].receive.transfermethod  &&
           network.computer[0].receive.transfermethodid == network.computer[0].receive.transfermethod->getid()  &&
           network.computer[0].receive.transfermethod->validateparams( &network.computer[0].receive.data, TN_RECEIVE ))
           go = 1;
   } while ( !go );


   try {
       displaymessage ( " starting data transfer ",0);

       network.computer[0].receive.transfermethod->initconnection ( TN_RECEIVE );
       network.computer[0].receive.transfermethod->inittransfer ( &network.computer[0].receive.data );

       tnetworkloaders nwl;
       nwl.loadnwgame ( network.computer[0].receive.transfermethod->stream );

       network.computer[0].receive.transfermethod->closetransfer();

       network.computer[0].receive.transfermethod->closeconnection();

       removemessage();
       if ( actmap->network )
          setallnetworkpointers ( actmap->network );
   } /* endtry */

   catch ( InvalidID err ) {
      displaymessage( err.getMessage().c_str(), 1 );
      throw NoMapLoaded();
   } /* endcatch */
   catch ( tinvalidversion err ) {
      displaymessage( "File %s has invalid version.\nExpected version %d\nFound version %d\n", 1, err.filename, err.expected, err.found );
      throw NoMapLoaded();
   } /* endcatch */
   catch ( tcompressionerror err ) {
      displaymessage( "The file cannot be decompressed. \nIt has probably been damaged during transmission from the previous player to you.\nTry sending it zip compressed or otherwise encapsulated.\n", 1 );
      throw NoMapLoaded();
   } /* endcatch */
   catch ( tfileerror err) {
      displaymessage( "error reading game %s ", 1, err.filename );
      throw NoMapLoaded();
   } /* endcatch */
   catch ( ASCexception ) {
      displaymessage( "error loading game", 1 );
      throw NoMapLoaded();
   } /* endcatch */
   if ( !actmap || actmap->xsize <= 0 || actmap->ysize <= 0 )
      throw NoMapLoaded();

   initNetworkGame( );
}





void returnresourcenuseforresearch ( const pbuilding bld, int research, int* energy, int* material )
{
   /*
   double esteigung = 55;
   double msteigung = 40;
   */

   double res = research;
   double deg = res / bld->typ->maxresearchpoints;

   double m = 1 / log ( minresearchcost + maxresearchcost );

   *energy   = (int)(researchenergycost   * research * ( exp ( deg / m ) - ( 1 - minresearchcost ) ) / 1000 * (researchcostdouble+res)/researchcostdouble);
   *material = (int)(researchmaterialcost * research * ( exp ( deg / m ) - ( 1 - minresearchcost ) ) / 1000 * (researchcostdouble+res)/researchcostdouble);
/*
   if ( bld->typ->maxresearchpoints > 0 ) {
      *material = researchmaterialcost * research * 
      ( exp ( res / msteigung ) - 1 ) / ( exp ( (double)bld->typ->maxresearchpoints / (msteigung*2) ) - 1 ) * (10000+res)/10000 / 1000;
      *energy   = researchenergycost   * ( exp ( res / esteigung ) - 1 ) / ( exp ( (double)bld->typ->maxresearchpoints / (esteigung*2) ) - 1 ) * (10000+res)/10000 / 1000;
   } else {
      *material = 0;
      *energy = 0;
   }
  */
}


void dissectvehicle ( pvehicle eht )
{
  int i,j,k;

   ptechnology techs[32];
   int technum = 0;
   memset ( techs, 0, sizeof ( techs ));
   for (i = 0; i <= eht->klasse; i++) 
      for (j = 0; j < 4; j++ )
         if ( eht->typ->classbound[i].techrequired[j] ) 
           if ( !actmap->player[actmap->actplayer].research.technologyresearched ( eht->typ->classbound[i].techrequired[j] )) {
               int found =  0;
               for (k = 0; k < technum; k++ )
                  if ( techs[k]->id == eht->typ->classbound[i].techrequired[j] )
                     found = 1;
               if ( !found ) {
                  ptechnology tec = gettechnology_forid ( eht->typ->classbound[i].techrequired[j] );
                  if ( tec )
                     techs[technum++] = tec;
               }
           }


   for ( i = 0; i < technum; i++ ) {

      if ( actmap->player[actmap->actplayer].research.activetechnology != techs[i] ) {

         int found = 0;     // Bit 1: Technologie gefunden
                            //     2: vehicletype gefunden      
                            //     3: Technologie+vehicletype gefunden
   
         pdissectedunit du = actmap->player[ actmap->actplayer ].dissectedunit;
         while ( du ) {
            if ( du->fzt == eht->typ ) 
               if ( du->tech == techs[i] )
                  found |= 4;
               else
                  found |= 2;
            
            if ( du->tech == techs[i] )
               found |= 1;
      
            du = du->next;
         }
   
         if ( found & 4 ) {
            du = actmap->player[ actmap->actplayer ].dissectedunit;
            while ( du ) {
               if ( du->fzt == eht->typ ) 
                  if ( du->tech == techs[i] ) {
                     du->points += du->orgpoints / ( 1 << du->num);
                     du->num++;
                  }
   
               du = du->next;
            }
         } else {
            pdissectedunit du2 = new tdissectedunit;
            du = actmap->player[ actmap->actplayer ].dissectedunit;
            if ( du ) {
               while ( du->next )
                  du = du->next;
               du->next = du2;
            } else {
              actmap->player[ actmap->actplayer ].dissectedunit = du2;
            }
   
            du2->next = NULL;
            du2->tech = techs[i];
            du2->fzt = eht->typ;
   
            if ( found & 1 )
               du2->orgpoints = du2->tech->researchpoints / dissectunitresearchpointsplus2;
            else
               du2->orgpoints = du2->tech->researchpoints / dissectunitresearchpointsplus;
   
            du2->points = du2->orgpoints;
            du2->num = 1;
   
         }
      } else 
         actmap->player[actmap->actplayer].research.progress+= techs[i]->researchpoints / dissectunitresearchpointsplus;
      

   }
}



void         generatevehicle_cl ( pvehicletype fztyp,
                                  int         col,
                                  pvehicle &   vehicle,
                                  int          x,
                                  int          y )
{ 
   if ( actmap->player[ actmap->actplayer ].research.vehicletypeavailable ( fztyp ) ) {

      vehicle = new Vehicle ( fztyp, actmap, col );
      if ( fztyp->classnum )
        for (int i = 0; i < fztyp->classnum ; i++ ) 
           if ( actmap->player[ actmap->actplayer ].research.vehicleclassavailable( fztyp, i ) )
              vehicle->klasse = i;
           else
              break;
              
      logtoreplayinfo ( rpl_produceunit, (int) fztyp->id , (int) col * 8, x, y, (int) vehicle->klasse, (int) vehicle->networkid );

      vehicle->xpos = x;
      vehicle->ypos = y;
      vehicle->setup_classparams_after_generation ();

      if ( actmap->getgameparameter(cgp_bi3_training) >= 1 ) {
         int cnt = 0;
         pbuilding bld = actmap->player[ actmap->actplayer ].firstbuilding;
         while ( bld ) {
            if ( bld->typ->special & cgtrainingb )
               cnt++;
            bld = bld->next;
         }
         vehicle->experience += cnt * actmap->getgameparameter(cgp_bi3_training);
         if ( vehicle->experience > maxunitexperience )
            vehicle->experience = maxunitexperience;
      }
   } else
     vehicle = NULL;
} 







void cmousecontrol :: reset ( void )
{
   mousestat = 0;
}


void cmousecontrol :: chkmouse ( void )
{
   if ( CGameOptions::Instance()->mouse.fieldmarkbutton )
      if ( mouseparams.taste == CGameOptions::Instance()->mouse.fieldmarkbutton ) {
         int x; 
         int y;
         int r = getfieldundermouse ( &x, &y );
   
         if ( r ) 
            if ( (cursor.posx != x || cursor.posy != y) && ( moveparams.movestatus == 0) ) {
               // collategraphicoperations cgo;
               mousestat = 1;
               mousevisible(false);
               cursor.hide();
               cursor.posx = x;
               cursor.posy = y;
               cursor.show();

               dashboard.paint( getactfield(), actmap-> playerView );

               mousevisible(true);
            } 
      }

   if ( CGameOptions::Instance()->mouse.centerbutton )
      if ( mouseparams.taste == CGameOptions::Instance()->mouse.centerbutton ) {
         int x; 
         int y;
         int r = getfieldundermouse ( &x, &y );
         x += actmap->xpos;
         y += actmap->ypos;
   
         if ( r ) {
            int newx = x - idisplaymap.getscreenxsize() / 2;
            int newy = y - idisplaymap.getscreenysize() / 2;

            if ( newx < 0 )
               newx = 0;
            if ( newy < 0 )
               newy = 0;
            if ( newx > actmap->xsize - idisplaymap.getscreenxsize() )
               newx = actmap->xsize - idisplaymap.getscreenxsize();
            if ( newy > actmap->ysize - idisplaymap.getscreenysize() )
               newy = actmap->ysize - idisplaymap.getscreenysize();
   
            if ( newy & 1 )
               newy--;

            if ( newx != actmap->xpos  || newy != actmap->ypos ) {
               // collategraphicoperations cgo;
               cursor.hide();
               actmap->xpos = newx;
               actmap->ypos = newy;
               displaymap();
               cursor.posx = x - actmap->xpos;
               cursor.posy = y - actmap->ypos;

               // cursor.gotoxy ( x, y );
               cursor.show();
            }
            while ( mouseparams.taste == CGameOptions::Instance()->mouse.centerbutton )
               releasetimeslice();
         }
      }

   if ( CGameOptions::Instance()->mouse.smallguibutton )
      if ( mouseparams.taste == CGameOptions::Instance()->mouse.smallguibutton ) {
         int x; 
         int y;
         int r = getfieldundermouse ( &x, &y );
   
         if ( r ) 
            if ( (cursor.posx != x || cursor.posy != y) && ( moveparams.movestatus == 0   ||  getfield(actmap->xpos + x , actmap->ypos + y)->a.temp == 0) ) {
               // collategraphicoperations cgo;
               mousestat = 1;
               mousevisible(false);
               cursor.hide();
               cursor.posx = x;
               cursor.posy = y;
               cursor.show();
               mousevisible(true);
            } else 
              if ( mousestat == 2  ||  mousestat == 0 ||  ((moveparams.movestatus || pendingVehicleActions.action) && getfield( actmap->xpos + x, actmap->ypos + y)->a.temp )  ) {
                 {
                    // collategraphicoperations cgo;
                    if ( cursor.posx != x || cursor.posy != y ) {
                       mousevisible(false);
                       cursor.hide();
                       cursor.posx = x;
                       cursor.posy = y;
                       cursor.show();

                       dashboard.paint( getactfield(), actmap-> playerView );

                       mousevisible(true);
                    }

                    actgui->painticons();
                 }
                 pfield fld = getactfield();
                 actgui->paintsmallicons( CGameOptions::Instance()->mouse.smallguibutton, !fld->vehicle && !fld->building && !fld->a.temp );
                 mousestat = 0;
              }
      } else 
        if ( mousestat == 1 )
           mousestat = 2;

   if ( CGameOptions::Instance()->mouse.largeguibutton )
      if ( mouseparams.taste == CGameOptions::Instance()->mouse.largeguibutton ) {
         int x; 
         int y;
         int r = getfieldundermouse ( &x, &y );
         if ( r && ( cursor.posx != x || cursor.posy != y) ) {
            mousevisible(false);
            cursor.hide();
            cursor.posx = x;
            cursor.posy = y;
            cursor.show();
            mousevisible(true);
         }
         actgui->painticons();

         actgui->runpressedmouse ( mouseparams.taste );
      }

   if ( CGameOptions::Instance()->mouse.unitweaponinfo )
      if ( mouseparams.taste == CGameOptions::Instance()->mouse.unitweaponinfo ) {
         int x; 
         int y;
         int r = getfieldundermouse ( &x, &y );
         if ( r && ( cursor.posx != x || cursor.posy != y) ) {
            mousevisible(false);
            cursor.hide();
            cursor.posx = x;
            cursor.posy = y;
            cursor.show();
            mousevisible(true);
         }
         actgui->painticons();
         if ( getactfield()->vehicle ) {
            dashboard.paintvehicleinfo( getactfield()->vehicle, NULL, NULL, NULL );
            dashboard.paintlweaponinfo();
         }
      }

/*
   if ( mouseparams.taste == minmenuekey ) {
      int x; 
      int y;
      if (  getfieldundermouse ( &x, &y ) ) {
         actgui->paintsmallicons( minmenuekey );
      }
   }
*/

}



