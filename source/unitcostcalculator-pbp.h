/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef unitcostcalculatorPBPH
#define unitcostcalculatorPBPH

#include "unitcostcalculator.h"

class PBPUnitCostCalculator : public UnitCostCalculator {
   public:
      Resources productionCost( const VehicleType* vehicle );
      ASCString getCalculatorName();
};

#endif
