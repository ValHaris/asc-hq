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
 

void TechDependency::read ( tnstream& stream )
{
   stream.readInt();
   requireAllListedTechnologies = stream.readInt();
   readClassContainer(requiredTechnologies, stream );
}

void TechDependency::write ( tnstream& stream ) const
{
   stream.writeInt(1);
   stream.writeInt(requireAllListedTechnologies);
   writeClassContainer(requiredTechnologies, stream );
}

void TechDependency::runTextIO ( PropertyContainer& pc )
{
   pc.addIntRangeArray ( "TechnologiesRequired", requiredTechnologies );
   pc.addBool( "RequireAllListedTechnologies", requireAllListedTechnologies, true );
}

bool TechDependency::available( const Research& research ) const
{
   if ( requiredTechnologies.size() )
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

   return true;
}





void TechAdapter::read ( tnstream& stream )
{
   stream.readInt();
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
   stream.readInt();
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
   if ( pc.find( "TechAdapterRequired")) {
      pc.addStringArray ( "TechAdapterRequired", requiredTechAdapter );
      if ( pc.isReading())
         for ( RequiredTechAdapter::iterator j = requiredTechAdapter.begin(); j != requiredTechAdapter.end(); ++j )
            j->toLower();

   } else
      if ( defaultTechAdapter.length() > 0 )
         requiredTechAdapter.push_back ( defaultTechAdapter );

   pc.addBool( "RequireAllListedTechAdapter", requireAllListedTechAdapter, true );
}




Technology::Technology()
{
   icon = NULL;
   id = 0;
   researchpoints = 0;
   requireEvent = false;
   techlevel = 0;
}


void Technology::read( tnstream& stream )
{
   int version = stream.readInt();
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
}

void Technology::write( tnstream& stream ) const
{
   stream.writeInt( 2 );
   stream.writeInt ( id );
   stream.writeInt( researchpoints );
   stream.writeInt( requireEvent );
   stream.writeInt( techlevel );
   stream.writeString( name );
   stream.writeString( infotext );
   techDependency.write( stream );
   stream.writeInt ( relatedUnitID );
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

   if ( pc.find ( "BlockingTechnologies" ) || !pc.isReading() )
      pc.addIntRangeArray ( "BlockingTechnologies", blockingTechnologies );


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


const int researchVersion = -2;

void Research :: read_struct ( tnstream& stream )
{
   int version = stream.readInt();
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
   // writeContainer( developedTechnologies, stream );
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
   return triggeredTechAdapter.find ( ta ) != triggeredTechAdapter.end();
}

void Research::evalTechAdapter()
{
   for ( TechAdapterContainer::iterator i = techAdapterContainer.begin(); i != techAdapterContainer.end(); ++i )
      if ( (*i)->available ( *this ))
         triggeredTechAdapter[(*i)->getName()] = true;
}


Research::AvailabilityStatus Research::techAvailable ( const Technology* tech )
{
   if ( techResearched( tech->id ))
      return researched;

   if ( tech->techDependency.available( *this ) ) {
      for ( vector<IntRange>::const_iterator j = tech->blockingTechnologies.begin(); j != tech->blockingTechnologies.end(); ++j )
          for ( int k = j->from; k <= j->to; ++k )
             if ( techResearched ( k ))
                return unavailable;

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

