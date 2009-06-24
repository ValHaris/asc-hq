/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef gotopositionH
#define gotopositionH

#include "../paradialog.h"

class PG_LineEdit;
class GameMap;

class GotoPosition: public ASC_PG_Dialog {
      PG_LineEdit* xfield;
      PG_LineEdit* yfield;
      GameMap* gamemap;
      
      bool ok();
      bool cancel();
      static const int border  = 20;
      bool line1completed();
   public:
      GotoPosition ( GameMap* gamemap );
      int RunModal();
};




#endif
