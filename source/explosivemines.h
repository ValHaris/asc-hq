
/***************************************************************************
                          gamemap.h  -  description
                             -------------------
    begin                : Tue Feb 17 2001
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef explosiveminesH
 #define explosiveminesH


 #include "objects.h"
 #include "basestreaminterface.h"
 



class Mine : public AgeableItem {
      Mine();
   public:
      Mine( MineTypes type, int strength, int player, GameMap* gamemap );

      Mine( MineTypes type, int strength, int player, GameMap* gamemap, int identifier );

      static Mine newFromStream ( tnstream& stream );
      
      //! a unique identifier that allows the identification of a single mine
      int identifier;
      
      MineTypes type;

      //! the effective punch of the mine
      int strength;

      //! the player who placed the mine; range 0 .. 7
      int player;

      //! can the mine attack this unit
      bool attacksunit ( const Vehicle* veh );
      
      //! display the mine
      void paint( Surface& surf, SPoint pos );

      void read ( tnstream& stream );
      void write ( tnstream& stream );
      
};



#endif

