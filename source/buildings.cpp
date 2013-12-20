/***************************************************************************
                          buildings.cpp  -  description
                             -------------------
    begin                : Sat Feb 17 2001
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/*! \file buildings.cpp
    \brief The buildings which a placed on the map
*/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <algorithm>
#include <cmath>

#include "vehicletype.h"
#include "buildingtype.h"
#include "buildings.h"
#include "viewcalculation.h"
#include "errors.h"
#include "spfst.h"
#include "resourcenet.h"
#include "itemrepository.h"
#include "containercontrols.h"
#include "misc.h"


#ifndef BUILDINGVERSIONLIMIT
# define BUILDINGVERSIONLIMIT -1000000000
#endif


const float repairEfficiencyBuilding[resourceTypeNum*resourceTypeNum] = { 1./3., 0,     1. / 3. ,
                                                                          0,     1./3., 0,
                                                                          0,     0,     0 };

Building :: Building ( GameMap* actmap, const MapCoordinate& _entryPosition, const BuildingType* type, int player, bool setupImages, bool chainToField )
           : ContainerBase ( type, actmap, player ), typ ( type ), repairEfficiency ( repairEfficiencyBuilding )
{
   
   viewOnMap = false;
         
   int i;
   for ( i = 0; i < 8; i++ )
      aiparam[i] = NULL;

   _completion = 0;
   connection = 0;

   lastmineddist= 0;

   for ( i = 0; i < waffenanzahl; i++ )
      ammo[i] = 0;

   netcontrol = 0;

   visible = 1;
   damage = 0;

   entryPosition = _entryPosition;

   gamemap->player[player].buildingList.push_back ( this );

   if ( chainToField )
      chainbuildingtofield ( entryPosition, setupImages );
}


int Building::repairableDamage() const
{
   int d = getMap()->getgameparameter ( cgp_maxbuildingrepair ) - repairedThisTurn;
   if ( d < 0 )
      d = 0;
   if ( d > damage )
      d = damage;
   return d;
}

void Building::postRepair ( int oldDamage, bool autoRepair )
{
   repairedThisTurn += oldDamage - damage;
}

bool Building::canRepair ( const ContainerBase* item ) const
{
   return typ->hasFunction( ContainerBaseType::InternalUnitRepair  ) ||
         typ->hasFunction( ContainerBaseType::ExternalRepair  ) ||
         (item == this  ) ;
}

int Building::getIdentification() const
{
   return -(getPosition().x + (getPosition().y << 16));
}

void Building :: convert ( int player, bool recursive )
{
   if (player > 8)
      fatalError("convertbuilding: \n player must be in range 0..8 sein ");

   #ifdef sgmain
   if ( typ->hasFunction( ContainerBaseType::SelfDestructOnConquer  ) ) {
      delete this;
      return;
   }


   int oldnetcontrol = netcontrol;
   netcontrol = cnet_stopenergyinput + (cnet_stopenergyinput << 1) + (cnet_stopenergyinput << 2);
   Resources put = putResource( actstorage, false );
   actstorage -= put;

   netcontrol = oldnetcontrol;

   #endif

   int oldcol = getOwner();

   #ifdef sgmain
   if ( oldcol == 8 )
      for ( int r = 0; r < 3; r++ )
         if ( gamemap->isResourceGlobal( r )) {
            gamemap->bi_resource[player].resource(r) += actstorage.resource(r);
            actstorage.resource(r) = 0;
         }

   #endif
   bool hadViewOnMap = viewOnMap;
   if ( color < 8*8  && hadViewOnMap)
      removeview();

   Player::BuildingList::iterator i = find ( gamemap->player[oldcol].buildingList.begin(), gamemap->player[oldcol].buildingList.end(), this );
   if ( i != gamemap->player[oldcol].buildingList.end())
      gamemap->player[oldcol].buildingList.erase ( i );

   gamemap->player[player].buildingList.push_back( this );

   color = player * 8;

   if ( (hadViewOnMap && (player < 8)) || ( oldcol ==8 ) )
      addview();

   if ( recursive )
      for ( Cargo::iterator i = cargo.begin(); i != cargo.end(); ++i )
         if ( *i ) 
            (*i)->convert( player );

   conquered();
   anyContainerConquered(this);
}


void Building :: registerForNewOwner( int player )
{
   int oldcol = getOwner();
   
   if ( oldcol < 8 )
      removeview();
   
   Player::BuildingList::iterator i = find ( gamemap->player[oldcol].buildingList.begin(), gamemap->player[oldcol].buildingList.end(), this );
   if ( i != gamemap->player[oldcol].buildingList.end())
      gamemap->player[oldcol].buildingList.erase ( i );

   gamemap->player[player].buildingList.push_back( this );
   
   color = player * 8;
  
   if ( player < 8 )
      addview();
   
   conquered();
   anyContainerConquered(this);
   
}

  


const Surface& Building :: getPicture ( const BuildingType::LocalCoordinate& localCoordinate ) const
{
   static Surface emptySurface;
   MapField* fld = getField ( localCoordinate );
   if ( fld ) {
      return typ->getPicture(localCoordinate, fld->getWeather(), _completion);
   } else
      return emptySurface;
}

void Building::paintSingleField ( Surface& s, SPoint imgpos, BuildingType::LocalCoordinate pos ) const
{
   if ( getHeight() <= chfahrend )
      paintField( getPicture( pos ), s, imgpos, 0, false, 0 );
   else
      paintField( getPicture( pos ), s, imgpos, 0, false );
}



// fucking namespace pollution, we are defining our own...

template<typename T>
T ASC_min ( T a, T b )
{
   if ( a > b )
      return b;
   else
      return a;
}
template<typename T>
T ASC_max ( T a, T b )
{
   if ( a > b )
      return a;
   else
      return b;
}



Surface Building::getImage() const
{
   int minx = maxint;
   int maxx = 0;
   int maxy = 0;
   int miny = maxint;

   for ( int x = 0; x < 4; x++ )
      for ( int y = 0; y < 6; y++ ) 
         if ( typ->fieldExists(BuildingType::LocalCoordinate(x,y) )) {
            SPoint pos ( x * fielddistx + ( y & 1 ) * fielddisthalfx, y * fielddisty);
            minx = ASC_min( minx, int(pos.x) );
            maxx = ASC_max( maxx, int(pos.x) + fieldsizex );
            miny = ASC_min ( miny, int(pos.y) );
            maxy = ASC_max ( maxy, int(pos.y) + fieldsizey );
         }

   Surface s = Surface::createSurface(maxx-minx+1,maxy-miny+1, 32,Surface::transparent << 24 );

   typ->paint ( s, SPoint(-minx,-miny), getOwningPlayer().getPlayerColor() ); 
   return s;
}

#ifndef sgmain
void Building :: execnetcontrol ( void ) {}
int Building :: putResource ( int amount, int resourcetype, bool queryonly, int scope, int player ) { return 0; };
int Building :: getResource ( int amount, int resourcetype, bool queryonly, int scope, int player ) { return actstorage.resource(resourcetype); };
int Building :: getAvailableResource ( int amount, int resourcetype, int scope ) const { return actstorage.resource(resourcetype); };
#endif

void Building :: setCompletion ( int completion, bool setupImages )
{
  _completion = completion;
}





int  Building :: chainbuildingtofield ( const MapCoordinate& entryPos, bool setupImages )
{
   MapCoordinate oldpos = entryPosition;
   entryPosition = entryPos;

   for ( int a = 0; a < 4; a++)
      for ( int b = 0; b < 6; b++)
         if ( typ->fieldExists ( BuildingType::LocalCoordinate( a, b) )) {
            MapField* f = getField( BuildingType::LocalCoordinate( a, b) );
            if ( !f || f->building ) {
               entryPosition = oldpos;
               return 1;
            }
         }

   for ( int a = 0; a < 4; a++)
      for ( int b = 0; b < 6; b++)
         if ( typ->fieldExists ( BuildingType::LocalCoordinate( a , b ) )) {
            MapField* field = getField( BuildingType::LocalCoordinate( a, b) );

            if ( visible ) {
               MapField::ObjectContainer::iterator i = field->objects.begin();
               while ( i != field->objects.end()) {
                  if ( !i->typ->canExistBeneathBuildings )
                     i = field->objects.erase ( i );
                  else
                     i++;
               };
            }

            if ( field->vehicle && (field->vehicle->height < chtieffliegend )) {
               delete field->vehicle;
               field->vehicle = NULL;
            }

            field->building = this;
         }

   for ( Cargo::iterator i = cargo.begin(); i != cargo.end(); ++i )
      if ( *i ) 
         (*i)->setnewposition ( entryPos.x, entryPos.y );

   MapField* field = getField( typ->entry );
   if ( field )
      field->bdt |= getTerrainBitType(cbbuildingentry) ;

   if ( setupImages ) 
      gamemap->calculateAllObjects();

      
   for ( int a = 0; a < 4; a++)
      for ( int b = 0; b < 6; b++) 
         if ( typ->fieldExists(BuildingType::LocalCoordinate(a,b))) {
            MapCoordinate p = getFieldCoordinates( BuildingType::LocalCoordinate(a,b));
            BuildingType::LocalCoordinate lc = getLocalCoordinate(p);
            if ( lc.x != a || lc.y!= b )
               warningMessage( "bug in building coordinate calculation");
         }
         

   return 0;
}


int  Building :: unchainbuildingfromfield ( void )
{
   int set = 0;
   for (int i = 0; i <= 3; i++)
      for (int j = 0; j <= 5; j++)
         if ( typ->fieldExists ( BuildingType::LocalCoordinate(i,j) ) ) {
            MapField* fld = getField( BuildingType::LocalCoordinate(i,j) );
            if ( fld && fld->building == this ) {
               set = 1;
               fld->building = NULL;

               TerrainBits t = getTerrainBitType(cbbuildingentry);
               t.flip();
               fld->bdt &= t;
            }
         }

   return set;
}


void Building :: addview ( void )
{
   if ( viewOnMap )
      fatalError ("void Building :: addview - the building is already viewing the map");

   viewOnMap = true;
   
   tcomputebuildingview bes ( gamemap );
   bes.init( this, +1 );
   bes.startsearch();
}

void Building :: resetview()
{
   viewOnMap = false;  
}


void Building :: removeview ( void )
{
   if ( color != 64 ) {
      if ( !viewOnMap )
         fatalError ("void Building :: removeview - the building is not viewing the map");
      
      tcomputebuildingview bes ( gamemap );
      bes.init( this, -1 );
      bes.startsearch();
      
      viewOnMap = false;
   }
}

MapCoordinate3D Building::getPosition3D() const 
{
   MapCoordinate3D e = getEntry();
   MapCoordinate3D p3;
   p3.setnum ( e.x, e.y, -1 );
   return p3;
}

int Building :: getArmor ( ) const
{
   return typ->_armor * gamemap->getgameparameter( cgp_buildingarmor ) / 100;
}


MapField*        Building :: getField( const BuildingType::LocalCoordinate& lc ) const
{
  return gamemap->getField ( getFieldCoordinates ( lc ));
}


MapField*        Building :: getEntryField( ) const
{
  return getField ( typ->entry );
}

MapCoordinate3D Building :: getEntry( ) const
{
  return MapCoordinate3D( entryPosition, typ->height);
}



MapCoordinate Building :: getFieldCoordinates ( const BuildingType::LocalCoordinate& lc ) const
{
  return typ->getFieldCoordinate ( entryPosition, lc );
}


BuildingType::LocalCoordinate Building::getLocalCoordinate( const MapCoordinate& field ) const
{
  return typ->getLocalCoordinate( entryPosition, field );
}


Building :: ~Building ()
{
   if ( gamemap ) {
      int c = color/8;

      Player::BuildingList::iterator i = find ( gamemap->player[c].buildingList.begin(), gamemap->player[c].buildingList.end(), this );
      if ( i != gamemap->player[c].buildingList.end() )
         gamemap->player[c].buildingList.erase ( i );
   }

   for ( int i = 0; i < 8; ++i ) {
      delete aiparam[ i ] ;
      aiparam[ i ] = NULL;
   }

   unchainbuildingfromfield();
}


const int buildingstreamversion = -7;


void Building :: write ( tnstream& stream, bool includeLoadedUnits ) const
{
    stream.writeInt ( max( buildingstreamversion, BUILDINGVERSIONLIMIT ));

    stream.writeInt ( typ->id );
    int i;
    for ( i = 0; i< resourceTypeNum; i++ )
       stream.writeInt ( bi_resourceplus.resource(i) );
    stream.writeUint8 ( color );
    stream.writeWord ( getEntry().x );
    stream.writeWord ( getEntry().y );
    stream.writeUint8 ( getCompletion() );
    for ( i = 0; i < waffenanzahl; i++ )
       stream.writeWord ( 0 ); // was: ammoautoproduction

    for ( i = 0; i< resourceTypeNum; i++ )
       stream.writeInt ( plus.resource(i) );

    for ( i = 0; i< resourceTypeNum; i++ )
       stream.writeInt ( maxplus.resource(i) );

    for ( i = 0; i< resourceTypeNum; i++ )
       stream.writeInt ( actstorage.resource(i) );

    for ( i = 0; i< waffenanzahl; i++ )
       stream.writeWord ( ammo[i] );

    stream.writeWord ( maxresearchpoints );
    stream.writeWord ( researchpoints );
    stream.writeUint8 ( visible );
    stream.writeUint8 ( damage );
    stream.writeInt  ( netcontrol );
    stream.writeString ( name );

    stream.writeInt ( repairedThisTurn );

    int c = 0;

    if ( includeLoadedUnits )
       for ( Cargo::const_iterator i = cargo.begin(); i != cargo.end(); ++i )
          if ( *i ) 
             ++c;

    if ( BUILDINGVERSIONLIMIT >= -3 )
       stream.writeUint8( c );
    else
       stream.writeInt ( c );

    if ( c )
       for ( Cargo::const_iterator i = cargo.begin(); i != cargo.end(); ++i )
          if ( *i ) 
             (*i)->write ( stream );


    if ( BUILDINGVERSIONLIMIT >= -4 )
       stream.writeUint8( internalUnitProduction.size() );
    else
       stream.writeInt( internalUnitProduction.size() );

    for (int k = 0; k < internalUnitProduction.size(); k++ ) {
       assert( internalUnitProduction[k] );
       stream.writeInt( internalUnitProduction[k]->id );
    }   

    if ( BUILDINGVERSIONLIMIT >= -3 )
       stream.writeUint8(0);

    stream.writeInt( view );
    stream.writeString( privateName );
}


Building* Building::newFromStream ( GameMap* gamemap, tnstream& stream, bool chainToField )
{
    int version = stream.readInt();
    int xpos, ypos, color;
    Resources res;

    BuildingType* typ;

    if ( version < buildingstreamversion )
       throw tinvalidversion( stream.getDeviceName(), buildingstreamversion, version );

    if ( version >= buildingstreamversion && version <= -1 ) {

       int id = stream.readInt ();
       typ = gamemap->getbuildingtype_byid ( id );
       if ( !typ )
          throw InvalidID ( "building", id );

       for ( int i = 0; i < 3; i++ )
          res.resource(i) = stream.readInt();

       color = stream.readUint8();
       xpos = stream.readWord() ;
    } else {
       int id = version;

       typ = gamemap->getbuildingtype_byid ( id );
       if ( !typ )
          throw InvalidID ( "building", id );

       color = stream.readUint8();
       xpos  = stream.readWord();
    }

    ypos = stream.readWord();

    Building* bld = new Building ( gamemap, MapCoordinate(xpos,ypos), typ, color/8, false, chainToField );
    bld->bi_resourceplus = res;
    bld->readData ( stream, version );
    return bld;
}



void Building:: read ( tnstream& stream )
{
    int version = stream.readInt();

    if ( version >= buildingstreamversion && version <= -1 ) {
       stream.readInt (); // id
       for ( int i = 0; i < 3; i++ )
          bi_resourceplus.resource(i) = stream.readInt();

       stream.readUint8(); // color
       stream.readWord(); // xpos
       stream.readWord(); // ypos
    } else {
       // int id = version;
       stream.readUint8(); // color
       stream.readWord(); // xpos
       stream.readWord(); // ypos
       bi_resourceplus = Resources ( 0, 0, 0);
    }

    readData ( stream, version );
}


void Building :: readData ( tnstream& stream, int version )
{
    setCompletion ( stream.readUint8(), false );

    int i;
    for ( i = 0; i < waffenanzahl; i++)
       stream.readWord(); // was : ammoautoproduction

    for ( i = 0; i< 3; i++ )
       plus.resource(i) = stream.readInt();

    for ( i = 0; i< 3; i++ )
       maxplus.resource(i) = stream.readInt();


    for ( i = 0; i< 3; i++ )
       actstorage.resource(i) = min(stream.readInt(), getStorageCapacity().resource(i) );

    for ( i = 0; i < waffenanzahl; i++)
       ammo[i] = stream.readWord();

    maxresearchpoints = stream.readWord();
    researchpoints = stream.readWord();

    if ( (researchpoints < typ->maxresearchpoints  &&  !typ->hasFunction(ContainerBaseType::Research)) || (researchpoints > typ->maxresearchpoints))
       researchpoints = typ->maxresearchpoints;


    visible = stream.readUint8();
    damage = stream.readUint8();
    
    netcontrol = stream.readInt();
    netcontrol = 0;
    
    name = stream.readString ();

    if ( version <= -2 )
       repairedThisTurn = stream.readInt ( );
    else
       repairedThisTurn = 0;

    int c;
    if ( version <= -4 )
       c = stream.readInt();
    else
       c = stream.readUint8();
       
    if ( c ) {
       for ( int k = 0; k < c; k++) {
          Vehicle* v = Vehicle::newFromStream ( gamemap, stream );
          v->setnewposition ( getEntry().x, getEntry().y );
          addToCargo(v);

          if ( v && v->reactionfire.getStatus() != Vehicle::ReactionFire::off && !v->baseType->hasFunction( ContainerBaseType::MoveWithReactionFire ))
             v->reactionfire.disable();

       }
    }

    
    internalUnitProduction.clear();
    if ( version <= -5 )
       c = stream.readInt();
    else
       c = stream.readUint8();
    
    if ( c ) {
       for ( int k = 0; k < c ; k++) {

           int id;
           if ( version <= -3 )
              id = stream.readInt();
           else
              id = stream.readWord();

           internalUnitProduction.push_back ( gamemap->getvehicletype_byid ( id ) );
           if ( !internalUnitProduction[k] )
              throw InvalidID ( "unit", id );
       }
    }

    if ( version >= -3 ) {
         c = stream.readUint8();
         if ( c ) {
            for ( int k = 0; k < c ; k++) {
               if ( version <= -3 )
                  stream.readInt();
               else
                  stream.readWord();
            }
         }
    }    

    for ( i = 0; i< 3; i++ ) {
       if ( typ->maxplus.resource(i) > 0 ) {
          if ( plus.resource(i) > typ->maxplus.resource(i) )
            plus.resource(i) = typ->maxplus.resource(i);

          if ( maxplus.resource(i) > typ->maxplus.resource(i) )
            maxplus.resource(i) = typ->maxplus.resource(i);
       } else {
          if ( plus.resource(i) < typ->maxplus.resource(i) )
            plus.resource(i) = typ->maxplus.resource(i);

          if ( maxplus.resource(i) < typ->maxplus.resource(i) )
            maxplus.resource(i) = typ->maxplus.resource(i);
       }
    }
    
    if ( version <= -6 )
       view = stream.readInt();
    else
       view = typ->view;
    
    if ( version <= -7 )
       privateName = stream.readString();
    else
       privateName = "";
}

ASCString Building::getName ( ) const
{
   if ( name.empty())
      return typ->name;
   else
      return name;
}

int Building::getAmmo( int type, int num, bool queryOnly ) 
{
   assert( type >= 0 && type < waffenanzahl );
   int got = min( ammo[type], num );
   if ( !queryOnly ) {
      ammo[type] -= got;
      // ammoChanged();
   }
   return got;
}

int Building::getAmmo( int type, int num ) const
{
   assert( type >= 0 && type < waffenanzahl );
   return min( ammo[type], num );
}


int Building::putAmmo( int type, int num, bool queryOnly ) 
{
   assert( type >= 0 && type < waffenanzahl );
   if ( !queryOnly ) {
      ammo[type] += num;
      // ammoChanged();
   }
   return num;
}




void Building::endRound(  )
{
   ContainerBase::endRound();
   repairedThisTurn = 0;
}


vector<MapCoordinate> Building::getCoveredFields()
{
   vector<MapCoordinate> fields;
   for ( int x = 0; x < 4; x++ )
      for ( int y = 0; y < 6; y++)
         if ( typ->fieldExists ( BuildingType::LocalCoordinate(x, y) ) )
            fields.push_back ( getFieldCoordinates( BuildingType::LocalCoordinate(x, y)));
   return fields;
}


int Building::getMemoryFootprint() const
{
   return sizeof(*this);
}



/*
void Building :: getresourceusage ( Resources* usage )
{
   returnresourcenuseforpowerplant ( this, 100, usage, 0 );
   if ( typ->special & cgresearchb ) {
      int material;
      int energy;
      returnresourcenuseforresearch ( this, researchpoints, &energy, &material );
      usage->material += material;
      usage->energy   += energy;
      usage->fuel  = 0;
   }
}
*/



   struct  ResearchEfficiency {
               float eff;
               Building*  bld;
               bool operator<( const ResearchEfficiency& re) const { return eff > re.eff; };
           };


void doresearch ( GameMap* actmap, int player )
{
   Research& research = actmap->getPlayer(player).research;
   
   if ( research.activetechnology == NULL && research.progress ) {
      // we don't accumulate research if there is not technology to research
      // this is to prevent player from accumulating lots of unused research points and then
      // developing several technologies at once  
      return;
   }
   
   
   typedef vector<ResearchEfficiency> VRE;
   VRE vre;

   for ( Player::BuildingList::iterator bi = actmap->player[player].buildingList.begin(); bi != actmap->player[player].buildingList.end(); bi++ ) {
      Building* bld = *bi;
      if ( bld->typ->hasFunction( ContainerBaseType::Research  ) ) {
         Resources res = returnResourcenUseForResearch ( bld, bld->researchpoints );

         int m = max ( res.energy, max ( res.material, res.fuel));

         ResearchEfficiency re;
         if ( m )
            re.eff = float(bld->researchpoints) / float(m);
         else
            re.eff = maxint;

         re.bld = bld;

         vre.push_back(re);
      } else
         if ( bld->researchpoints > 0 )
            research.progress += bld->researchpoints *  research.getMultiplier();
   }
   sort( vre.begin(), vre.end());

   for ( VRE::iterator i = vre.begin(); i != vre.end(); ++i ) {
      Building* bld = i->bld;
      Resources r = returnResourcenUseForResearch ( bld, bld->researchpoints );
      Resources got = bld->getResource ( r, true, 1, player );

      int res = bld->researchpoints;
      if ( got < r ) {
         int diff = bld->researchpoints / 2;
         while ( got < r || diff > 1) {
            if ( got < r  )
               res -= diff;
            else
               res += diff;

            if ( res < 0 )
               res = 0;

            if ( diff > 1 )
               diff /=2;
            else
               diff = 1;

            r = returnResourcenUseForResearch ( bld, res );
         }

         /*
         res = returnResourcenUseForResearch ( bld, res+1 );

         if ( ena >= energy  &&  maa >= material )
            res++;
         else
            returnresourcenuseforresearch ( bld, res, &energy, &material );
         */

      }

      got = bld->getResource ( r, false, 1, player );

      if ( got < r )
         fatalError( "controls : doresearch : inconsistency in getting energy or material for building" );

      research.progress += res * research.getMultiplier();
   }
}

ASCString getBuildingReference ( Building* bld )
{
   ASCString s = "The building " + bld->getName();
   s += " (position: "+ bld->getPosition().toString() + ") ";
   return s;
}

