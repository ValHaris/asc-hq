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


class tfight {
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
                     int einkeilung;
                     int weapnum;
                     int weapcount;
                     int color;
                     int initiative;
                  } av, dv;

           void calc ( void ) ;
           void calcdisplay ( int ad = -1, int dd = -1 );
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
           void setup ( pvehicle &attackingunit, pvehicle &attackedunit, int respond, int weapon );
           void setresult ( void );

      };

class tunitattacksbuilding : public tfight {
           pvehicle _attackingunit; 
           pbuilding _attackedbuilding; 
           int _x, _y;
           void paintimages ( int xa, int ya, int xd, int yd );
         public:
           void setup ( pvehicle attackingunit, int x, int y, int weapon );
           void setresult ( void );

      };


class tmineattacksunit : public tfight {
            pfield _mineposition;
            pvehicle _attackedunit;
            pvehicle* _pattackedunit;
            void paintimages ( int xa, int ya, int xd, int yd );
         public:
           void setup ( pfield mineposition, pvehicle &attackedunit );
           void setresult ( void );

      };

class tunitattacksobject : public tfight {
           pvehicle     _attackingunit; 
           pobjectcontainer      _object; 
           pobject _obji;
           void paintimages ( int xa, int ya, int xd, int yd );
           int _x, _y;
         public:
           void setup ( pvehicle attackingunit, int obj_x, int obj_y, int weapon );
           void setresult ( void );
      };


extern byte         attackstrength(byte         damage);


#endif



