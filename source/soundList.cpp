/*! \file soundList.cpp
    \brief The collection of all sounds used by ASC
*/

#include <stdio.h>
#include <stdlib.h>

#ifndef _DOS_
 #include "global.h"
 #include sdlheader
#endif

#include "soundList.h"
#include "basestrm.h"

SoundList* SoundList::instance = NULL;

const int soundNum = 28;

struct {
          SoundList::Sample sample;
          int subType;
          char* name;
          Sound* snd;
      } sounds[soundNum] = { { SoundList::shooting, 0, "SHOOT.CRUISEMISSILE", NULL },
                             { SoundList::shooting, 1, "SHOOT.MINE", NULL },
                             { SoundList::shooting, 2, "SHOOT.BOMB", NULL },
                             { SoundList::shooting, 3, "SHOOT.AIRMISSILE", NULL },
                             { SoundList::shooting, 4, "SHOOT.GROUNDMISSILE", NULL },
                             { SoundList::shooting, 5, "SHOOT.TORPEDO", NULL },
                             { SoundList::shooting, 6, "SHOOT.MACHINEGUN", NULL },
                             { SoundList::shooting, 7, "SHOOT.CANNON", NULL },
                             { SoundList::shooting, 10, "SHOOT.LASER", NULL },
                             { SoundList::moving,   0, "MOVE.DEFAULT", NULL },
                             { SoundList::moving,   1, "MOVE.LIGHT_TRACKED_VEHICLE", NULL },
                             { SoundList::moving,   2, "MOVE.MEDIUM_TRACKED_VEHICLE", NULL },
                             { SoundList::moving,   3, "MOVE.HEAVY_TRACKED_VEHICLE", NULL },
                             { SoundList::moving,   4, "MOVE.LIGHT_WHEELED_VEHICLE", NULL},
                             { SoundList::moving,   5, "MOVE.MEDIUM_WHEELED_VEHICLE", NULL },
                             { SoundList::moving,   6, "MOVE.HEAVY_WHEELED_VEHICLE", NULL },
                             { SoundList::moving,   7, "MOVE.TROOPER", NULL },
                             { SoundList::moving,   8, "MOVE.RAIL_VEHICLE", NULL },
                             { SoundList::moving,   9, "MOVE.MEDIUM_AIRCRAFT", NULL },
                             { SoundList::moving,  10, "MOVE.MEDIUM_SHIP", NULL },
                             { SoundList::moving,  11, "MOVE.TURRET", NULL },
                             { SoundList::moving,  12, "MOVE.LIGHT_AIRCRAFT", NULL },
                             { SoundList::moving,  13, "MOVE.HEAVY_AIRCRAFT", NULL },
                             { SoundList::moving,  14, "MOVE.LIGHT_SHIP", NULL },
                             { SoundList::moving,  15, "MOVE.HEAVY_SHIP", NULL },
                             { SoundList::menu_ack,  0, "MENU.ACKNOWLEDGE", NULL },
                             { SoundList::menu_fail, 0, "MENU.FAIL", NULL },
                             { SoundList::building_conq,  0, "BUILDING.CONQUER", NULL }
                           };



class StringTokenizer {
       ASCString& str;
       int i;
   private:
       int CharSpace ( char c );
   public:
       StringTokenizer ( ASCString & _str ) : str( _str ), i ( 0 ) {};
       ASCString getNextToken ( );
};


int StringTokenizer::CharSpace ( char c )
{
  if ( c <= ' ' )
     return 0;

  const char* ops = "=*/+-";
  const char* d = ops;
  do {
     if( *d == c )
        return 2;
     if ( *d == 0 )
        return 1;
     d++;
  } while ( true );
}

ASCString StringTokenizer::getNextToken( )
{
   while ( i < str.length() && !CharSpace(str[i]) )
     i++;

   if ( i == str.length() )
      return "";

   int begin = i;
   int cs = CharSpace( str[i] );
   do {
      i++;
   } while ( i < str.length() && CharSpace( str[i] ) == cs );
   return str.substr(begin, i-begin);
}


SoundList& SoundList::getInstance()
{
   if ( !instance )
      fatalError("SoundList::getInstance() - Soundlist not initialized");

   return *instance;
}


void SoundList::init( )
{
   if ( instance )
      fatalError("SoundList::init() - Soundlist already initialized");

   instance = new SoundList;
   instance->initialize ( );
}

void SoundList::initialize(  )
{
   const ASCString separator = "=";
   const ASCString filename = "sounds.txt";

   typedef map<ASCString, ASCString> SoundSetup;
   SoundSetup soundSetup;
   {
      tnfilestream list ( filename, tnstream::reading );
      bool cont;
      ASCString line;
      int linenumber = 0;
      do {
         linenumber++;
         cont = list.readTextString ( line );
         if ( !line.empty() && line[0] != '#' && line[0] != ';' ) {
            StringTokenizer tok ( line );
            ASCString snd = tok.getNextToken( );
            snd.toUpper();
            ASCString op = tok.getNextToken( );
            ASCString file = tok.getNextToken( );
            if ( !file.empty() && op==separator )
               soundSetup[snd] = file;
            else
               if ( op.empty() )
                  warning( "error parsing file " + filename + " , line " + strrr (linenumber ));
         }
      } while ( cont );
   }

   for ( SoundSetup::iterator i = soundSetup.begin(); i != soundSetup.end(); i++ ) {
      Sound* s = NULL;
      if ( !i->second.empty() )
         if ( soundFiles.find ( i->second ) == soundFiles.end() ) {
            s = new Sound ( i->second );
            soundFiles[i->second] = s;
         }

      for ( int n = 0; n < soundNum; n++ )
         if ( i->first == ASCString( sounds[n].name ) )
            sounds[n].snd =  s;
   }
}



Sound* SoundList::getSound( Sample snd, int subType )
{
   if ( SoundSystem::getInstance()->isOff() )
      return NULL;

   // once we have some more sounds this could be optimized to be faster than O(n)

   for ( int i = 0; i < soundNum; i++ )
      if ( snd == sounds[i].sample && subType == sounds[i].subType )
         if ( sounds[i].snd )
            return sounds[i].snd;

   return NULL;
}

Sound* SoundList::play( Sample snd, int subType , bool looping )
{
   Sound* sound = getSound ( snd, subType );
   if ( !sound )
      return NULL;

   if ( looping )
      sound->playLoop();
   else
      sound->play();

   return sound;
}

SoundList::~SoundList()
{
   for ( SoundFiles::iterator i = soundFiles.begin(); i != soundFiles.end(); i++ )
      delete i->second;
}

