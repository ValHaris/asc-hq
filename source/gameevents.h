/*! \file gameevents.h
    \brief Interface to the event handling of ASC
*/

//     $Id: gameevents.h,v 1.3 2004-02-07 12:34:50 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.2  2004/02/01 13:39:53  mbickel
//      New event action: set view sharing
//      Fixed lots of bugs
//
//     Revision 1.1  2004/01/16 19:14:55  mbickel
//      Adding files for new event system
//
//     Revision 1.12  2003/07/06 13:16:22  mbickel
//      Updated maps
//
//     Revision 1.11  2003/06/26 21:00:18  mbickel
//      Fixed: land mines could not be carried on bridges
//
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


#ifndef gameeventsH
#define gameeventsH

#include "typen.h"
#include "mapdisplay.h"
#include "gameeventsystem.h"
#include "mappolygons.h"



// The new event system here is not yet functional

// namespace GameEvents {


enum EventTrigger_ID {Trigger_NothingTrue,
                     Trigger_NothingFalse,
                     Trigger_TurnPassed,
                     Trigger_UnitLost,
                     Trigger_UnitConquered,
                     Trigger_UnitDestroyed,
                     Trigger_AllBuildingsLost,
                     Trigger_AllUnitsLost,
                     Trigger_BuildingConquered,
                     Trigger_BuildingLost,
                     Trigger_BuildingDestroyed,
                     Trigger_BuildingSeen,
                     Trigger_EventTriggered,
                     Trigger_AllEnemyUnitsDestroyed,
                     Trigger_AllEnemyBuildingsDestroyed,
                     Trigger_AnyUnitEntersPolygon,
                     Trigger_SpecificUnitEntersPolygon,
                     Trigger_ResourceTribute };


enum EventAction_ID { EventAction_Nothing,
                     EventAction_WindChange,
                     EventAction_ChangeGameParameter,
                     EventAction_DisplayMessage,
                     EventAction_WeatherChange,
                     EventAction_MapChange,
                     EventAction_AddObject,
                     EventAction_MapChangeCompleted,
                     EventAction_ChangeBuildingDamage,
                     EventAction_NextMap,
                     EventAction_LoseMap,
                     EventAction_DisplayEllipse,
                     EventAction_RemoveEllipse,
                     EventAction_ChangeBuildingOwner,
                     EventAction_RemoveAllObjects,
                     EventAction_DisplayImmediateMessage,
                     EventAction_AddProductionCapabiligy,
                     EventAction_ChangeDiplomaticStatus,
                     EventAction_AddResources,
                     EventAction_Reinforcements,
                     EventAction_SetViewSharing };


class EventTriggered;

class FieldAddressing: protected PolygonPainerSquareCoordinate  {
      pmap& gameMap;
   public:
      typedef vector<MapCoordinate> Fields;
      typedef vector< Poly_gon > Polygons;

      void setField ( const MapCoordinate& pos );
      void setPolygon ( const Poly_gon& poly_gon );
      void setGlobal();

   protected:
      FieldAddressing( pmap& gamemap ) : addressingMode( none ), gameMap(gamemap) {};
      enum AddressingMode { none, singleField, poly, global };

      AddressingMode addressingMode;
      Fields fields;
      Polygons polygons;

      void readMapModificationData ( tnstream& stream );
      void writeMapModificationData ( tnstream& stream );
      virtual void fieldOperator( const MapCoordinate& mc ) = 0;
      void operate();
      void setpointabs ( int x,  int y  );

      void setup();
      friend Event* readOldEvent( pnstream stream, pmap map, map<int,int>& eventTranslation, map<EventTriggered*,int>& eventTriggerEvents );
};



class TriggerNothing : public EventTrigger {
    public:
      TriggerNothing( EventTriggerID id ) : EventTrigger ( id ) {};
      virtual void readData ( tnstream& stream ) ;
      virtual void writeData ( tnstream& stream ) ;
      void setup() {};
};

class TriggerNothingTrue: public TriggerNothing {
    protected:
      State getState( int player ) { return finally_fulfilled; };
   public:
      TriggerNothingTrue() : TriggerNothing( Trigger_NothingTrue ) {};
      ASCString getName() const { return "Nothing (true)"; };
};

class TriggerNothingFalse: public TriggerNothing {
    protected:
      State getState( int player ) { return finally_failed; };
   public:
      TriggerNothingFalse() : TriggerNothing( Trigger_NothingFalse ) {};
      ASCString getName() const { return "Nothing (false)"; };
};



class TurnPassed : public EventTrigger {
      int turn;
      int move;
    protected:
      virtual State getState( int player );
    public:
      TurnPassed() : EventTrigger ( Trigger_TurnPassed ), turn( 0 ), move( 0 ) {};
      TurnPassed( int turn_, int move_ ) : EventTrigger ( Trigger_TurnPassed ), turn( turn_ ), move( move_ ) {};

      virtual void readData ( tnstream& stream ) ;
      virtual void writeData ( tnstream& stream ) ;
      ASCString getName() const;
      void setup();
      void arm();
};


class UnitTrigger : public EventTrigger, public SigC::Object {
    protected:
      int unitID;
    public:
      UnitTrigger( EventTriggerID id ) : EventTrigger ( id ), unitID ( -1 ) {};

      virtual void readData ( tnstream& stream ) ;
      virtual void writeData ( tnstream& stream ) ;
      ASCString getName() const;
      void setup();
      void triggered();
};


class UnitLost : public UnitTrigger {
    protected:
      virtual State getState( int player );
    public:
      UnitLost() : UnitTrigger ( Trigger_UnitLost ) {};
      UnitLost( int unitID_) : UnitTrigger ( Trigger_UnitLost ) { unitID = unitID_;};
      ASCString getName() const;
      void arm();
};

class UnitConquered : public UnitTrigger {
    protected:
      virtual State getState( int player );
    public:
      UnitConquered() : UnitTrigger ( Trigger_UnitConquered ) {};
      UnitConquered( int unitID_) : UnitTrigger ( Trigger_UnitConquered ) { unitID = unitID_;};
      ASCString getName() const;
      void arm();
};

class UnitDestroyed : public UnitTrigger {
    protected:
      virtual State getState( int player );
    public:
      UnitDestroyed() : UnitTrigger ( Trigger_UnitDestroyed ) {};
      UnitDestroyed( int unitID_) : UnitTrigger ( Trigger_UnitDestroyed ) { unitID = unitID_;};
      ASCString getName() const;
      void arm();
};



class AllBuildingsLost : public EventTrigger {
    protected:
      virtual State getState( int player );
    public:
      AllBuildingsLost() : EventTrigger ( Trigger_AllBuildingsLost ) {};

      virtual void readData ( tnstream& stream ) { stream.readInt(); };
      virtual void writeData ( tnstream& stream ) { stream.writeInt(1); };
      ASCString getName() const;
      void setup() {};
};

class AllUnitsLost : public EventTrigger {
    protected:
      virtual State getState( int player );
    public:
      AllUnitsLost() : EventTrigger ( Trigger_AllUnitsLost ) {};

      virtual void readData ( tnstream& stream ) { stream.readInt(); };
      virtual void writeData ( tnstream& stream ) { stream.writeInt(1); };
      ASCString getName() const;
      void setup() {};
};


class PositionTrigger : public EventTrigger {
   protected:
      MapCoordinate pos;
      PositionTrigger( EventTriggerID id ) : EventTrigger( id ) {};
   public:
      virtual void readData ( tnstream& stream ) ;
      virtual void writeData ( tnstream& stream ) ;
};

class BuildingPositionTrigger : public PositionTrigger {
   protected:
      BuildingPositionTrigger( EventTriggerID id ) : PositionTrigger( id ) {};
   public:
      ASCString getName() const;
      void setup();
};


class BuildingConquered : public BuildingPositionTrigger, public SigC::Object {
    protected:
      BuildingConquered( EventTriggerID id ) : BuildingPositionTrigger( id ) {};
      virtual State getState( int player );
    public:
      BuildingConquered() : BuildingPositionTrigger( Trigger_BuildingConquered ) {};
      BuildingConquered( const MapCoordinate& position) : BuildingPositionTrigger( Trigger_BuildingConquered ) { pos = position;};
      ASCString getName() const { return BuildingPositionTrigger::getName() + " conquered"; };
      void arm();
      void triggered();
};

class BuildingLost: public BuildingConquered  {
    protected:
      virtual State getState( int player );
   public:
      BuildingLost ( ) : BuildingConquered( Trigger_BuildingLost ) {};
      BuildingLost( const MapCoordinate& position) : BuildingConquered( position ) {};
      ASCString getName() const { return BuildingPositionTrigger::getName() + " Lost"; };
      void arm();
};

class BuildingDestroyed : public BuildingPositionTrigger {
    protected:
      virtual State getState( int player );
    public:
      BuildingDestroyed() : BuildingPositionTrigger ( Trigger_BuildingDestroyed ) {};
      BuildingDestroyed( const MapCoordinate& position) : BuildingPositionTrigger( Trigger_BuildingDestroyed ) { pos = position;};
      ASCString getName() const { return BuildingPositionTrigger::getName() + " destroyed"; };
};

class BuildingSeen : public BuildingPositionTrigger, public SigC::Object {
    protected:
      virtual State getState( int player );
    public:
      BuildingSeen() : BuildingPositionTrigger ( Trigger_BuildingSeen ) {};
      BuildingSeen( const MapCoordinate& position) : BuildingPositionTrigger( Trigger_BuildingSeen ) { pos = position;};
      ASCString getName() const { return BuildingPositionTrigger::getName() + " seen"; };
      void arm();
      void triggered();
};

class EventTriggered : public EventTrigger, public SigC::Object {
      int eventID;
      Event* getTargetEventName() const;
    protected:
      virtual State getState( int player );
    public:
      EventTriggered( int eventID_ = -1 ) : EventTrigger ( Trigger_EventTriggered ), eventID( eventID_ ) {};
      void setEventID( int eventID_ ) { eventID = eventID_; };

      virtual void readData ( tnstream& stream );
      virtual void writeData ( tnstream& stream );
      ASCString getName() const;
      void setup();
      void arm();
      void triggered();
};

class AllEnemyUnitsDestroyed : public EventTrigger, public SigC::Object {
    protected:
      virtual State getState( int player );
    public:
      AllEnemyUnitsDestroyed() : EventTrigger ( Trigger_AllEnemyUnitsDestroyed ) {};

      virtual void readData ( tnstream& stream ) { stream.readInt(); };
      virtual void writeData ( tnstream& stream ) { stream.writeInt(1); };

      ASCString getName() const;
      void setup() {};
      void arm();
      void triggered();
};

class AllEnemyBuildingsDestroyed : public EventTrigger, public SigC::Object {
    protected:
      virtual State getState( int player );
    public:
      AllEnemyBuildingsDestroyed() : EventTrigger ( Trigger_AllEnemyBuildingsDestroyed ) {};

      virtual void readData ( tnstream& stream ) { stream.readInt(); };
      virtual void writeData ( tnstream& stream ) { stream.writeInt(1); };
      ASCString getName() const;
      void setup() {};
      void arm();
      void triggered();
};

class SpecificUnitEntersPolygon : public EventTrigger, public FieldAddressing, public SigC::Object {
      int unitID;
      bool found;
      bool arming;
    protected:
      virtual State getState( int player );
    public:
      SpecificUnitEntersPolygon( int unitID_ = -1 ) : EventTrigger ( Trigger_SpecificUnitEntersPolygon ), FieldAddressing(gamemap),unitID( unitID_ ), found(false), arming(false) {};

      virtual void readData ( tnstream& stream );
      virtual void writeData ( tnstream& stream );
      void setup();
      void arm();
      void triggered();

    protected:
      void fieldOperator( const MapCoordinate& mc );
      ASCString getName() const;
};

class AnyUnitEntersPolygon : public EventTrigger, public FieldAddressing, public SigC::Object {
      int player;
      bool found;
      bool arming;
    protected:
      virtual State getState( int player );
    public:
      AnyUnitEntersPolygon( int player_ = 0 ) : EventTrigger ( Trigger_AnyUnitEntersPolygon ), FieldAddressing(gamemap), player( player_ ), found(false), arming(false) {};

      virtual void readData ( tnstream& stream );
      virtual void writeData ( tnstream& stream );
      void setup();
      void arm();
      void triggered();

    protected:
      void fieldOperator( const MapCoordinate& mc );
      ASCString getName() const;
};


class ResourceTribute : public EventTrigger, public SigC::Object {
      Resources demand;
      int payingPlayer;
    protected:
      virtual State getState( int player );
    public:

      ResourceTribute() : EventTrigger ( Trigger_ResourceTribute ), payingPlayer( -1 ) {};

      virtual void readData ( tnstream& stream );
      virtual void writeData ( tnstream& stream );
      ASCString getName() const;
      void setup();
      void arm();
      void triggered();
};





class Action_Nothing: public EventAction {
   public:
      Action_Nothing( ) : EventAction( EventAction_Nothing ) {};

      void readData ( tnstream& stream );
      void writeData ( tnstream& stream );

      void execute( MapDisplayInterface* md ) {};
      void setup() {};
};


class WindChange: public EventAction {
      int speed;
      int direction;
   public:
      WindChange() : EventAction( EventAction_WindChange ),
                     speed(0),
                     direction(0){};

      WindChange( int Speed, int Direction) : EventAction( EventAction_WindChange ),
                                              speed ( Speed ),
                                              direction ( Direction ) {};

      void readData ( tnstream& stream );
      void writeData ( tnstream& stream );

      void execute( MapDisplayInterface* md );
      void setup();
};

class ChangeGameParameter: public EventAction {
      int parameterNum;
      int parameterValue;
    public:
     ChangeGameParameter(): EventAction( EventAction_ChangeGameParameter ),
                            parameterNum(-1),
                            parameterValue(0){};

     ChangeGameParameter( int ParameterNum, int ParameterValue): EventAction( EventAction_ChangeGameParameter ),
                            parameterNum(ParameterNum),
                            parameterValue(ParameterValue){};

      void readData ( tnstream& stream );
      void writeData ( tnstream& stream );

      void execute( MapDisplayInterface* md );
      void setup();
};

class DisplayMessage: public EventAction {
      int messageNum;
    public:
      DisplayMessage(): EventAction( EventAction_DisplayMessage ),
                        messageNum(-1) {};
      DisplayMessage( int messageNumber ) : EventAction( EventAction_DisplayMessage ),
                        messageNum( messageNumber ) {};
      void readData ( tnstream& stream );
      void writeData ( tnstream& stream );

      void execute( MapDisplayInterface* md );

      void setup();
};


class MapModificationEvent : public EventAction, public FieldAddressing {
   protected:
      MapModificationEvent ( EventActionID id ) : EventAction ( id ), FieldAddressing(gamemap) {};
   public:
      void execute( MapDisplayInterface* md );
};

class WeatherChange : public MapModificationEvent {
      int weather;
   protected:
      void fieldOperator( const MapCoordinate& mc );
   public:
      WeatherChange( int weather_ = -1 ): MapModificationEvent( EventAction_WeatherChange ),
                        weather( weather_ ) {};
      void readData ( tnstream& stream );
      void writeData ( tnstream& stream );

      void setup();
};

class MapChange : public MapModificationEvent {
      int terrainID;
   protected:
      void fieldOperator( const MapCoordinate& mc );
   public:
      MapChange( int terrainID_ = -1 ): MapModificationEvent( EventAction_MapChange ),
                        terrainID( terrainID_ ) {};
      void readData ( tnstream& stream );
      void writeData ( tnstream& stream );
      void setup();
};

class AddObject : public MapModificationEvent {
      int objectID;
   protected:
      void fieldOperator( const MapCoordinate& mc );
   public:
      AddObject(): MapModificationEvent( EventAction_AddObject ),
                        objectID(-1) {};
      AddObject( int objectID_ ): MapModificationEvent( EventAction_AddObject ),
                        objectID(objectID_) {};
      void readData ( tnstream& stream );
      void writeData ( tnstream& stream );
      void setup();
};


class MapChangeCompleted : public EventAction {
    public:
      MapChangeCompleted(): EventAction( EventAction_MapChangeCompleted ) {};

      void readData ( tnstream& stream ) {};
      void writeData ( tnstream& stream ) {};

      void execute( MapDisplayInterface* md );
      void setup() {};
};


class ChangeBuildingDamage: public EventAction {
      int damage;
      MapCoordinate position;
   public:
      ChangeBuildingDamage() : EventAction( EventAction_ChangeBuildingDamage ),
                     damage(0) {};
      ChangeBuildingDamage( const MapCoordinate& pos, int dam ): EventAction( EventAction_ChangeBuildingDamage ),
                     position(pos), damage(dam) {};

      void readData ( tnstream& stream );
      void writeData ( tnstream& stream );

      void execute( MapDisplayInterface* md );
      void setup();
};

class NextMap : public EventAction {
      int mapID;
   public:
      NextMap() : EventAction ( EventAction_NextMap ), mapID(0) {};
      NextMap( int mapID_ ) : EventAction ( EventAction_NextMap ), mapID(mapID_) {};

      void readData ( tnstream& stream );
      void writeData ( tnstream& stream );

      void execute( MapDisplayInterface* md );
      void setup();
};

class LoseMap : public EventAction {
    public:
      LoseMap(): EventAction( EventAction_LoseMap ) {};

      void readData ( tnstream& stream ) {};
      void writeData ( tnstream& stream ) {};

      void execute( MapDisplayInterface* md );
      void setup() {};
};

class DisplayEllipse : public EventAction {
      int x1,y1,x2,y2;
      bool alignRight;
      bool alignBottom;
    public:
      DisplayEllipse(): EventAction( EventAction_DisplayEllipse ),
                        x1(-1),y1(-1),x2(-1),y2(-1),
                        alignRight(false),alignBottom(false) {};
      DisplayEllipse(int x1_, int y1_, int x2_, int y2_, bool alignRight_, bool alignBottom_): EventAction( EventAction_DisplayEllipse ),
                        x1(x1_),y1(y1_),x2(x2_),y2(y2_),
                        alignRight(alignRight_),alignBottom(alignBottom_) {};

      void readData ( tnstream& stream );
      void writeData ( tnstream& stream );

      void execute( MapDisplayInterface* md );
      void setup();
};

class RemoveEllipse : public EventAction {
    public:
      RemoveEllipse(): EventAction( EventAction_RemoveEllipse ) {};

      void readData ( tnstream& stream ) {};
      void writeData ( tnstream& stream ) {};

      void execute( MapDisplayInterface* md );
      void setup() {};
};

class RemoveAllObjects : public MapModificationEvent {
   protected:
      void fieldOperator( const MapCoordinate& mc );
   public:
      RemoveAllObjects(): MapModificationEvent( EventAction_RemoveAllObjects ) {};
      void readData ( tnstream& stream );
      void writeData ( tnstream& stream );
      void setup();
};

class ChangeBuildingOwner : public EventAction {
      MapCoordinate pos;
      int newOwner;
    public:
      ChangeBuildingOwner(): EventAction( EventAction_ChangeBuildingOwner ), newOwner(0) {};

      void readData ( tnstream& stream );
      void writeData ( tnstream& stream );

      void execute( MapDisplayInterface* md );
      void setup();
};

class DisplayImmediateMessage: public EventAction {
      ASCString message;
    public:
      DisplayImmediateMessage(): EventAction( EventAction_DisplayImmediateMessage ) {};
      void readData ( tnstream& stream );
      void writeData ( tnstream& stream );

      void execute( MapDisplayInterface* md );

      void setup();
};

class AddProductionCapability : public EventAction {
      MapCoordinate pos;
      int vehicleTypeID;
    public:
      AddProductionCapability(): EventAction( EventAction_AddProductionCapabiligy ), vehicleTypeID(-1) {};

      void readData ( tnstream& stream );
      void writeData ( tnstream& stream );

      void execute( MapDisplayInterface* md );
      void setup();
};


class ChangeDiplomaticStatus : public EventAction {
      int proposingPlayer;
      int targetPlayer;
      enum Proposal { Peace, War, SneakAttack } proposal;
    public:
      ChangeDiplomaticStatus(): EventAction( EventAction_ChangeDiplomaticStatus ), proposingPlayer(0), targetPlayer(0), proposal(Peace){};

      void readData ( tnstream& stream );
      void writeData ( tnstream& stream );

      void execute( MapDisplayInterface* md );
      void setup();
};

class SetViewSharing : public EventAction {
      int viewingPlayer;
      int providingPlayer;
      bool enable;
    public:
      SetViewSharing(): EventAction( EventAction_SetViewSharing ), viewingPlayer(0), providingPlayer(0), enable(true){};

      void readData ( tnstream& stream );
      void writeData ( tnstream& stream );

      void execute( MapDisplayInterface* md );
      void setup();
};


class AddResources : public EventAction {
      MapCoordinate pos;
      Resources res;
    public:
      AddResources(): EventAction( EventAction_AddResources ) {};

      void readData ( tnstream& stream );
      void writeData ( tnstream& stream );

      void execute( MapDisplayInterface* md );
      void setup();
};

class Reinforcements : public EventAction {
      tmemorystreambuf buf;
      int objectNum;
      enum Type { ReinfVehicle, ReinfBuilding };
   public:
      Reinforcements(): EventAction( EventAction_Reinforcements ), objectNum(0) {};

      void readData ( tnstream& stream );
      void writeData ( tnstream& stream );

      void execute( MapDisplayInterface* md );
      void setup();
};



// }; // namespace GameEvents

#endif
