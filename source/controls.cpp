//     $Id: controls.cpp,v 1.9 1999-12-14 20:23:49 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
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
#include "spfldutl.h"
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


         tdashboard  dashboard;
         int             windmovement[8];

         trunreplay runreplay;

         int startreplaylate = 0;


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
     if ( fld->vehicle )
        if ( fld->vehicle->height & bld->typ->externalloadheight ) {
           numberoffields++;
           fld->a.temp = 123;
        }
  }
} 


void tsearchexternaltransferfields :: searchtransferfields( pbuilding building )
{
   cleartemps( 7 );
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
   
              fld->resourceview->visible |= ( 1 << actmap->actplayer );
              fld->resourceview->fuelvisible[actmap->actplayer] = fld->fuel;
              fld->resourceview->materialvisible[actmap->actplayer] = fld->material;
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
    sfmf.done();
}




void         tsearchputbuildingfields::initputbuilding( word x, word y, pbuildingtype building )
{ 
  pvehicle     eht; 

   eht = getfield(x,y)->vehicle; 
   if (eht->attacked || (eht->typ->wait && (eht->movement < eht->typ->movement[log2(eht->height)]))) {
      dispmessage2(302,""); 
      return;
   } 
   cleartemps(7); 
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
  boolean      b; 


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

   if ( selectbuildinggui.selectedbuilding ) {
      spbf.initputbuilding( getxpos(), getypos(), selectbuildinggui.selectedbuilding );
      spbf.done();
   };
   mousevisible(true);
} 




void         putbuildinglevel2( const pbuildingtype bld,
                               integer      xp,
                               integer      yp)
{ 
  int          x1, y1, x2, y2;
  pfield        fld; 
  boolean      b = true;

   if (getfield(xp,yp)->a.temp == 20) 
      if (moveparams.movestatus == 111) { 
         cleartemps(7); 
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
         cleartemps(7); 
         bld = moveparams.buildingtobuild; 
         eht = moveparams.vehicletomove; 
         putbuilding2(x,y,eht->color,bld); 

         logtoreplayinfo ( rpl_putbuilding, (int) x, (int) y, (int) bld->id, (int) eht->color );

         int mf = 100;
         int ff = 100;
      
         if ( actmap->gameparameter ) {
            if ( actmap->gameparameter[cgp_building_material_factor] )
               mf = actmap->gameparameter[cgp_building_material_factor];
      
            if ( actmap->gameparameter[cgp_building_fuel_factor] )
               ff = actmap->gameparameter[cgp_building_fuel_factor];
         }


         if (eht->material < bld->produktionskosten.material * mf / 100 ) { 
            displaymessage("not enough material!",1); 
            eht->material = 0; 
         } 
         else 
            eht->material -= bld->produktionskosten.material * mf / 100; 


         if (eht->fuel < bld->produktionskosten.sprit * ff / 100) { 
            displaymessage("not enough fuel!",1); 
            eht->fuel = 0; 
         } 
         else 
            eht->fuel -= bld->produktionskosten.sprit * ff / 100; 

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
   cleartemps(7); 
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
   sdbf.done();
} 


void         destructbuildinglevel2( int xp, int yp)
{ 
   pfield fld = getfield(xp,yp);
   if (fld->a.temp == 20) 
      if (moveparams.movestatus == 115) { 
         cleartemps(7); 
         pvehicle eht = moveparams.vehicletomove; 
         pbuildingtype bld = fld->building->typ;



         pbuilding bb = fld->building;

         eht->material += bld->produktionskosten.material * (100 - bb->damage) / destruct_building_material_get / 100;
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
      if (beeline(startx,starty,xp,yp) <= 15) 
         for (i = 0; i < actvehicle->typ->weapons->count ; i++) 
            if ((actvehicle->typ->weapons->weapon[i].sourceheight & actvehicle->height) > 0)
                 /* if ((actvehicle^.typ^.weapons->weapon[i].typ and cwrepairb > 0) and (mode = 1))   { ##  */ 
                 /*   or ((actvehicle^.typ^.weapons->weapon[i].typ and (cwammunitionb or cwrefuellingb)  > 0)
               and (mode in [2,3]))  */ 
                 /*  ##  */ 
               if ((((actvehicle->typ->weapons->weapon[i].typ & cwserviceb) > 0) && (mode == 1)) || (((actvehicle->typ->weapons->weapon[i].typ & (cwammunitionb | cwserviceb)) > 0) && ((mode==2)||(mode==3)))) {
                  fld = getfield(xp,yp); 
                  if (fld->vehicle != NULL) 
                     if ( !(fld->vehicle->functions & cfnoairrefuel) || fld->vehicle->height <= chfahrend )
                        if (getdiplomaticstatus(fld->vehicle->color) == capeace) 
                           if ((fld->vehicle->height & actvehicle->typ->weapons->weapon[i].targ) > 0) {
                              if ((actvehicle->typ->weapons->weapon[i].typ & cwammunitionb) > 0)
                                 if (fld->vehicle->typ->weapons->count > 0) 
                                    for (j = 0; j < fld->vehicle->typ->weapons->count ; j++) 
                                       if (((fld->vehicle->typ->weapons->weapon[j].typ & (cwweapon | cwmineb)) == (actvehicle->typ->weapons->weapon[i].typ & (cwweapon | cwmineb))) && ((actvehicle->typ->weapons->weapon[i].typ & (cwweapon | cwmineb)) > 0))
                                          if ((fld->vehicle->typ->weapons->weapon[j].count > fld->vehicle->ammo[j]) || (mode == 3)) 
                                             { 
                                                fld->a.temp = 2; 
                                                numberoffields++;
                                             } 
                              if ((actvehicle->typ->weapons->weapon[i].typ & cwserviceb) > 0) 
                                 if ( actvehicle->height <= chfahrend || (actvehicle->height == fld->vehicle->height)) {
                                    if ( ((actvehicle->typ->tank > 0) && (fld->vehicle->typ->tank > 0)
                                       && ((fld->vehicle->typ->tank > fld->vehicle->fuel) || (mode == 3))
                                       &&  ((actvehicle->functions & cffuelref) > 0))
                                       ||
                                       ((actvehicle->typ->material > 0) && (fld->vehicle->typ->material > 0)
                                       && ((fld->vehicle->typ->material > fld->vehicle->material) || (mode == 3))
                                       && ((actvehicle->functions & cfmaterialref) > 0)) )
                                       { 
                                          fld->a.temp = 2; 
                                          numberoffields++;
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
                  if ( actvehicle->typ->weapons->weapon[a].typ & cwserviceb) 
                     if ( actvehicle->material ) 
                        if ( actvehicle->fuel ) 
                           f++;
               } 
         if (f < 1) return;
      } 
      if ((md == 2) || (md == 3)) { 
         f = 0; 
         for (a = 0; a < actvehicle->typ->weapons->count ; a++) { 
            if ((actvehicle->typ->weapons->weapon[a].typ & cwserviceb) > 0) {
               if ((actvehicle->typ->tank > 0) && ((actvehicle->functions & cffuelref) > 0)) {
                  if (md == 2) { 
                     if (actvehicle->fuel > 0) 
                        f++;
                  } 
                  else 
                     f++;
               } 
               if ((actvehicle->typ->material > 0) && ((actvehicle->functions & cfmaterialref) > 0)) {
                  if (md == 2) { 
                     if (actvehicle->material > 0) 
                        f++; 
                  } 
                  else 
                     f++; 
               } 
            } 
            if ((actvehicle->typ->weapons->weapon[a].typ & cwammunitionb) > 0)
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
      cleartemps(7); 
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
         if (fld->object && fld->object->mine ) { 
            fld->a.temp = 2; 
            numberoffields++; 
         } 
         else 
            if (mienenlegen) { 
               fld->a.temp = 1; 
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
         if ((eht->typ->weapons->weapon[i].typ & (cwshootableb | cwmineb)) == (cwshootableb | cwmineb)) {
            mienenraeumen = true; 
            if (eht->ammo[i] > 0) 
               mienenlegen = true; 
         } 
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






void  legemine(byte         typ)
{ 
  pvehicle     eht; 
  tputmine     ptm; 
  byte         i; 
  pvehicletype fzt; 

   if (moveparams.movestatus == 0) { 
      eht = getactfield()->vehicle; 
      moveparams.vehicletomove = eht; 
      if (eht == NULL) 
         return;
      if (eht->color != (actmap->actplayer << 3)) 
         return;
      ptm.initpm(typ,eht); 
      ptm.run(); 
      ptm.done(); 
   } 
   else 
      if (moveparams.movestatus == 90) { 
         netlevel += 10;
         eht = moveparams.vehicletomove; 
         if (getactfield()->a.temp ) { 
            int mn = 0;
            pfield fld = getactfield();
            if ( fld->object && fld->object->mine )
               mn = 1;
            if ( mn == 0) { 
               fzt = eht->typ; 
               int  strength = 64;
               for (i = 0; i < fzt->weapons->count ; i++) 
                  if ((fzt->weapons->weapon[i].typ & (cwmineb | cwshootableb)) == (cwmineb | cwshootableb)) {
                     eht->ammo[i]--; 
                     eht->movement -= mineputmovedecrease;
                     strength = eht->weapstrength[i];
                     break; 
                  } 
               int x = getxpos();
               int y = getypos();
               getactfield() -> putmine( actmap->actplayer, typ, cminestrength[typ-1] * strength / 64 );
               logtoreplayinfo ( rpl_putmine, x, y, (int) actmap->actplayer, (int) typ, (int) cminestrength[typ-1] * strength / 64 );

            } 
            else { 
               int x = getxpos();
               int y = getypos();
               getactfield() -> removemine( );
               logtoreplayinfo ( rpl_removemine, x, y );
            } 
            cleartemps(7); 
            computeview(); 
            moveparams.movestatus = 0; 
         } 
         netlevel -= 10;
      } 
} 








void         refuelvehicle(byte         b)
{ 
   pvehicle     actvehicle; 

   if (moveparams.movestatus == 0) { 
      trefuelvehicle rfe;
      rfe.initrefuelling(getxpos(),getypos(),b); 
      rfe.startsuche(); 
      rfe.done(); 
   } 
   else { 
      if (moveparams.movestatus == 65) { 
         if (getactfield()->a.temp > 0) { 
            actvehicle = getfield(moveparams.movesx,moveparams.movesy)->vehicle; 
            verlademunition(getactfield()->vehicle,actvehicle,NULL,3 - b); 
            cleartemps(7); 
            moveparams.movestatus = 0; 
         } 
      } 
      else 
         if (moveparams.movestatus == 66) 
            if (getactfield()->a.temp > 0) { 
               actvehicle = getfield(moveparams.movesx,moveparams.movesy)->vehicle; 
               actvehicle->repairunit( getactfield()->vehicle ); 
               cleartemps(7); 
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
   cleartemps(7); 
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
      int costmultiply = ( 8 + ( fld->movemalus[0] - 8 ) / ( objectbuildmovecost / 8 ) ) *  bridgemultiple / 8;
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

   *movecost =  ( 8 + ( fld->movemalus[0] - 8 ) / ( objectbuildmovecost / 8 ) ) * mvcost  / 8  *  bridgemultiple / 8;
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
      buildstreet.done(); 

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
   cleartemps(7); 
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
      svcf.done(); 

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
         pfield fld = getactfield();

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
   bes.done(); 
}
                 
void tvehicle :: removeview ( void )
{
   tcomputevehicleview bes; 
   bes.init( this, -1 ); 
   bes.startsuche(); 
   bes.done(); 
}


void tbuilding :: addview ( void )
{
   tcomputebuildingview bes; 
   bes.init( this, +1 ); 
   bes.startsuche(); 
   bes.done(); 
}
                 
void tbuilding :: removeview ( void )
{
   tcomputebuildingview bes; 
   bes.init( this, -1 ); 
   bes.startsuche(); 
   bes.done(); 
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

/*            if ( reset )
               for ( int p = 0; p < 8; p++ )
                 if (actmap->player[p].existent) {
                   if (((fld->visible >> (p * 2)) & 3) >= visible_ago) 
                        setvisibility(&fld->visible,visible_ago, p);
               } */
            l++;
         } 
} 

void evaluatevisibilityfield ( pfield fld, int player, int add )
{
   if (((fld->visible >> (player * 2)) & 3) >= visible_ago) 
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
         if ( mine  ||  ( ((fld->object->mine >> 1) & 7 ) == player))
            setvisibility(&fld->visible,visible_all, player);
         else
            setvisibility(&fld->visible,visible_now, player);
      } else
        if (( fld->vehicle  && ( fld->vehicle->color  == player * 8 )) ||
            ( fld->vehicle  && ( fld->vehicle->height  < chschwimmend ) && sonar ) ||
            ( fld->building && ( fld->building->typ->buildingheight < chschwimmend ) && sonar ) ||
            ( fld->vehicle  && ( fld->vehicle->height  >= chsatellit )  && satellite ))
               setvisibility(&fld->visible,visible_all, player);
        else
               setvisibility(&fld->visible,visible_now, player);
   }
}


void evaluateviewcalculation ( void )
{
   for ( int player = 0; player < 8; player++ )
      if ( actmap->player[player].existent ) {
         int add = 0;
         if ( actmap->shareview )
            for ( int i = 0; i < 8; i++ )
               if ( actmap->shareview->mode[i][player] )
                  add |= 1 << i;
      
         int nm = actmap->xsize * actmap->ysize;
         for ( int i = 0; i < nm; i++ )
             evaluatevisibilityfield ( &actmap->field[i], player, add );
      }
}



void         computeview( void )
{ 
   if ((actmap->xsize == 0) || (actmap->ysize == 0)) 
      return;

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
   

   evaluateviewcalculation (  );
} 




  class tsucheerreichbarefielder {
                public:
                               int              strck;
                               int              fieldnum;

                               char             mode;
                               char             tiefe;
                               word             startx, starty;
                               pvehicle         vehicle;
                               int              height;


                               void             init( integer x1, integer y1, const pvehicle evehicle, int hgt );
                               void             start ( void );
                               void             move( int x, int y, int direc, int streck, int fuelneeded );
                               void             done ( void );
                               virtual void     testfield( int  x1, int  y1);
                            };


#ifdef HEXAGON
 int sef_dirnum = 3;
 static const  byte         sef_directions[3]  = {0, 1, 5 };
 static const  byte         sef_searchorder[6]  = {0, 1, 2, 3, 4, 5 };
#else
 int sef_dirnum = 5;
 static const  byte         sef_directions[5]  = {0, 1, 7, 2, 6};
 static const  byte         sef_searchorder[8]  = {0, 2, 4, 6, 1, 3, 5, 7};
#endif


/* static const  signed char  windmovementdecrease[5] = { -50, -30, 20, 90, 128 };
                            Basis 64 */

void         tsucheerreichbarefielder::move(int         x,
                                            int         y,
                                            int         direc,
                                            int         streck,
                                            int         fuelneeded)
{ 

   byte         b, c; 
   pfield        fld; 
   integer      ox, oy;


   tiefe++;

   ox = x; 
   oy = y; 

   getnextfield(x, y, direc);


   if ((x < 0) || (y < 0) || (x >= actmap->xsize) || (y >= actmap->ysize)) 
      return;
   if ((x == startx) && (y == starty)) 
      return;

   fld = getfield(x,y); 
   c = fieldaccessible(fld,vehicle, height ); 


   if (c == 0) 
      return;
   else { 
      int mm1;   // Spritfuelconsumption
      int mm2;   // movementfuelconsumption

      calcmovemalus(ox,oy,x,y,vehicle,direc, mm1, mm2);

      streck -= mm2;
      fuelneeded   += mm1;
   } 
   if (streck < 0) return;
   if ( fuelneeded * vehicle->typ->fuelconsumption / 8 > vehicle->fuel)   return;

   if (mode == 1) { 

      #ifdef fastsearch   
      if ((fld->special & cbmovetempb) != 0) {
         if ((((fld->special & cbmovetempb) >> cbmovetempv) != maindir / 2 + 1) || ((maindir & 1) == 0))
            return;
      } 
      #endif   

      if ((fld->a.temp < streck + 1)) { 
           /*     if ((tiefe > 1) or (mode = 1)) then  */ 
         fld->a.temp = streck + 1; 
         fieldnum++;
      } 
      else 
         return;
   } 
   else { 
      displaymessage (" was soll denn der KÑse hier ?? ", 2 );
      /*
      fld->special |= ((direc / 2 + 1) << cbmovetempv);
      fld->a.temp = 1; 
      fieldnum++;
      */
   } 
   testfield(x,y); 
   if (mode == 1) { 
      for (b = 0; b < sef_dirnum; b++) { 
         c = sef_directions[b] + direc; 

         if (c >= sidenum ) c -= sidenum; 

         move(x,y,c,streck,fuelneeded); 
         tiefe--; 
      } 
   } 
   else { 
      move(x,y,direc,streck, fuelneeded); 
      tiefe--; 
   } 
} 



void         tsucheerreichbarefielder::init( integer x1, integer y1, const pvehicle evehicle, int hgt )
{ 
   startx = x1; 
   starty = y1; 
   vehicle = evehicle; 
   fieldnum = 0;
   height = hgt;
} 




void         tsucheerreichbarefielder::done(void)
{ 
} 


void         tsucheerreichbarefielder::start(void)
{ 
      byte         maindir; 


   cleartemps(5); 
   initwindmovement( vehicle );


   tiefe = 0; 


   #ifdef fastsearch   
   mode = 0; 
   strck = vehicle->movement(); 
   for (maindir = 0; maindir <= 3; maindir++) { 
      move(startx,starty,maindir * 2 + 1,strck);
      tiefe--; 
   } 
   #endif   


   mode = 1; 
   strck = vehicle->movement; 

   for (maindir = 0; maindir < sidenum; maindir++) { 

      move(startx,starty,sef_searchorder[maindir],strck, 0);
      tiefe--;
   } 
   // displaymap(); 
} 


void     tsucheerreichbarefielder :: testfield( int  x1, int  y1) 
{
   x1 += y1;
}

int          sucheerreichbarefielder(integer      x1,
                                    integer      y1,
                                    pvehicle     vehicle,
                                    int          height )

{ 
  tsucheerreichbarefielder sebf; 

   sebf.init( x1, y1, vehicle, height ); 
   sebf.start(); 
   sebf.done(); 
   return sebf.fieldnum;
} 

 typedef byte trichtungen[sidenum]; 

#ifdef HEXAGON
  static const trichtungen  directions[3][3]  = {{{5, 0, 4, 1, 3, 2 }, {0, 1, 5, 2, 4, 3 }, {1, 0, 2, 5, 3, 4 }},
                                                 {{5, 4, 0, 3, 1, 2 }, {0, 1, 2, 3, 4, 5 }, {1, 2, 0, 3, 5, 4 }}, 
                                                 {{4, 3, 5, 2, 0, 1 }, {3, 4, 2, 5, 1, 0 }, {2, 3, 1, 4, 0, 5 }}}; 
#else
  static const trichtungen  directions[3][3]  = {{{7, 6, 0, 5, 1, 4, 2, 3}, {0, 1, 7, 2, 6, 3, 5, 4}, {1, 0, 2, 7, 3, 6, 4, 5}},
                                                 {{6, 7, 5, 0, 4, 1, 3, 2}, {0, 1, 2, 3, 4, 5, 6, 7}, {2, 1, 3, 0, 4, 7, 5, 6}}, 
                                                 {{5, 6, 4, 7, 3, 0, 2, 1}, {4, 3, 5, 2, 6, 1, 7, 0}, {3, 2, 4, 1, 5, 0, 6, 7}}}; 
#endif

class tfieldreachablerek {

      int          distance;
      int          fuelusage;
      integer      maxwegstrecke; 
      tstrecke     shortestway; 
      tstrecke     strecke; 
      byte         c; 
      word         mm, ll; 
      byte         a, b; 
      trichtungen  direc; 
      int          dx, dy;
      boolean      zielerreicht; 
      integer      x1, y1, x2, y2;
      integer      ox, oy, oex, oey;
      pvehicle     oeht;
      pvehicle     vehicle;

      char          mode;
             void         move(int          x,
                                 int          y,
                                 integer      direc,
                                 int          streck,
                                 int          fuel);
    public:
         void         run(integer      x22,
                          integer      y22,
                          pvehicle     eht,
                          byte         md);
 };


 
void   tfieldreachablerek::move(int          x,
                                  int          y,
                                  integer      direc,
                                  int          streck,
                                  int          fuel )
{ 
   byte         b, c; 
   pfield        fld; 
   trichtungen  direc2; 
 
    //displaymap();
    //wait();

    ox = x; 
    oy = y; 
    strecke.tiefe++;
    if ( zielerreicht ) 
       return;

    if (mode == 1)
       if (streck > vehicle->movement)
          return;                      

    mm = minmalq; 
    getnextfield ( x, y, direc);

   #ifndef HEXAGON
    if ( (direc & 1) == 0 )
       mm = maxmalq; 
   #endif

    if ((x < 0) || (y < 0) || (x >= actmap->xsize) || (y >= actmap->ysize)) 
       return;

    if (actmap->weather.wind[ getwindheightforunit ( vehicle ) ].speed && vehicle->height >= chtieffliegend && vehicle->height <= chhochfliegend) {
       ll = windbeeline(x,y,x2,y2);
       if (ll > maxwegstrecke - streck) 
          return; 
    } else {
       ll = beeline(x2,y2,x,y);
       if (ll > maxwegstrecke - streck) 
          return; 
    }

    fld = getfield(x,y); 
    for (b = 1; b < strecke.tiefe ; b++)
       if ((strecke.field[b].x == x) && (strecke.field[b].y == y)) return;

    c = fieldaccessible(fld,vehicle,moveparams.uheight); 

    if (c == 0) 
       return;
    else { 
       int mm1;      // fuelusage
       int mm2;      // movementusage

       calcmovemalus(ox, oy, x, y, vehicle, direc, mm1, mm2);

       fuel += mm1;
       streck += mm2;
    } 
    if (streck > maxwegstrecke) 
       return;

    if (fuel * vehicle->typ->fuelconsumption / minmalq  > vehicle->fuel) 
       return;

    if (mode == 1) 
       if ((fld->a.temp > 0) && (streck > fld->a.temp)) return;

    if ((fld->a.temp > streck) || (fld->a.temp == 0)) 
        fld->a.temp = streck;

    strecke.field[strecke.tiefe].x = x; 
    strecke.field[strecke.tiefe].y = y; 
    if ((x == x2) && (y == y2)) { 
       distance = streck; 
       strecke.fuelremaining = vehicle->fuel - fuel * vehicle->typ->fuelconsumption / minmalq  ;
       if ( strecke.fuelremaining < 0 )
          strecke.fuelremaining = 0;

       strecke.distance = distance;
       shortestway = strecke; 
       if (actmap->weather.wind[ getwindheightforunit ( vehicle ) ].speed && vehicle->height >= chtieffliegend && vehicle->height <= chhochfliegend) {
          if ((mode == 2) || ((mode == 1) && (streck == windbeeline(x1,y1,x2,y2))))
             zielerreicht = true;
       } else {
          if ((mode == 2) || ((mode == 1) && (streck == beeline(x2,y2,x1,y1))))
             zielerreicht = true;
       }
    } 
    else { 
       dx = (2 * x2 + (y2 & 1)) - (2 * x + (y & 1)); 
       dy = y2 - y; 
       if (dx < 0) a = 0; 
       else 
          if (dx == 0) a = 1; 
          else a = 2; 
       if (dy < 0) b = 0; 
       else 
          if (dy == 0) b = 1; 
          else b = 2; 
       memcpy( direc2, directions[b][a], sizeof ( direc2)); 
       for (b = 0; b <= 4; b++) { 
          move(x,y,direc2[b],streck, fuel);
          strecke.tiefe--;
          if (zielerreicht) return;
       } 
    } 
 } 


void         fieldreachablerek(integer      x2,
                               integer      y2,
                               const pvehicle     vehicle,
                               byte         mode) 
{
   tfieldreachablerek ferb;

   ferb.run( x2, y2, vehicle, mode );
}



void         tfieldreachablerek::run(integer      x22,
                                    integer      y22,
                                    pvehicle     eht,
                                    byte         md)

/*  mode :  1 kÅrzesten weg finden
            2 irgendeinen weg finden  */ 
{ 

   x2 = x22;
   y2 = y22;
   mode = md;
   vehicle = eht;

   moveparams.movedist = 0; 
   x1 = vehicle->xpos; 
   y1 = vehicle->ypos; 

   maxwegstrecke = vehicle->movement; 
   zielerreicht = false; 
   distance = 0; 

   dx = (2 * x2 + (y2 & 1)) - (2 * x1 + (y1 & 1)); 
   dy = y2 - y1; 
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

   memcpy (direc, directions[b][a], sizeof (direc)); 
   for (c = 0; c < sidenum; c++) { 
      strecke.tiefe = 0; 
      move(x1,y1,direc[c],0, 0 );
      if ((mode == 2) && zielerreicht) 
         /* if (distance <= maxwegstrecke) {
            moveparams.movepath = shortestway; 
            moveparams.movedist = distance; 
            return;
         }  */
      if ((mode == 1) && zielerreicht) break; 
   } 
   if (mode == 1) { 
      cleartemps(5); 
      if (distance > 0) 
         for (c = 1; c <= shortestway.tiefe; c++) { 
            if ((shortestway.field[c].x >= actmap->xsize) || (shortestway.field[c].y >= actmap->ysize)) { 
               displaymessage("a severe error occured !\nplease switch immediately into debug-mode and check why shortestway.field[c] has invalid values !\n", 1);
            } 
            getfield(shortestway.field[c].x,shortestway.field[c].y)->a.temp = 2; 
         } 
   } 
   moveparams.movedist = distance; 
   moveparams.movepath = shortestway; 
} 

  /* 


procedure fieldreachable(x1,y1,x2,y2:integer; vehicle:pvehicle; strecke:integer);
var
  b:boolean;
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
                      if ( delta == 0 )
                         return 0;

                      if ( delta > 0 && delta <= pi )
                         return 1;
                      if ( delta < 0 && delta >= -pi )
                         return -1;

                      if ( delta > 0 && delta > pi )
                         return -1;
                      if ( delta < 0 && delta < -pi )
                         return 1;
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
   at -= offset;
   while ( at < 0 )
      at += 2 * 3.14159265;

   return at;
}

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


   int lastdir = winkelcomp ( w, dirs[dir] );

   getnextfield( x, y, dir );
   while ( x != x2 || y != y2 ) {
       putpix8 ( x, y );
       double w2 = winkel ( x, y );
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
      cleartemps ( 7 );
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
      if ( xp == 15 && yp == 52 ) {
         int a = 3;
         int b = 4;
         int c = a * b;
         }

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

   if ( eht->functions & cfautodigger )
      searchforminablefields ( eht );

   pfield efield = getfield( eht->xpos, eht->ypos ); 
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









class treactionfire {
          public:
             virtual void checkfield ( int x, int y, pvehicle &eht ) = 0;
             virtual void init ( pvehicle eht ) = 0;
        };

class treactionfirereplay : public treactionfire {
          protected:
             int num;
             dynamic_array<preactionfire_replayinfo> replay;
             pvehicle unit;
          public:
             treactionfirereplay ( void );
             ~treactionfirereplay ( );
             virtual void checkfield ( int x, int y, pvehicle &eht );
             virtual void init ( pvehicle eht );
   };

treactionfirereplay :: treactionfirereplay ( void )
{
   num = 0;
   unit = NULL;
}

void treactionfirereplay :: init ( pvehicle eht )
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

void treactionfirereplay :: checkfield ( int x, int y, pvehicle &eht )
{
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


             if ( attackvisible ) {
               cursor.setcolor ( 8 );  

               cursor.gotoxy ( rpli->x1, rpli->y1 );
               int t = ticker;
               while ( t + 15 > ticker );

               cursor.gotoxy ( x, y );
               t = ticker;
               while ( t + 15 > ticker );

               cursor.setcolor ( 0 );  
               cursor.hide();
             }

             tunitattacksunit battle;
             battle.setup ( fld->vehicle, targ->vehicle, 1, rpli->wpnum );
             battle.av.damage = rpli->ad1;
             battle.dv.damage = rpli->dd1;
             if ( attackvisible ) 
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


class tsearchreactionfireingunits : public treactionfire {
           protected:


                static int maxshootdist[8];     // fÅr jede Hîhenstufe eine
                void addunit ( pvehicle vehicle );
                void removeunit ( pvehicle vehicle );
           public:

                tsearchreactionfireingunits( void );
                void init ( pvehicle eht );
                void checkfield ( int x, int y, pvehicle &eht );  
                ~tsearchreactionfireingunits();
      };

int tsearchreactionfireingunits :: maxshootdist[8] = { -1, -1, -1, -1, -1, -1, -1, -1 };

tsearchreactionfireingunits :: tsearchreactionfireingunits ( void )
{
   for ( int i = 0; i < 8; i++ )
      unitlist[i] = NULL;
}

void tsearchreactionfireingunits :: init ( pvehicle vehicle )
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
               if ( fzt->weapons->weapon[j].typ & cwshootableb )
                  for (h = 0; h < 8; h++ )
                     if ( fzt->weapons->weapon[j].targ & ( 1 << h ) )
                        if ( fzt->weapons->weapon[j].maxdistance > maxshootdist[h] )
                           maxshootdist[h] = fzt->weapons->weapon[j].maxdistance;
      }
   }

   for ( i = 0; i <= moveparams.movepath.tiefe; i++) { 
      if ( moveparams.movepath.field[i].x > x2 )
         x2 = moveparams.movepath.field[i].x ;
      if ( moveparams.movepath.field[i].y > y2 )
         y2 = moveparams.movepath.field[i].y ;

      if ( moveparams.movepath.field[i].x < x1 )
         x1 = moveparams.movepath.field[i].x ;
      if ( moveparams.movepath.field[i].y < y1 )
         y1 = moveparams.movepath.field[i].y ;
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


void tsearchreactionfireingunits :: checkfield ( int x, int y, pvehicle &vehicle )
{
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
            if ( atw->count ) {

               int ad1, ad2, dd1, dd2;

               int strength = 0;
               int num;
               for ( int j = 0; j < atw->count; j++ )
                  if ( atw->strength[j]  > strength ) {
                     strength = atw->strength[j];
                     num = j;
                  }

               displaymessage2 ( "attacking with weapon %d ", atw->num[num] );
               cursor.setcolor ( 8 );  

               cursor.gotoxy ( ul->eht->xpos, ul->eht->ypos );
               int t = ticker;
               while ( t + 15 > ticker );

               cursor.gotoxy ( x, y );
               t = ticker;
               while ( t + 15 > ticker );

               cursor.setcolor ( 0 );  
               cursor.hide();

               tunitattacksunit battle;
               battle.setup ( ul->eht, fld->vehicle, 0, atw->num[num] );

               ad1 = battle.av.damage;
               dd1 = battle.dv.damage;
               battle.calcdisplay ();
               ad2 = battle.av.damage;
               dd2 = battle.dv.damage;

               if ( battle.dv.damage >= 100 )
                  result = 1;

               battle.setresult();

               logtoreplayinfo ( rpl_reactionfire, ul->eht->xpos, ul->eht->ypos, x, y, ad1, ad2, dd1, dd2, atw->num[num] );

               dashboard.x = 0xffff;
               ul->eht->reactionfire -= 1 << (vehicle->color / 8);
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
                                   
void         moveunitxy(pvehicle     vehicle,
                        word         xt1,
                        word         yt1)
{ 
       #ifdef logging
       {
           char tmpcbuf[200];
           logtofile ( "controls / moveunitxy / entering moveunitxy");
       }    
       #endif

  cursor.hide();

  pfield        oldfield;
  pfield        field3;
  word         i; 
  word         x, y; 
  byte         dir; 


   pfield fld = getfield(xt1,yt1); 
   for (i = 1; i <= moveparams.movepath.tiefe; i++) { 
      if ((moveparams.movepath.field[i].x == xt1) && (moveparams.movepath.field[i].y == yt1)) 
         break; 
   } 

   if (fld->a.temp == 0) { 
      dispmessage2(105,""); 
      return;
   } 
   if (fieldaccessible(fld,vehicle, moveparams.uheight) < 2) { 
      dispmessage2(106,""); 
      return;
   } 

   if (i < moveparams.movepath.tiefe) { 
      cleartemps(5); 
      fieldreachablerek(xt1,yt1,vehicle,1); 
      if (moveparams.movedist < 0) 
         moveparams.movedist = 0; 
      displaymap(); 
      moveparams.moveerr = 0; 
      return;
   } 

   oldfield = getfield(moveparams.movesx,moveparams.movesy); 

   netlevel += 10;

   moveparams.moveerr = 0; 
   push(tempsvisible,sizeof(tempsvisible)); 
   tempsvisible = false; 


   moveparams.movepath.field[0].x = moveparams.movesx; 
   moveparams.movepath.field[0].y = moveparams.movesy; 

   x = moveparams.movepath.field[0].x; 
   y = moveparams.movepath.field[0].y; 

   tsearchreactionfireingunits srfu;
   treactionfirereplay rfr;

   treactionfire* rf;
   if ( runreplay.status > 0  )
      rf = &rfr;
   else
      rf = &srfu;

   rf->init( vehicle );


     /*  herkunft der vehicle ermitteln und auf NULL setzen  */ 

   if (oldfield->vehicle == vehicle) {
      vehicle->removeview();
      oldfield->vehicle = NULL; 
   } else { 
      if ( oldfield->vehicle ) { 
         i = 0; 
         while (oldfield->vehicle->loading[i] != vehicle) 
            i++; 
         oldfield->vehicle->loading[i] = NULL; 
      } 
      else 
         if ( oldfield->building ) { 
            i = 0; 
            while (oldfield->building->loading[i] != vehicle) 
               i++; 
            oldfield->building->loading[i] = NULL; 
         } 
   } 

//   displaymap();


   /*  vehicle moven  */ 


   i = 0; 
   while (((x != xt1) || (y != yt1)) && (vehicle != NULL)) { 

      pfield fld1 = getfield ( moveparams.movepath.field[i].x,   moveparams.movepath.field[i].y );
      pfield fld2 = getfield ( moveparams.movepath.field[i+1].x, moveparams.movepath.field[i+1].y );

      int view1 = (fld1->visible >> ( actmap->playerview * 2)) & 3; 
      if ( godview ) 
         view1 = visible_all; 

      int view2 = (fld2->visible >> ( actmap->playerview * 2)) & 3; 
      if ( godview ) 
         view2 = visible_all; 

      if (  view1 >= visible_now  &&  view2 >= visible_now ) 
         if ( ((vehicle->height >= chschwimmend) && (vehicle->height <= chhochfliegend)) || (( view1 == visible_all) && ( view2 == visible_all )) || ( actmap->actplayer == actmap->playerview ))
            idisplaymap.movevehicle(moveparams.movepath.field[i].x, moveparams.movepath.field[i].y, moveparams.movepath.field[i + 1].x, moveparams.movepath.field[i + 1].y, vehicle);

      if (vehicle->functions & ( cffahrspur | cficebreaker ))  {
        dir = getdirection(moveparams.movepath.field[i].x, moveparams.movepath.field[i].y, moveparams.movepath.field[i + 1].x, moveparams.movepath.field[i + 1].y);
        field3 = getfield(x,y);
      
        if ( vehicle->functions & cffahrspur )
           if ( fahrspurobject )
              if ( field3->bdt & cbfahrspur )
                 if ( ! (actmap->objectcrc   &&   !actmap->objectcrc->speedcrccheck->checkobj2 ( fahrspurobject, 2 ))) 
                    field3 -> addobject ( fahrspurobject, 1 << dir );

        if ( vehicle->functions & cficebreaker )
           if ( eisbrecherobject )
              if ( ! (actmap->objectcrc   &&   !actmap->objectcrc->speedcrccheck->checkobj2 ( eisbrecherobject, 2 ))) 
                 if ( (field3->bdt & cbsnow1 )  || ( field3->bdt & cbsnow2 ) || field3->checkforobject ( eisbrecherobject ) ) {
                    getfield ( x, y ) -> addobject ( eisbrecherobject, 1 << dir );
                    field3->checkforobject ( eisbrecherobject )->time = actmap->time.a.turn;
                 }

      } 

      i++;
      x = moveparams.movepath.field[i].x; 
      y = moveparams.movepath.field[i].y; 

      field3 = getfield(x,y); 
      if (vehicle->functions & cffahrspur) {
         if ( fahrspurobject )
            if ( field3->bdt & cbfahrspur )
               if ( ! (actmap->objectcrc   &&   !actmap->objectcrc->speedcrccheck->checkobj2 ( fahrspurobject, 2 ))) 
                  if (dir >= sidenum/2) 
                    getfield ( x, y ) -> addobject ( fahrspurobject, 1 << (dir - sidenum/2));
                  else 
                    getfield ( x, y ) -> addobject ( fahrspurobject, 1 << (dir + sidenum/2));
      } 
      if ( vehicle->functions & cficebreaker )
         if ( eisbrecherobject )
            if ( ! (actmap->objectcrc   &&   !actmap->objectcrc->speedcrccheck->checkobj2 ( eisbrecherobject, 2 ))) 
                 if ( (field3->bdt & cbsnow1 )  || ( field3->bdt & cbsnow2 ) || field3->checkforobject ( eisbrecherobject ) ) {
                  if (dir >= sidenum/2) 
                    getfield ( x, y ) -> addobject ( eisbrecherobject, 1 << (dir - sidenum/2));
                  else 
                    getfield ( x, y ) -> addobject ( eisbrecherobject, 1 << (dir + sidenum/2));
      
                    field3->checkforobject ( eisbrecherobject )->time = actmap->time.a.turn;
      
               }

      if ( vehicle ) {
         vehicle->xpos = x;
         vehicle->ypos = y;
         vehicle->addview();
      }

      evaluateviewcalculation (  );

      rf->checkfield ( x, y, vehicle );

      if ( !vehicle )
         idisplaymap.deletevehicle();

      displaymap(); 


      /*
      if ( vehicle ) 
         if ( vehicleplattfahrbar(vehicle,field3) ) 
            bulldoze_trooper.fight( &vehicle, &field3->vehicle);
      */

      if ( vehicle ) {
         vehicle->removeview();
         if ( field3->object && field3->object->mine && (vehicle->height <= chfahrend) && (( !(vehicle->functions & cf_trooper) ) ||  ((field3->object->mine >> 4 ) == cmantipersonnelmine ))) {
            tmineattacksunit battle;
            battle.setup ( field3, vehicle );

            if ( fieldvisiblenow ( getfield ( x, y ), actmap->playerview) || ((field3->object->mine >> 1) % 7) == actmap->playerview )
               battle.calcdisplay ();
            else
               battle.calc();

            battle.setresult ();
            dashboard.x = 0xffff;
        }
      }
      if ( vehicle ) {
         npush ( field3->vehicle );
         field3->vehicle = vehicle;
         if ( field3->connection & cconnection_areaentered_anyunit )
            releaseevent ( vehicle, NULL, cconnection_areaentered_anyunit );

         if ((field3->connection & cconnection_areaentered_specificunit ) && ( vehicle->connection & cconnection_areaentered_specificunit ))
            releaseevent ( vehicle, NULL, cconnection_areaentered_specificunit );
         npop ( field3->vehicle );
      }
   }                           

   if ( vehicle ) { 
      if ((fld->vehicle == NULL) && (fld->building == NULL)) {
         fld->vehicle = vehicle; 
         vehicle->addview();
      } else { 
         if ( fld->vehicle  &&  fld->vehicle->typ->loadcapacity ) { 
            i = 0; 
            while ((fld->vehicle->loading[i] != NULL) && (i < 31)) 
              i++; 
            fld->vehicle->loading[i] = vehicle; 

           /*      Umladen der geladenen vehicle in den Transporter
            if (vehicle->typ->loadcapacity > 0) { 
               for (j = 0; j <= 31; j++) 
                  if (vehicle->loading[j] != NULL) { 
                     while ((fld->building->loading[i] != NULL) && (i < 31)) 
                        i++;               
                     vehicle->loading[j]->xpos = getxpos();
                     vehicle->loading[j]->ypos = getypos();
                     fld->vehicle->loading[i] = vehicle->loading[j]; 
                     vehicle->loading[j] = NULL; 
                  } 
            } 
            */
                                            
         }                                                           
         else 
            if ( fld->building ) { 
               i = 0; 
               while ( fld->building->loading[i]  && (i < 31)) 
                  i++; 
               fld->building->loading[i] = vehicle; 
               if (fld->building->color != (actmap->actplayer << 3)) 
                  fld->building->convert( actmap->actplayer ); 

            } 
      } 


      vehicle->fuel = moveparams.movepath.fuelremaining;

         if (fld->vehicle == vehicle) { 
            i = vehicle->movement - moveparams.movedist;
            if (i > 0) 
              vehicle->movement = i;
            else 
              vehicle->movement = 0;

            if ( (vehicle->movement >> 3) > (vehicle->fuel / vehicle->typ->fuelconsumption) )
               vehicle->movement = (vehicle->fuel << 3) / vehicle->typ->fuelconsumption;
         } 
         else { 
            vehicle->movement = 0;
            vehicle->attacked = true; 
            if (vehicle->height == chtieffliegend) 
               vehicle->height = chfahrend; 
         } 

         vehicle->setnewposition ( xt1, yt1 );

      
      dashboard.x = 0xffff; 
      if (moveparams.movestatus == 12) { 
         moveparams.movestatus = 0; 
         int res = changeheight(vehicle,moveparams.heightdir, 100 ); 
         if ( res )
            dispmessage2( res, NULL );
      } 
   } 
   netlevel -= 10;
   idisplaymap.resetmovement();
   computeview(); 
   displaymap(); 
   cleartemps(7); 
   moveparams.movestatus = 0; 
   moveparams.uheight = -1;
   pop(tempsvisible,sizeof(tempsvisible)); 

   if ( fieldvisiblenow ( getactfield(), actmap->playerview ))
      cursor.show();

} 



void         movement(pvehicle     vehicle, int height )
{ 
   pfield        fld;
   pfield        oldfield;

   if (moveparams.moveerr >= 5) { 
      cleartemps(7); 
      displaymap(); 
      moveparams.movestatus = 0; 
      moveparams.moveerr = 0; 
      moveparams.vehicletomove = NULL; 
      moveparams.uheight = -1;
   } 
   else 
      if (moveparams.movestatus == 0) { 
         cleartemps(7); 
         if ( vehicle ) { 
            moveparams.movesx = vehicle->xpos; 
            moveparams.movesy = vehicle->ypos; 
            if (vehicle->movement >= minmalq) { 
               if (vehicle->color == (actmap->actplayer << 3)) { 
                  moveparams.vehicletomove = vehicle; 
                  int fieldnum = sucheerreichbarefielder(vehicle->xpos,vehicle->ypos,vehicle, height ); 
                    /*             move(vehicle,vehicle^.movement shr 3+1,0,vehicle^.movement);  */ 
                  if ( fieldnum ) {
                     moveparams.moveerr = 0; 
                     moveparams.movestatus = 1; 
                     moveparams.uheight = height;
                  } else
                     dispmessage2( 107, NULL );
               } 
               else 
                  dispmessage2(101,""); 
            } 
            else 
               if (vehicle->typ->movement[log2(vehicle->height)] < minmalq ) 
                  dispmessage2(102,""); 
               else 
                  dispmessage2(103,""); 
         } 
         else 
            dispmessage2(104,""); 
      } 
      else 
         if ((moveparams.movestatus == 1) || (moveparams.movestatus == 11)) {
            vehicle = moveparams.vehicletomove; 
            fld = getactfield(); 
            if ( height == -1)
               height = moveparams.uheight;
            else
               moveparams.uheight = height;

            oldfield = getfield(moveparams.movesx,moveparams.movesy); 
            if ((fieldaccessible(fld,vehicle, height) == 2) && (fld->a.temp > 0)) { 
               moveparams.movestatus++;
               if (moveparams.movestatus == 1) 
                  cleartemps(7); 
               else 
                  cleartemps(5); 
               cursor.hide(); 
                 /*  displaymap;  */ 
               fieldreachablerek(getxpos(),getypos(),vehicle,1); 
               if (moveparams.movedist < 0) {
                  displaymessage("WARNING !\n void movement \n negative movedist !",1);
                  moveparams.movedist = 0; 
               }
               displaymap(); 
               cursor.show(); 
               moveparams.moveerr = 0; 
            } 
            else { 
               if (fld->a.temp == 0) 
                  dispmessage2(105,""); 
               else 
                  dispmessage2(106,""); 
               moveparams.moveerr++;
            } 
         } 
         else 
            if (( moveparams.movestatus == 2) || (moveparams.movestatus == 12))
               if (getactfield()->a.temp > 0) { 

       #ifdef logging
       {
           char tmpcbuf[200];
           logtofile ( "controls / movement / movementmode 2|12 ");
       }    
       #endif
                  int x1 = moveparams.vehicletomove->xpos;
                  int y1 = moveparams.vehicletomove->ypos;
                  int x2 = getxpos();
                  int y2 = getypos();

                  if ( moveparams.movestatus == 2 )
                     logtoreplayinfo ( rpl_move2, x1, y1, x2, y2, moveparams.vehicletomove->networkid, (int) moveparams.vehicletomove->height );
                  else
                     logtoreplayinfo ( rpl_changeheight, x1, y1, x2, y2, moveparams.vehicletomove->networkid, (int) moveparams.oldheight, (int) moveparams.newheight );

                  moveunitxy(moveparams.vehicletomove, x2, y2 );
                  actmap->time.a.move++;
               } 
               else 
                  moveparams.moveerr++; 
            else 
               moveparams.moveerr++; 
} 



void         moveheight(pvehicle     vehicle)
{ 
   pfield        fld; 
   int          x, xx, y, yy; 
   byte         direc; 
   boolean      ok, ok2; 

   initwindmovement( vehicle );

   ok2 = false; 
   cleartemps(7); 
   x = getxpos();
   xx = x; 
   y = getypos();
   yy = y; 
   for (direc = 0; direc < sidenum; direc++) { 
      ok = true; 
      x = xx; 
      y = yy; 

      int dist = 0;
      int mx = vehicle->movement;
      while ( dist < vehicle->typ->steigung * minmalq  && mx > 0 ) {
         getnextfield( x, y, direc );

         dist += minmalq - windmovement[direc];
         mx -= minmalq - windmovement[direc];

         if ((x < 0) || (y < 0) || (x >= actmap->xsize) || (y >= actmap->ysize)) 
            ok = false; 
         else { 
            fld = getfield(x,y); 
            if (fieldaccessible(fld,vehicle) < 1) 
               ok = false; 
         } 
      } 
      if ( mx < 0 )
         ok = false;

/*
      if ( fieldaccessible( getfield(x,y), vehicle ) < 2) 
         ok = false;
*/
      if ( ok )
         if ( fld->building || fld->vehicle )
            ok = false;

      if (ok) { 
         fld->a.temp = 2; 
         fld->a.temp2 = 2; 
         ok2 = true; 
      } 

   } 
   if ( ok2 ) {
      moveparams.movestatus = 11; 
      moveparams.uheight = -1;
   }
} 




void         tsearchattackablevehicles::init( const pvehicle     eht)
{ 
   messages = true; 
   angreifer = eht; 
   kamikaze = false; 
}



void         tsearchattackablevehicles::testfield(void)
{ 
  pattackweap  atw; 

   if (fieldvisiblenow(getfield(xp,yp))) { 
      atw = attackpossible(angreifer,xp,yp); 
      if (atw->count > 0) { 
         getfield(xp,yp)->a.temp = dist; 
         anzahlgegner++;
      } 
      delete atw;
   } 
} 


void         tsearchattackablevehicles::initattacksearch(void)
{ 
  integer      ma, mi, a, d; 

     /* cleartemps; */ 

   attackposs = false; 
   anzahlgegner = 0; 

   if (angreifer == NULL) { 
      if (messages) 
         dispmessage2(201,getactfield()->typ->terraintype->name); 
      return;
   } 

   moveparams.movesx = angreifer->xpos; 
   moveparams.movesy = angreifer->ypos; 
   if (fieldvisiblenow(getfield(moveparams.movesx,moveparams.movesy)) == false) 
      return;


   if (angreifer->attacked) { 
      if (messages) 
         dispmessage2(202,""); 
      return;
   } 

   if (angreifer->typ->weapons->count == 0) { 
      if (messages) 
         dispmessage2(204,""); 
      return;
   } 
   if (angreifer->typ->wait &&  angreifer->reactionfire_active < 3 ) 
      if (angreifer->movement != angreifer->typ->movement[log2(angreifer->height)]) { 
         if (messages) 
            dispmessage2(205,""); 
         return;
      } 

   d = 0; 
   ma = 0; 
   mi = 20000; 
   for (a = 0; a < angreifer->typ->weapons->count; a++) { 
      if (angreifer->ammo[a] > 0) 
         d++; 

         if ((angreifer->typ->weapons->weapon[a].maxdistance >> 3) > ma) ma = (angreifer->typ->weapons->weapon[a].maxdistance >> 3);
         if ((angreifer->typ->weapons->weapon[a].mindistance >> 3) < mi) mi = (angreifer->typ->weapons->weapon[a].mindistance >> 3);
      
   } 

   if (d == 0) { 
      if (messages) 
         dispmessage2(204,""); 
      return;
   } 
   attackposs = true; 
   initsuche(angreifer->xpos,angreifer->ypos,ma + 1,mi); 
} 


void         tsearchattackablevehicles::startsuche(void)
{ 
   if (attackposs) { 
      tsearchfields::startsuche();
      if (anzahlgegner > 0) { 
         moveparams.movestatus = 10; 
         displaymap(); 
      } 
      else 
         if (messages) 
            dispmessage2(206,""); 
   } 
} 


void         tsearchattackablevehicles::run(void)
{ 
   initattacksearch(); 
   startsuche(); 
} 


void         tsearchattackablevehicleskamikaze::init(  const pvehicle eht )
{ 
   tsearchattackablevehicles::init(eht);
   kamikaze = true; 
} 


void         tsearchattackablevehicleskamikaze::initattacksearch(void)
{ 


   if (angreifer == NULL) { 
      if (messages) 
         dispmessage2(201,getactfield()->typ->terraintype->name); 
      return;
   } 

     /* cleartemps; */ 

   attackposs = false; 
   anzahlgegner = 0; 

   moveparams.movesx = angreifer->xpos; 
   moveparams.movesy = angreifer->ypos; 

   if (angreifer->attacked) { 
      if (messages) 
         dispmessage2(202,""); 
      return;
   } 
   if (angreifer->typ->wait) 
      if (angreifer->movement != angreifer->typ->movement[log2(angreifer->height)]) { 
         if (messages) 
            dispmessage2(205,""); 
         return;
      } 


   attackposs = true; 
   initsuche(angreifer->xpos,angreifer->ypos,1,1); 
} 


void         tsearchattackablevehicleskamikaze::testfield(void)
{ 
 pvehicle     eht; 

   if (fieldvisiblenow(getfield(xp,yp))) { 
      eht = getfield(xp,yp)->vehicle; 
      if (eht != NULL) 
         if (((angreifer->height >= chtieffliegend) && (eht->height <= angreifer->height) && (eht->height >= chschwimmend)) 
           || ((angreifer->height == chfahrend) && (eht->height == chfahrend)) 
           || ((angreifer->height == chschwimmend) && (eht->height == chschwimmend))
           || ((angreifer->height == chgetaucht) && (eht->height >=  chgetaucht) && (eht->height <= chschwimmend))) {
            getfield(xp,yp)->a.temp = dist; 
            anzahlgegner++; 
         } 
   } 
} 



void         attack(boolean      kamikaze, int  weapnum )
{                                            

   if (moveparams.movestatus == 0) { 
      if (fieldvisiblenow(getactfield())) {
         if (getactfield()->vehicle != NULL) { 
            if (getactfield()->vehicle->color != (actmap->actplayer << 3)) { 
               dispmessage2(203,""); 
               return;
            } 
            cleartemps(7); 
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
          pvehicle vehicle1 = fld->vehicle;
          pfield    targ = getactfield();

          int x2 = getxpos();
          int y2 = getypos();
          int ad1, ad2, dd1, dd2;

          if ( targ->vehicle ) {
             tunitattacksunit battle;
             battle.setup ( fld->vehicle, targ->vehicle, 1, weapnum );
             ad1 = battle.av.damage;
             dd1 = battle.dv.damage;
             battle.calcdisplay ();
             ad2 = battle.av.damage;
             dd2 = battle.dv.damage;
             battle.setresult ();
             dashboard.x = 0xffff;
          } else 
          if ( targ->building ) {
             tunitattacksbuilding battle;
             battle.setup ( fld->vehicle, getxpos(), getypos() , weapnum );
             ad1 = battle.av.damage;
             dd1 = battle.dv.damage;
             battle.calcdisplay ();
             ad2 = battle.av.damage;
             dd2 = battle.dv.damage;
             battle.setresult ();
             dashboard.x = 0xffff;
          } else
          if ( targ->object ) {
             tunitattacksobject battle;
             battle.setup ( fld->vehicle, getxpos(), getypos() , weapnum );
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

          cleartemps(7); 
          dashboard.x = 0xffff; 
          moveparams.movestatus = 0; 

      } 
} 

int squareroot ( int i )
{
   double b = (double) i;
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
         mm2 = getfield(x2,y2)->movemalus[ vehicle->typ->movemalustyp ];
   } else {
      mm1 = maxmalq;
      if (vehicle->height >= chtieffliegend)
         mm2 = maxmalq;
      else
         mm2 = getfield(x2,y2)->movemalus[ vehicle->typ->movemalustyp ] * maxmalq / minmalq;
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
              fld2->vehicle = vehicle;
              atw = attackpossible(fld->vehicle,x2,y2);
              if (atw->count > 0)
                 mm2 += movemalus[mode][d];
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
      ekost = int(building->damage) * building->typ->produktionskosten.sprit / (100 * reparierersparnis); 
      mkost = int(building->damage) * building->typ->produktionskosten.material / (100 * reparierersparnis); 

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
      if ( weap->typ & cwammunitionb )
         putimage ( agmp->resolutionx - ( 640 - 465), 93 + h * 13, xlatpict ( &xlattables.light, icons.unitinfoguiweapons[ log2 ( weap->typ & (cwweapon | cwmineb) ) ] ));
      else
         putimage ( agmp->resolutionx - ( 640 - 465), 93 + h * 13, icons.unitinfoguiweapons[ log2 ( weap->typ & (cwweapon | cwmineb) ) ] );

      paintweaponammount( h, num, weap->count );

      activefontsettings.background = 172;
      activefontsettings.justify = righttext;
      activefontsettings.font = schriften.guifont;
      activefontsettings.height = 9;
      activefontsettings.length = 27;
      if ( weap->typ & cwshootableb ) {
         showtext2c( strrr(strength), agmp->resolutionx - ( 640 - 503), 93 + h * 13);

         weaps[h].displayed = 1;
         weaps[h].maxstrength = strength * weapdist->getweapstrength(weap, weap->mindistance, -1, -1 ) / 255;
         weaps[h].minstrength = strength * weapdist->getweapstrength(weap, weap->maxdistance, -1, -1 ) / 255;
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
                if ( vt->weapons->weapon[j].typ & cwserviceb) {
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
             if ( vt->weapons->weapon[j].typ & (cwweapon | cwmineb )) 
                count++;
             else 
                if (vt->weapons->weapon[j].typ & cwserviceb) 
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

       setinvisiblemouserectanglestk ( x1, y1, x1 + 640, y1 + count * 25 + 40 );
       void* imgbuf = asc_malloc ( imagesize ( x1, y1, x1 + 640, y1 + count * 25 + 40 ));
       getimage ( x1, y1, x1 + 640, y1 + count * 25 + 40, imgbuf );

       putimage ( x1, y1, icons.weaponinfo[0] );

       getinvisiblemouserectanglestk ();


       if ( vt->weapons->count ) 
          for ( int j = 0; j < vt->weapons->count ; j++) { 
             if ( vt->weapons->weapon[j].typ & (cwweapon | cwmineb )) { 
                int maxstrength = vt->weapons->weapon[j].maxstrength;
                int minstrength = vt->weapons->weapon[j].minstrength;
                if ( vehicle && maxstrength ) {
                   minstrength = minstrength * vehicle->weapstrength[j] / maxstrength;
                   maxstrength = vehicle->weapstrength[j];
                }

                paintlargeweapon(i, cwaffentypen[ log2 ( vt->weapons->weapon[j].typ & (cwweapon | cwmineb ))], 
                               ( vehicle ? vehicle->ammo[j] : vt->weapons->weapon[j].count ) , vt->weapons->weapon[j].count,
                               vt->weapons->weapon[j].typ & cwshootableb, vt->weapons->weapon[j].typ & cwammunitionb, 
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
          paintlargeweapon(i, cdnames[ 0 ], ( vehicle ? vehicle->energy : vt->energy ), vt->energy, -1, -1, -1, -1, -1, -1, -1, -1 );
          i++; 
       } 

       if ( (serv>= 0 || (funcs & cfmaterialref)) && vt->material ) { 
          paintlargeweapon(i, cdnames[ 1 ], ( vehicle ? vehicle->material : vt->material ), vt->material, -1, -1, -1, -1, -1, -1, -1, -1 );
          i++; 
       } 
       if ( (serv>= 0 || (funcs & cffuelref)) && vt->tank ) { 
          paintlargeweapon(i, cdnames[ 2 ], ( vehicle ? vehicle->fuel : vt->tank ), vt->tank, -1, -1, -1, -1, -1, -1, -1, -1 );
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
         for ( int j = 0; j < vt->weapons->count ; j++) {
            int x = (agmp->resolutionx - 640) / 2;
            int y = 150 + 28 + j * 14;
            if ( vt->weapons->weapon[j].typ & (cwweapon | cwmineb )) 
               if ( vt->weapons->weapon[j].typ & cwshootableb ) 
                  if ( mouseinrect ( x, y, x + 640, y+ 14 ))
                     topaint = j;
         }
         if ( topaint != lastpainted ) {
            if ( topaint == -1 )
               paintlargeweaponefficiency ( i, NULL, first );
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
   
               paintlargeweaponefficiency ( i, effic, first );
            }
            lastpainted = topaint;
            first = 0;
         }
      }

      setinvisiblemouserectanglestk ( x1, y1, x1 + 640, y1 + count * 25 + 40 );
      putimage ( x1, y1, imgbuf );
      getinvisiblemouserectanglestk ();

      asc_free  ( imgbuf );

      npop ( activefontsettings );
   }

}

void         tdashboard::paintlargeweaponefficiency ( int pos, int* e, int first )
{
   int x = (agmp->resolutionx - 640) / 2;
   int y = 150 + 28 + pos * 14;

   int height, width;
   getpicsize ( icons.weaponinfo[1], width, height );

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

void         tdashboard::checkformouse ( void )
{

    if ( mouseinrect ( agmp->resolutionx - ( 800 - 612), 213, agmp->resolutionx - ( 800 - 781), 305 ) && (mouseparams.taste == 2)) {
       gameoptions.smallmapactive = !gameoptions.smallmapactive;
       gameoptions.changed = 1;

       if ( gameoptions.smallmapactive )
          dashboard.paintsmallmap( 1 );
       else
          dashboard.paintwind( 1 );

       while ( mouseparams.taste == 2 );
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

    if ( smallmap  &&  gameoptions.smallmapactive )
       smallmap->checkformouse();

    if ( !gameoptions.smallmapactive ) {
       if ( mouseparams.x >= agmp->resolutionx - ( 640 - 588 )   &&   mouseparams.x <= agmp->resolutionx - ( 640 - 610 )  &&   mouseparams.y >= 227   &&   mouseparams.y <= 290  && (mouseparams.taste & 1) ) {
          displaywindspeed();
          while ( mouseparams.x >= agmp->resolutionx - ( 640 - 588 )  &&   mouseparams.x <= agmp->resolutionx - ( 640 - 610 )  &&   mouseparams.y >= 227   &&   mouseparams.y <= 290  && (mouseparams.taste & 1) ) ;
       }
       if ( mouseinrect ( agmp->resolutionx - ( 640 - 489 ), 284, agmp->resolutionx - ( 640 - 509 ), 294 ) && (mouseparams.taste & 1)) {
          dashboard.windheight++;
          if ( dashboard.windheight > 2 )
             dashboard.windheight = 0;
          dashboard.x = 0xffff;
          while ( mouseinrect ( agmp->resolutionx - ( 640 - 489 ), 284, agmp->resolutionx - ( 640 - 509 ), 294 ) && (mouseparams.taste & 1) );
       }
    }

    if ( mouseparams.x >= agmp->resolutionx - ( 640 - 578 )   &&   mouseparams.x <= agmp->resolutionx - ( 640 - 609 )  &&   mouseparams.y >=  59   &&   mouseparams.y <=  67  && (mouseparams.taste & 1) ) {
       dashboard.movedisp = !dashboard.movedisp;
       dashboard.x = 0xffff;
       while ( mouseparams.x >= agmp->resolutionx - ( 640 - 578 )   &&   mouseparams.x <= agmp->resolutionx - ( 640 - 609 )  &&   mouseparams.y >=  59   &&   mouseparams.y <=  67  && (mouseparams.taste & 1) ) ;
    }

    for ( int i = 0; i < 8; i++ ) {
       if ( dashboard.weaps[i].displayed )
          if ( mouseinrect ( agmp->resolutionx - ( 640 - 502 ), 92 + i * 13, agmp->resolutionx - ( 640 - 572 ), 102 + i * 13 ) && (mouseparams.taste == 1)) {
             char tmp1[100];
             char tmp2[100];
             strcpy ( tmp1, strrrd8d ( dashboard.weaps[i].maxdist ));
             strcpy ( tmp2, strrrd8u ( dashboard.weaps[i].mindist ));
             displaymessage2 ( "min strength is %d at %s fields, max strength is %d at %s fields", dashboard.weaps[i].minstrength, tmp1, dashboard.weaps[i].maxstrength, tmp2 );

             while ( mouseinrect ( agmp->resolutionx - ( 640 - 502 ), 92 + i * 13, agmp->resolutionx - ( 640 - 572 ), 102 + i * 13 ) && (mouseparams.taste == 1));
          }
   }

   if ( (vehicle || vehicletype  ) && mouseinrect ( agmp->resolutionx - ( 640 - 461 ), 89, agmp->resolutionx - ( 640 - 577 ), 196 ) && (mouseparams.taste == 2)) 
      paintlargeweaponinfo();
    
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



void   tdashboard :: paintvehicleinfo( const pvehicle     vehicle,
                                       const pbuilding    building,
                                       const pobjectcontainer      object,
                                       const pvehicletype vt )
{ 


   byte         ms; 
 
   push(activefontsettings,sizeof(activefontsettings)); 
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
   pop(activefontsettings,sizeof(activefontsettings)); 

   dashboard.repainthard = 0;

   if ( actmap && actmap->ellipse )
      actmap->ellipse->paint();

}   /*  paintvehicleinfo  */ 



int          changeheight(pvehicle     vehicle,
                          char         ch,
                          int          mode ) // nur interessant fÅr Flugzeuge mit Steigung:
                                              //      mode = 1 : test auf Hîhenwechsel, fielder markieren
                                              //      mode = 100 : wirklich setzen
{ 
  int      v, w; 
  byte         b; 
  byte         md; 
  byte         steigung; 

   if (moveparams.movestatus != 0) 
      return unspecified_error;

   if ( !vehicle ) 
      return unspecified_error;
   pfield fld = getfield(vehicle->xpos,vehicle->ypos); 

   steigung = vehicle->typ->steigung; 


   b = (vehicle->color >> 3);
   if (b != actmap->actplayer) 
      return 101;

   if ( vehicle ) { 
      if (vehicle->typ->height & (chtieffliegend | chfliegend | chhochfliegend)) {
         if (steigung == 0) { 
            if (ch == 'h') 
               w = (vehicle->height << 1);
            else 
               w = (vehicle->height >> 1);
            if ( w & vehicle->typ->height ) {

               if ((ch == 't') && (w == chfahrend)) { 
                  if ( !terrainaccessible ( fld, vehicle, w ))
                     return 109;

                  int newmovement = vehicle->typ->movement[log2(w)] * (vehicle->movement - (helicopter_landing_move_cost + air_heightdecmovedecrease)) / vehicle->typ->movement[log2(vehicle->height)];
                  if ( newmovement < 0)                      
                     return 111;

                  int fuelcost = vehicle->typ->fuelconsumption * (helicopter_landing_move_cost + air_heightdecmovedecrease) / 8;
                  if ( fuelcost > vehicle->fuel )
                     return 115;


                  vehicle->movement = newmovement ; 
                  if ( !helicopter_attack_after_descent )
                     vehicle->attacked = 0; 
                  vehicle->fuel -= fuelcost;

               } else
               if (ch == 't') { 
                  int newmovement = vehicle->typ->movement[log2(w)] * vehicle->movement / vehicle->typ->movement[log2(vehicle->height)];
                  if ( newmovement < air_heightdecmovedecrease )
                     return 111;
                  int fuelcost = vehicle->typ->fuelconsumption * air_heightdecmovedecrease / 8;
                  if ( fuelcost > vehicle->fuel )
                     return 115;


                  vehicle->movement = newmovement - air_heightdecmovedecrease; 
                  vehicle->fuel -= fuelcost;
               } else
               if ((ch == 'h') && (w > chtieffliegend)) { 
                  int newmovement = vehicle->typ->movement[log2(w)] * vehicle->movement / vehicle->typ->movement[log2(vehicle->height)];
                  if (newmovement < air_heightincmovedecrease) 
                     return 110;
                  int fuelcost = vehicle->typ->fuelconsumption * air_heightincmovedecrease / 8;
                  if ( fuelcost > vehicle->fuel )
                     return 115;

                  vehicle->movement = newmovement - air_heightincmovedecrease ; 
                  vehicle->fuel -= fuelcost;

                  if ((w == chtieffliegend) && helicopter_attack_after_ascent ) 
                     vehicle->attacked = 1; 
               } else
                  if (ch == 'h'  &&  w == chtieffliegend ) {
                     int newmovement = vehicle->typ->movement[log2(w)] * vehicle->movement / vehicle->typ->movement[log2(vehicle->height)];
                     if ( newmovement < air_heightincmovedecrease )
                        return 110;

                     if ( helicopter_attack_after_ascent ) 
                        vehicle->attacked = 1; 

                     vehicle->movement = newmovement - air_heightincmovedecrease;
                  }
                  
               dashboard.x = 0xffff; 
               logtoreplayinfo ( rpl_changeheight, (int) vehicle->xpos, (int) vehicle->ypos, 
                                                   (int) vehicle->xpos, (int) vehicle->ypos, vehicle->networkid, (int) vehicle->height, (int) w );
               vehicle->height = w; 
            } 
         } 
         else {   /* steigung > 0 */ 
            if ( mode == 1 ) {
               if (ch == 'h') { 
                  w = (vehicle->height << 1);
                  if ( w != chtieffliegend) { 
                     if (vehicle->movement < steigung * maxmalq + air_heightincmovedecrease) 
                        return 110;
   
                  } 
                  else { 
                     if (vehicle->movement < steigung * maxmalq) 
                        return 110;
                  } 
   
               } 
               else { 
                  w = (vehicle->height >> 1);
                  if (vehicle->movement < steigung * maxmalq) 
                     return 111;
   
               } 
               if ((w & vehicle->typ->height) == 0)
                  return 112;

               v = vehicle->height; 
               if (ch == 't') 
                  vehicle->height = w;   // fÅr moveheight
               moveheight(vehicle); 
               displaymap(); 
               vehicle->height = (char) v; 
               moveparams.newheight = (char) w; 
               moveparams.oldheight = (char) v;
               moveparams.heightdir = ch; 
               moveparams.vehicletomove = vehicle; 
               moveparams.movesx = vehicle->xpos; 
               moveparams.movesy = vehicle->ypos; 
            } else {
               if ( ch == 'h' ) {
                  if ( vehicle->height == chfahrend ) {
                     vehicle->fuel -= vehicle->typ->fuelconsumption * vehicle->typ->steigung;
                     if ( vehicle->fuel < 0 )
                        vehicle->fuel = 0;
                     vehicle->movement = airplanemoveafterstart;
                     vehicle->attacked = 1;
                     vehicle->height = chtieffliegend;
                  } else {
                      w = vehicle->height << 1;
 
                     vehicle->movement -= air_heightincmovedecrease;
                     if ( vehicle->movement < 0 )
                        vehicle->movement = 0;

                     vehicle->fuel -= vehicle->typ->fuelconsumption * air_heightincmovedecrease / 8;
                     if ( vehicle->fuel < 0 )
                        vehicle->fuel = 0;

                     int newmovement = vehicle->typ->movement[log2(w)] * vehicle->movement / vehicle->typ->movement[log2(vehicle->height)];
                     vehicle->movement = newmovement;
                     if ( vehicle->movement < 0 )
                        vehicle->movement = 0;

                     vehicle->height = w;
                  }
               } else {
                  if ( vehicle->height == chtieffliegend ) {
                     // vehicle->fuel -= vehicle->typ->fuelconsumption * vehicle->typ->steigung;
                     // if ( vehicle->fuel < 0 )
                     //    vehicle->fuel = 0;
                     vehicle->movement = airplanemoveafterlanding;
                     vehicle->attacked = 1;
                     vehicle->height = chfahrend;
                  } else {
                     w = vehicle->height >> 1;

                     vehicle->movement -= air_heightdecmovedecrease;
                     if ( vehicle->movement < 0 )
                        vehicle->movement = 0;

                     vehicle->fuel -= vehicle->typ->fuelconsumption * air_heightdecmovedecrease / 8;
                     if ( vehicle->fuel < 0 )
                        vehicle->fuel = 0;

                     int newmovement = vehicle->typ->movement[log2(w)] * vehicle->movement / vehicle->typ->movement[log2(vehicle->height)];
                     vehicle->movement = newmovement;
                     if ( vehicle->movement < 0 )
                        vehicle->movement = 0;

                     vehicle->height = w;
                  }
               }
            }
         } 
      } 
      else {   /*  kein flugzeug  */ 

         if (ch == 'h') { 
            w = (vehicle->height << 1);
            md = sub_heightincmovedecrease; 
            if ( vehicle->movement < md )
               return 110;
         } 
         else { 
            w = (vehicle->height >> 1);
            md = sub_heightdecmovedecrease; 
            if ( vehicle->movement < md )
               return 111;
         } 
         if (( w & vehicle->typ->height ) && (vehicle->movement >= md)) {

            int newmovement = vehicle->typ->movement[log2(w)] * (vehicle->movement - md ) / vehicle->typ->movement[log2(vehicle->height)];

            npush ( vehicle->height );
            vehicle->height = w;
            int res = terrainaccessible2 ( fld, vehicle );
            if ( res <= 0 ) {
               npop ( vehicle->height );
               if ( w > vehicle->height ) 
                  return 108;
               else {
                  if ( res == -1 )
                     return 113;
                  else
                     if ( res == -2 )
                        return 114;
                     else
                        return 109;
               }
            }
            npop ( vehicle->height );
            int fuelcost = vehicle->typ->fuelconsumption * md / 8;
            if ( fuelcost > vehicle->fuel )
               return 115;

            vehicle->movement = newmovement;
            vehicle->fuel -= fuelcost; 
            dashboard.x = 0xffff; 

            logtoreplayinfo ( rpl_changeheight, (int) vehicle->xpos, (int) vehicle->ypos, 
                                                (int) vehicle->xpos, (int) vehicle->ypos, vehicle->networkid, (int) vehicle->height, (int) w );

            vehicle->height = w; 

         } 
      } 
   } 
   return 0;
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
                  // Message "sneak attack" an alle au·er dem Angreifer selbst.


            char txt[200];
            char* sp = getmessage( 10001 ); 
            sprintf ( txt, sp, actmap->player[act].name, actmap->player[i].name );
            sp = strdup ( txt );
            pmessage msg = new tmessage ( sp, to );
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
            pmessage msg = new tmessage ( sp, to );
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
            pmessage msg = new tmessage ( sp, to );
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



void returnresourcenuseforpowerplant (  const pbuilding bld, int prod, int* material, int* fuel )
{
    getpowerplantefficiency ( bld, material, fuel );
 
    *material = *material * prod / 256 * bld->typ->efficiencymaterial / ( 4 * 1024 );
    *fuel     = *fuel     * prod / 256 * bld->typ->efficiencyfuel     / ( 4 * 1024 );
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


int  tbuilding :: put_energy ( int      need,    int resourcetype, int queryonly  ) 
{
   /*
  if ( actmap->resourcemode == 1 ) {
     if ( mode & 64 ) 
        return 1000000000;
     else
     if ( mode & 32 ) {       // usage
        tresources res,usg;
        if (( mode & 3) == 1 ) {
           getresourceusage ( &res );
           return res.resource[1];
         } else {
           usg.energy = 0;
           usg.material = 0;
           usg.fuel = 0;
           for ( pbuilding bld = actmap->player[ color/8 ].firstbuilding; bld ; bld = bld->next ) {
              bld->getresourceusage ( &res );
              usg.energy   +=  res.energy;
              usg.material +=  res.material;
              usg.fuel     +=  res.fuel;
           }
           return usg.resource[mode & 3 ];
         }
     } else
     if ( mode & 16 ) {  // plus
        if ( (mode & 3 ) == 1 )
           return bi_resourceplus.resource[1];
        else {
           int p = 0;
           for ( pbuilding bld = actmap->player[ color/8 ].firstbuilding; bld ; bld = bld->next ) 
              p += bi_resourceplus.resource[mode & 3];
           
           return p;
        }
     } else
     if ( mode & 8 ) // tribute
        return 0;
     else {
        if (( mode & 3) == 1 ) {
           int mx = need;
           if ( mx > typ->gettank(mode & 3) - actstorage.resource[mode & 3] )
              mx = typ->gettank(mode & 3) - actstorage.resource[mode & 3];
           if ( mode & 4 )
              actstorage.resource[mode & 3] += mx;

           return mx;
        } else {
           if ( mode & 4 )
              actmap->bi_resource[ color / 8].resource[ mode & 3] += need;
           return need;
        }
     }
  } else
     return ::putenergy ( this, need, mode );

   */

   PutResource putresource;
   return putresource.getresource ( xpos, ypos, resourcetype, need, queryonly, color/8, 1 );


}


int  tbuilding :: get_energy ( int      need,    int resourcetype, int queryonly  ) 
{
   /*
  if ( actmap->resourcemode == 1 ) {
     if ( mode & 64 ) 
        return 1000000000;
     else
     if ( mode & 32 ) 
        return 0;
     else
     if ( mode & 16 ) {
        if ( (mode & 3 ) == 1 )
           return bi_resourceplus.resource[1];
        else {
           int p = 0;
           for ( pbuilding bld = actmap->player[ color/8 ].firstbuilding; bld ; bld = bld->next ) 
              p += bi_resourceplus.resource[mode & 3];
           
           return p;
        }
     } else
     if ( mode & 8 ) 
        return 0;
     else {
        if (( mode & 3) == 1 ) {
           int mx = need;
           if ( mx > actstorage.resource[mode & 3] )
              mx = actstorage.resource[mode & 3];
           if ( mode & 4 )
              actstorage.resource[mode & 3] -= mx;

           return mx;
        } else {
           int mx = need;
           if ( actmap->bi_resource[ color / 8].resource[ mode & 3] < mx )
              mx = actmap->bi_resource[ color / 8].resource[ mode & 3];
           if ( mode & 4 )
              actmap->bi_resource[ color / 8].resource[ mode & 3] -= mx;
           return mx;
        }
     }
  } else
     return ::getenergy ( this, need, mode );
   */

   GetResource gr;
   return gr.getresource ( xpos, ypos, resourcetype, need, queryonly, color/8, 1 );

}



int  tbuilding :: getenergyplus( int mode )
{
   if ( actmap->resourcemode != 1 ) {
      int plus = 0;
      if ( typ->special & cgenergyproductionb ) {
         if ( (typ->special & cgwindkraftwerkb) && ( mode & 2 ) ) {
            int eplus =  maxplus.a.energy * actmap->weather.wind[0].speed / 256;
            if ( ! (mode & 1 )) {
               int putable = put_energy ( eplus + plus, 0, 1 ) - plus;
               if ( putable < eplus )
                  eplus = putable; 
            }
            plus += eplus;
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
               
      
            int eplus = maxplus.a.energy * sum / ( num * 1024 );
            if ( ! (mode & 1 )) {
               int putable = put_energy ( eplus + plus, 0, 1 ) - plus;
               if ( putable < eplus )
                  eplus = putable; 
            }
            plus += eplus;
         } 
   
         if ( (typ->special & cgconventionelpowerplantb) && ( mode & 8 ) ) {
            int material;
            int fuel;
   
            int eplus = this->plus.a.energy;
   
            returnresourcenuseforpowerplant ( this, eplus, &material, &fuel );
            int maa = get_energy ( material, 1, 1 );
            int fua = get_energy ( fuel    , 2, 1 );
      
            if ( maa < material ||  fua < fuel   ) {
               int diff = eplus / 2;
               while ( maa < material ||  fua < fuel  || diff > 1) {
                  if ( maa < material ||  fua < fuel   ) 
                     eplus -= diff;
                  else
                     eplus += diff;
      
                  if ( diff > 1 )
                     diff /=2;
                  else
                     diff = 1;
      
                  returnresourcenuseforpowerplant ( this, eplus, &material, &fuel );
               }
      
               returnresourcenuseforpowerplant ( this, eplus+1, &material, &fuel );
               if ( maa >= material  &&  fua >= fuel )
                  eplus++;
               else
                  returnresourcenuseforpowerplant ( this, eplus, &material, &fuel );
               
            }
      
            if ( maa < material || fua < fuel )
               displaymessage( "controls : int tbuilding :: getenergyplus( void ) : inconsistency in getting material or fuel for energyproduction", 2 );
      
            if ( ! (mode & 1 )) {
               int putable = put_energy ( eplus + plus, 0, 1 ) - plus;
               if ( putable < eplus )
                  eplus = putable; 
            }
            plus += eplus;
   
         }
      }
      return plus;
   } else
      return bi_resourceplus.a.energy;
}


int  tbuilding :: getmaterialplus( int mode )
{
   if ( actmap->resourcemode != 1 ) {
      int pl = 0;
      if ( typ->special & cgmaterialproductionb ) {
         if ( typ->special & cgminingstationb ) {
            pl += processmining ( 1, 0 );
         } else
            pl += plus.a.material;
   
      }
      return pl;
   } else
      return bi_resourceplus.a.material;

}

int  tbuilding :: getfuelplus( int mode )
{
   if ( actmap->resourcemode != 1 ) {
      int pl = 0;
      if ( typ->special & cgfuelproductionb ) {
         if ( typ->special & cgminingstationb ) {
            pl += processmining ( 2, 0 );
         } else
            pl += plus.a.fuel;
      } 
      return pl;
   } else 
      return bi_resourceplus.a.fuel;

}



void tbuilding :: produceenergy( void )
{
   if ( turnstatus & cgenergyproductionb ) {
      if ( turnstatus & cgwindkraftwerkb ) {
         put_energy ( maxplus.a.energy * actmap->weather.wind[0].speed / 256, 0, 0 );
         turnstatus ^= cgwindkraftwerkb;
      }
   
      if ( turnstatus & cgsolarkraftwerkb ) {
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
            
   
         put_energy ( maxplus.a.energy * sum / ( num * 1024 ), 0, 0 );

         turnstatus ^= cgsolarkraftwerkb;
      }

      if ( turnstatus & cgconventionelpowerplantb ) {
         int material;
         int fuel;

         int eplus = this->plus.a.energy;
         int putable = put_energy ( eplus, 0, 1 );
         if ( putable < eplus )
            eplus = putable;

         returnresourcenuseforpowerplant ( this, eplus, &material, &fuel );
         int maa = get_energy ( material, 1, 1 );
         int fua = get_energy ( fuel    , 2, 1 );
   
         if ( maa < material ||  fua < fuel   ) {
            if ( maa > lastmaterialavail  ||  fua > lastfuelavail ) {
               lastmaterialavail = maa;
               lastfuelavail = fua;
               return;
            }
               

            int diff = eplus / 2;
            while ( maa < material ||  fua < fuel  || diff > 1) {
               if ( maa < material ||  fua < fuel   ) 
                  eplus -= diff;
               else
                  eplus += diff;
   
               if ( diff > 1 )
                  diff /=2;
               else
                  diff = 1;
   
               returnresourcenuseforpowerplant ( this, eplus, &material, &fuel );
            }
   
            returnresourcenuseforpowerplant ( this, eplus+1, &material, &fuel );
            if ( maa >= material  &&  fua >= fuel )
               eplus++;
            else
               returnresourcenuseforpowerplant ( this, eplus, &material, &fuel );
            
         }
   
         maa = get_energy ( material, 1, 0 );
         fua = get_energy ( fuel    , 2, 0 );
   
         if ( maa < material || fua < fuel )
            displaymessage( "controls : int tbuilding :: getenergyplus( void ) : inconsistency in getting material or fuel for energyproduction", 2 );
   
         put_energy ( eplus, 0, 0 );
         turnstatus ^= cgconventionelpowerplantb;
      }
   }
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
             int  setup ( pbuilding bld, int& mm, int cm, int& mf, int cf, int abbuch );  // mm: maxmaterial, cm: capacity material
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

        if ( !fld->resourceview )
           fld->resourceview = new tresourceview;
        fld->resourceview->visible |= 1 << color;
        fld->resourceview->fuelvisible[color] = fld->fuel;
        fld->resourceview->materialvisible[color] = fld->material;


     }
  }
}


int   tprocessminingfields :: setup ( pbuilding bld, int& mm, int cm, int& mf, int cf, int abbuch )  // mm: maxmaterial, cm: capacity material
{
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


   lastmineddist = pmf.setup ( this, maxmaterial, capmaterial, maxfuel, capfuel, abbuchen );
   pmf.done();                                                                                 

   if ( abbuchen ) {
      put_energy ( maxmaterial, 1 , 0 );
      put_energy ( maxfuel,     2 , 0 );
   }
   if ( res==1 )
      return maxmaterial;
   else
      return maxfuel;

}

void tbuilding :: producematerial( void )
{
   if ( turnstatus & cgmaterialproductionb ) {
      if ( turnminestatus & cgmaterialproductionb ) {
         processmining ( 1 );
         turnminestatus ^= cgmaterialproductionb;

         if ( !turnminestatus )
           turnstatus ^= cgminingstationb;
      } 
      /* else
         if ( plus.material )
             putenergy ( plus.material, 1 + 4 );
      */
      turnstatus ^= cgmaterialproductionb;
   }
}

void tbuilding :: producefuel( void )
{
   if ( turnstatus & cgfuelproductionb ) {
      if ( turnminestatus & cgfuelproductionb ) {
         processmining ( 2 );
         turnminestatus ^= cgfuelproductionb;

         if ( !turnminestatus )
           turnstatus ^= cgminingstationb;
      } 
      /* else
        if ( plus.fuel )
           putenergy ( plus.fuel, 2 + 4 );
      */
      turnstatus ^= cgfuelproductionb;
   }
}


void tbuilding :: getresourceusage ( tresources* usage )
{
   usage->a.energy = 0;
   usage->a.material =  0;
   usage->a.fuel = 0;   
   if ( typ->special & cgconventionelpowerplantb ) {
      int material;
      int fuel;
      returnresourcenuseforpowerplant ( this, plus.a.energy, &material, &fuel );
      usage->a.material += material;
      usage->a.fuel     += fuel;
   }
   if ( typ->special & cgresearchb ) {
      int material;
      int energy;
      returnresourcenuseforresearch ( this, researchpoints, &energy, &material );
      usage->a.material += material;
      usage->a.energy   += energy;
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

   turnminestatus = 0;
   if ( (completion == typ->construction_steps - 1) ) {
      turnstatus = 0;
      if ( actmap->resourcemode == 0 ) {
         if ( typ->special & cgenergyproductionb ) 
            turnstatus |= typ->special & ( cgwindkraftwerkb | cgsolarkraftwerkb | cgconventionelpowerplantb ); // cgresearchb 
         if ( typ->special & ( cgmaterialproductionb | cgfuelproductionb ))
            turnstatus |=  typ->special & cgminingstationb ; 
      
         turnstatus |= typ->special & ( cgrefineryb | cgenergyproductionb | cgmaterialproductionb | cgfuelproductionb ); // cgresearchb 
                   
         if ( (typ->special & cgmaterialproductionb) && (typ->special & cgminingstationb))
            turnminestatus |= cgmaterialproductionb;
         if ( (typ->special & cgfuelproductionb) && (typ->special & cgminingstationb))
            turnminestatus |= cgfuelproductionb;
      } else {
         if ( bi_resourceplus.a.energy || bi_resourceplus.a.material || bi_resourceplus.a.fuel )
            turnstatus |= cgenergyproductionb | cgmaterialproductionb | cgfuelproductionb;
      }
   } else
      turnstatus = 0;
}

int tbuilding :: worktodo ( void )
{
   return turnstatus;
}


void tbuilding :: processwork ( void )
{
   if ( (completion == typ->construction_steps - 1) ) {

      if ( actmap->resourcemode == 1 ) {
         if ( turnstatus & cgmaterialproductionb ) {
            actstorage.a.material += bi_resourceplus.a.material;
            turnstatus ^= cgmaterialproductionb;
         }
                          
         if ( turnstatus & cgenergyproductionb ) {
            actmap->bi_resource[ color / 8].a.energy += bi_resourceplus.a.energy;
            turnstatus ^= cgenergyproductionb;
         }
   
         if ( turnstatus & cgfuelproductionb ) {
            actmap->bi_resource[ color / 8].a.fuel += bi_resourceplus.a.fuel;
            turnstatus ^= cgfuelproductionb;
         }

         execnetcontrol ();
       /*
       #ifdef HEXAGON
         if (  actstorage.energy >  typ->bi_maxstorage.energy ) {
            putenergy ( typ->bi_maxstorage.energy -  actstorage.energy, 0 + 4 );
            actstorage.energy =  typ->bi_maxstorage.energy;
         }
         if (  actstorage.material >  typ->bi_maxstorage.material ) {
            putenergy ( typ->bi_maxstorage.material -  actstorage.material, 1 + 4 );
            actstorage.material =  typ->bi_maxstorage.material;
         }
         if (  actstorage.fuel >  typ->bi_maxstorage.fuel ) {
            putenergy ( typ->bi_maxstorage.fuel -  actstorage.fuel, 2 + 4 );
            actstorage.fuel =  typ->bi_maxstorage.fuel;
         }
       #endif
       */
      } else {

         if ( turnstatus & cgmaterialproductionb )
            if ( ! ( turnstatus & cgminingstationb))
               turnstatus ^= cgmaterialproductionb;
                          
         if ( turnstatus & cgmaterialproductionb )
            producematerial ();
   
   
         if ( turnstatus & cgfuelproductionb )
            if ( !( turnstatus & cgminingstationb))
               turnstatus ^= cgfuelproductionb;
   
         if ( turnstatus & cgfuelproductionb )
            producefuel ();
    
   
         if ( turnstatus & cgenergyproductionb )
            if ( !(turnstatus & (cgwindkraftwerkb | cgsolarkraftwerkb | cgconventionelpowerplantb)))
               turnstatus ^= cgenergyproductionb;
   
         if ( turnstatus & cgenergyproductionb )
            produceenergy ();

         execnetcontrol ();

         for ( int r = 0; r < 3; r++ )
            if (  actstorage.resource[r] >  typ->gettank(r) ) {
               put_energy ( typ->gettank(r) -  actstorage.resource[r], r , 0 );
               actstorage.resource[r] =  typ->gettank(r);
            }
      }

 
      if (  typ->special & cgrefineryb )  {
          actstorage.a.fuel += ( actstorage.a.material * refineryefficiency / 8);
          actstorage.a.material = 0; 
         turnstatus ^= cgrefineryb;
      } 
      
   }
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

          int buildingwaiting = 0;
          do {
             buildingwaiting = 0;

             pbuilding actbuilding = actmap->player[i].firstbuilding; 
             while ( actbuilding ) { 

                if ( actbuilding->worktodo() ) {
                   actbuilding->processwork();
                   if ( actbuilding->worktodo() ) 
                      buildingwaiting = 1;
                }

                actbuilding = actbuilding->next; 
             } /* end while */
   
          } while ( buildingwaiting ); /* enddo */

          doresearch( i );

       } 

}


void initchoosentechnology( void )
{
   actmap->player[actmap->actplayer].research.progress = 0;
 
   pdissectedunit du = actmap->player[ actmap->actplayer ].dissectedunit;
   pdissectedunit last = NULL;
   while ( du ) {
 
      if ( du->tech = actmap->player[actmap->actplayer].research.activetechnology ) {
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

void newturnforplayer ( int forcepasswordchecking )
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
         
   
      if ( humannum > 1  ||  forcepasswordchecking ) {
         tlockdispspfld ldsf;
         backgroundpict.paint();

         /*
         {
            tnfilestream stream ( "bkgr42.pcx", 1 );
            loadpcxxy( &stream, 0, 0 ); 
         }
         */

         if ( (actmap->player[actmap->actplayer].passwordcrc && actmap->player[actmap->actplayer].passwordcrc != gameoptions.defaultpassword ) || actmap->time.a.turn == 1 || (actmap->network && actmap->network->globalparams.reaskpasswords) ) {
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

      displaymessage ( " starting network transfer ",0);

      try {
         compi->send.transfermethod->initconnection ( send );
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
      displaymessage( " network transfer finished",1);

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
            if ( !network.computer[0].receive.transfermethod->validateparams( &network.computer[0].receive.data, receive ))
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
           network.computer[0].receive.transfermethod->validateparams( &network.computer[0].receive.data, receive ))
           go = 1;
   } while ( !go ); 
   

   try {
       displaymessage ( " starting network transfer ",0);
    
       network.computer[0].receive.transfermethod->initconnection ( receive );
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
   double esteigung = 55;
   double msteigung = 40;

   double res = research;
   double deg = res / bld->typ->maxresearchpoints;

   double m = 1 / log ( minresearchcost + maxresearchcost );

   *energy   = (int)researchenergycost   * research * ( exp ( deg / m ) - ( 1 - minresearchcost ) ) / 1000 * (researchcostdouble+res)/researchcostdouble;
   *material = (int)researchmaterialcost * research * ( exp ( deg / m ) - ( 1 - minresearchcost ) ) / 1000 * (researchcostdouble+res)/researchcostdouble;
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
               if ( du->tech = techs[i] )
                  found |= 4;
               else
                  found |= 2;
            
            if ( du->tech = techs[i] )
               found |= 1;
      
            du = du->next;
         }
   
         if ( found & 4 ) {
            du = actmap->player[ actmap->actplayer ].dissectedunit;
            while ( du ) {
               if ( du->fzt == eht->typ ) 
                  if ( du->tech = techs[i] ) {
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

      if ( actmap->gameparameter[cgp_bi3_training] >= 1 ) {
         int cnt = 0;
         pbuilding bld = actmap->player[ actmap->actplayer ].firstbuilding;
         while ( bld ) {
            if ( bld->typ->special & cgtrainingb )
               cnt++;
            bld = bld->next;
         }
         vehicle->experience += cnt * actmap->gameparameter[cgp_bi3_training];
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
      if ( d > 0 ) {
         if ( fld->vehicle )
            if ( pass == 2 )
               checkvehicle ( fld->vehicle );
      } else
         return;

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

void MapNetwork :: searchbuilding ( int x, int y )
{
   pbuilding bld = getfield( x, y )->building;
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
            if ( fld2->building == bld ) 
               for ( int d = 0; d < sidenum; d++ ) {
                  int xp2 = xp;
                  int yp2 = yp;
                  getnextfield ( xp2, yp2, d );
                  pfield newfield = getfield ( xp2, yp2 );
                  if ( newfield->building != bld  && !newfield->a.temp )
                     searchfield ( xp2, yp2, d );
               } /* endfor */
         }
}


int MapNetwork :: instancesrunning = 0;

MapNetwork :: MapNetwork ( void )
{
   if ( instancesrunning )
      displaymessage(" fatal error at MapNetwork; there are other running instances ", 2 );

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
         }
   } else 
      if ( globalsearch() == 1 ) {
         cleartemps(7);
         startposition.x = x;
         startposition.y = y;
         searchfield ( x, y, -1 );
         cleartemps(7);
         if ( searchfinished() ) {
            pass++;
            startposition.x = x;
            startposition.y = y;
            searchfield ( x, y, -1 );
            cleartemps(7);
         }
      } else  
         if ( globalsearch() == 0 ) {
            pfield fld = getfield ( x, y );
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
    else */ {
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
             if ( (fld2->bdt & tb) ||  fld2->building )
                d |= ( 1 << i );
          }
          return d;

       } else
          return 0;
    }
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





GetResource :: GetResource ( void )
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
                     int avail = startbuilding->get_energy ( need, resourcetype, 1 );
                     if ( need > avail )
                        need = avail;
                     MapNetwork :: start ( x, y );
                     if ( !queryonly ) 
                        startbuilding->get_energy ( got, resourcetype, queryonly );
                     
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
                              sprintf( txt3, "%d %s", topay[r], cdnames[r] );
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
                              sprintf( txt3, "%d %s", got[r], cdnames[r] );
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
               pmessage msg = new tmessage ( sp, 1 << targplayer );
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
   got += v->typ->energy;
}


void GetResourcePlus :: checkbuilding ( pbuilding bld )
{
   if ( bld->color/8 == player ) {
      if ( resourcetype == 0) 
         got += bld->getenergyplus ( -1 ); 
       else 
          if ( resourcetype == 1) 
             got += bld->getmaterialplus ( -1 ); 
          else 
             got += bld->getfuelplus ( -1 ); 
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
  displaymessage ( "%d %s available", 1, net1.getresource ( getxpos(), getypos(), resource, maxint, 0, actmap->actplayer, 1 ), cdnames[resource]);

  PutResource net2;
  displaymessage ( "%d %s space available", 1, net2.getresource ( getxpos(), getypos(), resource, maxint, 0, actmap->actplayer, 1 ), cdnames[resource]);

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
               cursor.hide();
               actmap->xpos = newx;
               actmap->ypos = newy;
               displaymap();
               cursor.posx = x - actmap->xpos;
               cursor.posy = y - actmap->ypos;

               // cursor.gotoxy ( x, y );
               cursor.show();
            }
            while ( mouseparams.taste == gameoptions.mouse.centerbutton );
         }
      }

   if ( gameoptions.mouse.smallguibutton )
      if ( mouseparams.taste == gameoptions.mouse.smallguibutton ) {
         int x; 
         int y;
         int r = getfieldundermouse ( &x, &y );
   
         if ( r ) 
            if ( (cursor.posx != x || cursor.posy != y) && ( moveparams.movestatus == 0   ||  getfield(actmap->xpos + x , actmap->ypos + y)->a.temp == 0) ) {
               mousestat = 1;
               mousevisible(false);
               cursor.hide();
               cursor.posx = x;
               cursor.posy = y;
               cursor.show();
               mousevisible(true);
            } else 
              if ( mousestat == 2  ||  mousestat == 0 ||  (moveparams.movestatus && getfield( actmap->xpos + x, actmap->ypos + y)->a.temp )  ) {
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


void logtoreplayinfo ( trpl_actions action, ... )
{
   if ( actmap->replayinfo && actmap->replayinfo->actmemstream ) {
      pnstream stream = actmap->replayinfo->actmemstream;

      va_list paramlist;
      va_start ( paramlist, action );

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
    }


}

void trunreplay :: setcursorpos ( int x, int y )
{
   int i = fieldvisiblenow ( getfield ( x, y ), actmap->playerview );
   cursor.gotoxy ( x, y, i );
   if( i ) {
      lastvisiblecursorpos.x = x;
      lastvisiblecursorpos.y = y;
   }

}


void trunreplay :: execnextreplaymove ( void )
{
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
                           moveparams.uheight = -1;
                           moveparams.movesx = x1;
                           moveparams.movesy = y1;
                           moveparams.vehicletomove = eht;
   
                           setcursorpos ( x1, y1 );
                           int t = ticker;
                           fieldreachablerek(x2, y2, eht, 1);
                           wait( t );
   
                           setcursorpos ( x2, y2 );
                           // wait();
   
                           moveunitxy ( eht, x2, y2 );
                           // status = 1;
                        } else 
                           displaymessage("severe replay inconsistency:\nno vehicle for move command !", 1);
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
                           moveparams.uheight = -1;
                           moveparams.movesx = x1;
                           moveparams.movesy = y1;
                           moveparams.vehicletomove = eht;
                           eht->height = height;
   
                           setcursorpos ( x1, y1 );
// !!! debug
/*                           int fieldnum = sucheerreichbarefielder(eht->xpos,eht->ypos,eht, height ); 
                           displaymap();
                           getch();
  */
                           int t = ticker;
                           fieldreachablerek(x2, y2, eht, 1);
                           wait( t );
   
                           setcursorpos ( x2, y2 );
                           // wait();
   
                           moveunitxy ( eht, x2, y2 );
                           // status = 1;
                        } else 
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

                          setcursorpos ( x1, y1 );
                          wait ( );
                          setcursorpos ( x2, y2 );

                          pfield fld = getfield ( x1, y1 );
                          pfield targ = getfield ( x2, y2 );
                          int attackvisible = fieldvisiblenow ( fld, actmap->playerview ) || fieldvisiblenow ( targ, actmap->playerview );
                          if ( fld->vehicle ) {
   
                             if ( targ->vehicle ) {
                                tunitattacksunit battle;
                                battle.setup ( fld->vehicle, targ->vehicle, 1, wpnum );
                                battle.av.damage = ad1;
                                battle.dv.damage = dd1;
                                if ( attackvisible ) 
                                   battle.calcdisplay ( ad2, dd2 );
                                else {
                                   battle.calc ();
                                   battle.av.damage = ad2;
                                   battle.dv.damage = dd2;
                                }
                                battle.setresult ();
                                dashboard.x = 0xffff;
                             } else 
                             if ( targ->building ) {
                                tunitattacksbuilding battle;
                                battle.setup ( fld->vehicle, x2, y2 , wpnum );
                                battle.av.damage = ad1;
                                battle.dv.damage = dd1;
                                if ( attackvisible ) 
                                   battle.calcdisplay ( ad2, dd2 );
                                else {
                                   battle.calc ();
                                   battle.av.damage = ad2;
                                   battle.dv.damage = dd2;
                                }
                                battle.setresult ();
                                dashboard.x = 0xffff;
                             } else
                             if ( targ->object ) {
                                tunitattacksobject battle;
                                battle.setup ( fld->vehicle, x2, y2, wpnum );
                                if ( attackvisible ) 
                                   battle.calcdisplay ( ad2, dd2 );
                                else {
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
                           moveparams.oldheight = oldheight;
                           moveparams.newheight = newheight;
                           moveparams.uheight = -1;
                           moveparams.movesx = x1;
                           moveparams.movesy = y1;
                           moveparams.vehicletomove = eht;
                           moveparams.movestatus = 11;
                           if ( oldheight < newheight )
                              moveparams.heightdir = 'h';
                           else
                              moveparams.heightdir = 't';
   
                           if ( x1 != x2 || y1 != y2 ) {
                              setcursorpos ( x1, y1 );
                              int t = ticker;
                              fieldreachablerek(x2, y2, eht, 1);
                              wait( t );
      
                              setcursorpos ( x2, y2 );
                              // wait();
      
                              moveparams.movestatus = 12;
                              moveunitxy ( eht, x2, y2 );
                           } else {
                              setcursorpos ( x1, y1 );
                              eht->height = moveparams.newheight;
                              computeview();
                              displaymap();
                              wait();
                           }
                        } else 
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

                           setcursorpos ( x, y );
                           wait();
                           pfield fld = getfield ( x, y );
                           if ( fld->vehicle )
                              fld->vehicle->convert ( col );
                           else
                              if ( fld->building )
                                 fld->building->convert ( col );

                           computeview();
                           displaymap();
                           wait();

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

                           setcursorpos ( x, y );
                           wait();
                           pfield fld = getfield ( x, y );

                           pobjecttype obj = getobjecttype_forid ( id );

                           if ( obj ) {
                              if ( actaction == rpl_remobj )
                                 getfield ( x, y ) -> removeobject ( obj );      
                              else
                                 getfield ( x, y ) -> addobject ( obj );

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

                           setcursorpos ( x, y );
                           wait();
                           pfield fld = getfield ( x, y );

                           pvehicletype tnk = getvehicletype_forid ( id );

                           if ( tnk && !fld->vehicle ) {
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


                               setcursorpos ( x, y );
                               wait();
                               pfield fld = getfield ( x, y );
    
                               pbuildingtype bld = getbuildingtype_forid ( id );
    
                               if ( bld ) {
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

                           setcursorpos ( x, y );
                           wait();
                           pfield fld = getfield ( x, y );
                           getfield ( x, y ) -> putmine ( col, typ, strength );
                           computeview();
                           displaymap();
                           wait();

                       }
         break;
      case rpl_removemine: {
                           int x, y, size;
                           stream->readdata2 ( size );
                           stream->readdata2 ( x );
                           stream->readdata2 ( y );
                           readnextaction();

                           setcursorpos ( x, y );
                           wait();
                           pfield fld = getfield ( x, y );
                           getfield ( x, y ) -> removemine ( );
                           computeview();
                           displaymap();
                           wait();

                       }
         break;
      case rpl_removebuilding: {
                           int x, y, size;
                           stream->readdata2 ( size );
                           stream->readdata2 ( x );
                           stream->readdata2 ( y );
                           readnextaction();

                           setcursorpos ( x, y );
                           wait();
                           pfield fld = getfield ( x, y );
                           pbuilding bb = fld->building;
                           if ( bb ) {
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
                                 if ( tnk ) {
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
                                       setcursorpos ( x, y );
                                       wait( t );
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


                                 pfield fld = getfield ( x, y );


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
                                    readnextaction();
                                    if ( actmap->shareview ) 
                                       delete actmap->shareview;
                                    actmap->shareview = sv;
                                    actmap->shareview->recalculateview = 0;
                                 } else
                                    if ( actmap->shareview ) {
                                       delete actmap->shareview;
                                       actmap->shareview = NULL;
                                    }

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


   int cx = getxpos();
   int cy = getypos();

   cursor.hide();


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
   lastvisiblecursorpos.x = orgmap.cursorpos.position[ actplayer ].cx;
   lastvisiblecursorpos.y = orgmap.cursorpos.position[ actplayer ].cy;


   if ( stream->dataavail () )
      status = 1;
   else 
      status = 11;

   computeview();
   displaymap ();

   dashboard.x = 0xffff;
   mousevisible( true );
   cursor.show();


   do {
       if ( status == 2 ) {
          execnextreplaymove ( );
          if ( getxpos () != lastvisiblecursorpos.x || getypos () != lastvisiblecursorpos.y )
             setcursorpos ( lastvisiblecursorpos.x, lastvisiblecursorpos.y );
       }

       if ( nextaction == rpl_finished   &&  !cursor.an )
          cursor.show();


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


