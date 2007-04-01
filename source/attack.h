/*! \file attack.h
    \brief Interface for all the fighting routines of ASC. 
*/


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



#ifndef attackH
#define attackH

#include "typen.h"
#include "vehicletype.h"
#include "vehicle.h"
#include "buildings.h"
#include "objects.h"
#include "explosivemines.h"




class AttackFormula
{
      virtual bool checkHemming ( Vehicle* d_eht, int direc );
   public:
      float strength_experience ( int experience );
      float strength_damage ( int damage );
      float strength_attackbonus ( int abonus );
      float strength_hemming ( int  ax,  int ay,  Vehicle* d_eht );
      float defense_experience ( int experience );
      float defense_defensebonus ( int defensebonus );
      static float getHemmingFactor ( int relDir );  // 0 <= reldir <= sidenum-2
      virtual ~AttackFormula()
      {}
      ;
};



class tunitattacksobject;
class tmineattacksunit;
class tunitattacksbuilding;
class tunitattacksunit;

class FightVisitor
{
   public:
      virtual void visit( tunitattacksobject& battle ) = 0;
      virtual void visit( tmineattacksunit& battle ) = 0;
      virtual void visit( tunitattacksbuilding& battle ) = 0;
      virtual void visit( tunitattacksunit& battle ) = 0;
      virtual ~FightVisitor()
      {}
      ;
};


class tfight : public AttackFormula
{
   protected:
      tfight ( void );

   public:
      struct tavalues
      {
         tavalues() : weapontype(-1)
         {}
         ;
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
         int height;
         int weapontype;
      }
      av, dv;

      //! Performs the calculation of the attack. The result is only stored in the av and dv structures and is not written to the map
      void calc ( void ) ;

      //! Writes the result of the attack calculation to the actual units.
      virtual void setresult ( void ) = 0;

      virtual void visit ( FightVisitor& visitor ) = 0;

      virtual int getAttackingPlayer() = 0;
      virtual int getDefendingPlayer() = 0;
      
};

class UnitAttacksSomething : public tfight
{
   protected:
      Vehicle* _attackingunit;
      int getAttackingPlayer()
      {
         return _attackingunit->getOwner();
      };
   public:
      Vehicle* getAttackingUnit()
      {
         return _attackingunit;
      };
};

class tunitattacksunit : public UnitAttacksSomething
{
      Vehicle* _attackedunit;

      Vehicle** _pattackingunit;
      Vehicle** _pattackedunit;

      int _respond;
      bool reactionfire;

   protected:

   public:
      /*! Calculates the fight if one unit attacks another units.
          \param respond Does the unit that is being attacked retalliate ?
          \param weapon  The number of the weapon which the attacking unit attacks with. If it is -1, the best weapon is chosen.
      */
      tunitattacksunit ( Vehicle* &attackingunit, Vehicle* &attackedunit, bool respond = true, int weapon = -1, bool reactionfire = false );
      void setup ( Vehicle* &attackingunit, Vehicle* &attackedunit, bool respond, int weapon );
      void setresult ( void );

      void visit ( FightVisitor& visitor )
      {
         visitor.visit( *this );
      };
      Vehicle* getTarget()
      {
         return _attackedunit;
      };

      int getDefendingPlayer()
      {
         return _attackedunit->getOwner();
      };
      
};

class tunitattacksbuilding : public UnitAttacksSomething
{
      Building* _attackedbuilding;
      int _x, _y;
   public:
      int getAttackingPlayer()
      {
         return _attackingunit->getOwner();
      };
      int getDefendingPlayer()
      {
         return _attackedbuilding->getOwner();
      };

      /*! Calculates the fight if one unit attacks the building at coordinate x/y.
          \param weapon  The number of the weapon which the attacking unit attacks with. If it is -1, the best weapon is chosen.
      */
      tunitattacksbuilding ( Vehicle* attackingunit, int x, int y, int weapon = -1);
      void setup ( Vehicle* attackingunit, int x, int y, int weapon );
      void setresult ( void );

      void visit ( FightVisitor& visitor )
      {
         visitor.visit( *this );
      };
      Building* getTarget()
      {
         return _attackedbuilding;
      };
};


class tmineattacksunit : public tfight
{
      tfield* _mineposition;
      Vehicle* _attackedunit;
      int _minenum;
      Vehicle** _pattackedunit;

   public:
      int getAttackingPlayer()
      {
         return 8;
      };
      int getDefendingPlayer()
      {
         return _attackedunit->getOwner();
      };


      /*! Calculates the fight if a unit drives onto a mine.
          \param mineposition The field on which the mine was placed
          \param minenum The number of a specific mine which explodes. If -1 , all mines on this field which are able to attack the unit will explode.
          \param attackedunit The unit which moved onto the minefield.
      */
      tmineattacksunit ( tfield* mineposition, int minenum, Vehicle* &attackedunit );
      void setup ( tfield* mineposition, int minenum, Vehicle* &attackedunit );
      void setresult ( void );

      Mine* getFirstMine();

      void visit ( FightVisitor& visitor )
      {
         visitor.visit( *this );
      };

      Vehicle* getTarget()
      {
         return _attackedunit;
      };
};

class tunitattacksobject : public UnitAttacksSomething
{
      tfield*       targetField;
      Object*      _obji;
      int _x, _y;

   public:
      int getAttackingPlayer()
      {
         return _attackingunit->getOwner();
      };
      int getDefendingPlayer()
      {
         return 8;
      };

      /*! Calculates the fight if one unit attacks the objects at coordinate x/y.
          \param attackingunit the attacking unit
          \param obj_x the X coordinate of the object on the map
          \param obj_y the Y coordinate of the object on the map
          \param weapon  The number of the weapon which the attacking unit attacks with. If it is -1, the best weapon is chosen.
      */
      tunitattacksobject ( Vehicle* attackingunit, int obj_x, int obj_y, int weapon = -1 );
      void setup ( Vehicle* attackingunit, int obj_x, int obj_y, int weapon );
      void setresult ( void );
      Object* getTarget()
      {
         return _obji;
      };
      void visit ( FightVisitor& visitor )
      {
         visitor.visit( *this );
      };
};

//! Structure to store the weapons which a unit can use to perform an attack. \sa attackpossible
class AttackWeap
{
   public:
      int          count;
      int          strength[16];
      int          num[16];
      int          typ[16];

      enum Target { nothing, vehicle, building, object } target;
};
typedef class AttackWeap* pattackweap ;




//! \brief Is attacker able to attack anything in field x/y ?
extern pattackweap attackpossible( const Vehicle* attacker, int x, int y);



/*! \brief Is attacker able to attack target ? Distance is not evaluated.
 
     The distance is not evaluated. The routine is used for the movement routines for example,
     because the current distance of units A and B is not relevant for the check whether unit
     A can move across the field where B is standing.
 
     \param attacker the attacking unit
     \param target   the unit that is being attacked
     \param attackweap if != NULL, detailed information about the weapons which can perform
                          the attack are written to attackweap
     \param targetHeight if != -1 , assume the target unit was on this height (bitmapped!)
 */
extern bool attackpossible2u( const Vehicle* attacker, const Vehicle* target, pattackweap attackweap = NULL, int targetheight = -1);      // distance is not evaluated


/*! \brief Is attacker able to attack target ? Distance is assumed one field.
 
     The distance is assumed to be 1 field. The routine is used for the movement routines for 
     example, because units moving next to enemies get a movement malus.
     Does not evaluate any diplomatic states
 
     \param attacker the attacking unit
     \param target   the unit that is being attacked
     \param attackweap if != NULL, detailed information about the weapons which can perform
                          the attack are written to attackweap
     \param targetHeight if != -1 , assume the target unit was on this height (bitmapped!)
*/
extern bool attackpossible28( const Vehicle* attacker, const Vehicle* target, pattackweap attackweap = NULL, int targetHeight = -1);       // distance is fixed as 1 field


/*! \brief Is attacker able to attack target ? Actual distance used.
 
     \param attacker the attacking unit
     \param target   the unit that is being attacked
     \param attackweap if != NULL, detailed information about the weapons which can perform
                          the attack are written to attackweap
*/
extern bool attackpossible2n( const Vehicle* attacker, const Vehicle* target, pattackweap attackweap = NULL );

//! Can the vehicle drive across the field and destroy any unit there by moving over them?
extern bool vehicleplattfahrbar( const Vehicle* vehicle, const tfield* field );


//! Some very old system to calculate the weapon efficiency over a given distance.
class WeapDist
{
   public:
      static float getWeapStrength ( const SingleWeapon* weap, int dist =-1, int attacker_height =-1, int defender_height = -1, int reldiff = -1 );
};


#endif



