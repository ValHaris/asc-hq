//     $Id: typen.cpp,v 1.15 2000-05-06 20:25:25 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.14  2000/05/06 19:57:11  mbickel
//      Mapeditor/linux is now running
//
//     Revision 1.13  2000/04/27 16:25:30  mbickel
//      Attack functions cleanup
//      New vehicle categories
//      Rewrote resource production in ASC resource mode
//      Improved mine system: several mines on a single field allowed
//      Added unitctrl.* : Interface for vehicle functions
//        currently movement and height change included
//      Changed timer to SDL_GetTicks
//
//     Revision 1.12  2000/03/16 14:06:56  mbickel
//      Added unitset transformation to the mapeditor
//
//     Revision 1.11  2000/01/31 16:08:39  mbickel
//      Fixed crash in line
//      Improved error handling in replays
//
//     Revision 1.10  2000/01/25 19:28:16  mbickel
//      Fixed bugs:
//        invalid mouse buttons reported when moving the mouse
//        missing service weapon in weapon information
//        invalid text displayed in ammo production
//        undamaged units selected in repair vehicle function
//
//      speed up when playing weapon sound
//
//     Revision 1.9  2000/01/24 17:35:48  mbickel
//      Added dummy routines for sound under DOS
//      Cleaned up weapon specification
//
//     Revision 1.8  2000/01/24 08:16:52  steb
//     Changes to existing files to implement sound.  This is the first munge into
//     CVS.  It worked for me before the munge, but YMMV :)
//
//     Revision 1.7  2000/01/06 11:19:16  mbickel
//      Worked on the Linux-port again...
//
//     Revision 1.6  1999/12/27 13:00:14  mbickel
//      new vehicle function: each weapon can now be set to not attack certain
//                            vehicles
//
//     Revision 1.5  1999/12/07 22:02:08  mbickel
//      Added vehicle function "no air refuelling"
//
//     Revision 1.4  1999/11/23 21:07:38  mbickel
//      Many small bugfixes
//
//     Revision 1.3  1999/11/16 17:04:17  mbickel
//     Made ASC compilable for DOS again :-)
//     Merged all the bug fixes in that I did last week
//
//     Revision 1.2  1999/11/16 03:42:44  tmwilson
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

#include <stdio.h>
#include <string.h>

#include "tpascal.inc"
#include "misc.h"
#include "typen.h"

#ifndef converter
#include "spfst.h"
#endif

#ifdef sgmain
#include "network.h"
#endif

const char*  choehenstufen[choehenstufennum] = {"deep submerged", "submerged", "floating", "ground level", "low-level flight", "flight", "high-level flight", "orbit"}; 
                                                   // fahrend ??

const char* cconnections[6]  = {"destroyed", "conquered", "lost", "seen", "area entererd by any unit", "area entered by specific unit"};
const char* ceventtriggerconn[8]  = {"AND ", "OR ", "NOT ", "( ", "(( ", ")) ", ") ", "Clear "}; 
const char* ceventactions[ceventactionnum]  = {"message", "weather change", "new technology discovered", "lose campaign", "run script + next map",
                                               "new technology researchable", "map change", "discarded [ was erase event ]", "end campaign", "next map",
                                               "reinforcement","weather change completed", "new vehicle developed","palette change",
                                               "alliance change","wind change", "nothing", "change game parameter","paint ellipse","remove ellipse"};

const char* ceventtrigger[ceventtriggernum]  = {"*NONE*", "turn/move >=", "building conquered", "building lost",
                                                 "building destroyed", "unit lost", "technology researched",
                                                 "event", "unit conquered", "unit destroyed", "all enemy units destroyed",
                                                 "all units lost", "all enemy buildings destroyed/captured", "all buildings lost", 
                                                 "energy tribute <", "material tribute <", "fuel tribute <",
                                                 "any unit enters polygon", "specific unit enters polygon", "building is seen"};
const char*  cminentypen[cminenum]  = {"antipersonnel mine", "antitank mine", "moored mine", "floating mine"};
const int cminestrength[cminenum]  = { 60, 120, 180, 180 };

const char*  cbuildingfunctions[cbuildingfunctionnum]  = {"HQ",                "training",             "unused (was: refinery)",           "vehicle production", "ammunition production", 
                                                      "unused (was: energy prod)", "unused (was: material prod)",  "unused (was: fuel prod)",    "repair facility",    "recycling",
                                                      "research",          "sonar",                "wind power plant",     "solar power plant",    "matter converter (was: power plant)",
                                                      "mining station",    "external loading" };

const char*  cvehiclefunctions[cvehiclefunctionsnum]  = {"sonar",             "paratrooper",       "mine-layer",        "trooper",               "repair vehicle",
                                                         "conquer buildings", "move after attack","view satellites",   "construct ALL buildings", "view mines", 
                                                         "construct vehicles","construct specific buildings", "refuel units",      "icebreaker",  "cannot be refuelled in air", 
                                                            "refuels material", "!",               "makes tracks",   "drill for mineral resources manually",  "sailing",
                                                                                                    // ?? Fahrspurenleger 
                                                            "auto repair",        "generator",        "search for mineral resources automatically", "Kamikaze only",
                                                            "immune to mines" };
                                                            
const char*  cbodenarten[cbodenartennum]  = {"shallow water , coast"       ,    "normal lowland",   "swamp thick",       "forest",       
                                             "high mountains",                  "road",             "railroad",    "entry of building (not to be used for terrain)" ,
                                             "harbour (safe for ships at storm)",                         "runway"  ,         "pipeline",    "buried pipeline",        
                                             "water",                           "deep water",       "hard sand",   "soft sand",  
                                             "track possible",                  "small rocks",      "mud",         "snow", 
                                             "deep snow",                       "mountains",        "very shallow water",
                                             "large rocks",                     "small trench",     "ditch",  "hillside",
                                             "turret foundation",               "swamp thin",       "Installation", "pack ice", "river", "frozen water" };
                                                             

const char*  cwaffentypen[cwaffentypennum]  = {"cruise missile", "mine",    "bomb",       "air - missile", "ground - missile", "torpedo", "machine gun",
                                               "cannon",         "service", "ammunition refuel", "laser (not implemented yet!)", "shootable"};

const char*  cmovemalitypes[cmovemalitypenum] = { "default",
                                                 "light tracked vehicle", "medium tracked vehicle", "heavy tracked vehicle",
                                                 "light wheeled vehicle", "medium wheeled vehicle", "heavy wheeled vehicle",
                                                 "trooper",               "rail vehicle",           "aircraft",
                                                 "ships",                 "building / turret / object" };

const char* cnetcontrol[cnetcontrolnum] = { "store energy",           "store material",           "store fuel",           
                                            "move out all energy",           "move out all material",           "move out all fuel", 
                                            "stop storing energy", "stop storing material", "stop storing fuel", 
                                            "stop energy extraction", "stop material extraction", "stop fuelextraction" };
const char* cgeneralnetcontrol[4] = {       "store",  "move out", "stop storing", "stop extraction" };
                                          // Functionen in GebÑuden ....

const char*  cwettertypen[cwettertypennum] = {"dry (standard)","light rain", "heavy rain", "few snow", "lot of snow", "fog (don't use!!)"};
const char*  resourceNames[3]  = {"energy", "material", "fuel"}; 
const int  cwaffenproduktionskosten[cwaffentypennum][3]  = {{20, 15, 10}, {2, 2, 0}, {3, 2, 0}, {3, 3, 2}, {3, 3, 2}, {4, 3, 2},
                                                            {1, 1, 0},    {1, 2, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}; // jeweils fÅr weaponpackagesize Pack !

const int directionangle [ sidenum ] = 
#ifdef HEXAGON
 { 0, -53, -127, -180, -180 -53 , -180 -127 };
#else
 { 0, -45, -90, -135, -180, - 225, - 270, -315 };
#endif


tgameoptions gameoptions;

const int gameparameterdefault [ gameparameternum ] = { 1, 2, 0, 100, 100, 0, 0, 0, 1, 0, 0, 0, 0 };
const char* gameparametername[ gameparameternum ] = { "lifetime of tracks", 
                                                      "freezing time of broken ice cover ( icebreaker )",
                                                      "move vehicles from unaccessible fields",
                                                      "building construction material factor ( in percent )",
                                                      "building construction fuel factor ( in percent )",
                                                      "forbid construction of buildings",
                                                      "forbid units to build units",
                                                      "use BI3 style training factor ",
                                                      "maximum number of mines on a single field",
                                                      "lifetime of antipersonnel mine",
                                                      "lifetime of antitank mine",
                                                      "lifetime of moored mine",
                                                      "lifetime of floating mine" };


const int csolarkraftwerkleistung[cwettertypennum] = { 1024, 512, 256, 756, 384 }; // 1024 ist Maximum 

tobjectcontainer :: tobjectcontainer ( void )
{
   minenum = 0;
   for ( int i = 0; i < maxminesonfield; i++ )
      mine[ i ] = NULL;

   objnum = 0;
}

int  tobjectcontainer :: checkforemptyness ( void )
{
   return 0;

/*
   if ( mine )
     return 0;   

    if ( special & ( cbstreet | cbrailroad | cbpowerline | cbpipeline ))
       return 0;

    return 1;
*/

}

int tfield :: getweather ( void )
{
   for ( int w = 0; w < cwettertypennum; w++ )
      if ( typ == typ->terraintype->weather[w] )
         return w;
   return -1;
}

void tfield :: sortobjects ( void )
{
   if ( object )
      for ( int a = 0; a < object->objnum-1; ) {
         if ( object->object[ a ]->typ->height > object->object[ a+1 ]->typ->height ) {
            pobject o = object->object[ a ];
            object->object[ a ] = object->object[ a+1 ];
            object->object[ a+1 ] = o;
            if ( a > 0 )
               a--;
         } else
            a++;
      }

}


int tfield :: minenum ( void )
{
  if ( object ) 
     return object->minenum;
  else
     return 0;
}

int  tfield :: putmine( int col, int typ, int strength )
{ 
  #ifndef converter
   if ( mineowner() >= 0  && mineowner() != col )
      return 0;

   if ( !object )
      object = new tobjectcontainer;

   if ( object->minenum >= maxminesonfield )
      return 0;

   /*
   if ( object->minenum >= actmap->getgameparameter ( cgp_maxminesonfield ))
      return 0;
   */

   object->mine[ object->minenum ] = new tmine; 
   object->mine[ object->minenum ]->strength = strength ;
   object->mine[ object->minenum ]->color = col;
   object->mine[ object->minenum ]->type = typ;
   if ( actmap && actmap->time.a.turn>= 0 )
      object->mine[ object->minenum ]->time = actmap->time.a.turn;
   else
      object->mine[ object->minenum ]->time = 0;

   object->minenum++;
   return 1;
  #else
   return 0;
  #endif
} 

int tfield :: mineowner( void )
{
   if ( !object )
      return -1;
   if ( !object->minenum )
      return -1;

   return object->mine[0]->color;
}


void tfield :: removemine( int num )
{ 
   if ( !object )
      return;

   if ( num == -1 )
      num = object->minenum - 1;

   if ( num >= object->minenum )
      return;

   delete object->mine[num];

   for ( int i = num+1; i < object->minenum; i++ )
      object->mine[i-1] = object->mine[i];

   object->minenum--;
   object->mine[object->minenum] = NULL;
} 



#ifdef converter

int tfield :: mineattacks ( const pvehicle veh ) { return 0; }
void tfield :: checkminetime ( int time ) { }
int tfield :: getx( void ) { return 0; }
int tfield :: gety( void ) { return 0; }

void  tfield :: addobject( pobjecttype obj, int dir, int force )
{ 
} 


void tfield :: removeobject( pobjecttype obj )
{ 
} 
#else
int tfield :: getx( void )
{
   int n = this - actmap->field;
   return n % actmap->xsize;
}

int tfield :: gety( void )
{
   int n = this - actmap->field;
   return n / actmap->xsize;
}
#endif




int tfield :: getattackbonus ( void )
{
   int a = typ->attackbonus;
   if ( object )
      for ( int i = 0; i < object->objnum; i++ ) {
         if ( object->object[i]->typ->attackbonus_abs >= 0 )
            a = object->object[i]->typ->attackbonus_abs;
         else
            a += object->object[i]->typ->attackbonus_plus;
      }
   return a;
}

int tfield :: getdefensebonus ( void )
{
   int a = typ->defensebonus;
   if ( object )
      for ( int i = 0; i < object->objnum; i++ ) {
         if ( object->object[i]->typ->defensebonus_abs >= 0 )
            a = object->object[i]->typ->defensebonus_abs;
         else
            a += object->object[i]->typ->defensebonus_plus;
      }
   return a;
}

int tfield :: getjamming ( void )
{
   int a = typ->basicjamming;
   if ( object )
      for ( int i = 0; i < object->objnum; i++ ) {
         if ( object->object[i]->typ->basicjamming_abs >= 0 )
            a = object->object[i]->typ->basicjamming_abs;
         else
            a += object->object[i]->typ->basicjamming_plus;
      }
   return a;
}

int tfield :: getmovemalus ( const pvehicle veh )
{       
   int mnum = minenum();
   if ( mnum ) {
      int movemalus = _movemalus[veh->typ->movemalustyp];
      int col = mineowner();
      if ( veh->color == col*8 ) 
         movemalus += movemalus * mine_movemalus_increase * mnum / 100;

      return movemalus;
   } else
      return _movemalus[veh->typ->movemalustyp];
}

int tfield :: getmovemalus ( int type )
{
  return _movemalus[type];
}

void tfield :: setparams ( void )
{
   int i;
   bdt = typ->art;

   for ( i = 0; i < cmovemalitypenum; i++ )
      _movemalus[i] = typ->movemalus[i];

   if ( object ) 
      for ( int j = 0; j < object->objnum; j++ ) {
         pobjecttype o = object->object[j]->typ;

         bdt  &=  o->terrain_and;
         bdt  |=  o->terrain_or;

         for ( i = 0; i < cmovemalitypenum; i++ ) {
            _movemalus[i] += o->movemalus_plus[i];
            if ( o->movemalus_abs[i] && o->movemalus_abs[i] != -1 )
               _movemalus[i] = o->movemalus_abs[i];
            if ( _movemalus[i] < minmalq )
               _movemalus[i] = minmalq;
         }
      } /* endfor */

   #ifndef converter
   if ( building ) {
      if ( this == getbuildingfield( building, building->typ->entry.x, building->typ->entry.y)) 
         bdt |= cbbuildingentry; 

     #ifdef HEXAGON
      if ( building )
         for (int x = 0; x < 4; x++) 
            for ( int y = 0; y < 6; y++ ) 
               if ( getbuildingfield ( building, x, y ) == this )
                  if ( building->getpicture ( x, y ) )
                     picture = building->getpicture ( x, y );
     #endif
   }
   #endif
}


tobject :: tobject ( void )
{
   typ = NULL;
   dir = 0;
   damage = 0;
}

tobject :: tobject ( pobjecttype t )
{
   typ = t;
   dir = 0;
   damage = 0;
}


void tobject :: setdir ( int direc )
{
   dir = direc;
}

int  tobject :: getdir ( void )
{
   return dir;
}

int  tobjecttype :: buildable ( pfield fld )
{
   if ( fld->building )
      return 0;

   #ifdef HEXAGON
   if ( terrainaccess.accessible ( fld->bdt ) <= 0 )
       return 0;
   #endif

   return 1;
}


void* tobjecttype :: getpic ( int i, int w )
{
   #ifdef HEXAGON
   if ( !(weather & (1 << w )))
      w = 0;

   if ( pictnum <= i )
      i = 0;

   return picture [w][ i ].picture;

   #else
   return picture [ i  ];
   #endif
}


void tobject :: display ( int x, int y, int weather )
{
 #ifndef converter
  if ( typ->id == 7 ) {
    #ifndef HEXAGON
     for (int i = 0; i <= 7; i++) 
        if ( dir & (1 << i)) 
           putxlatfilter( x, y, typ->getpic ( i ), &xlattables.nochange[0] ); 
    #else
        if ( dir < typ->pictnum )
           putshadow  ( x, y,  typ->getpic ( dir, weather ) , &xlattables.a.dark1);
        else
           putshadow  ( x, y,  typ->getpic ( 0, weather ) , &xlattables.a.dark1);
    #endif
  } else
 #ifndef HEXAGON
  if ( typ->id == 8 ) {
     putspriteimage( x, y, typ->getpic ( 0 ) ); 
     for (int i = 0; i <= 7; i++) 
        if ( dir & (1 << i))
           putspriteimage( x, y, typ->getpic ( i + 1 ) ); 
  } else
 #endif
  if ( typ->id == 30 ) {
        if ( dir < typ->pictnum )
           putshadow  ( x, y,  typ->getpic ( dir, weather ) , &xlattables.a.dark05);
        else
           putshadow  ( x, y,  typ->getpic ( 0, weather ) , &xlattables.a.dark05);
  } else 
     typ->display ( x, y, dir, weather );

  #endif
}



void tobjecttype :: display ( int x, int y, int dir, int weather )
{
   #ifndef converter
  if ( id == 1 || id == 2 ) {
     putspriteimage ( x, y,  getpic( dir, weather ) );
  } else 
#ifdef HEXAGON
  if ( id == 4 ) {
#else
  if ( id == 3 ||  id == 4 ) {
#endif

     if ( dir == 68 ) 
        putspriteimage ( x, y,  getpic ( 9, weather ) );
     else
     if ( dir == 34 ) 
        putspriteimage ( x, y,  getpic ( 10, weather ) );
     else
     if ( dir == 17 ) 
        putspriteimage ( x, y,  getpic ( 11, weather ) );
     else
     if ( dir == 136) 
        putspriteimage ( x, y,  getpic ( 12, weather ) );
     else
     if ( dir == 0) 
        putspriteimage ( x, y,  getpic ( 0, weather ) );
     else
        for (int i = 0; i <= 7; i++) 
           if ( dir & (1 << i))
              putspriteimage( x, y,  getpic ( i + 1, weather ) ); 

  } else
  if (  id == 5 ) {
     putspriteimage  ( x, y,  getpic ( 0, weather ) );
  } else
  #ifndef HEXAGON
  if (  id == 6  ||  id == 7 || id == 8 ) {
     for (int i = 0; i <= 7; i++) 
        if ( dir & (1 << i))
           putspriteimage( x, y,  getpic ( i, weather ) ); 
  } else 
  #endif

      if ( dirlistnum ) {
         for ( int i = 0; i < dirlistnum; i++ )
            if ( dirlist [ i ] == dir ) {
               putspriteimage ( x, y, getpic ( i, weather ) );
               return;
            }

         for ( int j = 0; j < dirlistnum; j++ )
            if ( dirlist [ j ] == 0 ) {
               putspriteimage ( x, y, getpic ( j, weather ) );
               return;
            }

         putspriteimage ( x, y, getpic ( 0, weather ) );
   
      } else 
        if ( dir < pictnum )
           putspriteimage ( x, y, getpic ( dir, weather ) );
        else
           putspriteimage ( x, y, getpic ( 0, weather ) );
  
  #endif
}



void tobjecttype :: display ( int x, int y )
{
   display ( x, y, 34, 0 );
}


pobject tfield :: checkforobject ( pobjecttype o )
{
   if ( object )
      return object->checkforobject( o );
   else
      return NULL;
}

pobject tobjectcontainer :: checkforobject ( pobjecttype o )
{
   for ( int i = 0; i < objnum; i++ )
      if ( object[i]->typ == o )
         return object[i];
   return NULL;
}



tresourceview :: tresourceview ( void )
{
   visible = 0;
   memset ( &fuelvisible, 0, sizeof ( fuelvisible ));
   memset ( &materialvisible, 0, sizeof ( materialvisible ));
}

tbuilding :: tbuilding ( void )
{
   lastmineddist= 0;
   memset ( &bi_resourceplus, 0, sizeof ( bi_resourceplus ));
}

tbuilding :: tbuilding ( pbuilding src, tmap* actmap )
{
   work = src->work;
   lastenergyavail = src->lastenergyavail;
   lastmaterialavail = src->lastmaterialavail;
   lastfuelavail = src->lastfuelavail;
   typ = src->typ; 
   memcpy ( munitionsautoproduction , src->munitionsautoproduction, sizeof ( munitionsautoproduction ) ); 
   color = src->color; 
   memcpy ( production , src->production, sizeof ( production )); 
   damage = src->damage; 

   plus = src->plus;
   maxplus = src->maxplus;

   actstorage = src->actstorage;

   memcpy ( munition ,  src->munition, sizeof ( munition )); 

   maxresearchpoints = src->maxresearchpoints; 
   researchpoints = src->researchpoints; 

   if ( src->name )
      name = strdup ( src->name );
   else
      name = NULL;

   xpos = src->xpos;
   ypos = src->ypos; 

   completion = src->completion; 
   netcontrol = src->netcontrol; 
   connection = src->connection; 
   visible = src->visible; 
   memcpy ( productionbuyable , src->productionbuyable, sizeof ( productionbuyable ));

   bi_resourceplus = src->bi_resourceplus;

   lastmineddist = src->lastmineddist;

   for ( int i = 0; i < 32; i++ )
     if ( src->loading[i] )
        loading[i] = new tvehicle ( src->loading[i], actmap );
     else
        loading[i] = NULL;

   if ( actmap )
      actmap->chainbuilding ( this );

}


void*   tbuildingtype :: getpicture ( int x, int y )
{
   #ifdef HEXAGON
   return w_picture[0][0][x][y];
   #else
   return picture[0][x][y];
   #endif
}


void  tbuildingtype :: getfieldcoordinates ( int          bldx,
                                             int          bldy,
                                             int          x,
                                             int          y,
                                             int     *    xx,
                                             int     *    yy)

{
 
   int      orgx, orgy;
   int      dx;

   orgx=bldx - entry.x;
   orgy=bldy - entry.y;
   
   dx =orgy & 1;
   orgx = orgx + (dx & (~ entry.y));

   *yy = orgy + y; 
   *xx = orgx + x - (dx & *yy); 
} 


#ifdef converter
int tbuilding :: vehicleloadable ( pvehicle vehicle, int uheight )
{
   return 0;
}

void* tbuilding :: getpicture ( int x, int y )
{
   return NULL;
}

#endif


tvehicle :: tvehicle ( void )
{
   init();
}

tvehicle :: ~tvehicle (  )
{
   delete[] weapstrength;
   weapstrength = NULL;

   delete[] ammo;
   ammo = NULL;

}

tvehicle :: tvehicle ( pvehicle src, pmap actmap )
{
   init();
   clone ( src, actmap );
}

void tvehicle :: init ( void )
{
   dummy = 0;
   moredummy[0] = 0;
   moredummy[1] = 0;
   moredummy[2] = 0;
   evenmoredummy[0] = 0;
   evenmoredummy[1] = 0;
   evenmoredummy[2] = 0;
   name = NULL;
   weapstrength = new int[16];
   ammo = new int[16];
   for ( int i = 0; i < 16; i++ ) {
      weapstrength[i] = 0;
      ammo[i] = 0;
   }

   typ = NULL;          
   color = 0; 
   damage = 0; 
   fuel = 0; 

   experience = 0;
   attacked = 0; 
   height = 0;
   movement = 0;
   direction = 0;
   xpos = -1;
   ypos = -1;
   material = 0;
   energy = 0;
   prev = 0;
   next = 0;
   connection = 0; 
   klasse = 0;
   armor = 0; 
   networkid = -1; 
   name = NULL;
   functions = 0;
   reactionfire = 0;
   reactionfire_active = 0;
   generatoractive = 0;

   for ( int j = 0; j < 32; j++ )
       loading[j] = NULL;
}


void tvehicle :: clone ( pvehicle src, pmap actmap )
{

   typ = src->typ;          
   color = src->color; 
   damage = src->damage; 
   fuel = src->fuel; 
   memcpy ( ammo , src->ammo, 16*sizeof ( int  )); 
   memcpy ( weapstrength , src->weapstrength, 16*sizeof ( int )); 
   experience = src->experience;
   attacked = src->attacked; 
   height = src->height;
   movement = src->movement;
   direction = src->direction;
   xpos = src->xpos;
   ypos = src->ypos;
   material = src->material;
   energy = src->energy;
   connection = src->connection; 
   klasse = src->klasse;
   armor = src->armor; 
   networkid = src->networkid; 
   if ( src->name )
      name = strdup ( src->name );
   else
      name = NULL;
   functions = src->functions;
   reactionfire = src->reactionfire;
   reactionfire_active = src->reactionfire_active;
   generatoractive = src->generatoractive;

   for ( int i = 0; i < 32; i++ )
     if ( src->loading[i] )
        loading[i] = new tvehicle ( src->loading[i], NULL );
     else
        loading[i] = NULL;

   if ( actmap )
      actmap->chainunit ( this );
}

void tvehicle :: setpower ( int status )
{
   if ( functions & cfgenerator ) {
      generatoractive = status;
      if ( status )
         energy = typ->energy;
      else {
         int endiff = typ->energy- energy;
         if ( fuel < endiff * generatortruckefficiency )
            endiff = fuel / generatortruckefficiency;
 
         fuel -= endiff * generatortruckefficiency ;
         energy = 0;
      }
   } else
     generatoractive = 0;
}


void tvehicle :: setup_classparams_after_generation ( void )
{
      armor = typ->armor * typ->classbound[klasse].armor / 1024;
      if ( typ->weapons->count ) { 
         for ( int m = 0; m < typ->weapons->count ; m++) {
            ammo[m] = 0;
            weapstrength[m] = typ->weapons->weapon[m].maxstrength *
	      typ->classbound[klasse].weapstrength[typ->weapons->weapon[m].getScalarWeaponType()] / 1024;
         } 
      }

      if ( typ->classnum ) 
        functions = typ->functions & typ->classbound[klasse].vehiclefunctions;
      else
        functions = typ->functions;

      attacked = true;

}


int tvehicle::cargo ( void )
{
   int w = 0;
   if ( typ->loadcapacity > 0) 
      for (int c = 0; c <= 31; c++) 
         if ( loading[c] ) 
            w += loading[c]->weight();
   return w;
}

int tvehicle::weight( void ) 
{
   return typ->weight + fuel * fuelweight / 1024 + material * materialweight / 1024 + cargo();
}

int tvehicle::size ( void ) 
{ 
   return typ->weight;
}

void tvehicle::transform ( pvehicletype type )
{
   typ = type;

   fuel = typ->tank;
   material = typ->material;
   energy = 0;
   generatoractive = 0;

   for ( int m = 0; m < typ->weapons->count ; m++) {
      ammo[m] = typ->weapons->weapon[m].count;
      weapstrength[m] = typ->weapons->weapon[m].maxstrength;
   }
   armor = typ->armor;
   klasse = 255;
}



/* Translate the weapon/mine/service bit pattern into scalar
 * weapon number for use in fetching UI resources.
 */
int SingleWeapon::getScalarWeaponType(void) const {
   if ( typ & (cwweapon | cwmineb) )
      return log2 ( typ & (cwweapon | cwmineb) );
   else
      return -1;
}


int SingleWeapon::requiresAmmo(void) const
{
   return typ & ( cwweapon | cwmineb );
}

int SingleWeapon::shootable( void ) const
{
   return typ & cwshootableb;
}

int SingleWeapon::offensive( void ) const
{
   return typ & cwweapon;
}

int SingleWeapon::service( void ) const
{
   return typ & cwserviceb;
}

int SingleWeapon::canRefuel( void ) const
{
   return typ & cwammunitionb;
}

void SingleWeapon::set ( int type )
{
   typ = type;
}



#ifdef converter
void tvehicle::convert ( int color )
{
}

void tbuilding::convert ( int color )
{
}

int tvehicle :: getstrongestweapon( int aheight, int distance)
{ 
   return 0;
} 

void tvehicle :: constructvehicle ( pvehicletype tnk, int x, int y )
{
}
int  tvehicle :: vehicleconstructable ( pvehicletype tnk, int x, int y )
{
   return 0;
}

#endif


int tvehicletype::maxweight ( void )
{
   return weight + tank * fuelweight / 1024 + material * materialweight / 1024;
}
   
int tvehicletype::maxsize ( void ) 
{ 
   return weight;
}


tvehicletype :: tvehicletype ( void )
{
   memset ( &name, 0, (int) ((char*) &dummy[1] - (char*)&name)  );
   weapons = new UnitWeapon;
   terrainaccess = new tterrainaccess;
}

UnitWeapon :: UnitWeapon ( void )
{
   count = 0;
   memset ( weapon, 0, sizeof ( weapon ));
   memset ( reserved, 0, sizeof ( reserved ));
}


void tvehicle :: repairunit(pvehicle vehicle, int maxrepair )
{ 
   if ( vehicle->damage  &&  fuel  &&  material ) { 

      int dam;
      if ( vehicle->damage > maxrepair )
         dam = maxrepair;
      else
         dam = vehicle->damage;

      int fkost = dam * vehicle->typ->production.energy / (100 * repairefficiency_unit ); 
      int mkost = dam * vehicle->typ->production.material / (100 * repairefficiency_unit ); 
      int w;

      if (mkost <= material) 
         w = 10000; 
      else 
         w = 10000 * material / mkost; 

      if (fkost > fuel) 
         if (10000 * fuel / fkost < w) 
            w = 10000 * fuel / fkost; 


      vehicle->damage -= dam * w / 10000; 

      if ( vehicle != this ) {
         if ( vehicle->movement > movement_cost_for_repaired_unit )
            vehicle->movement -= movement_cost_for_repaired_unit;
         else
            vehicle->movement = 0;

         if ( !attack_after_repair )
            vehicle->attacked = 0;

         int unitloaded = 0;
         for ( int i = 0; i < 32; i++ )
            if ( loading[i] == vehicle )
               unitloaded = 1;
   
         if ( !unitloaded ) 
            if ( movement > movement_cost_for_repairing_unit )
               movement -= movement_cost_for_repairing_unit;
            else
               movement = 0;
      }

      material -= w * mkost / 10000; 
      fuel -= w * fkost / 10000; 

   } 
} 


void tvehicle :: nextturn( void )
{
   if ( typ->autorepairrate > 0 ) 
      if ( damage ) 
         repairunit ( this, typ->autorepairrate );

}

void tvehicle :: resetmovement ( void )
{
    int move = typ->movement[log2(height)];
    movement = move;
    /*
    if (actvehicle->typ->fuelconsumption == 0) 
       actvehicle->movement = 0;
    else { 
       if ((actvehicle->fuel << 3) / actvehicle->typ->fuelconsumption < move)
          actvehicle->movement = (actvehicle->fuel << 3) / actvehicle->typ->fuelconsumption ;
       else 
          actvehicle->movement = move;
    } 
    */
}


void tvehicle :: putimage ( int x, int y )
{
   #ifndef converter
  #ifdef sgmain
   int shaded = ( movement < minmalq ) && ( color == actmap->actplayer*8) && (attacked || !typ->weapons->count || gameoptions.units_gray_after_move );
  #else
   int shaded = 0;
  #endif
    if ( height <= chgetaucht ) {
          if ( shaded )
             putpicturemix ( x, y, xlatpict(xlatpictgraytable,  typ->picture[  direction ]),  color, (char*) colormixbuf );
          else
             putpicturemix ( x, y,  typ->picture[ direction],  color, (char*) colormixbuf );
          
    } else {
          if ( height >= chtieffliegend ) {
             int d = 6 * ( log2 ( height) - log2 ( chfahrend ));
             putshadow ( x + d, y + d, typ->picture[direction] , &xlattables.a.dark3);
          } else
             if ( height == chfahrend ) 
                putshadow ( x + 1, y + 1,  typ->picture[ direction] , &xlattables.a.dark3);

          if ( shaded )
             putrotspriteimage( x, y, xlatpict(xlatpictgraytable,  typ->picture[  direction ]),  color);
          else
             putrotspriteimage( x, y,  typ->picture[ direction],  color);
    }

   #endif

}

#ifdef converter
int  tvehicle :: vehicleloadable ( pvehicle vehicle,int uheight )
{
   return 0;
}
#endif

void tvehicle :: setnewposition ( int x , int y )
{
  xpos = x; 
  ypos = y; 
  if ( typ->loadcapacity > 0) 
     for ( int i = 0; i <= 31; i++) 
        if ( loading[i] ) 
           loading[i]->setnewposition ( x , y );
}

/*
void tbuilding :: repairunit(pvehicle vehicle, int maxrepair )
{ 
   if ( vehicle->damage ) { 

      int dam;
      if ( vehicle->damage > maxrepair )
         dam = maxrepair;
      else
         dam = vehicle->damage;

      int fkost = dam * vehicle->typ->production.energy / (100 * repairefficiency_building ); 
      int mkost = dam * vehicle->typ->production.material / (100 * repairefficiency_building ); 
      int w;

      if (mkost <= getenergy ( mkost material) 
         w = 10000; 
      else 
         w = 10000 * material / mkost; 

      if (fkost > fuel) 
         if (10000 * fuel / fkost < w) 
            w = 10000 * fuel / fkost; 


      vehicle->damage = vehicle->damage * (1 - w / 10000); 
      material -= w * mkost / 10000; 
      fuel -= w * fkost / 10000; 
   } 
} 
*/


#ifndef karteneditor
  #ifndef sgmain

int tvehicle::getmaxfuelforweight (  )
{
   return 0;
}

int tvehicle::getmaxmaterialforweight ( )
{
   return 0;
}
  #endif
#endif





tmessage :: tmessage ( void )
{
   from = 0;
   to = 0;
   time = 0;
   text = NULL;
   id = 0;
   next = NULL;
}

#ifndef sgmain

tmessage :: tmessage ( pmap spfld )
{
   from = 0;
   to = 0;
   time = 0;
   text = NULL;
   id = 0;
   next = NULL;
   runde = 0;
   move = 0;
}

tmessage :: tmessage ( char* txt, int rec )  // fÅr Meldungen vom System
{
   from = 0;
   to = 0;
   time = 0;
   text = NULL;
   id = 0;
   next = NULL;
   runde = 0;
   move = 0;
}

#else

tmessage :: tmessage ( pmap spfld  )
{
   from = 1 << spfld->actplayer;
   runde = spfld->time.a.turn;
   move = spfld->time.a.move;
   time = ::time( NULL );
   to = 0;
   text = NULL;
   spfld->messageid++;
   id = spfld->messageid;
   next = NULL;
   if ( spfld->message ) {
      pmessage mes = spfld->message;
      while ( mes->next )
         mes = mes->next;
      mes->next = this;
   } else 
     spfld->message = this;

}

extern tmap map;

tmessage :: tmessage ( char* txt, int rec )  // fÅr Meldungen vom System
{
   from = 1 << 9;
   runde = actmap->time.a.turn;
   move = actmap->time.a.move;
   time = ::time( NULL );
   to = rec;
   text = txt;
   actmap->messageid++;
   id = actmap->messageid;
   next = NULL;
   if ( actmap->message ) {
      pmessage mes = actmap->message;
      while ( mes->next )
         mes = mes->next;
      mes->next = this;
   } else 
     actmap->message = this;

    for ( int i = 0; i < 8; i++ )
       if ( to & ( 1 << i )) {
          pmessagelist dst = new tmessagelist ( &actmap->player[ i ].unreadmessage );
          dst->message = this;
       }   
}


#endif

tmessage :: ~tmessage()
{
   if ( text ) {
      delete[] text;
      text = NULL;
   }
}


tmessagelist :: tmessagelist ( pmessagelist* prv )
{
   message = NULL;
   pmessagelist p = *prv;
   if ( p )
      while ( p->next )
         p = p->next;

   prev = p;

   if ( p ) {
      next = p->next;
      if ( next )
         next->prev = this;
      p->next = this;
   
   } else {
      next = NULL;
      *prv = this;
   }
   
}

tmessagelist :: ~tmessagelist ()
{

   if ( prev )
      prev->next = next;

   if ( next )
      next->prev = prev;

}

int tmessagelist :: getlistsize ( void )
{
   if ( next || prev )
      return 1;
   else
      return 0;
}

/*    
tfieldarray :: fieldl1 :: fieldl1 ( void )
{
   for (int i = 0; i < 1024; i++) {
      fielder[i] = NULL;
   } 
}


tfieldarray :: tfieldarray ( int size )
{
   fieldl2 = new fieldl1[ size / 1024 + 1 ];
}      

void tfieldarray :: alloc ( int size )
{
   fieldl2 = new fieldl1[ size / 1024 + 1 ];
}      

tfieldarray :: tfieldarray ( void )
{
   fieldl2 = NULL;
}      

void tfieldarray :: free ( void )
{
   if ( fieldl2 )
      delete fieldl2;
   fieldl2 = NULL;
}

tfieldarray :: ~tfieldarray()
{
//   fieldl2 = NULL;
//   delete fieldl2;
}
*/


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

tgameoptions :: tgameoptions ( void )
{
    setdefaults ( );
}

void tgameoptions :: setdefaults ( void )
{
   version = 102;
   fastmove = 1;
   mouse.scrollbutton = 0;
   mouse.fieldmarkbutton = 2;
   mouse.smallguibutton = 1;
   mouse.largeguibutton = -1;
   mouse.smalliconundermouse = 2;
   mouse.centerbutton = 4;
   container.autoproduceammunition = 1;
   smallguiiconopenaftermove = 0;
   startupcount = 0;

   visibility_calc_algo = 0;
   container.emptyeverything = 1;
   container.filleverything = 1;
   movespeed = 20;
   changed = 0;
   bi3.dir = NULL;
   strcpy ( filename, "sg.cfg" );
   onlinehelptime = 150;

   memset ( dummy, 0, sizeof ( dummy ));
   memset ( dummy2, 0, sizeof ( dummy2 ));
   memset ( container.dummy, 0, sizeof ( container.dummy ));
   memset ( mouse.dummy, 0, sizeof ( mouse.dummy ));

   bi3.interpolate.terrain = 1;
   bi3.interpolate.units = 0;
   bi3.interpolate.objects = 0;
   bi3.interpolate.buildings = 1; 

   marcfielsnotaccessible_movement = 0;

   mapzoom = 80;
}

int twind::operator== ( const twind& b ) const
{
   return ( (speed == b.speed) && ( direction == b.direction));
}



tnetworkcomputer :: tnetworkcomputer ( void )
{
   #ifdef sgmain
   send.transfermethod = defaultnetworkconnection;
   send.transfermethodid = defaultnetworkconnection->getid();
   memset ( send.data, 0, sizeof ( send.data ));

   receive.transfermethod = defaultnetworkconnection;
   receive.transfermethodid = defaultnetworkconnection->getid();
   memset ( receive.data, 0, sizeof ( receive.data ));
   #endif

   name = NULL;
   existent = 0;
}

tnetworkcomputer :: ~tnetworkcomputer ( )
{
   #ifdef sgmain
   if ( send.transfermethod ) {
      if ( send.transfermethod->transferopen () )
         send.transfermethod->closetransfer();
      if ( send.transfermethod->connectionopen () )
         send.transfermethod->closeconnection();
   }

   if ( receive.transfermethod ) {
      if ( receive.transfermethod->transferopen () )
         receive.transfermethod->closetransfer();
      if ( receive.transfermethod->connectionopen () )
         receive.transfermethod->closeconnection();
   }
   #endif

}


tnetwork :: tnetwork ( void )
{
  for (int i = 0; i < 8; i++) {
     player[i].compposition = 0;
     player[i].codewordcrc = 0;
  } /* endfor */
  computernum = 0;
  turn = 0;
  globalparams.enablesaveloadofgames = 0;
  globalparams.reaskpasswords = 0;
}

void      twterraintype  ::   paint ( int x1, int y1 )
{
   int i;
 #ifndef converter
  #ifdef HEXAGON
   putspriteimage ( x1, y1, picture[0] ); 
  #else
   char* c = (char*) direcpict[0];

   if ( agmp->windowstatus == 100 ) {
      char* buf = (char*) (agmp->scanlinelength * y1 + x1 + agmp->linearaddress) + 19;

      for (i=1; i<= 19 ;i++ ) {
        for ( int num = 0; num < i * 2; num ++ ) {
           *buf = *c;
           c++;
           buf++;
        }

        buf += agmp->scanlinelength - 2 * i - 1;
      } /* endfor */
                            
      for (i=20; i > 0 ;i-- ) {
        for ( int num = 0; num < i * 2; num ++ ) {
           *buf = *c;
           c++;
           buf++;
        }

        buf += agmp->scanlinelength - 2 * i + 1;
      } /* endfor */
      
   }
  #endif
 #endif
}

tobjectcontainercrcs :: tobjectcontainercrcs ( void )
{
   speedcrccheck = NULL;
}

tcrcblock :: tcrcblock ( void )
{
   crcnum = 0;
   crc = NULL;
   restricted = 0;
}

treplayinfo :: treplayinfo ( void )
{
   for (int i = 0; i < 8; i++) {
      guidata[i] = NULL;
      map[i] = NULL;
   }
   actmemstream = NULL;
}

treplayinfo :: ~treplayinfo ( )
{
   #ifndef converter
   for (int i = 0; i < 8; i++)  {
      if ( guidata[i] ) {
         delete guidata[i];
         guidata[i] = NULL;
      }
      if ( map[i] ) {
         delete map[i];
         map[i] = NULL;
      }
  }
  if ( actmemstream ) {
     delete actmemstream ;
     actmemstream = NULL;
  }
  #endif
}

int tterrainbits :: toand ( tterrainbits bts )
{
   return ( (bts.terrain1 & terrain1) || (bts.terrain2 & terrain2));
}


tterrainaccess :: tterrainaccess ( void )
                : terrain ( -1, -1 )
{  
   /*
   terrain.terrain1 = -1;
   terrain.terrain2 = -1;
   terrainreq.terrain1 = 0;
   terrainreq.terrain2 = 0;
   terrainnot.terrain1 = 0;
   terrainnot.terrain2 = 0;
   terrainkill.terrain1 = 0;
   terrainkill.terrain2 = 0;
   */
}

int tterrainaccess :: accessible ( tterrainbits bts )
{
   int i = 0;
   if ( terrain.toand ( bts )  
         &&
         bts.existall ( terrainreq )
         &&
        !terrainnot.toand ( bts )
      ) return 1;
   else
      if ( terrainkill.toand ( bts ) )
         return -1;
      else
         return 0;

}


tterrainbits cbwater0 ( 1<<22, 0 );
tterrainbits cbwater1 ( 1, 0 );
tterrainbits cbwater2 ( 4096, 0 );
tterrainbits cbwater3 ( 8192, 0 );
/*
tterrainbits cbwater ( cbwater0.terrain1 + cbwater1.terrain1 + cbwater2.terrain1 + cbwater3.terrain1, 
                       cbwater0.terrain2 + cbwater1.terrain2 + cbwater2.terrain2 + cbwater3.terrain2 );
*/


tterrainbits cbwater ( cbwater0 | cbwater1 | cbwater2 | cbwater3 );


tterrainbits cb_all ( -1, -1 );
tterrainbits cbstreet ( 32, 0 );
tterrainbits cbrailroad ( 64, 0 );
tterrainbits cbbuildingentry ( 128, 0 );
tterrainbits cbharbour ( 256, 0 );
tterrainbits cbrunway ( 512, 0 );
tterrainbits cbpipeline ( 1024, 0 );
tterrainbits cbpowerline ( 2048, 0 );
tterrainbits cbfahrspur ( 1<<16, 0 );
tterrainbits cbfestland ( cb_all  ^ cbwater );
tterrainbits cbsnow1 ( 1<<19, 0 );
tterrainbits cbsnow2 ( 1<<20, 0 );
tterrainbits cbhillside ( 1<<26, 0 );
tterrainbits cbsmallrocks ( 1<<17, 0 );
tterrainbits cblargerocks ( 1<<23, 0 );


               tterrainbits& operator~ ( tterrainbits &tb ) 
               { 
                  tterrainbits tb2 = tb;
                  tterrainbits tbs  ( ~(tb2.terrain1), ~(tb2.terrain2) );
                  // tterrainbits tbs2 ( ~tb.terrain1, ~tb.terrain2 );
                  #if (__WATCOM_CPLUSPLUS__ >= 1100 )
                     return tbs; 
                  #else
                     return tbs |= tbs;
                  #endif
               };


               tterrainbits& operator| ( tterrainbits tb2, tterrainbits tb3 ) 
               { 
                  tterrainbits tb = tb2;
                  return tb |=tb3;
               };
/*
               tterrainbits& operator& ( tterrainbits tb2, tterrainbits tb3 ) 
               { 
                  tterrainbits tb = tb2;
                  return tb &=tb3;
               };
*/
               int operator& ( tterrainbits tb2, tterrainbits tb3 ) 
               { 
                  return tb2.toand ( tb3 ) ;
               };
               tterrainbits& operator^ ( tterrainbits tb2, tterrainbits tb3 ) 
               { 
                  tterrainbits tb = tb2;
                  return tb ^=tb3;
               };





int tvehicletype :: vehicleloadable ( pvehicletype fzt )
{
   if (( ( loadcapabilityreq & fzt->height || !loadcapabilityreq ) && 
         ((loadcapabilitynot & fzt->height) == 0))
        || (fzt->functions & cf_trooper)) 

      if ( maxunitweight >= fzt->weight )
        return 1;
      return 0;
}


int    tbuildingtype :: vehicleloadable ( pvehicletype fzt )
{
      if ( (loadcapacity >= fzt->maxsize()  &&  (unitheightreq & fzt->height) && !(unitheight_forbidden & fzt->height) )  || 
           ( fzt->functions & cf_trooper ) )
           return 1;
      else
           return 0;
}



int tresearch :: technologyresearched ( int id )
{                                                        
      pdevelopedtechnologies devtech = developedtechnologies;
      while (devtech) {
         if (devtech->tech->id == id )
               return 1;
         devtech = devtech->next;
      } /* endwhile */

      return 0;
}


/*
tmap :: tmap ( void )
{

}

tmap :: tmap ( const tmap &map )
{

}

tmap :: ~tmap ( )
{
}
*/

int tmap :: getgameparameter ( int num )
{
  if ( game_parameter && num < gameparameter_num ) {

     if ( num == cgp_maxminesonfield ) 
        if ( game_parameter[num] > maxminesonfield )
           return maxminesonfield;

     return game_parameter[num];
  } else 
     if ( num < gameparameternum )
        return gameparameterdefault[ num ];
     else
        return 0;
}

void tmap :: setgameparameter ( int num, int value )
{
   if ( game_parameter ) {
     if ( num < gameparameter_num ) 
        game_parameter[num] = value;
     else {
        int* oldparam = game_parameter;
        game_parameter = new int[num+1];
        for ( int i = 0; i < gameparameter_num; i++ )
           game_parameter[i] = oldparam[i];
        for ( int j = gameparameter_num; j < num; j++ )
           if ( j < gameparameternum )
              game_parameter[j] = gameparameterdefault[j];
           else
              game_parameter[j] = 0;
        game_parameter[num] = value;
        gameparameter_num = num + 1;
        delete[] oldparam;
     }
   } else {
       game_parameter = new int[num+1];
       for ( int j = 0; j < num; j++ )
          if ( j < gameparameternum )
             game_parameter[j] = gameparameterdefault[j];
          else
             game_parameter[j] = 0;
       game_parameter[num] = value;
       gameparameter_num = num + 1;
   }
}

void tmap :: chainunit ( pvehicle eht )
{
   if ( eht ) {
      eht->next = player[ eht->color / 8 ].firstvehicle;
      if ( eht->next )
         eht->next->prev = eht;
      eht->prev = NULL;
      player[ eht->color / 8 ].firstvehicle = eht;
      if ( eht->typ->loadcapacity > 0) 
         for ( int i = 0; i <= 31; i++) 
            if ( eht->loading[i] ) 
               chainunit ( eht->loading[i] );
   }
}

void tmap :: chainbuilding ( pbuilding bld )
{
   if ( bld ) {
      bld->next = player[ bld->color / 8 ].firstbuilding;
      if ( bld->next )
         bld->next->prev = bld;
      bld->prev = NULL;
      player[ bld->color / 8 ].firstbuilding = bld;
   }
}


int tmap :: eventpassed( int saveas, int action, int mapid )
{ 
   return eventpassed ( (action << 16) | saveas, mapid );
}



int tmap :: eventpassed( int id, int mapid )
{ 
  pevent       ev2; 
  peventstore  oldevent; 
  boolean      b; 
  word         i; 

   b = false; 
   if ( !mapid ) {
      ev2 = firsteventpassed; 
      while (ev2 != NULL) { 
         if (ev2->id == id)
            return 1; 
         ev2 = ev2->next; 
      } 
   }

   if (b == false) { 
      oldevent = oldevents; 
      while ( oldevent ) { 
         if (oldevent->num > 0) 
            for (i = 0; i < oldevent->num ; i++) 
               if (oldevent->eventid[i] == id && oldevent->mapid[i] == mapid )
                  return 1; 
      } 
   } 
   return 0; 
} 


pvehicle tmap :: getunit ( pvehicle eht, int nwid )
{
   if ( !eht )
      return NULL;
   else {
      if ( eht->networkid == nwid )
         return eht;
      else
         for ( int i = 0; i < 32; i++ )
            if ( eht->loading[i] )
               if ( eht->loading[i]->networkid == nwid )
                  return eht->loading[i];
               else {
                  pvehicle ld = getunit ( eht->loading[i], nwid );
                  if ( ld )
                     return ld;
               }
      return NULL;
   }
}


pvehicle tmap :: getunit ( int x, int y, int nwid )
{
  #ifndef converter
   pfield fld  = getfield ( x, y );
   if ( !fld )
      return NULL;

   if ( !fld->vehicle )
      if ( fld->building ) {
         for ( int i = 0; i < 32; i++ ) {
            pvehicle ld = getunit ( fld->building->loading[i], nwid );
            if ( ld )
               return ld;
         }
         return NULL;
      } else
         return NULL;
   else
      if ( fld->vehicle->networkid == nwid )
         return fld->vehicle;
      else 
         return getunit ( fld->vehicle, nwid );
 #else
  return NULL;
 #endif
}



int tresearch :: vehicleclassavailable ( const pvehicletype fztyp , int classnm, pmap map  )
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

   for (i=0; i< waffenanzahl; i++)
      if ( fztyp->classbound[classnm].weapstrength[i] > unitimprovement.weapons[i] )
         return false;


   if ( fztyp->classbound[classnm].armor > unitimprovement.armor )
      return false;

   return true;

}


int tresearch :: vehicletypeavailable ( const pvehicletype fztyp, pmap map  )
{
   if ( !fztyp )
      return 0;
   else
      return vehicleclassavailable( fztyp, 0, map );
}


tshareview :: tshareview ( const tshareview* org ) 
{ 
   memcpy ( mode, org->mode, sizeof ( mode ));
   recalculateview = org->recalculateview; 
}

#ifdef converter
int tbuildingtype :: gettank ( int resource )
{
   return _tank.resource[resource];
}
#endif

teventtrigger_polygonentered :: teventtrigger_polygonentered ( void )
{
   size = 0;
   vehicle = NULL;
   vehiclenetworkid = 0;
   data = NULL;
   tempnwid = 0;
   tempxpos = 0;
   tempypos = 0;
   color = 0;
   memset ( reserved, 0, sizeof ( reserved ));
}

teventtrigger_polygonentered :: teventtrigger_polygonentered ( const teventtrigger_polygonentered& poly )
{
   size = poly.size;
   vehicle = poly.vehicle;
   vehiclenetworkid = poly.vehiclenetworkid;
   if ( poly.data ) {
      data = new int [size];
      for ( int i = 0; i < size; i++ )
         data[i] = poly.data[i];
   } else
      data = NULL;
   tempnwid = poly.tempnwid;
   tempxpos = poly.tempxpos;
   tempypos = poly.tempypos;
   color = poly.color;
   memcpy ( reserved, poly.reserved, sizeof ( reserved ));

}
teventtrigger_polygonentered :: ~teventtrigger_polygonentered ( )
{
   if ( data ) {
      delete[] data;
      data = NULL;
   }
}


LargeTriggerData :: LargeTriggerData ( void )
{
   time.abstime = 0;
   xpos = -1;
   ypos = -1;
   building = NULL;
   vehicle = NULL;
   mapid = 0;  
   id = -1;  
   unitpolygon = NULL;  
   networkid = 0;
   memset ( reserved, 0, sizeof ( reserved ));
}

LargeTriggerData :: LargeTriggerData ( const LargeTriggerData& data )
{
   time = data.time;
   xpos = data.xpos;
   ypos = data.ypos;
   networkid = data.networkid;
   building = data.building;
   vehicle = data.vehicle;
   mapid = data.mapid;
   id = data.id;
   if ( data.unitpolygon ) {
      unitpolygon = new teventtrigger_polygonentered ( *data.unitpolygon );
   } else
     unitpolygon = NULL;
}

LargeTriggerData :: ~LargeTriggerData ( )
{
   if ( unitpolygon ) {
      delete unitpolygon;
      unitpolygon = NULL;
   }
}

tevent :: tevent ( void )
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
      triggertime.a.turn = -1;
      triggertime.a.move = -1;
      trigger_data[i] = new LargeTriggerData;
   }
   delayedexecution.turn = 0;
   delayedexecution.move = 0;
   description[0] = 0;
   triggertime.abstime = -1;
}

tevent :: tevent ( const tevent& event )
{
   id = event.id;
   player = event.player;
   strcpy ( description, event.description );
   rawdata = event.rawdata;
   datasize  = event.datasize;
   if ( datasize && event.rawdata ) {
      chardata = new char [ datasize ];
      memcpy ( rawdata, event.rawdata, datasize );
   } else
      datasize = 0;
   next = event.next;
   conn = event.conn;
   for ( int i = 0; i < 4; i++ ) {
      trigger[i] = event.trigger[i];
      if ( event.trigger_data[i] )
         trigger_data[i] = new LargeTriggerData ( *event.trigger_data[i] );
      else
         trigger_data[i] = NULL;

      triggerconnect[i] = event.triggerconnect[i];
      triggerstatus[i] = event.triggerstatus[i]; 
   }
   triggertime = event.triggertime;
   delayedexecution = event.delayedexecution;
}


tevent :: ~tevent ()
{
   for ( int i = 0; i < 4; i++ )
      if ( trigger_data[i] ) {
        delete trigger_data[i];
        trigger_data[i] = NULL;
      }

   if ( chardata ) {
      delete[]  chardata ;
      chardata = NULL;
   }
}

// is there any way to do this test at compile time ??
structure_size_tester sst1;

