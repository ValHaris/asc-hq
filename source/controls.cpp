//     $Id: controls.cpp,v 1.53 2000-08-02 15:52:42 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.52  2000/08/02 10:28:23  mbickel
//      Fixed: generator vehicle not working
//      Streams can now report their name
//      Field information shows units filename
//
//     Revision 1.51  2000/08/02 08:47:56  mbickel
//      Fixed: Mineral resources where visible for all players
//
//     Revision 1.50  2000/07/31 19:16:33  mbickel
//      Improved handing of multiple directories
//      Fixed: wind direction not displayed when cycling through wind heights
//      Fixed: oil rig not working
//      Fixed: resources becomming visible when checking mining station status
//      Fixed: division by zero when moving unit without fuel consumption
//
//     Revision 1.49  2000/07/29 14:54:12  mbickel
//      plain text configuration file implemented
//
//     Revision 1.48  2000/07/23 17:59:51  mbickel
//      various AI improvements
//      new terrain information window
//
//     Revision 1.47  2000/07/22 18:57:56  mbickel
//      New message during save operation
//      Weapon efficiency displayed did not correspond to mouse position when
//        first weapon was service
//
//     Revision 1.46  2000/07/16 16:15:49  mbickel
//      Building: ammotransfer improved
//
//     Revision 1.45  2000/07/16 14:57:41  mbickel
//      Datafile versioning
//
//     Revision 1.44  2000/07/16 14:19:59  mbickel
//      AI has now some primitive tactics implemented
//      Some clean up
//        moved weapon functions to attack.cpp
//      Mount doesn't modify PCX files any more.
//
//     Revision 1.43  2000/07/10 15:21:29  mbickel
//      Fixed crash in replay (alliancechange)
//      Fixed some movement problems when moving units out of transports / buildings
//      Removed save game description dialog
//
//     Revision 1.42  2000/07/06 11:07:26  mbickel
//      More AI work
//      Started modularizing the attack formula
//
//     Revision 1.41  2000/07/04 17:10:13  mbickel
//      Fixed crash in replay with invalid removebuilding coordinates
//
//     Revision 1.40  2000/07/02 21:04:11  mbickel
//      Fixed crash in Replay
//      Fixed graphic errors in replay
//
//     Revision 1.39  2000/06/28 18:30:58  mbickel
//      Started working on AI
//      Started making loaders independent of memory layout
//      Destroyed buildings can now leave objects behind.
//
//     Revision 1.38  2000/06/23 09:24:15  mbickel
//      Fixed crash in replay
//      enabled cursor movement in stredit
//
//     Revision 1.37  2000/06/19 20:05:04  mbickel
//      Fixed crash when transfering ammo to vehicle with > 8 weapons
//
//     Revision 1.36  2000/06/09 13:12:23  mbickel
//      Fixed tribute function and renamed it to "transfer resources"
//
//     Revision 1.35  2000/06/08 21:03:39  mbickel
//      New vehicle action: attack
//      wrote documentation for vehicle actions
//
//     Revision 1.34  2000/06/05 18:21:21  mbickel
//      Fixed a security hole which was opened with the new method of loading
//        mail games by command line parameters
//
//     Revision 1.33  2000/06/04 21:39:18  mbickel
//      Added OK button to ViewText dialog (used in "About ASC", for example)
//      Invalid command line parameters are now reported
//      new text for attack result prediction
//      Added constructors to attack functions
//
//     Revision 1.32  2000/05/30 18:39:21  mbickel
//      Added support for multiple directories
//      Moved DOS specific files to a separate directory
//
//     Revision 1.31  2000/05/23 20:40:37  mbickel
//      Removed boolean type
//
//     Revision 1.30  2000/05/22 15:40:31  mbickel
//      Included patches for Win32 version
//
//     Revision 1.29  2000/05/18 14:14:47  mbickel
//      Fixed bug in movemalus calculation for movement
//      Added "view movement range"
//
//     Revision 1.28  2000/05/10 21:13:42  mbickel
//      Improved error messages for mail game loader
//
//     Revision 1.27  2000/05/10 19:55:42  mbickel
//      Fixed empty loops when waiting for mouse events
//
//     Revision 1.26  2000/05/08 20:39:00  mbickel
//      Cleaned up makefiles
//      Fixed bug in mount: crash when file > 1 MB
//      Fixed bug in visibility calculation
//
//     Revision 1.25  2000/05/07 12:12:12  mbickel
//      New mouse option dialog
//      weapon info can now be displayed by clicking on a unit
//
//     Revision 1.24  2000/05/02 16:20:52  mbickel
//      Fixed bug with several simultaneous vehicle actions running
//      Fixed graphic error at ammo transfer in buildings
//      Fixed ammo loss at ammo transfer
//      Movecost is now displayed for mines and repairs
//      Weapon info now shows unhittable units
//
//     Revision 1.23  2000/04/27 17:59:19  mbickel
//      Updated Kdevelop project file
//      Fixed some graphical errors
//
//     Revision 1.22  2000/04/27 16:25:16  mbickel
//      Attack functions cleanup
//      New vehicle categories
//      Rewrote resource production in ASC resource mode
//      Improved mine system: several mines on a single field allowed
//      Added unitctrl.* : Interface for vehicle functions
//        currently movement and height change included
//      Changed timer to SDL_GetTicks
//
//     Revision 1.21  2000/03/16 14:06:51  mbickel
//      Added unitset transformation to the mapeditor
//
//     Revision 1.20  2000/02/24 10:54:07  mbickel
//      Some cleanup and bugfixes
//
//     Revision 1.19  2000/01/31 16:08:38  mbickel
//      Fixed crash in line
//      Improved error handling in replays
//
//     Revision 1.18  2000/01/25 19:28:09  mbickel
//      Fixed bugs:
//        invalid mouse buttons reported when moving the mouse
//        missing service weapon in weapon information
//        invalid text displayed in ammo production
//        undamaged units selected in repair vehicle function
//
//      speed up when playing weapon sound
//
//     Revision 1.17  2000/01/24 17:35:41  mbickel
//      Added dummy routines for sound under DOS
//      Cleaned up weapon specification
//
//     Revision 1.16  2000/01/19 22:14:17  mbickel
//      Fixed:
//        - crash in replay
//        - invalid character highliting in showtext3
//
//     Revision 1.15  2000/01/04 19:43:47  mbickel
//      Continued Linux port
//
//     Revision 1.14  2000/01/01 19:04:15  mbickel
//     /tmp/cvsVhJ4Z3
//
//     Revision 1.13  1999/12/30 20:30:24  mbickel
//      Improved Linux port again.
//
//     Revision 1.12  1999/12/29 17:38:07  mbickel
//      Continued Linux port
//
//     Revision 1.11  1999/12/28 21:02:43  mbickel
//      Continued Linux port
//      Added KDevelop project files
//
//     Revision 1.10  1999/12/27 12:59:45  mbickel
//      new vehicle function: each weapon can now be set to not attack certain
//                            vehicles
//
//     Revision 1.9  1999/12/14 20:23:49  mbickel
//      getfiletime now works on containerfiles too
//      improved BI3 map import tables
//      various bugfixes
//
//     Revision 1.8  1999/12/07 21:57:52  mbickel
//      Fixed bugs in Weapon information window
//      Added vehicle function "no air refuelling"
//
//     Revision 1.7  1999/11/25 22:00:02  mbickel
//      Added weapon information window
//      Added support for primary offscreen frame buffers to graphics engine
//      Restored file time handling for DOS version
//
//     Revision 1.6  1999/11/23 21:07:23  mbickel
//      Many small bugfixes
//
//     Revision 1.5  1999/11/22 18:26:59  mbickel
//      Restructured graphics engine:
//        VESA now only for DOS
//        BASEGFX should be platform independant
//        new interface for initialization
//      Rewrote all ASM code in C++, but it is still available for the Watcom
//        versions
//      Fixed bugs in RLE decompression, BI map importer and the view calculation
//
//     Revision 1.4  1999/11/18 17:31:02  mbickel
//      Improved BI-map import translation tables
//      Moved macros to substitute Watcom specific routines into global.h
//
//     Revision 1.3  1999/11/16 17:03:58  mbickel
//     Made ASC compilable for DOS again :-)
//     Merged all the bug fixes in that I did last week
//
//     Revision 1.2  1999/11/16 03:41:16  tmwilson
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

#include "config.h"
#include <stdio.h>           
#include <string.h>
#include <math.h>
#include <stdarg.h>

#include "tpascal.inc"
#include "typen.h"
#include "keybp.h"
#include "basegfx.h"
#include "newfont.h"
#include "spfst.h"
#include "loaders.h"
#include "misc.h"
#include "controls.h"
#include "network.h"
#include "mousehnd.h"
#include "loadpcx.h"
#include "stack.h"
#include "dlg_box.h"
#include "dialog.h"
#include "attack.h"
#include "building.h"
#include "gamedlg.h"
#include "missions.h"
#include "timer.h"
#include "sg.h"
#include "weather.h"
#include "gameoptions.h"

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
      getbuildingfieldcoordinates ( bld, bld->typ->entry.x, bld->typ->entry.y, x, y );
      initsuche( x, y, 1, 1 ); 
   
      startsuche();
   }
   if ( numberoffields )
      moveparams.movestatus = 130;
}

int searchexternaltransferfields ( pbuilding bld )
{
   tsearchexternaltransferfields setf;
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
                             };

  class   tsearchdestructbuildingfields : public tsearchfields {
                      public:
                                char                    numberoffields;
                                pfield                   startfield;
                                void                    initdestructbuilding( int x, int y );
                                virtual void            testfield ( void );
                             };

  class   tsearchforminablefields: public tsearchfields {
                                int shareview;
                      public:
                                int                     numberoffields;
                                void                    run ( pvehicle     eht );
                                virtual void            testfield ( void );
                       };


void         tsearchforminablefields::testfield(void)
{ 

  if ((xp >= 0) && (yp >= 0) && (xp < actmap->xsize) && (yp < actmap->ysize)) 
     if ( beeline ( xp, yp, startx, starty ) <= maxdistance * minmalq ) {
        pfield fld = getfield ( xp, yp );
        if ( !fld->building  ||  fld->building->color == actmap->actplayer*8  ||  fld->building->color == 8*8)
           if ( !fld->vehicle  ||  fld->vehicle->color == actmap->actplayer*8 ||  fld->vehicle->color == 8*8) {
              if ( !fld->resourceview )
                 fld->resourceview = new tresourceview;

              for ( int c = 0; c < 8; c++ )
                 if ( shareview & (1 << c) ) {
                    fld->resourceview->visible |= ( 1 << c );
                    fld->resourceview->fuelvisible[c] = fld->fuel;
                    fld->resourceview->materialvisible[c] = fld->material;
                 }
           }
     } 
} 



void         tsearchforminablefields::run( pvehicle     eht )
{ 
   if ( (eht->functions & cfmanualdigger) && !(eht->functions & cfautodigger) ) 
      if ( eht->attacked || 
          (eht->typ->wait && (eht->movement < eht->typ->movement[log2(eht->height)])) ||
          (eht->movement < searchforresorcesmovedecrease )) {
         dispmessage2(311,"");
         return;
      } 

   shareview = 1 << ( eht->color / 8);
   if ( actmap->shareview )
      for ( int i = 0; i < 8; i++ )
         if ( i*8 != eht->color )
            if ( actmap->player[i].existent )
               if ( actmap->shareview->mode[eht->color/8][i] == sv_shareview )
                  shareview += 1 << i;

   numberoffields = 0;
   initsuche(eht->xpos, eht->ypos,eht->typ->digrange,1); 
   if ( eht->typ->digrange ) 
      startsuche();

   xp = eht->xpos;
   yp = eht->ypos;
   dist = 0;
   testfield();

   if ( (eht->functions & cfmanualdigger) && !(eht->functions & cfautodigger) ) 
      eht->movement -= searchforresorcesmovedecrease;

   if ( !showresources && (eht->functions & cfmanualdigger) && !(eht->functions & cfautodigger))
      showresources = 1;
} 
                                      

void   searchforminablefields ( pvehicle eht )
{
    tsearchforminablefields sfmf;
    sfmf.run( eht );
}




void         tsearchputbuildingfields::initputbuilding( word x, word y, pbuildingtype building )
{ 
  pvehicle     eht; 

   eht = getfield(x,y)->vehicle; 
   if (eht->attacked || (eht->typ->wait && (eht->movement < eht->typ->movement[log2(eht->height)]))) {
      dispmessage2(302,""); 
      return;
   } 
   actmap->cleartemps(7); 
   initsuche(x,y,1,1); 
   bld = building; 
   numberoffields = 0; 
   startfield = getfield(x,y); 
   startsuche(); 
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
  byte         x1, y1; 
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

   tsearchputbuildingfields   spbf;
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
      
         if (eht->material < bld->productioncost.material * mf / 100 ) { 
            displaymessage("not enough material!",1); 
            eht->material = 0; 
         } 
         else 
            eht->material -= bld->productioncost.material * mf / 100; 


         if (eht->fuel < bld->productioncost.fuel * ff / 100) { 
            displaymessage("not enough fuel!",1); 
            eht->fuel = 0; 
         } 
         else 
            eht->fuel -= bld->productioncost.fuel * ff / 100; 

         moveparams.movestatus = 0; 
         eht->movement = 0;
         eht->attacked = true; 
         computeview(); 
      } 
} 








void         tsearchdestructbuildingfields::initdestructbuilding( int x, int y )
{ 
   pvehicle     eht = getfield(x,y)->vehicle; 
   if (eht->attacked || (eht->typ->wait && (eht->movement < eht->typ->movement[log2(eht->height)]))) {
      dispmessage2(305,NULL); 
      return;
   } 
   actmap->cleartemps(7); 
   initsuche(x,y,1,1); 
   numberoffields = 0; 
   startfield = getfield(x,y); 
   startsuche(); 
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
   tsearchdestructbuildingfields   sdbf;
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

         eht->material += bld->productioncost.material * (100 - bb->damage) / destruct_building_material_get / 100;
         if ( eht->material > eht->typ->material )
            eht->material = eht->typ->material;

         eht->movement = 0;
         eht->attacked = 1;
         eht->fuel -= destruct_building_fuel_usage * eht->typ->fuelconsumption;
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






void         trefuelvehicle::testfield(void)
{ 
   integer      i,j;
   pfield        fld;

   if ((xp >= 0) && (yp >= 0) && (xp < actmap->xsize) && (yp < actmap->ysize)) { 
      if (beeline(startx,starty,xp,yp) <= 15) {
         int targheight = 0;
         for (i = 0; i < actvehicle->typ->weapons->count ; i++) 
            if ( actvehicle->typ->weapons->weapon[i].sourceheight & actvehicle->height )
               if ( actvehicle->typ->weapons->weapon[i].service() )
                  targheight = actvehicle->typ->weapons->weapon[i].targ;


         for (i = 0; i < actvehicle->typ->weapons->count ; i++) 
            if ( actvehicle->typ->weapons->weapon[i].sourceheight & actvehicle->height ) 

               if (( actvehicle->typ->weapons->weapon[i].service()                    &&  mode == 1 ) || 
                   ((actvehicle->typ->weapons->weapon[i].canRefuel() || actvehicle->typ->weapons->weapon[i].service() )  && ( mode==2 || mode==3 ))) {
                  fld = getfield(xp,yp); 
                  if ( fld->vehicle ) 
                     if ( !(fld->vehicle->functions & cfnoairrefuel) || fld->vehicle->height <= chfahrend )
                        if (getdiplomaticstatus(fld->vehicle->color) == capeace) 
                           if ( fld->vehicle->height & targheight ) {
                              if ( actvehicle->typ->weapons->weapon[i].canRefuel() && mode != 1 )
                                 if (fld->vehicle->typ->weapons->count > 0) 
                                    for (j = 0; j < fld->vehicle->typ->weapons->count ; j++) 
                                       if ( fld->vehicle->typ->weapons->weapon[j].getScalarWeaponType() == actvehicle->typ->weapons->weapon[i].getScalarWeaponType()                                                        
                                            && actvehicle->typ->weapons->weapon[i].requiresAmmo() )
                                          if ((fld->vehicle->typ->weapons->weapon[j].count > fld->vehicle->ammo[j]) || (mode == 3)) 
                                             { 
                                                fld->a.temp = 2; 
                                                numberoffields++;
                                             }                        
                              if ( actvehicle->typ->weapons->weapon[i].service() )
                                 if ( actvehicle->height <= chfahrend || (actvehicle->height == fld->vehicle->height)) {
                                    if ( ((actvehicle->typ->tank > 0) && (fld->vehicle->typ->tank > 0)
                                       && ((fld->vehicle->typ->tank > fld->vehicle->fuel) || (mode == 3))
                                       &&  (actvehicle->functions & cffuelref) )
                                       ||
                                       ((actvehicle->typ->material > 0) && (fld->vehicle->typ->material > 0)
                                       && ((fld->vehicle->typ->material > fld->vehicle->material) || (mode == 3))
                                       && (actvehicle->functions & cfmaterialref ) ) )
                                       { 
				          if ( mode != 1 ) {
                                             fld->a.temp = 2; 
                                             numberoffields++;
					  }
                                       } 
      
                                    if ( actvehicle->functions & cfrepair )
                                       if ( actvehicle->fuel && actvehicle->material ) 
                                          // if ( fld->vehicle->movement >= movement_costtype_for_repaired_unit )
                                             if ( fld->vehicle->damage ) 
                                                { 
                                                   fld->a.temp = 2; 
                                                   numberoffields++;
                                                } 
                                 } 
                           } 
               } 
      }
   } 
} 


void         trefuelvehicle::initrefuelling( word xp1, word yp1, char md )   /*  md: 1 reparieren    */ 
{                                                                            /*      2 vollfÅllen    */
  byte         f, a;                                                         /*      3 dialogfÅllen  */
 
   mode = 0; 
   if (moveparams.movestatus == 0) { 
      numberoffields = 0; 
      actvehicle = getactfield()->vehicle; 
      if (actvehicle == NULL) return;
      if (actvehicle->typ->weapons->count == 0) return;
      if (md == 1) { 
         f = 0; 
         if (actvehicle->functions & cfrepair) 
            if ( actvehicle->movement >= movement_cost_for_repairing_unit )
               for (a = 0; a < actvehicle->typ->weapons->count ; a++) { 
                  if ( actvehicle->typ->weapons->weapon[a].service() ) 
                     if ( actvehicle->material ) 
                        if ( actvehicle->fuel ) 
                           f++;
               } 
         if (f < 1) return;
      } 
      if ((md == 2) || (md == 3)) { 
         f = 0; 
         for (a = 0; a < actvehicle->typ->weapons->count ; a++) { 
            if ( actvehicle->typ->weapons->weapon[a].service() )  {
               if ((actvehicle->typ->tank > 0) && (actvehicle->functions & cffuelref) ) {
                  if (md == 2) { 
                     if (actvehicle->fuel > 0) 
                        f++;
                  } 
                  else 
                     f++;
               } 
               if ((actvehicle->typ->material > 0) && (actvehicle->functions & cfmaterialref) ) {
                  if (md == 2) { 
                     if (actvehicle->material > 0) 
                        f++; 
                  } 
                  else 
                     f++; 
               } 
            }                                       
            if ( actvehicle->typ->weapons->weapon[a].canRefuel() )
               if (md == 2) { 
                  if (actvehicle->ammo[a] > 0) 
                     f++; 
               } 
               else 
                  f++; 
         } 
         if (f == 0) return;

      } 
      initsuche(xp1,yp1,1,1); 
      moveparams.movesx = xp1;
      moveparams.movesy = yp1; 
      
      mode = md; 
   } 
} 


void         trefuelvehicle::startsuche(void)
{ 
   if ((mode == 1) || ( mode == 3 )) {
      actmap->cleartemps(7); 
      tsearchfields::startsuche();
      if (numberoffields > 0) { 
         if ((mode == 2) || (mode == 3))
            moveparams.movestatus = 65; 
         else 
            moveparams.movestatus = 66; 

      } 
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
  byte         i; 

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
   if (eht->movement < mineputmovedecrease) { 
      mienenlegen = false; 
      mienenraeumen = false; 
   } 
   if (mienenlegen || mienenraeumen) 
      initsuche(getxpos(),getypos(),1,1);
} 


void         tputmine::run(void)
{ 
   if ((mienenlegen || mienenraeumen)) { 
      startsuche(); 
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
      tputmine ptm; 
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
                        eht->movement -= mineputmovedecrease;
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








void         refuelvehicle(byte         b)
{ 
   pvehicle     actvehicle; 

   if (moveparams.movestatus == 0) { 
      trefuelvehicle rfe;
      rfe.initrefuelling(getxpos(),getypos(),b); 
      rfe.startsuche(); 
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
               actvehicle->repairunit( getactfield()->vehicle ); 
               actmap->cleartemps(7); 
               moveparams.movestatus = 0; 
            } 
      dashboard.x = 0xffff; 
   } 

} 


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
        pguihost oldgui;
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


void    getobjbuildcosts ( pobjecttype obj, pfield fld, tresources* resource, int* movecost )
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
      for ( int i = 0; i < 3; i++ )
         resource->resource[i] = obj->buildcost.resource[i]; // * costmultiply / 8;

      #ifdef HEXAGON 
       mvcost = obj->build_movecost;
      #else
       mvcost = obj->movecost;
      #endif

   } 

   else { 
      for ( int i = 0; i < 3; i++ )
         resource->resource[i] = obj->removecost.resource[i] * bridgemultiple / 8;

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
                      tresources cost;
                      getobjbuildcosts ( objtype, fld, &cost, &movecost );
                      if ( actvehicle->energy   >= cost.a.energy   && 
                           actvehicle->material >= cost.a.material && 
                           actvehicle->fuel     >= cost.a.fuel     &&
                           actvehicle->movement >= movecost ) {
      
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
   startsuche(); 
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
   startsuche(); 
   if (numberoffields > 0)  
      moveparams.movestatus = 120; 
   
} 




void         setspec( pobjecttype obj )
{ 

   if (moveparams.movestatus == 0) { 

      tbuildstreet buildstreet; 

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

         int stat = 0;
         if ( actmap->objectcrc ) {
            if ( actmap->objectcrc->speedcrccheck->checkobj2 ( obj ))
               stat = 1;
         } else
            stat = 1;

         if ( stat ) {
            int movecost;
            tresources cost;
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
   
            eht->fuel -= cost.a.fuel;
            eht->energy -= cost.a.energy;
            eht->material -= cost.a.material;
            eht->movement -= movecost;
   
         }

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

      SearchVehicleConstructionFields svcf; 

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

         int stat = 0;
         if ( actmap->objectcrc ) {
            if ( actmap->objectcrc->speedcrccheck->checkunit2 ( tnk ))
               stat = 1;
         } else
            stat = 1;

         if ( stat ) {

            int x = getxpos();
            int y = getypos();
            eht->constructvehicle ( tnk, x, y );
            logtoreplayinfo ( rpl_buildtnk, x, y, tnk->id, moveparams.vehicletomove->color/8 );
            computeview();
   
         }

         build_vehicles_reset();
      } 
      dashboard.x = 0xffff;

   } 
} 






void tvehicle :: addview ( void )
{
   tcomputevehicleview bes; 
   bes.init( this, +1 ); 
   bes.startsuche(); 
}
                 
void tvehicle :: removeview ( void )
{
   tcomputevehicleview bes; 
   bes.init( this, -1 ); 
   bes.startsuche(); 
}


void tbuilding :: addview ( void )
{
   tcomputebuildingview bes; 
   bes.init( this, +1 ); 
   bes.startsuche(); 
}
                 
void tbuilding :: removeview ( void )
{
   tcomputebuildingview bes; 
   bes.init( this, -1 ); 
   bes.startsuche(); 
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

int  evaluatevisibilityfield ( pfield fld, int player, int add )
{
   int originalVisibility = (fld->visible >> (player * 2)) & 3;
   if ( originalVisibility >= visible_ago) 
        setvisibility(&fld->visible,visible_ago, player);

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


      if ( fld->object && fld->object->mine ) {
         if ( mine  ||  fld->mineowner() == player) {
            setvisibility(&fld->visible, visible_all, player);
            return originalVisibility != visible_all;
         } else {
            setvisibility(&fld->visible,visible_now, player);
            return originalVisibility != visible_now;
         }
      } else
        if (( fld->vehicle  && ( fld->vehicle->color  == player * 8 )) ||
            ( fld->vehicle  && ( fld->vehicle->height  < chschwimmend ) && sonar ) ||
            ( fld->building && ( fld->building->typ->buildingheight < chschwimmend ) && sonar ) ||
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
                fieldsChanged += evaluatevisibilityfield ( &actmap->field[i], player, add );
         else
            for ( int i = 0; i < nm; i++ )
                evaluatevisibilityfield ( &actmap->field[i], player, add );
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
      dx:=(2*x2+(y2 and 1)) -(2*x+(y and 1));   { ???????·
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

#else
class tdrawgettempline  {
                  int freefields;
                  int num;
                  int sx, sy;

                  static int initialized;
                  static double dirs[ sidenum ];
                  static double offset;

                  void init ( void );

               public:
                  int tempsum;
                  tdrawgettempline ( int _freefields ) 
                    { 
                       tempsum = 0; 
                       freefields = _freefields;
                       num = 0;
                    };

                  void start ( int x1, int y1, int x2, int y2 );

                  virtual void putpix8 ( int x, int y )
                     {
                        if ( !getfield ( x, y ) ){
                           x = sx;
                           y = sy;
                           return;
                        }
                        if ( num >= freefields )
                           tempsum += getfield ( x, y )->getjamming();
                        num++;
/*                        
                        getfield( x,y )->temp = 100;
                        displaymap();
                        cursor.gotoxy ( x , y );
                                                */
                     };
                   double winkel ( int x, int y );
                   int winkelcomp ( double w1, double w2 )
                   {
                      double pi = 3.141592654;
                      double delta = w2 - w1;
                      // printf(" wc : %f %f %f \n", w1, w2, delta );
                      if ( delta > -0.0001 && delta < 0.0001 )
                         return 0;

                      if ( delta > 0 ) {
                         if ( delta <= pi )
                            return 1;
                         else
                            return -1;
                      }

                      if ( delta < 0 ) {
                         if ( delta < -pi )
                            return 1;
                         else
                            return -1;
                      }

                      return 0;
                   }
              };

int tdrawgettempline :: initialized = 0;
double tdrawgettempline :: dirs[ sidenum ];
double tdrawgettempline :: offset;

void tdrawgettempline :: init ( void )
{ 
   if ( !initialized ) {
      offset = 0;
      sx = 10;
      sy = 10;
      int i;

      for ( i = 0; i < sidenum; i++ ) {
         sx = 10;
         sy = 10;
         getnextfield ( sx, sy, i );
         dirs[i] = winkel ( 10, 10 );
      }
      offset = dirs[0];

      for ( i = 0; i < sidenum; i++ ) {
         sx = 10;
         sy = 10;
         getnextfield ( sx, sy, i );
         dirs[i] = winkel ( 10, 10 );
      }

      initialized = 1;
   }
}
 

double tdrawgettempline :: winkel ( int x, int y )
{
   int xp2 = sx * fielddistx + (sy & 1) * fielddisthalfx;
   int yp2 = sy * fielddisty;

   int xp1 = x * fielddistx + (y & 1) * fielddisthalfx;
   int yp1 = y * fielddisty;

   int dx = xp2-xp1;
   int dy = yp2-yp1;
   double at = atan2 ( dy, dx );
   // printf("%d / %d / %f \n", dx, dy, at);
   at -= offset;
   while ( at < 0 )
      at += 2 * 3.14159265;

   // printf("%f \n", at);
   return at;
}

#define checkvisibility

void tdrawgettempline :: start ( int x1, int y1, int x2, int y2 )
{
   init();

   sx = x2;
   sy = y2;

   if ( y1 == y2  && x1 == x2 )
      return;

   int x = x1;
   int y = y1;

   double w = winkel ( x1, y1 );

   int dir = -1;
   double mindiff = 10000;
   for ( int i = 0; i < sidenum; i++ ) {
      double nd = fabs ( dirs[i] - w );
      if ( nd < mindiff ) {
         dir = i;
         mindiff = nd;
      }
   }

  #ifdef checkvisibility
   int ldist = beeline ( x1, y1, x2, y2 );
  #endif

   int lastdir = winkelcomp ( w, dirs[dir] );
/*
   if ( x1 == 18 && y1 == 24 && x2 == 18 && y2 == 9 ) {
      printf("blurb");
   }
*/

   getnextfield( x, y, dir );
   while ( x != x2 || y != y2 ) {
      #ifdef checkvisibility
       int ldist2 = beeline ( x, y, x2, y2 );
       if ( ldist2 > ldist ) {
          displaymessage ( "inconsistency in tdrawgettempline :: start ; parameters are %d/%d ; %d/%d ", 1, x1, y1, x2, y2 );
          return;
       }
      #endif

       putpix8 ( x, y );
       double w2 = winkel ( x, y );
       // printf("%f \n", w2);
       if ( lastdir > 0 ) {
          if ( winkelcomp ( w2,  w ) == 1 ) {
             dir--;
             lastdir = -1;
          }
       } else {
          if ( winkelcomp ( w2 , w ) == -1 ) {
             dir++;
             lastdir = 1;
          }
       }
       if ( dir < 0 )
          dir += sidenum;

       if ( dir >= sidenum )
          dir = dir % sidenum;

       getnextfield ( x, y, dir );
   }
   putpix8 ( x, y );

}


int rn = 0;
int rxx;
int ryy;

void testline ( void )
{
   if ( rn == 0 ) {
      rxx = getxpos();
      ryy = getypos();
      rn++;
   } else {
      actmap->cleartemps ( 7 );
      tdrawgettempline a ( 0 );
      a.start ( rxx, ryy, getxpos(), getypos() );
      rn = 0;
   }
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////      
//////                               S I C H T                                               ///////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

// #define DEBUGVIEW

//////////// tcomputeview          ////////////////////////////////



void         tcomputeview::initviewcalculation(  int view, int jamming, int sx, int sy, int _mode, int _height  )  // mode: +1 = add view  ;  -1 = remove view
{ 
   height = _height;
   viewdist = view; 
   jamdist  = jamming;
   mode = _mode;

   int md;
   if ( viewdist > jamdist )
      md = viewdist / minmalq + 1;
   else
      md = jamdist / minmalq + 1;

   initsuche( sx, sy, md, 1 );
} 



void         tcomputeview::testfield(void)
{ 

   if ((xp >= 0) && (yp >= 0) && (xp < actmap->xsize) && (yp < actmap->ysize)) { 

      int f = beeline(startx,starty,xp,yp);
      pfield efield = getfield(xp,yp); 

      if ( viewdist && ( f <= 15 )) 
         efield->view[player].direct += mode;

      int str = viewdist;
      if ( f )
         if ( gameoptions.visibility_calc_algo == 1 ) {
            int x = startx ;
            int y = starty ;
            while ( x != xp || y != yp ) {
               int d = getdirection ( x, y, xp, yp );
               getnextfield ( x, y, d );
               str -= getfield(x,y)->typ->basicjamming + actmap->weather.fog ;
               if ( d & 1 )
                  str-=8;
               else
                  str-=12;
            };
         } else {
            int freefields = 0;
            if ( height > chhochfliegend )
               freefields = 5;
            else
            if ( height == chhochfliegend )
               freefields = 3;
            else
            if ( height == chfliegend )
               freefields = 2;
            else
            if ( height == chtieffliegend )
               freefields = 1;
           tdrawgettempline lne ( freefields );
           if ( startx == -1 || starty == -1 )
              displaymessage("error in tcomputeview::testfield",1 );
           lne.start ( startx, starty, xp, yp );
           str -= f;
           str -= lne.tempsum;
         }

      if ( str > 0 ) { 
         efield->view[player].view += str * mode;

         if ( sonar )
            efield->view[player].sonar += mode;

         if ( satellitenview )
            efield->view[player].satellite += mode;

         if ( minenview )
            efield->view[player].mine += mode;
      } 

      if ( jamdist >= f )
         efield->view[player].jamming += (jamdist - f) * mode;

      #ifdef DEBUGVIEW
        if ( efield->view[player].view      < 0 ||
             efield->view[player].sonar     < 0 ||
             efield->view[player].satellite < 0 ||
             efield->view[player].jamming   < 0 ||
             efield->view[player].mine      < 0 )
           displaymessage ( "Warning: inconsistency in view calculation !!\n Please report this bug !", 1 );
      #endif
   } 
} 



void         tcomputevehicleview::init( const pvehicle eht, int _mode  )   // mode: +1 = add view  ;  -1 = remove view ); )
{ 
   player = eht->color / 8;
   
   if ( eht->height == chsatellit )
      satellitenview = 1;
   else
      satellitenview = !!(eht->functions & cfsatellitenview);

   sonar =           !!(eht->functions & cfsonar);
   minenview =      !!(eht->functions & cfmineview);

   if ( (eht->functions & cfautodigger) && mode == 1 )
      searchforminablefields ( eht );

  // pfield efield = getfield( eht->xpos, eht->ypos );
   xp = eht->xpos;
   yp = eht->ypos;
   tcomputeview::initviewcalculation( eht->typ->view+1, eht->typ->jamming, eht->xpos, eht->ypos, _mode, eht->height );
   testfield();

} 



void         tcomputebuildingview::init( const pbuilding    bld,  int _mode )
{ 
   player = bld->color / 8;

   int  c, j ; 

   if (bld->completion == bld->typ->construction_steps - 1) {
      c = bld->typ->view; 
      j = bld->typ->jamming;
   } else {
      c = 15; 
      j = 0;
   }

   initviewcalculation( c, j, bld->xpos, bld->ypos, _mode, bld->typ->buildingheight ); 
   sonar = !!(bld->typ->special & cgsonarb); 

   minenview = true;
   satellitenview = true;

   building = bld; 

   orgx = bld->xpos - building->typ->entry.x; 
   orgy = bld->ypos - building->typ->entry.y; 

   dx = orgy & 1; 

   orgx = orgx + (dx & (~bld->typ->entry.y));

   for ( int a = orgx; a <= orgx + 3; a++) 
      for (int b = orgy; b <= orgy + 5; b++) 
         if ( building->getpicture ( a - orgx, b - orgy )) {
            int xp = a - (dx & b);
            int yp = b;
            pfield efield = getfield ( xp, yp );
            if ( minenview )
               efield->view[player].mine += _mode;
            efield->view[player].direct += _mode;
         } 
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
               if ( eht->reactionfire_active )
                  if ( eht->reactionfire & ( 1 << ( vehicle->color / 8 )))
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
      evaluatevisibilityfield ( fld, i, -1 );
      if ( fieldvisiblenow ( fld, i )) {
         punitlist ul  = unitlist[i];
         while ( ul  &&  !result ) {
            punitlist next = ul->next;
            pattackweap atw = attackpossible ( ul->eht, x, y );
            if ( atw->count && (ul->eht->reactionfire & (1 << (vehicle->color / 8)))) {

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
               ul->eht->reactionfire &= 0xff ^ ( 1 <<  (vehicle->color / 8) );
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
                           shortint     direc,
                           int&         mm1,               // fÅr Spritfuelconsumption
                           int&         mm2 )             //  fÅr movementdecrease
{ 
#ifdef HEXAGON   
 static const  byte         movemalus[2][6]  = {{ 8, 6, 3, 0, 3, 6 }, {0, 0, 0, 0, 0, 0 }};
#else
 static const  byte         movemalus[2][8]  = {{2, 4, 2, 8, 0, 8, 2, 4}, {4, 2, 5, 2, 0, 2, 5, 2}};
#endif

  int          d;
  int           x, y;
  byte         c; 
  pfield        fld;
  pfield        fld2;
  byte         mode; 
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
      mm1 = minmalq;
      if (vehicle->height >= chtieffliegend)
         mm2 = minmalq;
      else
         mm2 = getfield(x2,y2)->getmovemalus( vehicle );
   } else {
      mm1 = maxmalq;
      if (vehicle->height >= chtieffliegend)
         mm2 = maxmalq;
      else
         mm2 = getfield(x2,y2)->getmovemalus( vehicle ) * maxmalq / minmalq;
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
                 mm2 += movemalus[mode][d];
              npop( vehicle->ypos );
              npop( vehicle->xpos );
              npop( fld2->vehicle );

              delete atw ;
           }
        }
      }
   } 

    /*******************************/
    /*    Wind calculation        ˇ */
    /*******************************/
   if (vehicle->height >= chtieffliegend && vehicle->height <= chhochfliegend && actmap->weather.wind[ getwindheightforunit ( vehicle ) ].speed  ) {
      mm1 -=  windmovement[direc];
      mm2 -=  windmovement[direc];
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
   if (fieldvisiblenow(ffield, playerview ))
      paintvehicleinfo ( ffield->vehicle, ffield->building, ffield->object, NULL );
   else 
      paintvehicleinfo( NULL, NULL, NULL, NULL ); 
}

void         tdashboard::putheight(integer      i, integer      sel) 
                                      //          hîhe           mîglichk.
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
    byte         c; 
 
    int x1 = agmp->resolutionx - ( 640 - 520);
    int x2 = agmp->resolutionx - ( 640 - 573);
    int y1 = 59;
    int y2 = 67;

    if ( vehicle ) 
       if ( vehicle->typ->tank ) 
          w = ( x2 - x1 + 1) * vehicle->fuel / vehicle->typ->tank; 
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
 

void         tdashboard::paintweapon(byte         h, int num, int strength,  const SingleWeapon  *weap )
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
                      if ( vt->tank ) { 
                         putimage ( xp, 93 + k * 13, xlatpict ( &xlattables.light, icons.unitinfoguiweapons[ 8 ] ));
                         paintweaponammount ( k, ( vehicle ? vehicle->fuel : vt->tank ), vt->tank );
                         k--;
                      } 
                } 
             }
          }
       
       if ( materialdisplayed ) {
          if ( vt->material ) { 
             if ( serv )
                putimage ( xp, 93 + k * 13, xlatpict ( &xlattables.light, icons.unitinfoguiweapons[ 11 ] ));
             else
                putimage ( xp, 93 + k * 13, icons.unitinfoguiweapons[ 11 ] );
              paintweaponammount ( k, ( vehicle ? vehicle->material : vt->material ), vt->material );
              k--;
          } 
          if ( vt->energy ) { 
             if ( serv )
                putimage ( xp, 93 + k * 13, xlatpict ( &xlattables.light, icons.unitinfoguiweapons[ 9 ] ));
             else
                putimage ( xp, 93 + k * 13, icons.unitinfoguiweapons[ 9 ] );
              paintweaponammount ( k, ( vehicle ? vehicle->energy : vt->energy ), vt->energy );
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
       
       if ( vt->energy ) 
          count++; 

       int funcs;
       if ( vehicle )
          funcs = vehicle->functions;
       else
          funcs  = vt->functions;


       if ( (serv>= 0 || (funcs & cfmaterialref)) && vt->material ) 
          count++; 

       if ( (serv>= 0 || (funcs & cffuelref)) && vt->tank ) 
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
                i++; 
             } 
          }
       
       if ( serv >= 0 ) {
          paintlargeweapon(i, cwaffentypen[ cwservicen ], -1, -1, -1, -1, -1, -1, 
                         vt->weapons->weapon[serv].maxdistance, vt->weapons->weapon[serv].mindistance,
                         vt->weapons->weapon[serv].sourceheight, vt->weapons->weapon[serv].targ );
          i++; 
       }
       if ( vt->energy ) { 
          paintlargeweapon(i, resourceNames[ 0 ], ( vehicle ? vehicle->energy : vt->energy ), vt->energy, -1, -1, -1, -1, -1, -1, -1, -1 );
          i++; 
       } 

       if ( (serv>= 0 || (funcs & cfmaterialref)) && vt->material ) { 
          paintlargeweapon(i, resourceNames[ 1 ], ( vehicle ? vehicle->material : vt->material ), vt->material, -1, -1, -1, -1, -1, -1, -1, -1 );
          i++; 
       } 
       if ( (serv>= 0 || (funcs & cffuelref)) && vt->tank ) { 
          paintlargeweapon(i, resourceNames[ 2 ], ( vehicle ? vehicle->fuel : vt->tank ), vt->tank, -1, -1, -1, -1, -1, -1, -1, -1 );
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
            if ( vt->weapons->weapon[j].getScalarWeaponType() >= 0) 
               if ( vt->weapons->weapon[j].shootable() ) 
                  if ( mouseinrect ( x, y, x + 640, y+ 14 ))
                     topaint = j; // + serv;
            if ( vt->weapons->weapon[j].service() )
               serv++;
            
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
    byte         c; 
 


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
       if ( vehicle->typ->fuelconsumption ) {
          if ( (movedisp  &&  vehicle->typ->fuelconsumption ) || (minmalq*vehicle->fuel / vehicle->typ->fuelconsumption  < vehicle->movement ))
             showtext2c( strrrd8d( minmalq*vehicle->fuel / vehicle->typ->fuelconsumption ), agmp->resolutionx - ( 640 - 591), 59);
          else
             showtext2c( strrrd8d(vehicle->movement), agmp->resolutionx - ( 640 - 591), 59);
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

   if ( !gameoptions.smallmapactive ) {
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
         if ( vehicle && vehicle->name && vehicle->name[0] )
            showtext2c( vehicle->name , agmp->resolutionx - ( 640 - 500 ), 27);
         else
            if ( vt->name && vt->name[0] )
               showtext2c( vt->name , agmp->resolutionx - ( 640 - 500 ), 27);
            else
               if ( vt->description  &&  vt->description[0] )
                  showtext2c( vt->description ,agmp->resolutionx - ( 640 - 500 ), 27);
               else
                  bar ( agmp->resolutionx - ( 640 - 499 ), 27, agmp->resolutionx - ( 640 - 575 ), 35, 171 );

      } else {
         if ( building->name && building->name[0] )
            showtext2c( building->name , agmp->resolutionx - ( 640 - 500), 27);
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
      gameoptions.smallmapactive = 1;
      gameoptions.changed = 1;
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
       gameoptions.smallmapactive = !gameoptions.smallmapactive;
       gameoptions.changed = 1;

       if ( gameoptions.smallmapactive )
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
       if ( smallmap  &&  gameoptions.smallmapactive )
          smallmap->checkformouse();
   
       if ( !gameoptions.smallmapactive ) {
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
             displaymessage2("unit has %d fuel", fld->vehicle->fuel );
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

   byte         ms; 
 
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

   if ( gameoptions.smallmapactive )
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
                  // Message "sneak attack" an alle au·er dem Angreifer selbst.


            char txt[200];
            char* sp = getmessage( 10001 ); 
            sprintf ( txt, sp, actmap->player[act].name, actmap->player[i].name );
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
            sprintf ( txt, sp, actmap->player[act].name );
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
            sprintf ( txt, sp, actmap->player[act].name );
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


      #ifndef karteneditor
        quedevents[player]++;
      #endif
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



void returnresourcenuseforpowerplant (  const pbuilding bld, int prod, tresources *usage, int percentagee_based_on_maxplus )
{
   tresources res;
   if ( percentagee_based_on_maxplus )
      res = bld->maxplus;
   else
      res = bld->plus;

   for ( int r = 0; r < 3; r++ )
      if ( res.resource[r] > 0 )
         usage->resource[r] = 0;
      else
         usage->resource[r] = -res.resource[r] * prod / 100;
}



void tbuilding :: execnetcontrol ( void ) 
{
   if ( actmap->resourcemode != 1 ) {
      for ( int i = 0; i < 3; i++ ) {
         if (  netcontrol & (cnet_moveenergyout << i )) {
            npush (  netcontrol );
            netcontrol |= (cnet_stopenergyinput << i );
            actstorage.resource[i] -= put_energy ( actstorage.resource[i], i, 0 );
            npop (  netcontrol );
         } else
            if (  netcontrol & (cnet_storeenergy << i )) {
               npush (  netcontrol );
               netcontrol |= (cnet_stopenergyoutput << i );
               actstorage.resource[i] += get_energy ( typ->gettank(i) -  actstorage.resource[i], i, 0 );
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
                                 +16         plus zurÅckliefern                 <  diese Bits schlie·en sich gegenseitig aus
                                 +32         usage zurÅckliefern                 \
                                 +64         tank zurÅckliefern                   \
                                 */


int  tbuilding :: put_energy ( int      need,    int resourcetype, int queryonly, int scope  ) 
{
   PutResource putresource ( scope );
   return putresource.getresource ( xpos, ypos, resourcetype, need, queryonly, color/8, scope );
}


int  tbuilding :: get_energy ( int      need,    int resourcetype, int queryonly, int scope ) 
{
   GetResource gr ( scope );
   return gr.getresource ( xpos, ypos, resourcetype, need, queryonly, color/8, scope );
}



int  tbuilding :: getresourceplus( int mode, tresources* gplus, int queryonly )
{
   int did_something = 0;

   if ( !queryonly && (mode & 1))
      mode -= 1;

   if ( actmap->resourcemode != 1 ) {
      gplus->resource[0] = 0;
      gplus->resource[1] = 0;
      gplus->resource[2] = 0;

      if ( (typ->special & cgwindkraftwerkb ) && ( mode & 2 ) ) 
         for ( int r = 0; r < 3; r++ ) {
            int eplus =  maxplus.resource[r] * actmap->weather.wind[0].speed / 255;
            if ( ! (mode & 1 )) {
               int putable = put_energy ( eplus + gplus->resource[r]*queryonly, r, 1 ) - gplus->resource[r]*queryonly;
               if ( putable < eplus )
                  eplus = putable; 
            }
            gplus->resource[r] += eplus;
            if ( !queryonly && !work.wind_done ) {
               put_energy ( eplus, r, 0 );
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
                  pfield fld = getbuildingfield ( this, x, y );
                  int weather = 0;
                  while ( fld->typ != fld->typ->terraintype->weather[weather] )
                     weather++;                
   
                  sum += csolarkraftwerkleistung[weather];
                  num ++;
               }
            
         tresources rplus;
         for ( int r = 0; r < 3; r++ ) {
            rplus.resource[r] = maxplus.resource[r] * sum / ( num * 1024 );
            if ( ! (mode & 1 )) {
               int putable = put_energy ( rplus.resource[r] + gplus->resource[r]*queryonly, r, 1 ) - gplus->resource[r]*queryonly;
               if ( putable < rplus.resource[r] )
                  rplus.resource[r] = putable; 
            }
            gplus->resource[r] += rplus.resource[r];
         }

         if ( !queryonly && !work.solar_done ) {
            for ( int s = 0; s < 3; s++ )
               put_energy ( rplus.resource[s], s, 0 );
            work.solar_done = 1;
            did_something++;
         }
      } 



      if ( (typ->special & cgconventionelpowerplantb) && ( mode & 8 ) ) {

         int perc = 100;
         tresources toproduce;
         if ( !queryonly )
            toproduce = work.resource_production.toproduce;
         else
            toproduce = plus;

         for ( int r = 0; r < 3; r++ )
            if ( plus.resource[r] > 0 ) {
               int p = plus.resource[r];
               if ( !(mode & 1))
                  p = put_energy ( plus.resource[r] + p, 0, 1 );

               if ( perc > 100 * p / maxplus.resource[r] )
                  perc = 100 * p / maxplus.resource[r] ;
            }


         tresources usage;
         returnresourcenuseforpowerplant ( this, perc, &usage, 0 );
         int ena ;
         int maa ;
         int fua ;
         if ( !queryonly ) {
            ena = get_energy ( usage.a.energy  , 0, 1 );
            maa = get_energy ( usage.a.material, 1, 1 );
            fua = get_energy ( usage.a.fuel    , 2, 1 );
         } else {
            ena = usage.a.energy;
            maa = usage.a.material;
            fua = usage.a.fuel;
         }
   

         // This calculation is done iteratively because the resourcenusage may be nonlinear
         if ( maa < usage.a.material ||  fua < usage.a.fuel  || ena < usage.a.energy  ) {
            int diff = perc / 2;
            while ( maa < usage.a.material ||  fua < usage.a.fuel  || ena < usage.a.energy  || diff > 1) {
               if ( maa < usage.a.material ||  fua < usage.a.fuel  || ena < usage.a.energy ) 
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
   
         if ( maa < usage.a.material ||  fua < usage.a.fuel  || ena < usage.a.energy  ) 
            displaymessage( "controls : int tbuilding :: getenergyplus( void ) : inconsistency in getting material or fuel for energyproduction", 2 );
   
         if ( !queryonly ) {
            work.resource_production.did_something_lastpass = 0;
            work.resource_production.finished = 1;
            for ( int r = 0; r < 3; r++ ) {
               get_energy ( usage.resource[r]  , r, 0 );
               put_energy ( toproduce.resource[r] * perc / 100, r, 0 );
               work.resource_production.toproduce.resource[r] -= toproduce.resource[r] * perc / 100;
               if ( toproduce.resource[r] * perc / 100  > 0 ) {
                  work.resource_production.did_something_lastpass = 1;
                  work.resource_production.did_something_atall = 1;
                  did_something++;
               }

               if ( work.resource_production.toproduce.resource[r] > 0 )
                  work.resource_production.finished = 0;
            }
         }
         

         for ( int s = 0; s < 3; s++ ) 
            if ( toproduce.resource[s] * perc / 100  > 0 ) 
               gplus->resource[s] += toproduce.resource[s] * perc / 100;
      }

      if ( (typ->special & cgminingstationb) && ( mode & 16 ) ) 
         if ( queryonly || work.mining.finished < 3 ) {
            int mp = 0;
            int fp = 0;
            if ( queryonly )
               initwork();
            mp = processmining ( 1, !queryonly );
            fp = processmining ( 2, !queryonly );
            gplus->a.material += mp;
            gplus->a.fuel     += fp;
            if ( mp || fp )
               did_something++;
         }
      

   } else {
      *gplus = bi_resourceplus;
      if ( !queryonly && !work.bimode_done ) {
         work.bimode_done = 1;
         for ( int r = 0; r < 3; r++ ) {
            put_energy ( bi_resourceplus.resource[r], r, 0 );
            if ( bi_resourceplus.resource[r] > 0 )
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



tgetmininginfo :: tgetmininginfo ( void )
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
        
        mininginfo->avail[dist].a.material += fld->material * resource_material_factor;
        mininginfo->avail[dist].a.fuel     += fld->fuel     * resource_fuel_factor;
        mininginfo->max[dist].a.material   += 255 * resource_material_factor;
        mininginfo->max[dist].a.fuel       += 255 * resource_fuel_factor;
  }
}


void tgetmininginfo :: run (  const pbuilding bld )
{
   initsuche ( bld->xpos, bld->ypos, maxminingrange, 0 );
   xp = bld->xpos;
   yp = bld->ypos;
   dist=0;
   testfield();
   startsuche();
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
   int oldmm = mm;
   int oldmf = mf;

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
         if ( bld->plus.resource[r] < 0 )
            bld->work.mining.touse.resource[r] = -bld->plus.resource[r];
         else
            bld->work.mining.touse.resource[r] = 0;
   }

   int perc = 1000;
   int resourcesrequired = 0;
   if ( abbuch ) 
      for ( int r = 0; r < 3; r++ )
         if ( bld->work.mining.touse.resource[r] > 0 ) {
            resourcesrequired = 1;
            int g = bld->get_energy ( bld->work.mining.touse.resource[r], r, 1 );
            if ( g * 1000 / (-bld->plus.resource[r]) < perc )
               perc = g * 1000 / (-bld->plus.resource[r]);
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

   initsuche( bld->xpos, bld->ypos, maxminingrange, 0 );
   xp = bld->xpos;
   yp = bld->ypos;
   dist = 0;
   testfield();
   startsuche();

   mm = materialgot * bld->typ->efficiencymaterial / 1024;
   mf = fuelgot     * bld->typ->efficiencyfuel     / 1024;

   if ( abbuch ) {
      int perc = 1000 * ( workbeforestart - worktodo ) / orgworktodo;
      for ( int r = 0; r < 3; r++ )
         if ( bld->plus.resource[r] < 0 ) {
            int g = bld->get_energy ( -bld->plus.resource[r] * perc / 1000, r, 0 );
            bld->work.mining.touse.resource[r] -= g;
         }
      if ( !resourcesrequired ) 
            bld->work.mining.finished+= res;
      

   }
   return range;
}

int  tbuilding :: processmining ( int res, int abbuchen )
{

   int maxfuel = plus.a.fuel;
   int maxmaterial = plus.a.material;
  
   int capfuel = 0;
   int capmaterial = 0;

   if ( abbuchen ) {
      if ( res == 1 ) 
         capmaterial = put_energy ( plus.a.material, 1, 1 );
      else 
         capfuel = put_energy ( plus.a.fuel, 2, 1 );
   } else {
      capmaterial = plus.a.material;
      capfuel = plus.a.fuel;
   }

   tprocessminingfields pmf;


   lastmineddist = pmf.setup ( this, maxmaterial, capmaterial, maxfuel, capfuel, abbuchen, res );

   if ( abbuchen ) {
      put_energy ( maxmaterial, 1 , 0 );
      put_energy ( maxfuel,     2 , 0 );
   }
   if ( res==1 )
      return maxmaterial;
   else
      return maxfuel;

}

void tbuilding :: getresourceusage ( tresources* usage )
{
   returnresourcenuseforpowerplant ( this, 100, usage, 0 );
   if ( typ->special & cgresearchb ) {
      int material;
      int energy;
      returnresourcenuseforresearch ( this, researchpoints, &energy, &material );
      usage->a.material += material;
      usage->a.energy   += energy;
      usage->a.fuel  = 0;
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
      int ena = bld->get_energy ( energy,   0, 1 );
      int maa = bld->get_energy ( material, 1, 1 );

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

      ena = bld->get_energy ( energy,   0, 0 );
      maa = bld->get_energy ( material, 1, 0 );

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

int tbuilding :: getmininginfo ( int res )
{
   return processmining ( res, 0 );
}


void tbuilding :: initwork ( void )
{
   lastmaterialavail = -1;
   lastfuelavail = -1;
   lastenergyavail = -1;

   tresources nul;
   nul.a.energy = 0; nul.a.material = 0; nul.a.fuel = 0;

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
      if ( actmap->resourcemode == 0 ) {
         if ( typ->special & cgwindkraftwerkb ) 
            work.wind_done = 0;

         if ( typ->special & cgsolarkraftwerkb ) 
            work.solar_done = 0;

         if ( typ->special & cgminingstationb ) {
            for ( int r = 0; r < 3; r++ )
               if ( plus.resource[r] < 0 )
                  work.mining.touse.resource[r] = -plus.resource[r];
               else
                  work.mining.touse.resource[r] = 0;

            work.mining.finished = 0;
         }

         if ( typ->special & cgconventionelpowerplantb ) {
            for ( int r = 0; r < 3; r++ )
               if ( plus.resource[r] > 0 )
                  work.resource_production.toproduce.resource[r] = plus.resource[r];
               else
                  work.resource_production.toproduce.resource[r] = 0;

            work.resource_production.finished = 0;
         }

      } else 
         work.bimode_done = 0;


   }
}

int tbuilding :: worktodo ( void )
{
   if ( actmap->resourcemode == 1 ) {
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




int  tbuilding :: processwork ( void )
{
   if ( (completion == typ->construction_steps - 1) ) {

      if ( actmap->resourcemode == 1 ) {
         tresources plus;

         int res = getresourceplus  ( -2, &plus, 0 );

         execnetcontrol ();

         return res;
      } else {

         tresources plus;
         int res = getresourceplus  ( -2, &plus, 0 );

         execnetcontrol ();

         for ( int r = 0; r < 3; r++ )
            if (  actstorage.resource[r] >  typ->gettank(r) ) {
               put_energy ( typ->gettank(r) -  actstorage.resource[r], r , 0 );
               actstorage.resource[r] =  typ->gettank(r);
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
             if ( eht->energy < eht->typ->energy  && eht->generatoractive )
                if ( eht->functions & cfgenerator ) {
                   int endiff = eht->typ->energy- eht->energy;
                   if ( eht->fuel < endiff * generatortruckefficiency )
                      endiff = eht->fuel / generatortruckefficiency;

                   eht->energy += endiff;
                   eht->fuel -= endiff * generatortruckefficiency ;
                }
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

void newturnforplayer ( int forcepasswordchecking, char* password )
{

   checkalliances_at_beginofturn ();

   if ( actmap->player[actmap->actplayer].stat == ps_human ) {
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
   
      dashboard.x = 0xffff; 
      
      moveparams.movestatus = 0; 
   
      int humannum = 0;
      for ( int i = 0; i < 8; i++ )
         if ( actmap->player[i].existent )
            if ( actmap->player[i].stat == ps_human )
               humannum++;
         
   
      if ( humannum > 1  ||  forcepasswordchecking > 0 ) {
         tlockdispspfld ldsf;
         backgroundpict.paint();

         if ( (actmap->player[actmap->actplayer].passwordcrc && actmap->player[actmap->actplayer].passwordcrc != gameoptions.defaultpassword && actmap->player[actmap->actplayer].passwordcrc != encodepassword ( password )) 
            || actmap->time.a.turn == 1 || (actmap->network && actmap->network->globalparams.reaskpasswords) ) {
               if ( forcepasswordchecking < 0 ) {
                  erasemap( actmap );
                  throw tnomaploaded();
               } else
                  enterpassword ( &actmap->player[actmap->actplayer].passwordcrc );
         } else
            displaymessage("next player is:\n%s",3,actmap->player[actmap->actplayer].name );
      }
   }
   computeview(); 

/*
   // Wenn der allererste player zum ersten mal drankommt, mu· auch fÅr die anderen player die view computet werden
   if ( actmap->time.turn == 1 ) {
      int i = 0;
      while ( !actmap->player[i].existent )
         i++;

      if ( actmap->actplayer == i) {       // wirklich der erste player
         npush ( actmap->playerview );
         for ( int i = actmap->actplayer+1; i< 8; i++ ) {
            actmap->playerview = i;
            computeview();
         }

         npop ( actmap->playerview );
      }


   }
*/
   actmap->playerview = actmap->actplayer;

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

      removemessage ();

   }

   {
      /*
      int xp = actmap->cursorpos.position[ actmap->actplayer ].cx;
      int yp = actmap->cursorpos.position[ actmap->actplayer ].cy;
      int xss = idisplaymap.getscreenxsize();
      int yss = idisplaymap.getscreenysize();
      */

      actmap->xpos = actmap->cursorpos.position[ actmap->actplayer ].sx;
      actmap->ypos = actmap->cursorpos.position[ actmap->actplayer ].sy;

      cursor.gotoxy ( actmap->cursorpos.position[ actmap->actplayer ].cx, actmap->cursorpos.position[ actmap->actplayer ].cy , 0);
   
   }
   if ( actmap->lastjournalchange.abstime ) 
      if ( (actmap->lastjournalchange.a.turn == actmap->time.a.turn ) || 
           (actmap->lastjournalchange.a.turn == actmap->time.a.turn-1  &&  actmap->lastjournalchange.a.move > actmap->actplayer ) )
              viewjournal();

   dashboard.x = 0xffff;
   transfer_all_outstanding_tribute();
}





void sendnetworkgametonextplayer ( int oldplayer, int newplayer )
{
   int num = 0;
   int pl[8];

   {
      for (int i = 0; i < 8; i++) {
         if ( actmap->player[i].existent )
            if ( actmap->player[i].firstvehicle || actmap->player[i].firstbuilding ) {
               pl[num] = i;
               num++;
             }
      } /* endfor */
   }
/*
   if ( num == 1  && !actmap->continueplaying ) {
      displaymessage ( "You won !\n press ok to exit to main menu",1);
      erasemap();
      throw tnomaploaded ();
   }        
*/

   if ( oldplayer == -1  ||  actmap->network->player[ newplayer ].compposition == actmap->network->player[ oldplayer ].compposition ) {
      tlockdispspfld ldsf;

      newturnforplayer( 0 );
      checkforreplay();
   } else {
      tnetworkcomputer* compi = &actmap->network->computer[ actmap->network->player[ oldplayer ].compposition ];
      while ( compi->send.transfermethod == NULL  ||  compi->send.transfermethodid == 0 )
           setupnetwork( actmap->network, 2, oldplayer );

      displaymessage ( " starting data transfer ",0);

      try {
         compi->send.transfermethod->initconnection ( TN_SEND );
         compi->send.transfermethod->inittransfer ( &compi->send.data );
   
         char* desciption = NULL;
/*         {
            tenterfiledescription efd;
            efd.init();
            efd.run();
            desciption = efd.description;
            efd.done();
         }
*/   
   
         tnetworkloaders nwl;
         nwl.savenwgame ( compi->send.transfermethod->stream, desciption );
   
         if ( desciption ) 
            delete desciption;
   
         compi->send.transfermethod->closetransfer();
         compi->send.transfermethod->closeconnection();
      } /* endtry */
      catch ( tfileerror ) {
         displaymessage ( "error saving file", 1 );
      } /* endcatch */

      erasemap();
      displaymessage( " data transfer finished",1);

      throw tnomaploaded ();
   }
}

#define ignorecomputers




void         nextturn(void)
{ 
   if ( actmap->replayinfo )
      if ( actmap->replayinfo->actmemstream ) {
         delete actmap->replayinfo->actmemstream;
         actmap->replayinfo->actmemstream = NULL;
      }


   if ( actmap->objectcrc ) 
      if ( actmap->objectcrc->speedcrccheck->getstatus ( )  ) {
         erasemap();
         throw tnomaploaded();
      }

   int bb = cursor.an;
   if (bb)  
      cursor.hide();

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
   
   
            // Bei énderungen hier auch die Windanzeige dashboard.PAINTWIND aktualisieren !!!
   
            if (( actvehicle->height >= chtieffliegend )   &&   ( getfield(actvehicle->xpos,actvehicle->ypos)->vehicle == actvehicle)) { // || ((actvehicle->height == chfahrend) && (getfield(actvehicle->xpos,actvehicle->ypos)->typ->art & cbwater ))) && ) {
               if ( getmaxwindspeedforunit ( actvehicle ) < actmap->weather.wind[ getwindheightforunit ( actvehicle ) ].speed*maxwindspeed ){
                  removevehicle ( &actvehicle );
                  j = -1;
               } else {
      
                  j = actvehicle->fuel - actvehicle->typ->fuelconsumption * nowindplanefuelusage;

                  int move = actvehicle->movement;
                  if ( actvehicle->reactionfire_active  &&  !move )
                     move = actvehicle->typ->movement[log2(actvehicle->height)];

                  if ( actvehicle->height <= chhochfliegend )
                     j -= ( move * 64 / actvehicle->typ->movement[log2(actvehicle->height)] ) * (actmap->weather.wind[ getwindheightforunit ( actvehicle ) ].speed * maxwindspeed / 256 ) * actvehicle->typ->fuelconsumption / ( minmalq * 64 );
      
                 //          movement * 64        windspeed * maxwindspeed         fuelconsumption
                 // j -=   ----------------- *  ----------------------------- *   -----------
                 //          typ->movement                 256                       64 * 8
                 //
                 //
      
                 //gekÅrzt:
                 //
                 //             movement            windspeed * maxwindspeed        
                 // j -= --------------------- *  ----------------------------   * fuelconsumption
                 //           typ->movement             256   *      8 
                 //
                 //
                 //
                 // Falls eine vehicle sich nicht bewegt hat, bekommt sie soviel Sprit abgezogen, wie sie zum zurÅcklegen der Strecke,
                 // die der Wind pro Runde zurÅckgelegt hat, fuelconsumptionen wÅrde.
                 // Wenn die vehicle sich schon bewegt hat, dann wurde dieser Abzug schon beim movement vorgenommen, so da· er hier nur
                 // noch fÅr das Åbriggebliebene movement stattfinden mu·.
                 //
      
      
                  if (j < 0) 
                     removevehicle(&actvehicle); 
                  else 
                     actvehicle->fuel = j; 
               }
            } 
            if (j >= 0)  {
                  if ( actvehicle->reactionfire_active ) {
                     if ( actvehicle->reactionfire_active < 3 ) 
                        actvehicle->reactionfire_active++;

                     if ( actvehicle->reactionfire_active == 3 )  {
                        actvehicle->reactionfire = 0xff;
                        actvehicle->attacked = false; 
                     } else {
                        actvehicle->reactionfire = 0;
                        actvehicle->attacked = true; 
                     }

                     actvehicle->movement = 0;
                     actvehicle->attacked = false; 

                  } else {
                     actvehicle->resetmovement();
                     actvehicle->attacked = false; 
                  }
               } 

            if ( actvehicle )   
               actvehicle->nextturn();

            actvehicle = nxeht; 
         } 
   
         checkunitsforremoval ();
      }
   
        /* *********************  allianzen ********************  */ 

     checkalliances_at_endofturn ();

        /* *********************  messages ********************  */ 

     {
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
     }
     if ( actmap->newjournal ) {
        int s = 0;
        if ( actmap->journal )
           s = strlen ( actmap->journal );

        char* n = new char[ strlen ( actmap->newjournal ) + s + 200 ];

        char tempstring[100];
        char tempstring2[50];
        sprintf( tempstring, "#color0# %s ; turn %d #color0##crt##crt#", actmap->player[actmap->actplayer].name, actmap->time.a.turn );
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




  /* ****** NÑchster player ********************************************  */ 


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
         erasemap();
         throw tnomaploaded ();
      } 

      int newplayer = actmap->actplayer;
      actmap->playerview = actmap->actplayer;
      if ( actmap->network &&  oldplayer != actmap->actplayer)
        sendnetworkgametonextplayer ( oldplayer, newplayer );
      else {

         tlockdispspfld ldsf;

         int forcepwd;  // Wenn der aktuelle player gerade verloren hat, mu· fÅr den nÑchsten player die Passwortabfrage kommen, auch wenn er nur noch der einzige player ist !

         if ( oldplayer >= 0  &&  !actmap->player[oldplayer].existent )
            forcepwd = 1;
         else
            forcepwd = 0;

        newturnforplayer( forcepwd );
        checkforreplay();
      }
   if (bb)  
     cursor.display();
} 


void next_turn ( void )
{
     #ifdef ignorecomputers
     do {
     #endif
  
       nextturn();
  
     #ifdef ignorecomputers
     } while ( actmap->player[actmap->actplayer].stat != ps_human ); /* enddo */
     #endif
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

   catch ( tinvalidid err ) {
      displaymessage( err.msg, 1 );
      throw tnomaploaded();
   } /* endcatch */
   catch ( tinvalidversion err ) {
      displaymessage( "File %s has invalid version.\nExpected version %d\nFound version %d\n", 1, err.filename, err.expected, err.found );
      throw tnomaploaded();
   } /* endcatch */
   catch ( tcompressionerror err ) {
      displaymessage( "The file cannot be decompressed. \nIt has probably been damaged during transmission from the previous player to you.\nTry sending it zip compressed or otherwise encapsulated.\n", 1 );
      throw tnomaploaded();
   } /* endcatch */
   catch ( tfileerror err) {
      displaymessage( "error reading game %s ", 1, err.filename );
      throw tnomaploaded();
   } /* endcatch */
   catch ( terror ) {
      displaymessage( "error loading game", 1 );
      throw tnomaploaded();
   } /* endcatch */
   if ( actmap->xsize <= 0 || actmap->ysize <= 0 )
      throw tnomaploaded();

   newturnforplayer( 0 );

   checkforreplay();

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






void initweather ( void )
{
   actmap->weather.fog = 0;
   actmap->weather.wind[0].speed = actmap->weather.wind[1].speed = actmap->weather.wind[2].speed = 0;
   actmap->weather.wind[0].direction = actmap->weather.wind[1].direction = actmap->weather.wind[2].direction = 0;
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
                                  byte         col,
                                  pvehicle &   vehicle,
                                  int          x,
                                  int          y )
{ 
   if ( actmap->player[ actmap->actplayer ].research.vehicletypeavailable ( fztyp, actmap ) ) {

      generate_vehicle ( fztyp, col, vehicle );
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

   pfield entry = getbuildingfield ( bld, bld->typ->entry.x, bld->typ->entry.y );
   if ( entry->a.temp )
      return;

   if ( pass == 1 )
      checkbuilding( bld );

   entry->a.temp = 1;

   if ( !searchfinished() ) 
      for( int i = 0; i < 4; i++ )
         for ( int j = 0; j < 6; j++ ) {
            int xp, yp;
            getbuildingfieldcoordinates ( bld, i, j, xp, yp );
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
   if ( resource != 1   &&   actmap->resourcemode == 1 ) 
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
      if ( v->energy < toget ) 
         toget = v->energy;
   
      if ( !queryonly )
         v->energy -= toget;
      got += toget;
   }
}


void GetResource :: checkbuilding ( pbuilding b )
{
   if ( b->color/8 == player ) {
      if ((b->netcontrol & (cnet_stopenergyoutput << resourcetype)) == 0) {
         int toget = need-got;
         if ( b->actstorage.resource[ resourcetype ] < toget ) 
            toget = b->actstorage.resource[ resourcetype ];
      
         if ( !queryonly )
            b->actstorage.resource[ resourcetype ] -= toget;
         got += toget;
      }
   } else
      if ( actmap->tribute ) {
         int gettable = actmap->tribute->avail.resource[resourcetype][ b->color / 8 ][ player ] - tributegot[ resourcetype ][ b->color / 8];
         if ( gettable > 0 ) {
            int toget = need-got;
            if ( toget > gettable )
               toget = gettable;
 
            // int found = b->get_energy( toget, resourcetype, queryonly );
            int found = b->actstorage.resource[resourcetype];
            if ( toget < found )
               found = toget;
            
            if ( !queryonly )
               b->actstorage.resource[resourcetype] -= found;

            tributegot[ resourcetype ][ b->color / 8] += found;
   
            if ( !queryonly ) {
               actmap->tribute->avail.resource[ resourcetype ][ b->color / 8 ][ player ] -= found;
               actmap->tribute->paid .resource[ resourcetype ][ b->color / 8 ][ player ] += found;
            }
 
            got += found;
         }
      }
 


}

void GetResource :: start ( int x, int y )
{
   if ( scope == 3 ) {
      got = need;
      if ( got > actmap->bi_resource[player].resource[ resourcetype ] )
         got = actmap->bi_resource[player].resource[ resourcetype ];

      if ( !queryonly )
         actmap->bi_resource[player].resource[ resourcetype ] -= got;

   } else
      MapNetwork :: start ( x, y );
}








void PutResource :: checkbuilding ( pbuilding b )
{
   if ( b->color/8 == player ) {
      if ((b->netcontrol & (cnet_stopenergyinput << resourcetype)) == 0) {
         int tostore = need-got;
         if ( b->typ->gettank ( resourcetype ) - b->actstorage.resource[ resourcetype ] < tostore ) 
            tostore = b->typ->gettank ( resourcetype ) - b->actstorage.resource[ resourcetype ];
      
         if ( !queryonly )
            b->actstorage.resource[ resourcetype ] += tostore;
         got += tostore;
      }
   }
}


void PutResource :: start ( int x, int y )
{
   if ( scope == 3 ) {

      got = need;
      if ( got > maxint - actmap->bi_resource[player].resource[ resourcetype ] )
         got = maxint - actmap->bi_resource[player].resource[ resourcetype ];

      if ( !queryonly )
         actmap->bi_resource[player].resource[ resourcetype ] += got;

   } else
      MapNetwork :: start ( x, y );
}





void PutTribute :: checkbuilding ( pbuilding b )
{
   if ( b->color/8 == targplayer ) {
      if ((b->netcontrol & (cnet_stopenergyinput << resourcetype)) == 0) {
         int tostore = need-got;
         if ( b->typ->gettank ( resourcetype ) - b->actstorage.resource[ resourcetype ] < tostore ) 
            tostore = b->typ->gettank ( resourcetype ) - b->actstorage.resource[ resourcetype ];
      
         if ( !queryonly ) {
            b->actstorage.resource[ resourcetype ] += tostore;
            actmap->tribute->avail.resource[ resourcetype ][ player ][ targplayer ] -= tostore;
            actmap->tribute->paid .resource[ resourcetype ][ targplayer ][ player ] += tostore;
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

   if ( actmap->tribute )
      do {
         if ( targplayer != player )
            if ( actmap->player[targplayer].existent ) {
               need = actmap->tribute->avail.resource[ resourcetype ][ player ][ targplayer ];
               if ( need > 0 ) {
                  if ( scope == 3 ) {
               
                     got = need;
                     if ( got > maxint - actmap->bi_resource[targplayer].resource[ resourcetype ] )
                        got = maxint - actmap->bi_resource[targplayer].resource[ resourcetype ];

                     if ( got > actmap->bi_resource[player].resource[ resourcetype ] )
                        got = actmap->bi_resource[player].resource[ resourcetype ];

                     if ( !queryonly ) {
                        actmap->bi_resource[targplayer].resource[ resourcetype ] += got;
                        actmap->bi_resource[player].resource[ resourcetype ] -= got;
                        
                        actmap->tribute->avail.resource[ resourcetype ][ player ][ targplayer ] -= got;
                        actmap->tribute->paid .resource[ resourcetype ][ targplayer ][ player ] += got;
                     }
               
                  } else {
                     int avail = startbuilding->get_energy ( need, resourcetype, 1, 0 );
                     if ( need > avail )
                        need = avail;
                     MapNetwork :: start ( x, y );
                     if ( !queryonly ) 
                        startbuilding->get_energy ( got, resourcetype, queryonly, 0 );
                     
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
   if ( actmap->tribute ) 
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
                        topay[ resourcetype ] = actmap->tribute->avail.resource[ resourcetype ][ player ][ targplayer ];
                        got[ resourcetype ] = 0;

                        if ( resourcetype == 1 || actmap->resourcemode == 0 ) {
                           pbuilding bld = actmap->player[ player ].firstbuilding;
                           while ( bld  &&  topay[resourcetype] > got[resourcetype] ) {
                              PutTribute pt;
                              got[resourcetype] += pt.puttribute ( bld, resourcetype, 0, targplayer, player, 1 );
                              bld = bld->next;
                           }
                        } else {
                           int i;
                           if ( actmap->bi_resource[ player ].resource[resourcetype] < topay[resourcetype] )
                              i = actmap->bi_resource[ player ].resource[resourcetype];
                           else
                              i = topay[resourcetype];
                           got [resourcetype ] = i;
                           actmap->bi_resource[ player ].resource[resourcetype] -= i;
                           actmap->bi_resource[ targplayer ].resource[resourcetype] += i;
                        }

                        actmap->tribute->avail.resource[ resourcetype ][ player ][ targplayer ] -= got[resourcetype];
                        actmap->tribute->paid.resource[ resourcetype ][ targplayer ][ player ] += got[resourcetype];

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
                        sprintf ( txt1b, sp, txt_topay, actmap->player[player].name, txt_got );
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
      int t = b->typ->gettank ( resourcetype );
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
   if ( (_scope > 0)  &&  (actmap->resourcemode == 1)  &&  (resource != 1) ) 
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
      got += v->typ->energy;
}


void GetResourcePlus :: checkbuilding ( pbuilding bld )
{
   if ( bld->color/8 == player ) {
      tresources plus;
      bld->getresourceplus ( -1, &plus, 1 );
      got += plus.resource[resourcetype];
   }
}





void GetResourceUsage :: checkbuilding ( pbuilding b )
{
   if ( b->color/8 == player ) {
      tresources usage;
      b->getresourceusage ( &usage );
      got += usage.resource[ resourcetype ];
   }
}





void testnet ( void )
{
  static int resource = 0;

  GetResource net1;
  displaymessage ( "%d %s available", 1, net1.getresource ( getxpos(), getypos(), resource, maxint, 0, actmap->actplayer, 1 ), resourceNames[resource]);

  PutResource net2;
  displaymessage ( "%d %s space available", 1, net2.getresource ( getxpos(), getypos(), resource, maxint, 0, actmap->actplayer, 1 ), resourceNames[resource]);

  resource++;
  if ( resource > 2 )
     resource = 0;
}




int tvehicle::enablereactionfire( void ) 
{
   if ( ! reactionfire_active ) {
      reactionfire = 0;
      if (  /* movement <  typ->movement[ log2 (  height ) ]  || */  typ->wait  ||  attacked )
          reactionfire_active  = 1;
      else
          reactionfire_active  = 2;

      movement = 0;

      attacked = 1;
   }
   return 0;
}

int tvehicle::disablereactionfire ( void ) 
{
   if (  reactionfire_active ) {
       reactionfire_active = 0;
       reactionfire = 0;
   }
   return 0;
}






cmousecontrol :: cmousecontrol ( void )
{
   mousestat = 0;
}

void cmousecontrol :: chkmouse ( void )
{
   if ( gameoptions.mouse.fieldmarkbutton )
      if ( mouseparams.taste == gameoptions.mouse.fieldmarkbutton ) {
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

               dashboard.paint( getactfield(), actmap-> playerview );

               mousevisible(true);
            } 
      }

   if ( gameoptions.mouse.centerbutton )
      if ( mouseparams.taste == gameoptions.mouse.centerbutton ) {
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
            while ( mouseparams.taste == gameoptions.mouse.centerbutton )
               releasetimeslice();
         }
      }

   if ( gameoptions.mouse.smallguibutton )
      if ( mouseparams.taste == gameoptions.mouse.smallguibutton ) {
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

                       dashboard.paint( getactfield(), actmap-> playerview );

                       mousevisible(true);
                    }

                    actgui->painticons();
                 }
                 pfield fld = getactfield();
                 actgui->paintsmallicons( gameoptions.mouse.smallguibutton, !fld->vehicle && !fld->building && !fld->a.temp );
                 mousestat = 0;
              }
      } else 
        if ( mousestat == 1 )
           mousestat = 2;

   if ( gameoptions.mouse.largeguibutton )
      if ( mouseparams.taste == gameoptions.mouse.largeguibutton ) {
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

   if ( gameoptions.mouse.unitweaponinfo )
      if ( mouseparams.taste == gameoptions.mouse.unitweaponinfo ) {
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

void cmousecontrol :: reset ( void )
{                           
   mousestat = 0;
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
   if ( fieldvisiblenow ( getfield ( x1, y1 ), actmap->playerview) || fieldvisiblenow ( getfield ( x2, y2 ), actmap->playerview)) {
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
   if ( fieldvisiblenow ( getfield ( x, y ), actmap->playerview )) {
      checkMapPosition  ( x, y );
      mapDisplay->displayPosition ( x, y );
   }
}


void ReplayMapDisplay :: displayActionCursor ( int x1, int y1, int x2, int y2, int secondWait )
{
   if ( x1 >= 0 && y1 >= 0 ) {
      int i = fieldvisiblenow ( getfield ( x1, y1 ), actmap->playerview );
      if( i ) {
         cursor.gotoxy ( x1, y1, i );
         wait();
      }
   }

   if ( x2 >= 0 && y2 >= 0 ) {
      int i = fieldvisiblenow ( getfield ( x2, y2 ), actmap->playerview );
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
   if ( fieldvisiblenow ( getactfield(), actmap->playerview ))
    while ( ticker < t + gameoptions.replayspeed  && !keypress()) {
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
    rmd.setCursorDelay ( gameoptions.replayspeed );
    rmd.displayActionCursor ( x1, y1, x2, y2, secondWait );
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

                        pvehicle eht = actmap->getunit ( x1, y1, nwid );
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

                        pvehicle eht = actmap->getunit ( x1, y1, nwid );
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
                          int attackvisible = fieldvisiblenow ( fld, actmap->playerview ) || fieldvisiblenow ( targ, actmap->playerview );
                          if ( fld && targ && fld->vehicle ) {
                             if ( targ->vehicle ) {
                                tunitattacksunit battle ( fld->vehicle, targ->vehicle, 1, wpnum );
                                battle.av.damage = ad1;
                                battle.dv.damage = dd1;
                                if ( attackvisible ) {
                                   displayActionCursor ( x1, y1, x2, y2, 0 );
                                   battle.calcdisplay ( ad2, dd2 );
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

                        pvehicle eht = actmap->getunit ( x1, y1, nwid );
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

                           displayActionCursor ( x, y );

                           pfield fld = getfield ( x, y );
                           if ( fld ) {
                              if ( fld->vehicle )
                                 fld->vehicle->convert ( col );
                              else
                                 if ( fld->building )
                                    fld->building->convert ( col );

                              computeview();
                              displaymap();
                              wait();
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

                           displayActionCursor ( x, y );

                           pobjecttype obj = getobjecttype_forid ( id );

                           pfield fld = getfield ( x, y );
                           if ( obj && fld ) {
                              if ( actaction == rpl_remobj )
                                 fld->removeobject ( obj );      
                              else
                                 fld->addobject ( obj );

                              computeview();
                              displaymap();
                              wait();
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

                           displayActionCursor ( x, y );
                           pfield fld = getfield ( x, y );

                           pvehicletype tnk = getvehicletype_forid ( id );

                           if ( fld && tnk && !fld->vehicle ) {
                              pvehicle v;
                              generate_vehicle ( tnk, col, v );
                              v->xpos = x;
                              v->ypos = y;
                              fld->vehicle = v;

                              computeview();
                              displaymap();
                              wait();
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

                               displayActionCursor ( x, y );

                               pfield fld = getfield ( x, y );
    
                               pbuildingtype bld = getbuildingtype_forid ( id );
    
                               if ( bld && fld ) {
                                  putbuilding2( x, y, color, bld ); 
                                  computeview();
                                  displaymap();
                                  wait();
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

                           displayActionCursor ( x, y );

                           pfield fld = getfield ( x, y );
                           if ( fld ) {
                              fld -> putmine ( col, typ, strength );
                              computeview();
                              displaymap();
                              wait();
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

                           displayActionCursor ( x, y );

                           pfield fld = getfield ( x, y );
                           if ( fld ) {
                              fld -> removemine ( -1 );
                              computeview();
                              displaymap();
                              wait();
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

                           displayActionCursor ( x, y );

                           pfield fld = getfield ( x, y );
                           if ( fld && fld->building ) {
                              pbuilding bb = fld->building;
                              if ( bb->completion ) {
                                 bb->changecompletion ( -1 );
                              } else {
                                 removebuilding ( &bb );
                              }
                              computeview();
                              displaymap();
                              wait();
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


                                 int t = ticker;
                                 pfield fld = getfield ( x, y );

                                 pvehicle eht;

                                 pvehicletype tnk = getvehicletype_forid ( id );
                                 if ( tnk && fld) {
                                    generate_vehicle ( tnk, col / 8, eht );
                                    eht->klasse = cl;
                                    eht->xpos = x;
                                    eht->ypos = y;
                                    eht->setup_classparams_after_generation ();
                                    eht->fuel = eht->typ->tank;
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


                                 pvehicle eht = actmap->getunit ( x, y, nwid );
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

   orgmap = *actmap;
   memset ( actmap, 0, sizeof ( *actmap ));

   loadreplay ( orgmap.replayinfo->map[player]  );

   actmap->playerview = actplayer;

   tmemorystream guidatastream ( orgmap.replayinfo->guidata [ player ], 1 );
   stream = &guidatastream;

   if ( stream->dataavail () )
      stream->readdata2 ( nextaction );
   else
      nextaction = rpl_finished;

//   orgmap.replayinfo->actmemstream = stream;

   npush ( godview );
//   godview = 1;

   npush (actgui);
   actgui = &gui;

   actmap->xpos = orgmap.cursorpos.position[ actplayer ].sx;
   actmap->ypos = orgmap.cursorpos.position[ actplayer ].sy;

   cursor.gotoxy ( orgmap.cursorpos.position[ actplayer ].cx, orgmap.cursorpos.position[ actplayer ].cy , 0);
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

   erasemap ( );

//    orgmap.replayinfo->actmemstream = NULL;
   *actmap = orgmap;

   npop ( actgui );

   npop ( godview );

   int st = status;
   status = 0;

   cursor.gotoxy ( orgmap.cursorpos.position[ actplayer ].cx, orgmap.cursorpos.position[ actplayer ].cy );
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


