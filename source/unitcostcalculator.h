/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef unitcostcalculationH
#define unitcostcalculationH

class VehicleType;
class Resources;
class ASCString;

class UnitCostCalculator {
   public:
      virtual Resources productionCost( const VehicleType* vehicle ) = 0;
      virtual ASCString getCalculatorName() = 0;
      virtual ~UnitCostCalculator() {};
};

#endif