/*! \file missions.cpp
    \brief The event handling of ASC
*/

//     $Id: missions.cpp,v 1.33 2002-12-23 13:43:22 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.32  2002/11/01 12:40:50  mbickel
//      Added supervisor-save-map function
//
//     Revision 1.31  2001/12/19 17:16:29  mbickel
//      Some include file cleanups
//
//     Revision 1.30  2001/12/14 10:20:05  mbickel
//      Cleanup and enhancements to configure.in
//      Removed last remains of octagonal version from source files
//
//     Revision 1.29  2001/10/28 20:42:17  mbickel
//      Fixed AI crashes
//      Fixed problems when next campaign map could not be found.
//      Fixed forgotten events.
//      Added several new experimental map parameters
//      Some improvements to the campaign maps
//
//     Revision 1.28  2001/10/02 14:06:28  mbickel
//      Some cleanup and documentation
//      Bi3 import tables now stored in .asctxt files
//      Added ability to choose amoung different BI3 import tables
//      Added map transformation tables
//
//     Revision 1.27  2001/08/19 12:50:03  mbickel
//      fixed event trigger allenemybuildings
//
//     Revision 1.26  2001/08/19 12:31:26  mbickel
//      Fixed several bugs in event and campaign handling
//
//     Revision 1.25  2001/07/28 11:19:12  mbickel
//      Updated weaponguide
//      moved item repository from spfst to itemrepository
//
//     Revision 1.24  2001/02/26 12:35:22  mbickel
//      Some major restructuing:
//       new message containers
//       events don't store pointers to units any more
//       tfield class overhauled
//
//     Revision 1.23  2001/02/11 11:39:40  mbickel
//      Some cleanup and documentation
//
//     Revision 1.22  2001/02/01 22:48:45  mbickel
//      rewrote the storing of units and buildings
//      Fixed bugs in bi3 map importing routines
//      Fixed bugs in AI
//      Fixed bugs in mapeditor
//
//     Revision 1.21  2001/01/28 17:19:13  mbickel
//      The recent cleanup broke some source files; this is fixed now
//
//     Revision 1.20  2001/01/28 14:04:14  mbickel
//      Some restructuring, documentation and cleanup
//      The resource network functions are now it their own files, the dashboard
//       as well
//      Updated the TODO list
//
//     Revision 1.19  2001/01/23 21:05:20  mbickel
//      Speed up of AI
//      Lot of bugfixes in AI
//      Moved Research to own files (research.*)
//      Rewrote storing of developed technologies
//      Some cleanup and documentation
//
//     Revision 1.18  2000/12/23 13:19:47  mbickel
//      Made ASC compileable with Borland C++ Builder
//
//     Revision 1.17  2000/11/11 11:05:19  mbickel
//      started AI service functions
//
//     Revision 1.16  2000/11/08 19:31:11  mbickel
//      Rewrote IO for the tmap structure
//      Fixed crash when entering damaged building
//      Fixed crash in AI
//      Removed item CRCs
//
//     Revision 1.15  2000/10/26 18:14:59  mbickel
//      AI moves damaged units to repair
//      tmap is not memory layout sensitive any more
//
//     Revision 1.14  2000/10/18 14:14:15  mbickel
//      Rewrote Event handling; DOS and WIN32 may be currently broken, will be
//       fixed soon.
//
//     Revision 1.13  2000/10/11 14:26:44  mbickel
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
//     Revision 1.12  2000/09/17 15:20:33  mbickel
//      AI is now automatically invoked (depending on gameoptions)
//      Some cleanup
//
//     Revision 1.11  2000/08/21 17:51:00  mbickel
//      Fixed: crash when unit reaching max experience
//      Fixed: crash when displaying research image
//      Fixed: crash when events referenced a unit that has been shot down
//      Fixed: screenshot being written to wrong directory
//
//     Revision 1.10  2000/08/12 12:52:49  mbickel
//      Made DOS-Version compile and run again.
//
//     Revision 1.9  2000/07/05 10:49:36  mbickel
//      Fixed AI bugs
//      setbuildingdamage event now updates the screen
//
//     Revision 1.8  2000/07/05 09:24:00  mbickel
//      New event action: change building damage
//
//     Revision 1.7  2000/05/23 20:40:47  mbickel
//      Removed boolean type
//
//     Revision 1.6  2000/04/27 16:25:26  mbickel
//      Attack functions cleanup
//      New vehicle categories
//      Rewrote resource production in ASC resource mode
//      Improved mine system: several mines on a single field allowed
//      Added unitctrl.* : Interface for vehicle functions
//        currently movement and height change included
//      Changed timer to SDL_GetTicks
//
//     Revision 1.5  1999/12/29 17:38:17  mbickel
//      Continued Linux port
//
//     Revision 1.4  1999/11/23 21:07:34  mbickel
//      Many small bugfixes
//
//     Revision 1.3  1999/11/22 18:27:40  mbickel
//      Restructured graphics engine:
//        VESA now only for DOS
//        BASEGFX should be platform independant
//        new interface for initialization
//      Rewrote all ASM code in C++, but it is still available for the Watcom
//        versions
//      Fixed bugs in RLE decompression, BI map importer and the view calculation
//
//     Revision 1.2  1999/11/16 03:42:08  tmwilson
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

#include <cstring>

#include "global.h"
#include "buildingtype.h"
#include "vehicletype.h"
#include "newfont.h"
#include "typen.h"
#include "basegfx.h"

#include "spfst.h"
#include "loaders.h"
#include "missions.h"
#include "dlg_box.h"
#include "dialog.h"
#include "weather.h"
#include "gamedlg.h"
#include "controls.h"
#include "sg.h"
#include "errors.h"
#include "dashboard.h"
#include "itemrepository.h"

const  int    translatetriggerstatus[4]  = {0, 1, 1, 0};


   int         stt[4]; 
   GameTime    nexttimedevent[8];
   char      eject; 





/////////////////////////////////////////////////  ev->player == 8 ??




void         getnexteventtime(void)
{ 
  int b;

  for (b = 0; b < 8; b++ )
    nexttimedevent[b].abstime = maxint; 

   pevent ev = actmap->firsteventtocome; 
   while ( ev ) { 
      for ( b = 0; b <= 3; b++) 
         if (ev->trigger[b] == ceventt_turn) { 
            int t = ev->trigger_data[b]->time.abstime; 
            if ( ev->player == 8  ||  1 ) {
               for ( int i = 0; i < 8; i++ )
                  if ( t < nexttimedevent[i].abstime ) 
                     nexttimedevent[i].abstime = t; 

            } else
               if ( t < nexttimedevent[ev->player].abstime )
                  nexttimedevent[ev->player].abstime = t; 

         } 

      if ( ev->triggertime.abstime != -1 ) {
         int t = ev->triggertime.abstime; 
         if ( ev->player == 8 || 1 ) {
            for ( int i = 0; i < 8; i++ )
               if ( t < nexttimedevent[i].abstime ) 
                  nexttimedevent[i].abstime = t; 

         } else
            if ( t < nexttimedevent[ev->player].abstime )
               nexttimedevent[ev->player].abstime = t; 

      }

      ev = ev->next; 
   } 
} 


void         checktimedevents ( MapDisplayInterface* md )
{ 
   eject = false; 
   if ( actmap->time.abstime >= nexttimedevent[actmap->actplayer].abstime ) { 
      checkevents( md ); 
      if (eject) 
        return;
      getnexteventtime(); 
   } 
   if ( actmap->player[actmap->actplayer].queuedEvents ) {
      checkevents( md ); 
      if (eject) 
        return;
   }

} 





int         evaluatetrigger(pevent       ev,
                             int *   bracketstoopen,
                             int     pos,
                             int *   mxpos)
{
  int     bto ;
  int         stat, stat2;
  int         conn;
  int     mxp;

   (*bracketstoopen)--;
   if (*bracketstoopen == 0) {
      *mxpos = pos + 1;
      if (pos < 0) {
         stat = 1;
         conn = ceventtrigger_and;
      }
      else {
         stat = translatetriggerstatus[ev->triggerstatus[pos]];
         conn = ( ev->triggerconnect[pos + 1] & (ceventtrigger_and | ceventtrigger_or | ceventtrigger_not) );
      }
   }
   else {
      *mxpos = pos;
      stat = 1;
      conn = ceventtrigger_and;
   }

   bto = *bracketstoopen;
   do {
      pos = *mxpos;
      if ((pos < 3) && (stt[pos] == 0)) {
         if (ev->triggerconnect[pos] & ceventtrigger_2klammerauf )
            bto+=2;
         if (ev->triggerconnect[pos] & ceventtrigger_klammerauf )
            bto++;
         stt[pos]++;
      }

      if (bto > 0) {
         stat2 = evaluatetrigger(ev,&bto,pos,&mxp);
         if (mxp > *mxpos)
            *mxpos = mxp;
      }
      else {
         (*mxpos)++;
         stat2 = translatetriggerstatus[ev->triggerstatus[pos]];
      }

      if (conn & ceventtrigger_not )
         stat2 = 1 - stat2;

      if (conn & ceventtrigger_and )
         stat = stat * stat2;
      else
         stat = stat + stat2;
      if (stat > 1)
         stat = 1;

      if (*mxpos < 4)
         conn = ( ev->triggerconnect[*mxpos] & (ceventtrigger_and | ceventtrigger_or | ceventtrigger_not));

      if (stt[pos] < 2) {
         if (ev->triggerconnect[pos] & ceventtrigger_2klammerzu )
            bto-=2;
         if (ev->triggerconnect[pos] & ceventtrigger_klammerzu )
            bto--;
         stt[pos]++;
      }

   }  while (!((bto != 0) || (*mxpos > 3) || (ev->trigger[*mxpos] == 0)));

   if (bto != 0)
      *bracketstoopen = bto + 1;

   return stat;

}



void         checksingleevent(pevent       ev, MapDisplayInterface* md )
{
  int         b;
  word         i;
  pevent       ev2;
  peventstore  oldevent;
  int     si1, si2;

   eject = false; 

  if ( ev->triggertime.abstime != -1 )
     execevent ( ev, md );
  else {

      for (b = 0; b <= 3; b++) { 
         if (ev->triggerstatus[b] < 2) { 
            switch (ev->trigger[b]) {
               
               case ceventt_turn:   { 
                                 if ( ( actmap->time.abstime >=  ev->trigger_data[b]->time.abstime )  && (actmap->actplayer == ev->player  || ev->player == 8 )) 
                                    ev->triggerstatus[b] = 2; 
                                 else 
                                    ev->triggerstatus[b] = 0; 
   
                              } 
               break; 
                                
               case ceventt_buildingconquered:   { 
                                             if ( ev->trigger_data[b]->building->color == ev->player * 8) 
                                                ev->triggerstatus[b] = 1; 
                                             else 
                                                ev->triggerstatus[b] = 0; 
                                          } 
               break; 
               
               case ceventt_building_seen:   { 
                                             int player = 0;
                                             if ( ev->player == 8 )
                                                player = 255;
                                             else
                                                player = 1 << ev->player;

                                             int cnt = 0;
                                             for ( int p = 0; p < 8; p++ )
                                                if ( player & (1 << p))
                                                   for ( int x = 0; x < 4; x++ )
                                                      for ( int y = 0; y < 6; y++ ) {
                                                         pbuilding bld = ev->trigger_data[b]->building;
                                                         if ( bld->typ->getpicture ( BuildingType::LocalCoordinate(x, y) ) ) {
                                                            pfield fld = bld->getField ( BuildingType::LocalCoordinate( x, y) );
                                                            if ( fld ) {
                                                               int vis = (fld-> visible >> (p*2) ) & 3;
                                                               if ( bld->typ->buildingheight >= chschwimmend && bld->typ->buildingheight <= chhochfliegend ) {
                                                                  if ( vis >= visible_now )
                                                                     cnt++;
                                                               } else {
                                                                  if ( vis == visible_all )
                                                                     cnt++;
                                                               }
                                                            }
                                                         }
                                                      }
                                             if ( cnt ) {
                                                ev->triggerstatus[b] = 2; 
                                                // ev->trigger_data[b]->building->connection &= ~cconnection_seen;
                                             }
                                             else 
                                                ev->triggerstatus[b] = 0; 
                                          } 
               break; 
               
               case ceventt_buildinglost:   { 
                                        if (ev->trigger_data[b]->building->color != ev->player * 8) 
                                           ev->triggerstatus[b] = 1; 
                                        else 
                                           ev->triggerstatus[b] = 0; 
                                     } 
               break; 
               
               case ceventt_unitlost:   {
                                    pvehicle v = actmap->getUnit ( ev->trigger_data[b]->networkid );
                                    if ( !v )
                                       ev->triggerstatus[b] = 2;
                                    else {
                                       if (v->color != ev->player * 8)
                                          ev->triggerstatus[b] = 1;
                                       else
                                          ev->triggerstatus[b] = 0;
                                    }
                                 } 
               break;

               case ceventt_unitdestroyed:   {
                                    if ( actmap->getUnit ( ev->trigger_data[b]->networkid ))
                                       ev->triggerstatus[b] = 0;
                                    else
                                       ev->triggerstatus[b] = 2;
                                 }
               break;

               case ceventt_technologyresearched:   { 
                                                ev->triggerstatus[b] = 0; 
                                                if ( ev->player == 8 ) {
                                                   for ( int i = 0; i < 8; i++ ) {
                                                       if ( actmap->player[i].research.technologyresearched ( ev->trigger_data[b]->id ))
                                                          ev->triggerstatus[b] = 2;
                                                   }
                                                } else {
                                                   if ( actmap->player[ev->player].research.technologyresearched ( ev->trigger_data[b]->id ))
                                                      ev->triggerstatus[b] = 2;
                                                }
                                             } 
               break; 
               
               case ceventt_event:   { 
                                 ev->triggerstatus[b] = 0; 
                                 if ( !ev->trigger_data[b]->mapid ) {
                                    ev2 = actmap->firsteventpassed; 
                                    while (ev2 != NULL) { 
                                       if (ev2->id == ev->trigger_data[b]->id) 
                                          ev->triggerstatus[b] = 1; 
                                       ev2 = ev2->next; 
                                    } 
                                 }
   
                                 oldevent = actmap->oldevents; 
                                 while (oldevent != NULL) { 
                                    for (i = 0; i < oldevent->num ; i++) 
                                       if (oldevent->eventid[i] == ev->trigger_data[b]->id && oldevent->mapid[i] == ev->trigger_data[b]->mapid) 
                                          ev->triggerstatus[b] = 2; 
                                    oldevent = oldevent->next;
                                 } 
                              } 
               break; 
               
               case ceventt_unitconquered:   {
                                         pvehicle v = actmap->getUnit ( ev->trigger_data[b]->networkid );
                                         if ( !v ) {
                                            ev->triggerstatus[b] = 3;
                                         } else {
                                            if ( v->color == ev->player * 8)
                                               ev->triggerstatus[b] = 1;
                                            else
                                               ev->triggerstatus[b] = 0;
                                         }
                                      } 
               break; 
               
               
               case ceventt_allenemyunitsdestroyed:   { 
                                                  ev->triggerstatus[b] = 1; 
                                                  int sp = ev->player;
                                                  if ( sp == 8 )
                                                     sp = actmap->actplayer;
   
                                                  for (i = 0; i <= 7; i++) 
                                                     if ( ev->trigger_data[b]->id == 1 ) {
                                                        if (i != sp ) 
                                                           if (getdiplomaticstatus2(sp * 8, i * 8) == cawar) 
                                                              if ( !actmap->player[i].vehicleList.empty() )
                                                                 ev->triggerstatus[b] = 0; 
                                                     } else {
                                                        if (i != sp ) 
                                                          if ( ev->trigger_data[b]->id & ( 1 << (2+i)))
                                                              if ( !actmap->player[i].vehicleList.empty() )
                                                                 ev->triggerstatus[b] = 0;
                                                     }
                                               } 
               break; 
               
               case ceventt_allunitslost:   { 
                                               int sp = ev->player;
                                               if ( sp == 8 )
                                                   sp = actmap->actplayer;
   
                                               ev->triggerstatus[b] = actmap->player[sp].vehicleList.empty();
                                            } 
               break; 
               
               case ceventt_allenemybuildingsdestroyed:   { 
                                                     int sp = ev->player;
                                                     if ( sp == 8 )
                                                        sp = actmap->actplayer;
   
                                                      ev->triggerstatus[b] = 1; 
                                                      for (i = 0; i <= 7; i++) 
                                                         if ( ev->trigger_data[b]->id == 1 ) {
                                                            if (i != sp )
                                                               if (getdiplomaticstatus2(sp * 8, i * 8) == cawar)
                                                                  if ( !actmap->player[i].buildingList.empty() )
                                                                     ev->triggerstatus[b] = 0;
                                                         } else {
                                                            if (i != sp )
                                                               if ( ev->trigger_data[b]->id & ( 1 << (2+i)))
                                                                  if ( !actmap->player[i].buildingList.empty())
                                                                     ev->triggerstatus[b] = 0;
                                                         }
                                                   }
               break;

               case ceventt_allbuildingslost:   {
                                            int sp = ev->player;
                                            if ( sp == 8 )
                                               sp = actmap->actplayer;
   
                                            ev->triggerstatus[b] = actmap->player[sp].buildingList.empty();
                                         }
   
               break; 
               case ceventt_energytribute: {
                                              int sp = ev->player;
                                              if ( sp == 8 )
                                                 sp = actmap->actplayer;
                                              int amountrequired = ev->trigger_data[b]->id & 0x00ffffff;
                                              int player = ev->trigger_data[b]->id >> 24;
                                              if ( actmap->tribute.avail[player][sp].energy < amountrequired )
                                                 ev->triggerstatus[b] = 1; 
                                              else 
                                                 ev->triggerstatus[b] = 0; 
                                          }
               break;
               case ceventt_materialtribute: {
                                              int sp = ev->player;
                                              if ( sp == 8 )
                                                 sp = actmap->actplayer;
   
                                              int amountrequired = ev->trigger_data[b]->id & 0x00ffffff;
                                              int player = ev->trigger_data[b]->id >> 24;
                                              if ( actmap->tribute.avail[player][sp].material < amountrequired )
                                                 ev->triggerstatus[b] = 1; 
                                              else 
                                                 ev->triggerstatus[b] = 0; 
                                          }
               break;
               case ceventt_fueltribute: {
                                              int sp = ev->player;
                                              if ( sp == 8 )
                                                 sp = actmap->actplayer;
   
                                              int amountrequired = ev->trigger_data[b]->id & 0x00ffffff;
                                              int player = ev->trigger_data[b]->id >> 24;
                                              if ( actmap->tribute.avail[player][sp].fuel < amountrequired )
                                                 ev->triggerstatus[b] = 1; 
                                              else 
                                                 ev->triggerstatus[b] = 0; 
                                          }
               break;
               case ceventt_any_unit_enters_polygon:
               case ceventt_specific_unit_enters_polygon: {
                                                             int res = unit_in_polygon (  ev->trigger_data[b]->unitpolygon );
                                                             if ( res )
                                                                ev->triggerstatus[b] = 2; 
                                                             else
                                                                ev->triggerstatus[b] = 0;
                                                           }
               break;
               
   
            } 
         } 
   
      } 
   
   
      si1 = 1; 
      si2 = -1; 
   
      memset(stt, 0, sizeof(stt));
      if (evaluatetrigger(ev,&si1,-1,&si2) == 1) 
         execevent( ev, md ); 

  } // triggertime == -1

} 



// Muá noch auf ev->player == 8 umgestellt werden !!!!!!!!!!!!! #######

void         releaseevent(pvehicle     eht,
                          pbuilding    bld,
                          int      action)
{ 
  int         b; 

   pevent ev1 = actmap->firsteventtocome; 
   while ( ev1 != NULL) { 
      if (bld != NULL) 
         for (b = 0; b <= 3; b++)
            if ( ev1->trigger_data[b] ) {
               if (ev1->trigger_data[b]->building == bld) {
                  if (action == cconnection_destroy) {
                     if (ev1->trigger[b] == ceventt_buildingdestroyed) {
                        ev1->triggerstatus[b] = 2;
                        actmap->player[ev1->player].queuedEvents++;
                        ev1->trigger[b] = ceventt_irrelevant;
                     }
                     else
                        if ((ev1->trigger[b] == ceventt_buildingconquered) || (ev1->trigger[b] == ceventt_buildinglost)) {
                           ev1->triggerstatus[b] = 3;
                           actmap->player[ev1->player].queuedEvents++;
                           ev1->trigger[b] = ceventt_irrelevant;
                        }
                  }
                  if (action == cconnection_conquer) {
                     if (ev1->trigger[b] == ceventt_buildingconquered) {
                        if (ev1->player == bld->color / 8)   /*  eventuell allies  */
                           ev1->triggerstatus[b] = 1;
                        else
                           ev1->triggerstatus[b] = 0;
                        actmap->player[ev1->player].queuedEvents++;
                     }

                     else
                        if (ev1->trigger[b] == ceventt_buildinglost) {
                           if (ev1->player == bld->color / 8)   /*  eventuell allies  */
                              ev1->triggerstatus[b] = 0;
                           else
                              ev1->triggerstatus[b] = 1;

                           actmap->player[ev1->player].queuedEvents++;
                        }
                  }

                  if (action == cconnection_lose) {
                     if (ev1->trigger[b] == ceventt_buildingconquered) {
                        if (ev1->player == bld->color / 8)   /*  eventuell allies  */
                           ev1->triggerstatus[b] = 0;
                        else
                           ev1->triggerstatus[b] = 1;
                        actmap->player[ev1->player].queuedEvents++;
                     }
                     else
                        if (ev1->trigger[b] == ceventt_buildinglost) {
                           if (ev1->player == bld->color / 8)   /*  eventuell allies  */
                              ev1->triggerstatus[b] = 1;
                           else
                              ev1->triggerstatus[b] = 0;
                           actmap->player[ev1->player].queuedEvents++;
                        }
                  }

                  if (action == cconnection_seen )
                     if (ev1->trigger[b] == ceventt_building_seen )
                        actmap->player[ev1->player].queuedEvents++;


               }
            }

      if (eht != NULL) 
         for (b = 0; b <= 3; b++)
            if ( ev1->trigger_data[b] ) {
               /*
               if (ev1->trigger_data[b]->vehicle == eht) {
                  if (action == cconnection_destroy) {
                     if (ev1->trigger[b] == ceventt_unitdestroyed) {
                        ev1->triggerstatus[b] = 2;
                        actmap->player[ev1->player].queuedEvents++;
                        ev1->trigger[b] = ceventt_irrelevant;
                     }
                     else
                        if ((ev1->trigger[b] == ceventt_unitconquered) || (ev1->trigger[b] == ceventt_unitlost)) {
                           ev1->triggerstatus[b] = 3;
                           actmap->player[ev1->player].queuedEvents++;
                           ev1->trigger[b] = ceventt_irrelevant;
                        }
                  }
                  if (action == cconnection_conquer) {
                     if (ev1->trigger[b] == ceventt_unitconquered) {
                        if (ev1->player == eht->color / 8)
                           ev1->triggerstatus[b] = 1;
                        else
                           ev1->triggerstatus[b] = 0;
                        actmap->player[ev1->player].queuedEvents++;
                     }
                     else
                        if (ev1->trigger[b] == ceventt_unitlost) {
                           if (ev1->player == eht->color / 8)   //  eventuell allies
                              ev1->triggerstatus[b] = 0;
                           else
                              ev1->triggerstatus[b] = 1;
                           actmap->player[ev1->player].queuedEvents++;
                        }
                  }

                  if (action == cconnection_lose) {
                     if (ev1->trigger[b] == ceventt_unitconquered) {
                        if (ev1->player == eht->color / 8)   //  eventuell allies
                           ev1->triggerstatus[b] = 0;
                        else
                           ev1->triggerstatus[b] = 1;
                        actmap->player[ev1->player].queuedEvents++;
                     }
                     else
                        if (ev1->trigger[b] == ceventt_unitlost) {
                           if (ev1->player == eht->color / 8)   //  eventuell allies
                              ev1->triggerstatus[b] = 1;
                           else
                              ev1->triggerstatus[b] = 0;
                           actmap->player[ev1->player].queuedEvents++;
                        }
                  }
               } */
               if ( (ev1->trigger[b] == ceventt_any_unit_enters_polygon  &&
                    action          == cconnection_areaentered_anyunit ) ||
                    (ev1->trigger[b] == ceventt_specific_unit_enters_polygon  &&
                    action          == cconnection_areaentered_specificunit )) {
                       int res = unit_in_polygon (  ev1->trigger_data[b]->unitpolygon );
                       if ( res ) {
                          ev1->triggerstatus[b] = 2;
                          actmap->player[ev1->player].queuedEvents++;
                       }


               }
            }


      ev1 = ev1->next; 
   } 
} 

void    viewtextmessage ( int id, int player )
{
   ASCString txt = readtextmessage( id );
   if ( !txt.empty() ) {

      int to;
      if ( player < 8 )
         to = (1 << player);
      else
         to = 0xff;

      new Message ( txt, actmap, to );
      if ( player == actmap->actplayer )
         viewunreadmessages (  );
   }
}




void         executeevent ( pevent ev, MapDisplayInterface* md )
{ 
  pevent       ev2;
  ptechnology  tech; 

  if ( actmap->actplayer == ev->player  ||  ev->player == 8 ) {
      eject = false; 
      if (ev->a.action == cemessage) { 
         viewtextmessage ( ev->a.saveas , actmap->actplayer ); 
      } 
   
      if (ev->a.action == cenewtechnology) { 
         tech = gettechnology_forid ( ev->a.saveas );
         if ( tech )
            if ( actmap->player[ ev->player ].research.technologyresearched ( ev->a.saveas ) == 0 ) {
      
               showtechnology(tech); 
   
               tprotfzt   pfzt;
               pfzt.initbuffer ();
   
               actmap->player[ ev->player ].research.addanytechnology( tech );
   
               pfzt.evalbuffer ();
               if ( actmap->player[ ev->player ].research.activetechnology == tech )
                  actmap->player[ ev->player ].research.activetechnology = NULL;
            } 
      } 
   
      if (ev->a.action == ceeraseevent) { 
         /*
         ev3 = NULL; 
         pl = ev2->intdata; 
         ev2 = actmap->firsteventpassed; 
         while (ev2 != NULL) { 
            if (ev2->id == *pl) { 
               if (ev3 == NULL) 
                  actmap->firsteventpassed = ev2->next; 
               else  
                  ev3->next = ev2->next; 
               if (ev2->rawdata )
                  delete ev2->rawdata;
               delete ( ev2 );
                       
               if (ev3 == NULL) 
                  ev2 = actmap->firsteventpassed; 
               else 
                  ev2 = ev3->next; 
            } 
                
            ev3 = ev2; 
            ev2 = ev2->next; 
         } 
   
         oldevent = actmap->oldevents; 
         while (oldevent != NULL) { 
            w = 0; 
            while (w < oldevent->num) { 
               if (oldevent->id[w] == *pl) { 
                  memcpy( &oldevent->id[w + 1], &oldevent->id[w],(oldevent->num - 1 - w) * 4); 
                  oldevent->num--;
               } 
               else 
                  w++;
            } 
            oldevent = oldevent->next; 
         } 
         */
         displaymessage("the eraseevent action has been removed !", 1 );
      } 
   
      if (ev->a.action == cenextmap) {
         if ( !actmap->continueplaying ) {
            if (actmap->campaign != NULL) {
               startnextcampaignmap(ev->a.saveas);
               eject = true;
               return;
            }
            else {
              viewtext2(904);
              if (choice_dlg("Do you want to continue playing ?","~y~es","~n~o") == 2) {
                 delete actmap;
                 actmap = NULL;
                 throw NoMapLoaded();
              } else {
                 actmap->continueplaying = 1;
                 if ( actmap->replayinfo ) {
                    delete actmap->replayinfo;
                    actmap->replayinfo = 0;
                 }
              }
            }
         }
      }
   

      if (ev->a.action == cewindchange) {
         int* i = ev->intdata;
         for ( int j = 0; j < 3; j++ ) {
            if ( i[j] != -1 )
               actmap->weather.wind[j].speed = i[j];

            if ( i[j+3] != -1 )
               actmap->weather.wind[j].direction = i[j+3];
            
         }
         dashboard.x = 0xffff;
         resetallbuildingpicturepointers();
         if ( md )
            md->displayMap();

      }


      if (ev->a.action == ceweatherchange) {
         int* i = ev->intdata;
         int  wthr = *i;
         i++;
   
   
         /*int intensity = *i;
         i++;*/

         int addressmode = *i;
         i++;

         if ( addressmode == 0) {           // Polygonadressierung
            int polygonnum = *i;
            i++;
   
            int j;
            for (j=0; j < polygonnum ;j++ ) {
               ppolygon poly = new  tpolygon ;
               poly->vertexnum = *i;
               i++;
   
               poly->vertex = (tpunkt*) i;
               i += poly->vertexnum * 2;
   
               setweatherpoly ( wthr, 0, poly );
               delete poly;
            } /* endfor */
   
         } else    
            if ( addressmode == 1) 
               setweatherall ( wthr, 0 );
   
      }
   
   
   
      if (ev->a.action == cemapchange ) {
         int* i = ev->intdata;
         int  bdtid = *i;
         i++;

         // int direction = *i;
         i++;
   
         int addressmode = *i;
         i++;
         
         if ( addressmode == 1) {           // Polygonadressierung
            int polygonnum = *i;
            i++;
   
            int j;
            for (j=0; j < polygonnum ;j++ ) {
               ppolygon poly = new  tpolygon ;
               poly->vertexnum = *i;
               i++;
   
               poly->vertex = (tpunkt*) i;
               i += poly->vertexnum * 2;
   
               setbdtpoly ( bdtid, poly );
               delete poly;
            } /* endfor */
   
         } else
            if ( addressmode == 1) 
               setbdtall ( bdtid  );
   
      }
   
   
      if (ev->a.action == ceweatherchangecomplete ) {
         checkobjectsforremoval();
         checkunitsforremoval ();

         dashboard.x = 0xffff;
         if ( md )
            md->displayMap();
      }
       
      if ( ev->a.action == cegameparamchange ) {
         int* i = ev->intdata;
         if ( gameParameterChangeableByEvent [ i[0] ] )
            actmap->setgameparameter( GameParameter(i[0]) , i[1] );
      }

      if ( ev->a.action == ceellipse ) {
         if ( !actmap->ellipse )
            actmap->ellipse = new EllipseOnScreen; 

         if ( ev->intdata[4] ) {     // x orientation
            actmap->ellipse->x1 = agmp->resolutionx - ( 640 - ev->intdata[0] );
            actmap->ellipse->x2 = agmp->resolutionx - ( 640 - ev->intdata[2] );
         } else {
            actmap->ellipse->x1 = ev->intdata[0];
            actmap->ellipse->x2 = ev->intdata[2];
         }

         if ( ev->intdata[5] ) {     // y orientation
            actmap->ellipse->y1 = agmp->resolutiony - ( 480 - ev->intdata[1] );
            actmap->ellipse->y2 = agmp->resolutiony - ( 480 - ev->intdata[3] );
         } else {
            actmap->ellipse->y1 = ev->intdata[1];
            actmap->ellipse->y2 = ev->intdata[3];
         }

         actmap->ellipse->color = white;
         actmap->ellipse->precision = 0.15;
         actmap->ellipse->active = 1;
         if ( md ) {
            dashboard.paint( getactfield(), actmap->playerView );
            actmap->ellipse->paint();
         }
      }

      if ( ev->a.action == ceremoveellipse ) 
         if ( actmap->ellipse ) {
            actmap->ellipse->active = 0;
            if ( md ) 
               repaintdisplay();
         }

      if ( ev->a.action == celosecampaign ) {
         displaymessage ( "you have lost", 3 );
         delete actmap;
         actmap = NULL;
         throw NoMapLoaded();
      }


      if ( ev->a.action == cechangebuildingdamage ) {
         pfield fld = getfield ( ev->intdata[0], ev->intdata[1] );
         if ( fld && fld->building ) {
            if ( ev->intdata[2] >= 100 ) {
               delete fld->building;
               fld->building = NULL;
               if ( md )
                 md->displayMap();
            } else
               fld->building->damage  = ev->intdata[2];

            dashboard.x = 0xffff;
         }
      }


    
      if (actmap->firsteventtocome == ev) { 
         actmap->firsteventtocome = ev->next; 

      } 
      else { 
   
         ev2 = actmap->firsteventtocome; 
         while ((ev2->next != ev) && (ev2->next != NULL)) 
            ev2 = ev2->next; 
   
         ev2->next = ev->next; 
   
      } 
      ev->next = NULL; 
   
      if (actmap->firsteventpassed == NULL) { 
         actmap->firsteventpassed = ev; 
      } 
      else { 
         ev2 = actmap->firsteventpassed; 
         while (ev2->next != NULL) 
            ev2 = ev2->next; 
         ev2->next = ev; 
      } 


      if ( ev->conn & 1 )
         for ( int i = 0; i < 9; i++ )
            actmap->player[i].queuedEvents++;

  } else
     actmap->player[ev->player].queuedEvents++;
} 


void execevent ( pevent ev, MapDisplayInterface* md )
{
   if ( ev->triggertime.abstime != -1 ) {
      if ( actmap->time.abstime >= ev->triggertime.abstime ) 
         executeevent ( ev, md );

   } else {

      if ( ev->delayedexecution.move  || ev->delayedexecution.turn ) {
   
         if ( ev->triggertime.abstime == -1 ) {
            ev->triggertime.set ( actmap->time.turn() + ev->delayedexecution.turn, actmap->time.move() + ev->delayedexecution.move );
            if ( ev->triggertime.move() < 0 )
               ev->triggertime.set ( ev->triggertime.turn(), 0 );
         }
   
         if ( actmap->time.abstime >= ev->triggertime.abstime ) 
            executeevent ( ev, md );
         else
            getnexteventtime();
   
   
      } else
         executeevent ( ev, md );

   }
}




void         checkevents( MapDisplayInterface* md )
{ 
   eject = false; 
   actmap->player[actmap->actplayer].queuedEvents++;
   while ( actmap->player[actmap->actplayer].queuedEvents ) {

      actmap->player[actmap->actplayer].queuedEvents = 0;
   
      pevent ev = actmap->firsteventtocome; 
      while ( ev ) { 
         checksingleevent(ev, md ); 
         if (eject) return;
             
         ev = ev->next; 
      } 
   }

} 





void initmissions( void )
{ 
   memset ( nexttimedevent, 0, sizeof ( nexttimedevent )); 
   eject = false;
}





void         startnextcampaignmap(word         id)
{ 
  tcontinuecampaign ncm; 


   ncm.init(); 
   ncm.setid(id); 
   ncm.run(); 
   ncm.done(); 

   cursor.hide(); 
   //computeview(); 
   displaymap(); 
   cursor.show(); 
} 


class tfillpolygon_connectionmarker : public tfillpolygonsquarecoord {
             pmap gamemap;
        public:
             virtual void initmark ( pmap _gamemap, int _mark ) { mark = _mark; gamemap = _gamemap; };
        protected:
             virtual void setpointabs ( int x,  int y  );
             int mark;
    };

void tfillpolygon_connectionmarker :: setpointabs ( int x,  int y  )
{
   gamemap->getField ( x, y )->connection |= mark;
}


void mark_polygon_fields_with_connection ( pmap gamemap, int* data, int mark )
{
   int polynum = data[0];
   data++;
   displayLogMessage ( 10, "mpfwc1 ");
   for ( int i = 0; i < polynum; i++ ) {
      tpolygon poly;
      poly.vertexnum = *data;
      data++;

      displayLogMessage ( 10, "mpfwc2 ");
      poly.vertex = new tpunkt[ poly.vertexnum ];
      for ( int j = 0; j < poly.vertexnum; j++ ) {
         poly.vertex[j].x = data[0];
         poly.vertex[j].y = data[1];
         data+=2;
      }
      displayLogMessage ( 10, "mpfwc3 ");
      tfillpolygon_connectionmarker fpcm;
      displayLogMessage ( 10, "mpfwc4 ");
      fpcm.initmark ( gamemap, mark );
      displayLogMessage ( 10, "mpfwc5 ");
      fpcm.paint_polygon ( &poly );
      displayLogMessage ( 10, "mpfwc6 ");
      delete[] poly.vertex;
   }
   displayLogMessage ( 10, "mpfwc end ");
}



class tcheckpolygon_anyunitentered : public tfillpolygonsquarecoord {
        public:
             virtual void init ( int _color ) { color = _color; found = 0;};
             int getresult ( void ) { return found; };
        protected:
             virtual void setpointabs ( int x,  int y  );
             int color;
             int found;
    };

void tcheckpolygon_anyunitentered :: setpointabs ( int x,  int y  )
{
   pfield fld = getfield ( x, y );
   if ( fld->vehicle ) {
      if ( (1 << ( fld->vehicle->color / 8)) & color )
         found++;
   }
}


class tcheckpolygon_specificunitentered : public tfillpolygonsquarecoord {
        public:
             virtual void init ( pvehicle _eht ) { eht = _eht; found = 0;};
             int getresult ( void ) { return found; };
        protected:
             virtual void setpointabs ( int x,  int y  )
             {
                pfield fld = getfield ( x, y );
                if ( fld->vehicle == eht ) 
                   found++;
                
             };
             pvehicle eht;
             int found;
    };



int unit_in_polygon ( tevent::LargeTriggerData::PolygonEntered* trigger )
{
   int found = 0;
   int* data = trigger->data;
   int polynum = *data;
   data++;
   for ( int i = 0; i < polynum; i++ ) {
      tpolygon poly;
      poly.vertexnum = *data;
      data++;

      poly.vertex = new tpunkt[ poly.vertexnum ];
      for ( int j = 0; j < poly.vertexnum; j++ ) {
         poly.vertex[j].x = data[0];
         poly.vertex[j].y = data[1];
         data+=2;
      }

      if ( trigger->vehiclenetworkid ) {
         tcheckpolygon_specificunitentered cpsue;
         cpsue.init ( actmap->getUnit ( trigger->vehiclenetworkid) );
         cpsue.paint_polygon ( &poly );
         found+= cpsue.getresult();
      } else {
         tcheckpolygon_anyunitentered cpaue;
         cpaue.init ( trigger->color );
         cpaue.paint_polygon ( &poly );
         found+= cpaue.getresult();
      }

      delete[] poly.vertex;
   }
   return found;
}
