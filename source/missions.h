/*! \file missions.h
    \brief Interface to the event handling of ASC
*/

//     $Id: missions.h,v 1.11 2003-06-26 21:00:18 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.10  2003/05/01 18:02:22  mbickel
//      Fixed: no movement decrease for cargo when transport moved
//      Fixed: reactionfire not working when descending into range
//      Fixed: objects not sorted
//      New map event: add object
//
//     Revision 1.9  2003/01/28 17:48:42  mbickel
//      Added sounds
//      Rewrote soundsystem
//      Fixed: tank got stuck when moving from one transport ship to another
//
//     Revision 1.8  2001/02/26 12:35:23  mbickel
//      Some major restructuing:
//       new message containers
//       events don't store pointers to units any more
//       tfield class overhauled
//
//     Revision 1.7  2001/02/11 11:39:40  mbickel
//      Some cleanup and documentation
//
//     Revision 1.6  2001/01/28 14:04:14  mbickel
//      Some restructuring, documentation and cleanup
//      The resource network functions are now it their own files, the dashboard
//       as well
//      Updated the TODO list
//
//     Revision 1.5  2001/01/19 13:33:51  mbickel
//      The AI now uses hemming
//      Several bugfixes in Vehicle Actions
//      Moved all view calculation to viewcalculation.cpp
//      Mapeditor: improved keyboard support for item selection
//
//     Revision 1.4  2000/10/11 14:26:44  mbickel
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
//     Revision 1.3  2000/07/05 10:49:36  mbickel
//      Fixed AI bugs
//      setbuildingdamage event now updates the screen
//
//     Revision 1.2  1999/11/16 03:42:10  tmwilson
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


#if defined(karteneditor) && !defined(pbpeditor)
#error The mapeditor should not use missions.h
#endif


#ifndef missionsH
#define missionsH

#include "controls.h"
#include "weather.h"


extern void  checkevents( MapDisplayInterface* md );

extern void  checksingleevent( pevent ev, MapDisplayInterface* md );


extern void  releaseevent(pvehicle     eht,
                          pbuilding    bld,
                          int      action);

extern void  execevent( pevent ev, MapDisplayInterface* md );

extern void  checktimedevents( MapDisplayInterface* md );

extern void  getnexteventtime(void);

extern void  initmissions( void );

extern void mark_polygon_fields_with_connection ( pmap gamemap, int* data, int mark );
extern int unit_in_polygon ( tevent::LargeTriggerData::PolygonEntered* trigger );


// The new event system here is not yet functional

class EventTrigger {
      int triggerID;
   protected:
      EventTrigger ( int id ) : triggerID ( id ) {};
   public:
      enum State { unfulfilled, fulfilled, finally_failed, finally_fulfilled };
      virtual State getState( int player ) = 0;
      virtual void read ( tnstream& stream ) = 0;
      virtual void write ( tnstream& stream ) = 0;
      virtual const ASCString& getName() = 0;
};


class TurnPassed : public EventTrigger {
    public:
      TurnPassed();
      int turn;
      int move;

      virtual State getState( int player );
      virtual void read ( tnstream& stream ) ;
      virtual void write ( tnstream& stream ) ;
};


class UnitTrigger : public EventTrigger {
    public:
      UnitTrigger( int id ) : EventTrigger ( id ), unitID ( -1 ) {};
      int unitID;

      virtual void read ( tnstream& stream ) ;
      virtual void write ( tnstream& stream ) ;
};

class UnitLost : public UnitTrigger {
    public:
      UnitLost() : UnitTrigger ( ceventt_unitlost ) {};
      virtual State getState( int player );
};

class UnitConquered : public UnitTrigger {
    public:
      UnitConquered() : UnitTrigger ( ceventt_unitconquered ) {};
      virtual State getState( int player );
};

class UnitDestroyed : public UnitTrigger {
    public:
      UnitDestroyed() : UnitTrigger ( ceventt_unitdestroyed ) {};
      virtual State getState( int player );
};



class AllBuildingsLost : public EventTrigger {
    public:
      AllBuildingsLost() : EventTrigger ( ceventt_allbuildingslost ) {};

      virtual State getState( int player );
      virtual void read ( tnstream& stream ) { stream.readInt(); };
      virtual void write ( tnstream& stream ) { stream.writeInt(1); };
};

class AllUnitsLost : public EventTrigger {
    public:
      AllUnitsLost() : EventTrigger ( ceventt_allunitslost ) {};

      virtual State getState( int player );
      virtual void read ( tnstream& stream ) { stream.readInt(); };
      virtual void write ( tnstream& stream ) { stream.writeInt(1); };
};


class PositionTrigger : public EventTrigger {
   protected:
      PositionTrigger( int id ) : EventTrigger( id ) {};
   public:
      MapCoordinate3D pos;
      virtual void read ( tnstream& stream ) ;
      virtual void write ( tnstream& stream ) ;
};

class BuildingConquered : public PositionTrigger {
    protected:
      BuildingConquered( int id ) : PositionTrigger( id ) {};
    public:
      BuildingConquered() : PositionTrigger(ceventt_buildingconquered) {};
      virtual State getState( int player );
};

class BuildingLost: public BuildingConquered  {
   public:
      BuildingLost ( ) : BuildingConquered( ceventt_buildinglost ) {};
      virtual State getState( int player );
};

class BuildingDestroyed : public PositionTrigger {
    public:
      BuildingDestroyed() : PositionTrigger (ceventt_buildingdestroyed) {};
      virtual State getState( int player );
};

class BuildingSeen : public PositionTrigger {
    public:
      BuildingSeen() : PositionTrigger (ceventt_building_seen) {};
      virtual State getState( int player );
};

class EventTriggered : public EventTrigger {
    public:
      EventTriggered() : EventTrigger ( ceventt_event ), eventID(-1) {};

      int eventID;
      virtual State getState( int player );
      virtual void read ( tnstream& stream );
      virtual void write ( tnstream& stream );
};

class AllEnemyUnitsDestroyed : public EventTrigger {
    public:
      AllEnemyUnitsDestroyed() : EventTrigger ( ceventt_allenemyunitsdestroyed ) {};

      virtual State getState( int player );
      virtual void read ( tnstream& stream ) { stream.readInt(); };
      virtual void write ( tnstream& stream ) { stream.writeInt(1); };
};

class AllEnemyBuildingsDestroyed : public EventTrigger {
    public:
      AllEnemyBuildingsDestroyed() : EventTrigger ( ceventt_allenemybuildingsdestroyed ) {};

      virtual State getState( int player );
      virtual void read ( tnstream& stream ) { stream.readInt(); };
      virtual void write ( tnstream& stream ) { stream.writeInt(1); };
};






class EventAction {
      int actionID;
   protected:
      EventAction( int id ) : actionID ( id ) {};
   public:

      virtual void read ( tnstream& stream ) = 0;
      virtual void write ( tnstream& stream ) = 0;
      virtual ASCString getName();

      virtual void execute() = 0;

};

class WindChange: public EventAction {
   public:
      WindChange() : EventAction(cewindchange),
                     speed(-1),
                     direction(-1){};

      void read ( tnstream& stream );
      void write ( tnstream& stream );

      void execute();


      int speed;
      int direction;
};

class ChangeGameParameter: public EventAction {
    public:
     ChangeGameParameter(): EventAction(cegameparamchange),
                            parameterNum(-1),
                            parameterValue(0){};

      void read ( tnstream& stream );
      void write ( tnstream& stream );

      void execute();

      int parameterNum;
      int parameterValue;
};

class DisplayMessage: public EventAction {
    public:
      DisplayMessage(): EventAction(cegameparamchange),
                        messageNum(-1) {};
      int messageNum;

      void read ( tnstream& stream );
      void write ( tnstream& stream );

      void execute();
};

class MapModificationEvent : public EventAction {
   protected:
      MapModificationEvent ( int id ) : EventAction ( id ), addressingMode( none ) {};
      void readMapModificationData ( tnstream& stream );
      void writeMapModificationData ( tnstream& stream );

      void operate();
      virtual void fieldOperator( const MapCoordinate& mc ) = 0;
   public:

      enum AddressingMode { none, singleField, poly, global };

      AddressingMode addressingMode;
      typedef vector<MapCoordinate> Fields;
      Fields fields;

      typedef vector< Poly_gon > Polygons;
      Polygons polygons;

      void execute() { operate(); };
};

class WeatherChange : public MapModificationEvent {
   protected:
      void fieldOperator( const MapCoordinate& mc );
   public:
      WeatherChange(): MapModificationEvent(ceweatherchange),
                        weather(-1) {};
      int weather;

      void read ( tnstream& stream );
      void write ( tnstream& stream );
};

class MapChange : public MapModificationEvent {
   protected:
      void fieldOperator( const MapCoordinate& mc );
   public:
      MapChange(): MapModificationEvent(cemapchange),
                        terrainID(-1) {};
      int terrainID;

      void read ( tnstream& stream );
      void write ( tnstream& stream );
};

class AddObject : public MapModificationEvent {
   protected:
      void fieldOperator( const MapCoordinate& mc );
   public:
      AddObject(): MapModificationEvent(ceaddobject),
                        objectID(-1) {};
      int objectID;

      void read ( tnstream& stream );
      void write ( tnstream& stream );
};


class MapChangeCompleted : public EventAction {
    public:
      MapChangeCompleted(): EventAction(ceweatherchangecomplete) {};

      void read ( tnstream& stream ) {};
      void write ( tnstream& stream ) {};

      void execute();
};


class ChangeBuildingDamage: public EventAction {
   public:
      ChangeBuildingDamage() : EventAction(cechangebuildingdamage),
                     damage(0) {};

      void read ( tnstream& stream );
      void write ( tnstream& stream );

      void execute();

      int damage;
      MapCoordinate position;
};


class Event {
   public:
      Event();

      int triggerNum;
      int id;
      int player;
      ASCString  description;
      GameTime   triggerTime;
      struct {
         int turn;
         int move;   // negative values allowed !!
      } delayedexecution;

      EventAction* action;
};


#endif
