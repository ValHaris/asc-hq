//     $Id: controls.cpp,v 1.85 2000-11-21 20:26:56 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.84  2000/11/11 11:05:15  mbickel
//      started AI service functions
//
//     Revision 1.83  2000/11/08 19:30:56  mbickel
//      Rewrote IO for the tmap structure
//      Fixed crash when entering damaged building
//      Fixed crash in AI
//      Removed item CRCs
//
//     Revision 1.82  2000/10/31 10:42:39  mbickel
//      Added building->vehicle service to vehicle controls
//      Moved tmap methods to gamemap.cpp
//
//     Revision 1.81  2000/10/26 18:14:55  mbickel
//      AI moves damaged units to repair
//      tmap is not memory layout sensitive any more
//
//     Revision 1.80  2000/10/18 14:13:53  mbickel
//      Rewrote Event handling; DOS and WIN32 may be currently broken, will be
//       fixed soon.
//
//     Revision 1.79  2000/10/14 10:52:45  mbickel
//      Some adjustments for a Win32 port
//
//     Revision 1.78  2000/10/11 14:26:22  mbickel
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
//     Revision 1.77  2000/09/27 16:08:22  mbickel
//      AI improvements
//
//     Revision 1.76  2000/09/17 15:20:29  mbickel
//      AI is now automatically invoked (depending on gameoptions)
//      Some cleanup
//
//     Revision 1.75  2000/09/16 13:02:51  mbickel
//      Put the AI in place
//
//     Revision 1.74  2000/09/16 11:47:21  mbickel
//      Some cleanup and documentation again
//
//     Revision 1.73  2000/09/07 15:49:38  mbickel
//      some cleanup and documentation
//
//     Revision 1.72  2000/08/29 17:42:42  mbickel
//      Restructured GUI to make it compilable with VisualC.
//
//     Revision 1.71  2000/08/28 19:49:38  mbickel
//      Fixed: replay exits when moving satellite out of orbiter
//      Fixed: airplanes being able to endlessly takeoff and land
//      Fixed: buildings constructable by unit without resources
//
//     Revision 1.70  2000/08/28 14:37:12  mbickel
//      Fixed: satellite not able to leave orbiter
//      Restructured next-turn routines
//
//     Revision 1.69  2000/08/26 15:33:37  mbickel
//      Warning message displayed if empty password is entered
//      pcxtank now displays error messages
//
//     Revision 1.68  2000/08/25 13:42:51  mbickel
//      Fixed: zoom dialogbox in mapeditor was invisible
//      Fixed: ammoproduction: no numbers displayed
//      game options: produceammo and fillammo are now modified together
//      Fixed: sub could not be seen when standing on a mine
//      Some AI improvements
//
//     Revision 1.67  2000/08/13 10:24:07  mbickel
//      Fixed: movement decrease when cloning units
//      Fixed: refuel skipped next action in replay
//
//     Revision 1.66  2000/08/13 09:53:57  mbickel
//      Refuelling is now logged for replays
//
//     Revision 1.65  2000/08/12 15:03:19  mbickel
//      Fixed bug in unit movement
//      ASC compiles and runs under Linux again...
//
//     Revision 1.64  2000/08/12 12:52:42  mbickel
//      Made DOS-Version compile and run again.
//
//     Revision 1.63  2000/08/12 09:17:19  gulliver
//     *** empty log message ***
//
//     Revision 1.62  2000/08/08 13:38:32  mbickel
//      Fixed: construction of buildings doesn't consume resources
//      Fixed: no unit information visible for satellites
//
//     Revision 1.61  2000/08/08 13:21:54  mbickel
//      Added unitCategoriesLoadable property to buildingtypes and vehicletypes
//      Added option: showUnitOwner
//
//     Revision 1.60  2000/08/08 09:48:00  mbickel
//
//      speed up of dialog boxes in linux
//      fixed graphical errors in attack
//      fixed graphical error in ammo transfer
//      fixed reaction fire not allowing manual attack
//
//     Revision 1.59  2000/08/07 16:29:19  mbickel
//      orbiting units don't consume fuel any more
//      Fixed bug in attack formula; improved attack formula
//      Rewrote reactionfire
//
//     Revision 1.58  2000/08/06 11:38:37  mbickel
//      New map paramter: fuel globally available
//      Mapeditor can now filter buildings too
//      Fixed unfreed memory in fullscreen image loading
//      Fixed: wasted cpu cycles in building
//      map parameters can be specified when starting a map
//      map parameters are reported to all players in multiplayer games
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

         tdashboard  dashboard;
         int             windmovement[8];

         trunreplay runreplay;

         int startreplaylate = 0;
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
         computeview();
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
         computeview();
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
            computeview();
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
            computeview();

         // }

         build_vehicles_reset();
      }
      dashboard.x = 0xffff;

   }
}









void         clearvisibility( int  reset )
{
  if ((actmap->xsize == 0) || (actmap->ysize == 0))
     return;

   int l = 0;
   for ( int x = 0; x < actmap->xsize ; x++)
         for ( int y = 0; y < actmap->ysize ; y++) {
            pfield fld = &actmap->field[l];
            memset ( fld->view, 0, sizeof ( fld->view ));
            l++;
         }
}

int  evaluatevisibilityfield ( pfield fld, int player, int add, int initial )
{
   int originalVisibility;
   if ( initial == 2 ) {
      setvisibility(&fld->visible,visible_all, player);
      return 0;
   } else {
      originalVisibility = (fld->visible >> (player * 2)) & 3;
      if ( originalVisibility >= visible_ago || initial == 1)
           setvisibility(&fld->visible,visible_ago, player);
   }

   if ( add == -1 ) {
      add = 0;
      if ( actmap->shareview )
         for ( int i = 0; i < 8; i++ )
            if ( actmap->shareview->mode[i][player] )
               add |= 1 << i;
   }
   add |= 1 << player;

   int sight = 0;
   int jamming = 0;
   int mine = 0;
   int satellite = 0;
   int direct = 0;
   int sonar = 0;
   for ( int i = 0; i < 8; i++ ){
      if ( add & ( 1 << i )) {
         sight += fld->view[i].view;
         mine  += fld->view[i].mine;
         satellite += fld->view[i].satellite;
         direct += fld->view[i].direct;
         sonar += fld->view[i].sonar;
      } else
         jamming += fld->view[i].jamming;
   }
   if ( sight > jamming   ||  direct  ) {
      if ( fld->building && (fld->building->connection & cconnection_seen))
         releaseevent ( NULL, fld->building, cconnection_seen );

      if (( fld->vehicle  && ( fld->vehicle->color  == player * 8 )) ||
          ( fld->vehicle  && ( fld->vehicle->height  < chschwimmend ) && sonar ) ||
          ( fld->building && ( fld->building->typ->buildingheight < chschwimmend ) && sonar ) ||
          ( fld->object && fld->object->mine && ( mine  ||  fld->mineowner() == player)) ||
          ( fld->vehicle  && ( fld->vehicle->height  >= chsatellit )  && satellite )) {
             setvisibility(&fld->visible,visible_all, player);
             return originalVisibility != visible_all;
      } else {
             setvisibility(&fld->visible,visible_now, player);
             return originalVisibility != visible_now;
      }
   }
   return 0;
}


int  evaluateviewcalculation ( int player_fieldcount_mask )
{
   int initial = actmap->getgameparameter ( cgp_initialMapVisibility );
   int fieldsChanged = 0;
   for ( int player = 0; player < 8; player++ )
      if ( actmap->player[player].existent ) {
         int add = 0;
         if ( actmap->shareview )
            for ( int i = 0; i < 8; i++ )
               if ( actmap->shareview->mode[i][player] )
                  add |= 1 << i;

         int nm = actmap->xsize * actmap->ysize;
         if ( player_fieldcount_mask & (1 << player ))
            for ( int i = 0; i < nm; i++ )
                fieldsChanged += evaluatevisibilityfield ( &actmap->field[i], player, add, initial );
         else
            for ( int i = 0; i < nm; i++ )
                evaluatevisibilityfield ( &actmap->field[i], player, add, initial );
      }
   return fieldsChanged;
}



int computeview( int player_fieldcount_mask )
{
   if ((actmap->xsize == 0) || (actmap->ysize == 0))
      return 0;

   clearvisibility( 1 );

   for ( int a = 0; a < 8; a++)
      if (actmap->player[a].existent ) {
         pvehicle actvehicle = actmap->player[a].firstvehicle;
         while ( actvehicle ) {
            if (actvehicle == getfield(actvehicle->xpos,actvehicle->ypos)->vehicle)
               actvehicle->addview();

            actvehicle = actvehicle->next;
         }

         pbuilding actbuilding = actmap->player[a].firstbuilding;
         while ( actbuilding ) {
            actbuilding->addview();
            actbuilding = actbuilding->next;
         }
      }


   return evaluateviewcalculation ( player_fieldcount_mask );
}





  /*


procedure fieldreachable(x1,y1,x2,y2:integer; vehicle:pvehicle; strecke:integer);
var
  b:char;
  reststrecke:integer;
  m,n,o,p,q,dx,dy,x,y:integer;
  w:word;
  pf:pfield;
  mm:byte;
begin
   mm:=8;
   if x1 < 0 then x1:=0;
   if y1 < 0 then y1:=0;
   if x1 >= actmap->xsize then x1:=actmap->xsize-1;
   if y1 >= actmap->ysize then y1:=actmap->ysize-1;
   if x2 < 0 then x2:=0;
   if y2 < 0 then y2:=0;
   if x2 >= actmap->xsize then x2:=actmap->xsize-1;
   if y2 >= actmap->ysize then y2:=actmap->ysize-1;
   m:=beeline(x1,y1,x2,y2);
   if strecke < m then exit;
   reststrecke:=strecke;
   getfield(x1,y1)^.temp:=1;
   x:=x1;
   y:=y1;
   dx:=(2*x2+(y2 and 1)) -(2*x+(y and 1));   {????
   dy:=y2-y;
   while ((x <> x2) or (y <> y2)) and (reststrecke >=0) do begin
      mm:=8;
      if (dx <> 0) and (dy = 0) then begin
         if dx > 0 then inc(x) else dec(x);
         mm:=maxmalq;
      end
      else
         if (dx <> 0) and (dy <> 0) then begin
            if (dx > 0) then begin
               if y and 1 = 1 then
                  inc(x);
               if dy > 0 then inc(y) else dec(y);
            end
            else if dx < 0 then begin
               if y and 1 = 0 then
                  dec(x);
               if dy > 0 then inc(y) else dec(y);
            end;
         end
         else begin

            dx:=(2*x2+(y2 and 1)) -(2*x+(y and 1));  { ????????

            dy:=y2-y;
            if dy > 0 then
               if dy > 1 then begin
                  inc(y,2);
                  mm:=maxmalq;
               end
               else inc(y)
            else if dy < 0 then
               if dy < -1 then begin
                  dec(y,2);
                  mm:=maxmalq;
               end
               else dec(y);
         end;
      dx:=(2*x2+(y2 and 1)) -(2*x+(y and 1));   { ???????á
      dy:=y2-y;

      pf:=getfield(x,y);
      n:=fieldaccessible(pf,vehicle);
      if n = 0 then
         reststrecke:=-1
      else
         if vehicle^.height >= chtieffliegend
         then reststrecke:=reststrecke-mm
         else reststrecke:=reststrecke - (pf^.movemalus * mm shr 3);
      if reststrecke >=0 then
         if n = 2 then
            if pf^.temp < reststrecke +1 then pf^.temp:=reststrecke +1;
   end;
   if (reststrecke < 0) { and (gm = 0)  then
      fieldreachablerek(x1,y1,x2,y2,vehicle,strecke,true);
end;


procedure move(vehicle:pvehicle; maxdist,mindist,movedist:integer);
var
   startx,starty,x,y,strecke:integer;
   a,c,d,e,f,g:integer;
   afield,efield:pfield;

 procedure gotofield;
 begin
    efield := getfield(x,y);
    if efield^.temp = 0 then
       if fieldaccessible(efield,vehicle) = 2 then
          fieldreachable(startx,starty,x,y,vehicle,movedist);
 end;

begin
   startx:=getxpos;
   starty:=getypos;
   moveparams.movesx:=startx;
   moveparams.movesy:=starty;
   afield:=getactfield;
   afield^.temp:=1;
   for strecke:=maxdist downto mindist do begin;

      a:=integer(startx)-integer(strecke);

      x:=a;
      y:=starty;
      for c:=1 to 2*strecke do begin
         dec(y);
         if starty and 1 = c and 1 then inc(x);
         gotofield;
      end;

      x:=startx+strecke+(starty and 1);
      y:=starty;
      for c:=0 to 2*strecke-1 do begin
         if starty and 1 <> c and 1 then dec(x);
         gotofield;
         dec(y);
      end;

      x:=startx;
      y:=starty+strecke*2-1;
      for c:=1 to 2*strecke do begin
         if starty and 1 <> c and 1 then dec(x);
         gotofield;
         dec(y);
      end;

      x:=startx;
      y:=starty+2*strecke;
      for c:=0 to 2*strecke-1 do begin
         gotofield;
         if starty and 1 <> c and 1 then inc(x);
         dec(y);
      end;
   end;
   displaymap;
   moveparams.movestatus:=1;
end;   */



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











#ifndef HEXAGON
class tdrawgettempline : public tdrawline8 {
                  int freefields;
                  int num;
               public:
                  int tempsum;
                  tdrawgettempline ( int _freefields )
                    {
                       tempsum = 0;
                       freefields = _freefields;
                       num = 0;
                    };

                  virtual void putpix8 ( int x, int y )
                     {
                        if ( num >= freefields )
                           tempsum += getfield ( x, y )->getjamming();
                        num++;
                     };
              };
#endif






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
      evaluatevisibilityfield ( fld, i, -1, actmap->getgameparameter ( cgp_initialMapVisibility ) );
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


/*
void         attack(char      kamikaze, int  weapnum )
{

   if (moveparams.movestatus == 0) {
      if (fieldvisiblenow(getactfield())) {
         if (getactfield()->vehicle != NULL) {
            if (getactfield()->vehicle->color != (actmap->actplayer << 3)) {
               dispmessage2(203,"");
               return;
            }
            actmap->cleartemps(7);
            if (!kamikaze) {
               tsearchattackablevehicles sae;
               sae.init(getactfield()->vehicle);
               sae.run();
               sae.done();
            }
            else {
               tsearchattackablevehicleskamikaze ske;
               ske.init(getactfield()->vehicle);
               ske.run();
               ske.done();
            }
            if ( moveparams.movestatus == 10 ) {
               actgui->restorebackground();
               selectweaponguihost.init( hgmp->resolutionx, hgmp->resolutiony );
               //selectweaponguihost.restorebackground();
               actgui = &selectweaponguihost;
               actgui->painticons();
            }

         }
      }
   }
   else
      if (moveparams.movestatus == 10) {
          pfield    fld = getfield(moveparams.movesx,moveparams.movesy);
          pfield    targ = getactfield();

          int x2 = getxpos();
          int y2 = getypos();
          int ad1, ad2, dd1, dd2;

          if ( targ->vehicle ) {
             tunitattacksunit battle ( fld->vehicle, targ->vehicle, 1, weapnum );
             ad1 = battle.av.damage;
             dd1 = battle.dv.damage;
             battle.calcdisplay ();
             ad2 = battle.av.damage;
             dd2 = battle.dv.damage;
             battle.setresult ();
             dashboard.x = 0xffff;
          } else
          if ( targ->building ) {
             tunitattacksbuilding battle ( fld->vehicle, getxpos(), getypos() , weapnum );
             ad1 = battle.av.damage;
             dd1 = battle.dv.damage;
             battle.calcdisplay ();
             ad2 = battle.av.damage;
             dd2 = battle.dv.damage;
             battle.setresult ();
             dashboard.x = 0xffff;
          } else
          if ( targ->object ) {
             tunitattacksobject battle ( fld->vehicle, getxpos(), getypos() , weapnum );
             ad1 = battle.av.damage;
             dd1 = battle.dv.damage;
             battle.calcdisplay ();
             ad2 = battle.av.damage;
             dd2 = battle.dv.damage;
             battle.setresult ();
             dashboard.x = 0xffff;
          }

          int x1 = moveparams.movesx;
          int y1 = moveparams.movesy;
          logtoreplayinfo ( rpl_attack, x1, y1, x2, y2, ad1, ad2, dd1, dd2, weapnum );

          computeview();

          actmap->cleartemps(7);
          dashboard.x = 0xffff;
          moveparams.movestatus = 0;

      }
}
*/

int squareroot ( int i )
{
   double a =  sqrt ( i );
   int c = (int) a;
   return c;
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






/*
void         repairbuilding(pbuilding    building,
                            int *    energy,
                            int *    material)
{
    integer      ekost, mkost;
    int      w;


   if ((building->damage != 0) && (*energy > 0) && (*material > 0)) {
      ekost = int(building->damage) * building->typ->productioncost.sprit / (100 * reparierersparnis);
      mkost = int(building->damage) * building->typ->productioncost.material / (100 * reparierersparnis);

      if (mkost <= *material)
         w = 10000;
      else
         w = 10000 * *material / mkost;

      if (ekost > *energy)
         if (10000 * *energy / ekost < w)
            w = 10000 * *energy / ekost;


      building->damage = building->damage * (1 - w / 10000);
      *material = *material - (w * mkost / 10000);
      *energy = *energy - (w * ekost / 10000);
   }
}
*/


tdashboard::tdashboard ( void )
{
   fuelbkgr  = NULL;
   imagebkgr = NULL;
   movedisp  = 0;
   windheight = 0;
   windheightshown = 0;
   for ( int i = 0; i< 8; i++ )
      weaps[i].displayed = 0;
   repainthard = 1;
   materialdisplayed = 1;
}

void tdashboard::paint ( const pfield ffield, int playerview )
{
   if ( playerview >= 0 ) {
      if (fieldvisiblenow(ffield, playerview ))
         paintvehicleinfo ( ffield->vehicle, ffield->building, ffield->object, NULL );
      else
         paintvehicleinfo( NULL, NULL, NULL, NULL );
   }
}

void         tdashboard::putheight(integer      i, integer      sel)
                                      //          h”he           m”glichk.
{
   putrotspriteimage ( agmp->resolutionx - ( 640 - 589), 92  + i * 13, icons.height2[sel][i], actmap->actplayer * 8);
}






void         tdashboard::paintheight(void)
{
   if ( vehicle )
          for ( int i = 0; i <= 7; i++) {
             if (vehicle->typ->height & (1 << (7 - i)))
                if (vehicle->height & (1 << (7 - i)))
                  putheight(i,1);
                else
                  putheight(i,2);
             else
               putheight(i,0);
          }

    else
       if ( vehicletype ) {
          for ( int i = 0; i <= 7; i++) {
             if (vehicletype->height & (1 << (7 - i)))
               putheight(i,2);
             else
               putheight(i,0);
          }
       } else
          for ( int i = 0; i <= 7; i++)
              putheight(i,0);
}



void         tdashboard::painttank(void)
{
    word         w;
    int         c;

    int x1 = agmp->resolutionx - ( 640 - 520);
    int x2 = agmp->resolutionx - ( 640 - 573);
    int y1 = 59;
    int y2 = 67;

    if ( vehicle )
       if ( vehicle->typ->tank.fuel )
          w = ( x2 - x1 + 1) * vehicle->tank.fuel / vehicle->typ->tank.fuel;
       else
          w = 0;
    else
      w = 0;

    if (w < ( x2 - x1 + 1) )
       bar( x1 + w , y1 , x2, y2 , 172);

    c = vgcol;
    if (w < 25)
       c = 14;
    if (w < 15)
       c = red;
    if ( w )
       bar(x1, y1, x1 + w - 1 , y2 ,c);

    putspriteimage ( x1, y1, fuelbkgr );
}


char*         tdashboard:: str_2 ( int num )
{
      char* tmp;

      if ( num >= 1000000 ) {
         tmp = strrr ( num / 1000000 );
         strcat ( tmp, "M");
         return tmp;
      } else
         if ( num >= 10000 ) {
            tmp = strrr ( num / 1000 );
            strcat ( tmp, "k");
            return tmp;
         } else
            return strrr ( num );
}


void         tdashboard::paintweaponammount(int h, int num, int max )
{
      word         w;

      w = 20 * num / max;
      if (w > 0)
         bar( agmp->resolutionx - ( 640 - 552),     93 + h * 13, agmp->resolutionx - ( 640 - 551 ) + w, 101 + h * 13, 168 );
      if (w < 20)
         bar( agmp->resolutionx - ( 640 - 552) + w, 93 + h * 13, agmp->resolutionx - ( 640 - 571 ),     101 + h * 13, 172 );

      activefontsettings.justify = righttext;
      activefontsettings.font = schriften.guifont;
      activefontsettings.height = 9;
      activefontsettings.background = 255;
      activefontsettings.length = 19;

      showtext2c( str_2( num ), agmp->resolutionx - ( 640 - 552), 93 + h * 13);
}


void         tdashboard::paintweapon(int         h, int num, int strength,  const SingleWeapon  *weap )
{
      if ( weap->getScalarWeaponType() >= 0 )
         if ( weap->canRefuel() )
            putimage ( agmp->resolutionx - ( 640 - 465), 93 + h * 13, xlatpict ( &xlattables.light, icons.unitinfoguiweapons[ weap->getScalarWeaponType() ] ));
         else
            putimage ( agmp->resolutionx - ( 640 - 465), 93 + h * 13, icons.unitinfoguiweapons[ weap->getScalarWeaponType() ] );
      else
         if ( weap->service() )
            putimage ( agmp->resolutionx - ( 640 - 465), 93 + h * 13, icons.unitinfoguiweapons[ cwservicen ] );


      paintweaponammount( h, num, weap->count );


      activefontsettings.background = 172;
      activefontsettings.justify = righttext;
      activefontsettings.font = schriften.guifont;
      activefontsettings.height = 9;
      activefontsettings.length = 27;
      if ( weap->shootable() ) {
         showtext2c( strrr(strength), agmp->resolutionx - ( 640 - 503), 93 + h * 13);

         weaps[h].displayed = 1;
         weaps[h].maxstrength = int(strength * weapDist.getWeapStrength(weap, weap->mindistance, -1, -1 ));
         weaps[h].minstrength = int(strength * weapDist.getWeapStrength(weap, weap->maxdistance, -1, -1 ));
         weaps[h].mindist = weap->mindistance;
         weaps[h].maxdist = weap->maxdistance;

      } else {
         bar( agmp->resolutionx - ( 640 - 503),  93 + h * 13 ,agmp->resolutionx - ( 640 - 530), 101 + h * 13, 172 );
         weaps[h].displayed = 0;
      }



}




void         tdashboard::paintweapons(void)
{
   memset ( weaps, 0, sizeof ( weaps ));

   int i, j;

   activefontsettings.color = black;
   activefontsettings.background = 255;
   activefontsettings.justify = lefttext;
   activefontsettings.font = font;

   int serv = 0;


   int xp = agmp->resolutionx - ( 640 - 465);

   activefontsettings.justify = righttext;
   i = 0;
   int k = 7;

   pvehicletype vt;
   if ( vehicle )
      vt = vehicle->typ;
   else
      vt = vehicletype;

    if ( vt ) {
       if ( vt->weapons->count )
          for (j = 0; j < vt->weapons->count && j < 8; j++) {
             if ( vt->weapons->weapon[j].count ) {
                paintweapon(i, ( vehicle ? vehicle->ammo[j] : vt->weapons->weapon[j].count ), ( vehicle ? vehicle-> weapstrength[j] : vt->weapons->weapon[j].maxstrength ), &vt->weapons->weapon[j] );
                i++;
             }
             else {
                if ( vt->weapons->weapon[j].service() ) {
                   serv = 1;
                   if ( materialdisplayed )
                      if ( vt->tank.fuel ) {
                         putimage ( xp, 93 + k * 13, xlatpict ( &xlattables.light, icons.unitinfoguiweapons[ 8 ] ));
                         paintweaponammount ( k, ( vehicle ? vehicle->tank.fuel : vt->tank.fuel ), vt->tank.fuel );
                         k--;
                      }
                }
             }
          }

       if ( materialdisplayed ) {
          if ( vt->tank.material ) {
             if ( serv )
                putimage ( xp, 93 + k * 13, xlatpict ( &xlattables.light, icons.unitinfoguiweapons[ 11 ] ));
             else
                putimage ( xp, 93 + k * 13, icons.unitinfoguiweapons[ 11 ] );
              paintweaponammount ( k, ( vehicle ? vehicle->tank.material : vt->tank.material ), vt->tank.material );
              k--;
          }
          if ( vt->tank.energy ) {
             if ( serv )
                putimage ( xp, 93 + k * 13, xlatpict ( &xlattables.light, icons.unitinfoguiweapons[ 9 ] ));
             else
                putimage ( xp, 93 + k * 13, icons.unitinfoguiweapons[ 9 ] );
              paintweaponammount ( k, ( vehicle ? vehicle->tank.energy : vt->tank.energy ), vt->tank.energy );
              k--;
          }
       }
    }

    for (j = i; j <= k; j++) {
       putimage( xp, 93 + j * 13, icons.unitinfoguiweapons[12]);
       bar( agmp->resolutionx - ( 640 - 552),  93 + j * 13 ,agmp->resolutionx - ( 640 - 571), 101 + j * 13, 172 );
       bar( agmp->resolutionx - ( 640 - 503),  93 + j * 13 ,agmp->resolutionx - ( 640 - 530), 101 + j * 13, 172 );
    }
}

void         tdashboard :: paintlweaponinfo ( void )
{
   paintlargeweaponinfo();
}

void         tdashboard :: paintlargeweaponinfo ( void )
{
   int i = 0;
   for ( int lw = 0; lw < 16; lw++ )
      largeWeaponsDisplayPos[lw] = -1;

   int serv = -1;
   pvehicletype vt;
   if ( vehicle )
      vt = vehicle->typ;
   else
      vt = vehicletype;
   if ( vt ) {
       npush ( activefontsettings );

       int x1 = (agmp->resolutionx - 640) / 2;
       int y1 = 150;

       int count = 0;
       if ( vt->weapons->count )
          for ( int j = 0; j < vt->weapons->count ; j++)
             if ( vt->weapons->weapon[j].getScalarWeaponType() >= 0 )
                count++;
             else
                if (vt->weapons->weapon[j].service() )
                   serv = count;


       if ( serv >= 0 )
          count++;

       if ( vt->tank.energy )
          count++;

       int funcs;
       if ( vehicle )
          funcs = vehicle->functions;
       else
          funcs  = vt->functions;


       if ( (serv>= 0 || (funcs & cfmaterialref)) && vt->tank.material )
          count++;

       if ( (serv>= 0 || (funcs & cffuelref)) && vt->tank.fuel )
          count++;

       count++;

       setinvisiblemouserectanglestk ( x1, y1, x1 + 640, y1 + count * 25 + 110 );
       void* imgbuf = asc_malloc ( imagesize ( x1, y1, x1 + 640, y1 + count * 25 + 110 ));
       getimage ( x1, y1, x1 + 640, y1 + count * 25 + 110, imgbuf );

       putimage ( x1, y1, icons.weaponinfo[0] );

       getinvisiblemouserectanglestk ();


       if ( vt->weapons->count )
          for ( int j = 0; j < vt->weapons->count ; j++) {
             if ( vt->weapons->weapon[j].getScalarWeaponType() >= 0 ) {
                int maxstrength = vt->weapons->weapon[j].maxstrength;
                int minstrength = vt->weapons->weapon[j].minstrength;
                if ( vehicle && maxstrength ) {
                   minstrength = minstrength * vehicle->weapstrength[j] / maxstrength;
                   maxstrength = vehicle->weapstrength[j];
                }

                paintlargeweapon(i, cwaffentypen[ vt->weapons->weapon[j].getScalarWeaponType() ],
                               ( vehicle ? vehicle->ammo[j] : vt->weapons->weapon[j].count ) , vt->weapons->weapon[j].count,
                               vt->weapons->weapon[j].shootable(), vt->weapons->weapon[j].canRefuel(),
                               maxstrength, minstrength,
                               vt->weapons->weapon[j].maxdistance, vt->weapons->weapon[j].mindistance,
                               vt->weapons->weapon[j].sourceheight, vt->weapons->weapon[j].targ );
                largeWeaponsDisplayPos[i] = j;
                i++;
             }
          }

       if ( serv >= 0 ) {
          paintlargeweapon(i, cwaffentypen[ cwservicen ], -1, -1, -1, -1, -1, -1,
                         vt->weapons->weapon[serv].maxdistance, vt->weapons->weapon[serv].mindistance,
                         vt->weapons->weapon[serv].sourceheight, vt->weapons->weapon[serv].targ );
          largeWeaponsDisplayPos[i] = serv;
          i++;
       }
       if ( vt->tank.energy ) {
          paintlargeweapon(i, resourceNames[ 0 ], ( vehicle ? vehicle->tank.energy : vt->tank.energy ), vt->tank.energy, -1, -1, -1, -1, -1, -1, -1, -1 );
          largeWeaponsDisplayPos[i] = -1;
          i++;
       }

       if ( (serv>= 0 || (funcs & cfmaterialref)) && vt->tank.material ) {
          paintlargeweapon(i, resourceNames[ 1 ], ( vehicle ? vehicle->tank.material : vt->tank.material ), vt->tank.material, -1, -1, -1, -1, -1, -1, -1, -1 );
          largeWeaponsDisplayPos[i] = -1;
          i++;
       }
       if ( (serv>= 0 || (funcs & cffuelref)) && vt->tank.fuel ) {
          paintlargeweapon(i, resourceNames[ 2 ], ( vehicle ? vehicle->tank.fuel : vt->tank.fuel ), vt->tank.fuel, -1, -1, -1, -1, -1, -1, -1, -1 );
          largeWeaponsDisplayPos[i] = -1;
          i++;
       }

      {
         int x = x1;
         int y = y1 + i * 14 + 28;

         int height, width;
         getpicsize ( icons.weaponinfo[4], width, height );

         setinvisiblemouserectanglestk ( x, y, x + width, y + height );

         putspriteimage ( x, y, icons.weaponinfo[4] );

         activefontsettings.justify = centertext;
         activefontsettings.font = schriften.guifont;
         activefontsettings.height = 11;
         activefontsettings.length = 80;
         activefontsettings.background = 255;
         if ( vt->wait )
            showtext2c ( "no", x + 140, y +  2 );
         else
            showtext2c ( "yes", x + 140, y +  2 );

         if ( funcs & cf_moveafterattack )
            showtext2c ( "yes", x + 364, y +  2 );
         else
            showtext2c ( "no", x + 364, y +  2 );

         getinvisiblemouserectanglestk ();
         i++;
      }



      int lastpainted = -1;
      int first = 1;
      while ( mouseparams.taste == 2) {
         int topaint  = -1;
         int serv = 0;
         for ( int j = 0; j < vt->weapons->count ; j++) {
            int x = (agmp->resolutionx - 640) / 2;
            int y = 150 + 28 + (j - serv) * 14;
            if ( mouseinrect ( x, y, x + 640, y+ 14 ))
               if ( largeWeaponsDisplayPos[j] != -1 )
                  topaint = largeWeaponsDisplayPos[j];

         }
         if ( topaint != lastpainted ) {
            if ( topaint == -1 )
               paintlargeweaponefficiency ( i, NULL, first, 0 );
            else {
               int effic[13];
               for ( int k = 0; k < 13; k++ )
                  effic[k] = vt->weapons->weapon[topaint].efficiency[k];
               int mindelta = 1000;
               int maxdelta = -1000;
               for ( int h1 = 0; h1 < 8; h1++ )
                  for ( int h2 = 0; h2 < 8; h2++ )
                     if ( vt->weapons->weapon[topaint].sourceheight & ( 1 << h1 ))
                        if ( vt->weapons->weapon[topaint].targ & ( 1 << h2 )) {
                           int delta = getheightdelta ( h1, h2);
                           if ( delta > maxdelta )
                              maxdelta = delta;
                           if ( delta < mindelta )
                              mindelta = delta;
                        }
               for ( int a = -6; a < mindelta; a++ )
                  effic[6+a] = -1;
               for ( int b = maxdelta+1; b < 7; b++ )
                  effic[6+b] = -1;

               paintlargeweaponefficiency ( i, effic, first, vt->weapons->weapon[topaint].targets_not_hittable );
            }
            lastpainted = topaint;
            first = 0;
         }
         releasetimeslice();
      }

      setinvisiblemouserectanglestk ( x1, y1, x1 + 640, y1 + count * 25 + 40 );
      putimage ( x1, y1, imgbuf );
      getinvisiblemouserectanglestk ();

      asc_free  ( imgbuf );

      npop ( activefontsettings );
   }

}

void         tdashboard::paintlargeweaponefficiency ( int pos, int* e, int first, int nohit )
{
   int x = (agmp->resolutionx - 640) / 2;
   int y = 150 + 28 + pos * 14;

   int height, width;
   getpicsize ( icons.weaponinfo[3], width, height );

   setinvisiblemouserectanglestk ( x, y, x + width, y + height );

   if ( first )
      putspriteimage ( x, y, icons.weaponinfo[3] );

   static int bk1 = -1;
   static int bk2 = -1;
   if ( bk1 == -1 )
      bk1 = getpixel ( x + 100, y + 5 );
   if ( bk2 == -1 )
      bk2 = getpixel ( x + 100, y + 19);

   activefontsettings.justify = centertext;
   activefontsettings.font = schriften.guifont;
   activefontsettings.height = 10;
   activefontsettings.length = 36;
   for ( int i = 0; i < 13; i++ )
      if ( e && e[i] != -1 ) {
         activefontsettings.background = bk1;
         showtext2c ( strrr ( i - 6 ), x + 88 + i * 42, y +  2 );
         activefontsettings.background = bk2;
         showtext4c ( "%s%%", x + 88 + i * 42, y + 15, strrr ( e[i] ) );
      } else {
         activefontsettings.background = bk1;
         showtext2c ( "", x + 88 + i * 42, y +  2 );
         activefontsettings.background = bk2;
         showtext2c ( "",  x + 88 + i * 42, y + 15 );
     }

   activefontsettings.length = 179;
   // activefontsettings.background = white;
   activefontsettings.color = 86;
   activefontsettings.justify = lefttext;
   // activefontsettings.color = black;
   for ( int j = 0; j < cmovemalitypenum; j++ ) {
      int pnt;
      if ( dataVersion >= 2 )
         pnt = !(nohit & (1 << j ));
      else
         pnt = nohit & (1 << j );
      if ( pnt ) {
         activefontsettings.font = schriften.guifont;
         showtext2c ( cmovemalitypes[j],   x + 88 + (j % 3) * 180, y + 15 + 16 + (j / 3) * 12 );
      } else {
         activefontsettings.font = schriften.monogui;
         showtext2  ( cmovemalitypes[j],   x + 88 + (j % 3) * 180, y + 15 + 16 + (j / 3) * 12 );
      }
   }
   getinvisiblemouserectanglestk ();
}


void         tdashboard::paintlargeweapon ( int pos, const char* name, int ammoact, int ammomax, int shoot, int refuel, int strengthmax, int strengthmin, int distmax, int distmin, int from, int to )
{
   int height, width;
   getpicsize ( icons.weaponinfo[1], width, height );



   int x = (agmp->resolutionx - 640) / 2;
   int y = 150 + 28 + pos * 14;

   setinvisiblemouserectanglestk ( x, y, x + width, y + height );

   putspriteimage ( x, y, icons.weaponinfo[1] );
   y += 4;

   activefontsettings.background = 255;
   activefontsettings.font = schriften.guifont;
   activefontsettings.height = 11;
   activefontsettings.length = 75;

   if ( name ) {
      activefontsettings.justify = lefttext;
      activefontsettings.length = 75;
      showtext2c ( name, x + 2, y );
   }

   if ( ammoact >= 0 ) {
      activefontsettings.length = 20;
      char buf[100];
      char buf2[100];
      sprintf(buf, "%s / %s", int2string ( ammoact, buf ), int2string ( ammomax, buf2 ) );
      activefontsettings.length = 50;
      activefontsettings.justify = centertext;
      showtext2c ( buf, x + 77, y );
   }

   if ( shoot >= 0 ) {
      activefontsettings.justify = centertext;
      activefontsettings.length = 25;
      if ( shoot )
         showtext2c ( "yes", x + 130, y );
      else
         showtext2c ( "no", x + 130, y );
   }

   if ( refuel >= 0 ) {
      activefontsettings.justify = centertext;
      activefontsettings.length = 25;
      if ( refuel )
         showtext2c ( "yes", x + 158, y );
      else
         showtext2c ( "no", x + 158, y );
   }

   if ( strengthmax >= 0 ) {
      activefontsettings.justify = lefttext;
      activefontsettings.length = 38;
      showtext2c ( strrr( strengthmax ), x + 190, y );
   }

   if ( strengthmin >= 0 ) {
      activefontsettings.length = 38;
      activefontsettings.justify = righttext;
      showtext2c ( strrr( strengthmin ), x + 190, y );
   }

   if ( distmin >= 0 ) {
      activefontsettings.length = 36;
      activefontsettings.justify = lefttext;
      showtext2c ( strrrd8u( distmin ), x + 237, y );
   }

   if ( distmax >= 0 ) {
      activefontsettings.length = 36;
      activefontsettings.justify = righttext;
      showtext2c ( strrrd8d( distmax ), x + 237, y );
   }


   if ( from > 0 )
      for ( int i = 0; i < 8; i++ )
         if ( from & ( 1 << i ))
            putimage ( x + 285 + i * 22, y-2, icons.weaponinfo[2] );

   if ( to > 0 )
      for ( int i = 0; i < 8; i++ )
         if ( to & ( 1 << i ))
            putimage ( x + 465 + i * 22, y-2, icons.weaponinfo[2] );

   activefontsettings.justify = lefttext;

   getinvisiblemouserectanglestk ();
}




void         tdashboard::allocmem ( void )
{
    int x1 = 520;
    int x2 = 573;
    int y1 = 71;
    int y2 = 79;

    fuelbkgrread = 0;
    fuelbkgr = new char[  imagesize ( x1, y1, x2, y2 ) ] ;

    x1 = 460;
    y1 = 31;

    imagebkgr = new char[  imagesize ( x1, y1, x1 + 30, y1 + 30 ) ];
    imageshown = 10;

}


void         tdashboard::paintdamage(void)
{
    int x1 = agmp->resolutionx - ( 640 - 520);
    int x2 = agmp->resolutionx - ( 640 - 573);
    int y1 = 71;
    int y2 = 79;

    if ( fuelbkgrread == 0 ) {
       getimage ( x1, y1, x2, y2, fuelbkgr );
       int sze = imagesize ( x1, y1, x2, y2 );
       char *pc = (char*) fuelbkgr;
       int cl  = getpixel ( agmp->resolutionx - 81, 63 );
       for ( int m = 4; m < sze; m++ )
          if ( pc[m] == cl )
             pc[m] = 255;
       fuelbkgrread = 1;
    }


    int w = 0;
    int         c;



    if ( vehicle ) {
       w = (x2 - x1 + 1) * ( 100 - vehicle->damage ) / 100;
       if ( w > 23 )       // container :: subwin :: buildinginfo :: damage verwendet die selben Farben
          c = vgcol;
       else
          if ( w > 15 )
             c = yellow;
          else
             if ( w > 7 )
                c = lightred;
             else
                c = red;
    } else
       if ( building ) {
          w = (x2 - x1 + 1) * ( 100 - building->damage ) / 100;
          if (building->damage >= mingebaeudeeroberungsbeschaedigung)
             c = red;
          else
             c = vgcol;

       }
       else
          if ( object ) {
             c = darkgray;
             for ( int i = object->objnum-1; i >= 0; i-- )
               if ( object->object[ i ] -> typ->armor > 0 )
                  w = (x2 - x1 + 1) * ( 100 - object->object[ i ] -> damage ) / 100;

          } else
             w = 0;


    if (w < (x2 - x1 + 1) )
       bar( x1 + w , y1 , x2, y2 , 172);

    if ( w )
       bar(x1, y1, x1 + w - 1, y2 ,c);

    putspriteimage ( x1, y1, fuelbkgr );
}




void         tdashboard::paintexperience(void)
{
    if (vehicle)
       putimage( agmp->resolutionx - ( 640 - 587),  27, icons.experience[vehicle->experience]);
    else
       bar( agmp->resolutionx - ( 640 - 587), 27,agmp->resolutionx - ( 640 - 611), 50, 171);
}




void         tdashboard::paintmovement(void)
{
    if ( vehicle ) {
       activefontsettings.justify = centertext;
       activefontsettings.color = white;
       activefontsettings.background = 172;
       activefontsettings.font = schriften.guifont;
       activefontsettings.length = 17;
       activefontsettings.height = 9;
       if ( vehicle->typ->fuelConsumption ) {
          if ( (movedisp  &&  vehicle->typ->fuelConsumption ) || (minmalq*vehicle->tank.fuel / vehicle->typ->fuelConsumption  < vehicle->getMovement() ))
             showtext2c( strrrd8d( minmalq*vehicle->tank.fuel / vehicle->typ->fuelConsumption ), agmp->resolutionx - ( 640 - 591), 59);
          else
             showtext2c( strrrd8d(vehicle->getMovement() ), agmp->resolutionx - ( 640 - 591), 59);
       } else
          showtext2c( strrrd8d(0), agmp->resolutionx - ( 640 - 591), 59);
    } else
       bar( agmp->resolutionx - ( 640 - 591), 59,agmp->resolutionx - ( 640 - 608), 67, 172);
}

void         tdashboard::paintarmor(void)
{
    if ( vehicle || vehicletype ) {
       activefontsettings.justify = centertext;
       activefontsettings.color = white;
       activefontsettings.background = 172;
       activefontsettings.font = schriften.guifont;
       activefontsettings.length = 18;
       activefontsettings.height = 9;
       int arm;
       if ( vehicle )
          arm = vehicle->armor;
       else
          arm = vehicletype->armor;

       showtext2c( strrr(arm),agmp->resolutionx - ( 640 - 591), 71);
    } else
       bar(agmp->resolutionx - ( 640 - 591), 71,agmp->resolutionx - ( 640 - 608), 79, 172);
}

void         tdashboard::paintwind( int repaint )
{
  int j, i;

/*   void *p;
   if (actmap->weather.wind.direction & 1)
      p = icons.wind.southwest[actmap->weather.wind.speed >> 6];
   else
      p = icons.wind.south[actmap->weather.wind.speed >> 6];

   switch (actmap->weather.wind.direction >> 1) {
      case 0: putimage(430,320,p);
         break;
      case 1: putrotspriteimage90(430,320,p,0);
         break;
      case 2: putrotspriteimage180(430,320,p,0);
         break;
      case 3: putrotspriteimage270(430,320,p,0);
         break;
   }

   activefontsettings.justify = centertext;
   activefontsettings.color = black;
   activefontsettings.background = white;
   activefontsettings.font = schriften.smallarial;
   activefontsettings.length = 30;
   showtext2( strrr(actmap->weather.wind.speed),430,354);

   */

   if ( !CGameOptions::Instance()->smallmapactive ) {
      setinvisiblemouserectanglestk ( agmp->resolutionx - ( 800 - 612), 213, agmp->resolutionx - ( 800 - 781), 305 );

      static int lastdir = -1;

      if ( repaint ) {
         putimage ( agmp->resolutionx - ( 640 - 450), 211, icons.windbackground );
         lastdir = -1;
      }


      if ( !windheightshown ) {
         int x1 = agmp->resolutionx - ( 640 - 489 );
         int x2 = agmp->resolutionx - ( 640 - 509 );
         int y1 = 284;
         int y2 = 294;
         windheightbackground = new char [imagesize ( x1, y1, x2, y2 )];
         getimage ( x1, y1, x2, y2, windheightbackground );
         windheightshown = 1;
      }


      int unitspeed;
      int height = windheight;

      if ( vehicle ) {
          unitspeed = getmaxwindspeedforunit ( vehicle );
          if ( unitspeed < 255*256 )
             height = getwindheightforunit ( vehicle );
      }

      if ( actmap->weather.wind[height].speed ) {
        #ifdef HEXAGON
          if ( lastdir != actmap->weather.wind[height].direction ) {
             putimage ( agmp->resolutionx - ( 640 - 506), 227, icons.wind[ 8 ] );
             char* pic = rotatepict ( icons.windarrow, directionangle[ actmap->weather.wind[height].direction ] );
             int h1,w1, h2, w2;
             getpicsize ( pic, w2, h2 );
             getpicsize ( icons.wind[ 8 ], w1, h1 );
             putspriteimage ( agmp->resolutionx - ( 640 - (506 + w1/2 - w2/2)), 227 + h1/2- h2/2, pic );
             delete[] pic;
             lastdir = actmap->weather.wind[height].direction;
          }
        #else
         putimage ( agmp->resolutionx - ( 640 - 506), 227, icons.wind[ actmap->weather.wind[height].direction ] );
        #endif
      } else
         putimage ( agmp->resolutionx - ( 640 - 506), 227, icons.wind[ 8 ] );

      if ( (actmap->weather.wind[0] == actmap->weather.wind[1]) && (actmap->weather.wind[1] == actmap->weather.wind[2]) ) {
         if ( windheightshown == 2 ) {
            putimage ( agmp->resolutionx - ( 640 - 489), 284, windheightbackground );
            windheightshown = 1;
         }
      } else {
         windheightshown = 2;
         putimage ( agmp->resolutionx - ( 640 - 489), 284, icons.height2[0][ 3 - height] );
      }

      windheight = height;

      for (i = 0; i < (actmap->weather.wind[height].speed+31) / 32 ; i++ ) {
         int color = green;

         if ( vehicle == NULL ) {
            /*
            if ( i >= 6 )
               color = red;
            else
               if ( i >= 4 )
                  color = yellow;
             */
         } else {
             int windspeed = actmap->weather.wind[ height ].speed*maxwindspeed ;
             if ( unitspeed < 255*256 )
                if ( windspeed > unitspeed*9/10 )
                   color = red;
                else
                   if ( windspeed > unitspeed*66/100 )
                     color = yellow;
         }
         bar ( agmp->resolutionx - ( 640 - 597), 282-i*7, agmp->resolutionx - ( 640 - 601), 284-i*7, color );
      } /* endfor */
      for (j = i; j < 8; j++ )
         bar ( agmp->resolutionx - ( 640 - 597), 282-j*7, agmp->resolutionx - ( 640 - 601), 284-j*7, black );

      getinvisiblemouserectanglestk (  );
   }
}


void         tdashboard::paintimage(void)
 {

    int x1 = agmp->resolutionx - ( 640 - 460);
    int y1 = 31;
    if ( imageshown == 10 ) {
       getimage ( x1, y1, x1 + 30, y1 + 30 , imagebkgr );
       imageshown = 0;
    }

    if ( imageshown )
       putimage ( x1, y1, imagebkgr );

    if ( vehicle ) {
       #ifdef HEXAGON

       TrueColorImage* zimg = zoomimage ( vehicle->typ->picture[0], fieldsizex/2, fieldsizey/2, pal, 0 );
       char* pic = convertimage ( zimg, pal ) ;
       putrotspriteimage ( x1+3, y1+3, pic, vehicle->color );
       delete[] pic;
       delete zimg;

       /*
       {
          tvirtualdisplay vi ( 50, 50, 255 );
          putrotspriteimage ( 0, 0, vehicle->typ->picture[0], vehicle->color );
          putmask ( 0, 0, icons.hex2octmask, 0 );
          getimage ( (fieldsizex-30) / 2, (fieldsizey-30) / 2, (fieldsizex-30) / 2 + 30, (fieldsizey-30) / 2 + 30, xlatbuffer );
       }
       putspriteimage( x1, y1, xlatbuffer );
       */

       #else
       putrotspriteimage ( x1, y1, vehicle->typ->picture[0], vehicle->color );
       #endif
       imageshown = 1;
    } else
       imageshown = 0;

}


void         tdashboard::paintclasses ( void )
{
        if ( CGameOptions::Instance()->showUnitOwner ) {
      const char* owner = NULL;
      if ( vehicle )
         owner = actmap->getPlayerName(vehicle->color / 8);
      else
         if ( building )
            owner = actmap->getPlayerName(building->color / 8);

      if ( owner ) {
         activefontsettings.justify = lefttext;
         activefontsettings.color = white;
         activefontsettings.background = 171;
         activefontsettings.font = schriften.guifont;
         activefontsettings.length = 75;
         activefontsettings.height = 0;
         showtext2c( owner, agmp->resolutionx - ( 640 - 500), 42);
         activefontsettings.height = 9;
      } else
         bar ( agmp->resolutionx - ( 640 - 499), 42, agmp->resolutionx - ( 640 - 575), 50, 171 );
   } else
      if (vehicle)
         if (vehicle->typ->classnum) {
            activefontsettings.justify = lefttext;
            activefontsettings.color = white;
            activefontsettings.background = 171;
            activefontsettings.font = schriften.guifont;
            activefontsettings.length = 75;
            activefontsettings.height = 0;
            showtext2c( vehicle->typ->classnames[vehicle->klasse] ,agmp->resolutionx - ( 640 - 500), 42);
            activefontsettings.height = 9;
         } else
            bar ( agmp->resolutionx - ( 640 - 499), 42, agmp->resolutionx - ( 640 - 575), 50, 171 );
      else
         bar ( agmp->resolutionx - ( 640 - 499), 42, agmp->resolutionx - ( 640 - 575), 50, 171 );
}

void         tdashboard::paintname ( void )
{
   if ( vehicle || building || vehicletype) {
      activefontsettings.justify = lefttext;
      activefontsettings.color = white;
      activefontsettings.background = 171;
      activefontsettings.font = schriften.guifont;
      activefontsettings.length = 75;
      activefontsettings.height = 9;
      pvehicletype vt;
      if ( vehicle )
         vt = vehicle->typ;
      else
         vt = vehicletype;

      if ( vehicle || vt ) {
         if ( vehicle && !vehicle->name.empty() )
            showtext2c( vehicle->name.c_str() , agmp->resolutionx - ( 640 - 500 ), 27);
         else
            if ( vt->name && vt->name[0] )
               showtext2c( vt->name , agmp->resolutionx - ( 640 - 500 ), 27);
            else
               if ( vt->description  &&  vt->description[0] )
                  showtext2c( vt->description ,agmp->resolutionx - ( 640 - 500 ), 27);
               else
                  bar ( agmp->resolutionx - ( 640 - 499 ), 27, agmp->resolutionx - ( 640 - 575 ), 35, 171 );

      } else {
         if ( !building->name.empty() )
            showtext2c( building->name.c_str() , agmp->resolutionx - ( 640 - 500), 27);
         else
            showtext2c( building->typ->name , agmp->resolutionx - ( 640 - 500), 27);

      }
      activefontsettings.height = 0;
   } else
      bar ( agmp->resolutionx - ( 640 - 499), 27, agmp->resolutionx - ( 640 - 575), 35, 171 );
}

void         tdashboard::paintplayer( void )
{
   putspriteimage ( agmp->resolutionx - ( 640 - 540), 127, icons.player[actmap->actplayer] );
}


void         tdashboard::paintalliances ( void )
{
   int j = 0;
   for (int i = 0; i< 8 ; i++ ) {
      if ( i != actmap->actplayer ) {
         if ( actmap->player[i].existent ) {
            if ( getdiplomaticstatus ( i*8 ) == capeace )
               putimage ( agmp->resolutionx - ( 640 - 476) + j * 19, agmp->resolutiony - ( 480 - 452), icons.allianz[i][0] );
            else
               putimage ( agmp->resolutionx - ( 640 - 476) + j * 19, agmp->resolutiony - ( 480 - 452), icons.allianz[i][1] );
         } else
             putimage ( agmp->resolutionx - ( 640 - 476) + j * 19, agmp->resolutiony - ( 480 - 452), icons.allianz[i][2] );
         j++;
      }
   } /* endfor */
}

#ifdef FREEMAPZOOM
void         tdashboard::paintzoom( void )
{
   int h;
   getpicsize ( zoom.pic, zoom.picwidth, h );
   zoom.x1 = agmp->resolutionx - ( 640 - 464);
   zoom.x2 = agmp->resolutionx - ( 640 - 609);
   zoom.y1 = agmp->resolutiony - ( 480 - 444);
   zoom.y2 = agmp->resolutiony - ( 480 - 464);

   setinvisiblemouserectanglestk ( zoom.x1, zoom.y1, zoom.x2, zoom.y2 );

   static void* background = NULL;
   if ( !background ) {
      background = asc_malloc ( imagesize ( zoom.x1, zoom.y1, zoom.x2, zoom.y2 ));
      getimage ( zoom.x1, zoom.y1, zoom.x2, zoom.y2, background );
   } else
      putimage ( zoom.x1, zoom.y1, background );

   int actzoom = zoomlevel.getzoomlevel() - zoomlevel.getminzoom();
   int maxzoom = zoomlevel.getmaxzoom() - zoomlevel.getminzoom();
   int dist = zoom.x2 - zoom.picwidth - zoom.x1;
   putimage ( zoom.x1 + dist - dist * actzoom / maxzoom, zoom.y1, zoom.pic );

   getinvisiblemouserectanglestk ();
}
#endif



class tmainshowmap : public tbasicshowmap {
          public:
            void checkformouse ( void );
       };

void tmainshowmap :: checkformouse ( void )
{
   if ( mouseparams.taste == 1 ) {
       int oldx = dispxpos;
       int oldy = dispypos;
       tbasicshowmap :: checkformouse();
       if ( oldx != dispxpos  ||  oldy != dispypos ) {
          setmapposition();
          displaymap();
       }
   }
}

tmainshowmap* smallmap = NULL;

void         tdashboard::paintsmallmap ( int repaint )
{
   setinvisiblemouserectanglestk ( agmp->resolutionx - ( 800 - 612), 213, agmp->resolutionx - ( 800 - 781), 305 );
   if ( !smallmap ) {
      smallmap = new tmainshowmap;
      CGameOptions::Instance()->smallmapactive = 1;
      CGameOptions::Instance()->setChanged();
      repaint = 1;
   }

   if ( repaint )
      bar ( agmp->resolutionx - ( 800 - 612), 213, agmp->resolutionx - ( 800 - 781), 305, greenbackgroundcol );

   smallmap->init ( agmp->resolutionx - ( 800 - 612 ) , 213, 781-612, 305-213 );
   smallmap->generatemap ( 1 );
   smallmap->dispimage ( );

   getinvisiblemouserectanglestk (  );

}

void         tdashboard::checkformouse ( int func )
{

    if ( mouseinrect ( agmp->resolutionx - ( 800 - 612), 213, agmp->resolutionx - ( 800 - 781), 305 ) && (mouseparams.taste == 2)) {
       CGameOptions::Instance()->smallmapactive = !CGameOptions::Instance()->smallmapactive;
       CGameOptions::Instance()->setChanged();

       if ( CGameOptions::Instance()->smallmapactive )
          dashboard.paintsmallmap( 1 );
       else
          dashboard.paintwind( 1 );

       while ( mouseparams.taste == 2 )
          releasetimeslice();
    }
    /*
    if ( mouseinrect ( agmp->resolutionx - ( 800 - 620),  90, agmp->resolutionx - ( 800 - 735), 196 ) && (mouseparams.taste == 2)) {
       npush ( activefontsettings );
       materialdisplayed = !materialdisplayed;
       setinvisiblemouserectanglestk ( agmp->resolutionx - ( 800 - 620),  90, agmp->resolutionx - ( 800 - 735), 196 );
       paintweapons();
       while ( mouseparams.taste == 2 );
       getinvisiblemouserectanglestk ();
       npop ( activefontsettings );
    }
    */

    if ( (func & 1) == 0 ) {
       if ( smallmap  &&  CGameOptions::Instance()->smallmapactive )
          smallmap->checkformouse();

       if ( !CGameOptions::Instance()->smallmapactive ) {
          if ( mouseparams.x >= agmp->resolutionx - ( 640 - 588 )   &&   mouseparams.x <= agmp->resolutionx - ( 640 - 610 )  &&   mouseparams.y >= 227   &&   mouseparams.y <= 290  && (mouseparams.taste & 1) ) {
             displaywindspeed();
             while ( mouseparams.x >= agmp->resolutionx - ( 640 - 588 )  &&   mouseparams.x <= agmp->resolutionx - ( 640 - 610 )  &&   mouseparams.y >= 227   &&   mouseparams.y <= 290  && (mouseparams.taste & 1) )
                releasetimeslice();
          }
          if ( mouseinrect ( agmp->resolutionx - ( 640 - 489 ), 284, agmp->resolutionx - ( 640 - 509 ), 294 ) && (mouseparams.taste & 1)) {
             dashboard.windheight++;
             if ( dashboard.windheight > 2 )
                dashboard.windheight = 0;
             dashboard.x = 0xffff;
             paintwind(1);
             while ( mouseinrect ( agmp->resolutionx - ( 640 - 489 ), 284, agmp->resolutionx - ( 640 - 509 ), 294 ) && (mouseparams.taste & 1) )
                releasetimeslice();
          }
       }

      #ifdef FREEMAPZOOM
       if ( mouseparams.taste == 1 )
          if ( mouseinrect ( zoom.x1, zoom.y1, zoom.x2, zoom.y2 )) {
             int pos = mouseparams.x - zoom.x1;
             pos -= zoom.picwidth / 2;
             int w = zoom.x2 - zoom.x1 - zoom.picwidth;
             int perc = 1000 * pos / w;
             if ( perc < 0 )
                perc = 0;
             if ( perc > 1000 )
                perc = 1000;
             int newzoom = zoomlevel.getminzoom() + (zoomlevel.getmaxzoom() - zoomlevel.getminzoom()) * ( 1000 - perc ) / 1000;
             if ( newzoom != zoomlevel.getzoomlevel() ) {
                cursor.hide();
                zoomlevel.setzoomlevel( newzoom );
                paintzoom();
                cursor.show();
                displaymap();
                displaymessage2("new zoom level %d%%", newzoom );
                dashboard.x = 0xffff;
             }
          }
       #endif
    }

    if ( mouseparams.x >= agmp->resolutionx - ( 640 - 578 )   &&   mouseparams.x <= agmp->resolutionx - ( 640 - 609 )  &&   mouseparams.y >=  59   &&   mouseparams.y <=  67  && (mouseparams.taste & 1) ) {
       dashboard.movedisp = !dashboard.movedisp;
       dashboard.x = 0xffff;
       while ( mouseparams.x >= agmp->resolutionx - ( 640 - 578 )   &&   mouseparams.x <= agmp->resolutionx - ( 640 - 609 )  &&   mouseparams.y >=  59   &&   mouseparams.y <=  67  && (mouseparams.taste & 1) )
          releasetimeslice();
    }

    for ( int i = 0; i < 8; i++ ) {
       if ( dashboard.weaps[i].displayed )
          if ( mouseinrect ( agmp->resolutionx - ( 640 - 502 ), 92 + i * 13, agmp->resolutionx - ( 640 - 572 ), 102 + i * 13 ) && (mouseparams.taste == 1)) {
             char tmp1[100];
             char tmp2[100];
             strcpy ( tmp1, strrrd8d ( dashboard.weaps[i].maxdist ));
             strcpy ( tmp2, strrrd8u ( dashboard.weaps[i].mindist ));
             displaymessage2 ( "min strength is %d at %s fields, max strength is %d at %s fields", dashboard.weaps[i].minstrength, tmp1, dashboard.weaps[i].maxstrength, tmp2 );

             while ( mouseinrect ( agmp->resolutionx - ( 640 - 502 ), 92 + i * 13, agmp->resolutionx - ( 640 - 572 ), 102 + i * 13 ) && (mouseparams.taste == 1))
                releasetimeslice();
          }
   }

   if ( (vehicle || vehicletype  ) && mouseinrect ( agmp->resolutionx - ( 640 - 461 ), 89, agmp->resolutionx - ( 640 - 577 ), 196 ) && (mouseparams.taste == 2))
      paintlargeweaponinfo();


   if ( mouseparams.x >= agmp->resolutionx - ( 640 - 501 )   &&   mouseparams.x <= agmp->resolutionx - ( 640 - 573 )   &&   mouseparams.y >= 71    &&   mouseparams.y <= 79   && (mouseparams.taste & 1) ) {
       pfield fld = getactfield();
       if ( fieldvisiblenow ( fld ) ) {
          if ( fld->vehicle )
             displaymessage2("damage is %d", fld->vehicle->damage );
          else
          if ( fld->building )
             displaymessage2("damage is %d", fld->building->damage );
          else
          if ( fld->object ) {
             char temp[1000];
             strcpy ( temp, "damage is " );
             for ( int i = fld->object->objnum-1; i >= 0; i-- )
                if ( fld->object->object[i]->typ->armor >= 0 ) {
                   strcat ( temp, strrr ( fld->object->object[i]->damage ));
                   strcat ( temp, " ");
                }

             displaymessage2( temp );
          }

       }
       while ( mouseparams.x >= agmp->resolutionx - ( 640 - 501 )   &&   mouseparams.x <= agmp->resolutionx - ( 640 - 573 )  &&   mouseparams.y >= 71    &&   mouseparams.y <= 79   && (mouseparams.taste & 1) )
          releasetimeslice();
   }

   if ( mouseparams.x >= agmp->resolutionx - ( 640 - 501 )   &&   mouseparams.x <= agmp->resolutionx - ( 640 - 573 )   &&   mouseparams.y >= 59    &&   mouseparams.y <= 67   && (mouseparams.taste & 1) ) {
       pfield fld = getactfield();
       if ( fieldvisiblenow ( fld ) ) {
          if ( fld->vehicle )
             displaymessage2("unit has %d fuel", fld->vehicle->tank.fuel );
       }
       while ( mouseparams.x >= agmp->resolutionx - ( 640 - 501 )   &&   mouseparams.x <= agmp->resolutionx - ( 640 - 573 )   &&   mouseparams.y >= 59    &&   mouseparams.y <= 67   && (mouseparams.taste & 1) )
          releasetimeslice();
   }
}



void   tdashboard :: paintvehicleinfo( const pvehicle     vehicle,
                                       const pbuilding    building,
                                       const pobjectcontainer      object,
                                       const pvehicletype vt )
{
   collategraphicoperations cgo ( agmp->resolutionx - 800 + 610, 15, agmp->resolutionx - 800 + 783, 307 );

   int         ms;

   npush( activefontsettings );
   ms = getmousestatus();
   if (ms == 2) mousevisible(false);
   dashboard.backgrndcol    = 24;
   dashboard.vgcol          = green;    /* 26 / 76  */
   dashboard.ymx            = 471;    /*  469 / 471  */
   dashboard.ymn            = 380;
   dashboard.ydl            = dashboard.ymx - dashboard.ymn;
   dashboard.munitnumberx   = 545;
   dashboard.vehicle        = vehicle;
   dashboard.building       = building;
   dashboard.object         = object;
   dashboard.vehicletype    = vt;

   dashboard.paintheight();
   dashboard.paintweapons();
   dashboard.paintdamage();
   dashboard.painttank();
   dashboard.paintexperience();
   dashboard.paintmovement();
   dashboard.paintarmor();

   if ( CGameOptions::Instance()->smallmapactive )
      dashboard.paintsmallmap( dashboard.repainthard );
   else
      dashboard.paintwind( dashboard.repainthard );

   dashboard.paintname();
   dashboard.paintclasses ();
   dashboard.paintimage();
   dashboard.paintplayer();
  #ifndef FREEMAPZOOM
   dashboard.paintalliances();
  #else
   dashboard.paintzoom();
  #endif
   dashboard.x = getxpos();
   dashboard.y = getypos();
   if (ms == 2) mousevisible(true);
   npop( activefontsettings );

   dashboard.repainthard = 0;

   if ( actmap && actmap->ellipse )
      actmap->ellipse->paint();

}   /*  paintvehicleinfo  */



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




void addanytechnology ( ptechnology tech, int player )
{
   if ( tech ) {
      presearch resrch = &actmap->player[player].research;
      pdevelopedtechnologies devtech = new tdevelopedtechnologies;

      devtech->tech = tech;
      devtech->next = resrch->developedtechnologies;
      resrch->developedtechnologies = devtech;
      for ( int i = 0; i < waffenanzahl; i++)
         resrch->unitimprovement.weapons[i] = resrch->unitimprovement.weapons[i] * tech->unitimprovement.weapons[i] / 1024;
      resrch->unitimprovement.armor = resrch->unitimprovement.armor * tech->unitimprovement.armor / 1024;

      resrch->activetechnology = NULL;
      if ( tech->techlevelset )
         settechlevel ( tech->techlevelset, 1 << player );


      actmap->player[player].queuedEvents++;
   }

}

void addtechnology ( void )
{
   presearch resrch = &actmap->player[actmap->actplayer].research;
   if ( resrch->activetechnology )
      addanytechnology ( resrch->activetechnology, actmap->actplayer );
}


void    tprotfzt::initbuffer( void )
{
   buf = new char[ vehicletypenum ];

   int i;
   for ( i=0; i < vehicletypenum ; i++ )
      buf[i] = actmap->player[ actmap->actplayer ].research.vehicletypeavailable ( getvehicletype_forpos ( i ), actmap );

}



void    tprotfzt::evalbuffer( void )
{

   int i, num = 0;
   for ( i=0; i < vehicletypenum ;i++ ) {
      if (buf[i] == 0) {
          buf[i] = actmap->player[ actmap->actplayer ].research.vehicletypeavailable ( getvehicletype_forpos ( i ), actmap );
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























void getpowerplantefficiency (  const pbuilding bld, int* material, int* fuel )
{
   *material = bld->typ->efficiencymaterial;
   *fuel = bld->typ->efficiencyfuel;
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
   typedef struct tresbuild* presbuild;

   struct  tresbuild {
               int        eff;
               pbuilding  bld;
               presbuild  next;
           };


   presbuild first = NULL;

   pbuilding bld = actmap->player[i].firstbuilding;
   while ( bld ) {
      if ( bld->typ->special & cgresearchb ) {
         int energy, material;
         returnresourcenuseforresearch ( bld, bld->researchpoints, &energy, &material );

         presbuild  a = new ( tresbuild );
         if ( energy )
            a->eff = 16384 * bld->researchpoints / energy ;
         else
            a->eff = maxint;

         a->bld = bld;

         presbuild b  = first;
         presbuild bp = NULL;
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

   presbuild  a = first;
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

   presbuild  b;
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

         if ( (actmap->player[actmap->actplayer].passwordcrc && actmap->player[actmap->actplayer].passwordcrc != CGameOptions::Instance()->defaultpassword ) // && actmap->player[actmap->actplayer].passwordcrc != encodepassword ( password )
            || actmap->time.a.turn == 1 || (actmap->network && actmap->network->globalparams.reaskpasswords) ) {
               if ( forcepasswordchecking < 0 ) {
                  delete actmap;
                  actmap = NULL;
                  throw NoMapLoaded();
               } else {
                  int stat;
                  do {
                     stat = enterpassword ( &actmap->player[actmap->actplayer].passwordcrc );
                  } while ( !actmap->player[actmap->actplayer].passwordcrc && stat==1 && viewtextquery ( 910, "warning", "~e~nter password", "~c~ontinue without password" ) == 0 ); /* enddo */
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

                 addtechnology();

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
   computeview();

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
                  j -= ( actvehicle->getMovement() * 64 / actvehicle->typ->movement[log2(actvehicle->height)] ) * (actmap->weather.wind[ getwindheightforunit ( actvehicle ) ].speed * maxwindspeed / 256 ) * actvehicle->typ->fuelConsumption / ( minmalq * 64 );

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
         actmap->player[ actmap->actplayer ].ai = new AI ( actmap );

      actmap->player[ actmap->actplayer ].ai->run();

   } else {
      tlockdispspfld displock;
      checkalliances_at_beginofturn ();
      computeview();
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


void checkforreplay ( void )
{
   if ( !actmap->replayinfo )
      return;

   int rpnum  = 0;
   int s = actmap->actplayer + 1;
   if ( s >= 8 )
       s = 0;
   while ( s != actmap->actplayer ) {
       if ( actmap->replayinfo->map[s] && actmap->replayinfo->guidata[s] )
          rpnum++;

       if ( s < 7 )
          s++;
       else
          s = 0;
   }


   if ( actmap->replayinfo  &&  rpnum  &&  actmap->player[ actmap->actplayer ].stat == ps_human )
      if (choice_dlg("run replay of last turn ?","~y~es","~n~o") == 1) {
         int s = actmap->actplayer + 1;
         if ( s >= 8 )
            s = 0;
         while ( s != actmap->actplayer ) {
            if ( s >= 8 )
               s = 0;
             if ( actmap->replayinfo->map[s] && actmap->replayinfo->guidata[s] ) {

                npush ( lockdisplaymap );
                lockdisplaymap = 0;

                int t;

                do {
                   t = runreplay.run ( s );
                } while ( t ); /* enddo */

                npop ( lockdisplaymap );

             }
             if ( s < 7 )
                 s++;
              else
                 s = 0;
          }


      }
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
   if ( actmap->player[ actmap->actplayer ].research.vehicletypeavailable ( fztyp, actmap ) ) {

      vehicle = new Vehicle ( fztyp, actmap, col );
      if ( fztyp->classnum )
        for (int i = 0; i < fztyp->classnum ; i++ ) 
           if ( actmap->player[ actmap->actplayer ].research.vehicleclassavailable( fztyp, i, actmap ) )
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







void MapNetwork :: searchfield ( int x, int y, int dir )
{
  int s;

   pfield fld = getfield ( x, y );
   if ( !fld )
      return;

   int arr[sidenum]; 

   do { 

      if ( fld->a.temp )
         return;

      #ifdef netdebug
      cursor.gotoxy ( a, b );
      displaymap();
      #endif

      if ( fld->building ) { 
         searchbuilding ( x, y );
         return;
      }

      fld->a.temp = 1;

      int d = fieldavail( x, y );
      if ( d <= 0 )
         return;

      searchvehicle ( x, y );

      int olddir = dir + sidenum/2; 
      while (olddir >= sidenum ) 
         olddir -= sidenum; 

         int r = 0; 
         for (s = 0; s < sidenum; s++) { 
            if ( (d & (1 << s))  &&  ( s != olddir )) {
               arr[ r ] = s; 
               r++;
            } 
         } 


         if (r > 1) {      // Kreuzungsstelle 
            for ( s = 0; s < r; s++) { 
               int nx = x;
               int ny = y;
               getnextfield ( nx, ny, arr[s] );
               searchfield( nx, ny, arr[s] );
               if ( searchfinished() )
                  return ;
            } 
            return;
         } else  
            if ( r == 1 ) {
               dir = arr[0];
               getnextfield ( x, y, dir );
               fld = getfield( x, y ); 
               if ( !fld )
                  return;
            } else
               return;
   }  while ( 1 ); 

}

void MapNetwork :: searchvehicle ( int x, int y )
{
   if ( pass == 2 ) {
      pfield newfield = getfield ( x, y );
      if ( newfield )
         if ( !newfield->a.temp2 )
           if ( newfield->vehicle ) {
              checkvehicle ( newfield->vehicle );
              newfield->a.temp2 = 1;
           }
   }
}


void MapNetwork :: searchbuilding ( int x, int y )
{
   pbuilding bld = getfield( x, y )->building;
   if ( !bld )
      return;

   pfield entry = bld->getField ( bld->typ->entry.x, bld->typ->entry.y );
   if ( entry->a.temp )
      return;

   if ( pass == 1 )
      checkbuilding( bld );

   entry->a.temp = 1;

   if ( !searchfinished() ) 
      for( int i = 0; i < 4; i++ )
         for ( int j = 0; j < 6; j++ ) {
            int xp, yp;
            bld->getFieldCoordinates ( i, j, xp, yp );
            pfield fld2 = getfield ( xp, yp );
            if ( fld2 && fld2->building == bld )
               for ( int d = 0; d < sidenum; d++ ) {
                  int xp2 = xp;
                  int yp2 = yp;
                  getnextfield ( xp2, yp2, d );
                  pfield newfield = getfield ( xp2, yp2 );
                  if ( newfield && newfield->building != bld  && !newfield->a.temp )
                     searchfield ( xp2, yp2, d );

                  searchvehicle ( xp2, yp2 );

               } /* endfor */
         }
}


int MapNetwork :: instancesrunning = 0;

MapNetwork :: MapNetwork ( int checkInstances )
{
   if ( checkInstances ) {
      if ( instancesrunning )
         displaymessage(" fatal error at MapNetwork; there are other running instances ", 2 );
   } 
   // else displaymessage("warning: Mapnetwork instance check disabled !", 1 );
// this could be resolved by using the different bits of field->a.temp 

   instancesrunning++;
   pass = 1;
}


MapNetwork :: ~MapNetwork ()
{ 
   instancesrunning--;
}


void MapNetwork :: start ( int x, int y )
{
   if ( globalsearch() == 2 ) {
      for ( int i = 0; i < 8; i++ )
         if ( actmap->player[i].existent ) {
            pbuilding bld  = actmap->player[i].firstbuilding;
            while ( bld ) {
               checkbuilding ( bld );
               bld = bld->next;
            } /* endwhile */

            if ( !searchfinished() ) {
               pass++;
               pvehicle veh  = actmap->player[i].firstvehicle;
               while ( veh ) {
                  checkvehicle ( veh );
                  veh = veh->next;
               } /* endwhile */
            }

         }
   } else 
      if ( globalsearch() == 1 ) {
         actmap->cleartemps(7);
         startposition.x = x;
         startposition.y = y;
         searchfield ( x, y, -1 );
         actmap->cleartemps(7);
         if ( !searchfinished() ) {
            pass++;
            startposition.x = x;
            startposition.y = y;
            searchfield ( x, y, -1 );
            actmap->cleartemps(7);
         }
      } else  
         if ( globalsearch() == 0 ) {
            pfield fld = getfield ( x, y );
            if ( fld )
               if ( fld->building ) {
                  if ( pass == 1 )
                     checkbuilding( fld->building );
               } else
                  if ( fld->vehicle )
                     if ( pass == 2 )
                        checkvehicle ( fld->vehicle );
         }
}



int ResourceNet :: fieldavail ( int x, int y )
{
    pfield fld = getfield ( x, y );
/*    pobject o = fld->checkforobject ( pipelineobject ) ; 
    if ( o )
       return o->dir;
    else */

    if ( fld ) {
       tterrainbits tb = cbpipeline;
       if ( resourcetype == 0)
         tb |= cbpowerline;

       if ( fld->bdt & tb ) {
          int d = 0;
          for ( int i = 0; i < sidenum; i++ ) {
             int xp = x;
             int yp = y;
             getnextfield ( xp, yp , i );
             pfield fld2 = getfield ( xp, yp );
             if ( fld2 )
                if ( (fld2->bdt & tb) ||  fld2->building )
                   d |= ( 1 << i );
          }
          return d;

       } else
          return 0;
    }
    return 0;

}


int StaticResourceNet :: getresource ( int x, int y, int resource, int _need, int _queryonly, int _player, int _scope )
{
   if ( actmap->isResourceGlobal ( resource ))
      scope = 3;
   else
      scope = _scope;

   player = _player;
   resourcetype = resource;
   got = 0;
   need = _need;
   queryonly = _queryonly;

   if (scope == 3 && player == 8 )     // neutral player has no map-wide pool
      scope = 0;

   start ( x , y );
   return got;
}


int StaticResourceNet :: searchfinished ( void )
{
   return got >= need;
}





GetResource :: GetResource ( int scope )
             : StaticResourceNet ( scope )
{
   memset ( tributegot, 0, sizeof ( tributegot ));
}


void GetResource :: checkvehicle ( pvehicle v )
{
   if ( v->color/8 == player && resourcetype == 0 ) {
      int toget = need-got;
      if ( v->tank.energy < toget )
         toget = v->tank.energy;
   
      if ( !queryonly )
         v->tank.energy -= toget;
      got += toget;
   }
}


void GetResource :: checkbuilding ( pbuilding b )
{
   if ( b->color/8 == player ) {
      if ((b->netcontrol & (cnet_stopenergyoutput << resourcetype)) == 0) {
         int toget = need-got;
         if ( b->actstorage.resource( resourcetype ) < toget )
            toget = b->actstorage.resource( resourcetype );
      
         if ( !queryonly )
            b->actstorage.resource( resourcetype ) -= toget;
         got += toget;
      }
   } else {
      int gettable = actmap->tribute.avail[ b->color / 8 ][ player ].resource(resourcetype) - tributegot[ resourcetype ][ b->color / 8];
      if ( gettable > 0 ) {
         int toget = need-got;
         if ( toget > gettable )
            toget = gettable;

         // int found = b->getResource( toget, resourcetype, queryonly );
         int found = b->actstorage.resource(resourcetype);
         if ( toget < found )
            found = toget;

         if ( !queryonly )
            b->actstorage.resource(resourcetype) -= found;

         tributegot[ resourcetype ][ b->color / 8] += found;

         if ( !queryonly ) {
            actmap->tribute.avail[ b->color / 8 ][ player ].resource( resourcetype ) -= found;
            actmap->tribute.paid [ b->color / 8 ][ player ].resource( resourcetype ) += found;
         }

         got += found;
      }
   }
}

void GetResource :: start ( int x, int y )
{
   if ( scope == 3 ) {
      got = need;
      if ( got > actmap->bi_resource[player].resource( resourcetype ) )
         got = actmap->bi_resource[player].resource( resourcetype );

      if ( !queryonly )
         actmap->bi_resource[player].resource( resourcetype ) -= got;

   } else
      MapNetwork :: start ( x, y );
}








void PutResource :: checkbuilding ( pbuilding b )
{
   if ( b->color/8 == player ) {
      if ((b->netcontrol & (cnet_stopenergyinput << resourcetype)) == 0) {
         int tostore = need-got;
         if ( b->gettank ( resourcetype ) - b->actstorage.resource( resourcetype ) < tostore )
            tostore = b->gettank ( resourcetype ) - b->actstorage.resource( resourcetype );
      
         if ( !queryonly )
            b->actstorage.resource( resourcetype ) += tostore;
         got += tostore;
      }
   }
}


void PutResource :: start ( int x, int y )
{
   if ( scope == 3 ) {

      got = need;
      if ( got > maxint - actmap->bi_resource[player].resource( resourcetype ) )
         got = maxint - actmap->bi_resource[player].resource( resourcetype );

      if ( !queryonly )
         actmap->bi_resource[player].resource( resourcetype ) += got;

   } else
      MapNetwork :: start ( x, y );
}





void PutTribute :: checkbuilding ( pbuilding b )
{
   if ( b->color/8 == targplayer ) {
      if ((b->netcontrol & (cnet_stopenergyinput << resourcetype)) == 0) {
         int tostore = need-got;
         if ( b->gettank ( resourcetype ) - b->actstorage.resource( resourcetype ) < tostore )
            tostore = b->gettank ( resourcetype ) - b->actstorage.resource( resourcetype );
      
         if ( !queryonly ) {
            b->actstorage.resource( resourcetype ) += tostore;
            actmap->tribute.avail[ player ][ targplayer ].resource( resourcetype ) -= tostore;
            actmap->tribute.paid [ targplayer ][ player ].resource( resourcetype ) += tostore;
         }
         got += tostore;
      }
   }
}


void PutTribute :: start ( int x, int y )
{
   int pl = targplayer;
   if ( pl == -1 )
      targplayer = 0;

   do {
      if ( targplayer != player )
         if ( actmap->player[targplayer].existent ) {
            need = actmap->tribute.avail[ player ][ targplayer ].resource( resourcetype );
            if ( need > 0 ) {
               if ( scope == 3 ) {

                  got = need;
                  if ( got > maxint - actmap->bi_resource[targplayer].resource( resourcetype ) )
                     got = maxint - actmap->bi_resource[targplayer].resource( resourcetype );

                  if ( got > actmap->bi_resource[player].resource( resourcetype ) )
                     got = actmap->bi_resource[player].resource( resourcetype );

                  if ( !queryonly ) {
                     actmap->bi_resource[targplayer].resource( resourcetype ) += got;
                     actmap->bi_resource[player].resource( resourcetype ) -= got;

                     actmap->tribute.avail[ player ][ targplayer ].resource( resourcetype ) -= got;
                     actmap->tribute.paid [ targplayer ][ player ].resource( resourcetype ) += got;
                  }

               } else {
                  int avail = startbuilding->getResource ( need, resourcetype, 1, 0 );
                  if ( need > avail )
                     need = avail;
                  MapNetwork :: start ( x, y );
                  if ( !queryonly )
                     startbuilding->getResource ( got, resourcetype, queryonly, 0 );

               }
            }
         }
      targplayer++;
   } while ( targplayer < 8  && pl == -1 );
}

int PutTribute :: puttribute ( pbuilding start, int resource, int _queryonly, int _forplayer, int _fromplayer, int _scope )
{
   startbuilding = start;
   targplayer = _forplayer;
   return getresource ( startbuilding->xpos, startbuilding->ypos, resource, 0, _queryonly, _fromplayer, _scope );
}


void transfer_all_outstanding_tribute ( void )
{
   int targplayer = actmap->actplayer;
     // for ( int player = 0; player < 8; player++ )
   if ( actmap->player[targplayer].existent ) {
      char text[10000];
      text[0] = 0;

      for ( int player = 0; player < 8; player++ ) {
         if ( targplayer != player )
            if ( actmap->player[player].existent ) {
               int topay[3];
               int got[3];
               for ( int resourcetype = 0; resourcetype < 3; resourcetype++ ) {
                  topay[ resourcetype ] = actmap->tribute.avail[ player ][ targplayer ].resource( resourcetype );
                  got[ resourcetype ] = 0;

                  if ( !actmap->isResourceGlobal (resourcetype) ) {
                     pbuilding bld = actmap->player[ player ].firstbuilding;
                     while ( bld  &&  topay[resourcetype] > got[resourcetype] ) {
                        PutTribute pt;
                        got[resourcetype] += pt.puttribute ( bld, resourcetype, 0, targplayer, player, 1 );
                        bld = bld->next;
                     }
                  } else {
                     int i;
                     if ( actmap->bi_resource[ player ].resource(resourcetype) < topay[resourcetype] )
                        i = actmap->bi_resource[ player ].resource(resourcetype);
                     else
                        i = topay[resourcetype];
                     got [resourcetype ] = i;
                     actmap->bi_resource[ player ].resource(resourcetype) -= i;
                     actmap->bi_resource[ targplayer ].resource(resourcetype) += i;
                  }

                  actmap->tribute.avail[ player ][ targplayer ].resource( resourcetype ) -= got[resourcetype];
                  actmap->tribute.paid[ targplayer ][ player ].resource( resourcetype ) += got[resourcetype];

               }
               if ( topay[0] || topay[1] || topay[2] ) {
                  char txt1b[1000];
                  char txt_topay[100];
                  txt_topay[0] = 0;
                  int r;
                  int cnt = 0;
                  for ( r = 0; r < 3; r++ )
                     if ( topay[r] )
                        cnt++;

                  int ps = 0;
                  for ( r = 0; r < 3; r++ )
                     if ( topay[r] ) {
                        ps++;
                        char txt3[100];
                        sprintf( txt3, "%d %s", topay[r], resourceNames[r] );
                        if ( ps>1 && ps < cnt )
                           strcat ( txt_topay, ", ");
                        if ( ps>1 && ps == cnt )
                           strcat ( txt_topay, " and ");
                        strcat ( txt_topay, txt3 );
                     }

                  char txt_got[100];
                  txt_got[0] = 0;
                  cnt = 0;
                  ps = 0;
                  for ( r = 0; r < 3; r++ )
                     if ( got[r] )
                        cnt++;

                  for ( r = 0; r < 3; r++ )
                     if ( got[r] ) {
                        ps++;
                        char txt3[100];
                        sprintf( txt3, "%d %s", got[r], resourceNames[r] );
                        if ( ps>1  && ps < cnt )
                           strcat ( txt_got, ", ");
                        if ( ps>1 && ps == cnt )
                           strcat ( txt_got, " and ");
                        strcat ( txt_got, txt3 );
                     }
                  if ( !txt_got[0] )
                     strcpy ( txt_got, "nothing" );

                  char* sp = getmessage( 10020 );
                  sprintf ( txt1b, sp, txt_topay, actmap->player[player].getName().c_str(), txt_got );
                  strcat ( text, txt1b );
               }
            }
      }
      if ( text[0] ) {
         char* sp = strdup ( text );
         new tmessage ( sp, 1 << targplayer );
      }
   }
}


void GetResourceCapacity :: checkbuilding ( pbuilding b )
{
   if ( b->color/8 == player ) {
      int t = b->gettank ( resourcetype );
      if ( t > maxint - got )
         got = maxint;
      else
         got += t;
   }
}


void GetResourceCapacity :: start ( int x, int y )
{
   if ( scope == 3 ) {
      got = maxint;
   } else
      MapNetwork :: start ( x, y );
}










int ResourceChangeNet :: getresource ( int x, int y, int resource, int _player, int _scope )
{
   if ( (_scope > 0)  &&  (actmap->isResourceGlobal ( resource )) )
      scope = 2;
   else
      scope = _scope;

   player = _player;
   resourcetype = resource;
   got = 0;
   start ( x , y );
   return got;
}




void GetResourcePlus :: checkvehicle ( pvehicle v )
{
   if ( v->generatoractive )
      got += v->typ->tank.energy;
}


void GetResourcePlus :: checkbuilding ( pbuilding bld )
{
   if ( bld->color/8 == player ) {
      Resources plus;
      bld->getresourceplus ( -1, &plus, 1 );
      got += plus.resource(resourcetype);
   }
}





void GetResourceUsage :: checkbuilding ( pbuilding b )
{
   if ( b->color/8 == player ) {
      Resources usage;
      b->getresourceusage ( &usage );
      got += usage.resource( resourcetype );
   }
}





int ReplayMapDisplay :: checkMapPosition ( int x, int y )
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

   if ((actmap->xpos != a) || (actmap->ypos != b))
      return 1;
   else
      return 0;
}


int  ReplayMapDisplay :: displayMovingUnit ( int x1,int y1, int x2, int y2, pvehicle vehicle, int height1, int height2, int fieldnum, int totalmove )
{
   if ( actmap->playerView < 0 )
      return 0;

   if ( fieldvisiblenow ( getfield ( x1, y1 ), actmap->playerView) || fieldvisiblenow ( getfield ( x2, y2 ), actmap->playerView)) {
      if ( checkMapPosition  ( x1, y1 ))
         displayMap();

      int fc = mapDisplay->displayMovingUnit ( x1, y1, x2, y2, vehicle, height1, height2, fieldnum, totalmove );
      if ( fc == 1 ) {
         mapDisplay->resetMovement();
         mapDisplay->displayMap();
      }

      return fc;
   } else
      return 0;
}

void ReplayMapDisplay :: displayPosition ( int x, int y )
{
   if ( fieldvisiblenow ( getfield ( x, y ), actmap->playerView )) {
      checkMapPosition  ( x, y );
      mapDisplay->displayPosition ( x, y );
   }
}

void ReplayMapDisplay :: removeActionCursor ( void )
{
   cursor.hide();
}

void ReplayMapDisplay :: displayActionCursor ( int x1, int y1, int x2, int y2, int secondWait )
{
   if ( x1 >= 0 && y1 >= 0 ) {
      int i = fieldvisiblenow ( getfield ( x1, y1 ), actmap->playerView );
      if( i ) {
         cursor.gotoxy ( x1, y1, i );
         wait();
      }
   }

   if ( x2 >= 0 && y2 >= 0 ) {
      int i = fieldvisiblenow ( getfield ( x2, y2 ), actmap->playerView );
      if( i ) {
         cursor.gotoxy ( x2, y2, i );
         if ( secondWait )
            wait();
      }
   }
}

void ReplayMapDisplay :: wait ( void )
{
   int t = ticker;
   while ( ticker < t + cursorDelay ) 
      releasetimeslice();
}




void logtoreplayinfo ( trpl_actions _action, ... )
{
   char action = _action;
   if ( actmap->replayinfo && actmap->replayinfo->actmemstream ) {
      pnstream stream = actmap->replayinfo->actmemstream;

      va_list paramlist;
      va_start ( paramlist, _action );

      if ( action == rpl_attack || action == rpl_reactionfire ) {
         int x1 =  va_arg ( paramlist, int );
         int y1 =  va_arg ( paramlist, int );
         int x2 =  va_arg ( paramlist, int );
         int y2 =  va_arg ( paramlist, int );
         int ad1 =  va_arg ( paramlist, int );
         int ad2 =  va_arg ( paramlist, int );
         int dd1 =  va_arg ( paramlist, int );
         int dd2 =  va_arg ( paramlist, int );
         int wpnum =  va_arg ( paramlist, int );
         int size = 9;
         stream->writedata2 ( action );
         stream->writedata2 ( size );
         stream->writedata2 ( x1 );
         stream->writedata2 ( y1 );
         stream->writedata2 ( x2 );
         stream->writedata2 ( y2 );
         stream->writedata2 ( ad1 );
         stream->writedata2 ( ad2 );
         stream->writedata2 ( dd1 );
         stream->writedata2 ( dd2 );
         stream->writedata2 ( wpnum );
      }

      if ( action == rpl_move ) {
         int x1 =  va_arg ( paramlist, int );
         int y1 =  va_arg ( paramlist, int );
         int x2 =  va_arg ( paramlist, int );
         int y2 =  va_arg ( paramlist, int );
         int nwid = va_arg ( paramlist, int );
         stream->writedata2 ( action );
         int size = 5;
         stream->writedata2 ( size );
         stream->writedata2 ( x1 );
         stream->writedata2 ( y1 );
         stream->writedata2 ( x2 );
         stream->writedata2 ( y2 );
         stream->writedata2 ( nwid );
      }
      if ( action == rpl_move2 ) {
         int x1 =  va_arg ( paramlist, int );
         int y1 =  va_arg ( paramlist, int );
         int x2 =  va_arg ( paramlist, int );
         int y2 =  va_arg ( paramlist, int );
         int nwid = va_arg ( paramlist, int );
         int height = va_arg ( paramlist, int );
         stream->writedata2 ( action );
         int size = 6;
         stream->writedata2 ( size );
         stream->writedata2 ( x1 );
         stream->writedata2 ( y1 );
         stream->writedata2 ( x2 );
         stream->writedata2 ( y2 );
         stream->writedata2 ( nwid );
         stream->writedata2 ( height );
      }

      if ( action == rpl_changeheight ) {
         int x1 =  va_arg ( paramlist, int );
         int y1 =  va_arg ( paramlist, int );
         int x2 =  va_arg ( paramlist, int );
         int y2 =  va_arg ( paramlist, int );
         int nwid = va_arg ( paramlist, int );
         int oldheight = va_arg ( paramlist, int );
         int newheight = va_arg ( paramlist, int );
         stream->writedata2 ( action );
         int size = 7;
         stream->writedata2 ( size );
         stream->writedata2 ( x1 );
         stream->writedata2 ( y1 );
         stream->writedata2 ( x2 );
         stream->writedata2 ( y2 );
         stream->writedata2 ( nwid );
         stream->writedata2 ( oldheight );
         stream->writedata2 ( newheight );
      }
      if ( action == rpl_convert ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         int col =  va_arg ( paramlist, int );
         stream->writedata2 ( action );
         int size = 3;
         stream->writedata2 ( size );
         stream->writedata2 ( x );
         stream->writedata2 ( y );
         stream->writedata2 ( col );
      }
      if ( action == rpl_buildobj || action == rpl_remobj ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         int id =  va_arg ( paramlist, int );
         stream->writedata2 ( action );
         int size = 3;
         stream->writedata2 ( size );
         stream->writedata2 ( x );
         stream->writedata2 ( y );
         stream->writedata2 ( id );
      }
      if ( action == rpl_buildtnk ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         int id =  va_arg ( paramlist, int );
         int col =  va_arg ( paramlist, int );
         stream->writedata2 ( action );
         int size = 4;
         stream->writedata2 ( size );
         stream->writedata2 ( x );
         stream->writedata2 ( y );
         stream->writedata2 ( id );
         stream->writedata2 ( col );
      }
      if ( action == rpl_putbuilding ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         int id = va_arg ( paramlist, int );
         int col =  va_arg ( paramlist, int );
         stream->writedata2 ( action );
         int size = 4;
         stream->writedata2 ( size );
         stream->writedata2 ( x );
         stream->writedata2 ( y );
         stream->writedata2 ( id );
         stream->writedata2 ( col );
      }
      if ( action == rpl_putmine ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         int col =  va_arg ( paramlist, int );
         int typ = va_arg ( paramlist, int );
         int strength = va_arg ( paramlist, int );
         stream->writedata2 ( action );
         int size = 5;
         stream->writedata2 ( size );
         stream->writedata2 ( x );
         stream->writedata2 ( y );
         stream->writedata2 ( col );
         stream->writedata2 ( typ );
         stream->writedata2 ( strength );
      }
      if ( action == rpl_removemine ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         stream->writedata2 ( action );
         int size = 2;
         stream->writedata2 ( size );
         stream->writedata2 ( x );
         stream->writedata2 ( y );
      }
      if ( action == rpl_removebuilding ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         stream->writedata2 ( action );
         int size = 2;
         stream->writedata2 ( size );
         stream->writedata2 ( x );
         stream->writedata2 ( y );
      }

      if ( action == rpl_produceunit ) {
         int id = va_arg ( paramlist, int );
         int col = va_arg ( paramlist, int );
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         int cl = va_arg ( paramlist, int );
         int nwid = va_arg ( paramlist, int );
         stream->writedata2 ( action );
         int size = 6;
         stream->writedata2 ( size );
         stream->writedata2 ( id );
         stream->writedata2 ( col );
         stream->writedata2 ( x );
         stream->writedata2 ( y );
         stream->writedata2 ( cl );
         stream->writedata2 ( nwid );
      }
      if ( action == rpl_removeunit ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         int nwid = va_arg ( paramlist, int );
         stream->writedata2 ( action );
         int size = 3;
         stream->writedata2 ( size );
         stream->writedata2 ( x );
         stream->writedata2 ( y );
         stream->writedata2 ( nwid );
      }
      if ( action == rpl_trainunit ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         int exp =  va_arg ( paramlist, int );
         int nwid = va_arg ( paramlist, int );
         stream->writedata2 ( action );
         int size = 4;
         stream->writedata2 ( size );
         stream->writedata2 ( x );
         stream->writedata2 ( y );
         stream->writedata2 ( exp );
         stream->writedata2 ( nwid );
      }
      if ( action == rpl_shareviewchange ) {
         stream->writedata2 ( action );
         if ( actmap->shareview ) {
            int size = sizeof ( tshareview ) / sizeof ( int );
            stream->writedata2 ( size );
            stream->writedata2 ( *actmap->shareview );
         } else {
            int size = 0;
            stream->writedata2 ( size );
         }
      }
      if ( action == rpl_alliancechange ) {
         stream->writedata2 ( action );
         int size = sizeof ( actmap->alliances ) / sizeof ( int );
         stream->writedata2 ( size );
         stream->writedata2 ( actmap->alliances );
      }
      if ( action == rpl_refuel ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         int nwid = va_arg ( paramlist, int );
         int pos = va_arg ( paramlist, int );
         int amnt = va_arg ( paramlist, int );
         stream->writedata2 ( action );
         int size = 5;
         stream->writedata2 ( size );
         stream->writedata2 ( x );
         stream->writedata2 ( y );
         stream->writedata2 ( nwid );
         stream->writedata2 ( pos );
         stream->writedata2 ( amnt );
      }
      if ( action == rpl_bldrefuel ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         int pos = va_arg ( paramlist, int );
         int amnt = va_arg ( paramlist, int );
         stream->writedata2 ( action );
         int size = 4;
         stream->writedata2 ( size );
         stream->writedata2 ( x );
         stream->writedata2 ( y );
         stream->writedata2 ( pos );
         stream->writedata2 ( amnt );
      }

      va_end ( paramlist );


   }
}


trunreplay :: trunreplay ( void )
{
   status = -1;
   movenum = 0;
}

int    trunreplay :: removeunit ( pvehicle eht, int nwid )
{
   if ( !eht )
      return 0;

    for ( int i = 0; i < 32; i++ )
       if ( eht->loading[i] )
          if ( eht->loading[i]->networkid == nwid ) {
             removevehicle ( &eht->loading[i] );
             // eht->loading[i] = NULL;
             return 1;
          } else {
             int ld = removeunit ( eht->loading[i], nwid );
             if ( ld )
                return ld;
          }
   return 0;
}

int  trunreplay :: removeunit ( int x, int y, int nwid )
{
   pfield fld  = getfield ( x, y );
   if ( !fld->vehicle )
      if ( fld->building ) {
         for ( int i = 0; i < 32; i++ ) {
            if ( fld->building->loading[i]->networkid == nwid ) {
               removevehicle ( &fld->building->loading[i] );
               // eht->loading[i] = NULL;
               return 1;
            } else {
               int ld = removeunit ( fld->building->loading[i], nwid );
               if ( ld )
                  return ld;
            }
         }
         return 0;
      } else
         return 0;
   else
      if ( fld->vehicle->networkid == nwid ) {
         removevehicle ( &fld->vehicle );
         return 1;
      } else 
         return removeunit ( fld->vehicle, nwid );
}


void trunreplay :: wait ( int t )
{
   if ( fieldvisiblenow ( getactfield(), actmap->playerView ))
    while ( ticker < t + CGameOptions::Instance()->replayspeed  && !keypress()) {
       /*
       tkey input;
       while (keypress ()) {
           input = r_key ( );
           if ( input == ct_'+' )
       }
       */
       releasetimeslice();
    }


}

/*
void trunreplay :: setcursorpos ( int x, int y )
{
   int i = fieldvisiblenow ( getfield ( x, y ), actmap->playerview );
   cursor.gotoxy ( x, y, i );
   if( i ) {
      lastvisiblecursorpos.x = x;
      lastvisiblecursorpos.y = y;
   }
}
*/


void trunreplay :: displayActionCursor ( int x1, int y1, int x2, int y2, int secondWait )
{
    ReplayMapDisplay rmd( &defaultMapDisplay );
    rmd.setCursorDelay ( CGameOptions::Instance()->replayspeed );
    rmd.displayActionCursor ( x1, y1, x2, y2, secondWait );
}

void trunreplay :: removeActionCursor ( void )
{
    ReplayMapDisplay rmd( &defaultMapDisplay );
    rmd.removeActionCursor (  );
}


void trunreplay :: execnextreplaymove ( void )
{
   if ( verbosity >= 8 )
     printf("executing replay move %d\n", movenum );

   displaymessage2("executing replay move %d\n", movenum );
   movenum++;
   int actaction = nextaction;
   if ( nextaction != rpl_finished ) {
      switch ( nextaction ) {
      case rpl_move: {
                        int x1, y1, x2, y2, nwid, size;
                        stream->readdata2 ( size );
                        stream->readdata2 ( x1 );
                        stream->readdata2 ( y1 );
                        stream->readdata2 ( x2 );
                        stream->readdata2 ( y2 );
                        stream->readdata2 ( nwid );
                        readnextaction();

                        pvehicle eht = actmap->getUnit ( x1, y1, nwid );
                        if ( eht ) {
                           ReplayMapDisplay rmd ( &defaultMapDisplay );
                           VehicleMovement vm ( &rmd, NULL );
                           vm.execute ( eht, -1, -1, 0 , -1, -1 );

                           int t = ticker;
                           vm.execute ( NULL, x2, y2, 2, -1, -1 );
                           wait( t );
                           vm.execute ( NULL, x2, y2, 3, -1, -1 );

                           if ( vm.getStatus() != 1000 )
                              eht = NULL;
                        }  

                        if ( !eht )
                           displaymessage("severe replay inconsistency:\nno vehicle for move1 command !", 1);
                     }
         break;
      case rpl_move2: {
                        int x1, y1, x2, y2, nwid, size, height;
                        stream->readdata2 ( size );
                        stream->readdata2 ( x1 );
                        stream->readdata2 ( y1 );
                        stream->readdata2 ( x2 );
                        stream->readdata2 ( y2 );
                        stream->readdata2 ( nwid );
                        stream->readdata2 ( height );
                        readnextaction();

                        pvehicle eht = actmap->getUnit ( x1, y1, nwid );
                        if ( eht ) {
                           ReplayMapDisplay rmd( &defaultMapDisplay );
                           VehicleMovement vm ( &rmd, NULL );
                           vm.execute ( eht, -1, -1, 0 , height, -1 );

                           int t = ticker;
                           vm.execute ( NULL, x2, y2, 2, -1, -1 );
                           wait( t );
                           vm.execute ( NULL, x2, y2, 3, -1, -1 );

                           if ( vm.getStatus() != 1000 )
                              eht = NULL;
                        }  

                        if ( !eht )
                           displaymessage("severe replay inconsistency:\nno vehicle for move2 command !", 1);
                     }
         break;
      case rpl_attack: {
                          int x1, y1, x2, y2, ad1, ad2, dd1, dd2, size, wpnum;
                          stream->readdata2 ( size );
                          stream->readdata2 ( x1 );
                          stream->readdata2 ( y1 );
                          stream->readdata2 ( x2 );
                          stream->readdata2 ( y2 );
                          stream->readdata2 ( ad1 );
                          stream->readdata2 ( ad2 );
                          stream->readdata2 ( dd1 );
                          stream->readdata2 ( dd2 );
                          stream->readdata2 ( wpnum );
                          readnextaction();

                          pfield fld = getfield ( x1, y1 );
                          pfield targ = getfield ( x2, y2 );
                          int attackvisible = fieldvisiblenow ( fld, actmap->playerView ) || fieldvisiblenow ( targ, actmap->playerView );
                          if ( fld && targ && fld->vehicle ) {
                             if ( targ->vehicle ) {
                                tunitattacksunit battle ( fld->vehicle, targ->vehicle, 1, wpnum );
                                battle.av.damage = ad1;
                                battle.dv.damage = dd1;
                                if ( attackvisible ) {
                                   displayActionCursor ( x1, y1, x2, y2, 0 );
                                   battle.calcdisplay ( ad2, dd2 );
                                   removeActionCursor();
                                } else {
                                   battle.calc ();
                                   battle.av.damage = ad2;
                                   battle.dv.damage = dd2;
                                }
                                battle.setresult ();
                                dashboard.x = 0xffff;
                             } else 
                             if ( targ->building ) {
                                tunitattacksbuilding battle ( fld->vehicle, x2, y2 , wpnum );
                                battle.av.damage = ad1;
                                battle.dv.damage = dd1;
                                if ( attackvisible ) {
                                   displayActionCursor ( x1, y1, x2, y2, 0 );
                                   battle.calcdisplay ( ad2, dd2 );
                                   removeActionCursor();
                                } else {
                                   battle.calc ();
                                   battle.av.damage = ad2;
                                   battle.dv.damage = dd2;
                                }
                                battle.setresult ();
                                dashboard.x = 0xffff;
                             } else
                             if ( targ->object ) {
                                tunitattacksobject battle ( fld->vehicle, x2, y2, wpnum );
                                if ( attackvisible ) {
                                   displayActionCursor ( x1, y1, x2, y2, 0 );
                                   battle.calcdisplay ( ad2, dd2 );
                                   removeActionCursor();
                                } else {
                                   battle.calc ();
                                   battle.av.damage = ad2;
                                   battle.dv.damage = dd2;
                                }
                                battle.setresult ();
                                dashboard.x = 0xffff;
                             }
                             computeview();
                             displaymap();
                          } else
                             displaymessage("severe replay inconsistency:\nno vehicle for attack command !", 1);

                      }
         break;
      case rpl_changeheight: {
                        int x1, y1, x2, y2, nwid, size, oldheight, newheight;
                        stream->readdata2 ( size );
                        stream->readdata2 ( x1 );
                        stream->readdata2 ( y1 );
                        stream->readdata2 ( x2 );
                        stream->readdata2 ( y2 );
                        stream->readdata2 ( nwid );
                        stream->readdata2 ( oldheight );
                        stream->readdata2 ( newheight );
                        readnextaction();

                        pvehicle eht = actmap->getUnit ( x1, y1, nwid );
                        if ( eht ) {
                           ReplayMapDisplay rmd( &defaultMapDisplay );
                           VehicleAction* va;
                           if ( newheight > oldheight )
                              va = new IncreaseVehicleHeight ( &rmd, NULL );
                           else
                              va = new DecreaseVehicleHeight ( &rmd, NULL );

                           va->execute ( eht, -1, -1, 0 , newheight, -1 );

                           int t = ticker;
                           va->execute ( NULL, x2, y2, 2, -1, -1 );
                           wait( t );
                           va->execute ( NULL, x2, y2, 3, -1, -1 );

                           if ( va->getStatus() != 1000 )
                              eht = NULL;

                           delete va;
                        }  

                        if ( !eht )
                           displaymessage("severe replay inconsistency:\nno vehicle for changeheight command !", 1);


                     }
         break;
      case rpl_convert: {
                           int x, y, col, size;
                           stream->readdata2 ( size );
                           stream->readdata2 ( x );
                           stream->readdata2 ( y );
                           stream->readdata2 ( col );
                           readnextaction();


                           pfield fld = getfield ( x, y );
                           if ( fld ) {
                              displayActionCursor ( x, y );
                              if ( fld->vehicle )
                                 fld->vehicle->convert ( col );
                              else
                                 if ( fld->building )
                                    fld->building->convert ( col );

                              computeview();
                              displaymap();
                              wait();
                              removeActionCursor();
                           } else
                              displaymessage("severe replay inconsistency:\nno vehicle for convert command !", 1);


                       }
         break;
      case rpl_remobj: 
      case rpl_buildobj: {
                           int x, y, id, size;
                           stream->readdata2 ( size );
                           stream->readdata2 ( x );
                           stream->readdata2 ( y );
                           stream->readdata2 ( id );
                           readnextaction();


                           pobjecttype obj = getobjecttype_forid ( id );

                           pfield fld = getfield ( x, y );
                           if ( obj && fld ) {
                              displayActionCursor ( x, y );

                              if ( actaction == rpl_remobj )
                                 fld->removeobject ( obj );      
                              else
                                 fld->addobject ( obj );

                              computeview();
                              displaymap();
                              wait();
                              removeActionCursor();
                           } else
                              displaymessage("severe replay inconsistency:\nCannot find Object to build/remove !", 1 );

                       }
         break;
      case rpl_buildtnk: {
                           int x, y, id, size, col;
                           stream->readdata2 ( size );
                           stream->readdata2 ( x );
                           stream->readdata2 ( y );
                           stream->readdata2 ( id );
                           stream->readdata2 ( col );
                           readnextaction();

                           pfield fld = getfield ( x, y );

                           pvehicletype tnk = getvehicletype_forid ( id );

                           if ( fld && tnk && !fld->vehicle ) {
                              displayActionCursor ( x, y );
                              pvehicle v = new Vehicle ( tnk, actmap, col );
                              v->xpos = x;
                              v->ypos = y;
                              fld->vehicle = v;

                              computeview();
                              displaymap();
                              wait();
                              removeActionCursor();
                           } else
                              displaymessage("severe replay inconsistency:\nCannot find Vehicle to build !", 1 );

                       }
         break;
      case rpl_putbuilding : {
                              int x, y, id, size, color;
                               stream->readdata2 ( size );
                               stream->readdata2 ( x );
                               stream->readdata2 ( y );
                               stream->readdata2 ( id );
                               stream->readdata2 ( color );
                               readnextaction();


                               pfield fld = getfield ( x, y );
    
                               pbuildingtype bld = getbuildingtype_forid ( id );
    
                               if ( bld && fld ) {
                                  displayActionCursor ( x, y );
                                  putbuilding2( x, y, color, bld );
                                  computeview();
                                  displaymap();
                                  wait();
                                  removeActionCursor();
                               } else
                                  displaymessage("severe replay inconsistency:\nCannot find building to build/remove !", 1 );
                            }
         break;
      case rpl_putmine: {
                           int x, y, col, typ, strength, size;
                           stream->readdata2 ( size );
                           stream->readdata2 ( x );
                           stream->readdata2 ( y );
                           stream->readdata2 ( col );
                           stream->readdata2 ( typ );
                           stream->readdata2 ( strength );
                           readnextaction();


                           pfield fld = getfield ( x, y );
                           if ( fld ) {
                              displayActionCursor ( x, y );
                              fld -> putmine ( col, typ, strength );
                              computeview();
                              displaymap();
                              wait();
                              removeActionCursor();
                           } else
                              displaymessage("severe replay inconsistency:\nno field for putmine command !", 1);

                       }
         break;
      case rpl_removemine: {
                           int x, y, size;
                           stream->readdata2 ( size );
                           stream->readdata2 ( x );
                           stream->readdata2 ( y );
                           readnextaction();


                           pfield fld = getfield ( x, y );
                           if ( fld ) {
                              displayActionCursor ( x, y );
                              fld -> removemine ( -1 );
                              computeview();
                              displaymap();
                              wait();
                              removeActionCursor ( );
                           } else
                              displaymessage("severe replay inconsistency:\nno field for remove mine command !", 1);

                       }
         break;
      case rpl_removebuilding: {
                           int x, y, size;
                           stream->readdata2 ( size );
                           stream->readdata2 ( x );
                           stream->readdata2 ( y );
                           readnextaction();


                           pfield fld = getfield ( x, y );
                           if ( fld && fld->building ) {
                              displayActionCursor ( x, y );
                              pbuilding bb = fld->building;
                              if ( bb->completion ) {
                                 bb->changecompletion ( -1 );
                              } else {
                                 removebuilding ( &bb );
                              }
                              computeview();
                              displaymap();
                              wait();
                              removeActionCursor();
                           } else
                              displaymessage("severe replay inconsistency:\nno building for removebuilding command !", 1);

                       }
         break;
      case  rpl_produceunit : {
                                 int id, col, x, y, cl, size, nwid;
                                 stream->readdata2 ( size );
                                 stream->readdata2 ( id );
                                 stream->readdata2 ( col );
                                 stream->readdata2 ( x );
                                 stream->readdata2 ( y );
                                 stream->readdata2 ( cl );
                                 stream->readdata2 ( nwid );
                                 readnextaction();

                                 pfield fld = getfield ( x, y );

                                 pvehicletype tnk = getvehicletype_forid ( id );
                                 if ( tnk && fld) {
                                    pvehicle eht = new Vehicle ( tnk, actmap, col / 8 );
                                    eht->klasse = cl;
                                    eht->xpos = x;
                                    eht->ypos = y;
                                    eht->setup_classparams_after_generation ();
                                    eht->tank.fuel = eht->typ->tank.fuel;
                                    eht->networkid = nwid;
   
                                    if ( fld->building ) {
                                       int i = 0;
                                       while ( fld->building->loading[i]) 
                                          i++;
                                       fld->building->loading[i] = eht;
                                    } else {
                                       displayActionCursor ( x, y );
                                       fld->vehicle = eht;
                                       computeview();
                                       displaymap();
                                       wait();
                                       removeActionCursor();
                                    }
                                 } else
                                    displaymessage("severe replay inconsistency:\nCannot find vehicle to build/remove !", 1 );

                              }
         break;
      case rpl_removeunit : {
                                 int x, y, size, nwid;
                                 stream->readdata2 ( size );
                                 stream->readdata2 ( x );
                                 stream->readdata2 ( y );
                                 stream->readdata2 ( nwid );
                                 readnextaction();

                                 if ( !removeunit ( x, y, nwid ))  
                                    displaymessage ( "severe replay inconsistency:\nCould not remove unit %d!", 1, nwid );
                              }
         break;
      case rpl_trainunit:{
                                 int x, y, size, exp, nwid;
                                 stream->readdata2 ( size );
                                 stream->readdata2 ( x );
                                 stream->readdata2 ( y );
                                 stream->readdata2 ( exp );
                                 stream->readdata2 ( nwid );
                                 readnextaction();


                                 pvehicle eht = actmap->getUnit ( x, y, nwid );
                                 if ( eht ) {
                                    eht->experience = exp;
                                 } else 
                                    displaymessage("severe replay inconsistency:\nno vehicle for trainunit command !", 1);

                              }
         break;
      case rpl_shareviewchange: {
                                 int size;
                                 pshareview sv = new tshareview;
                                 stream->readdata2 ( size );
                                 if ( size ) {
                                    stream->readdata2 ( *sv );
                                    if ( actmap->shareview ) 
                                       delete actmap->shareview;
                                    actmap->shareview = sv;
                                    actmap->shareview->recalculateview = 0;
                                 } else
                                    if ( actmap->shareview ) {
                                       delete actmap->shareview;
                                       actmap->shareview = NULL;
                                    }

                                 readnextaction();
                                 computeview();
                                 displaymap();
                              }
         break;
      case rpl_alliancechange: {
                                 int size;
                                 stream->readdata2 ( size );
                                 stream->readdata2 ( actmap->alliances );
                                 readnextaction();
                                 dashboard.x = 0xffff;
                              }
         break;
      case rpl_refuel : {
                                 int x, y, size, nwid, pos, amnt;
                                 stream->readdata2 ( size );
                                 stream->readdata2 ( x );
                                 stream->readdata2 ( y );
                                 stream->readdata2 ( nwid );
                                 stream->readdata2 ( pos );
                                 stream->readdata2 ( amnt );
                                 readnextaction();

                                 pvehicle eht = actmap->getUnit ( x, y, nwid );
                                 if ( eht ) {
                                    if ( pos < 16 )
                                       eht->ammo[pos] = amnt;
                                     else {
                                        switch ( pos ) {
                                        case 1000: eht->tank.energy = amnt;
                                           break;
                                        case 1001: eht->tank.material = amnt;
                                           break;
                                        case 1002: eht->tank.fuel = amnt;
                                           break;
                                        } /* endswitch */
                                     }
                                 } else 
                                    displaymessage("severe replay inconsistency:\nno vehicle for refuel-unit command !", 1);
                              }
         break;
      case rpl_bldrefuel : {
                                 int x, y, size, pos, amnt;
                                 stream->readdata2 ( size );
                                 stream->readdata2 ( x );
                                 stream->readdata2 ( y );
                                 stream->readdata2 ( pos );
                                 stream->readdata2 ( amnt );
                                 readnextaction();

                                 pbuilding bld = actmap->getField(x,y)->building;
                                 if ( bld ) {
                                    if ( pos < 16 )
                                        bld->munition[pos] = amnt;
                                     else
                                        bld->getResource ( pos-1000, amnt, 0 );
                                 } else
                                    displaymessage("severe replay inconsistency:\nno building for refuel-unit command !", 1);
                              }
         break;
      default:{
                 int size, temp;
                 stream->readdata2 ( size );
                 for ( int i = 0; i< size; i++ )
                    stream->readdata2 ( temp );

                 readnextaction();

              }
        break;
      } /* endswitch */

   } else {
      status = 10;
      dashboard.x = -1;
   }

}

void trunreplay :: readnextaction ( void )
{
      if ( stream->dataavail () )
         stream->readdata2 ( nextaction );
      else
         nextaction = rpl_finished;
}


preactionfire_replayinfo trunreplay::getnextreplayinfo ( void )
{
   if ( nextaction == rpl_reactionfire ) {
      preactionfire_replayinfo reac = new treactionfire_replayinfo;
      int size;
      stream->readdata2 ( size );

      stream->readdata2 ( reac->x1 );
      stream->readdata2 ( reac->y1 );
      stream->readdata2 ( reac->x2 );
      stream->readdata2 ( reac->y2 );
      stream->readdata2 ( reac->ad1 );
      stream->readdata2 ( reac->ad2 );
      stream->readdata2 ( reac->dd1 );
      stream->readdata2 ( reac->dd2 );
      stream->readdata2 ( reac->wpnum );

      readnextaction();

      return reac;
   } else
      return NULL;

}


int  trunreplay :: run ( int player )
{
   if ( status < 0 )
      firstinit ( );


   cursor.hide();
   movenum = 0;

   actplayer = actmap->actplayer;

   orgmap = actmap;
   actmap = NULL;

   loadreplay ( orgmap->replayinfo->map[player]  );

   actmap->playerView = actplayer;

   tmemorystream guidatastream ( orgmap->replayinfo->guidata [ player ], 1 );
   stream = &guidatastream;

   if ( stream->dataavail () )
      stream->readdata2 ( nextaction );
   else
      nextaction = rpl_finished;

//   orgmap.replayinfo->actmemstream = stream;

   npush (actgui);
   actgui = &gui;
   actgui->restorebackground();

   actmap->xpos = orgmap->cursorpos.position[ actplayer ].sx;
   actmap->ypos = orgmap->cursorpos.position[ actplayer ].sy;

   cursor.gotoxy ( orgmap->cursorpos.position[ actplayer ].cx, orgmap->cursorpos.position[ actplayer ].cy , 0);
   // lastvisiblecursorpos.x = orgmap.cursorpos.position[ actplayer ].cx;
   // lastvisiblecursorpos.y = orgmap.cursorpos.position[ actplayer ].cy;


   if ( stream->dataavail () )
      status = 1;
   else 
      status = 11;

   computeview();
   displaymap ();

   dashboard.x = 0xffff;
   mousevisible( true );
//   cursor.show();

   cursor.checkposition( getxpos(), getypos() );
   do {
       if ( status == 2 ) {
          execnextreplaymove ( );
         /*
          if ( getxpos () != lastvisiblecursorpos.x || getypos () != lastvisiblecursorpos.y )
             setcursorpos ( lastvisiblecursorpos.x, lastvisiblecursorpos.y );
         */
       }

       if (nextaction == rpl_finished  || status != 2) {
          if ( !cursor.an )
             cursor.show();
       } else
          if ( cursor.an )
             cursor.hide();


       tkey input;
       while (keypress ()) {
           mainloopgeneralkeycheck ( input );
       }

       mainloopgeneralmousecheck ();

   } while ( status > 0  &&  status < 100 ) ;

   actgui->restorebackground();

   delete actmap;
   actmap = orgmap;

   npop ( actgui );

   int st = status;
   status = 0;

   cursor.gotoxy ( orgmap->cursorpos.position[ actplayer ].cx, orgmap->cursorpos.position[ actplayer ].cy );
   dashboard.x = 0xffff;

   if ( st == 101 )
      return 1;
   else
      return 0;
}

void trunreplay :: firstinit ( void )
{
    gui.init ( hgmp->resolutionx, hgmp->resolutiony );
    gui.starticonload();
    status = 0;
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



