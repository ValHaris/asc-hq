//     $Id: loaders.cpp,v 1.21 2000-08-07 16:29:21 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.20  2000/08/05 13:38:26  mbickel
//      Rewrote height checking for moving units in and out of
//        transports / building
//
//     Revision 1.19  2000/08/04 15:11:12  mbickel
//      Moving transports costs movement for units inside
//      refuelled vehicles now have full movement in the same turn
//      terrain: negative attack / defensebonus allowed
//      new mapparameters that affect damaging and repairing of building
//
//     Revision 1.18  2000/08/03 19:45:15  mbickel
//      Fixed some bugs in DOS code
//      Removed submarine.ascent != 0 hack
//
//     Revision 1.17  2000/08/03 13:12:15  mbickel
//      Fixed: on/off switching of generator vehicle produced endless amounts of energy
//      Repairing units now reduces their experience
//      negative attack- and defenseboni possible
//      changed attackformula
//
//     Revision 1.16  2000/07/29 14:54:37  mbickel
//      plain text configuration file implemented
//
//     Revision 1.15  2000/07/26 15:58:10  mbickel
//      Fixed: infinite loop when landing with an aircraft which is low on fuel
//      Fixed a bug in loadgame
//
//     Revision 1.14  2000/07/16 14:20:03  mbickel
//      AI has now some primitive tactics implemented
//      Some clean up
//        moved weapon functions to attack.cpp
//      Mount doesn't modify PCX files any more.
//
//     Revision 1.13  2000/06/28 19:26:16  mbickel
//      fixed bug in object generation by building removal
//      Added artint.cpp to makefiles
//      Some cleanup
//
//     Revision 1.12  2000/06/28 18:31:00  mbickel
//      Started working on AI
//      Started making loaders independent of memory layout
//      Destroyed buildings can now leave objects behind.
//
//     Revision 1.11  2000/05/25 11:07:44  mbickel
//      Added functions to check files for valid mail / savegame files.
//
//     Revision 1.10  2000/05/06 19:57:09  mbickel
//      Mapeditor/linux is now running
//
//     Revision 1.9  2000/04/27 16:25:24  mbickel
//      Attack functions cleanup
//      New vehicle categories
//      Rewrote resource production in ASC resource mode
//      Improved mine system: several mines on a single field allowed
//      Added unitctrl.* : Interface for vehicle functions
//        currently movement and height change included
//      Changed timer to SDL_GetTicks
//
//     Revision 1.8  2000/01/25 19:28:14  mbickel
//      Fixed bugs:
//        invalid mouse buttons reported when moving the mouse
//        missing service weapon in weapon information
//        invalid text displayed in ammo production
//        undamaged units selected in repair vehicle function
//
//      speed up when playing weapon sound
//
//     Revision 1.7  2000/01/24 17:35:45  mbickel
//      Added dummy routines for sound under DOS
//      Cleaned up weapon specification
//
//     Revision 1.6  1999/12/28 21:03:03  mbickel
//      Continued Linux port
//      Added KDevelop project files
//
//     Revision 1.5  1999/12/14 20:23:56  mbickel
//      getfiletime now works on containerfiles too
//      improved BI3 map import tables
//      various bugfixes
//
//     Revision 1.4  1999/11/23 21:07:31  mbickel
//      Many small bugfixes
//
//     Revision 1.3  1999/11/22 18:27:34  mbickel
//      Restructured graphics engine:
//        VESA now only for DOS
//        BASEGFX should be platform independant
//        new interface for initialization
//      Rewrote all ASM code in C++, but it is still available for the Watcom
//        versions
//      Fixed bugs in RLE decompression, BI map importer and the view calculation
//
//     Revision 1.2  1999/11/16 03:41:57  tmwilson
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

#ifdef _DOS_               
#include <dos.h> 
#endif
#include "config.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>

#include "tpascal.inc"
#include "misc.h"
#include "keybp.h"
#include "basegfx.h"
#include "newfont.h"
#include "typen.h"
#include "spfst.h"
#include "loaders.h"
#include "dlg_box.h"
#include "dlg_box.h"
#include "dialog.h"
#include "sgstream.h"
#include "sg.h"
#include "attack.h"

#ifdef sgmain
#include "missions.h"
#endif

/* 
  bei der n„chsten revision des map-formates zu „ndern:
     cawar ... Konstanten  >> auch loadicons „ndern !

  bei der n„chsten revision des sav-formates zu „ndern:
     history

 bei der n„chsten „nderung des Tank-Formates zu „ndern:

 bei der n„chsten „nderung des Building-Formates zu „ndern:

 bei der n„chsten „nderung des Building-typ-Formates zu „ndern:

*/ 

word fileterminator = 0xa01a;


char*         cloaderrormsg[6]  = {"no error occured",
                                   "could not open file",
                                               "file not found", 
                                               "invalid file version", 
                                               "not enough memory",
                                               "stream error" };

ticons icons;

int guiiconnum;



 const char* savegameextension = "*.sav";
 const char* mapextension = "*.map";
 const char* tournamentextension = "*.eml";


tinvalidid :: tinvalidid ( char* s, int iid ) 
{
   strcpy ( msg, s );
   strcpy ( orgmsg, s );
   strcat ( msg, strrr ( iid ));
   id = iid;
}


char *       loaderrormsg(byte         b)
{ 
/*   if (b < high(cloaderrormsg))  */
      return cloaderrormsg[b-1]; 
/*   else
      return scat("io error %s",strr(b));  */
} 




void         setbuildingsonmap(void)         /*   prir test ok */
{ 
   byte         i;
   integer      dx, a, b;
   pbuilding    aktbuilding[9];   /*  leseposition der linearen liste der buildingen  */ 
   integer      orgx, orgy; 
   pfield        field; 

   for (i = 0; i <= 8; i++) { 
      aktbuilding[i] = actmap->player[i].firstbuilding; 
      while (aktbuilding[i] != NULL) { 
         orgx = aktbuilding[i]->xpos - aktbuilding[i]->typ->entry.x; 
         orgy = aktbuilding[i]->ypos - aktbuilding[i]->typ->entry.y; 

         dx = orgy & 1; 

         orgx += dx & (~aktbuilding[i]->typ->entry.y);


         if (aktbuilding[i]->completion >= aktbuilding[i]->typ->construction_steps)
             aktbuilding[i]->completion = aktbuilding[i]->typ->construction_steps - 1;


         for (a = orgx; a <= orgx + 3; a++) 
            for (b = orgy; b <= orgy + 5; b++) {
               void* pnt = aktbuilding[i]->getpicture ( a - orgx , b - orgy );
               if ( pnt ) {
                    field = getfield(a - (dx & b), b);
                    field->building = aktbuilding[i];

                    field->picture = pnt; 
                    field->setparams();
                } 
            }      

        aktbuilding[i] = aktbuilding[i]->next; 
      } 
   } 
} 




void         seteventtriggers(void)
{ 
  pevent       event;

  for ( int l = 0; l < 2; l++ ) {
      if ( l == 0 )
         event = actmap->firsteventtocome; 
      else
         event = actmap->firsteventpassed; 

      while ( event ) { 
         for ( int j = 0; j <= 3; j++) { 
            if ((event->trigger[j] == ceventt_buildingconquered) || 
                (event->trigger[j] == ceventt_buildinglost) || 
                (event->trigger[j] == ceventt_buildingdestroyed) ||
                (event->trigger[j] == ceventt_building_seen )) {
                 
               int xpos = event->trigger_data[j]->xpos; 
               int ypos = event->trigger_data[j]->ypos; 
               if ( xpos != -1  &&  ypos != -1  &&  event->triggerstatus[j] != 2 ) {
                  pbuilding building = getfield(xpos,ypos)->building; 
                  event->trigger_data[j]->building = building; 
                  if ((event->trigger[j] == ceventt_buildingconquered)) 
                     building->connection |= cconnection_conquer;
                  if ((event->trigger[j] == ceventt_buildinglost)) 
                     building->connection |= cconnection_lose;
                  if ((event->trigger[j] == ceventt_buildingdestroyed)) 
                     building->connection |= cconnection_destroy;
                  if ((event->trigger[j] == ceventt_building_seen )) 
                     building->connection |= cconnection_seen;
                } else
                  event->trigger_data[j]->building = NULL;

               event->trigger_data[j]->xpos = -1;
               event->trigger_data[j]->ypos = -1;
            } 
            if ((event->trigger[j] == ceventt_unitconquered) || 
                (event->trigger[j] == ceventt_unitlost) || 
                (event->trigger[j] == ceventt_unitdestroyed)) { 
               int xpos = event->trigger_data[j]->xpos;
               int ypos = event->trigger_data[j]->ypos;
               if ( xpos != -1 && ypos != -1  && event->triggerstatus[j] != 2 ) {
                  pvehicle vehicle;
                  if ( event->trigger_data[j]->networkid != -1 ) 
                     vehicle = actmap->getunit ( xpos, ypos, event->trigger_data[j]->networkid );
                  else
                     vehicle = getfield(xpos,ypos)->vehicle;

                  event->trigger_data[j]->vehicle = vehicle; 
                  if ((event->trigger[j] == ceventt_unitconquered)) 
                     vehicle->connection |= cconnection_conquer;
                  if ((event->trigger[j] == ceventt_unitlost)) 
                     vehicle->connection |= cconnection_lose;
                  if ((event->trigger[j] == ceventt_unitdestroyed)) 
                     vehicle->connection |= cconnection_destroy;
               } else
                  event->trigger_data[j]->vehicle = NULL;

               event->trigger_data[j]->xpos = -1;
               event->trigger_data[j]->ypos = -1;
               event->trigger_data[j]->networkid = -1;
            } 
   
            if ((event->trigger[j] == ceventt_event)) {     
              // int id = event->trigger_data[j]->id;
               pevent event1 = actmap->firsteventtocome; 
               if ( !event->trigger_data[j]->mapid )
                  while (event1 != NULL) { 
                     if (event1->id == event->id) 
                        event1->conn = 1; 
                     event1 = event1->next; 
                  } 
              /* event1 = actmap->firsteventpassed; 
               while (event1 != NULL) { 
                  if (event1->id == id) 
                     event->triggerdata[j].event = event1; 
                  event1 = event1->next; 
               }*/ 
            }  
           if (event->trigger[j] == ceventt_any_unit_enters_polygon || 
               event->trigger[j] == ceventt_specific_unit_enters_polygon) {

               if ( event->trigger_data[j]->unitpolygon->vehicle ) {
                  event->trigger_data[j]->unitpolygon->vehicle = actmap->getunit ( event->trigger_data[j]->unitpolygon->tempxpos, event->trigger_data[j]->unitpolygon->tempypos, event->trigger_data[j]->unitpolygon->tempnwid );
                  event->trigger_data[j]->unitpolygon->vehicle->connection |= cconnection_areaentered_specificunit;
               }
              #ifndef karteneditor
               if ( event->trigger[j] == ceventt_any_unit_enters_polygon )
                  mark_polygon_fields_with_connection ( event->trigger_data[j]->unitpolygon->data, cconnection_areaentered_anyunit );
               else 
                  mark_polygon_fields_with_connection ( event->trigger_data[j]->unitpolygon->data, cconnection_areaentered_specificunit );
              #endif 
           }
         } 
         event = event->next; 
      } 
   }
  #ifndef karteneditor
   for ( int i = 0; i < 8; i++ )
      quedevents[ i ] = 1;
  #endif

} 


void         renumevents(void)
{ 
/* 
  word         j, i;
  pevent       event; 
      j:=0;
   event:=actmap->firsteventtocome;
   while event <> NULL do begin
      inc(j);
      event^.diskid:=j;
      event:=event^.next;
   end;

   j:=10000;
   event:=actmap->firsteventpassed;
   while event <> NULL do begin
      inc(j);
      event^.diskid:=j;
      event:=event^.next;
   end;  */ 
} 


  #define csm_typid32 1      /*  b1  */
  #define csm_direction 2    /*  b1  */ 
  #define csm_vehicle 4      /*  b1  */
  #define csm_building 8     /*  b1  */
  #define csm_height 32      /*  b1  */
  #define csm_cnt2 64        /*  b1  */
  #define csm_b3 128         /*  b1  */

  #define csm_material 1     /*  b3  */
  #define csm_fuel 2         /*  b3  */
  #define csm_visible 4      /*  b3  */
//  #define csm_mine 8         /*  b3  */
  #define csm_weather 16     /*  b3  */
//  #define csm_fahrspur 32    /*  b3  */
  #define csm_object 64      /*  b3  */
  #define csm_b4 128         /* b3 */ 

  #define csm_resources  1     /* b4 */
  #define csm_connection 2     // b4
  #define csm_newobject  4     // b4



#define cem_experience    0x1
#define cem_damage        0x2
#define cem_fuel          0x4
#define cem_ammunition    0x8
#define cem_weapstrength  0x10
#define cem_loading       0x20
#define cem_attacked      0x40
#define cem_height        0x80
#define cem_movement      0x100
#define cem_direction     0x200
#define cem_material      0x400
#define cem_energy        0x800
#define cem_class         0x1000
#define cem_networkid     0x2000
#define cem_name          0x4000
#define cem_armor         0x8000
#define cem_reactionfire  0x10000
#define cem_reactionfire2 0x20000
#define cem_poweron       0x40000
#define cem_weapstrength2 0x80000
#define cem_ammunition2   0x100000
#define cem_energyUsed    0x200000






/**************************************************************/
/*     einzelne vehicle schreiben / lesen                   ÿ */
/**************************************************************/


void         tspfldloaders::writeunit ( pvehicle eht )
{

    stream->writedata2 ( eht->typ->id );
    stream->writedata2 ( eht->color );

    int bm = 0;

    if ( eht->experience )
       bm |= cem_experience;
    if ( eht->damage    )
       bm |= cem_damage;

    if ( eht->fuel < eht->typ->tank )
       bm |= cem_fuel;

    if ( eht->typ->weapons->count ) 
       for (char m = 0; m < eht->typ->weapons->count ; m++) {
          if ( eht->ammo[m] < eht->typ->weapons->weapon[m].count )
             bm |= cem_ammunition2;
          if ( eht->weapstrength[m] != eht->typ->weapons->weapon[m].maxstrength )
             bm |= cem_weapstrength2;

       } 
    for ( int i = 0; i < 32; i++ )
       if ( eht->loading[i] )
           bm |= cem_loading;

    if ( eht->attacked  )
       bm |= cem_attacked;
    if ( eht->height != chfahrend )
       bm |= cem_height;
    if ( eht->getMovement() < eht->typ->movement[log2(eht->height)] )
       bm |= cem_movement;

    if ( eht->direction )
       bm |= cem_direction;

    if ( eht->material < eht->typ->material )
       bm |= cem_material  ;

    if ( eht->energy   < eht->typ->energy   )
       bm |= cem_energy;

    if ( eht->energyUsed )
       bm |= cem_energyUsed;

    if ( eht->klasse    )
       bm |= cem_class;
    
    if ( eht->armor != eht->typ->armor )
       bm |= cem_armor;

    if ( eht->networkid )
       bm |= cem_networkid;

    if ( eht->name      )
       bm |= cem_name;
 
    if ( eht->reactionfire.status )
       bm |= cem_reactionfire;

    if ( eht->reactionfire.enemiesAttackable )
       bm |= cem_reactionfire2;

    if ( eht->generatoractive )
       bm |= cem_poweron;


    stream->writedata2( bm );

    if ( bm & cem_experience )              
         stream->writedata2 ( eht->experience );

    if ( bm & cem_damage )
         stream->writedata2 ( eht->damage );

    if ( bm & cem_fuel )
         stream->writedata2 ( eht->fuel );

    if ( bm & cem_ammunition2 )              
       for ( int j= 0; j < 16; j++ )
         stream->writedata2 ( eht->ammo[j] );

    if ( bm & cem_weapstrength2 )
       for ( int j = 0; j < 16; j++ )
         stream->writedata2 ( eht->weapstrength[j] );

    if ( bm & cem_loading ) {
       char k;
       char c=0;
       for (k = 0; k <= 31; k++)
          if ( eht->loading[k] )
             c++;
   
       stream->writedata2 ( c );
   
       if (c)
          for (k = 0; k <= 31; k++) 
             if ( eht->loading[k] )
                writeunit ( eht->loading[k] );
    }

    if ( bm & cem_height )
         stream->writedata2 ( eht->height );

    if ( bm & cem_movement )
         stream->writeChar ( eht->getMovement() );

    if ( bm & cem_direction )
         stream->writedata2 ( eht->direction );

    if ( bm & cem_material )
         stream->writedata2 ( eht->material );

    if ( bm & cem_energy )
         stream->writedata2 ( eht->energy );

    if ( bm & cem_class )
         stream->writedata2 ( eht->klasse );

    if ( bm & cem_armor )
         stream->writedata2 ( eht->armor );
    
    if ( bm & cem_networkid )
         stream->writedata2 ( eht->networkid );

    if ( bm & cem_attacked )
         stream->writedata2 ( eht->attacked );

    if ( bm & cem_name     )
         stream->writepchar ( eht->name );

    if ( bm & cem_reactionfire )
       stream->writeChar ( eht->reactionfire.status );

    if ( bm & cem_reactionfire2 )
       stream->writeChar ( eht->reactionfire.enemiesAttackable );

    if ( bm & cem_poweron )
       stream->writedata2 ( eht->generatoractive );

    if ( bm & cem_energyUsed )
       stream->writeInt ( eht->energyUsed );
}


void         tspfldloaders::readunit ( pvehicle &eht )
{
    word id;
    stream->readdata2 ( id );

    pvehicletype fzt = getvehicletype_forid ( id, 0 );

    if ( !fzt )
       throw tinvalidid ( "no unit with matching ID found; ID = ", id );

    eht = new tvehicle ;

    eht->typ = fzt;

    if ( spfld->objectcrc ) 
       spfld->objectcrc->speedcrccheck->checkunit2 ( eht->typ );


    stream->readdata2 ( eht->color );
    
    int bm;

    stream->readdata2( bm );

    if ( bm & cem_experience )
         stream->readdata2 ( eht->experience );
    else
       eht->experience = 0;

    if ( bm & cem_damage )
         stream->readdata2 ( eht->damage );
    else
       eht->damage = 0;

    if ( bm & cem_fuel )
         stream->readdata2 ( eht->fuel );
    else
       eht->fuel = eht->typ->tank;

    if ( bm & cem_ammunition ) {
       word old;
       for ( int i = 0; i < 8; i++ ) {
         stream->readdata2 ( old );
         eht->ammo[i] = old;
       }
    } else
     if ( bm & cem_ammunition2 ) {
        for ( int i = 0; i < 16; i++ ) {
          stream->readdata2 ( eht->ammo[i] );
          if ( eht->ammo[i] > eht->typ->weapons->weapon[i].count )
             eht->ammo[i] = eht->typ->weapons->weapon[i].count;
          if ( eht->ammo[i] < 0 )
             eht->ammo[i] = 0;
        }
        
     } else
       for (int i=0; i < eht->typ->weapons->count ;i++ )
          eht->ammo[i] = eht->typ->weapons->weapon[i].count;


    if ( bm & cem_weapstrength ) {
       word old;
       for ( int i = 0; i < 8; i++ ) {
         stream->readdata2 ( old );
         eht->weapstrength[i] = old;
       }
    } else
     if ( bm & cem_weapstrength2 ) {
        for ( int i = 0; i < 16; i++ ) {
          stream->readdata2 ( eht->weapstrength[i] );
        }
     } else
       for (int i=0; i < eht->typ->weapons->count ;i++ )
          eht->weapstrength[i] = eht->typ->weapons->weapon[i].maxstrength;

    if ( bm & cem_loading ) {
       char c;
   
       stream->readdata2 ( c );
   
       if (c)
          for (int k = 0; k < c; k++)
              readunit ( eht->loading[k]  );
    }

    if ( bm & cem_height )
         stream->readdata2 ( eht->height );
    else
       eht->height = chfahrend;

    if ( ! (eht->height & eht->typ->height) )
       eht->height = 1 << log2 ( eht->typ->height );

    if ( bm & cem_movement ) 
         eht->setMovement ( stream->readChar ( ), -1 );
    else
       eht->setMovement ( eht->typ->movement [ log2 ( eht->height ) ], -1 );

    if ( bm & cem_direction )
         stream->readdata2 ( eht->direction );
    else
         eht->direction = 0;

    if ( bm & cem_material )
         stream->readdata2 ( eht->material );
    else
         eht->material = eht->typ->material;

    if ( bm & cem_energy )
         stream->readdata2 ( eht->energy );
    else
         eht->energy = eht->typ->energy;

    if ( bm & cem_class )
         stream->readdata2 ( eht->klasse );
    else
       eht->klasse = 0;

    if ( bm & cem_armor )
         stream->readdata2 ( eht->armor );
    else
       eht->armor = eht->typ->armor;
    
    if ( bm & cem_networkid )
         stream->readdata2 ( eht->networkid );
    else 
       eht->networkid = 0;

    if ( bm & cem_attacked )
         stream->readdata2 ( eht->attacked );

    if ( bm & cem_name     ) 
         stream->readpchar ( &eht->name );
    
    if ( bm & cem_reactionfire )
       eht->reactionfire.status = stream->readChar (  );
    else
       eht->reactionfire.status = 0;

    if ( bm & cem_reactionfire2 )
       eht->reactionfire.enemiesAttackable = stream->readChar (  );
    else
       eht->reactionfire.enemiesAttackable = 0;

    if ( bm & cem_poweron )
       stream->readdata2 ( eht->generatoractive );
    else
       eht->generatoractive = 0;

    if ( bm & cem_energyUsed )
       eht->energyUsed =  stream->readInt ();
    else
       eht->energyUsed = 0;



    #ifdef sgmain
    if (eht->klasse == 255) {
       if ( eht->typ->classnum ) {
          for (int i = 0; i < eht->typ->classnum ; i++ ) 
             if ( spfld->player[ eht->color/8 ].research.vehicleclassavailable( eht->typ, i, spfld ) )
                eht->klasse = i;
             else
                break;
                
          
          eht->functions = eht->typ->functions & eht->typ->classbound[eht->klasse].vehiclefunctions;
       } else {
          eht->functions = eht->typ->functions ;
          eht->klasse = 0;
       }

       eht->armor = eht->typ->armor * eht->typ->classbound[eht->klasse].armor / 1024;
       if (eht->typ->weapons->count ) 
          for ( int m = 0; m < eht->typ->weapons->count ; m++)
             if ( eht->typ->weapons->weapon[m].getScalarWeaponType() >= 0 )
                eht->weapstrength[m] = eht->typ->weapons->weapon[m].maxstrength * eht->typ->classbound[eht->klasse].weapstrength[ eht->typ->weapons->weapon[m].getScalarWeaponType()] / 1024;
             else
                eht->weapstrength[m] = 0;

    } else {
      if ( eht->typ->classnum ) 
        eht->functions = eht->typ->functions & eht->typ->classbound[eht->klasse].vehiclefunctions;
      else
        eht->functions = eht->typ->functions ;
    }
    #endif

}




/**************************************************************/
/*     einzelnes Geb„ude schreiben / lesen                  ÿ */
/**************************************************************/


const int buildingstreamversion = -2;

void         tspfldloaders::writebuilding ( pbuilding bld )
{
    stream->writedata2 ( buildingstreamversion );

    stream->writedata2 ( bld->typ->id );
    stream->writedata2 ( bld->bi_resourceplus );
    stream->writedata2 ( bld->color );
    stream->writedata2 ( bld->xpos );
    stream->writedata2 ( bld->ypos );
    stream->writedata2 ( bld->completion );
    stream->writedata2 ( bld->munitionsautoproduction );

    stream->writedata2 ( bld->plus );
    stream->writedata2 ( bld->maxplus );
    stream->writedata2 ( bld->actstorage );

    stream->writedata2 ( bld->munition );
    stream->writedata2 ( bld->maxresearchpoints );
    stream->writedata2 ( bld->researchpoints );
    stream->writedata2 ( bld->visible );
    stream->writedata2 ( bld->damage );
    stream->writedata2 ( bld->netcontrol );
    stream->writepchar ( bld->name );

    stream->writeInt ( bld->repairedThisTurn );

    char c = 0;
    char k;

    if (bld->typ->loadcapacity )  
       for (k = 0; k <= 31; k++) 
          if (bld->loading[k] ) 
             c++;
    stream->writedata2 ( c );
    if (c)
       for (k = 0; k <= 31; k++) 
          if ( bld->loading[k] )
             writeunit ( bld->loading[k]  );


    c = 0;
    if (bld->typ->special & cgvehicleproductionb ) 
       for (k = 0; k <= 31; k++) 
          if ( bld->production[k] )
             c++;
    stream->writedata2 ( c );
    if (c)
       for (k = 0; k <= 31; k++) 
          if (bld->production[k] != NULL) 
             stream->writedata2( bld->production[k]->id );


    c = 0;
    if (bld->typ->special & cgvehicleproductionb ) 
       for (k = 0; k <= 31; k++) 
          if ( bld->productionbuyable[k] )
             c++;
    stream->writedata2 ( c );
    if (c)
       for (k = 0; k <= 31; k++) 
          if (bld->productionbuyable[k] != NULL) 
             stream->writedata2( bld->productionbuyable[k]->id );

}

void         tspfldloaders::readbuilding ( pbuilding &bld )
{
    bld = new ( tbuilding );
    memset ( bld, 0, sizeof ( *bld ));

   #ifdef HEXAGON
    int id;
   #else
    word id;
   #endif

    int version;
    stream->readdata2 ( version );
    if ( version == buildingstreamversion || version == -1 ) {

       stream->readdata2 ( id );
       bld->typ = getbuildingtype_forid ( id, 0 );
       if ( !bld->typ )
          throw tinvalidid ( "no building with matching ID found; ID = ", id );
   
       stream->readdata2 ( bld->bi_resourceplus );
   
       if ( spfld->objectcrc ) 
          spfld->objectcrc->speedcrccheck->checkbuilding2 ( bld->typ );
   
       stream->readdata2 ( bld->color );
       stream->readdata2 ( bld->xpos );
    } else {
       int idsize = sizeof ( id );
       memcpy ( &id, &version, idsize );
   
       bld->typ = getbuildingtype_forid ( id, 0 );
       if ( !bld->typ )
          throw tinvalidid ( "no building with matching ID found; ID = ", id );

       if ( spfld->objectcrc ) 
          spfld->objectcrc->speedcrccheck->checkbuilding2 ( bld->typ );
   
       if ( idsize == 4 ) {
          stream->readdata2 ( bld->color );
          stream->readdata2 ( bld->xpos );
       } else {
          bld->color = (version >> 16) & 0xff;
          char bb;
          stream->readdata2 ( bb );

          bld->xpos =  ( version >> 24) + (bb << 8);

       }

    }



    stream->readdata2 ( bld->ypos );
    stream->readdata2 ( bld->completion );

    stream->readdata2 ( bld->munitionsautoproduction );

    stream->readdata2 ( bld->plus );
    stream->readdata2 ( bld->maxplus );

    stream->readdata2 ( bld->actstorage );

    stream->readdata2 ( bld->munition );
    stream->readdata2 ( bld->maxresearchpoints );
    stream->readdata2 ( bld->researchpoints );
    stream->readdata2 ( bld->visible );
    stream->readdata2 ( bld->damage );
    stream->readdata2 ( bld->netcontrol );
    stream->readpchar ( &bld->name );
    if ( version == -2 )
       bld->repairedThisTurn = stream->readInt ( );
    else
       bld->repairedThisTurn = 0;



    char c;
    int k;

    stream->readdata2 ( c );
    if (c)
       for (k = 0; k < c; k++)
          readunit ( bld->loading[k]  );

    stream->readdata2 ( c );
    if (c)
       for (k = 0; k < c ; k++) {

           word id;
           stream->readdata2( id );
           bld->production[k] = getvehicletype_forid ( id, 0 ) ;
           if ( !bld->production[k] )
              throw tinvalidid ( "no unit with matching ID found; ID = ", id );

           if ( spfld->objectcrc ) 
              spfld->objectcrc->speedcrccheck->checkunit2 ( bld->production[k] );

       }

    stream->readdata2 ( c );
    if (c)
       for (k = 0; k < c ; k++) {
           word id;
           stream->readdata2( id );
           bld->productionbuyable[k] = getvehicletype_forid ( id, 0 );

           if ( !bld->productionbuyable[k] )
              throw tinvalidid ( "no unit with matching ID found; ID = ", id );

           if ( spfld->objectcrc ) 
              spfld->objectcrc->speedcrccheck->checkunit2 ( bld->productionbuyable[k] );
       }

}




/**************************************************************/
/*     einzelnes Event schreiben / lesen                    ÿ */
/**************************************************************/


void   tspfldloaders::writeevent ( pevent event )
{
   int magic = -1;
   stream->writedata2 ( magic );
   int eventversion = 1;
   stream->writedata2 ( eventversion );

    stream->writedata2( *event );
    if ( event->datasize  &&  event->rawdata ) 
       stream->writedata( event->rawdata, event->datasize);
   
    for (char j = 0; j <= 3; j++) { 
       if ((event->trigger[j] == ceventt_buildingconquered) || 
           (event->trigger[j] == ceventt_buildinglost) || 
           (event->trigger[j] == ceventt_buildingdestroyed) || 
           (event->trigger[j] == ceventt_building_seen )) { 
             if ( event->triggerstatus[j] != 2 ) {
                stream->writedata2( event->trigger_data[j]->building->xpos );
                stream->writedata2( event->trigger_data[j]->building->ypos );
             } else {
                integer w = -1;
                stream->writedata2( w );
                stream->writedata2( w );
             }
       } 
       if ((event->trigger[j] == ceventt_unitconquered) || 
           (event->trigger[j] == ceventt_unitlost) || 
           (event->trigger[j] == ceventt_unitdestroyed)) { 
           int xp = event->trigger_data[j]->vehicle->xpos;
           int yp = event->trigger_data[j]->vehicle->ypos;
           int nwid = event->trigger_data[j]->vehicle->networkid;
           if ( event->triggerstatus[j] == 2 ) {
              xp = -1;
              yp = -1;
              nwid = -1;
           }
           stream->writedata2( xp );
           stream->writedata2( yp );
           stream->writedata2( nwid );
       } 
       if ((event->trigger[j] == ceventt_event) || (event->trigger[j] == ceventt_technologyresearched)) {
          stream->writedata2( event->trigger_data[j]->id );
       } 
       if (event->trigger[j] == ceventt_turn ) {
          stream->writedata2( event->trigger_data[j]->time.abstime );
       }
       if (event->trigger[j] == ceventt_any_unit_enters_polygon || 
           event->trigger[j] == ceventt_specific_unit_enters_polygon) {
              stream->writedata2( *event->trigger_data[j]->unitpolygon );
              int sz = event->trigger_data[j]->unitpolygon->size - sizeof ( *event->trigger_data[j]->unitpolygon );
              stream->writedata( event->trigger_data[j]->unitpolygon->data, sz );
              if ( event->trigger_data[j]->unitpolygon->vehicle ) {
                 int x = event->trigger_data[j]->unitpolygon->vehicle->xpos;
                 int y = event->trigger_data[j]->unitpolygon->vehicle->ypos;
                 int nwid = event->trigger_data[j]->unitpolygon->vehicle->networkid;
                 stream->writedata2( x );
                 stream->writedata2( y );
                 stream->writedata2( nwid );
              }
              
       }
       
    } 

}



void    tspfldloaders::readevent ( pevent& event1 )
{
     int magic;
     int version;
     stream->readdata2( magic );
     if ( magic == -1 ) {
        stream->readdata2 ( version );
        stream->readdata2 ( *event1 );
     } else {
        memcpy ( event1, &magic, sizeof ( magic ));
        int* pi = (int*) event1;
        pi++;
        stream->readdata ( pi, sizeof ( *event1) - sizeof ( int ));
        version = 0;
     }

     event1->next = NULL; 
     event1->conn = 0;
     if ( event1->datasize && event1->rawdata ) {
        event1->rawdata = asc_malloc ( event1->datasize );
        stream->readdata ( event1->rawdata, event1->datasize );
     }  else {
        event1->datasize = 0;
        event1->rawdata = NULL;
     }

     for (char m = 0; m <= 3; m++) {
        if ( !event1->trigger[m] )
           event1->trigger_data[m] = NULL;
        else {
           event1->trigger_data[m] = new LargeTriggerData;

           if ((event1->trigger[m] == ceventt_buildingconquered) || 
              (event1->trigger[m] == ceventt_buildinglost) ||
              (event1->trigger[m] == ceventt_buildingdestroyed) || 
              (event1->trigger[m] == ceventt_building_seen )) {
   
              integer xpos, ypos;
              stream->readdata2 ( xpos );
              stream->readdata2 ( ypos );
              event1->trigger_data[m]->xpos = xpos;
              event1->trigger_data[m]->ypos = ypos;
           } 
   
           if ((event1->trigger[m] == ceventt_unitconquered) || 
              (event1->trigger[m] == ceventt_unitlost) ||
              (event1->trigger[m] == ceventt_unitdestroyed)) {
   
              if ( version == 0 ) {
                 integer xpos, ypos;
                 stream->readdata2 ( xpos );
                 stream->readdata2 ( ypos );
                 event1->trigger_data[m]->xpos = xpos;
                 event1->trigger_data[m]->ypos = ypos;
                 event1->trigger_data[m]->networkid = -1;
              } else {
                 stream->readdata2( event1->trigger_data[m]->xpos );
                 stream->readdata2( event1->trigger_data[m]->ypos );
                 stream->readdata2( event1->trigger_data[m]->networkid );
              }
           } 
   
           if ((event1->trigger[m] == ceventt_event) ||
               (event1->trigger[m] == ceventt_technologyresearched)) {
               stream->readdata2 ( event1->trigger_data[m]->id );
           } 
   
           if (event1->trigger[m] == ceventt_turn) {
               stream->readdata2 ( event1->trigger_data[m]->time.abstime );
           } 
           if (event1->trigger[m] == ceventt_any_unit_enters_polygon || 
               event1->trigger[m] == ceventt_specific_unit_enters_polygon) {
                  event1->trigger_data[m]->unitpolygon = new teventtrigger_polygonentered;
                  stream->readdata2( *event1->trigger_data[m]->unitpolygon );
                  int sz = event1->trigger_data[m]->unitpolygon->size - sizeof ( *event1->trigger_data[m]->unitpolygon );
                  event1->trigger_data[m]->unitpolygon->data = new int [ (sz + sizeof(int) -1 ) / sizeof ( int ) ];
                  stream->readdata( event1->trigger_data[m]->unitpolygon->data, sz );
                  if ( event1->trigger_data[m]->unitpolygon->vehicle ) {
                     stream->readdata2( event1->trigger_data[m]->unitpolygon->tempxpos );
                     stream->readdata2( event1->trigger_data[m]->unitpolygon->tempypos );
                     stream->readdata2( event1->trigger_data[m]->unitpolygon->tempnwid );
                  } 
                  
           }
        }

     } 

}




/**************************************************************/
/*     sezierungen schreiben / lesen                        ÿ */
/**************************************************************/


void   tspfldloaders::readdissections ( void )
{
   for (int i = 0; i < 8; i ++ ) {
      if ( spfld->player[ i ].dissectedunit ) {
         int k = 1;
         spfld->player[ i ].dissectedunit = NULL;

         while ( k ) {
            pdissectedunit du = new tdissectedunit;
            stream->readdata2 ( *du );
            int j;
            stream->readdata2 ( j );
            du->fzt = getvehicletype_forid ( j, 0 );
            if ( !du->fzt )
               throw tinvalidid ( "no vehicle with matching ID found; ID = ", j );

            stream->readdata2 ( j );
            du->tech = gettechnology_forid  ( j, 0 );
            if ( !du->tech )
               throw tinvalidid ( "no technology with matching ID found; ID = ", j );

            k = ( du->next != NULL );
            du->next = spfld->player[ i ].dissectedunit;
            spfld->player[ i ].dissectedunit = du;
         }
      }
   } /* endfor */
}

void   tspfldloaders::writedissections ( void )
{
   for (int i = 0; i < 8; i ++ ) {
      pdissectedunit du = spfld->player[ i ].dissectedunit;
      while ( du ) {
            stream->writedata2 ( *du );

            int j = du->tech->id;
            stream->writedata2 ( j );

            j = du->fzt->id;
            stream->writedata2 ( j );

            du = du->next;
      }
   } /* endfor */
}



/**************************************************************/
/*        Messagess  schreiben / lesen                      ÿ */
/**************************************************************/


void      tspfldloaders:: writemessagelist( pmessagelist lst )
{
   if ( lst ) {
      pmessagelist msg = lst;
      while ( msg ) {
         stream->writedata2 ( msg->message->id );
         msg = msg->next;
      }
      int i = 0;
      stream->writedata2 ( i );
   }
}


void      tspfldloaders:: writemessages ( void )
{
  int i;
   int j = 0xabcdef;
   stream->writedata2 ( j );


   int id = 0;
   pmessage msg = spfld->message;
   while ( msg ) {
      id++;
      msg->id = id;
      stream->writedata2 ( *msg );
      if ( msg->text )
         stream->writepchar ( msg->text );

      msg = msg->next;
   }

   for (i = 0; i < 8; i++ ) 
      writemessagelist ( spfld->player[ i ].oldmessage );

   for ( i = 0; i < 8; i++ ) 
      writemessagelist ( spfld->player[ i ].unreadmessage );

   for ( i = 0; i < 8; i++ ) 
      writemessagelist ( spfld->player[ i ].sentmessage );

   writemessagelist ( spfld->unsentmessage );

   stream->writedata2 ( j );

   if ( spfld->journal )
      stream->writepchar ( spfld->journal );

   if ( spfld->newjournal )
      stream->writepchar ( spfld->newjournal );

}



void      tspfldloaders:: readmessagelist( pmessagelist* lst )
{
   if ( *lst ) {
      int i;
      *lst = NULL;
      do {
          stream->readdata2 ( i );
          pmessage msg = spfld->message;
          if ( i ) {
             while ( msg && msg->id != i )
                msg = msg->next;
   
             if ( msg  &&  msg->id == i ) {
                pmessagelist n = new tmessagelist ( lst );
                n->message = msg;
             } else
                displaymessage ( "message list corrupted !\nplease report this bug!\nthe game will continue, but some messages will probably be missing\nand other instabilities may occur.",1);
          }
      } while ( i ); /* enddo */
   }
}


void      tspfldloaders:: readmessages ( void )
{
   int j;
   int i;
   stream->readdata2 ( j );

   pmessage msg = spfld->message;
   spfld->message = NULL;
   while ( msg ) {
      pmessage msg2 = new tmessage;
      stream->readdata2 ( *msg2 );
      if ( msg2->text )
         stream->readpchar ( &msg2->text );

      msg = msg2->next;
      msg2->next = spfld->message;
      spfld->message = msg2;
   }

   for (i = 0; i < 8; i++ ) 
      readmessagelist ( &spfld->player[ i ].oldmessage );

   for ( i = 0; i < 8; i++ ) 
      readmessagelist ( &spfld->player[ i ].unreadmessage );

   for ( i = 0; i < 8; i++ ) 
      readmessagelist ( &spfld->player[ i ].sentmessage );

   readmessagelist ( &spfld->unsentmessage );


   stream->readdata2 ( j );

   if ( spfld->journal )
      stream->readpchar ( &spfld->journal );

   if ( spfld->newjournal )
      stream->readpchar ( &spfld->newjournal );
}



/**************************************************************/
/*     Events  schreiben / lesen                            ÿ */
/**************************************************************/

void   tspfldloaders::writeeventstocome ( void )
{
       #ifdef logging
       {
           char tmpcbuf[200];
           sprintf(tmpcbuf,"loaders / tspfldloaders::writeeventstocome;" );
           logtofile ( tmpcbuf );
       }    
       #endif
 
    int      j = 0;
    pevent   event = spfld->firsteventtocome;
    while ( event ) {

       #ifdef logging
       {
           char tmpcbuf[2000];
           sprintf(tmpcbuf,"loaders / tspfldloaders::writeeventstocome; #%d addr=%x id=%x player=%d desc=%s data=%x next=%x datasize=%d conn=%x timer=%d delay=%d/%d ", j, event, event->id, event->player, event->description, event->rawdata, event->next, event->datasize, event->conn, event->triggertime.abstime, event->delayedexecution.turn, event->delayedexecution.move );
           logtofile ( tmpcbuf );
           for ( int i = 0; i < 4; i++ ) {
              if ( event->trigger_data[i] )
                 sprintf(tmpcbuf,"  trigger %d : trig=%d addr=%x time=%x xp=%d yp=%d nwid=%d bld=%x veh=%x mapid=%d id=%d unitpoly=%x", i, event->trigger[i], event->trigger_data[i], event->trigger_data[i]->time.abstime, event->trigger_data[i]->xpos, event->trigger_data[i]->ypos, event->trigger_data[i]->networkid, event->trigger_data[i]->building, event->trigger_data[i]->vehicle, event->trigger_data[i]->mapid, event->trigger_data[i]->id, event->trigger_data[i]->unitpolygon );
              else
                 sprintf(tmpcbuf,"  trigger %d : trig=%d addr=%x ", i, event->trigger[i], event->trigger_data[i]);
              logtofile ( tmpcbuf );
           }
       }    
       #endif

       j++;
       event = event->next; 
    } 
 
    stream->writedata2( j );
 
    event = spfld->firsteventtocome; 
    while ( event ) {
       writeevent ( event   );
       event = event->next; 
    } ;
}


void         tspfldloaders::readeventstocome ( void )
{
   int j;
   pevent event1, event2;

   stream->readdata2 ( j );

   int k;
   if ( j ) {
      for (k = 1; k <= j; k++) { 
         event1 = new ( tevent );

         readevent ( event1  );

         if (k == 1) 
            spfld->firsteventtocome = event1; 
         else 
            event2->next = event1; 


         event2 = event1; 
      } 

      event1 = spfld->firsteventtocome;
      while ( event1 ) {
         for ( k = 0; k < 4; k++ )
            if ( event1->trigger[k] == ceventt_event ) {
               event2 = spfld->firsteventtocome;
               while ( event2 ) {
                  if ( event2->id == event1->trigger_data[k]->id )
                     event2->conn |= 1;
                  event2 = event2->next;
               } /* endwhile */
            }


         event1 = event1->next;
      }

   } 
}


void   tspfldloaders::writeeventspassed ( void )
{
    pevent event = spfld->firsteventpassed;
    int j = 0;
    while ( event ) {
       j++;
       event = event->next; 
    } 
                        
    stream->writedata2( j );
 
    event = spfld->firsteventpassed;
    while ( event ) {
       writeevent ( event  );
       event = event->next; 
    } 
}


void   tspfldloaders::readeventspassed ( void )
{                      
   int j;
   pevent event1, event2;
   stream->readdata2 ( j );
   if ( j ) {   
      for (int k = 1; k <= j; k++) {
         event1 = new ( tevent );

         readevent ( event1  );

         if (k == 1) 
            spfld->firsteventpassed = event1;
         else 
            event2->next = event1; 

         event2 = event1; 
      } 
   } 
}

void   tspfldloaders::writeoldevents ( void )
{
     peventstore oldevent =  spfld->oldevents;
     while ( oldevent ) {
        stream->writedata2( oldevent->num );
        if (oldevent->num) {
           stream->writedata ( oldevent->eventid, oldevent->num * sizeof( oldevent->eventid[0] ));
           stream->writedata ( oldevent->mapid, oldevent->num * sizeof( oldevent->mapid[0] ));
        }

        oldevent = oldevent->next; 
     }
     if ( spfld->oldevents ) {
        int n = 0;
        stream->writedata2( n );
     }
}

void   tspfldloaders::readoldevents ( void )
{
   if ( spfld->oldevents ) {
      int  num;
      stream->readdata2 ( num );
      spfld->oldevents = NULL;
      peventstore oldevt = NULL;

      while ( num ) {
         oldevt = new ( teventstore );
         oldevt->num = num;
         oldevt->next = spfld->oldevents;
         stream->readdata ( oldevt->eventid, num * sizeof ( oldevt->eventid[0] ));
         stream->readdata ( oldevt->mapid, num * sizeof ( oldevt->mapid[0] ));

         stream->readdata2 ( num );
      }
      spfld->oldevents = oldevt;
   }
}



/**************************************************************/
/*     map schreiben / lesen / initialisieren         ÿ */
/**************************************************************/

void    tspfldloaders::writemap ( void )
{
  int v;

       #ifdef logging
       logtofile ( "loaders / tspfldloaders::writemap / started" );
       #endif

       if ( !spfld )
          displaymessage ( "tspfldloaders::writemap  ; no map to write ! ",2);

       char* temp[8];
       for (v= 0; v < 8; v++) {
          temp[v] = spfld->player[v].name;
          spfld->player[v].name = NULL;
       }

       stream->writedata2( *spfld );
    
       if ( spfld->title )
          stream->writepchar( spfld->title );
    
       if ( spfld->campaign )
          stream->writedata2( *spfld->campaign );

       for (int w=0; w<8 ; w++ ) {

          // if (spfld->player[w].name)
          //    stream->writepchar ( spfld->player[w].name );

         /*
          if (spfld->player[w].ai)
             stream->writedata2 ( *spfld->player[w].aiparams );
         */

          if ( spfld->humanplayername[w] )
             stream->writepchar ( spfld->humanplayername[w] );
   
          if ( spfld->computerplayername[w] )
             stream->writepchar ( spfld->computerplayername[w] );
       } /* endfor */

       #ifdef logging
       logtofile ( "loaders / tspfldloaders::writemap / names written" );
       #endif

       int t = 0;
       if ( spfld->tribute )
          for (int i = 0; i < 8; i++) {
             for (int j = 0; j < 8; j++) {
                for (int k = 0; k < 3; k++) {
                   if ( spfld->tribute->avail.resource[k][i][j] )
                      t++;
                   if ( spfld->tribute->paid.resource[k][i][j] )
                      t++;
                } /* endfor */
             } /* endfor */
          } /* endfor */


       #ifdef logging
       logtofile ( "loaders / tspfldloaders::writemap / tribute written" );
       #endif

        stream->writedata2 ( t );
        if ( t )
           stream->writedata2 ( *spfld->tribute );

        for ( int i = 0; i < 8; i++ )
           if ( spfld->alliance_names_not_used_any_more[i] ) {
              char nl = 0;
              stream->writedata2 ( nl );
           }


        int h = 0;
        stream->writedata2 ( h );

        for (  v= 0; v < 8; v++) 
           spfld->player[v].name = temp[v];


       #ifdef logging
       logtofile ( "loaders / tspfldloaders::writemap / vor crc" );
       #endif

        if ( spfld->objectcrc ) {
           stream->writedata2 ( *spfld->objectcrc );
    
           if ( spfld->objectcrc->unit.crcnum ) 
              stream->writedata ( spfld->objectcrc->unit.crc, spfld->objectcrc->unit.crcnum * sizeof ( tcrc ) );
    
           if ( spfld->objectcrc->building.crcnum ) 
              stream->writedata ( spfld->objectcrc->building.crc, spfld->objectcrc->building.crcnum * sizeof ( tcrc ) );
    
           if ( spfld->objectcrc->object.crcnum ) 
              stream->writedata ( spfld->objectcrc->object.crc, spfld->objectcrc->object.crcnum * sizeof ( tcrc ) );
    
           if ( spfld->objectcrc->terrain.crcnum ) 
              stream->writedata ( spfld->objectcrc->terrain.crc, spfld->objectcrc->terrain.crcnum * sizeof ( tcrc ) );
    
           if ( spfld->objectcrc->technology.crcnum ) 
              stream->writedata ( spfld->objectcrc->technology.crc, spfld->objectcrc->technology.crcnum * sizeof ( tcrc ) );
    
        }
        if ( spfld->shareview ) 
           stream->writedata2 ( *(spfld->shareview) );

       #ifdef logging
       logtofile ( "loaders / tspfldloaders::writemap / shareview written" );
       #endif

        if ( spfld->preferredfilenames ) {
           stream->writedata2 ( *(spfld->preferredfilenames) );
           for ( int i = 0; i < 8; i++ ) {
              if ( spfld->preferredfilenames->mapname[i] )
                 stream->writepchar ( spfld->preferredfilenames->mapname[i] );
              if ( spfld->preferredfilenames->mapdescription[i] )
                 stream->writepchar ( spfld->preferredfilenames->mapdescription[i] );
              if ( spfld->preferredfilenames->savegame[i] )
                 stream->writepchar ( spfld->preferredfilenames->savegame[i] );
              if ( spfld->preferredfilenames->savegamedescription[i] )
                 stream->writepchar ( spfld->preferredfilenames->savegamedescription[i] );
           }
        }

        if ( spfld->ellipse )
           stream->writedata2 ( *(spfld->ellipse) );

        for ( int ii = 0 ; ii < spfld->gameparameter_num; ii++ )
           stream->writedata2 ( spfld->game_parameter[ii] );

}


void     tmaploaders::initmap ( void )
{
   int i,j;

    for (i = 0; i <= 8; i++) { 
       spfld->player[i].firstvehicle = NULL; 
       spfld->player[i].firstbuilding = NULL; 
       spfld->player[i].research.progress = 0; 
       spfld->player[i].research.activetechnology = NULL; 
       for (j = 0; j <= 11; j++) 
          spfld->player[i].research.unitimprovement.weapons[j] = 1024; 
       spfld->player[i].research.unitimprovement.armor = 1024; 
    } 
    spfld->oldevents = NULL; 
    spfld->firsteventtocome = NULL; 
    spfld->firsteventpassed = NULL; 
    spfld->network          = NULL;
    spfld->game_parameter = NULL;
}


void     tgameloaders::initmap ( void )
{
    for ( int i = 0; i <= 8; i++) { 
       spfld->player[i].firstvehicle = NULL; 
       spfld->player[i].firstbuilding = NULL; 
    } 
    spfld->game_parameter = NULL;
}


void     tspfldloaders::readmap ( void )
{
    spfld = new tmap;

    stream->readdata2 ( *spfld );

    initmap();
 
    if ( spfld->title )
       stream->readpchar( &spfld->title );
 
    if ( spfld->campaign ) {
       spfld->campaign = new ( tcampaign );
       stream->readdata2( *spfld->campaign );
    }

    for (char w=0; w<8 ; w++ ) {
       if (spfld->player[w].name) {
          stream->readpchar ( &spfld->player[w].name );
          delete[] spfld->player[w].name;
          spfld->player[w].name = NULL;
       }
          
      /*
       if (spfld->player[w].aiparams) {
          spfld->player[w].aiparams = new ( taiparams );
          stream->readdata2 ( *spfld->player[w].aiparams );
       }
      */
       spfld->player[w].ai = NULL;


       #ifdef logging
       {
           char tmpcbuf[200];
           sprintf(tmpcbuf,"loaders / tspfldloaders::readmap / humanplayername; org address is %x, del address is %x",spfld->humanplayername[w], spfld->humanplayername[w]);
           logtofile ( tmpcbuf );
       }    
       #endif
      
       if ( spfld->humanplayername[w] ) {
          char* tempname = NULL;
          stream->readpchar ( &tempname );
          spfld->humanplayername[w] = new char[100];
          strncpy( spfld->humanplayername[w], tempname, 99 );
          spfld->humanplayername[w][99] = 0;
          delete[] tempname;
       }

       #ifdef logging
       {
           char tmpcbuf[200];
           sprintf(tmpcbuf,"loaders / tspfldloaders::readmap / humanplayername; new address is %x",spfld->humanplayername[w] );
           logtofile ( tmpcbuf );
       }    
       #endif


       if ( spfld->computerplayername[w] ){
          char* tempname = NULL;
          stream->readpchar ( &tempname );
          spfld->computerplayername[w] = new char[100];
          strncpy( spfld->computerplayername[w], tempname, 99 );
          spfld->computerplayername[w][99] = 0;
          delete[] tempname;
       }

    } /* endfor */


       #ifdef logging
       logtofile ( "loaders / tspfldloaders::readmap / nach namen" );
       #endif

    int t;
    stream->readdata2 ( t );
    spfld->tribute = new ( tresourcetribute );
    if ( t )
       stream->readdata2 ( *spfld->tribute );
    else
       memset ( spfld->tribute, 0, sizeof ( *spfld->tribute ));


       #ifdef logging
       logtofile ( "loaders / tspfldloaders::readmap / vor alliances" );
       #endif

    for ( int i = 0; i < 8; i++ )
       if ( spfld->alliance_names_not_used_any_more[i] ) {
          char* tempname = NULL;
          stream->readpchar ( &tempname );
          delete[] tempname;

          spfld->alliance_names_not_used_any_more[i] = 0;
       }




    int h;
    stream->readdata2 ( h );



    if ( spfld->objectcrc ) {
       #ifdef logging
       logtofile ( "loaders / tspfldloaders::readmap / vor objectcrcs" );
       #endif

       spfld->objectcrc = new tobjectcontainercrcs;
       stream->readdata2 ( *spfld->objectcrc );

       #ifdef logging
       logtofile ( "loaders / tspfldloaders::readmap / vor unitcrcs" );
       #endif
       if ( spfld->objectcrc->unit.crcnum ) {
          spfld->objectcrc->unit.crc = new tcrc[spfld->objectcrc->unit.crcnum];
          stream->readdata ( spfld->objectcrc->unit.crc, spfld->objectcrc->unit.crcnum * sizeof ( tcrc ) );
       } else
          spfld->objectcrc->unit.crc = NULL;

       #ifdef logging
       logtofile ( "loaders / tspfldloaders::readmap / vor buildingcrcs" );
       #endif
       if ( spfld->objectcrc->building.crcnum ) {
          spfld->objectcrc->building.crc = new tcrc[spfld->objectcrc->building.crcnum];
          stream->readdata ( spfld->objectcrc->building.crc, spfld->objectcrc->building.crcnum * sizeof ( tcrc ) );
       } else
          spfld->objectcrc->building.crc = NULL;

       #ifdef logging
       logtofile ( "loaders / tspfldloaders::readmap / vor object.crcs" );
       #endif
       if ( spfld->objectcrc->object.crcnum ) {
          spfld->objectcrc->object.crc = new tcrc[spfld->objectcrc->object.crcnum];
          stream->readdata ( spfld->objectcrc->object.crc, spfld->objectcrc->object.crcnum * sizeof ( tcrc ) );
       } else
          spfld->objectcrc->object.crc = NULL;

       #ifdef logging
       logtofile ( "loaders / tspfldloaders::readmap / vor terraincrcs" );
       #endif
       if ( spfld->objectcrc->terrain.crcnum ) {
          spfld->objectcrc->terrain.crc = new tcrc[spfld->objectcrc->terrain.crcnum];
          stream->readdata ( spfld->objectcrc->terrain.crc, spfld->objectcrc->terrain.crcnum * sizeof ( tcrc ) );
       } else
          spfld->objectcrc->terrain.crc = NULL;

       #ifdef logging
       logtofile ( "loaders / tspfldloaders::readmap / vor techcrcs" );
       #endif
       if ( spfld->objectcrc->technology.crcnum ) {
          spfld->objectcrc->technology.crc = new tcrc[spfld->objectcrc->technology.crcnum];
          stream->readdata ( spfld->objectcrc->technology.crc, spfld->objectcrc->technology.crcnum * sizeof ( tcrc ) );
       } else
          spfld->objectcrc->technology.crc = NULL;

       #ifdef logging
       logtofile ( "loaders / tspfldloaders::readmap / vor speedcrccheck" );
       #endif
       spfld->objectcrc->speedcrccheck = new tspeedcrccheck ( spfld->objectcrc );

    }

    if ( spfld->shareview ) {
       spfld->shareview = new tshareview;
       stream->readdata2 ( *(spfld->shareview) );
    }

    if ( spfld->preferredfilenames ) {
       spfld->preferredfilenames = new PreferredFilenames;
       stream->readdata2 ( *(spfld->preferredfilenames) );
       for ( int i = 0; i < 8; i++ ) {
          if ( spfld->preferredfilenames->mapname[i] )
             stream->readpchar ( &spfld->preferredfilenames->mapname[i] );
          if ( spfld->preferredfilenames->mapdescription[i] )
             stream->readpchar ( &spfld->preferredfilenames->mapdescription[i] );
          if ( spfld->preferredfilenames->savegame[i] )
             stream->readpchar ( &spfld->preferredfilenames->savegame[i] );
          if ( spfld->preferredfilenames->savegamedescription[i] )
             stream->readpchar ( &spfld->preferredfilenames->savegamedescription[i] );
       }
    }

    if ( spfld->ellipse ) {
       spfld->ellipse = new EllipseOnScreen;
       stream->readdata2 ( *(spfld->ellipse) );
    }

    int orggpnum = spfld->gameparameter_num;
    spfld->gameparameter_num = 0;
    for ( int gp = 0; gp < 8; gp ++ )
       spfld->setgameparameter ( gp, spfld->_oldgameparameter[gp] );

    for ( int ii = 0 ; ii < orggpnum; ii++ ) {
       int gpar;
       stream->readdata2 ( gpar );
       spfld->setgameparameter ( ii, gpar );
    }

       #ifdef logging
       logtofile ( "loaders / tspfldloaders::readmap / returning" );
       #endif
}


void           tspfldloaders::freespfld()
{
   if ( spfld ) {

      if ( spfld->title )
         delete[] spfld->title;

      if ( spfld->campaign )
         delete spfld->campaign;
     
      for ( int w=0; w<8 ; w++ )
         if ( spfld->player[w].ai ) 
            delete spfld->player[w].ai;
     

      delete spfld;
   }
   spfld = NULL;
}


/**************************************************************/
/*     Network schreiben / lesen                            ÿ */
/**************************************************************/

void        tspfldloaders::writenetwork ( void )
{
  if ( spfld->network ) {
     int i;
     stream->writedata2 ( *spfld->network );
     for ( i = 0; i < 8  ; i++ ) 
        if (spfld->network->computer[i].name != NULL)
           stream->writepchar ( spfld->network->computer[i].name );

  }
}


void        tspfldloaders::readnetwork ( void )
{
   if ( spfld->network ) {
      int i;
      spfld->network = new ( tnetwork );
      stream->readdata2 ( *spfld->network );
      for (i=0; i<8 ; i++ ) 
         if (spfld->network->computer[i].name != NULL )
            stream->readpchar ( &spfld->network->computer[i].name );
   }
}





/**************************************************************/
/*     Technologien schreiben / lesen                       ÿ */
/**************************************************************/

void      tspfldloaders::writetechnologies ( void )
{
   for (int i = 0; i< 8 ; i++ ) {
      pdevelopedtechnologies devtech = spfld->player[i].research.developedtechnologies;

      while ( devtech ) {
         stream->writedata2( devtech->tech->id );
         devtech = devtech->next;
      } /* endwhile */

      int  w = 0;
      stream->writedata2 ( w );

      if ( spfld->player[i].research.activetechnology )
         stream->writedata2 ( spfld->player[i].research.activetechnology->id );

   } /* endfor */
}

void      tspfldloaders::readtechnologies ( void )
{
    for (int i = 0; i<8 ; i++ ) {
       spfld->player[i].research.developedtechnologies = NULL;

       int  w;
       stream->readdata2 ( w );

       pdevelopedtechnologies devtech;
       while ( w ) {

          ptechnology tec = gettechnology_forid ( w, 0 );
          if ( !tec )
             throw tinvalidid ( "no technology with matching ID found; ID = ", w );

          devtech = new ( tdevelopedtechnologies );
          devtech->tech = tec;

          if ( spfld->objectcrc ) 
             spfld->objectcrc->speedcrccheck->checktech2 ( devtech->tech );

          devtech->next = spfld->player[i].research.developedtechnologies;
          spfld->player[i].research.developedtechnologies = devtech;

          stream->readdata2 ( w );

       } /* endwhile */

       if ( spfld->player[i].research.activetechnology ) {
          stream->readdata2 ( w );

          spfld->player[i].research.activetechnology = gettechnology_forid ( w, 0 );

          if ( !spfld->player[i].research.activetechnology )
             throw tinvalidid ( "no technology with matching ID found; ID = ", w );

          if ( spfld->objectcrc ) 
             spfld->objectcrc->speedcrccheck->checktech2 ( spfld->player[i].research.activetechnology );
       }

    } /* endfor */
 }


/**************************************************************/
/*     Replay Data  schreiben / lesen                       ÿ */
/**************************************************************/

void            tspfldloaders::writereplayinfo ( void )
{
   if ( spfld->replayinfo ) {
       stream->writedata2 ( *(spfld->replayinfo) );
       for ( int i = 0; i < 8; i++ ) {
          if ( spfld->replayinfo->guidata[i] )
             spfld->replayinfo->guidata[i]->writetostream ( stream );
             
          if ( spfld->replayinfo->map[i] )
             spfld->replayinfo->map[i]->writetostream ( stream );
       }
   }
}


void            tspfldloaders::readreplayinfo ( void )
{
   if ( spfld->replayinfo ) {
       spfld->replayinfo = new treplayinfo;
       stream->readdata2 ( *(spfld->replayinfo) );
       for ( int i = 0; i < 8; i++ ) {
          if ( spfld->replayinfo->guidata[i] ) {
             spfld->replayinfo->guidata[i] = new tmemorystreambuf;
             spfld->replayinfo->guidata[i]->readfromstream ( stream );
          }
             
          if ( spfld->replayinfo->map[i] ) {
             spfld->replayinfo->map[i] = new tmemorystreambuf;
             spfld->replayinfo->map[i]->readfromstream ( stream );
          }
       }

       spfld->replayinfo->actmemstream = NULL;
   }
}



/**************************************************************/
/*     fielder schreiben / lesen                             ÿ */
/**************************************************************/

const int objectstreamversion = 1;

void   tspfldloaders::writefields ( void )
{
   int l = 0;
   int cnt1 = spfld->xsize * spfld->ysize;
   int cnt2;
   int l2;

   pfield fld, fld2;

   do { 
      cnt2 = 0; 
      fld = &spfld->field[l];
      if (l + 2 < cnt1) { 
         l2 = l + 1; 
         fld2 = &spfld->field[l2];
         while ((l2 + 2 < cnt1) && ( memcmp(fld2, fld, sizeof(*fld2)) == 0) ) {
            cnt2++;
            l2++;
            fld2 = &spfld->field[l2];
         } 
      } 


      char b1 = 0;
      char b3 = 0;
      char b4 = 0;

      if (fld->typ->terraintype->id > 255) 
         b1 |= csm_typid32; 
      if (fld->direction != 0) 
         b1 |= csm_direction; 
      if (fld->vehicle != NULL) 
         b1 |= csm_vehicle; 
      if (fld->bdt & cbbuildingentry ) 
         b1 |= csm_building; 

      if (cnt2 > 0) 
         b1 |= csm_cnt2; 

      if (fld->material > 0) 
         b3 |= csm_material; 
      if (fld->fuel > 0) 
         b3 |= csm_fuel; 

      if (fld->typ != fld->typ->terraintype->weather[0])
         b3 |= csm_weather;
      if (fld->visible)
         b3 |= csm_visible;
      if (fld->object )
         b4 |= csm_newobject;

      if ( fld->resourceview )
         b4 |= csm_resources;

      if ( fld->connection )
         b4 |= csm_connection;

      if ( b4 )
         b3 |= csm_b4;

      if ( b3 ) 
         b1 |= csm_b3; 

      stream->writedata2( b1 );

      if (b1 & csm_b3 ) 
         stream->writedata2 ( b3 );

      if (b3 & csm_b4 )
         stream->writedata2 ( b4 );

      if (b1 & csm_cnt2 ) 
         stream->writedata2 ( cnt2 );

      if (b3 & csm_weather ) {
         int k = 1;
         while ( fld->typ != fld->typ->terraintype->weather[k]    &&   k < cwettertypennum ) {
            k++;
         } /* endwhile */
         
         if ( k == cwettertypennum ) {
            k = 0;
            displaymessage ( "invalid terrain ( weather not found ) at position %d \n",1,l );
          }
         stream->writedata2 ( k );
      }

      if (b1 & csm_typid32 )                  
         stream->writedata2 ( fld->typ->terraintype->id );
      else {  
         char b2 = fld->typ->terraintype->id;
         stream->writedata2 ( b2 );
      } 

      if (b1 & csm_direction )                    
         stream->writedata2 ( fld->direction );

      if (b1 & csm_vehicle ) 
         writeunit ( fld->vehicle  );


      if (b1 & csm_building ) 
         writebuilding ( fld->building  );

      if (b3 & csm_material ) 
         stream->writedata2 ( fld->material );
      if (b3 & csm_fuel ) 
         stream->writedata2 ( fld->fuel );

      if (b3 & csm_visible )
         stream->writedata2( fld->visible );

      if ( b4 & csm_newobject ) {
         stream->writedata2 ( objectstreamversion );

         stream->writedata2 ( fld->object->minenum );
         for ( int i = 0; i < fld->object->minenum; i++ ) {
            stream->writedata2 ( fld->object->mine[i]->type );
            stream->writedata2 ( fld->object->mine[i]->strength );
            stream->writedata2 ( fld->object->mine[i]->time );
            stream->writedata2 ( fld->object->mine[i]->color );
         }

         stream->writedata2 ( fld->object->objnum );

         for ( int n = 0; n < fld->object->objnum; n++ ) {
            stream->writedata2 ( *fld->object->object[n] );
            stream->writedata2 ( fld->object->object[n]->typ->id );
         }
      }

      if (b4 & csm_resources )
         stream->writedata2 ( *fld->resourceview );

      if ( b4 & csm_connection )
         stream->writedata2 ( fld->connection );

      l += 1 + cnt2;
   }  while (l < cnt1);
}


void tspfldloaders::readfields ( void )
{
   int cnt2 = 0;
        
   int cnt1 = spfld->xsize * spfld->ysize;

       #ifdef logging
       logtofile ( "loaders / tspfldloaders::readfields / vor memalloc" );
       #endif

   spfld->field = new tfield [ cnt1 ];

       #ifdef logging
       logtofile ( "loaders / tspfldloaders::readfields / nach memalloc" );
       #endif

   if (spfld->field == NULL)
      displaymessage ( "Could not allocate memory for map ",2);

   int l = 0;
   do { 
      pfield fld2;
      tfield lfld;

      if (cnt2 == 0) { 

       #ifdef loggging
       {
           char tmpcbuf[200];
           sprintf(tmpcbuf,"loaders / tspfldloaders::readfields  / reading field %d", l );
           logtofile ( tmpcbuf );
       }    
       #endif

         fld2 = & spfld->field[l];

         fld2->a.temp = 0;
         fld2->a.temp2 = 0; 
         fld2->picture = NULL;
         fld2->vehicle = NULL; 
         fld2->building = NULL;
         fld2->object = NULL;
         fld2->resourceview = NULL; 
         fld2->connection = 0;
         fld2->bdt.set ( 0 , 0 );

         char b1, b3, b4;
         stream->readdata2 ( b1 );

         if (b1 & csm_b3 ) 
            stream->readdata2 ( b3 );
         else 
            b3 = 0; 

         if (b3 & csm_b4 )
            stream->readdata2 ( b4 );
         else
            b4 = 0;

         if (b1 & csm_cnt2 ) 
            stream->readdata2 ( cnt2 );
         else
            cnt2 = 0; 

         int weather;
         if (b3 & csm_weather )
            stream->readdata2 ( weather );
         else 
            weather = 0;


         int k;
         char b2;

         if (b1 & csm_typid32 ) 
            stream->readdata2 ( k );
         else { 
            stream->readdata2 ( b2 );
            k = b2; 
         } 
       
         pterraintype trn = getterraintype_forid ( k, 0 );
         if ( !trn ) 
            throw tinvalidid ( "no terrain with matching id found; ID = ", k );

         fld2->typ = trn->weather[weather];
         if ( !fld2->typ ) 
            throw tinvalidid ( "no terrain with matching id found; ID = ", k );

         if ( spfld->objectcrc ) 
            spfld->objectcrc->speedcrccheck->checkterrain2 ( fld2->typ->terraintype );


         if (b1 & csm_direction ) 
            stream->readdata2 ( fld2->direction );
         else                                              
            fld2->direction = 0; 


         if (b1 & csm_vehicle ) 
            readunit ( fld2->vehicle   );


         if (b1 & csm_building ) {
            readbuilding ( fld2->building   );
            fld2->bdt |= cbbuildingentry;
         } else
            fld2->building = NULL;

         if (b3 & csm_material) 
            stream->readdata2 ( fld2->material );
         else 
            fld2->material = 0; 

         if (b3 & csm_fuel) 
            stream->readdata2 ( fld2->fuel );
         else 
            fld2->fuel = 0; 

         if (b3 & csm_visible)
            stream->readdata2 ( fld2->visible );
         else
            fld2->visible = 0;


         if (b3 & csm_object ) {

            if ( !fld2->object )
               fld2->object = new tobjectcontainer;

            char minetype;
            char minestrength;
            stream->readdata2 ( minetype );
            stream->readdata2 ( minestrength );

            if ( minetype >> 4 ) {
               fld2->putmine ( (minetype >> 1) & 7, minetype >> 4, minestrength );
               fld2->object->mine[0]->time = 0;
            } else
               fld2->object->minenum = 0;

            int objnum;
            stream->readdata2 ( objnum );
            fld2->object->objnum = objnum;
                                                 
            pobject object[ 16 ];
            stream->readdata ( object, sizeof ( pobject ) * 16 ); 
         }

         if ( b4 & csm_newobject ) {
            int objectversion;
            stream->readdata2 ( objectversion );

            if ( objectversion != objectstreamversion )
               throw tinvalidversion ( "object", objectstreamversion, objectversion );


            if ( !fld2->object ) 
               fld2->object = new tobjectcontainer;

            int minenum;
            fld2->object->minenum = 0;
            stream->readdata2 ( minenum );
            for ( int i = 0; i < minenum; i++ ) {
               int type;
               int strength;
               int time;
               int color;
               stream->readdata2 ( type );
               stream->readdata2 ( strength );
               stream->readdata2 ( time );
               stream->readdata2 ( color );

               if ( type ) {
                  fld2->object->mine[fld2->object->minenum] = new tmine;
                  fld2->object->mine[fld2->object->minenum]->type = type;
                  fld2->object->mine[fld2->object->minenum]->strength = strength;
                  fld2->object->mine[fld2->object->minenum]->time = time;
                  fld2->object->mine[fld2->object->minenum]->color = color;
                  fld2->object->minenum++;
               }
            }

            stream->readdata2 ( fld2->object->objnum );
         }

         if ( (b3 & csm_object) || (b4 & csm_newobject )) {
            for ( int n = 0; n < fld2->object->objnum; n++ ) {
               fld2->object->object[n] = new tobject;
               stream->readdata2 ( *fld2->object->object[n] );
               int id;
               stream->readdata2 ( id );

               fld2->object->object[n]->typ = getobjecttype_forid ( id, 0 );

               if ( !fld2->object->object[n]->typ )
                  throw tinvalidid ( "no object with matching ID found; ID = ", id );

               if ( spfld->objectcrc ) 
                  spfld->objectcrc->speedcrccheck->checkobj2 ( fld2->object->object[n]->typ );
            }
            fld2->sortobjects();
         }

         if (b4 & csm_resources ) {
            fld2->resourceview = new tresourceview;
            stream->readdata2 ( *fld2->resourceview ); 
         }

         if ( b4 & csm_connection ) 
            stream->readdata2 ( fld2->connection );
         
         // fld2->setparams();

         if (b1 & csm_cnt2 ) 
            memcpy( &lfld, fld2, sizeof ( lfld ));

      } 
      else { 
         memcpy( &spfld->field[l], &lfld,  sizeof ( lfld ));
         cnt2--;
      } 
      l++ ;
   }  while (l < cnt1);

}



/**************************************************************/
/*     Chain Items                                          ÿ */
/**************************************************************/

void   tspfldloaders::chainitems ( void )
{
   pbuilding bld;

   int i = 0;
   for (int y = 0; y < actmap->ysize; y++)
      for (int x = 0; x < actmap->xsize; x++) {
          if ( actmap->field[i].bdt & cbbuildingentry ) {
             bld = actmap->field[i].building;
             actmap->chainbuilding ( bld );
             bld->xpos = x;
             bld->ypos = y;
 
             for ( int k = 0; k <= 31 ; k++ ) 
                if ( bld->loading[ k ] ) {
                   actmap->chainunit ( bld->loading[ k ] );
 
                   bld->loading[ k ] -> setnewposition ( x, y );

                }
          }


          pvehicle eht = actmap->field[i].vehicle;
          if ( eht ) {
             eht->setnewposition ( x, y );
             actmap->chainunit ( eht );
          }
             
          actmap->field[i].setparams();
          i++;
      } /* endfor x */
}



/**************************************************************/
/*     Set Player Existencies                               ÿ */
/**************************************************************/

void   tspfldloaders::setplayerexistencies ( void )
{
   int num = 0;
   for (int sp = 7; sp >= 0; sp--) {
         if ( actmap->player[sp].firstvehicle || actmap->player[sp].firstbuilding ) {
            actmap->player[sp].existent = true;
            actmap->actplayer = sp;
            num++;
         } 
         else 
            actmap->player[sp].existent = false;
   } 

/*
   int anum = 0;
   for ( sp = 0; sp < 8; sp++ )
     if ( actmap->player[sp].existent ) {
        if ( actmap->player[sp].alliance > num )
           actmap->player[sp].alliance = anum;
        anum++;
     }
*/

}


tspfldloaders::tspfldloaders ( void )
{
   spfld = NULL;
}


tspfldloaders::~tspfldloaders ( void )
{
  freespfld();
}












































int          tmaploaders::savemap(char *       name,
                                  char *       description)
{ 
   #ifdef logging
   logtofile ( "loaders / tmaploaders::savemap / started " );
   #endif

   tnfilestream filestream ( name, 2 );

   stream = &filestream;

   spfld = actmap;


   /********************************************************************************/
   /*   Stream initialisieren, Dateiinfo schreiben , map schreiben         ÿ */
   /********************************************************************************/
   {
    
       stream->writepchar ( description    );
       stream->writedata2 ( fileterminator );
       stream->writedata2 ( actmapversion  );


       writemap ( );
   }

   #ifdef logging
   logtofile ( "loaders / tmaploaders::savemap / vor eventstocome" );
   #endif

   writeeventstocome ();

   #ifdef logging
   logtofile ( "loaders / tmaploaders::savemap / vor writefields" );
   #endif

   writefields ();

   #ifdef logging
   logtofile ( "loaders / tmaploaders::savemap / nach writefields" );
   #endif

   stream->writedata2 ( actmapversion );

   spfld = NULL;

   return 0;
} 







tmaploaders :: ~tmaploaders()
{
   if ( oldmap )
      erasemap ( oldmap );
}

int          tmaploaders::loadmap(char *       name )
{ 
    oldmap = actmap;
    actmap = NULL;

    #ifdef logging
    logtofile ( "loaders / tmaploaders::loadmap / opening stream");
    #endif
    tnfilestream filestream ( name, 1);

    stream = &filestream;


    char* description = NULL;
 
    #ifdef logging
    logtofile ( "loaders / tmaploaders::loadmap / reading description");
    #endif
    
    stream->readpchar ( &description );
    delete[] description;
 
    word w;
    stream->readdata2 ( w );
 
    if ( w != fileterminator ) 
       throw tinvalidversion ( name, fileterminator, (int) w );


    int version;
    stream->readdata2( version );
 
    if ( version > actmapversion || version < minmapversion )
       throw tinvalidversion ( name, actmapversion, version );
   

   #ifdef logging
   logtofile ( "loaders / tmaploaders::loadmap / readmap");
   #endif
   readmap ();

   #ifdef logging
   logtofile ( "loaders / tmaploaders::loadmap / readeventstocome");
   #endif
   readeventstocome ();

   #ifdef logging
   logtofile ( "loaders / tmaploaders::loadmap / readfields");
   #endif
   readfields ();


  /*****************************************************************************************************/
  /*   šberprfen,  Stream schlieáen                 ÿ                                                 */
  /*****************************************************************************************************/

   stream->readdata( &version, sizeof(version)); 
   if (version > actmapversion || version < minmapversion ) { 
      erasemap_unchained ( spfld );
      throw tinvalidversion ( name, actmapversion, version );
   } 

   #ifdef logging
   logtofile ( "loaders / tmaploaders::loadmap / erasemap");
   #endif
   erasemap( oldmap );
   oldmap = NULL;
   
   actmap = spfld;
/*
   memcpy ( actmap, spfld, sizeof ( *actmap ));
   delete  ( spfld );
*/
   spfld = NULL;


   #ifdef logging
   logtofile ( "loaders / tmaploaders::loadmap / chainitems");
   #endif
   chainitems ();

   #ifdef logging
   logtofile ( "loaders / tmaploaders::loadmap / setbuildingsonmap");
   #endif
   setbuildingsonmap(); 

   #ifdef logging
   logtofile ( "loaders / tmaploaders::loadmap / setplayerexistencies");
   #endif
   setplayerexistencies ();

   #ifdef logging
   logtofile ( "loaders / tmaploaders::loadmap / seteventtriggers");
   #endif
   seteventtriggers(); 

   actmap->time.a.turn = 1; 
   actmap->time.a.move = 0;
             
   #ifdef logging
   logtofile ( "loaders / tmaploaders::loadmap / calculateallobjects");
   #endif
   calculateallobjects(); 

/*   starthistory();  */
   actmap->levelfinished = false; 

   #ifdef logging
   logtofile ( "loaders / tmaploaders::loadmap / checkplayernames");
   #endif
   checkplayernames ();

   #ifdef logging
   logtofile ( "loaders / tmaploaders::loadmap / returning");
   #endif

   
   return 0;
} 



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */ 


/*
void         readhistory(pfilestream  stream);

void         writehistory(pfilestream  stream);
*/


















int          tsavegameloaders::savegame(char *       name,
                                    char *       description)
{ 

   tnfilestream filestream ( name, 2 );

   stream = &filestream;

   spfld = actmap;

 

   stream->writepchar ( description );
   stream->writedata2 ( fileterminator );

   stream->writedata2( actsavegameversion );
   writemap ();

   writenetwork ( );

   writetechnologies ();
   writemessages();
   writeeventstocome ();
   writeeventspassed ();
   writeoldevents    ();

   writefields ( );

   writedissections();
   writereplayinfo ();

   stream->writedata2 ( actsavegameversion );

   spfld = NULL;

   return 0;
} 





int          tsavegameloaders::loadgame(char *       name )
{ 

   tnfilestream filestream ( name, 1 );

   stream = &filestream;


   char* description = NULL;

   stream->readpchar ( &description );
   delete[] description;

   word w;
   stream->readdata2 ( w );

   if ( w != fileterminator ) 
      throw tinvalidversion ( name, fileterminator, (int) w );
   

   int version;
   stream->readdata2( version );

   if (version > actsavegameversion || version < minsavegameversion ) 
      throw tinvalidversion ( name, actsavegameversion, version );
   

   readmap ();

   readnetwork ();

   readtechnologies ();

   readmessages();

   readeventstocome ();
   readeventspassed ();
   readoldevents    ();

       #ifdef logging
       logtofile ( "loaders / tsavegameloaders::loadgame / vor readfields" );
       #endif
                     
   readfields ( );

       #ifdef logging
       logtofile ( "loaders / tsavegameloaders::loadgame / vor readdissections" );
       #endif
 
   readdissections();

       #ifdef logging
       logtofile ( "loaders / tsavegameloaders::loadgame / vor readreplayinfo" );
       #endif
   readreplayinfo ();
 
    stream->readdata( &version, sizeof(version));
    if (version > actsavegameversion || version < minsavegameversion ) {
       erasemap_unchained ( spfld );
       throw tinvalidversion ( name, actsavegameversion, version );
    } 
 
    erasemap();
    actmap = spfld;
/*
    memcpy ( actmap, spfld, sizeof ( *actmap ));
    delete  spfld; */
    spfld = NULL;
 
 



   chainitems ();

   setbuildingsonmap(); 

//   setplayerexistencies ();

   seteventtriggers(); 

   calculateallobjects(); 

/*   starthistory();  */

   actmap->levelfinished = false; 

   checkplayernames ();

   if ( actmap->objectcrc ) 
      if ( actmap->objectcrc->speedcrccheck->getstatus ( )  ) {
         erasemap();
         throw tnomaploaded();
      }

   if ( actmap->replayinfo ) {
      if ( actmap->replayinfo->actmemstream )
         displaymessage2( "actmemstream already open at begin of turn ",2 );

      if ( actmap->replayinfo->guidata[actmap->actplayer] ) 
         actmap->replayinfo->actmemstream = new tmemorystream ( actmap->replayinfo->guidata[actmap->actplayer], 22 );
      else {
         actmap->replayinfo->guidata[actmap->actplayer] = new tmemorystreambuf;
         actmap->replayinfo->actmemstream = new tmemorystream ( actmap->replayinfo->guidata[actmap->actplayer], 2 );
      }
   }

   return 0;
} 



   /*****************************************************************************************************/
   /*   Netzwerk                                      ÿ                                                 */
   /*****************************************************************************************************/




void         tnetworkloaders::checkcrcs ( void )
{
   if ( actmap )
      if ( actmap->objectcrc ) {
         tspeedcrccheck* ck = actmap->objectcrc->speedcrccheck;
   
         int i;
         for ( i = 0; i < 9; i++ ) {
            pbuilding bld = actmap->player[i].firstbuilding;
            while ( bld ) {
               ck->checkbuilding2 ( bld->typ );
               for (int j = 0; j< 32 ; j++) {
                   ck->checkunit2 ( bld->production[j] );
                   ck->checkunit2 ( bld->loading[j]->typ );
               }
   
               bld = bld->next;
            }
         }
   
         for ( i = 0; i < 8; i++ ) {
            pvehicle eht = actmap->player[i].firstvehicle;
            while ( eht ) {
               ck->checkunit2 ( eht->typ );
               for (int j = 0; j< 32 ; j++) 
                   ck->checkunit2 ( eht->loading[j]->typ );
               
               eht = eht->next;
            }
         }
      }
}



int          tnetworkloaders::savenwgame( pnstream strm, char* description )
{ 
   spfld = actmap;

   stream = strm;

   stream->writepchar ( description );
   stream->writedata2 ( fileterminator );
 
   stream->writedata2( actnetworkversion );

   writemap ();

   writetechnologies ();
   writemessages();

   writenetwork ( );

   stream->writedata2 ( actnetworkversion );

   writeeventstocome ();
   writeeventspassed ();
   writeoldevents    ();


   writefields ( );

   writedissections();
   writereplayinfo ();

   stream->writedata2 ( actnetworkversion );

   spfld = NULL;

   return 0;
} 





int          tnetworkloaders::loadnwgame( pnstream strm )
{ 
   char* name = "network game";

   stream = strm;


   char* description = NULL;
   stream->readpchar ( &description );
   delete[] description;

   word w;
   stream->readdata2 ( w );

   if ( w != fileterminator ) 
      throw tinvalidversion ( name, fileterminator, (int) w );


   int version;
   stream->readdata2( version );

   if (version > actnetworkversion || version < minnetworkversion ) 
      throw tinvalidversion ( name, actnetworkversion, version );
   
   #ifdef logging 
   logtofile ( "loaders / tnetworkloaders::loadnwgame / vor readmap" );
   #endif

   readmap ();

   #ifdef logging
   logtofile ( "loaders / tnetworkloaders::loadnwgame / vor readtechnologies" );
   #endif
   readtechnologies ();


   #ifdef logging
   logtofile ( "loaders / tnetworkloaders::loadnwgame / vor readmessages" );
   #endif
   readmessages();

   #ifdef logging
   logtofile ( "loaders / tnetworkloaders::loadnwgame / vor readnetwork" );
   #endif
   readnetwork ();


   stream->readdata2( version );

   if (version > actnetworkversion || version < minnetworkversion ) 
      throw tinvalidversion ( name, actnetworkversion, version );


   #ifdef logging
   logtofile ( "loaders / tnetworkloaders::loadnwgame / vor readevents" );
   #endif
   readeventstocome ();
   readeventspassed ();
   readoldevents    ();
                     
   #ifdef logging
   logtofile ( "loaders / tnetworkloaders::loadnwgame / vor readfields" );
   #endif
   readfields ( );
 
   readdissections();
   readreplayinfo ();


   stream->readdata2( version );
   if (version > actnetworkversion || version < minnetworkversion ) {
      erasemap_unchained ( spfld );
      throw tinvalidversion ( name, actnetworkversion, version );
   } 

   #ifdef logging
   logtofile ( "loaders / tnetworkloaders::loadnwgame / vor erasemap" );
   #endif
   erasemap();
   actmap = spfld;
/*
   memcpy ( actmap, spfld, sizeof ( *actmap ));
   delete  spfld ; */
   spfld = NULL;





   #ifdef logging
   logtofile ( "loaders / tnetworkloaders::loadnwgame / vor chainitems" );
   #endif
  chainitems ();

   #ifdef logging
   logtofile ( "loaders / tnetworkloaders::loadnwgame / vor setbuildingsonmap" );
   #endif
  setbuildingsonmap(); 

   #ifdef logging
   logtofile ( "loaders / tnetworkloaders::loadnwgame / vor checkcrcs" );
   #endif
  checkcrcs();


//  setplayerexistencies ();

   #ifdef logging
   logtofile ( "loaders / tnetworkloaders::loadnwgame / vor seteventtriggers" );
   #endif
  seteventtriggers(); 

   #ifdef logging
   logtofile ( "loaders / tnetworkloaders::loadnwgame / vor calculateallobjects" );
   #endif
  calculateallobjects(); 

/*   starthistory();  */

  actmap->levelfinished = false; 

   #ifdef logging
   logtofile ( "loaders / tnetworkloaders::loadnwgame / vor checkplayernames" );
   #endif
  checkplayernames ();

   if ( actmap->objectcrc ) 
      if ( actmap->objectcrc->speedcrccheck->getstatus ( )  ) {
         #ifdef logging
         logtofile ( "loaders / tnetworkloaders::loadnwgame / crc check failed; vor erasemap" );
         #endif
         erasemap();
         throw tnomaploaded();
      }

   #ifdef logging
   logtofile ( "loaders / tnetworkloaders::loadnwgame / returning" );
   #endif

  #ifdef sgmain
   getnexteventtime();
  #endif

  return 0;


} 














void  savemap(char *       name,
              char *       description)
{

   #ifdef logging
   logtofile ( "loaders / savemap / started " );
   #endif

   try {
     tmaploaders gl;
     gl.savemap ( name, description );
   } /* endtry */

   catch ( tfileerror err) {
      displaymessage( "file error writing map to filename %s ", 1, err.filename );
   } /* endcatch */
   catch ( terror err) {
      displaymessage( "error writing map ", 1 );
   } /* endcatch */

   #ifdef logging
   logtofile ( "loaders / savemap / finished " );
   #endif

}             

void  loadmap(char *       name )
{
   #ifdef logging
   logtofile ( "loaders.cpp / loadmap / loadmap started ");
   #endif

   try {
     tmaploaders gl;
     gl.loadmap ( name );

      if ( !actmap->preferredfilenames ) {
         actmap->preferredfilenames = new PreferredFilenames;
         memset ( actmap->preferredfilenames, 0 , sizeof ( PreferredFilenames ));
      }
      if ( actmap->preferredfilenames->mapname[0] )
         asc_free ( actmap->preferredfilenames->mapname[0] );
      actmap->preferredfilenames->mapname[0] = strdup ( name );
   }
   catch ( tinvalidid err ) {
      displaymessage( err.msg, 1 );
      if ( actmap->xsize == 0)
         throw tnomaploaded();
   } /* endcatch */
   catch ( tinvalidversion err ) {
      displaymessage( "File %s has invalid version.\nExpected version %d\nFound version %d\n", 1, err.filename, err.expected, err.found );
      if ( actmap->xsize == 0)
         throw tnomaploaded();
   } /* endcatch */
   catch ( tfileerror err) {
      displaymessage( "error reading map filename %s ", 1, err.filename );
      if ( actmap->xsize == 0)
         throw tnomaploaded();
   } /* endcatch */
   catch ( terror ) {
      displaymessage( "error loading map", 1 );
      if ( actmap->xsize == 0)
         throw tnomaploaded();
   } /* endcatch */

   #ifdef logging
   logtofile ( "loaders.cpp / loadmap / loadmap finished ");
   #endif
}


void  savegame(char *       name,
               char *       description)
{
   try {
      tsavegameloaders gl;
      gl.savegame ( name, description );
   }
   catch ( tfileerror err) {
      displaymessage( "error writing map to filename %s ", 1, err.filename );
   } /* endcatch */
   catch ( terror err) {
      displaymessage( "error writing map ", 1 );
   } /* endcatch */
}

void  loadgame(char *       name )
{
   #ifdef logging
   logtofile ( "loaders.cpp / loadgame / loadgame started ");
   #endif

   try {
      tsavegameloaders gl;
      gl.loadgame ( name );
   }
   catch ( tinvalidid err ) {
      displaymessage( err.msg, 1 );
      if ( actmap->xsize == 0)
         throw tnomaploaded();
   } /* endcatch */
   catch ( tinvalidversion err ) {
      displaymessage( "File %s has invalid version.\nExpected version %d\nFound version %d\n", 1, err.filename, err.expected, err.found );
      if ( actmap->xsize == 0)
         throw tnomaploaded();
   } /* endcatch */
   catch ( tfileerror err) {
      displaymessage( "error reading map filename %s ", 1, err.filename );
      if ( actmap->xsize == 0)
         throw tnomaploaded();
   } /* endcatch */
   catch ( terror ) {
      displaymessage( "error loading game", 1 );
      if ( actmap->xsize == 0)
         throw tnomaploaded();
   } /* endcatch */

   #ifdef logging
   logtofile ( "loaders.cpp / loadgame / loadgame finished ");
   #endif

  #ifdef sgmain
   getnexteventtime();
  #endif
}



void  savereplay( int num )
{
   try {
      treplayloaders rl;
      rl.savereplay ( num );
   }
   catch ( terror err) {
      displaymessage( "error saving replay information", 1 );
   } /* endcatch */
}

void  loadreplay( pmemorystreambuf streambuf )
{
   #ifdef logging
   logtofile ( "loaders.cpp / loadreplay / loadreplay started ");
   #endif

   try {
      treplayloaders rl;
      rl.loadreplay ( streambuf );
   }
   catch ( tinvalidid err ) {
      displaymessage( err.msg, 1 );
      if ( actmap->xsize == 0)
         throw tnomaploaded();
   } /* endcatch */
   catch ( tinvalidversion err ) {
      displaymessage( "Replay stream %s has invalid version.\nExpected version %d\nFound version %d\n", 1, err.filename, err.expected, err.found );
      if ( actmap->xsize == 0)
         throw tnomaploaded();
   } /* endcatch */
   catch ( tfileerror err) {
      displaymessage( "error reading map filename %s ", 1, err.filename );
      if ( actmap->xsize == 0)
         throw tnomaploaded();
   } /* endcatch */
   catch ( terror ) {
      displaymessage( "error loading replay", 1 );
      if ( actmap->xsize == 0)
         throw tnomaploaded();
   } /* endcatch */

   #ifdef logging
   logtofile ( "loaders.cpp / loadreplay / loadreplay finished ");
   #endif

}






void treplayloaders :: initmap ( void )
{
    for ( int i = 0; i <= 8; i++) { 
       spfld->player[i].firstvehicle = NULL; 
       spfld->player[i].firstbuilding = NULL; 
    } 
    spfld->game_parameter = NULL;
}


void treplayloaders :: loadreplay ( pmemorystreambuf streambuf )
{

   char* name = "memorystream actmap->replayinfo";

   tmemorystream memstream ( streambuf, 1 );

   stream = &memstream;

   int version;
   stream->readdata2( version );

   if (version > actreplayversion || version < minreplayversion ) 
      throw tinvalidversion ( name, actreplayversion, version );

                   
   readmap ();

   readfields ( );
 
   stream->readdata2( version );
   if (version > actreplayversion || version < minreplayversion ) {
      erasemap_unchained ( spfld );
      throw tinvalidversion ( name, actreplayversion, version );
   } 

   actmap = spfld;
   spfld = NULL;





   chainitems ();

   setbuildingsonmap(); 

//   setplayerexistencies ();

  seteventtriggers(); 

  calculateallobjects(); 

  actmap->levelfinished = false; 

  checkplayernames ();

}



void treplayloaders :: savereplay ( int num )
{


   if ( !actmap->replayinfo ) 
      displaymessage ( "treplayloaders :: savereplay   ;   No replay activated !",2);

   if ( actmap->replayinfo->map[num] ) {
      delete actmap->replayinfo->map[num];
      actmap->replayinfo->map[num] = NULL;
   }

   actmap->replayinfo->map[num] = new tmemorystreambuf;

   tmemorystream memstream ( actmap->replayinfo->map[num], 2 );

   tmap replayfield = *actmap;

   replayfield.campaign = NULL;
   replayfield.title = NULL;
   for ( int i = 0; i < 8; i++ ) {
      replayfield.player[i].dissectedunit = NULL;
      replayfield.player[i].unreadmessage = NULL;
      replayfield.player[i].oldmessage = NULL;
      replayfield.player[i].sentmessage = NULL;
      replayfield.player[i].ai = NULL;
      replayfield.player[i].research.activetechnology = NULL;
      replayfield.player[i].research.developedtechnologies = NULL;
      replayfield.humanplayername[i] = NULL;
      replayfield.computerplayername[i] = NULL;
   }
   replayfield.oldevents = NULL;
   replayfield.firsteventtocome = NULL;
   replayfield.firsteventpassed = NULL;
   replayfield.network = NULL;
   replayfield.tribute = NULL;
   replayfield.unsentmessage = NULL;
   replayfield.message = NULL;
   replayfield.journal = NULL;
   replayfield.newjournal = NULL;
   replayfield.objectcrc = NULL;
   if ( actmap->shareview )
      replayfield.shareview = new tshareview ( actmap->shareview );

   replayfield.replayinfo = NULL;



   stream = &memstream;

   spfld = &replayfield;

   stream->writedata2( actreplayversion );
   writemap ();
           
   writefields ( );

   stream->writedata2 ( actreplayversion );

   spfld = NULL;

}














int validatemapfile ( char* s )
{

   char* description = NULL;

   try {

      tnfilestream stream ( s, 1 );
      stream.readpchar ( &description, 200 );
      if ( description ) {
         delete[]  description ;
         description = NULL;
      }

      word w;
      stream.readdata2 ( w );
      if ( w != fileterminator )
         throw tinvalidversion ( s, fileterminator, (int) w );

      int version;
      stream.readdata2( version );
   
      if (version > actmapversion || version < minmapversion ) 
         throw tinvalidversion ( s, actmapversion, version );

   } /* endtry */

   catch ( terror ) {
       return 0;
   } /* endcatch */

   return 1;
} 





int validateemlfile ( char* s )
{

   char* description = NULL;

   try {

      tnfilestream stream ( s, 1 );
      stream.readpchar ( &description, 200 );
      if ( description ) {
         delete[]  description ;
         description = NULL;
      }
      
      word w;
      stream.readdata2 ( w );
      if ( w != fileterminator )
         throw tinvalidversion ( s, fileterminator, (int) w );

      int version;
      stream.readdata2( version );
   
      if (version > actnetworkversion || version < minnetworkversion ) 
         throw tinvalidversion ( s, actnetworkversion, version );

   } /* endtry */

   catch ( terror ) {
       return 0;
   } /* endcatch */


   return 1;
} 


int validatesavfile ( char* s )
{

   char* description = NULL;

   try {

      tnfilestream stream ( s, 1 );
      stream.readpchar ( &description, 200 );
      if ( description ) {
         delete[]  description ;
         description = NULL;
      }

      word w;
      stream.readdata2 ( w );
      if ( w != fileterminator )
         throw tinvalidversion ( s, fileterminator, (int) w );

      int version;
      stream.readdata2( version );
   
      if (version > actsavegameversion || version < minsavegameversion ) 
         throw tinvalidversion ( s, actsavegameversion, version );

   } /* endtry */

   catch ( terror ) {
       return 0;
   } /* endcatch */

   return 1;
} 











void         savecampaignrecoveryinformation(char*        name,
                                             word         id)
{ 
   displaymessage("This has not been implemented yet, sorry!", 2 );

} 


void deletemessagelist ( pmessagelist list )
{
   if ( list ) {
      while ( list->prev ) 
         list = list->prev;
   
      while ( list->next )
         delete list->next;
   
      delete list;
   }
}


void         erasemap( pmap spfld )
{ 
   if ( !spfld )
      return;
  byte         i; 
  pvehicle     aktvehicle; 
  pbuilding    aktbuilding; 
  pevent       event;
  pevent       event2;
  pdevelopedtechnologies devtech1, devtech2;

   if (spfld->xsize == 0) 
      return;

   for (i = 0; i <= 8; i++) { 
      #ifdef logging
      logtofile ( "5/loaders.cpp / erasemap / deleting units ");
      #endif
      aktvehicle = spfld->player[i].firstvehicle; 
      while (aktvehicle != NULL) { 
         if (aktvehicle->next != NULL) { 
            aktvehicle = aktvehicle->next; 
            delete  ( aktvehicle->prev);
         } 
         else { 
            delete  (aktvehicle);
            aktvehicle = NULL; 
         } 
      } 
      spfld->player[i].firstvehicle = NULL; 


      #ifdef logging
      logtofile ( "5/loaders.cpp / erasemap / deleting buildings ");
      #endif
      aktbuilding = spfld->player[i].firstbuilding; 
      while (aktbuilding != NULL) { 
         if (aktbuilding->next != NULL) { 
            aktbuilding = aktbuilding->next; 
            delete   (aktbuilding->prev); 
         } 
         else { 
            delete  (aktbuilding); 
            aktbuilding = NULL; 
         } 
      } 
      spfld->player[i].firstbuilding = NULL; 


      if ( spfld->player[i].ai ) {
         delete spfld->player[i].ai;
         spfld->player[i].ai = NULL;
      }




      #ifdef logging
      logtofile ( "5/loaders.cpp / erasemap / deleting developed technologies ");
      #endif
      devtech1 = spfld->player[i].research.developedtechnologies;
      while ( devtech1 ) {
         devtech2 = devtech1->next;
         delete   ( devtech1 );
         devtech1 = devtech2;
      } 
      spfld->player[i].research.developedtechnologies = NULL;

   }


   for (i = 0; i < 8; i++) { 
      #ifdef logging
      {
          char tmpcbuf[200];
          sprintf(tmpcbuf,"5/loaders.cpp / erasemap / names ; address is %x",spfld->humanplayername[i] );
          logtofile ( tmpcbuf );
      }    
      #endif
      if ( spfld->humanplayername[i] ) {
         delete[] spfld->humanplayername[i];
         spfld->humanplayername[i] = NULL;
      }
      if ( spfld->computerplayername[i] ) {
         delete[] spfld->computerplayername[i];
         spfld->computerplayername[i] = NULL;
      }
      /*
      if ( spfld->alliancenames[i] ) {
         delete spfld->alliancenames[i];
         spfld->alliancenames[i] = NULL;
      }
      */
   }

   /****************************************/
   /*     Events l”schen                 ÿ */
   /****************************************/


   #ifdef logging
   logtofile ( "loaders.cpp / erasemap / deleting events ");
   #endif
   event = spfld->firsteventtocome; 
   while (event != NULL) { 
      event2 = event; 
      event = event->next; 
      delete  event2; 
   } 
   event = spfld->firsteventpassed; 
   while (event != NULL) { 
      event2 = event; 
      event = event->next; 
      delete event2; 
   } 



   /****************************************/
   /*     Sezierungen l”schen            ÿ */
   /****************************************/
   
   #ifdef logging
   logtofile ( "loaders.cpp / erasemap / deleting dissected units ");
   #endif
   for ( i = 0; i < 8; i++ ) {
      pdissectedunit du = spfld->player[ i ].dissectedunit;
      while ( du ) {
         pdissectedunit du2 = du->next;
         delete du;
         du = du2;
      }
   }


   /****************************************/
   /*        CRCs  l”schen               ÿ */
   /****************************************/

   #ifdef logging
   logtofile ( "loaders.cpp / erasemap / deleting CRCs ");
   #endif
   if ( spfld->objectcrc ) {
      if ( spfld->objectcrc->speedcrccheck )
         delete spfld->objectcrc->speedcrccheck;
      if ( spfld->objectcrc->unit.crc )
         delete spfld->objectcrc->unit.crc;
      if ( spfld->objectcrc->building.crc )
         delete spfld->objectcrc->building.crc;
      if ( spfld->objectcrc->object.crc )
         delete spfld->objectcrc->object.crc;
      if ( spfld->objectcrc->terrain.crc )
         delete spfld->objectcrc->terrain.crc;
      if ( spfld->objectcrc->technology.crc )
         delete spfld->objectcrc->technology.crc;

      delete spfld->objectcrc;
      spfld->objectcrc = NULL;
   }

   /****************************************/
   /*        Messages l”schen            ÿ */
   /****************************************/
   
   #ifdef logging
   logtofile ( "loaders.cpp / erasemap / deleting messages ");
   #endif
   pmessage msg = spfld->message;
   while ( msg ) {
      pmessage temp = msg->next;
      delete msg;
      msg= temp;
   }

   for ( i = 0; i < 8; i++ ) {
      deletemessagelist ( spfld->player[ i ].sentmessage ) ;
      spfld->player[ i ].sentmessage = NULL;

      deletemessagelist ( spfld->player[ i ].unreadmessage );
      spfld->player[ i ].unreadmessage = NULL;

      deletemessagelist ( spfld->player[ i ].oldmessage );
      spfld->player[ i ].oldmessage = NULL;
   }
   deletemessagelist ( spfld->unsentmessage );
   spfld->unsentmessage = NULL;

   if ( spfld->journal ) {
      delete[] spfld->journal;
      spfld->journal = NULL;
   }
   if ( spfld->newjournal ) {
      delete[] spfld->newjournal;
      spfld->newjournal = NULL;
   }

   if ( spfld->shareview ) {
      delete spfld->shareview;
      spfld->shareview = NULL;
   }

   if ( spfld->replayinfo ) {
      delete spfld->replayinfo;
      spfld->replayinfo = NULL;
   }

   if ( spfld->game_parameter ) {
      delete[] spfld->game_parameter;
      spfld->game_parameter = NULL;
   }


   #ifdef logging
   logtofile ( "loaders.cpp / erasemap / deleting fields ");
   #endif
   delete[] spfld->field;
   spfld->field = NULL;
   spfld->xsize = 0; 
   spfld->ysize = 0; 
   spfld->xpos = 0; 
   spfld->ypos = 0; 
   
   cursor.hide(); 
   cursor.posx = 0;
   cursor.posy = 0; 
   
   memset (spfld, 0, sizeof(*spfld));
   #ifdef logging
   logtofile ( "loaders.cpp / erasemap / returning");
   #endif
} 



void         erasemap_unchained( tmap* spfld )
{ 
   if ( !spfld )
      return;


  byte         i; 
  pvehicle     aktvehicle; 
  pbuilding    aktbuilding; 
  pevent       event;
  pevent       event2;

   if (spfld->xsize == 0) return;

   int l;
   for (l=0 ;l < spfld->xsize * spfld->ysize ; l++ ) {

      if ( spfld->field[l].bdt & cbbuildingentry ) {
         aktbuilding = spfld->field[l].building;
         for (i=0; i<31 ; i++ ) 
            if (aktbuilding->loading[i]) {
               if (aktbuilding->loading[i]->name)
                  delete[] aktbuilding->loading[i]->name;
               delete aktbuilding->loading[i] ;
            }
         if (aktbuilding->name)
            delete[] aktbuilding->name;
         delete aktbuilding ;
      }

      aktvehicle = spfld->field[l].vehicle;
      if ( aktvehicle ) {
         for (i=0; i<31 ; i++ ) 
            if (aktvehicle->loading[i]) {
               if (aktvehicle->loading[i]->name)
                  delete[] aktvehicle->loading[i]->name ;
               delete aktvehicle->loading[i] ;
            }
         if (aktvehicle->name)
            delete[] aktvehicle->name;
         delete aktvehicle;
      }

   } /* endfor */

   for (i=0; i<9 ; i++) {
      if ( spfld->player[i].ai  ) {
         delete spfld->player[i].ai;
         spfld->player[i].ai = NULL;
      }


      if ( spfld->humanplayername[i] ) {
         delete[] spfld->humanplayername[i];
         spfld->humanplayername[i] = NULL;
      }
      if ( spfld->computerplayername[i] ) {
         delete[] spfld->computerplayername[i];
         spfld->computerplayername[i] = NULL;
      }
      /*
      if ( spfld->alliancenames[i] ) {
         delete spfld->alliancenames[i];
         spfld->alliancenames[i] = NULL;
      }
      */


      pdevelopedtechnologies devtech1, devtech2;

      devtech1 = spfld->player[i].research.developedtechnologies;
      while ( devtech1 ) {
         devtech2 = devtech1->next;
         delete     devtech1;
         devtech1 = devtech2;
      } 
      spfld->player[i].research.developedtechnologies = NULL;

   }


   /****************************************/
   /*     Events l”schen                 ÿ */
   /****************************************/

   event = spfld->firsteventtocome; 
   while (event != NULL) { 
      event2 = event; 
      event = event->next; 
      delete event2; 
   } 
   event = spfld->firsteventpassed; 
   while (event != NULL) { 
      event2 = event; 
      event = event->next; 
      delete event2; 
   } 


   /****************************************/
   /*     Sezierungen l”schen            ÿ */
   /****************************************/
   
   for ( i = 0; i < 8; i++ ) {
      pdissectedunit du = spfld->player[ i ].dissectedunit;
      while ( du ) {
         pdissectedunit du2 = du->next;
         delete du;
         du = du2;
      }
   }


   /****************************************/
   /*        CRCs  l”schen               ÿ */
   /****************************************/

   if ( spfld->objectcrc ) {
      if ( spfld->objectcrc->speedcrccheck )
         delete spfld->objectcrc->speedcrccheck;
      if ( spfld->objectcrc->unit.crc )
         delete spfld->objectcrc->unit.crc;
      if ( spfld->objectcrc->building.crc )
         delete spfld->objectcrc->building.crc;
      if ( spfld->objectcrc->object.crc )
         delete spfld->objectcrc->object.crc;
      if ( spfld->objectcrc->terrain.crc )
         delete spfld->objectcrc->terrain.crc;
      if ( spfld->objectcrc->technology.crc )
         delete spfld->objectcrc->technology.crc;

      delete spfld->objectcrc;
      spfld->objectcrc = NULL;
   }

   /****************************************/
   /*        Messages l”schen            ÿ */
   /****************************************/
   
   pmessage msg = spfld->message;
   while ( msg ) {
      pmessage temp = msg->next;
      delete msg;
      msg= temp;
   }

   for ( i = 0; i < 8; i++ ) {
      deletemessagelist ( spfld->player[ i ].sentmessage ) ;
      spfld->player[ i ].sentmessage = NULL;

      deletemessagelist ( spfld->player[ i ].unreadmessage );
      spfld->player[ i ].unreadmessage = NULL;

      deletemessagelist ( spfld->player[ i ].oldmessage );
      spfld->player[ i ].oldmessage = NULL;
   }
   deletemessagelist ( spfld->unsentmessage );
   spfld->unsentmessage = NULL;


   if ( spfld->journal ) {
      delete[] spfld->journal;
      spfld->journal = NULL;
   }
   if ( spfld->newjournal ) {
      delete[] spfld->newjournal;
      spfld->newjournal = NULL;
   }
   if ( spfld->shareview ) {
      delete spfld->shareview;
      spfld->shareview = NULL;
   }
   if ( spfld->replayinfo ) {
      delete spfld->replayinfo;
      spfld->replayinfo = NULL;
   }
   if ( spfld->game_parameter ) {
      delete[] spfld->game_parameter;
      spfld->game_parameter = NULL;
   }


   delete[] spfld->field;
   memset (spfld, 0, sizeof( *spfld ));
} 





void         loadstreets(void)
{ 

#ifdef HEXAGON
  int          w;

  tnfilestream stream ( "hexmines.raw", 1 ); 
  for ( int i = 0; i < 4; i++) 
      stream.readrlepict( &icons.mine[i], false, &w);
  
#else

  int          w;
  word         y;
  void*        p;


   tnfilestream stream ( "mines.dat", 1 ); 
   stream.readdata2 ( y );
   for ( int i = 0; i <= y; i++) { 
      stream.readrlepict( &p,false,&w);
      streets.mineposition[i].position = p; 
   } 
#endif
} 




void         loadallvehicletypes(void)
{ 
   {
      tfindfile ff ( "*.veh" );
      char *c = ff.getnextname();
                       
      while ( c ) {
          if ( actprogressbar )
             actprogressbar->point();
    
          pvehicletype t = loadvehicletype( c );

          // if ( t->steigung && ((t->height & 6 ) == 6 )) 
          //   t->steigung = 0;

          addvehicletype ( t );
          if ( verbosity >= 2)
            printf("vehicletype %s loaded\n", c );


          c = ff.getnextname();
       }
   }
} 


void         loadallobjecttypes (void)
{ 
  tfindfile ff ( "*.obl" );

  char *c = ff.getnextname();

  while ( c ) {
      if ( actprogressbar )
         actprogressbar->point();

      addobjecttype ( loadobjecttype( c )); 

      if ( verbosity >= 2)
         printf("objecttype %s loaded\n", c );

      c = ff.getnextname();
   }

   #ifndef converter
   for ( int i = 0; i < objecttypenum; i++ ) {
      pobjecttype vt = getobjecttype_forpos ( i );
      for ( int j = 0; j < vt->objectslinkablenum; j++ )
          vt->objectslinkable[j] = getobjecttype_forid ( vt->objectslinkableid[j] );
   }
   #endif

} 



void         loadalltechnologies(void)
{ 
  int i;

  tfindfile ff ( "*.tec" );
  char *c = ff.getnextname();

  while ( c ) {
      if ( actprogressbar )
         actprogressbar->point();

      addtechnology ( loadtechnology( c ));
      if ( verbosity >= 2)
         printf("technology %s loaded\n", c );

      c = ff.getnextname();
   } 

   for (i = 0; i < technologynum; i++) 
      for (int l = 0; l < 6; l++) { 
         ptechnology tech = gettechnology_forpos ( i, 0 );
         int j = tech->requiretechnologyid[l]; 
         if ( j > 0 ) 
            tech->requiretechnology[l] = gettechnology_forid ( j ); 
      } 

   for (i = 0; i < technologynum; i++) 
      gettechnology_forpos ( i, 0 ) -> getlvl();

} 


void         loadallterraintypes(void)
{ 
   tfindfile ff ( "*.trr" );

   char *c = ff.getnextname();

   while( c ) {
      if ( actprogressbar )
         actprogressbar->point();

      addterraintype ( loadterraintype( c ));
      if ( verbosity >= 2)
         printf("terraintype %s loaded\n", c );

      c = ff.getnextname();
   } 
};




void         loadallbuildingtypes(void)
{ 
   tfindfile ff ( "*.bld" );

   char *c = ff.getnextname();

   while( c ) {
      if ( actprogressbar )
         actprogressbar->point();

      addbuildingtype ( loadbuildingtype( c ));
      if ( verbosity >= 2)
         printf("buildingtype %s loaded\n", c );

      c = ff.getnextname();
   } 
} 



void         loadicons(void)
{ 
  int w2;
  int          *w = & w2, i;


  {
      int xl[5] = { cawar, cawarannounce, capeaceproposal, capeace, capeace_with_shareview };
      tnfilestream stream ("allianc2.raw",1); 
      for ( i = 0; i < 5; i++ )
         stream.readrlepict( &icons.diplomaticstatus[xl[i]],false,w); 
      icons.diplomaticstatus[canewsetwar1] = icons.diplomaticstatus[cawar];
      icons.diplomaticstatus[canewsetwar2] = icons.diplomaticstatus[cawar];
      icons.diplomaticstatus[canewpeaceproposal] = icons.diplomaticstatus[capeaceproposal];
  }
   
  {
      tnfilestream stream ("iconship.raw",1); 
      stream.readrlepict( &icons.statarmy[2],false,w);
  }
   
  {
      tnfilestream stream ("icontank.raw",1); 
      stream.readrlepict( &icons.statarmy[1],false,w);
  }
   
  {
      tnfilestream stream ("iconplan.raw",1);
      stream.readrlepict( &icons.statarmy[0],false,w);
  }
   
  {
      tnfilestream stream ("pfeil2.raw",1); 
      stream.readrlepict( &icons.weapinfo.pfeil1, false, w);
  }
   
  {
      tnfilestream stream ("pfeil3.raw",1);
      stream.readrlepict( &icons.weapinfo.pfeil2, false, w);
  }
   
  {
      tnfilestream stream ("height.raw",1); 
      for (i = 0; i <= 7; i++) 
         stream.readrlepict( &icons.height[i],false,w);
   }

   weapDist.loaddata(); 
} 



tspeedcrccheck :: tspeedcrccheck ( pobjectcontainercrcs crclist )
{
   status = 0;

   strng = NULL;
   strnglen = 0;

   int i;

/*
   for ( i = 0; i <= maxterrainanz; i++ )
      bdt[i] = 0;

   for ( i = 0; i <= maxvehicletypeenanz; i++ )
      fzt[i] = 0;

   for ( i = 0; i <= maxbuildingnumber; i++ )
      bld[i] = 0;

   for ( i = 0; i <= maxtechnologynumber; i++ )
      tec[i] = 0;

   for ( i = 0; i <= maxobjectnumber; i++ )
      obj[i] = 0;
*/

   list = crclist;







   for ( i = 0; i < list->unit.crcnum; i++ ) 
      if ( getcrc ( getvehicletype_forid ( list->unit.crc[i].id, 0 )) == list->unit.crc[i].crc )
         fzt[ list->unit.crc[i].id ] = 1;
      else    
         fzt[ list->unit.crc[i].id ] = 2;

   for ( i = 0; i < list->building.crcnum; i++ ) 
      if ( getcrc ( getbuildingtype_forid ( list->building.crc[i].id, 0 )) == list->building.crc[i].crc )
         bld[ list->building.crc[i].id ] = 1;
      else
         bld[ list->building.crc[i].id ] = 2;

   for ( i = 0; i < list->object.crcnum; i++ ) 
      if ( getcrc ( getobjecttype_forid ( list->object.crc[i].id, 0 )) == list->object.crc[i].crc )
         obj[ list->object.crc[i].id ] = 1;
      else
         obj[ list->object.crc[i].id ] = 2;

   for ( i = 0; i < list->terrain.crcnum; i++ ) 
      if ( getcrc ( getterraintype_forid ( list->terrain.crc[i].id, 0 )) == list->terrain.crc[i].crc )
         bdt[ list->terrain.crc[i].id ] = 1;
      else
         bdt[ list->terrain.crc[i].id ] = 2;

   for ( i = 0; i < list->technology.crcnum; i++ ) 
      if ( getcrc ( gettechnology_forid ( list->technology.crc[i].id, 0 )) == list->technology.crc[i].crc )
         tec[ list->technology.crc[i].id ] = 1;
      else
         tec[ list->technology.crc[i].id ] = 2;
}




void tspeedcrccheck :: additemtolist ( pcrcblock lst, int id, int crc )
{
    pcrc pc = lst->crc;
    lst->crc = new tcrc[ lst->crcnum + 1 ];
    for ( int i = 0; i < lst->crcnum; i++ )
       lst->crc[i] = pc[i];

    lst->crc[ lst->crcnum ].crc = crc;
    lst->crc[ lst->crcnum ].id  = id;

    if ( pc )
       delete pc;

    lst->crcnum++;
}


int  tspeedcrccheck :: checkunit     ( pvehicletype f, int add )
{
   if ( !f )
      return 1;

   int stat = fzt[ f->id ];
   if ( stat == 1 )
      return 1;
   else
      if ( stat == 2 )
         return 0;
      else
         if ( stat == 0 ) {
            if ( list->unit.restricted == 0 ||  ( add == 0  && list->unit.restricted == 1 ))
               return 1;
            else
              if ( list->unit.restricted == 1 ) {

                 additemtolist ( &list->unit, f->id, getcrc ( f ) );
                 appendstring ( "unit", f->description, f->id, 0 );

                 fzt[ f->id ] = 1;

                 return 1;
              } else
                 if ( list->unit.restricted == 2 )
                    return 0;

               
         }
   return 0;
}

int  tspeedcrccheck :: checkbuilding     ( pbuildingtype b, int add )
{
   if ( !b )
      return 1;

   int stat = bld[ b->id ];
   if ( stat == 1 )
      return 1;
   else
      if ( stat == 2 )
         return 0;
      else
         if ( stat == 0 ) {
            if ( list->building.restricted == 0 ||  ( add == 0  && list->building.restricted == 1 ) )
               return 1;
            else
              if ( list->building.restricted == 1 ) {
                 additemtolist ( &list->building, b->id, getcrc ( b ) );
                 appendstring ( "building", b->name, b->id, 0 );

                 bld[ b->id ] = 1;

                 return 1;
              } else
                 if ( list->building.restricted == 2 )
                    return 0;

               
         }
   return 0;
}

int  tspeedcrccheck :: checktech     ( ptechnology t, int add )
{
   if ( !t )
      return 1;

   int stat = tec[ t->id ];
   if ( stat == 1 )
      return 1;
   else
      if ( stat == 2 )
         return 0;
      else
         if ( stat == 0 ) {
            if ( list->technology.restricted == 0 ||  ( add == 0  && list->technology.restricted == 1 ))
               return 1;
            else
              if ( list->technology.restricted == 1 ) {
                 additemtolist ( &list->technology, t->id, getcrc ( t ) );
                 appendstring ( "technology", t->name, t->id, 0 );
                 tec[ t->id ] = 1;

                 return 1;
              } else
                 if ( list->technology.restricted == 2 )
                    return 0;

               
         }
   return 0;
}




int  tspeedcrccheck :: checkobj   ( pobjecttype o, int add )
{
   if ( !o )
      return 1;

   int stat = obj[ o->id ];
   if ( stat == 1 )
      return 1;
   else
      if ( stat == 2 )
         return 0;
      else
         if ( stat == 0 ) {
            if ( list->object.restricted == 0 ||  ( add == 0  && list->object.restricted == 1 ))
               return 1;
            else
              if ( list->object.restricted == 1 ) {
                 additemtolist ( &list->object, o->id, getcrc ( o ) );
                 appendstring ( "object", o->name, o->id, 0 );

                 obj[ o->id ] = 1;

                 return 1;
              } else
                 if ( list->object.restricted == 2 )
                    return 0;

               
         }
   return 0;
}

int  tspeedcrccheck :: checkterrain    ( pterraintype b, int add )
{
   if ( !b )
      return 1;

   int stat = bdt[ b->id ];
   if ( stat == 1 )
      return 1;
   else
      if ( stat == 2 )
         return 0;
      else
         if ( stat == 0 ) {
            if ( list->terrain.restricted == 0  ||  ( add == 0  && list->terrain.restricted == 1 ))
               return 1;
            else
              if ( list->terrain.restricted == 1 ) {
                 additemtolist ( &list->terrain, b->id, getcrc ( b ) );
                 appendstring ( "terrain", b->name, b->id, 0 );

                 bdt[ b->id ] = 1;

                 return 1;
              } else
                 if ( list->terrain.restricted == 2 )
                    return 0;

               
         }
   return 0;
}


void tspeedcrccheck :: appendstring ( char* s, char* d, int id, int mode )
{
   char st[200];
   switch ( mode ) {
      case 0:sprintf ( st, "NOTE : the crc of the %s named %s , id %d was appended to the crc-list\n", s, d, id );
         break;
      case 1:sprintf ( st, "#color4#ERROR#color0#: the %s named %s , id %d , failed the crc-check\n", s, d, id );
         break;
      case 2:sprintf ( st, "#color4#ERROR#color0#: the crc of the %s named %s , id %d , is not in the crc-list\n", s, d, id );
         break;
      case 3:sprintf ( st, "#color4#ERROR#color0#: the %s named %s , id %d , has an invalid crc\n", s, d, id );
         break;
   } /* endswitch */

   if ( !strng ) {
      strnglen = 500;
      strng = new char[strnglen];
      strng[0] = 0;
   }

#ifdef _DOS_
   if ( _heapchk() != _HEAPOK ) 
      beep();
#endif

   while ( strlen ( strng ) + strlen ( st ) > strnglen ) {
      char* tmp = strng;
      int newsize = strnglen + 500;
      strng = new char[ newsize ];
      strcpy ( strng, tmp );
      delete[] tmp;
      strnglen = newsize;
   }

   strcat ( strng, st );
}



int tspeedcrccheck :: checkunit2     ( pvehicletype f,     int add  )
{
   int s = checkunit ( f, add );
   if ( !s  &&  add == 1  ) {
      appendstring ( "unit", f->description, f->id, 1 );
      status = 1;
   }
   return s;
}

int tspeedcrccheck :: checkbuilding2 ( pbuildingtype b,    int add   )
{
   int s = checkbuilding ( b, add );
   if ( !s  &&  add == 1  ) {
      appendstring ( "building", b->name, b->id, 1 );
      status = 1;
   }
   return s;
}

int tspeedcrccheck :: checktech2     ( ptechnology t,    int add    )
{
   int s = checktech ( t, add );
   if ( !s  &&  add == 1  ) {
      appendstring ( "technology", t->name, t->id, 1 );
      status = 1;
   }
   return s;
}

int tspeedcrccheck :: checkobj2      ( pobjecttype o,    int add    )
{
   int s = checkobj ( o, add );
   if ( !s  &&  add == 1  ) {
      appendstring ( "object", o->name, o->id, 1 );
      status = 1;
   }
   return s;
}

int tspeedcrccheck :: checkterrain2  ( pterraintype b,    int add    )
{
   int s = checkterrain ( b, add );
   if ( !s  &&  add == 1 ) {
      appendstring ( "terrain", b->name, b->id, 1 );
      status = 1;
   }
   return s;
}



int  tspeedcrccheck :: getstatus ( void )
{
   if ( strng  && strng[0] ) {
      tviewanytext tvat;
      tvat.init ( "crc status", strng ,60, 90, 520, 300 );
      tvat.run();
      tvat.done();
      delete[] strng;
      strng = NULL;
   }
   return status;
}
