/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef unitcostcalculatorstandardH
#define unitcostcalculatorstandardH

#include "unitcostcalculator.h"

class StandardUnitCostCalculator : public UnitCostCalculator {
   public:
      Resources productionCost( const VehicleType* vehicle );
      ASCString getCalculatorName();
};

#endif