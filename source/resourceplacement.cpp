//
// C++ Implementation: resourceplacement
//
// Description:
//
//
// Author: Martin Bickel <bickel@asc-hq.org>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>

#include "resourceplacement.h"


const int MAXFUELVALUE = 255;
const int MAXMATERIALVALUE = 255;
const int MINFUELVALUE = 0;
const int MINMATERIALVALUE = 0;

ResourcePlacement::ResourcePlacement(tmap& m,double fr, double mr, unsigned short maxFOffset, unsigned short maxMOffset): map(m), fuelRoughness(fr), materialRoughness(fr), maxFuelOffset(maxFOffset), maxMaterialOffset(maxMOffset), stepCount(0) {
  double depth = 0.0;
  srand(static_cast<unsigned>(time(0)));
  if(map.xsize > map.ysize) {
    depth = map.ysize;
  } else {
    depth = map.xsize ;
  }
  if(fuelRoughness > 4.0)
    fuelRoughness = 4.0;
  else if(fuelRoughness < 0.1)
    fuelRoughness = 0.1;

  if(materialRoughness > 4.0)
    materialRoughness = 4.0;
  else if(materialRoughness < 0.1)
    materialRoughness = 0.1;
}


ResourcePlacement::~ResourcePlacement() {}

void ResourcePlacement::placeResources() {
  placeFuel = true;
  placeMaterial = true;
  runDS();
}

void ResourcePlacement::placeFuelResources() {
  placeFuel = true;
  placeMaterial = false;
  runDS();
}

void ResourcePlacement::placeMaterialResources() {
  placeFuel = false;
  placeMaterial = true;
  runDS();
}


void ResourcePlacement::runDS() {
  pfield a = map.getField(0,0);

  pfield b = map.getField( map.xsize -1, 0 );
  pfield c = map.getField(map.xsize -1, map.ysize -1);
  pfield e = map.getField((map.xsize -1)/2, (map.ysize -1)/2);
  pfield d = map.getField(0, map.ysize -1 );
  if(placeFuel) {
    a->fuel = createRandomValue(MAXFUELVALUE);
    d->fuel = createRandomValue(MAXFUELVALUE);
    b->fuel = createRandomValue(MAXFUELVALUE);
    c->fuel = createRandomValue(MAXFUELVALUE);
  }
  if(placeMaterial) {
    c->material = createRandomValue(MAXMATERIALVALUE);
    d->material = createRandomValue(MAXMATERIALVALUE);
    a->material = createRandomValue(MAXMATERIALVALUE);
    b->material = createRandomValue(MAXMATERIALVALUE);
  }
  Rect r ={a, b, c, d};
  step(r);
}


pfield ResourcePlacement::calculateCornerPoint(pfield a, pfield b, pfield diamondPoint) {
  int x = 0;
  int y = 0;  
  if((a->getx() == b->getx())) {
    x = a->getx();
    y = diamondPoint->gety();    
  } else {
    x = diamondPoint->getx();
    y = a->gety();
  }  
  pfield cornerPoint = map.getField(x,y);
  if(placeFuel) {
    cornerPoint->fuel  = calculateCornerValueFuel(a, b, diamondPoint);
  }
  if(placeMaterial) {
    cornerPoint->material  = calculateCornerValueMaterial(a, b, diamondPoint);
  }
  return cornerPoint;
}

void ResourcePlacement::step(Rect r) {
  ++stepCount;
  pfield diamondPoint = calculateDiamondPoint(r.a, r.b, r.c, r.d);
  pfield f = calculateCornerPoint(r.a, r.b, diamondPoint);
  pfield g = calculateCornerPoint(r.b, r.c, diamondPoint);
  pfield h = calculateCornerPoint(r.d, r.c, diamondPoint);
  pfield i = calculateCornerPoint(r.a, r.d, diamondPoint);

  Rect r1 ={r.a, f, diamondPoint, i};
  Rect r2 ={f, r.b, g, diamondPoint};
  Rect r3 ={diamondPoint, g, r.c, h};
  Rect r4 ={i, diamondPoint, h, r.d};

  if(!((f->getx()- r.a->getx()<2)&& (diamondPoint->gety()-f->gety()<2))) {
    step(r1);
  }
  if(!((r.b->getx()-f->getx()<2)&&(diamondPoint->gety() - r.b->gety() <2))) {
    step(r2);
  }
  if(!((g->getx() - diamondPoint->getx() < 2) && (r.c->gety()-g->gety()<2))) {
    step(r3);
  }
  if(!((diamondPoint->getx()-i->getx()<2)&&(r.d->gety()-diamondPoint->gety()<2))) {
    step(r4);
  }
  --stepCount;
}


pfield ResourcePlacement::calculateDiamondPoint(pfield a, pfield b, pfield c, pfield d) {
  int x = 0;
  int y = 0;
  double xd = (b->getx() - a->getx())/2;
  double yd = (d->gety() - a->gety())/2;
  x =  xd + a->getx();
  y = yd  + a->gety();
  pfield e = map.getField(x, y);
  if(placeFuel) {
    e->fuel  = calculateDiamondValueFuel(a, b, c, d);
  }
  if(placeMaterial) {
    e->material  = calculateDiamondValueMaterial(a, b, c, d);
  }
  return e;
}

unsigned short ResourcePlacement::createRandomValue(int limit) {
  int random_integer = rand();
  return (random_integer % limit);
}

short ResourcePlacement::createAlgebraicSign() {
  int random_integer = rand();
  random_integer = random_integer % 2;
  if(random_integer == 0)
    return -1;
  else
    return 1;
}

int ResourcePlacement::calculateCornerValueFuel(pfield a, pfield b, pfield c) {
  int value = (a->fuel + b->fuel + c->fuel)/3 + createRandomValue(calculateCurrentOffset(maxFuelOffset) * fuelRoughness)* createAlgebraicSign();
  if(value > MAXFUELVALUE) {
    value = MAXFUELVALUE;
  } else if( value < MINFUELVALUE) {
    value = MINFUELVALUE;
  }
  return value;
}

int ResourcePlacement::calculateDiamondValueFuel(pfield a, pfield b, pfield c, pfield d) {
  int value = (a->fuel + b->fuel + c->fuel + d->fuel)/4 + createRandomValue(calculateCurrentOffset(maxFuelOffset)* fuelRoughness) * createAlgebraicSign();
  if(value > MAXFUELVALUE) {
    value = MAXFUELVALUE;
  } else if( value < MINFUELVALUE) {
    value = MINFUELVALUE;
  }       
  return value;
}

int ResourcePlacement::calculateCornerValueMaterial(pfield a, pfield b, pfield c) {
  int value = (a->material + b->material + c->material)/3 + createRandomValue(calculateCurrentOffset(maxMaterialOffset) * materialRoughness)* createAlgebraicSign();
  if(value > MAXMATERIALVALUE) {
    value = MAXMATERIALVALUE;
  } else if( value < MINMATERIALVALUE) {
    value = MINMATERIALVALUE;
  }
  return value;
}

int ResourcePlacement::calculateDiamondValueMaterial(pfield a, pfield b, pfield c, pfield d) {
  int value = (a->material + b->material + c->material + d->material)/4 + createRandomValue(calculateCurrentOffset(maxMaterialOffset)* materialRoughness) * createAlgebraicSign();
  if(value > MAXMATERIALVALUE) {
    value = MAXMATERIALVALUE;
  } else if( value < MINMATERIALVALUE) {
    value = MINMATERIALVALUE;
  }
  return value;
}

int ResourcePlacement::calculateCurrentOffset(int currentOffset) {
  return  currentOffset / stepCount;
}

