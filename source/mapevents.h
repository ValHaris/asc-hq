/***************************************************************************
                          mapevents.h  -  description
                             -------------------
    begin                : Tue Mar 13 2001
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


// this is work in progress and it will take some time until this code compiles or even works
// I'm working on this with quite low priority



class MapEventTrigger {
     int id;
   protected:
     Map* gamemap;
     MapEventTrigger ( int _id, Map* _gamemap ) : gamemap ( _gamemap ) { id = _id; };

   public:
     virtual void read  ( tnstream& stream ) = 0;
     virtual void write ( tnstream& stream ) = 0;
     int getID ( ) { return id; };

     enum TriggerIDs { trig_Time };


     enum Status { unFulfilled, Fulfilled, ultimatelyUnFulfilled, ultimatelyFulfilled };
     bool inverted;

     Status getStatus();

   protected:
     //! this function is called by #getStatus . It returns the Status of the trigger, without checking for inverted (which is done by #getStatus )
     virtual Status getDirectStatus();
};


class MapEventAction {
   public:
     virtual void read  ( tnstream& stream ) = 0;
     virtual void write ( tnstream& stream ) = 0;
     virtual int getID ( ) = 0;

}

class MapEvent {
      Map* gamemap;
   protected:
      void readDerived  ( tnstream& stream ) = 0;
      void writeDerived ( tnstream& stream ) = 0;

   public:
      MapEvent ( Map* _gamemap ) : gamemap ( _gamemap ) {};

      //! bitmapped: players who can run this event
      int player;

      //! some information, just for the person that creates or edits the map
      ASCString description;

      typedef PointerList<MapEventTrigger*> TriggerContainer;
      TriggerContainer trigger;

      int getID ( );
      void read  ( tnstream& stream ) ;
      void write ( tnstream& stream ) ;

      struct {
        int turn;
        int move;
      } delayedexecution;

      //! the time when this event was trigger. If the event wasn't triggered yet, it will be set to -1 .
      tgametime     triggered;

}



class


    class  LargeTriggerData {
      public:

       class  PolygonEntered {
         public:
           int size;
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

        tgametime time;
        int xpos, ypos;
        int networkid;
        pbuilding    building;
        int         dummy;
        int          mapid;
        int          id;
        tevent::LargeTriggerData::PolygonEntered* unitpolygon;
        int reserved[32];
        LargeTriggerData ( void );
        LargeTriggerData ( const LargeTriggerData& data );
        ~LargeTriggerData();
    };

    LargeTriggerData* trigger_data[4];

    pascal_byte         triggerconnect[4];   /*  CEventTriggerConn */
    pascal_byte         triggerstatus[4];   /*  Nur im Spiel: 0: noch nicht erf?llt
                                         1: erf?llt, kann sich aber noch „ndern
                                         2: unwiederruflich erf?llt
                                         3: unerf?llbar */


    /* Funktionsweise der verzoegerten Events:
       Sobald die Trigger erf?llt sind, wird triggertime[0] ausgef?llt. Dadurch wird das event ausgeloest,
       sobald das Spiel diese Zeit erreicht ist, unabh„ngig vom Zustand des mapes
       ( Trigger werden nicht erneut ausgewertet !)
    */
    tevent ( void );
    tevent ( const tevent& event );
    ~tevent ( void );
};


class Trigger_Time : public MapEventTrigger {
   public:
      tgametime time;

      Trigger_Time ( Map* _gamemap ) : MapEventTrigger ( trig_Time, _gamemap ) {};
      Status getDirectStatus( );

   protected:
      void writeDerived ( tnstream& stream );
      void readDerived  ( tnstream& stream );
};


class Trigger_BuildingConquered : public MapEventTrigger {
   public:
      tgametime time;

      Trigger_Time ( Map* _gamemap ) : MapEventTrigger ( trig_Time, _gamemap ) {};
      Status getDirectStatus( );

   protected:
      void writeDerived ( tnstream& stream );
      void readDerived  ( tnstream& stream );
};



const char* ceventtrigger[ceventtriggernum]  = { "building conquered", "building lost",
                                                 "building destroyed", "unit lost", "technology researched",
                                                 "event", "unit conquered", "unit destroyed", "all enemy units destroyed",
                                                 "all units lost", "all enemy buildings destroyed/captured", "all buildings lost",
                                                 "energy tribute <", "material tribute <", "fuel tribute <",
                                                 "any unit enters polygon", "specific unit enters polygon", "building is seen", "irrelevant (used internally)"};

const char* ceventactions[ceventactionnum]  = {"message", "weather change", "new technology discovered", "lose campaign", "run script + next map",
                                               "new technology researchable", "map change", "discarded [ was erase event ]", "end campaign", "next map",
                                               "reinforcement","weather change completed", "new vehicle developed","palette change",
                                               "alliance change","wind change", "nothing", "change game parameter","paint ellipse","remove ellipse",
                                               "change building damage"};
