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


#include "research.h"
#include "errors.h"
#include "typen.h"
#include "vehicletype.h"

int  ttechnology :: getlvl( void )
{
   if ( lvl == -1 ) {
      lvl = 0;
      for (int l = 0; l <= 5; l++)
         if ( requiretechnology[l] )
            lvl += requiretechnology[l]->getlvl();
   }
   return lvl;
}


tresearch :: tresearch ( )
{
   progress = 0;
   activetechnology = NULL;
   techlevel = 0;
}


void tresearch :: read ( tnstream& stream )
{
   read_struct ( stream );
   read_techs ( stream );
}

void tresearch :: write ( tnstream& stream )
{
   write_struct ( stream );
   write_techs ( stream );
}



void tresearch :: read_struct ( tnstream& stream ) {
   progress = stream.readInt();
   activetechnology = (ptechnology) stream.readInt();

   for ( int i = 0; i < researchableWeaponImprovements; i++ )
      unitimprovement.weapons[i] = stream.readWord();
   unitimprovement.armor = stream.readWord();
   for ( int j = 0; j < 44-researchableWeaponImprovements*2; j++ )
       stream.readChar(); // dummy

   techlevel = stream.readInt();

   __loader_techsAvail = stream.readInt();

}

void tresearch :: write_struct ( tnstream& stream ) {
   stream.writeInt( progress );
   if ( activetechnology )
      stream.writeInt( 1 );
   else
      stream.writeInt ( 0 );

   for ( int i = 0; i < researchableWeaponImprovements; i++ )
      stream.writeWord ( unitimprovement.weapons[i] );

   stream.writeWord ( unitimprovement.armor );
   for ( int j = 0; j < 44-researchableWeaponImprovements*2; j++ )
       stream.writeChar( 0 ); // dummy

   stream.writeInt ( techlevel );
   if ( developedTechnologies.empty() )
      stream.writeInt ( 0 );
   else
      stream.writeInt ( 1 );
}


void  tresearch :: write_techs ( tnstream& stream )
{
   for ( DevelopedTechnologies::iterator i = developedTechnologies.begin(); i != developedTechnologies.end(); i++ )
      stream.writeInt ( i->first );

   stream.writeInt ( 0 );

   if ( activetechnology )
      stream.writeInt ( activetechnology->id );
}

void tresearch :: read_techs ( tnstream& stream )
{
   developedTechnologies.clear();

   int w = stream.readInt ();

   while ( w ) {
      ptechnology tec = map->gettechnology_byid ( w );
      if ( !tec )
         throw InvalidID ( "technology", w );

      developedTechnologies[w] = tec;

      w = stream.readInt();
   } /* endwhile */

   if ( activetechnology ) {
      w = stream.readInt ();

      activetechnology = map->gettechnology_byid ( w );

      if ( !activetechnology )
         throw InvalidID ( "technology", w );
   }
}




void tresearch :: addanytechnology ( const ptechnology tech )
{
   if ( tech ) {
      developedTechnologies[tech->id] = tech;

      for ( int i = 0; i < researchableWeaponImprovements; i++)
         unitimprovement.weapons[i] = unitimprovement.weapons[i] * tech->unitimprovement.weapons[i] / 1024;
      unitimprovement.armor = unitimprovement.armor * tech->unitimprovement.armor / 1024;

      if ( tech->techlevelset )
         settechlevel ( tech->techlevelset );

      map->player[player].queuedEvents++;
   }
}

void tresearch :: addtechnology ( void )
{
   if ( activetechnology )
      addanytechnology ( activetechnology );

   activetechnology = NULL;
}

void tresearch :: settechlevel ( int _techlevel )
{
   if ( _techlevel > 0 ) {
      techlevel = _techlevel;
      for ( int j = 0; j < map->getTechnologyNum(); j++ ) {
         ptechnology tech = map->gettechnology_bypos ( j );
         if ( tech )
            if ( tech->techlevelget <= techlevel )
               if ( !technologyresearched ( tech->id ))
                  addanytechnology ( tech );
      }
   }
}

bool tresearch :: technologyresearched ( int id )
{
   return developedTechnologies.find ( id ) != developedTechnologies.end();
}





int tresearch :: vehicleclassavailable ( const pvehicletype fztyp , int classnm )
{
   if ( fztyp->classbound[classnm].techlevel )
      if ( fztyp->classbound[classnm].techlevel <= techlevel )
         return true;


   int  i;

   #ifndef karteneditor
   for (i=0;i<4 ;i++ )
      if (fztyp->classbound[classnm].techrequired[i])
         if ( !technologyresearched ( fztyp->classbound[classnm].techrequired[i] ))
            return false;

/*
   if ( fztyp->classbound[classnm].eventrequired )
      if (!map->eventpassed (fztyp->classbound[classnm].eventrequired ))
         return false;
*/
   #endif

   for (i=0; i< researchableWeaponImprovements; i++)
      if ( fztyp->classbound[classnm].weapstrength[i] > unitimprovement.weapons[i] )
         return false;


   if ( fztyp->classbound[classnm].armor > unitimprovement.armor )
      return false;

   return true;

}


int tresearch :: vehicletypeavailable ( const pvehicletype fztyp )
{
   if ( !fztyp )
      return 0;
   else
      return vehicleclassavailable( fztyp, 0 );
}


tresearch :: ~tresearch ()
{
}
