//     $Id: loaders.cpp,v 1.41 2001-02-01 22:48:43 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.40  2001/01/31 14:52:39  mbickel
//      Fixed crashes in BI3 map importing routines
//      Rewrote memory consistency checking
//      Fileselect dialog now uses ASCStrings
//
//     Revision 1.39  2001/01/23 21:05:18  mbickel
//      Speed up of AI
//      Lot of bugfixes in AI
//      Moved Research to own files (research.*)
//      Rewrote storing of developed technologies
//      Some cleanup and documentation
//
//     Revision 1.38  2001/01/22 20:00:09  mbickel
//      Fixed bug that made savegamefrom campaign games unloadable
//      Optimized the terrainAccess-checking
//
//     Revision 1.37  2001/01/04 15:13:59  mbickel
//      configure now checks for libSDL_image
//      AI only conquers building that cannot be conquered back immediately
//      tfindfile now returns strings instead of char*
//
//     Revision 1.36  2000/11/29 11:05:29  mbickel
//      Improved userinterface of the mapeditor
//      map::preferredfilenames uses now strings (instead of char*)
//
//     Revision 1.35  2000/11/26 22:18:53  mbickel
//      Added command line parameters for setting the verbosity
//      Increased verbose output
//
//     Revision 1.34  2000/11/11 11:05:18  mbickel
//      started AI service functions
//
//     Revision 1.33  2000/11/08 19:31:09  mbickel
//      Rewrote IO for the tmap structure
//      Fixed crash when entering damaged building
//      Fixed crash in AI
//      Removed item CRCs
//
//     Revision 1.32  2000/10/26 18:14:57  mbickel
//      AI moves damaged units to repair
//      tmap is not memory layout sensitive any more
//
//     Revision 1.31  2000/10/18 14:14:14  mbickel
//      Rewrote Event handling; DOS and WIN32 may be currently broken, will be
//       fixed soon.
//
//     Revision 1.30  2000/10/14 13:07:00  mbickel
//      Moved DOS version into own subdirectories
//      Win32 version with Watcom compiles and links ! But doesn't run yet...
//
//     Revision 1.29  2000/10/14 10:52:51  mbickel
//      Some adjustments for a Win32 port
//
//     Revision 1.28  2000/10/11 14:26:42  mbickel
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
//     Revision 1.27  2000/09/07 15:49:43  mbickel
//      some cleanup and documentation
//
//     Revision 1.26  2000/09/02 15:36:49  mbickel
//      Some minor cleanup and documentation
//
//     Revision 1.25  2000/08/28 15:58:58  mbickel
//      Fixed short displaying of map before password dialog when loading email
//        games by command line parameter
//      Fixed reaction fire not working when loading an old ( < ASC1.2.0 ) file
//
//     Revision 1.24  2000/08/21 17:50:59  mbickel
//      Fixed: crash when unit reaching max experience
//      Fixed: crash when displaying research image
//      Fixed: crash when events referenced a unit that has been shot down
//      Fixed: screenshot being written to wrong directory
//
//     Revision 1.23  2000/08/12 12:52:48  mbickel
//      Made DOS-Version compile and run again.
//
//     Revision 1.22  2000/08/11 12:24:03  mbickel
//      Fixed: no movement after refuelling unit
//      Restructured reading/writing of units
//
//     Revision 1.21  2000/08/07 16:29:21  mbickel
//      orbiting units don't consume fuel any more
//      Fixed bug in attack formula; improved attack formula
//      Rewrote reactionfire
//
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
//      Added CVS keywords to most of the files.
//      Started porting the code to Linux (ifdef'ing the DOS specific stuff)
//      Wrote replacement routines for kbhit/getch for Linux
//      Cleaned up parts of the code that gcc barfed on (char vs unsigned char)
//      Added autoconf/automake capabilities
//      Added files used by 'automake --gnu'
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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>

#include "typen.h"
#include "misc.h"
#include "basegfx.h"
#include "newfont.h"
#include "buildingtype.h"
#include "vehicletype.h"
#include "spfst.h"
#include "loaders.h"
#include "dlg_box.h"
#include "dlg_box.h"
#include "dialog.h"
#include "sgstream.h"
#include "sg.h"
#include "attack.h"
#include "errors.h"

#ifdef sgmain
#include "missions.h"
#endif


const word fileterminator = 0xa01a;
ticons icons;


 const char* savegameextension = "*.sav";
 const char* mapextension = "*.map";
 const char* tournamentextension = "*.asc";



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
                     vehicle = actmap->getUnit ( xpos, ypos, event->trigger_data[j]->networkid );
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
                  event->trigger_data[j]->unitpolygon->vehicle = actmap->getUnit ( event->trigger_data[j]->unitpolygon->tempxpos, event->trigger_data[j]->unitpolygon->tempypos, event->trigger_data[j]->unitpolygon->tempnwid );
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
   for ( int i = 0; i < 8; i++ )
      actmap->player[i].queuedEvents = 1;

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
                stream->writedata2( event->trigger_data[j]->building->getEntry().x );
                stream->writedata2( event->trigger_data[j]->building->getEntry().y );
             } else {
                integer w = -1;
                stream->writedata2( w );
                stream->writedata2( w );
             }
       } 
       if ((event->trigger[j] == ceventt_unitconquered) || 
           (event->trigger[j] == ceventt_unitlost) || 
           (event->trigger[j] == ceventt_unitdestroyed)) { 
           int xp;
           int yp;
           int nwid;
           if ( event->triggerstatus[j] == 2 ) {
              xp = -1;
              yp = -1;
              nwid = -1;
           } else {
              xp = event->trigger_data[j]->vehicle->xpos;
              yp = event->trigger_data[j]->vehicle->ypos;
              nwid = event->trigger_data[j]->vehicle->networkid;
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
               throw InvalidID ( "vehicle", j );

            stream->readdata2 ( j );
            du->tech = gettechnology_forid  ( j, 0 );
            if ( !du->tech )
               throw InvalidID ( "technology", j );

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
    int      j = 0;
    pevent   event = spfld->firsteventtocome;
    while ( event ) {
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
   if ( spfld->loadOldEvents ) {
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
       if ( !spfld )
          displaymessage ( "tspfldloaders::writemap  ; no map to write ! ",2);

       spfld->write ( *stream );
}


void     tmaploaders::initmap ( void )
{
    spfld->oldevents = NULL;
    spfld->firsteventtocome = NULL; 
    spfld->firsteventpassed = NULL; 
    spfld->network          = NULL;
    spfld->game_parameter = NULL;
}


void     tgameloaders::initmap ( void )
{
    spfld->game_parameter = NULL;
}


void     tspfldloaders::readmap ( void )
{
    spfld = new tmap;

    spfld->read ( *stream );
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
         fld->vehicle->write ( *stream );


      if (b1 & csm_building ) 
         fld->building->write ( *stream );

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

   spfld->field = new tfield [ cnt1 ];
   if (spfld->field == NULL)
      displaymessage ( "Could not allocate memory for map ",2);

   for ( int i = 0; i < cnt1; i++ ) {
      spfld->field[i].building = NULL;
      spfld->field[i].picture = NULL;
   }

   int l = 0;
   do { 
      pfield fld2;
      tfield lfld;

      if (cnt2 == 0) { 

         fld2 = & spfld->field[l];

         fld2->a.temp = 0;
         fld2->a.temp2 = 0; 
         fld2->vehicle = NULL;
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
            throw InvalidID ( "terrain", k );

         fld2->typ = trn->weather[weather];
         if ( !fld2->typ ) 
            throw InvalidID ( "terrain", k );

         if (b1 & csm_direction )
            stream->readdata2 ( fld2->direction );
         else                                              
            fld2->direction = 0; 


         if (b1 & csm_vehicle ) {
             fld2->vehicle = Vehicle::newFromStream ( spfld, *stream );
             fld2->vehicle->setnewposition ( l%spfld->xsize, l/spfld->xsize );
         }

         if (b1 & csm_building ) {
            fld2->building = Building::newFromStream ( spfld, *stream );
            fld2->bdt |= cbbuildingentry;
         }

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
                  throw InvalidID ( "object", id );

               /*
               if ( spfld->objectcrc ) 
                  spfld->objectcrc->speedcrccheck->checkobj2 ( fld2->object->object[n]->typ );
               */
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
   int i = 0;
   for (int y = 0; y < actmap->ysize; y++)
      for (int x = 0; x < actmap->xsize; x++) {
          pfield fld = &actmap->field[i];
          fld->setparams();
          if ( fld->bdt & cbbuildingentry )
             fld->building->resetPicturePointers();
          i++;
      }
}



/**************************************************************/
/*     Set Player Existencies                               ÿ */
/**************************************************************/

void   tspfldloaders::setplayerexistencies ( void )
{
   for ( int sp = 7; sp >= 0; sp--)
      if ( actmap->player[sp].exist() )
         actmap->actplayer = sp;
}


tspfldloaders::tspfldloaders ( void )
{
   spfld = NULL;
}


tspfldloaders::~tspfldloaders ( void )
{
  delete spfld;
  spfld = NULL;
}












































int          tmaploaders::savemap( const char * name )
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
       stream->writepchar ( NULL );  // description is not used any more
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
   if ( oldmap ) {
      delete oldmap ;
      oldmap = NULL;
   }
}

int          tmaploaders::loadmap( const char *       name )
{ 
    oldmap = actmap;
    actmap = NULL;

    displayLogMessage ( 4, "loading map %s ... ", name );

    tnfilestream filestream ( name, 1);

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
 
    if ( version > actmapversion || version < minmapversion )
       throw tinvalidversion ( name, actmapversion, version );
   

   displayLogMessage ( 10, "map, ");
   readmap ();

   displayLogMessage ( 10, "eventsToCome, ");
   readeventstocome ();

   displayLogMessage ( 10, "fields, ");
   readfields ();


  /*****************************************************************************************************/
  /*   šberprfen,  Stream schlieáen                 ÿ                                                 */
  /*****************************************************************************************************/

   stream->readdata( &version, sizeof(version)); 
   if (version > actmapversion || version < minmapversion ) { 
      delete spfld;
      spfld = NULL;
      throw tinvalidversion ( name, actmapversion, version );
   } 

   displayLogMessage ( 10, "~oldmap, ");
   delete oldmap;
   oldmap = NULL;
   
   actmap = spfld;
   spfld = NULL;


   displayLogMessage ( 10, "chainItems, ");
   chainitems ();

   setplayerexistencies ();

   displayLogMessage ( 10, "setEventTriggers, ");
   seteventtriggers();

   actmap->time.a.turn = 1; 
   actmap->time.a.move = 0;
             
   calculateallobjects();
   actmap->levelfinished = false;

   displayLogMessage ( 4, "done\n");

   
   return 0;
} 



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */ 


/*
void         readhistory(pfilestream  stream);

void         writehistory(pfilestream  stream);
*/


















int          tsavegameloaders::savegame( const char* name )
{ 

   tnfilestream filestream ( name, 2 );

   stream = &filestream;

   spfld = actmap;

 

   stream->writepchar ( NULL ); // description is not used any more
   stream->writedata2 ( fileterminator );

   stream->writedata2( actsavegameversion );
   writemap ();

   writenetwork ( );

   for ( int i = 0; i < 8; i++ )
      spfld->player[i].research.write_techs ( *stream );

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





int          tsavegameloaders::loadgame( const char *       name )
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

   for ( int i = 0; i < 8; i++ )
      spfld->player[i].research.read_techs ( *stream );

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
      delete spfld ;
      spfld = NULL;
      throw tinvalidversion ( name, actsavegameversion, version );
   }

   delete actmap;
   actmap = spfld;
   spfld = NULL;
 
   chainitems ();

   seteventtriggers();

   calculateallobjects();

   actmap->levelfinished = false;

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



/*
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
*/


int          tnetworkloaders::savenwgame( pnstream strm )
{ 
   spfld = actmap;

   stream = strm;

   stream->writepchar ( NULL );  // description is not used any more
   stream->writedata2 ( fileterminator );
 
   stream->writedata2( actnetworkversion );

   writemap ();

   for ( int i = 0; i < 8; i++ )
      spfld->player[i].research.write_techs ( *stream );

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

   for ( int i = 0; i < 8; i++ )
      spfld->player[i].research.read_techs ( *stream );


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
   if (version > actnetworkversion || version < minnetworkversion )
      throw tinvalidversion ( name, actnetworkversion, version );


   delete actmap;
   actmap = spfld;
   spfld = NULL;

  chainitems ();

  seteventtriggers();

  calculateallobjects();

  actmap->levelfinished = false;

  #ifdef sgmain
   getnexteventtime();
  #endif

  return 0;
}














void  savemap( const char * name )
{

   #ifdef logging
   logtofile ( "loaders / savemap / started " );
   #endif

   try {
     tmaploaders gl;
     gl.savemap ( name );
   } /* endtry */

   catch ( tfileerror err) {
      displaymessage( "file error writing map to filename %s ", 1, err.filename );
   } /* endcatch */
   catch ( ASCexception err) {
      displaymessage( "error writing map ", 1 );
   } /* endcatch */

   #ifdef logging
   logtofile ( "loaders / savemap / finished " );
   #endif

}             

void  loadmap( const char *       name )
{
   #ifdef logging
   logtofile ( "loaders.cpp / loadmap / loadmap started ");
   #endif

   try {
     tmaploaders gl;
     gl.loadmap ( name );

     actmap->preferredFileNames.mapname[0] = name ;
   }
   catch ( InvalidID err ) {
      displaymessage( err.getMessage().c_str(), 1 );
      if ( !actmap || actmap->xsize <= 0)
         throw NoMapLoaded();
   } /* endcatch */
   catch ( tinvalidversion err ) {
      displaymessage( "File %s has invalid version.\nExpected version %d\nFound version %d\n", 1, err.filename, err.expected, err.found );
      if ( !actmap || actmap->xsize <= 0)
         throw NoMapLoaded();
   } /* endcatch */
   catch ( tfileerror err) {
      displaymessage( "error reading map filename %s ", 1, err.filename );
      if ( !actmap || actmap->xsize <= 0)
         throw NoMapLoaded();
   } /* endcatch */
   catch ( ASCexception ) {
      displaymessage( "error loading map", 1 );
      if ( !actmap || actmap->xsize <= 0)
         throw NoMapLoaded();
   } /* endcatch */

   #ifdef logging
   logtofile ( "loaders.cpp / loadmap / loadmap finished ");
   #endif
}


void  savegame( const char *       name )
{
   try {
      tsavegameloaders gl;
      gl.savegame ( name );
   }
   catch ( tfileerror err) {
      displaymessage( "error writing map to filename %s ", 1, err.filename );
   } /* endcatch */
   catch ( ASCexception err) {
      displaymessage( "error writing map ", 1 );
   } /* endcatch */
}

void  loadgame( const char *       name )
{
   #ifdef logging
   logtofile ( "loaders.cpp / loadgame / loadgame started ");
   #endif

   try {
      tsavegameloaders gl;
      gl.loadgame ( name );
   }
   catch ( InvalidID err ) {
      displaymessage( err.getMessage().c_str(), 1 );
      if ( !actmap || actmap->xsize <= 0)
         throw NoMapLoaded();
   } /* endcatch */
   catch ( tinvalidversion err ) {
      displaymessage( "File %s has invalid version.\nExpected version %d\nFound version %d\n", 1, err.filename, err.expected, err.found );
      if ( !actmap || actmap->xsize <= 0)
         throw NoMapLoaded();
   } /* endcatch */
   catch ( tfileerror err) {
      displaymessage( "error reading map filename %s ", 1, err.filename );
      if ( !actmap || actmap->xsize <= 0)
         throw NoMapLoaded();
   } /* endcatch */
   catch ( ASCexception ) {
      displaymessage( "error loading game", 1 );
      if ( !actmap || actmap->xsize <= 0)
         throw NoMapLoaded();
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
   catch ( ASCexception err) {
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
   catch ( InvalidID err ) {
      displaymessage( err.getMessage().c_str(), 1 );
      if ( actmap->xsize == 0)
         throw NoMapLoaded();
   } /* endcatch */
   catch ( tinvalidversion err ) {
      displaymessage( "Replay stream %s has invalid version.\nExpected version %d\nFound version %d\n", 1, err.filename, err.expected, err.found );
      if ( actmap->xsize == 0)
         throw NoMapLoaded();
   } /* endcatch */
   catch ( tfileerror err) {
      displaymessage( "error reading map filename %s ", 1, err.filename );
      if ( actmap->xsize == 0)
         throw NoMapLoaded();
   } /* endcatch */
   catch ( ASCexception ) {
      displaymessage( "error loading replay", 1 );
      if ( actmap->xsize == 0)
         throw NoMapLoaded();
   } /* endcatch */

   #ifdef logging
   logtofile ( "loaders.cpp / loadreplay / loadreplay finished ");
   #endif

}






void treplayloaders :: initmap ( void )
{
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
   if (version > actreplayversion || version < minreplayversion )
      throw tinvalidversion ( name, actreplayversion, version );

   delete actmap;
   actmap = spfld;
   spfld = NULL;


  chainitems ();

  seteventtriggers();

  calculateallobjects(); 

  actmap->levelfinished = false; 

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

   tmap* replayfield = new tmap;
   *replayfield = *actmap;

   replayfield->campaign = NULL;
   replayfield->title = NULL;
   for ( int i = 0; i < 8; i++ ) {
      replayfield->player[i].dissectedunit = NULL;
      replayfield->player[i].unreadmessage = NULL;
      replayfield->player[i].oldmessage = NULL;
      replayfield->player[i].sentmessage = NULL;
      replayfield->player[i].ai = NULL;
      replayfield->player[i].research.activetechnology = NULL;
      replayfield->player[i].research.____setDevTechToNULL ( );
      replayfield->player[i].humanname = "";
      replayfield->player[i].computername = "";
   }
   replayfield->oldevents = NULL;
   replayfield->firsteventtocome = NULL;
   replayfield->firsteventpassed = NULL;
   replayfield->network = NULL;
   replayfield->unsentmessage = NULL;
   replayfield->message = NULL;
   replayfield->journal = NULL;
   replayfield->newjournal = NULL;
   // replayfield->objectcrc = NULL;
   if ( actmap->shareview )
      replayfield->shareview = new tshareview ( actmap->shareview );

   replayfield->replayinfo = NULL;

   stream = &memstream;

   spfld = replayfield;

   stream->writedata2( actreplayversion );
   writemap ();
           
   writefields ( );

   stream->writedata2 ( actreplayversion );

   spfld = NULL;

}














bool validatemapfile ( const char* filename )
{

   char* description = NULL;

   try {

      tnfilestream stream ( filename, 1 );
      stream.readpchar ( &description, 200 );
      if ( description ) {
         delete[]  description ;
         description = NULL;
      }

      word w;
      stream.readdata2 ( w );
      if ( w != fileterminator )
         throw tinvalidversion ( filename, fileterminator, (int) w );

      int version;
      stream.readdata2( version );
   
      if (version > actmapversion || version < minmapversion ) 
         throw tinvalidversion ( filename, actmapversion, version );

   } /* endtry */

   catch ( ASCexception ) {
       return 0;
   } /* endcatch */

   return 1;
} 





bool validateemlfile ( const char* filename )
{

   char* description = NULL;

   try {

      tnfilestream stream ( filename, 1 );
      stream.readpchar ( &description, 200 );
      if ( description ) {
         delete[]  description ;
         description = NULL;
      }
      
      word w;
      stream.readdata2 ( w );
      if ( w != fileterminator )
         throw tinvalidversion ( filename, fileterminator, (int) w );

      int version;
      stream.readdata2( version );
   
      if (version > actnetworkversion || version < minnetworkversion ) 
         throw tinvalidversion ( filename, actnetworkversion, version );

   } /* endtry */

   catch ( ASCexception ) {
       return 0;
   } /* endcatch */


   return 1;
} 


bool validatesavfile ( const char* filename )
{

   char* description = NULL;

   try {

      tnfilestream stream ( filename, 1 );
      stream.readpchar ( &description, 200 );
      if ( description ) {
         delete[]  description ;
         description = NULL;
      }

      word w;
      stream.readdata2 ( w );
      if ( w != fileterminator )
         throw tinvalidversion ( filename, fileterminator, (int) w );

      int version;
      stream.readdata2( version );
   
      if (version > actsavegameversion || version < minsavegameversion ) 
         throw tinvalidversion ( filename, actsavegameversion, version );

   } /* endtry */

   catch ( ASCexception ) {
       return 0;
   } /* endcatch */

   return 1;
} 











void         savecampaignrecoveryinformation( const ASCString& filename,
                                             int id)
{ 
   displaymessage("This has not been implemented yet, sorry!", 2 );

} 



void         __erasemap( pmap& spfld )
{ 
   if ( !spfld )
      return;

  int         i; 
  // pvehicle     aktvehicle;
  // pbuilding    aktbuilding;


   if (spfld->xsize == 0) 
      return;


   for (i = 0; i <= 8; i++) { 
      if ( spfld->player[i].ai ) {
         delete spfld->player[i].ai;
         spfld->player[i].ai = NULL;
      }
   }

   delete spfld;
   spfld = NULL;

}



void         __erasemap_unchained( pmap& spfld )
{ 
   if ( !spfld )
      return;


  int         i; 

   if (spfld->xsize == 0) return;


   for (i=0; i<9 ; i++) {
      if ( spfld->player[i].ai  ) {
         delete spfld->player[i].ai;
         spfld->player[i].ai = NULL;
      }

   }


   delete spfld;
   spfld = NULL;
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
      string c = ff.getnextname();
                       
      while ( !c.empty() ) {
          if ( actprogressbar )
             actprogressbar->point();
    
          pvehicletype t = loadvehicletype( c.c_str() );

          // if ( t->steigung && ((t->height & 6 ) == 6 )) 
          //   t->steigung = 0;

          addvehicletype ( t );

          c = ff.getnextname();
       }
   }
} 


void         loadallobjecttypes (void)
{ 
  tfindfile ff ( "*.obl" );

  string c = ff.getnextname();

  while ( !c.empty() ) {
      if ( actprogressbar )
         actprogressbar->point();

      addobjecttype ( loadobjecttype( c.c_str() ));

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
  string c = ff.getnextname();

  while ( !c.empty() ) {
      if ( actprogressbar )
         actprogressbar->point();

      addtechnology ( loadtechnology( c.c_str() ));

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

   string c = ff.getnextname();

   while( !c.empty() ) {
      if ( actprogressbar )
         actprogressbar->point();

      addterraintype ( loadterraintype( c.c_str() ));

      c = ff.getnextname();
   } 
};




void         loadallbuildingtypes(void)
{ 
   tfindfile ff ( "*.bld" );

   string c = ff.getnextname();

   while( !c.empty() ) {
      if ( actprogressbar )
         actprogressbar->point();

      addbuildingtype ( loadbuildingtype( c.c_str() ));

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


/*
tspeedcrccheck :: tspeedcrccheck ( pobjectcontainercrcs crclist )
{
   status = 0;

   strng = NULL;
   strnglen = 0;

   int i;
*/
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
/*
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
   }

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
*/