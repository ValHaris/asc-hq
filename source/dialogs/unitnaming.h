/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef unitnamingH
#define unitnamingH

#include "../paradialog.h"

class PG_LineEdit;
class ContainerBase;

class UnitNaming: public ASC_PG_Dialog {
      PG_LineEdit* publicName;
      PG_LineEdit* privateName;
      ContainerBase* unit;
         
      bool ok();
      bool cancel();
      static const int border  = 20;
      bool line1completed();
   public:
      UnitNaming ( ContainerBase* unit );
      int RunModal();
};




#endif
