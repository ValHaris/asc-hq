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

const int ResourcePlacement::MAXFUELVALUE = 255;
const int ResourcePlacement::MAXMATERIALVALUE = 255;
const int ResourcePlacement::MINFUELVALUE = 0;
const int ResourcePlacement::MINMATERIALVALUE = 0;

ResourcePlacement::ResourcePlacement(tmap& m,double fr, double mr, unsigned short maxFOffset, unsigned short maxMOffset, int addFreeFieldsPercFuel, int addFreeFieldsPercMaterial): map(m), fuelRoughness(fr), materialRoughness(fr), maxFuelOffset(maxFOffset), maxMaterialOffset(maxMOffset), stepCount(0),
additionalResourceFreeFieldsPercentageFuel(addFreeFieldsPercFuel), additionalResourceFreeFieldsPercentageMaterial(addFreeFieldsPercMaterial) {
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
  pfield e = map.getField((map.xsize -1)/2, (map.ysize -1)/2);  //First diamond point
  pfield d = map.getField(0, map.ysize -1 );
  if(placeFuel) {   
    a->temp3 = createRandomValue(MAXFUELVALUE * (1 + additionalResourceFreeFieldsPercentageFuel/100 ));    
    b->temp3 = createRandomValue(MAXFUELVALUE * (1 + additionalResourceFreeFieldsPercentageFuel/100 ));    
    c->temp3 = createRandomValue(MAXFUELVALUE * (1 + additionalResourceFreeFieldsPercentageFuel/100 ));    
    d->temp3 = createRandomValue(MAXFUELVALUE * (1 + additionalResourceFreeFieldsPercentageFuel/100 ));
    
    
    setFieldValueFuel(a);    
    setFieldValueFuel(b);
    setFieldValueFuel(c);
    setFieldValueFuel(d);
  }
  if(placeMaterial) {
    c->temp4 = createRandomValue(MAXMATERIALVALUE * (1 + additionalResourceFreeFieldsPercentageMaterial/100 ));
    d->temp4 = createRandomValue(MAXMATERIALVALUE * (1 + additionalResourceFreeFieldsPercentageMaterial/100 ));
    a->temp4 = createRandomValue(MAXMATERIALVALUE * (1 + additionalResourceFreeFieldsPercentageMaterial/100 ));
    b->temp4 = createRandomValue(MAXMATERIALVALUE * (1 + additionalResourceFreeFieldsPercentageMaterial/100 ));
    
    setFieldValueMaterial(a);    
    setFieldValueMaterial(b);
    setFieldValueMaterial(c);
    setFieldValueMaterial(d);
  }
  Rect r ={a, b, c, d};
  step(r);
}




pfield ResourcePlacement::calculateCornerPoint(pfield a, pfield b, pfield diamondPoint) {
  int x = 0;
  int y = 0;  
  if((a->getx() == b->getx())) {
    x = a->getx();
    
  } else {    
    x = diamondPoint->getx();     
  }  
  if(a->gety() == b->gety()){
    y = a->gety();
  }else{
     y = diamondPoint->gety();
  }
  pfield cornerPoint = map.getField(x,y);
  if(placeFuel) {
    cornerPoint->temp3  = calculateCornerValueFuel(a, b, diamondPoint);
    setFieldValueFuel(cornerPoint);
  }
  if(placeMaterial) {
    cornerPoint->temp4  = calculateCornerValueMaterial(a, b, diamondPoint);
    setFieldValueMaterial(cornerPoint);
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
    //cout << "r.a: " << r.a->getx() << "," << r.a->gety() << " f: " << f->getx() << "," << f->gety() << " diamondPoint: " << diamondPoint->getx() << "," << diamondPoint->gety() << "i: " << i->getx() << "," << i->gety() << endl;
    step(r1);
  }
  if(!((r.b->getx()-f->getx()<2)&&(diamondPoint->gety() - r.b->gety() <2))) {
    //cout << " f: " << f << "r.b: " << r.b << "g: " << g  <<" diamondPoint: " << diamondPoint <<  endl;
    step(r2);
  }
  if(!((g->getx() - diamondPoint->getx() < 2) && (r.c->gety()-g->gety()<2))) {
    //cout << "g: " << g->getx() << "," << g->gety()<< " diamond: " << diamondPoint->getx()<< "," << diamondPoint->gety() << " c: " << r.c->getx()<< "," << r.c->gety() << " h: " << h->getx() << "," << h->gety() << endl;
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
  x =  static_cast<int>(xd) + a->getx();
  y = static_cast<int>(yd)  + a->gety();
  pfield e = map.getField(x, y);
  if(placeFuel) {
    e->temp3 = calculateDiamondValueFuel(a, b, c, d);
    setFieldValueFuel(e);
    
  }
  if(placeMaterial) {
    e->temp4  = calculateDiamondValueMaterial(a, b, c, d);
    setFieldValueMaterial(e);
  }
  return e;
}

unsigned short ResourcePlacement::createRandomValue(int limit) {
  if(limit == 0){
    return 1;
  }
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
  int value = (a->temp3 + b->temp3 + c->temp3)/3 + createRandomValue(static_cast<int>(calculateCurrentOffset(maxFuelOffset) * fuelRoughness))* createAlgebraicSign();
  if(value > MAXFUELVALUE * (1 + additionalResourceFreeFieldsPercentageFuel / 100)) {
    value = MAXFUELVALUE * (1 + additionalResourceFreeFieldsPercentageFuel /100);
  } else if( value < MINFUELVALUE) {
    value = MINFUELVALUE;
  }  
  return value;
}


int ResourcePlacement::calculateDiamondValueFuel(pfield a, pfield b, pfield c, pfield d) {  
  int value = static_cast<int>((a->temp3 + b->temp3 + c->temp3 + d->temp3)/4 + createRandomValue(static_cast<int>(calculateCurrentOffset(maxFuelOffset) * fuelRoughness)) * createAlgebraicSign());
  if(value > MAXFUELVALUE * (1 + additionalResourceFreeFieldsPercentageFuel / 100)) {
    value = MAXFUELVALUE * (1 + additionalResourceFreeFieldsPercentageFuel /100);
  } else if( value < MINFUELVALUE) {
    value = MINFUELVALUE;
  }
  return value;
}

int ResourcePlacement::calculateCornerValueMaterial(pfield a, pfield b, pfield c) {
  int value = (a->temp4 + b->temp4 + c->temp4)/3 + createRandomValue(static_cast<int>(calculateCurrentOffset(maxMaterialOffset) * materialRoughness ) * createAlgebraicSign());
  if(value > MAXMATERIALVALUE * (1 + additionalResourceFreeFieldsPercentageMaterial / 100)) {
    value = MAXMATERIALVALUE * (1 + additionalResourceFreeFieldsPercentageMaterial / 100);
  } else if( value < MINMATERIALVALUE) {
    value = MINMATERIALVALUE;
  }
  return value;
}

int ResourcePlacement::calculateDiamondValueMaterial(pfield a, pfield b, pfield c, pfield d) {
  int value = (a->temp4 + b->temp4 + c->temp4 + d->temp4)/4 + createRandomValue(static_cast<int>(calculateCurrentOffset(maxMaterialOffset)* materialRoughness)) * createAlgebraicSign();
  if(value > MAXMATERIALVALUE * (1 + additionalResourceFreeFieldsPercentageMaterial / 100)) {
    value = MAXMATERIALVALUE * (1 + additionalResourceFreeFieldsPercentageMaterial / 100);
  } else if( value < MINMATERIALVALUE) {
    value = MINMATERIALVALUE;
  }
  return value;
}

int ResourcePlacement::calculateCurrentOffset(int currentOffset) {
  return  currentOffset / stepCount;
}

void ResourcePlacement::setFieldValueFuel(tfield* f){   
   int value = f->temp3 -( MAXFUELVALUE * additionalResourceFreeFieldsPercentageFuel/100);
   if(value < MINFUELVALUE) {    
     f->fuel = MINFUELVALUE;
   }else{
     f->fuel = value;
   }                
}

void ResourcePlacement::setFieldValueMaterial(tfield* f){   
   int value = f->temp4 -( MAXMATERIALVALUE * additionalResourceFreeFieldsPercentageMaterial/100);
   if(value < MINMATERIALVALUE) {    
     f->material = MINMATERIALVALUE;
   }else{
     f->material = value;
   }                
}

