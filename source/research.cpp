/***************************************************************************
                          research.cpp  -  description
                             -------------------
    begin                : Tue Jan 23 2001
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/*! \file research.h
    \brief Everything related to research
*/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <cmath>
#include "research.h"
#include "errors.h"
#include "typen.h"
#include "vehicletype.h"
#include "gamemap.h"
#include "itemrepository.h"

const int techDependencyVersion = 2;

void TechDependency::read ( tnstream& stream )
{
   int version = stream.readInt();
   if ( version > techDependencyVersion || version < 1 )
      throw tinvalidversion( stream.getDeviceName(), techDependencyVersion, version );

   requireAllListedTechnologies = stream.readInt();
   readClassContainer(requiredTechnologies, stream );
   if ( version >= 2 )
      readClassContainer(blockingTechnologies, stream );
}

void TechDependency::write ( tnstream& stream ) const
{
   stream.writeInt(techDependencyVersion);
   stream.writeInt(requireAllListedTechnologies);
   writeClassContainer(requiredTechnologies, stream );
   writeClassContainer(blockingTechnologies, stream );
}

void TechDependency::runTextIO ( PropertyContainer& pc )
{
   pc.addIntRangeArray ( "TechnologiesRequired", requiredTechnologies );
   pc.addBool( "RequireAllListedTechnologies", requireAllListedTechnologies, true );
   if ( pc.find( "BlockingTechnologies" ) || !pc.isReading() )
      pc.addIntRangeArray ( "BlockingTechnologies", blockingTechnologies );
}

bool TechDependency::available( const Research& research ) const
{
   for ( vector<IntRange>::const_iterator j = blockingTechnologies.begin(); j != blockingTechnologies.end(); ++j )
       for ( int k = j->from; k <= j->to; ++k )
          if ( research.techResearched ( k ))
             return false;


   if ( requiredTechnologies.size() ) {
      for ( RequiredTechnologies::const_iterator j = blockingTechnologies.begin(); j != blockingTechnologies.end(); ++j )
         for ( int k = j->from; k <= j->to; ++k )
            if ( research.techResearched( k ))
               return false;

      if ( requireAllListedTechnologies ) {
         for ( RequiredTechnologies::const_iterator j = requiredTechnologies.begin(); j != requiredTechnologies.end(); ++j )
            for ( int k = j->from; k <= j->to; ++k )
               if ( !research.techResearched( k ))
                  return false;
          return true;
      } else {
         for ( RequiredTechnologies::const_iterator j = requiredTechnologies.begin(); j != requiredTechnologies.end(); ++j )
            for ( int k = j->from; k <= j->to; ++k )
                if ( research.techResearched ( k ))
                  return true;
         return false;
      }
   }

   return true;
}


int TechDependency::findInheritanceLevel( int id, vector<int>& stack, const ASCString& sourceTechName ) const
{
   for ( RequiredTechnologies::const_iterator j = requiredTechnologies.begin(); j != requiredTechnologies.end(); ++j )
      for ( int k = j->from; k <= j->to; ++k )
         if ( k > 0 ) {
            if ( find( stack.begin(), stack.end(), k ) != stack.end() ) {
               ASCString s = "Cyclic reference for technology \n" + sourceTechName + "\n";
               for ( vector<int>::iterator i = stack.begin(); i != stack.end(); ++i ) {
                  Technology* t = technologyRepository.getObject_byID( *i );
                  if ( t )
                     s += t->name + "\n";
               }
               longWarning ( s );
               return -1;
            }

            Technology* t = technologyRepository.getObject_byID( k );

            if ( t ) {
               // if ( requireAllListedTechnologies  )
                  stack.push_back ( k );

               int i = t->techDependency.findInheritanceLevel ( id, stack, sourceTechName );

               // if ( requireAllListedTechnologies )
                  stack.pop_back();
               if ( i > 0 )
                  return i+1;
            }
         }

   for ( RequiredTechnologies::const_iterator j = requiredTechnologies.begin(); j != requiredTechnologies.end(); ++j )
      for ( int k = j->from; k <= j->to; ++k )
         if ( id == k && k > 0 )
            return 1;

   return -1;
}


void TechDependency::writeTreeOutput ( const ASCString& sourceTechName, tnstream& stream, bool reduce ) const
{
   map<int,int> ihl;
   vector<int> stack;
   for ( RequiredTechnologies::const_iterator j = requiredTechnologies.begin(); j != requiredTechnologies.end(); ++j )
      for ( int k = j->from; k <= j->to; ++k ) {
         ihl[k] = findInheritanceLevel( k , stack, sourceTechName);
      }

   for ( RequiredTechnologies::const_iterator j = requiredTechnologies.begin(); j != requiredTechnologies.end(); ++j )
      for ( int k = j->from; k <= j->to; ++k ) {
         const Technology* t = technologyRepository.getObject_byID( k );
         if ( t && (ihl[k] == 1 || !reduce) ) {
            ASCString s = "\"";
            ASCString stn = sourceTechName;

            while ( stn.find ( "\"" ) != ASCString::npos )
               stn.erase ( stn.find ( "\"" ),1 );

            s += stn;
            s += "\" -> \"";

            ASCString stn2 = t->name;
            while ( stn2.find ( "\"" ) != ASCString::npos )
               stn2.erase ( stn2.find ( "\"" ),1 );

            s += stn2 + "\"";

            if ( !requireAllListedTechnologies )
               s += "[style=dashed]";
               
            s += "\n";
            
            stream.writeString ( s, false );

            stream.writeString ( "\"" + stn + "\" [color=black] \n", false );
         }
      }
}


void TechDependency::writeInvertTreeOutput ( const ASCString& sourceTechName, tnstream& stream, vector<int>& history, const vector<IntRange>* onlyWithBaseTechs ) const
{
   if ( onlyWithBaseTechs && !onlyWithBaseTechs->empty() ) {
      vector<int> inheritanceStack;
      bool found = false;
      for ( vector<IntRange>::const_iterator i = onlyWithBaseTechs->begin(); i != onlyWithBaseTechs->end(); ++i )
         for ( int j = i->from; j <= i->to; ++j )
            if ( findInheritanceLevel ( j, inheritanceStack, "" ) >= 0 )
               found = true;
               
      if ( !found )
         return;
   }

   for ( RequiredTechnologies::const_iterator j = requiredTechnologies.begin(); j != requiredTechnologies.end(); ++j )
      for ( int k = j->from; k <= j->to; ++k ) {
         const Technology* t = technologyRepository.getObject_byID( k );
         if ( t ) {
            ASCString s = "\"";
            ASCString stn = sourceTechName;

            while ( stn.find ( "\"" ) != ASCString::npos )
               stn.erase ( stn.find ( "\"" ),1 );

            s += stn;
            s += "\" -> \"";

            ASCString stn2 = t->name;
            while ( stn2.find ( "\"" ) != ASCString::npos )
               stn2.erase ( stn2.find ( "\"" ),1 );

            s += stn2 + "\"";

            if ( !requireAllListedTechnologies )
               s += "[style=dotted]";

            s += "\n";

            stream.writeString ( s, false );

            stream.writeString ( "\"" + stn + "\" [color=black] \n", false );
            if ( find ( history.begin(), history.end(), t->id) == history.end()) {
               history.push_back ( t->id );
               t->techDependency.writeInvertTreeOutput ( t->name, stream, history );
            }
         }
      }
}



void TechAdapter::read ( tnstream& stream )
{
   int version = stream.readInt();
   if ( version > 1 || version < 1 )
      throw tinvalidversion( stream.getDeviceName(), 1, version );
   name = stream.readString();
   techDependency.read( stream );
}

void TechAdapter::write ( tnstream& stream ) const
{
   stream.writeInt(1);
   stream.writeString( name );
   techDependency.write ( stream );
}

void TechAdapter::runTextIO ( PropertyContainer& pc )
{
   pc.addString ( "Name", name );
   name.toLower();
   techDependency.runTextIO ( pc );
}

bool TechAdapter::available( const Research& research ) const
{
   return techDependency.available ( research );
}




TechAdapterDependency::TechAdapterDependency() : requireAllListedTechAdapter ( false ) {};

bool TechAdapterDependency::available( const Research& research ) const
{
   if ( requiredTechAdapter.size() )
      if ( requireAllListedTechAdapter ) {
          for ( RequiredTechAdapter::const_iterator j = requiredTechAdapter.begin(); j != requiredTechAdapter.end(); ++j )
             if ( !research.techAdapterAvail( *j ))
                return false;
          return true;
      } else {
          for ( RequiredTechAdapter::const_iterator j = requiredTechAdapter.begin(); j != requiredTechAdapter.end(); ++j )
             if ( research.techAdapterAvail ( *j ))
                return true;
         return false;
      }

   return true;
}


TechAdapter :: TechAdapter() {}

void TechAdapterDependency::read ( tnstream& stream )
{
   int version = stream.readInt();
   if ( version > 1 || version < 1 )
      throw tinvalidversion( stream.getDeviceName(), 1, version );
   requireAllListedTechAdapter = stream.readInt();
   requiredTechAdapter.clear();
   for ( int i = stream.readInt(); i >0 ; --i )
      requiredTechAdapter.push_back ( stream.readString() );
}

void TechAdapterDependency::write ( tnstream& stream ) const
{
   stream.writeInt(1);
   stream.writeInt(requireAllListedTechAdapter);
   stream.writeInt( requiredTechAdapter.size() );
   for ( RequiredTechAdapter::const_iterator i = requiredTechAdapter.begin(); i != requiredTechAdapter.end(); ++i )
       stream.writeString(*i);
}

void TechAdapterDependency::runTextIO ( PropertyContainer& pc, const ASCString& defaultTechAdapter )
{
   if ( pc.find( "TechAdapterRequired") || !pc.isReading() ) {
      pc.addStringArray ( "TechAdapterRequired", requiredTechAdapter );
      if ( pc.isReading())
         for ( RequiredTechAdapter::iterator j = requiredTechAdapter.begin(); j != requiredTechAdapter.end(); ++j )
            j->toLower();

   } else
      if ( defaultTechAdapter.length() > 0 )
         requiredTechAdapter.push_back ( defaultTechAdapter );

   pc.addBool( "RequireAllListedTechAdapter", requireAllListedTechAdapter, true );
}

void TechAdapterDependency::writeInvertTreeOutput ( const ASCString& sourceTechName, tnstream& stream, const vector<IntRange>* onlyWithBaseTechs ) const
{
   vector<int> history;
   for ( RequiredTechAdapter::const_iterator r = requiredTechAdapter.begin(); r != requiredTechAdapter.end(); ++r )
      for ( TechAdapterContainer::iterator i = techAdapterContainer.begin(); i != techAdapterContainer.end(); ++i )
         if ( *r == (*i)->getName() )
            (*i)->techDependency.writeInvertTreeOutput ( sourceTechName, stream, history, onlyWithBaseTechs );
}



Technology::Technology()
{
   icon = NULL;
   id = 0;
   researchpoints = 0;
   requireEvent = false;
   techlevel = 0;
}

const int technologyVersion = 3;

void Technology::read( tnstream& stream )
{
   int version = stream.readInt();
   if ( version > technologyVersion || version < 1 )
      throw tinvalidversion( stream.getDeviceName(), technologyVersion, version );
   id = stream.readInt();
   researchpoints = stream.readInt();
   requireEvent = stream.readInt();
   techlevel = stream.readInt();
   name = stream.readString();
   infotext = stream.readString();
   techDependency.read( stream );
   if ( version >= 2 )
      relatedUnitID = stream.readInt();
   else
      relatedUnitID = -1;

   if ( version >= 3 )
      readClassContainer( blockingOtherTechnologies, stream );

}

void Technology::write( tnstream& stream ) const
{
   stream.writeInt( technologyVersion );
   stream.writeInt ( id );
   stream.writeInt( researchpoints );
   stream.writeInt( requireEvent );
   stream.writeInt( techlevel );
   stream.writeString( name );
   stream.writeString( infotext );
   techDependency.write( stream );
   stream.writeInt ( relatedUnitID );
   writeClassContainer( blockingOtherTechnologies, stream );
}

void Technology::runTextIO ( PropertyContainer& pc )
{
   pc.addString( "Name", name );
   pc.addString( "Infotext", infotext);

   pc.addInteger( "Id", id );
   pc.addInteger( "Researchpoints", researchpoints );

   pc.addInteger( "Techlevel", techlevel );
   pc.addBool( "RequireEvent", requireEvent, false );

   techDependency.runTextIO( pc );

   if ( pc.find ( "BlockingOtherTechnologies" ) || !pc.isReading() )
      pc.addIntRangeArray ( "BlockingOtherTechnologies", blockingOtherTechnologies );


   pc.addInteger( "RelatedUnitID", relatedUnitID, 0 );
}


Research :: Research ( )
{
   progress = 0;
   activetechnology = 0;
   ___loadActiveTech = 0;
   ___oldVersionLoader = false;
   techsAvail = true;
}


void Research :: read ( tnstream& stream )
{
   read_struct ( stream );
   read_techs ( stream );
   evalTechAdapter();
}


const int researchableWeaponImprovements = 8;


const int researchVersion = -3;

void Research :: read_struct ( tnstream& stream )
{
   int version = stream.readInt();
   
   if ( version < researchVersion )
      throw tinvalidversion( stream.getLocation(), -researchVersion,  -version );

   if ( version >= 0 ) {
      progress = version;
      ___loadActiveTech = stream.readInt();

      for ( int i = 0; i < researchableWeaponImprovements; i++ )
         stream.readWord(); // unitimprovement.weapons[i] =
      stream.readWord(); // unitimprovement.armor =

      for ( int j = 0; j < 44-researchableWeaponImprovements*2; j++ )
          stream.readChar(); // dummy

      stream.readInt(); // techlevel =
      stream.readInt(); // __loader_techsAvail =
      ___oldVersionLoader = true;
      techsAvail = true;
   } else {
      progress = stream.readInt();
      activetechnology = technologyRepository.getObject_byID( stream.readInt());
      int size = stream.readInt();
      for ( int i = 0; i < size; ++i )
         developedTechnologies.push_back ( stream.readInt());
      ___oldVersionLoader = false;

      if ( version <= -2 )
         techsAvail = stream.readInt();
      else
         techsAvail = true;

      if ( version <= -3 )
         readClassContainer( predefinedTechAdapter, stream );
   }
}

void Research :: write ( tnstream& stream ) {
   stream.writeInt ( researchVersion );
   stream.writeInt( progress );
   stream.writeInt ( activetechnology ? activetechnology->id : 0);
   stream.writeInt( developedTechnologies.size());
   for ( int i = 0; i < developedTechnologies.size(); ++i )
       stream.writeInt( developedTechnologies[i] );

   stream.writeInt ( techsAvail );
   writeClassContainer( predefinedTechAdapter, stream );
}


void Research :: read_techs ( tnstream& stream )
{
   if ( !___oldVersionLoader )
      return;

   developedTechnologies.clear();

   int w = stream.readInt ();

   while ( w ) {
      const Technology* tec = technologyRepository.getObject_byID( w );
      if ( !tec )
         throw InvalidID ( "technology", w );

      developedTechnologies.push_back(tec->id);

      w = stream.readInt();
   } /* endwhile */

   if ( ___loadActiveTech ) {
      w = stream.readInt ();

      activetechnology = technologyRepository.getObject_byID( w );

      if ( !activetechnology )
         throw InvalidID ( "technology", w );
   }
}


bool Research::techResearched ( int id ) const
{
   for ( vector<int>::const_iterator i = developedTechnologies.begin(); i != developedTechnologies.end(); ++i )
      if ( *i == id )
         return true;
   return false;
}


bool Research::techAdapterAvail( const ASCString& ta ) const
{
   if( find ( predefinedTechAdapter.begin(), predefinedTechAdapter.end(), ta ) != predefinedTechAdapter.end() )
      return true; 

   return triggeredTechAdapter.find ( ta ) != triggeredTechAdapter.end();
}

void Research::evalTechAdapter()
{
   for ( TechAdapterContainer::iterator i = techAdapterContainer.begin(); i != techAdapterContainer.end(); ++i )
      if ( (*i)->available ( *this ))
         triggeredTechAdapter[(*i)->getName()] = true;
}


ASCString Research::listTriggeredTechAdapter() const
{
   ASCString s;
   for ( TriggeredTechAdapter::const_iterator i = triggeredTechAdapter.begin(); i != triggeredTechAdapter.end(); ++i )
      s += i->first + "\n";
   
   return s;
}



Research::AvailabilityStatus Research::techAvailable ( const Technology* tech )
{
   if ( techResearched( tech->id ))
      return researched;

   if ( tech->techDependency.available( *this ) ) {
      for ( int i = 0; i < developedTechnologies.size(); ++i ) {
         Technology* t = technologyRepository.getObject_byID( developedTechnologies[i] );
         if ( t && !t->techDependency.available ( *this ) ) {
            // this is a root technology
            for ( Technology::BlockingOtherTechnologies::iterator j = t->blockingOtherTechnologies.begin(); j != t->blockingOtherTechnologies.end(); ++j)
                if ( j->from <= tech->id && tech->id <= j->to )
                    return unavailable;
         }
      }

      return available;
   } else
      return unavailable;

/*
    if ( tech->requireevent)
       if ( ! actmap->eventpassed(tech->id, cenewtechnologyresearchable) )
          fail = true;
*/
}


void Research :: addanytechnology ( const Technology* tech )
{
   if ( tech ) {
      developedTechnologies.push_back ( tech->id );

      map->player[player].queuedEvents++;
      evalTechAdapter();
   }
}

void Research :: addtechnology ( void )
{
   if ( activetechnology )
      addanytechnology ( activetechnology );

   activetechnology = 0;
}

void Research :: settechlevel ( int techlevel )
{
   if ( techlevel > 0 ) {
      for ( int j = 0; j < technologyRepository.getNum(); j++ ) {
         const Technology* tech = technologyRepository.getObject_byPos ( j );
         if ( tech )
            if ( tech->techlevel <= techlevel )
               if ( !techResearched ( tech->id ))
                  addanytechnology ( tech );
      }
   }
   evalTechAdapter();
}








bool Research :: vehicletypeavailable ( const Vehicletype* fztyp )
{
   if ( !fztyp )
      return false;
   else
      return true; // vehicleclassavailable( fztyp, 0 );
}

/*
void Research::initchoosentechnology()
{
   Player& player = map->player[map->actplayer];
   player.research.progress = 0;

   Player::DissectionContainer::iterator di = player.dissections.begin();
   while ( di != player.dissections.end() ) {
      if ( di->tech->id == player.research.activetechnology->id ) {
         player.research.progress += di->points;
         di = player.dissections.erase ( di );
      } else
         di++;
   }
}
*/

Research::~Research () {};



Resources returnResourcenUseForResearch ( const pbuilding bld, int research )
{
   Resources res;
   if ( bld->typ->nominalresearchpoints == 0 )
      return res;

   for ( int r = 0; r < 3; ++r )
      if ( bld->typ->maxplus.resource(r) < 0 ) {
         float a = -bld->typ->maxplus.resource(r) / pow(double(bld->typ->nominalresearchpoints),2);
         res.resource(r) = int(pow(double(research),2) * a );
      }

   return res;
}

#if 0
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
#endif

