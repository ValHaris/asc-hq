/*! \file gameevents.cpp
    \brief The event handling of ASC
*/

/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2003  Martin Bickel  and  Marc Schellenberger

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
#include "prehistoricevents.h"
#include "gameeventsystem.h"
#include "gameevents.h"
#include "dialog.h"

//        .l             .l                !              .l                !               !
 enum { cemessage,   ceweatherchange, cenewtechnology, celosecampaign, cerunscript,     cenewtechnologyresearchable,
//        .l             !                 !               .l                 !                 .l
        cemapchange, ceeraseevent,    cecampaignend,   cenextmap,      cereinforcement, ceweatherchangecomplete,
//         !                      !                  !                       .l            .l
        cenewvehicledeveloped, cepalettechange, cealliancechange,      cewindchange,    cenothing,
//        .l                   .l         .l               .l                       .l
        cegameparamchange, ceellipse, ceremoveellipse, cechangebuildingdamage, ceaddobject };


extern const char*  ceventtrigger[];
//         .l                                     .l                         .l                        .l                          .l
 enum { ceventt_turn = 1 ,               ceventt_buildingconquered, ceventt_buildinglost,  ceventt_buildingdestroyed, ceventt_unitlost,
//         !                                     .                          .l                       .l
        ceventt_technologyresearched,    ceventt_event,             ceventt_unitconquered, ceventt_unitdestroyed,
//         .                                     .l                          .
        ceventt_allenemyunitsdestroyed,  ceventt_allunitslost,      ceventt_allenemybuildingsdestroyed,
//                .l
        ceventt_allbuildingslost,        ceventt_energytribute,     ceventt_materialtribute, ceventt_fueltribute,
//                .l                               .l                                     .l
        ceventt_any_unit_enters_polygon, ceventt_specific_unit_enters_polygon, ceventt_building_seen, ceventt_irrelevant };


  /*  Datenaufbau des triggerData fieldes: [ hi 16 Bit ] [ low 16 Bit ] [ 32 bit Integer ] [ Pointer ]      [ low 24 Bit       ]  [ high 8 Bit ]
     'turn/move',                            move           turn
     'building/unit     ',Kartened/Spiel                                                   PBuilding/Pvehicle
                          disk               ypos           xpos
     'technology researched',                                             Tech. ID
     'event',                                                             Event ID
     'tribut required'                                                                                         Hoehe des Tributes      Spieler, von dem Tribut gefordert wird
     'all enemy *.*'                                                      Bit 0: alle nicht allierten
                                                                          Bit 1: alle, die ?ber die folgenden Bits festgelegt werden, ob alliiert oder nicht
                                                                            Bit 2 : Spieler 0
                                                                            ...
                                                                            Bit 9 : Spieler 7

     'unit enters polygon'  pointer auf teventtrigger_polygonentered

     der Rest benoetigt keine weiteren Angaben
    */



     /*  DatenAufbau des Event-Data-Blocks:

      TLosecampaign, TEndCampaign, TWeatherchangeCompleted
                 benoetigen keine weiteren Daten


      TNewTechnologyEvent :
                 data = NULL;
                 SaveAs = TechnologyID;
              Gilt fuer researched wie auch available

      TMessageEvent
             Data = NULL;
             SaveAs: MessageID des Abschnittes in TextDatei

      TNextMapEvent:
             Data = NULL;
             saveas: ID der n„chsten Karte;

      TRunScript+NextMapEvent:
             Data = pointer auf Dateinamen des Scriptes ( *.scr );
             saveas: ID der n„chsten Karte;

      TeraseEvent:
             data[0] = ^int
                       ID des zu loeschenden Events
             data[1] = mapid

      Tweatherchange            ( je ein int , alles unter Data )
              wetter            ( -> cwettertypen , Wind ist eigene eventaction )
              fieldadressierung      ( 1: gesamtes map     )
                      ³              ( 0: polygone               )
                      ³
                      ÃÄÄÄÄÄ 0 ÄÄÄ>  polygonanzahl
                      ³                   ÃÄÄ   eckenanzahl
                      ³                             ÃÄÄ x position
                      ³                                 y position
                      ³
                      ³
                      ÀÄÄÄÄÄ 1 ÄÄÄ|

+     Twindchange
              intensit„t[3]         ( fuer tieffliegend, normalfliegend und hochfliegend ; -1 steht fuer keine Aenderung )
              Richtung[3]           ( dito )


      Tmapchange               ( je ein int , alles unter Data )        { wetter wird beibehalten ! }
            numberoffields ( nicht die Anzahl fielder insgesamt,
               ÃÄÄ>  bodentypid
                     drehrichtung
                     fieldadressierung   ( wie bei tweatherchange )


      Treinforcements        ( alles unter DATA )
             int num      // ein int , der die Anzahl der vehicle angibt. Die vehicle, die ein Transporter geladen hat, werden NICHT mitgez„hlt.
                  ÃÄÄÄÄ > die vehicle, mit tspfldloaders::writeunit in einen memory-stream geschrieben.


      TnewVehicleDeveloped
            saveas  = ID des nun zur Verf?gung stehenden vehicletypes


      Tpalettechange
           Data =  Pointer auf String, der den Dateinamen der Palettendatei enth„lt.

      Talliancechange
           Data : Array[8][8] of int                      // status der Allianzen. Sollte vorerst symetrisch bleiben, also nur jeweils 7 Werte abfragen.
                                                             Vorerst einfach Zahlwerte eingeben.
                                                             256 steht fuer unver„ndert,
                                                             257 fuer umkehrung

+     TGameParameterchange
           int nummer_des_parameters ( -> gameparametername[] )
           int neuer_wert_des_parameters

      Ellipse
           int x1 , y1, x2, y2, x orientation , y orientation


    Wenn Data != NULL ist, MUss datasize die Groesse des Speicherbereichs, auf den Data zeigt, beinhalten.

 */



   class PrehistoricEventStructure {
     public:
       union {
         struct {  word         saveas; char action, num;  }a;  /*  CEventActions  */
         int      id;               /* Id-Nr      ==> Technology.Requireevent; Tevent.trigger; etc.  */
       } ;

       pascal_byte         player;   // 0..7  fuer die normalen Spieler
       // 8 wenn das Event unabh"ngig vom Spieler sofort auftreten soll

       char         description[20];

       union {
         void*    rawdata;
         char*    chardata;
         int*     intdata;
       };

       int          datasize;
       pevent       next;
       int          conn;   // wird nur im Spiel gebraucht, BIt 0 gibt an, das andere events abh"nging sind von diesem
       word         trigger[4];   /*  CEventReason  */

       class  LargeTriggerData {
         public:

          class  PolygonEntered {
            public:
              int dataSize;
              int dummy;
              int vehiclenetworkid;
              int* data;
              int tempnwid;
              int tempxpos;
              int tempypos;
              int color;                // bitmapped
              int reserved[7];
              PolygonEntered ( void );
              PolygonEntered ( const PolygonEntered& poly );
              ~PolygonEntered ( );
           };

           GameTime time;
           int xpos, ypos;
           int networkid;
           pbuilding    building;
           int         dummy;
           int          mapid;
           int          id;
           PrehistoricEventStructure::LargeTriggerData::PolygonEntered* unitpolygon;
           int reserved[32];
           LargeTriggerData ( void );
           LargeTriggerData ( const LargeTriggerData& data );
           ~LargeTriggerData();
       };

       LargeTriggerData* trigger_data[4];

       pascal_byte         triggerconnect[4];   /*  CEventTriggerConn */
       pascal_byte         triggerstatus[4];   /*  Nur im Spiel: 0: noch nicht erf?llt
                                            1: erf?llt, kann sich aber noch "ndern
                                            2: unwiederruflich erf?llt
                                            3: unerf?llbar */
       GameTime     triggertime;     // Im Karteneditor auf  -1 setzen !!
       // Werte ungleich -1 bedeuten automatisch, dass das event bereits erf?llt ist und evt. nur noch die Zeit abzuwait ist

       struct {
         int turn;
         int move;   // negative Zahlen SIND hier zul"ssig !!!
       } delayedexecution;

       /* Funktionsweise der verzoegerten Events:
          Sobald die Trigger erf?llt sind, wird triggertime[0] ausgef?llt. Dadurch wird das event ausgeloest,
          sobald das Spiel diese Zeit erreicht ist, unabh"ngig vom Zustand des mapes
          ( Trigger werden nicht erneut ausgewertet !)
       */
       PrehistoricEventStructure();
       ~PrehistoricEventStructure();

       /*EventAction* eventAction;
       int eventActionType;*/

   };

extern  const char* cconnections[6];
 #define cconnection_destroy 1
 #define cconnection_conquer 2
 #define cconnection_lose 4
 #define cconnection_seen 8
 #define cconnection_areaentered_anyunit 16
 #define cconnection_areaentered_specificunit 32
 //   conquered = You conquered sth.
 //   lost      = an enemy conquered sth. from you


const int ceventtriggernum = 21;
extern const char* ceventtriggerconn[];
 #define ceventtrigger_and 1
 #define ceventtrigger_or 2
 #define ceventtrigger_not 4
 #define ceventtrigger_klammerauf 8
 #define ceventtrigger_2klammerauf 16
 #define ceventtrigger_2klammerzu 32
 #define ceventtrigger_klammerzu 64
  /*  reihenfolgenpriorit„t: in der Reihenfolge von oben nach unten wird der TriggerCon ausgewertet
               AND   OR
               NOT
               (
               eigentliches event
               )
    */


#define ceventactionnum 22
extern const char* ceventactions[ceventactionnum]; // not bitmapped




//! The different connections of something, be it unit, building, or field, to an event.
const char* cconnections[6]  = {"destroyed", "conquered", "lost", "seen", "area entererd by any unit", "area entered by specific unit"};

const char* ceventtriggerconn[8]  = {"AND ", "OR ", "NOT ", "( ", "(( ", ")) ", ") ", "Clear "};

//! All actions that can be performed by events
const char* ceventactions[ceventactionnum]  = {"message", "weather change", "new technology discovered", "lose campaign", "run script + next map",
                                               "new technology researchable", "map change", "discarded [ was erase event ]", "end campaign", "next map",
                                               "reinforcement","weather change completed", "new vehicle developed","palette change",
                                               "alliance change","wind change", "nothing", "change game parameter","paint ellipse","remove ellipse",
                                               "change building damage", "add object"};

const char* ceventtrigger[ceventtriggernum]  = {"*NONE*", "turn/move >=", "building conquered", "building lost",
                                                 "building destroyed", "unit lost", "technology researched",
                                                 "event", "unit conquered", "unit destroyed", "all enemy units destroyed",
                                                 "all units lost", "all enemy buildings destroyed/captured", "all buildings lost",
                                                 "energy tribute <", "material tribute <", "fuel tribute <",
                                                 "any unit enters polygon", "specific unit enters polygon", "building is seen", "irrelevant (used internally)"};



void  readOldEventLists ( pnstream stream, bool passedEvents, pmap spfld )
{
   #if SDL_BYTEORDER == SDL_LIL_ENDIAN

   map<int,int> eventTranslation;
   map<EventTriggered*,int> eventTriggerEvents;

   int j = stream->readInt();
   if ( j )
      for (int k = 1; k <= j; k++) {
         Event* ev = readOldEvent ( stream, spfld, eventTranslation, eventTriggerEvents );
         if ( ev )
            spfld->events.push_back ( ev );
      }

   if ( passedEvents ) {
      int j = stream->readInt();
      if ( j )
         for (int k = 1; k <= j; k++) {
            Event* ev = readOldEvent ( stream, spfld, eventTranslation, eventTriggerEvents );
            ev->status = Event::Executed;
            if ( ev )
               spfld->events.push_back ( ev );
         }
   }

   for ( map<EventTriggered*,int>::iterator i = eventTriggerEvents.begin(); i != eventTriggerEvents.end(); ++i ) {
       map<int,int>::iterator e = eventTranslation.find ( i->second );
       if ( e == eventTranslation.end() ) {
          displaymessage ("Could not find oldEvent-ID for setting the event Trigger",1);
          return;
       }

       i->first->setEventID ( e->second );
   }
   #else
   displaymessage ("Unable to load map in old file format on a big endian machine\nPlease convert this file to the new file format on a little endian machine and try again",1);
   throw tfileerror();
   #endif
}


PrehistoricEventStructure::LargeTriggerData::PolygonEntered :: PolygonEntered ( void )
{
   dataSize = 0;
   vehiclenetworkid = 0;
   data = NULL;
   tempnwid = 0;
   tempxpos = 0;
   tempypos = 0;
   color = 0;
   memset ( reserved, 0, sizeof ( reserved ));
}

PrehistoricEventStructure::LargeTriggerData::PolygonEntered :: PolygonEntered ( const PrehistoricEventStructure::LargeTriggerData::PolygonEntered& poly )
{
   dataSize = poly.dataSize;
   vehiclenetworkid = poly.vehiclenetworkid;
   if ( poly.data ) {
      data = new int [dataSize];
      for ( int i = 0; i < dataSize; i++ )
         data[i] = poly.data[i];
   } else
      data = NULL;
   tempnwid = poly.tempnwid;
   tempxpos = poly.tempxpos;
   tempypos = poly.tempypos;
   color = poly.color;
   memcpy ( reserved, poly.reserved, sizeof ( reserved ));
}

PrehistoricEventStructure::LargeTriggerData::PolygonEntered :: ~PolygonEntered ( )
{
   if ( data ) {
      delete[] data;
      data = NULL;
   }
}


PrehistoricEventStructure::LargeTriggerData :: LargeTriggerData ( void )
{
   time.abstime = 0;
   xpos = -1;
   ypos = -1;
   building = NULL;
   mapid = 0;
   id = -1;
   unitpolygon = NULL;
   networkid = 0;
   memset ( reserved, 0, sizeof ( reserved ));
}

PrehistoricEventStructure::LargeTriggerData :: LargeTriggerData ( const LargeTriggerData& data )
{
   time = data.time;
   xpos = data.xpos;
   ypos = data.ypos;
   networkid = data.networkid;
   building = data.building;
   mapid = data.mapid;
   id = data.id;
   if ( data.unitpolygon ) {
      unitpolygon = new PrehistoricEventStructure::LargeTriggerData::PolygonEntered ( *data.unitpolygon );
   } else
      unitpolygon = NULL;
}

PrehistoricEventStructure::LargeTriggerData :: ~LargeTriggerData ( )
{
   if ( unitpolygon ) {
      delete unitpolygon;
      unitpolygon = NULL;
   }
}

PrehistoricEventStructure :: PrehistoricEventStructure()
{
   a.action= 255;
   a.saveas = 0;
   a.num = 0;
   player = 0;
   rawdata = NULL;
   next = NULL;
   datasize = 0;
   conn = 0;
   for ( int i = 0; i < 4; i++) {
      trigger[i] = 0;
      triggerstatus[i] = 0;
      triggerconnect[i] = 0;
      triggertime.set( -1, -1 );
      trigger_data[i] = new LargeTriggerData;
   }
   delayedexecution.turn = 0;
   delayedexecution.move = 0;
   description[0] = 0;
   triggertime.abstime = -1;
}



PrehistoricEventStructure :: ~PrehistoricEventStructure ()
{
   for ( int i = 0; i < 4; i++ )
      if ( trigger_data[i] ) {
        delete trigger_data[i];
        trigger_data[i] = NULL;
      }

   if ( intdata ) {
      delete[]  intdata ;
      intdata = NULL;
   }
}






Event*   readOldEvent( pnstream stream, pmap map, map<int,int>& eventTranslation, map<EventTriggered*,int>& eventTriggerEvents )
{

/////////////////////////////////////////////////////
// this code works only on little-endian system,
// but there is no need to change this, because it is
// only used for converting old maps and savegames
/////////////////////////////////////////////////////

     Event* ev = new Event( *map );

     PrehistoricEventStructure event1;

     int magic = stream->readInt();
     int version;
     if ( magic == -1 ) {
        version = stream->readInt();
        stream->readdata ( &event1, 85 );
     } else {
        memcpy ( &event1, &magic, sizeof ( magic ));
        int* pi = (int*) &event1;
        pi++;
        stream->readdata ( pi, 85 - sizeof ( int ));
        version = 0;
     }

     eventTranslation[event1.id] = ev->id;

     if ( version > 2 )
        throw tinvalidversion ( "OldEvent", 2, version );

     if ( event1.datasize && event1.rawdata ) {
        event1.rawdata = asc_malloc ( event1.datasize );
        stream->readdata ( event1.rawdata, event1.datasize );
     }  else {
        event1.datasize = 0;
        event1.rawdata = NULL;
     }

     EventAction* ea = NULL;

     switch ( event1.a.action ) {
       case cewindchange:           ea = new WindChange ( event1.intdata[0], event1.intdata[3] );
                                    break;
       case cegameparamchange:      ea = new ChangeGameParameter( event1.intdata[0], event1.intdata[1] );
                                    break;
       case cemessage:              ea = new DisplayMessage ( event1.a.saveas );
                                    break;
       case cenothing:              ea = new Action_Nothing();
                                    break;
       case cechangebuildingdamage: ea = new ChangeBuildingDamage( MapCoordinate( event1.intdata[0], event1.intdata[1]), event1.intdata[2] );
                                    break;
       case ceweatherchangecomplete:ea = new MapChangeCompleted();
                                    break;
       case cenextmap:              ea = new NextMap( event1.a.saveas );
                                    break;
       case celosecampaign:         ea = new LoseMap();
                                    break;
       case ceaddobject:            {
                                       AddObject* ao = new AddObject( event1.intdata[2] );
                                       ao->setField ( MapCoordinate( event1.intdata[0], event1.intdata[1] ));
                                       ea = ao;
                                    }
                                    break;
       case ceellipse:              ea = new DisplayEllipse( event1.intdata[0], event1.intdata[1], event1.intdata[2], event1.intdata[3], event1.intdata[4], event1.intdata[5] );
                                    break;
       case ceremoveellipse:        ea = new RemoveEllipse();
                                    break;
       case ceweatherchange:        {
                                       WeatherChange* wc = new WeatherChange( event1.intdata[0] );
                                       if ( event1.intdata[1] == 0 ) {
                                          Poly_gon poly;
                                          if ( event1.intdata[2] > 1 )
                                              displaymessage("Only the first polygon will be used!", 1 );

                                          for ( int i = 0; i < event1.intdata[3]; ++i )
                                             poly.vertex.push_back ( MapCoordinate( event1.intdata[4+2*i], event1.intdata[5+2*i] ));

                                          wc->setPolygon ( poly );
                                       } else
                                          wc->setGlobal();
                                       ea = wc;
                                    }
                                    break;
       case cemapchange:            {
                                       MapChange* mc = new MapChange( event1.intdata[0] );
                                       Poly_gon poly;
                                       for ( int i = 0; i < event1.intdata[4]; ++i )
                                          poly.vertex.push_back ( MapCoordinate( event1.intdata[5+2*i], event1.intdata[6+2*i] ));

                                       mc->setPolygon ( poly );
                                       ea = mc;
                                    }
                                    break;

       default:                     displaymessage("The event action %s has not been converted and will be missing!", 1, ceventactions[event1.a.action] );
     };

     if ( ea ) {
        ev->action = ea;
        ea->setMap ( map );
     }

     int ANDcounter = 0;
     int ORcounter = 0;
     bool missingConnection = false;

     for (int m = 0; m <= 3; m++) {

        EventTrigger* et = 0;

        event1.trigger_data[m] = new PrehistoricEventStructure::LargeTriggerData;
        if ( event1.trigger[m] ) {
           if ((event1.trigger[m] == ceventt_buildingconquered) ||
              (event1.trigger[m] == ceventt_buildinglost) ||
              (event1.trigger[m] == ceventt_buildingdestroyed) ||
              (event1.trigger[m] == ceventt_building_seen )) {

              integer xpos, ypos;
              stream->readdata2 ( xpos );
              stream->readdata2 ( ypos );

              event1.trigger_data[m]->xpos = xpos;
              event1.trigger_data[m]->ypos = ypos;

              switch ( event1.trigger[m] ) {
               case ceventt_buildingconquered: et = new  BuildingConquered ( MapCoordinate( xpos, ypos ));
                                               break;
               case ceventt_buildinglost:      et = new  BuildingLost ( MapCoordinate( xpos, ypos ));
                                               break;
               case ceventt_buildingdestroyed: et = new  BuildingDestroyed ( MapCoordinate( xpos, ypos ));
                                               break;
               case ceventt_building_seen:     et = new  BuildingSeen ( MapCoordinate( xpos, ypos ));
                                               break;
              };

           }

           if ((event1.trigger[m] == ceventt_unitconquered) ||
              (event1.trigger[m] == ceventt_unitlost) ||
              (event1.trigger[m] == ceventt_unitdestroyed)) {

              if ( version == 0 ) {
                 integer xpos, ypos;
                 stream->readdata2 ( xpos );
                 stream->readdata2 ( ypos );
                 event1.trigger_data[m]->xpos = xpos;
                 event1.trigger_data[m]->ypos = ypos;
                 event1.trigger_data[m]->networkid = -1;

                 displaymessage("The event trigger %s has not been converted and will be missing!", 1, ceventtrigger[event1.trigger[m]] );

              } else {
                 stream->readdata2( event1.trigger_data[m]->xpos );
                 stream->readdata2( event1.trigger_data[m]->ypos );
                 stream->readdata2( event1.trigger_data[m]->networkid );

                 switch ( event1.trigger[m] ) {
                   case ceventt_unitconquered: et = new  UnitConquered ( event1.trigger_data[m]->networkid );
                                               break;
                   case ceventt_unitlost:      et = new  UnitLost ( event1.trigger_data[m]->networkid );
                                               break;
                   case ceventt_unitdestroyed: et = new  UnitDestroyed ( event1.trigger_data[m]->networkid );
                                               break;
                 };
              }
           }

           if ( event1.trigger[m] == ceventt_event )  {
               stream->readdata2 ( event1.trigger_data[m]->id );
               EventTriggered* ett = new EventTriggered();
               et = ett;
               eventTriggerEvents[ett] = event1.trigger_data[m]->id;
           }
           
           if ( event1.trigger[m] == ceventt_technologyresearched ) {
               stream->readdata2 ( event1.trigger_data[m]->id );
           }

           if ( version >=2 ) {
              if ( event1.trigger[m] == ceventt_allenemybuildingsdestroyed ) {
                 stream->readdata2 ( event1.trigger_data[m]->id );
                 et = new AllEnemyBuildingsDestroyed;
              }

              if (event1.trigger[m] == ceventt_allenemyunitsdestroyed  ) {
                 stream->readdata2 ( event1.trigger_data[m]->id );
                 et = new AllEnemyUnitsDestroyed;
              }
           }

           if (event1.trigger[m] == ceventt_turn) {
               stream->readdata2 ( event1.trigger_data[m]->time.abstime );
               et = new TurnPassed ( event1.trigger_data[m]->time.turn(), event1.trigger_data[m]->time.move() );
           }
           if (event1.trigger[m] == ceventt_any_unit_enters_polygon ||
               event1.trigger[m] == ceventt_specific_unit_enters_polygon) {
                  int i = stream->readInt();
                  event1.trigger_data[m]->unitpolygon = new PrehistoricEventStructure::LargeTriggerData::PolygonEntered;
                  stream->readdata2( *event1.trigger_data[m]->unitpolygon );
                  event1.trigger_data[m]->unitpolygon->data = new int [ event1.trigger_data[m]->unitpolygon->dataSize ];
                  stream->readdata( event1.trigger_data[m]->unitpolygon->data, event1.trigger_data[m]->unitpolygon->dataSize * sizeof(int) );
                  if ( event1.trigger_data[m]->unitpolygon->vehiclenetworkid  ) { // || event1.trigger_data[m]->unitpolygon->dummy
                     stream->readdata2( event1.trigger_data[m]->unitpolygon->tempxpos );
                     stream->readdata2( event1.trigger_data[m]->unitpolygon->tempypos );
                     stream->readdata2( event1.trigger_data[m]->unitpolygon->tempnwid );
                     event1.trigger_data[m]->unitpolygon->vehiclenetworkid = event1.trigger_data[m]->unitpolygon->tempnwid;
                  }

                  Poly_gon pg;
                  for( int i = 0; i < event1.trigger_data[m]->unitpolygon->data[1]; ++i )
                     pg.vertex.push_back ( MapCoordinate( event1.trigger_data[m]->unitpolygon->data[2+2*i], event1.trigger_data[m]->unitpolygon->data[3+2*i] ));

                  if ( event1.trigger[m] == ceventt_any_unit_enters_polygon ) {
                     AnyUnitEntersPolygon* auep = new AnyUnitEntersPolygon ( event1.trigger_data[m]->unitpolygon->color );
                     auep->setPolygon ( pg );
                     et = auep;
                  } else {
                     SpecificUnitEntersPolygon* suep = new SpecificUnitEntersPolygon ( event1.trigger_data[m]->unitpolygon->vehiclenetworkid );
                     suep->setPolygon ( pg );
                     et = suep;
                  }
           }

           if (event1.trigger[m] == ceventt_allbuildingslost )
              et = new AllBuildingsLost();

           if (event1.trigger[m] == ceventt_allunitslost )
              et = new AllUnitsLost();

           if ( event1.triggerconnect[m] & ceventtrigger_not )
              if ( et  )
                 et->invert = true;

           if ( event1.triggerconnect[m] & ceventtrigger_and )
              ++ANDcounter;

           if ( event1.triggerconnect[m] & ceventtrigger_or )
              ++ORcounter;

           if ( event1.triggerconnect[m] & ( ceventtrigger_klammerauf | ceventtrigger_2klammerauf | ceventtrigger_klammerzu | ceventtrigger_2klammerzu ))
              missingConnection = true;

           if ( et ) {
              ev->trigger.push_back ( et );
              et->setMap ( map );
              et->setEvent ( ev );
           } else
              displaymessage("The trigger %d (%s) for event %s could not be converted !", 1,m,ceventtrigger[event1.trigger[m]],event1.description );

        }

     }

     if ( ANDcounter & ORcounter )
        missingConnection = true;

     if ( missingConnection )
        displaymessage("The connection of the triggers for event %s could not be completely converted !", 1,event1.description );



     if ( ev && ev->action ) {
        if ( ANDcounter > ORcounter )
           ev->triggerConnection = Event::AND;
        else
           ev->triggerConnection = Event::OR;

        ev->description = event1.description;

        return ev;
     } else {
        delete ev;
        return NULL;
     }
}


