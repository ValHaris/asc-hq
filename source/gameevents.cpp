/*! \file gameevents.cpp
    \brief The event handling of ASC
*/

/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2005  Martin Bickel  and  Marc Schellenberger

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
// #include "basegfx.h"

#include "gameevent_dialogs.h"
#include "spfst.h"
#include "loaders.h"
#include "gameevents.h"
#include "dlg_box.h"
#include "dialog.h"
#include "errors.h"
#include "itemrepository.h"
#include "messagedlg.h"
#include "mapdisplayinterface.h"

#ifdef sgmain
// # include "gamedlg.h"
# include "viewcalculation.h"
# include "resourcenet.h"
# include "unitctrl.h"
#endif


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
      #ifdef sgmain
      if ( player == actmap->actplayer )
         viewunreadmessages ( actmap->player[ actmap->actplayer ] );
      #endif
   } else
      displaymessage( "Message %d not found", 1, id );
}




/////////////////////////////////////////////////////////////////////////////
// Trigger


int versionTest( tnstream& stream, int min, int max )
{
   int version = stream.readInt();
   if ( version < min || version > max )
      throw tinvalidversion( stream.getLocation(), max, version );
   return version;
}      

void TriggerNothing::readData( tnstream& stream )
{
   versionTest(stream,1,1);
}

void TriggerNothing::writeData( tnstream& stream )
{
   stream.writeInt(1);
}



EventTrigger::State TurnPassed::getState( int player )
{
   if ( gamemap->time.turn() > turn || (gamemap->time.turn() == turn && gamemap->time.move() >= move ))
      return finally_fulfilled;
   else
      return unfulfilled;
}


void TurnPassed::arm()
{
   GameTime t;
   t.set ( turn, move );
   gamemap->eventTimes.push_back ( t );
   sort( gamemap->eventTimes.begin(), gamemap->eventTimes.end(), GameTime::comp );
}

void TurnPassed::readData( tnstream& stream )
{
   versionTest(stream,1,1);
   turn = stream.readInt();
   move = stream.readInt();
}

void TurnPassed::writeData( tnstream& stream )
{
   stream.writeInt(1);
   stream.writeInt( turn );
   stream.writeInt( move );
}

void TurnPassed::setup()
{
   turn = editInt( "turn", turn );
   move = editInt( "move", move );
}


ASCString TurnPassed::getName() const
{
   ASCString s;
   s.format( "turn %d , move %d passed ", turn, move );
   return s; 
}



ASCString BuildingPositionTrigger::getName() const
{
   ASCString s = "Building ";
   if ( gamemap->getField(pos) && gamemap->getField(pos)->building )
      s += gamemap->getField(pos)->building->getName();
   else
      s += " <not found> ";

   return s;
}

void BuildingPositionTrigger::setup()
{
   getxy_building( &pos.x, &pos.y );
}



EventTrigger::State BuildingConquered::getState( int player )
{
   tfield* fld = gamemap->getField ( pos );
   if( !fld ) {
      displaymessage ("invalid event - map field not found!", 1);
      return finally_failed;
   }

   if ( !fld->building )
      return finally_failed;

   if ( fld->building->getOwner() == player )
      return fulfilled;
   else
      return unfulfilled;
}

void BuildingConquered::arm()
{
   if( !gamemap->getField ( pos ) ) {
      displaymessage ("invalid event - map field not found!", 1);
      return;
   }

   Building* bld = gamemap->getField ( pos )->building;
   if ( bld )
      bld->conquered.connect( SigC::slot( *this, &BuildingConquered::triggered ));
}

void BuildingConquered::triggered()
{
   if ( isFulfilled() )
      eventReady();
}


EventTrigger::State BuildingLost::getState( int player )
{
   State s = BuildingConquered::getState ( player );
   if ( s == fulfilled)
      return unfulfilled;
   if ( s == unfulfilled )
      return fulfilled;
   return s;
}


void BuildingLost::arm()
{
   if( !gamemap->getField ( pos ) ) {
      displaymessage ("invalid event - map field not found!", 1);
      return;
   }

   Building* bld = gamemap->getField ( pos )->building;
   if ( bld ) {
      bld->conquered.connect( SigC::slot( *this, &BuildingConquered::triggered ));
      bld->destroyed.connect( SigC::slot( *this, &BuildingConquered::triggered ));
   }
}



void PositionTrigger::readData( tnstream& stream )
{
   versionTest(stream,1,1);
   pos.read( stream );
}


void PositionTrigger::writeData( tnstream& stream )
{
   stream.writeInt(1);
   pos.write( stream );
}

EventTrigger::State BuildingDestroyed::getState( int player )
{
   if( !gamemap->getField ( pos ) ) {
      displaymessage ("invalid event - map field not found!", 1);
      return finally_fulfilled;
   }

   tfield* fld = gamemap->getField ( pos );
   if ( !fld->building )
      return finally_fulfilled;
   else
      return unfulfilled;
}


EventTrigger::State BuildingSeen::getState( int player )
{
   if( !gamemap->getField ( pos ) ) {
      displaymessage ("invalid event - map field not found!", 1);
      return finally_fulfilled;
   }

   Building* bld = gamemap->getField ( pos )->building;
   if ( !bld )
      return finally_failed;

   int cnt = 0;
   for ( int x = 0; x < 4; x++ )
      for ( int y = 0; y < 6; y++ ) {
         if ( bld->typ->fieldExists ( BuildingType::LocalCoordinate(x, y) ) ) {
            tfield* fld = bld->getField ( BuildingType::LocalCoordinate( x, y) );
            if ( fld ) {
               int vis = (fld-> visible >> (player*2) ) & 3;
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

   if ( cnt )
      return finally_fulfilled;
   else
      return unfulfilled;
}

void BuildingSeen::arm()
{
   if( !gamemap->getField ( pos ) ) {
      displaymessage ("invalid event - map field not found!", 1);
      return;
   }

   Building* bld = gamemap->getField ( pos )->building;
   if ( bld ) {
      bld->connection |= cconnection_seen;
      #ifdef sgmain
      buildingSeen.connect( SigC::slot( *this, &BuildingSeen::triggered ));
      #endif
   }
}

void BuildingSeen::triggered()
{
   if ( isFulfilled() )
      eventReady();
}


EventTrigger::State AllBuildingsLost::getState( int player )
{
   if ( gamemap->player[player].buildingList.empty() )
      return fulfilled;
   else
      return unfulfilled;
}

ASCString AllBuildingsLost::getName() const
{
   return "All Buildings Lost";
}


EventTrigger::State AllUnitsLost::getState( int player )
{
   if ( gamemap->player[player].vehicleList.empty() )
      return fulfilled;
   else
      return unfulfilled;
}

ASCString AllUnitsLost::getName() const
{
   return "All Units Lost";
}


void UnitTrigger::readData ( tnstream& stream )
{
   versionTest(stream,1,1);
   unitID = stream.readInt();
}


void UnitTrigger::writeData ( tnstream& stream )
{
   stream.writeInt(1);
   stream.writeInt( unitID );
}

ASCString UnitTrigger::getName() const
{
   ASCString s = "unit ";
   if ( gamemap )
      if ( gamemap->getUnit( unitID ))
         s += gamemap->getUnit( unitID )->getName();
      else
         s += "<not found>";
   return s;
}

void UnitTrigger::setup()
{
   unitID = selectunit ( unitID );
}

void UnitTrigger::triggered()
{
   state( -1 );
   eventReady();
}


void UnitLost::arm()
{
   Vehicle* veh = gamemap->getUnit( unitID );
   if ( veh ) {
      veh->destroyed.connect( SigC::slot( *this, &UnitLost::triggered ));
      veh->conquered.connect( SigC::slot( *this, &UnitLost::triggered ));
   }
}


EventTrigger::State UnitLost::getState( int player )
{
  Vehicle* veh = gamemap->getUnit( unitID );
  if ( !veh )
     return finally_fulfilled;
  if ( veh->getOwner() != player )
     return fulfilled;
  return unfulfilled;
}

ASCString UnitLost::getName() const
{
   return UnitTrigger::getName() + " lost";
}





EventTrigger::State UnitConquered::getState( int player )
{
  Vehicle* veh = gamemap->getUnit( unitID );
  if ( !veh )
     return finally_failed;
  if ( veh->getOwner() == player )
     return fulfilled;
  return unfulfilled;

}

ASCString UnitConquered::getName() const
{
   return UnitTrigger::getName() + " conquered";
}

void UnitConquered::arm()
{
   Vehicle* veh = gamemap->getUnit( unitID );
   if ( veh )
      veh->conquered.connect( SigC::slot( *this, &UnitConquered::triggered ));
}




EventTrigger::State UnitDestroyed::getState( int player )
{
  Vehicle* veh = gamemap->getUnit( unitID );
  if ( !veh )
     return finally_fulfilled;
  return unfulfilled;
}

ASCString UnitDestroyed::getName() const
{
   return UnitTrigger::getName() + " destroyed";
}

void UnitDestroyed::arm()
{
  Vehicle* veh = gamemap->getUnit( unitID );
  if ( veh )
     veh->destroyed.connect( SigC::slot( *this, &UnitDestroyed::triggered ));
}




EventTrigger::State EventTriggered::getState( int player )
{
   for ( GameMap::Events::iterator i = gamemap->events.begin(); i != gamemap->events.end(); ++i )
      if ( (*i)->id == eventID && (*i)->status == Event::Executed )
        return finally_fulfilled;
  return unfulfilled;
}

void EventTriggered::readData ( tnstream& stream )
{
   versionTest(stream,1,1);
   eventID = stream.readInt();
}

void EventTriggered::writeData ( tnstream& stream )
{
   stream.writeInt(1);
   stream.writeInt( eventID );
}


Event* EventTriggered::getTargetEventName() const
{
   if ( gamemap )
      for ( GameMap::Events::iterator i = gamemap->events.begin(); i != gamemap->events.end(); ++i )
         if ( (*i)->id == eventID )
            return *i;
   return NULL;
}

ASCString EventTriggered::getName() const
{
   Event* e = getTargetEventName();
   if ( e ) {
      ASCString s = "Event ";
      if ( !e->description.empty() )
         s += e->description;
      else
         s += e->action->getName();
      s += " triggered";
      return s;
   } else
      return "Event triggered";
}

void EventTriggered::setup()
{
   vector<int> eventIDs;
   vector<ASCString> eventnames;
   int fnd = -1;
   int counter = 0;
   for ( GameMap::Events::iterator i = gamemap->events.begin(); i != gamemap->events.end(); ++i ) {
      eventnames.push_back( (*i)->action->getName() + " : " + (*i)->description );
      eventIDs.push_back ( (*i)->id );
      if ( (*i)->id == eventID )
         fnd = counter;
      ++counter;
   }

   int res = chooseString ( "select event", eventnames, fnd );
   if ( res >= 0)
      eventID = eventIDs[res];
}

void EventTriggered::arm()
{
   Event* e = getTargetEventName();
   if ( e )
      e->executed.connect( SigC::slot( *this, &EventTriggered::triggered));
}

void EventTriggered::triggered()
{
   state( -1 );
   eventReady();
}


EventTrigger::State AllEnemyUnitsDestroyed::getState( int player )
{
   for ( int i = 0; i < 8; i++ )
         if ( !gamemap->player[i].vehicleList.empty() )
            return unfulfilled;

    return fulfilled;
}

ASCString AllEnemyUnitsDestroyed::getName() const
{
   return "All enemy units destroyed";
}

void AllEnemyUnitsDestroyed::arm()
{
   ContainerBase::anyContainerDestroyed.connect( SigC::slot( *this, &AllEnemyUnitsDestroyed::triggered));
}

void AllEnemyUnitsDestroyed::triggered( ContainerBase* c )
{
   if ( isFulfilled() )
      eventReady();
}


EventTrigger::State AllEnemyBuildingsDestroyed::getState( int player )
{
   for ( int i = 0; i < 8; i++ )
      if ( actmap->getPlayer(player).diplomacy.isHostile(i))
         if ( !gamemap->player[i].buildingList.empty() )
            return unfulfilled;

    return fulfilled;
}

ASCString AllEnemyBuildingsDestroyed::getName() const
{
   return "All enemy buildings destroyed";
}


void AllEnemyBuildingsDestroyed::arm()
{
   ContainerBase::anyContainerDestroyed.connect( SigC::slot( *this, &AllEnemyBuildingsDestroyed::triggered));
}

void AllEnemyBuildingsDestroyed::triggered( ContainerBase* c )
{
   if ( isFulfilled() )
      eventReady();
}


EventTrigger::State SpecificUnitEntersPolygon::getState( int player )
{
  found = false;
  operate();
  if ( found )
     return fulfilled;
  else
     return unfulfilled;
}

void SpecificUnitEntersPolygon::fieldOperator( const MapCoordinate& mc )
{
   tfield* fld = gamemap->getField ( mc );
   if ( !arming ) {
      if ( fld && fld->vehicle )
         if ( fld->vehicle->networkid == unitID || unitID == -1 )
            found = true;

      Vehicle* veh = gamemap->getUnit( unitID );
      if ( veh )
         if ( mc.x == veh->getPosition().x && mc.y == veh->getPosition().y )
            found = true;
   } else
      fld->connection |= cconnection_areaentered_specificunit;
}

void SpecificUnitEntersPolygon::readData ( tnstream& stream )
{
  versionTest(stream,1,1);
  readMapModificationData( stream );
  unitID = stream.readInt();
}

void SpecificUnitEntersPolygon::writeData ( tnstream& stream )
{
  stream.writeInt(1);
  writeMapModificationData( stream );
  stream.writeInt( unitID );
}

ASCString SpecificUnitEntersPolygon::getName() const
{
   ASCString s;
   if ( unitID > 0  && gamemap->getUnit( unitID )) {
      s = "unit ";
      s += gamemap->getUnit( unitID )->getName();
   } else
      s = "any unit";
   s += " enters polygon";
   return s;
}

void SpecificUnitEntersPolygon::setup()
{
   FieldAddressing::setup();
   unitID = selectunit ( unitID );
}

void SpecificUnitEntersPolygon::arm ()
{
   #ifdef sgmain
   arming = true;
   operate();
   arming = false;
   fieldCrossed.connect( SigC::slot( *this, &SpecificUnitEntersPolygon::triggered));
   Vehicle* veh = gamemap->getUnit( unitID );
   if ( veh )
      veh->connection |= cconnection_areaentered_specificunit;
   #endif
}

void SpecificUnitEntersPolygon::triggered()
{
   if ( isFulfilled() )
      eventReady();
}



EventTrigger::State AnyUnitEntersPolygon::getState( int player )
{
  found = false;
  operate();
  if ( found )
     return finally_fulfilled;
  else
     return unfulfilled;
}

void AnyUnitEntersPolygon::fieldOperator( const MapCoordinate& mc )
{
   tfield* fld = gamemap->getField ( mc );
   if ( !arming ) {
      if ( fld && fld->vehicle )
         if ( (1 << fld->vehicle->getOwner()) & player )
            found = true;

      if ( fld && fld->building )
         for ( ContainerBase::Cargo::const_iterator i = fld->building->getCargo().begin(); i != fld->building->getCargo().end(); ++i )
            if ( *i ) 
               if ( (1 << (*i)->getOwner()) & player )
                  found = true;

   } else {
     fld->connection |= cconnection_areaentered_anyunit;
   }
}


void AnyUnitEntersPolygon::readData ( tnstream& stream )
{
  versionTest(stream,1,1);
  readMapModificationData( stream );
  player = stream.readInt();
}

void AnyUnitEntersPolygon::writeData ( tnstream& stream )
{
  stream.writeInt(1);
  writeMapModificationData( stream );
  stream.writeInt( player );
}

ASCString AnyUnitEntersPolygon::getName() const
{
   ASCString s = "any unit from ";
   for ( int i = 0; i < 8; ++i )
      if ( player & ( 1 << i)) {
         s += gamemap->player[i].getName();
         s += " ";
      }
   s += " enters polygon";
   return s;
}

void AnyUnitEntersPolygon::setup()
{
   FieldAddressing::setup();
   playerselall( &player );
}

void AnyUnitEntersPolygon::arm()
{
   #ifdef sgmain
   arming = true;
   operate();
   arming = false;
   fieldCrossed.connect( SigC::slot( *this, &AnyUnitEntersPolygon::triggered));
   #endif
}

void AnyUnitEntersPolygon::triggered()
{
   if ( isFulfilled() )
      eventReady();
}


EventTrigger::State ResourceTribute::getState( int player )
{
    if ( payingPlayer < 0 || gamemap->tribute.avail[payingPlayer][player] + gamemap->tribute.paid[player][payingPlayer] < demand )
       return unfulfilled;
    else
       return finally_fulfilled;
}

void ResourceTribute::readData ( tnstream& stream )
{
   versionTest(stream,1,1);
   payingPlayer = stream.readInt();
   demand.read ( stream );
}


void ResourceTribute::writeData ( tnstream& stream )
{
   stream.writeInt( 1 );
   stream.writeInt( payingPlayer );
   demand.write ( stream );
}


ASCString ResourceTribute::getName() const
{
   ASCString s;
   s.format ( "Resource tribute: %d E ; %d M ; %d F", demand.energy, demand.material, demand.fuel );
   return s;
}

void ResourceTribute::setup()
{
   payingPlayer = editInt( "Paying Player", payingPlayer );
   demand.energy = editInt( "energy", demand.energy );
   demand.material = editInt( "material", demand.material );
   demand.fuel = editInt( "fuel", demand.fuel );
}

void ResourceTribute::arm()
{
  #ifdef sgmain
   tributeTransferred.connect( SigC::slot( *this, &ResourceTribute::triggered));
  #endif 
}

void ResourceTribute::triggered()
{
   if ( isFulfilled() )
      eventReady();
}

/////////////////////////////////////////////////////////////////////////////
// Actions


void Action_Nothing::readData( tnstream& stream )
{
   stream.readInt();
}

void Action_Nothing::writeData( tnstream& stream )
{
   stream.writeInt(1);
}


void WindChange::execute( MapDisplayInterface* md )
{
   if ( speed != -1 )
      gamemap->weather.windSpeed = speed;

   if ( direction != -1 )
      gamemap->weather.windDirection = direction;

   if ( md )
      md->updateDashboard();
}

void WindChange::readData( tnstream& stream )
{
   versionTest(stream,1,1);
   speed = stream.readInt();
   direction = stream.readInt();
}

void WindChange::writeData( tnstream& stream )
{
   stream.writeInt(1);
   stream.writeInt( speed );
   stream.writeInt( direction );
}

void WindChange::setup()
{
   speed     = editInt("Wind Speed", speed, 0, 255);
   direction = editInt("Wind Direction", direction, 0, 5);
}



void ChangeGameParameter::execute( MapDisplayInterface* md )
{
   if ( parameterNum >= 0 )
      if ( gameParameterSettings[parameterNum ].changeableByEvent )
         gamemap->setgameparameter( GameParameter(parameterNum) , parameterValue );
}

void ChangeGameParameter::readData( tnstream& stream )
{
   versionTest(stream,1,1);
   parameterNum = stream.readInt();
   parameterValue = stream.readInt();
}

void ChangeGameParameter::writeData( tnstream& stream )
{
   stream.writeInt(1);
   stream.writeInt( parameterNum );
   stream.writeInt( parameterValue );
}

int ChangeGameParameter::selectgameparameter( int para )
{
   vector<ASCString> list;
   for ( int i = 0; i < gameparameternum; ++i )
      list.push_back ( gameParameterSettings[i].name );
   return chooseString("Select GameParameter", list, para );
}

void ChangeGameParameter::setup()
{
    int nr = selectgameparameter( parameterNum );
    if ( (nr >= 0) && ( nr < gameparameternum) ) {
       if ( gameParameterSettings[nr].changeableByEvent ) {
          int org = parameterValue;
          if ( org < gameParameterSettings[nr].minValue && org > gameParameterSettings[nr].maxValue )
             org = gameParameterSettings[nr].defaultValue;
          parameterValue = getid("Parameter Val", org, gameParameterSettings[nr].minValue, gameParameterSettings[nr].maxValue);
          parameterNum = nr;
       } else
          displaymessage("This parameter cannot be changed by events",1);
    }
    
}



void DisplayMessage::execute( MapDisplayInterface* md )
{
   if ( gamemap->state == GameMap::Replay )
      return;

   viewtextmessage ( messageNum , gamemap->actplayer );
}

void DisplayMessage::readData( tnstream& stream )
{
   versionTest(stream,1,1);
   messageNum = stream.readInt();
}

void DisplayMessage::writeData( tnstream& stream )
{
   stream.writeInt(1);
   stream.writeInt( messageNum );
}

void DisplayMessage::setup()
{
  messageNum = editInt ( "Message ID", messageNum );
}




void FieldAddressing::setpointabs ( int x,  int y  )
{
   fieldOperator ( MapCoordinate(x,y) );
}

void FieldAddressing::operate (  )
{
   if ( addressingMode == singleField ) {
      for ( Fields::iterator i = fields.begin(); i != fields.end(); ++i )
         fieldOperator ( *i );
   }
   if ( addressingMode == poly ) {
      for ( Polygons::iterator i = polygons.begin(); i != polygons.end(); ++i )
         paintPolygon ( *i );
   }
   if ( addressingMode == global ) {
      for ( int y = 0; y < gameMap->ysize; ++y )
         for ( int x = 0; x < gameMap->xsize; ++x )
             fieldOperator ( MapCoordinate ( x, y ));
   }
}

void FieldAddressing::setField ( const MapCoordinate& pos )
{
   addressingMode = singleField;
   fields.clear();
   fields.push_back ( pos );
}

void FieldAddressing::setPolygon ( const Poly_gon& poly_gon )
{
   addressingMode = poly;
   polygons.clear();
   polygons.push_back ( poly_gon );
}

void FieldAddressing::setGlobal()
{
   addressingMode = global;
}



void FieldAddressing::readMapModificationData ( tnstream& stream )
{
   versionTest(stream,1000,1000);
   addressingMode = AddressingMode ( stream.readInt() );
   if ( addressingMode == singleField )
      readClassContainer( fields, stream );

   if ( addressingMode == poly )
      readClassContainer( polygons, stream );
}


void FieldAddressing::setup ()
{
   vector<ASCString> entries;
   entries.push_back ( "none" );
   entries.push_back ( "Single Fields");
   entries.push_back ( "Polygons" );
   entries.push_back ( "global");

   int c = chooseString ( "choose operation target", entries, addressingMode );
   if ( c < 0 )
      return;

   addressingMode = AddressingMode( c );

   if (  addressingMode == singleField )
      selectFields( fields );
   else
      if ( addressingMode == poly ) {
         if ( !polygons.size() )
            polygons.push_back ( Poly_gon() );
         editpolygon ( polygons[0] );
      }
}


void FieldAddressing::writeMapModificationData ( tnstream& stream )
{
   stream.writeInt( 1000 );
   stream.writeInt ( addressingMode );

   if ( addressingMode == singleField )
      writeClassContainer ( fields, stream );

   if ( addressingMode == poly )
      writeClassContainer ( polygons, stream );
}

void MapModificationEvent::execute( MapDisplayInterface* md )
{
   operate();
   if ( md ) {
      md->displayMap();
      md->updateDashboard();
   }
}


void WeatherChange :: readData ( tnstream& stream )
{
   versionTest(stream,1,1);
   weather = stream.readInt();
   readMapModificationData ( stream );
}


void WeatherChange :: writeData ( tnstream& stream )
{
   stream.writeInt( 1 );
   stream.writeInt( weather );
   writeMapModificationData ( stream );
}

void WeatherChange :: fieldOperator( const MapCoordinate& mc )
{
   tfield* field = gamemap->getField ( mc );
   if ( field ) {
     if ( field->typ->terraintype->weather[ weather ] )
        field->typ = field->typ->terraintype->weather[ weather ];
     else
        field->typ = field->typ->terraintype->weather[ 0 ];

     field->setparams();
   }
}

void WeatherChange :: setup ()
{
  FieldAddressing::setup();
  chooseWeather( weather );
}


void MapChange :: readData ( tnstream& stream )
{
   versionTest(stream,1,1);
   terrainID = stream.readInt();
   readMapModificationData ( stream );
}


void MapChange :: writeData ( tnstream& stream )
{
   stream.writeInt( 1 );
   stream.writeInt( terrainID );
   writeMapModificationData ( stream );
}

void MapChange :: fieldOperator( const MapCoordinate& mc )
{
   TerrainType* typ = terrainTypeRepository.getObject_byID ( terrainID );
   if ( !typ )
      return;

   tfield* field = gamemap->getField ( mc );
   if ( field ) {
      int w = field->getweather();
      if (typ->weather[w] == NULL)
         w = 0;

      field->typ = typ->weather[ w ];
      field->setparams();
   }
}

void MapChange :: setup ()
{
  FieldAddressing::setup();
  chooseTerrain( terrainID );
}



void AddObject :: readData ( tnstream& stream )
{
   versionTest(stream,1,1);
   objectID = stream.readInt();
   readMapModificationData ( stream );
}


void AddObject :: writeData ( tnstream& stream )
{
   stream.writeInt( 1 );
   stream.writeInt( objectID );
   writeMapModificationData ( stream );
}


void AddObject :: fieldOperator( const MapCoordinate& mc )
{
   ObjectType* obj = objectTypeRepository.getObject_byID ( objectID );
   if ( !obj )
      return;

   tfield* field = gamemap->getField ( mc );
   if ( field ) {
      field->addobject ( obj, -1, true );
      field->setparams();
    }
}

void AddObject :: setup ()
{
  FieldAddressing::setup();
  chooseObject( objectID );
}




void RemoveAllObjects :: readData ( tnstream& stream )
{
   versionTest(stream,1,1);
   readMapModificationData ( stream );
}


void RemoveAllObjects :: writeData ( tnstream& stream )
{
   stream.writeInt( 1 );
   writeMapModificationData ( stream );
}


void RemoveAllObjects :: fieldOperator( const MapCoordinate& mc )
{
   tfield* field = gamemap->getField ( mc );
   if ( field ) {
      field->objects.clear();
      field->setparams();
    }
}

void RemoveAllObjects :: setup ()
{
  FieldAddressing::setup();
}



void MapChangeCompleted :: execute( MapDisplayInterface* md )
{
   checkobjectsforremoval();
   checkunitsforremoval ();

   if ( md ) {
      md->displayMap();
      md->updateDashboard();
   }

}


void ChangeBuildingDamage::readData ( tnstream& stream )
{
   versionTest(stream,1,1);
   damage = stream.readInt();
   position.read ( stream );
}

void ChangeBuildingDamage::writeData ( tnstream& stream )
{
   stream.writeInt( 1);
   stream.writeInt( damage );
   position.write ( stream );
}

void ChangeBuildingDamage::execute( MapDisplayInterface* md )
{
   tfield* fld = gamemap->getField ( position );
   if ( fld && fld->building ) {
      if ( damage >= 100 ) {
         delete fld->building;
         fld->building = NULL;

         if ( md ) {
           md->displayMap();
           md->updateDashboard();
         }
      } else {
         fld->building->damage  = damage;
         if ( md )
           md->updateDashboard();
      }
   }
}

void ChangeBuildingDamage::setup()
{
   getxy_building ( &position.x, &position.y );
   damage = editInt("new building damage", damage, 0, 100);
}


void NextMap::readData ( tnstream& stream )
{
   versionTest(stream,1,1);
   mapID = stream.readInt();
}

void NextMap::writeData ( tnstream& stream )
{
   stream.writeInt( 1);
   stream.writeInt( mapID );
}

void NextMap::execute( MapDisplayInterface* md )
{
   if ( gamemap->state == GameMap::Replay )
      return;

   ASCString name = gamemap->preferredFileNames.mapname[0];
   if ( name.find('.') != ASCString::npos )
      name.erase( name.find('.') );

   
   savegame( "map-" + name + "-completed" + (savegameextension + 1) );
   throw  LoadNextMap(mapID);
}

void NextMap::setup()
{
   mapID = editInt("ID of next map", mapID, 0, maxint);
}



void LoseMap::execute( MapDisplayInterface* md )
{
   if ( gamemap->state == GameMap::Replay )
      return;

   if ( !gamemap->continueplaying ) {
      displaymessage ( "You have been defeated !", 3 );
      delete gamemap;
      gamemap = NULL;
      throw NoMapLoaded();
   }
}


void DisplayEllipse::readData ( tnstream& stream )
{
   versionTest(stream,1,1);
   x1 = stream.readInt();
   x2 = stream.readInt();
   y1 = stream.readInt();
   y2 = stream.readInt();
   alignRight = stream.readInt();
   alignBottom = stream.readInt();
}

void DisplayEllipse::writeData ( tnstream& stream )
{
   stream.writeInt( 1 );
   stream.writeInt( x1 );
   stream.writeInt( x2 );
   stream.writeInt( y1 );
   stream.writeInt( y2 );
   stream.writeInt( alignRight );
   stream.writeInt( alignBottom );
}

void DisplayEllipse::execute( MapDisplayInterface* md )
{
   warning("Ellipses are not supported any more!");
   /*
   if ( !gamemap->ellipse )
      gamemap->ellipse = new EllipseOnScreen;

   if ( alignRight ) {     // x orientation
      gamemap->ellipse->x1 = agmp->resolutionx - ( 640 - x1 );
      gamemap->ellipse->x2 = agmp->resolutionx - ( 640 - x2 );
   } else {
      gamemap->ellipse->x1 = x1;
      gamemap->ellipse->x2 = x2;
   }

   if ( alignBottom ) {     // y orientation
      gamemap->ellipse->y1 = agmp->resolutiony - ( 480 - y1 );
      gamemap->ellipse->y2 = agmp->resolutiony - ( 480 - y2 );
   } else {
      gamemap->ellipse->y1 = y1;
      gamemap->ellipse->y2 = y2;
   }

   gamemap->ellipse->color = white;
   gamemap->ellipse->precision = 0.15;
   gamemap->ellipse->active = 1;

   if ( md )
      md->updateDashboard();

   */
}

void DisplayEllipse::setup()
{
   x1 = getid("x1",x1,0,639);
   y1 = getid("y1",y1,0,479);
   x2 = getid("x2",x2,0,639);
   y2 = getid("y2",y2,0,479);
   alignRight = choice_dlg("x orientation","~l~eft","~r~ight") - 1;
   alignBottom = choice_dlg("y orientation","~t~op","~b~ottom") - 1;
}


void RemoveEllipse::execute( MapDisplayInterface* md )
{
   /*
   if ( gamemap->ellipse ) {
      gamemap->ellipse->active = 0;
      if ( md )
         md->repaintDisplay();
   }
   */
}




void ChangeBuildingOwner :: readData ( tnstream& stream )
{
   versionTest(stream,1,1);
   pos.read( stream );
   newOwner = stream.readInt();
}


void ChangeBuildingOwner :: writeData ( tnstream& stream )
{
   stream.writeInt( 1 );
   pos.write( stream );
   stream.writeInt ( newOwner );
}


void ChangeBuildingOwner :: setup ()
{
   getxy_building( &pos.x, &pos.y );
   newOwner = getid("new owner",newOwner,0,8);
}

void ChangeBuildingOwner :: execute( MapDisplayInterface* md )
{
   tfield* fld = gamemap->getField ( pos );
   if ( fld && fld->building ) {
      fld->building->convert ( newOwner );
      #ifdef sgmain
      evaluateviewcalculation ( gamemap );
      #endif
      if ( md ) {
         md->displayMap();
         md->updateDashboard();
      }
   }
}


void DisplayImmediateMessage::execute( MapDisplayInterface* md )
{
   if ( gamemap->state == GameMap::Replay )
      return;

   if ( !message.empty() ) {
      if ( recipients == 0 )
         new Message ( message, gamemap, 1 << gamemap->actplayer, 0 );
      else
         new Message ( message, gamemap, recipients, 0 );

      #ifdef sgmain
      viewunreadmessages ( gamemap->player[ gamemap->actplayer ] );
      #endif
   }
}

void DisplayImmediateMessage::readData( tnstream& stream )
{
   int version = versionTest(stream,1,2);
   message = stream.readString(true);
   if ( version >= 2 )
      recipients = stream.readInt();
   else
      recipients = 0;
}

void DisplayImmediateMessage::writeData( tnstream& stream )
{
   stream.writeInt(2);
   stream.writeString( message );
   stream.writeInt( recipients );
}

void DisplayImmediateMessage::setup()
{

   vector<ASCString> player;
   for ( int p = 0; p < gamemap->getPlayerCount(); ++p )
      player.push_back( gamemap->getPlayer(p).getName() );

   BitMapEditor bme (recipients, "Select recieving players", player );
   bme.Show();
   bme.RunModal();


   while ( message.find ( "#CRT#" ) != ASCString::npos )
      message.replace ( message.find ( "#CRT#" ), 5, "\n" );
   while ( message.find ( "#crt#" ) != ASCString::npos )
      message.replace ( message.find ( "#crt#" ), 5, "\n" );
   MultiLineEditor( "Message", message );
}




void AddProductionCapability :: readData ( tnstream& stream )
{
   versionTest(stream,1,1);
   pos.read( stream );
   vehicleTypeID = stream.readInt();
}


void AddProductionCapability :: writeData ( tnstream& stream )
{
   stream.writeInt( 1 );
   pos.write( stream );
   stream.writeInt ( vehicleTypeID );
}


void AddProductionCapability :: setup ()
{
   getxy_building( &pos.x, &pos.y );
   chooseVehicleType( vehicleTypeID );
}

void AddProductionCapability :: execute( MapDisplayInterface* md )
{
   tfield* fld = gamemap->getField ( pos );
   if ( fld && fld->building && vehicleTypeID >= 0 )
      fld->building->addProductionLine( gamemap->getvehicletype_byid(vehicleTypeID) );
   
}



void ChangeDiplomaticStatus :: readData ( tnstream& stream )
{
   versionTest(stream,1,1);
   proposingPlayer = stream.readInt();
   targetPlayer = stream.readInt();
   proposal = Proposal(stream.readInt());
}


void ChangeDiplomaticStatus :: writeData ( tnstream& stream )
{
   stream.writeInt( 1 );
   stream.writeInt ( proposingPlayer );
   stream.writeInt ( targetPlayer );
   stream.writeInt ( proposal );
}


void ChangeDiplomaticStatus :: setup ()
{
   proposingPlayer = editInt("Announcing Player",proposingPlayer,0,7);
   targetPlayer = editInt("Receiving Player",targetPlayer,0,7);

   vector<ASCString> list;
   list.push_back ( "Propose Peace");
   list.push_back ( "Declare War");
   list.push_back ( "Sneak Attack");

   proposal = Proposal ( chooseString ( "Action", list, proposal ));
}

void ChangeDiplomaticStatus :: execute( MapDisplayInterface* md )
{
   if ( proposal == Peace )
      gamemap->getPlayer(proposingPlayer).diplomacy.propose( targetPlayer, PEACE );
   else
      if ( proposal == War )
         gamemap->getPlayer(proposingPlayer).diplomacy.propose( targetPlayer, WAR );
      else {
         gamemap->getPlayer(proposingPlayer).diplomacy.setState( targetPlayer, WAR );
      }
}






class ChangePlayerState : public EventAction {
      int player;
      Player::PlayerStatus state;
    public:
       ChangePlayerState(): EventAction( EventAction_ChangePlayerState), player(0), state (Player::off){};

         void readData ( tnstream& stream )
         {
            versionTest(stream,1,1);
            player = stream.readInt();
            state = (Player::PlayerStatus) stream.readInt();
         }


         void writeData ( tnstream& stream )
         {
            stream.writeInt( 1 );
            stream.writeInt ( player );
            stream.writeInt ( state );
         }

      void execute( MapDisplayInterface* md )
      {
         gamemap->getPlayer(player).stat  = state;
      }

      void setup()
      {
         player = editInt("Player",player,0,7);

         vector<ASCString> list;
         list.push_back ( "human");
         list.push_back ( "computer");
         list.push_back ( "off");
         list.push_back ( "supervisor");
         list.push_back ( "suspended");

         do {
            state = Player::PlayerStatus( chooseString ( "State", list, state ));
            if ( state == Player::supervisor )
               warning("setting a player to supervisor is not allowed");
         } while ( state == Player::supervisor );

      }

      ASCString getName() const { return "Change player state"; };
      
};





void SetViewSharing :: readData ( tnstream& stream )
{
   versionTest(stream,1,1);
   viewingPlayer = stream.readInt();
   providingPlayer = stream.readInt();
   enable = stream.readInt();
}


void SetViewSharing :: writeData ( tnstream& stream )
{
   stream.writeInt( 1 );
   stream.writeInt ( viewingPlayer );
   stream.writeInt ( providingPlayer );
   stream.writeInt ( enable );
}


void SetViewSharing :: setup ()
{
   viewingPlayer = editInt("Viewing Player",viewingPlayer,0,7);
   providingPlayer = editInt("Providing Player",providingPlayer,0,7);

   vector<ASCString> list;
   list.push_back ( "Disable View Sharing");
   list.push_back ( "Enable View Sharing");

   enable = chooseString ( "Action", list, enable );
}

void SetViewSharing :: execute( MapDisplayInterface* md )
{
   if ( enable )
      gamemap->player[providingPlayer].diplomacy.setState( viewingPlayer, PEACE_SV );
   else
      if ( gamemap->player[providingPlayer].diplomacy.getState( viewingPlayer ) >= PEACE_SV )
         gamemap->player[providingPlayer].diplomacy.setState( viewingPlayer, PEACE );
         
   #ifdef sgmain
   computeview( gamemap );
   #endif
   if ( md ) {
      md->displayMap();
      md->updateDashboard();
   }
}




void AddResources :: readData ( tnstream& stream )
{
   versionTest(stream,1,1);
   res.read( stream );
   pos.read( stream );
}


void AddResources :: writeData ( tnstream& stream )
{
   stream.writeInt( 1 );
   res.write( stream );
   pos.write( stream );
}


void AddResources :: setup ()
{
   getxy_building( &pos.x, &pos.y );

   res.energy   = editInt("Energy",res.energy,minint);
   res.material = editInt("Material",res.material,minint);
   res.fuel     = editInt("Fuel",res.fuel ,minint);
}

void AddResources :: execute( MapDisplayInterface* md )
{
   tfield* fld = gamemap->getField ( pos );
   if ( fld && fld->building )
      fld->building->putResource( res, 0 );
}


void Reinforcements :: readData ( tnstream& stream )
{
   versionTest(stream,1,1);
   objectNum = stream.readInt();
   buf.readfromstream ( &stream );
}


void Reinforcements :: writeData ( tnstream& stream )
{
   stream.writeInt( 1 );
   stream.writeInt( objectNum );
   buf.writetostream ( &stream );
}


void Reinforcements :: setup ()
{
   vector<ASCString> list;
   list.push_back ( "Append units / buildings");
   list.push_back ( "Reposition and reselect units / buildings");
   list.push_back ( "Clear buffer and select other units / buildings");

   int i = chooseString ( "Choose Action", list, 0 );
   if ( i == 2 ){
      buf.clear();
      objectNum = 0;
   }
   if ( i == 1 ){
      execute( NULL );
      buf.clear();
      objectNum = 0;
   }

   ReinforcementSelector::CoordinateList fieldlist;
   ReinforcementSelector rs( fieldlist, gamemap, buf, objectNum );
   rs.Show();
   rs.RunModal();


}

class FindUnitPlacementPos : public SearchFields {
      Vehicle* vehicle;
   public:
      FindUnitPlacementPos ( GameMap* gamemap, Vehicle* veh )
        : SearchFields ( gamemap ), vehicle(veh)
        {
           initsearch(MapCoordinate(veh->xpos, veh->ypos),0,10);
           startsearch();
        };

      void testfield ( const MapCoordinate& pos )
      {
         tfield* fld = gamemap->getField( pos );
         if ( fld && !fld->vehicle ) {
            if ( fieldAccessible( fld, vehicle, -2, NULL, true ) == 2 ) {
               fld->vehicle = vehicle;
               fld->vehicle->setnewposition ( pos );
               fld->vehicle->addview();
               cancelSearch = true;
            }
         }
      };
};


void Reinforcements :: execute( MapDisplayInterface* md )
{
   int cnt = 0;
   tmemorystream stream ( &buf, tnstream::reading );
   while ( cnt < objectNum ) {
      Type type = Type(stream.readInt());
      if ( type == ReinfVehicle ) {
         try {
         Vehicle* veh = Vehicle::newFromStream( gamemap, stream, ++gamemap->unitnetworkid );
         FindUnitPlacementPos fupp( gamemap, veh );
         } 
         catch ( InvalidID err ) {
            displaymessage( "Error executing event 'Reinforcements'\n" +  err.getMessage(), 1);
            throw ShutDownMap();
         }
      }
      if ( type == ReinfBuilding ) {
        Building* bld = Building::newFromStream ( gamemap, stream );
        #if 0
        MapCoordinate pos = bld->getPosition();

        for ( int x = 0; x < 4; x++ )
           for ( int y = 0; y < 6; y++ )
              if ( bld->typ->getpicture ( BuildingType::LocalCoordinate( x , y ) )) {
                 tfield* field = gamemap->getField( bld->typ->getFieldCoordinate( pos, BuildingType::LocalCoordinate( x, y) ));
                 if ( !field ) {
                    delete bld;
                    // displaymessage("building does not fit here", 1 );
                    return;
                 }

                 /*
                 if ( !bld->typ->terrainaccess.accessible ( field->bdt ) ) {
                    delete bld;
                    displaymessage("building does can not be build here", 1 );
                    return;
                 }
                 */


                 if ( field->vehicle ) {
                    delete field->vehicle;
                    field->vehicle = NULL;
                 }
                 if ( field->building ) {
                    delete field->building;
                    field->building = NULL;
                 }
              }
         bld->chainbuildingtofield( pos );
         #endif
         gamemap->calculateAllObjects();
         bld->addview();
      }
      ++cnt;
   }
   #ifdef sgmain
   evaluateviewcalculation ( gamemap );
   // computeview ( gamemap );

   #endif

   if ( md ) {
      md->displayMap();
      md->updateDashboard();
   }
}

template<class T>
ASCString TriggerNameProvider() {
   T t;
   return t.getName();
};



template <typename TriggerType > 
bool registerTrigger( EventTrigger_ID id )
{
   return triggerFactory::Instance().registerClass( id, ObjectCreator<EventTrigger, TriggerType>,  TriggerNameProvider<TriggerType>() );
}

template <typename ActionType > 
bool registerAction( EventAction_ID id )
{
   return actionFactory::Instance().registerClass( id, ObjectCreator<EventAction, ActionType>,  TriggerNameProvider<ActionType>() );
}




namespace {
   const bool r1 = registerTrigger<TurnPassed>                 ( Trigger_TurnPassed );
   const bool r2 = registerTrigger<UnitLost>                   ( Trigger_UnitLost );
   const bool r3 = registerTrigger<UnitConquered>              ( Trigger_UnitConquered );
   const bool r4 = registerTrigger<UnitDestroyed>              ( Trigger_UnitDestroyed );
   const bool r5 = registerTrigger<AllBuildingsLost>           ( Trigger_AllBuildingsLost );
   const bool r6 = registerTrigger<AllUnitsLost>               ( Trigger_AllUnitsLost );
   const bool r7 = registerTrigger<TriggerNothingFalse>        ( Trigger_NothingFalse );
   const bool r8 = registerTrigger<TriggerNothingTrue>         ( Trigger_NothingTrue );
   const bool r9 = registerTrigger<BuildingConquered>          ( Trigger_BuildingConquered );
   const bool r10 = registerTrigger<BuildingLost>              ( Trigger_BuildingLost );
   const bool r11 = registerTrigger<BuildingDestroyed>         ( Trigger_BuildingDestroyed );
   const bool r12 = registerTrigger<BuildingSeen>              ( Trigger_BuildingSeen );
   const bool r13 = registerTrigger<EventTriggered>            ( Trigger_EventTriggered );
   const bool r14 = registerTrigger<AllEnemyBuildingsDestroyed>( Trigger_AllEnemyBuildingsDestroyed );
   const bool r15 = registerTrigger<AllEnemyUnitsDestroyed>    ( Trigger_AllEnemyUnitsDestroyed );
   const bool r16 = registerTrigger<SpecificUnitEntersPolygon> ( Trigger_SpecificUnitEntersPolygon );
   const bool r17 = registerTrigger<AnyUnitEntersPolygon>      ( Trigger_AnyUnitEntersPolygon );
   const bool r18 = registerTrigger<ResourceTribute>           ( Trigger_ResourceTribute );

   const bool s0 = registerAction<Action_Nothing>( EventAction_Nothing );
   const bool s1 = registerAction<DisplayMessage>( EventAction_DisplayMessage );
   const bool s2 = registerAction<WindChange>( EventAction_WindChange );
   const bool s3 = registerAction<ChangeGameParameter>( EventAction_ChangeGameParameter );
   const bool s4 = registerAction<WeatherChange>( EventAction_WeatherChange );
   const bool s5 = registerAction<MapChange>( EventAction_MapChange );
   const bool s6 = registerAction<AddObject>( EventAction_AddObject );
   const bool s7 = registerAction<MapChangeCompleted>( EventAction_MapChangeCompleted );
   const bool s8 = registerAction<ChangeBuildingDamage>( EventAction_ChangeBuildingDamage );
   const bool s9 = registerAction<NextMap>( EventAction_NextMap );
   const bool s10 = registerAction<LoseMap>( EventAction_LoseMap );
   const bool s11 = registerAction<DisplayEllipse>( EventAction_DisplayEllipse );
   const bool s12 = registerAction<RemoveEllipse>( EventAction_RemoveEllipse );
   const bool s13 = registerAction<ChangeBuildingOwner>( EventAction_ChangeBuildingOwner );
   const bool s14 = registerAction<RemoveAllObjects>( EventAction_RemoveAllObjects );
   const bool s15 = registerAction<DisplayImmediateMessage>( EventAction_DisplayImmediateMessage );
   const bool s16 = registerAction<AddProductionCapability>( EventAction_AddProductionCapabiligy );
   const bool s17 = registerAction<ChangeDiplomaticStatus>( EventAction_ChangeDiplomaticStatus );
   const bool s18 = registerAction<AddResources>( EventAction_AddResources );
   const bool s19 = registerAction<Reinforcements>( EventAction_Reinforcements );
   const bool s20 = registerAction<SetViewSharing>( EventAction_SetViewSharing );
   const bool s21 = registerAction<ChangePlayerState>( EventAction_ChangePlayerState );
};

