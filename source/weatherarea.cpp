//
// C++ Implementation: weatherarea
//
// Description:
//
//
// Author: Kevin Hirschmann <hirsch@dhcppc0>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <iostream>
#include <ctime>
#include "weatherarea.h"
#include "geometry.h"
IllegalValueException::IllegalValueException(const ASCString& msg):ASCmsgException(msg) {}

IllegalValueException::~IllegalValueException() {}


//****************************************************************************************************************************************

WeatherArea::WeatherArea(tmap* m, int xCenter, int yCenter, int w, int h): map(m), ft(DRY), center(xCenter, yCenter), width(w), height(h) {
  int x =  0;
  int y =  0;
  counter = 0;
  WeatherField* wf;
  for(int i = 0; i < w * h; i++) {
    x =  i % width;
    y =  i / width;
    wf = new WeatherField(Point2D(x + (center.getX() - width/2), y + (center.getY() - height/2)), this);
    area.push_back(wf);
  }
}

WeatherArea::WeatherArea(tmap* m): map(m) {}

WeatherArea::WeatherArea(tmap* m, int xCenter, int yCenter, int r):map(m), center(xCenter, yCenter), width(r), height(r), radius(r) {}

WeatherArea::~WeatherArea() {
  cout << " Deleting area" << endl;

}

tmap* WeatherArea::getMap() const {
  return map;
}

void WeatherArea::setWindVector(unsigned int speed, Direction wd) {
  updateMovementVector(speed, wd, 1.0);
}

void WeatherArea::setFalloutType(FalloutType fallout) {
  ft = fallout;
}

void WeatherArea::updateMovementVector(unsigned int windspeed, Direction windDirection, double ratio) {
  cout << "updating movement vector" << endl;
  if(windDirection == N) {
    currentMovement = Vector2D(0, -1 * ratio * windspeed);
  } else if(windDirection == NE) {
    currentMovement = Vector2D(ratio * windspeed, -1 * ratio * windspeed);
  } else if(windDirection == SE) {
    currentMovement = Vector2D(ratio * windspeed, ratio * windspeed);
  } else if(windDirection == S) {
    currentMovement = Vector2D(0,  ratio * windspeed);
  } else if(windDirection == SW) {
    currentMovement = Vector2D(-1 * ratio * windspeed, ratio * windspeed);
  } else if(windDirection == NW) {
    currentMovement = Vector2D(-1 * ratio * windspeed, -1 * ratio * windspeed);
  } else if(windDirection == W) {
    currentMovement = Vector2D(-1 * ratio * windspeed, 0);
  } else if(windDirection == E) {
    currentMovement = Vector2D(ratio * windspeed, 0);
  }

}


void WeatherArea::update(WeatherSystem* wSystem, FieldSet& processedFields) {
  duration--;
  if(currentMovement.isZeroVector()) {
    return;
  }
  center.move(currentMovement.getXComponent(), currentMovement.getYComponent());
  for(int i = 0; i < area.size(); i++) {
    WeatherField* wf = area[i];
    if(wf->isOnMap(map)) {
      wf->reset(processedFields);
    }
  }
  for(int i = 0; i < area.size(); i++) {
    WeatherField* wf = area[i];
    wf->move(currentMovement);
    center.move(getWindVector());
    if(wf->isOnMap(map)) {
      wf->update(this, processedFields);
    }
  }
}

void WeatherArea::write (tnstream& outputStream) const {
  outputStream.writeInt(duration);
  outputStream.writeInt(width);
  outputStream.writeInt(height);
  outputStream.writeInt(center.getX());
  outputStream.writeInt(center.getY());  
  outputStream.writeInt(currentMovement.getXComponent());
  outputStream.writeInt(currentMovement.getYComponent());  
  outputStream.writeInt(ft);
  outputStream.writeInt ( area.size() );
  for ( WeatherFields::const_iterator i = area.begin(); i != area.end(); i++ ) {
    (*i)->write ( outputStream );
  }
}

void WeatherArea::read (tnstream& inputStream) {
  duration = inputStream.readInt();
  width = inputStream.readInt();
  height = inputStream.readInt();
  center = Point2D(inputStream.readInt(), inputStream.readInt());
  int xmov = inputStream.readInt();
  int ymov = inputStream.readInt();  
  currentMovement = Vector2D(xmov, ymov );
  ft = static_cast<FalloutType>(inputStream.readInt());
  int size = inputStream.readInt();
  for ( int i = 0; i < size; i++ ) {
    WeatherField* newWf = new WeatherField(map);
    newWf->read(inputStream);
    area.push_back(newWf);
  }
}

Vector2D WeatherArea::getWindVector() const {
  return currentMovement;
}

FalloutType WeatherArea::getFalloutType()const {
  return ft;
}

void WeatherArea::placeArea() {
  FieldSet processedFields;
  for(int i = 0; i < area.size(); i++) {
    WeatherField* wf = area[i];
    if(wf->isOnMap(map)) {
      wf->update(this, processedFields);
    }
  }
}

void WeatherArea::removeArea(FieldSet& processedFields) {
  for(int i = 0; i < area.size(); i++) {
    WeatherField* wf = area[i];
    if(wf->isOnMap(map)) {
      wf->reset(processedFields);
      delete wf;
    }
  }

}

int WeatherArea::getDuration() const {
  return duration;
}

void WeatherArea::setDuration(int d) {
  duration = d;
}
//******************************************************************************************************************************************

WeatherField::WeatherField(tmap* m):mapField(0), map(m) {}

WeatherField::WeatherField(Point2D mapPos, const WeatherArea* area):posInArea(mapPos), mapField(0) {
  if(isOnMap(area->getMap())) {
    mapField = area->getMap()->getField(posInArea.getX(), posInArea.getY());
  }
  counter = 0;
}

WeatherField::~WeatherField() {}

void WeatherField::move(const Vector2D& vector) {
  posInArea.move(vector.getXComponent(), vector.getYComponent());
}

void WeatherField::setMapField(tfield* field) {
  mapField = field;
}

bool WeatherField::isOnMap(const tmap* map) const {
  if((posInArea.getX()>=0) && (posInArea.getX() < map->xsize)&&
      (posInArea.getY()>=0) && (posInArea.getY() < map->ysize))
    return true;
  else
    return false;

}

void WeatherField::reset(FieldSet& processedFields) {
  if(processedFields.find(mapField)== processedFields.end()) {
    mapField->setweather(DRY);
    mapField->setparams();
  }
}
void WeatherField::update(const WeatherArea* area, FieldSet& processedFields) {
  mapField = area->getMap()->getField(posInArea.getX(), posInArea.getY());
  mapField->setweather(area->getFalloutType());
  mapField->setparams();
  processedFields.insert(mapField);
}

void WeatherField::write (tnstream& outputStream) const {
  outputStream.writeInt(posInArea.getX());
  outputStream.writeInt(posInArea.getY());

}

void  WeatherField::read(tnstream& inputStream) {
  int x = inputStream.readInt();
  int y = inputStream.readInt();
  posInArea.set(x,y);
  if(isOnMap(map))
    mapField = map->getField(x,y);
}

//******************************************************************************************************************************************
WeatherSystem::WeatherSystem(tmap* map, int spawn, float ws2fr, unsigned int tInterval, WeatherSystemMode mode):timeInterval(tInterval), windspeed2FieldRatio(ws2fr), areaSpawnAmount(spawn), gameMap(map), currentMode(mode), windspeed(2), globalWindDirection(S), lowerRandomSize(0.25), upperRandomSize(0.8) {
  Percentages defaultFallOut;
  int sum = 0;
  for(int i = 0; i < FallOutNum - 1; i++) {
    int weight = 100 / FallOutNum;
    defaultFallOut.push_back(weight);
    sum += weight;
  }
  defaultFallOut.push_back(100 - sum);
  setLikelihoodFallOut(defaultFallOut);

  sum = 0;
  Percentages defaultWindDirections;
  for(int i = 0; i < WindDirNum - 1; i++) {
    int weight = (100 / WindDirNum);
    defaultWindDirections.push_back(weight);
    sum += weight;
  }
  defaultWindDirections.push_back(100 - sum);
  setLikelihoodWindDirection(defaultWindDirections);

  sum = 0;
  Percentages defaultWindSpeeds;
  for(int i = 0; i < 9; i++) {
    int weight = (100 / 10);
    defaultWindSpeeds.push_back(weight);
    sum += weight;
  }

  defaultWindSpeeds.push_back(100 - sum);
  setLikelihoodWindSpeed(defaultWindSpeeds);
}

WeatherSystem::WeatherSystem(tmap* map):gameMap(map) {}

WeatherSystem::~WeatherSystem() {
  WeatherAreas::iterator it;
  for(it = weatherAreas.begin(); it!= weatherAreas.end(); it++) {
    WeatherArea* area2Delete = it->second;
    weatherAreas.erase(it);
    delete area2Delete;
  }
}



void WeatherSystem::setLikelihoodFallOut(const Percentages& fol)
throw (IllegalValueException) {
  int sum = 0;
  for(int i=0; i < fol.size();i++) {    
    sum += fol[i];   
  }
  if(sum != 100) {
    throw IllegalValueException("Total percentages for Fallouts not equal 100");
  }  
  falloutPercentages.resize(fol.size());
  for(int i=0; i < fol.size();i++) {    
    falloutPercentages[i] = fol[i];
  }  
}


void WeatherSystem::setLikelihoodWindDirection(const Percentages&  wd) throw (IllegalValueException) {
  int sum = 0;
  for(int i=0; i < wd.size();i++) {
    int value = wd[i];
    windDirPercentages.push_back(value);
    sum += value;
  }
  if(sum != 100) {
    throw IllegalValueException("Total percentages for WindDirections do not equal 100");
  }
}

void WeatherSystem::setLikelihoodWindSpeed(const Percentages&  ws) throw (IllegalValueException) {
  int sum = 0;
  for(int i=0; i < ws.size();i++) {
    int value = ws[i];
    windSpeedPercentages.push_back(value);
    sum += value;
  }
  if(sum != 100) {
    throw IllegalValueException("Total percentages for WindSpeed do not equal 100");
  }


}

void WeatherSystem::setRandomSizeBorders(float lower, float upper) {
  lowerRandomSize = lower;
  upperRandomSize = upper;
}


void WeatherSystem::setGlobalWind(unsigned int ws, Direction d) throw (IllegalValueException) {
  windspeed = ws;
  globalWindDirection = d;
}

void WeatherSystem::addWeatherArea(WeatherArea* area, FalloutType fallout, unsigned int turn, unsigned int duration) {
  area->setFalloutType(fallout);
  area->setDuration(duration);
  pair<int, WeatherArea*>p(turn, area);
  weatherAreas.insert(p);
}

Direction WeatherSystem::randomWindChange(int currentTurn) {
  int newWindDirection = createRandomValue(100);
  int newWindSpeed = 3;//createRandomValue(120);
  Direction newDirection;
  int sum = 0;
  for(int i = 0; i < windDirPercentages.size(); i++) {
    sum += windDirPercentages[i];
    if(newWindDirection <= sum) {
      newDirection = static_cast<Direction>(i);
      break;
    }
  }
  addGlobalWindChange(newWindSpeed, newDirection, currentTurn);
  return newDirection;
}

void WeatherSystem::randomWeatherChange(int currentTurn, Direction windDirection) {
  WeatherArea* newArea;
  int xpos;
  int ypos;
  int width = gameMap->xsize * createRandomValue(lowerRandomSize, upperRandomSize);
  cout << " chosen xsize: " << width << endl;
  int height = gameMap->ysize * createRandomValue(lowerRandomSize, upperRandomSize);
  cout << " chosen ysize: " << height << endl;
  //Determine where the new weatherfield is placed
  if(windDirection == N) {
    ypos = gameMap->ysize;
    xpos = createRandomValue(gameMap->xsize);
  } else if(windDirection == S) {
    ypos = 0;
    xpos = createRandomValue(gameMap->xsize);
  } else if(windDirection == E) {
    xpos = 0;
    xpos = createRandomValue(gameMap->ysize);
  } else if(globalWindDirection == W) {
    ypos = gameMap->xsize;
    xpos = createRandomValue(gameMap->ysize);
  } else {
    int sideDecision = createRandomValue(1);
    if(windDirection == NE) {
      if(sideDecision == 0) {
        ypos = gameMap->ysize;
        xpos = createRandomValue(gameMap->xsize);
      } else {
        ypos = createRandomValue(gameMap->ysize);
        xpos = 0;
      }
    } else if(windDirection == SE) {
      if(sideDecision == 0) {
        ypos = 0;
        xpos = createRandomValue(gameMap->xsize);
      } else {
        ypos = createRandomValue(gameMap->ysize);;
        xpos = 0;
      }
    } else if(windDirection == SW) {
      if(sideDecision == 0) {
        ypos = 0;
        xpos = createRandomValue(gameMap->xsize);
      } else {
        ypos = createRandomValue(gameMap->ysize);;
        xpos = gameMap->xsize;
      }
    } else if(windDirection == NW) {
      if(sideDecision == 0) {
        ypos = gameMap->ysize;
        xpos = createRandomValue(gameMap->xsize);
      } else {
        ypos = createRandomValue(gameMap->ysize);;
        xpos = gameMap->xsize;
      }
    }
  }  
  newArea = new WeatherArea(gameMap, xpos, ypos, width, height);
  FalloutType ft;
  int sum = 0;
  int newFallout = createRandomValue(100);
  for(int i = 0; i < falloutPercentages.size(); i++) {
    sum += falloutPercentages[i];
    if(newFallout <= sum) {
      ft = static_cast<FalloutType>(i);
      cout << "FalloutType " << i << endl;
      break;
    }
  }
  this->addWeatherArea(newArea, ft, currentTurn, 6);
}
void WeatherSystem::update(int currentTurn) {
  int result = (currentTurn % timeInterval);
  if((currentMode == RANDOMMODE)  && (currentTurn % timeInterval == 0)) {
    Direction newDirection = randomWindChange(currentTurn);
    for(int i=0; i < areaSpawnAmount; i++){ 
      randomWeatherChange(currentTurn, newDirection);
    }
  }
  WeatherAreas::iterator lower = weatherAreas.lower_bound(0);
  WeatherAreas::iterator medium = weatherAreas.upper_bound(currentTurn-1);
  WeatherAreas::iterator upper = weatherAreas.upper_bound(currentTurn);
  WeatherAreas::iterator it;
  WindChanges::iterator itTr = windTriggers.find(currentTurn);
  if(itTr != windTriggers.end()) {
    setGlobalWind(itTr->second.speed , itTr->second.direction );
  }
  for(it = lower; it != medium; it++) {
    if(it->second->getDuration()==0) {
      WeatherArea* area2Delete = it->second;
      weatherAreas.erase(it);
      area2Delete->removeArea(processedFields);
      delete area2Delete;
    } else {
      it->second->update(this, processedFields);
      it->second->setWindVector(windspeed, globalWindDirection);
    }
  }
  for(it = medium; it != upper; it++) {
    it->second->setWindVector(windspeed , globalWindDirection);
    it->second->placeArea();
  }
  processedFields.clear();
}


void WeatherSystem::setSeedValue() {
  seedValue = time(0);

}

unsigned short WeatherSystem::createRandomValue(int limit) const {
  int random_integer = rand();
  return (random_integer % limit);
}

float WeatherSystem::createRandomValue(float lowerLimit, float upperLimit) const {
  return (lowerLimit + (float)rand()/(float)RAND_MAX * (upperLimit - lowerLimit));
}

void WeatherSystem::addGlobalWindChange(int speed, Direction direction, unsigned int turn) throw (IllegalValueException) {
  WindData data ={speed, direction};
  pair<int, WindData>p(turn, data);
  windTriggers.insert(p);
}

void WeatherSystem::write(tnstream& outputStream) const {
  cout << "Writing weahter" << endl;
  outputStream.writeInt(weather_version );
  cout << "weatherversion: " << weather_version << endl;
  outputStream.writeFloat( seedValue );  
  cout << "seedValue: " << seedValue << endl;
  outputStream.writeInt( timeInterval );
  outputStream.writeInt(areaSpawnAmount);
  outputStream.writeInt(currentMode);
  
  
  outputStream.writeInt(windspeed );
  outputStream.writeFloat( windspeed2FieldRatio );
  outputStream.writeInt(globalWindDirection );
  
  outputStream.writeFloat(lowerRandomSize);
  outputStream.writeFloat(upperRandomSize);
   
  outputStream.writeInt(falloutPercentages.size());  
  cout << "FalloutPer SIZE: " << falloutPercentages.size() << endl;
  for (int i = 0; i < falloutPercentages.size(); i++ ){
    outputStream.writeInt(falloutPercentages[i]);    
    cout << "FalloutPer: " << falloutPercentages[i] << endl;
  }
  
  outputStream.writeInt(windDirPercentages.size());
  for (int i  = 0; i < windDirPercentages.size(); i++ ){
    outputStream.writeInt(windDirPercentages[i]);    
  }
  
  outputStream.writeInt(windSpeedPercentages.size());
  for (int i = 0; i < windSpeedPercentages.size(); i++ ){
    outputStream.writeInt(windSpeedPercentages[i]);    
  }

  outputStream.writeInt(weatherAreas.size());
  for ( WeatherAreas::const_iterator i = weatherAreas.begin(); i != weatherAreas.end(); ++i ) {
    outputStream.writeInt(i->first);
    i->second->write ( outputStream );
  }
  outputStream.writeInt( 123456 );
}

void WeatherSystem::read (tnstream& inputStream) {

  int version = inputStream.readInt();
  cout << "weatherversion: " << version << endl;
  seedValue = inputStream.readFloat();
  cout << "seedValue: " << seedValue << endl;
  timeInterval = inputStream.readInt();
  areaSpawnAmount = inputStream.readInt();
  currentMode  = static_cast<WeatherSystemMode>(inputStream.readInt());
  cout << "currentmode is " << currentMode << endl;
  
  windspeed = inputStream.readInt();
  windspeed2FieldRatio = inputStream.readFloat();  
  globalWindDirection = static_cast<Direction>(inputStream.readInt());
  
  lowerRandomSize = inputStream.readFloat();
  upperRandomSize = inputStream.readFloat();
  
  int size = 0;

  size= inputStream.readInt();
  for(int i = 0; i < size; i++) {
    falloutPercentages.push_back(inputStream.readInt());    
  }
  
  size= inputStream.readInt();
  for(int i = 0; i < size; i++) {
    windDirPercentages.push_back(inputStream.readInt());    
  }
  
  size= inputStream.readInt();
  for(int i = 0; i < size; i++) {
    windSpeedPercentages.push_back(inputStream.readInt());    
  }
  
  size= inputStream.readInt();
  for(int i = 0; i < size; i++) {
    int turn = inputStream.readInt();    
    WeatherArea* newArea = new WeatherArea(gameMap);
    newArea->read(inputStream);
    pair<int, WeatherArea*>p(turn, newArea);
    weatherAreas.insert(p);    
  }
  cout << "check: " << inputStream.readInt() << endl;
}



