//
// C++ Interface: resourceplacement
//
// Description:
//
//
// Author: Martin Bickel <bickel@asc-hq.org>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef RESOURCEPLACEMENT_H
#define RESOURCEPLACEMENT_H

#include <list>
#include "gamemap.h"

/**
@author Kevin Hirschmann
*/

struct Rect {
  pfield a, b, c, d;

};

typedef list<Rect> RectList;
/**
@author Kevin Hirschmann
@brief Fills a map with resources using the diamond-square algorithm
*/

class ResourcePlacement {
private:
  tmap& map;
  double fuelRoughness;
  double materialRoughness;
  bool placeFuel;
  bool placeMaterial;
  unsigned short maxFuelOffset;
  unsigned short maxMaterialOffset;
  int additionalResourceFreeFieldsPercentageFuel;
  int additionalResourceFreeFieldsPercentageMaterial;
  
  int stepCount;

  unsigned short createRandomValue(int limit);
  short createAlgebraicSign();
  
  void setFieldValueFuel(tfield* f);
  void setFieldValueMaterial(tfield* f);
  int calculateCornerValueFuel(pfield a, pfield b, pfield c);
  int calculateDiamondValueFuel(pfield a, pfield b, pfield c, pfield d);
  int calculateCornerValueMaterial(pfield a, pfield b, pfield c);
  int calculateDiamondValueMaterial(pfield a, pfield b, pfield c, pfield d);
  pfield calculateCornerPoint(pfield a, pfield b, pfield c);
  pfield calculateDiamondPoint(pfield a, pfield b, pfield c, pfield d);
  int calculateCurrentOffset(int offset);
  void step(Rect r);
  void runDS();

public:
  /**  
  @brief Constructor. Configures algorithm with algorithm determining parameters
  @param map The map which is filled with resources
  @param fuelRoughness Decides how large the offset (relative) for fuel between the fields may be. Value must be 0 < value < 4.0
         The higher the value the more "hills" and "valleys" you get
  @param materialRoughness Decides how large the offset (relative) for material between neighbouring fields may be. Value must be 0 < value < 4.0
         The higher the value the more "hills" and "valleys" you get
  @param maxFuelOffSet Determines the absolut offset fuelFields might have
  @param maxMaterialOffSet Determines the absolut offset materialFields might have
  */
  ResourcePlacement(tmap& map, double fuelRoughness, double materialRoughness, unsigned short maxFuelOffSet, unsigned short maxMaterialOffSet, 
                    int additionalFreeFieldsPercFuel = 0, int additionalFreeFieldsPercMaterial = 0);
  /**  
  @brief Destructor
  */
  ~ResourcePlacement();
  /**  
  @brief Fills the map with all resources 
  */
  void placeResources();
  /**  
  @brief Fills the map only with fuel resources
  */
  void placeFuelResources();
  /**  
  @brief Fills the map only with material resources
  */
  void placeMaterialResources();
  
  static const int MAXFUELVALUE;
  static const int MINFUELVALUE;
  static const int MAXMATERIALVALUE;
  static const int MINMATERIALVALUE;

};

#endif


