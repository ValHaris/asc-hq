//     $Id: attack.h,v 1.9 2000-07-16 14:19:59 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.8  2000/07/06 11:07:25  mbickel
//      More AI work
//      Started modularizing the attack formula
//
//     Revision 1.7  2000/06/08 21:03:39  mbickel
//      New vehicle action: attack
//      wrote documentation for vehicle actions
//
//     Revision 1.6  2000/06/04 21:39:18  mbickel
//      Added OK button to ViewText dialog (used in "About ASC", for example)
//      Invalid command line parameters are now reported
//      new text for attack result prediction
//      Added constructors to attack functions
//
//     Revision 1.5  2000/04/27 16:25:14  mbickel
//      Attack functions cleanup
//      New vehicle categories
//      Rewrote resource production in ASC resource mode
//      Improved mine system: several mines on a single field allowed
//      Added unitctrl.* : Interface for vehicle functions
//        currently movement and height change included
//      Changed timer to SDL_GetTicks
//
//     Revision 1.4  2000/01/24 08:16:49  steb
//     Changes to existing files to implement sound.  This is the first munge into
//     CVS.  It worked for me before the munge, but YMMV :)
//
//     Revision 1.3  2000/01/20 16:52:09  mbickel
//      Added Kamikaze attack
//
//     Revision 1.2  1999/11/16 03:41:04  tmwilson
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



#ifndef attack_h
#define attack_h

#include "typen.h"


struct   tattackresult {
               byte adamage, ddamage, adifferenz, ddifferenz;
            };


typedef struct tattackresult* pattackresult;


class AttackFormula {
            int checkHemming ( pvehicle d_eht, int direc );
         public:
            float experience ( int experience );
            float damage ( int damage );
            float attackbonus ( int abonus );
            float hemming ( int  ax,  int ay,  pvehicle d_eht );
        };

class tfight : public AttackFormula {
           void paintbar ( int num, int val, int col );
           void paintline ( int num, int val, int col );
           virtual void paintimages ( int xa, int ya, int xd, int yd ) = 0;
        protected:   
           int synchronedisplay;
        public:  
           struct tavalues {
                     int strength;
                     int armor;
                     int damage;
                     int experience;
                     int defensebonus;
                     int attackbonus;
                     float hemming;
                     int weapnum;
                     int weapcount;
                     int color;
                     int initiative;
                     int kamikaze;
                  } av, dv;

           void calc ( void ) ;
           virtual void calcdisplay ( int ad = -1, int dd = -1 );
           virtual void setresult ( void ) = 0;
           tfight ( void );
      };

class tunitattacksunit : public tfight {
           pvehicle _attackingunit; 
           pvehicle _attackedunit; 

           pvehicle* _pattackingunit; 
           pvehicle* _pattackedunit; 

           int _respond; 
           void paintimages ( int xa, int ya, int xd, int yd );
         public:
           tunitattacksunit ( pvehicle &attackingunit, pvehicle &attackedunit, int respond, int weapon );
           void setup ( pvehicle &attackingunit, pvehicle &attackedunit, int respond, int weapon );
           void setresult ( void );
           virtual void calcdisplay(int ad = -1, int dd = -1);

      };

class tunitattacksbuilding : public tfight {
           pvehicle _attackingunit; 
           pbuilding _attackedbuilding; 
           int _x, _y;
           void paintimages ( int xa, int ya, int xd, int yd );
         public:
           tunitattacksbuilding ( pvehicle attackingunit, int x, int y, int weapon );
           void setup ( pvehicle attackingunit, int x, int y, int weapon );
           void setresult ( void );

      };


class tmineattacksunit : public tfight {
            pfield _mineposition;
            pvehicle _attackedunit;
            int _minenum;
            pvehicle* _pattackedunit;
            void paintimages ( int xa, int ya, int xd, int yd );
         public:
           tmineattacksunit ( pfield mineposition, int minenum, pvehicle &attackedunit );
           void setup ( pfield mineposition, int minenum, pvehicle &attackedunit );
           void setresult ( void );

      };

class tunitattacksobject : public tfight {
           pvehicle     _attackingunit; 
           pobjectcontainer      _object; 
           pobject _obji;
           void paintimages ( int xa, int ya, int xd, int yd );
           int _x, _y;
         public:
           tunitattacksobject ( pvehicle attackingunit, int obj_x, int obj_y, int weapon );
           void setup ( pvehicle attackingunit, int obj_x, int obj_y, int weapon );
           void setresult ( void );
      };


   class AttackWeap { 
               public:
                    int          count; 
                    int          strength[16]; 
                    int          num[16]; 
                    int          typ[16];

                    enum Target { nothing, vehicle, building, object } target;
                 }; 

   typedef class AttackWeap* pattackweap ;


extern pattackweap attackpossible( const pvehicle  angreifer, int x, int y);

extern int attackpossible2u( const pvehicle     angreifer,
                             const pvehicle     verteidiger, pattackweap attackweap = NULL);      // distance is not evaluated 

extern int attackpossible28( const pvehicle     angreifer,
                             const pvehicle     verteidiger, pattackweap attackweap = NULL);       // distance is fixed as 1 field

extern int attackpossible2n( const pvehicle     angreifer,
                             const pvehicle     verteidiger, pattackweap attackweap = NULL );       // actual distance is used 

extern int vehicleplattfahrbar( const pvehicle     vehicle,
                                const pfield        field );


class WeapDist { 
            char         data[7][256];        /* mg,bomb,gmissile,amissile,torpedo,cannon,cruise missile  */ 
         public:
            void loaddata ( void ) ;
            float getWeapStrength ( const SingleWeapon* weap, int dist =-1, int attacker_height =-1, int defender_height = -1, int reldiff = -1 );
         }; 

extern WeapDist weapDist;


#endif



