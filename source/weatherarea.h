//
// C++ Interface: weatherarea
//
// Description: 
//
//
// Author: Kevin Hirschmann <hirsch@dhcppc0>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef WEATHERAREA_H
#define WEATHERAREA_H

#include <list>
#include <map>
#include <set>
#include "gamemap.h"
#include "error.h"
#include "ascstring.h"
#include "geometry.h"
#include "basestreaminterface.h"
#include "graphics/ascttfont.h"

class IllegalValueException: public ASCmsgException{
public:
IllegalValueException(const ASCString& msg);
~IllegalValueException();
};


enum Direction {
N,
NE,
E,
SE,
S,
SW,
W,
NW,

};

enum FalloutType {
DRY,
LRAIN,
HRAIN,
LSNOW,
HSNOW,
ICE
};

struct WindData{
  unsigned int speed;
  Direction direction;
};



typedef map<int, WindData> WindChanges;
typedef multiset<tfield*> FieldSet;
class WeatherSystem;
class WeatherField;


typedef list<tfield*> MapFields;
typedef vector<WeatherField*> WeatherFields;
typedef vector<int> Percentages;


/**
@author Kevin Hirschmann
*/
class WeatherArea{
private:
WeatherFields area;
tmap* map;
int duration;
int width;
int height;
int radius;
FalloutType ft;
Vector2D currentMovement;
Point2D center;
int counter;

WeatherArea &operator=(const WeatherArea&);
WeatherArea(const WeatherArea&);
void updateMovementVector(unsigned int speed, Direction windDirection, double ratio);
Point2D calculateFieldPosition(Point2D center, Vector2D relPos);

public:
    WeatherArea(tmap* map, int xCenter, int yCenter, int width, int height);
    WeatherArea(tmap* map, int xCenter, int yCenter, int radius);
    WeatherArea(tmap* map);
    
    ~WeatherArea();
    inline tmap* getMap() const;
    
    Vector2D getWindVector() const;
    void setWindVector(unsigned int speed, Direction windDirection);
    
    void setFalloutType(FalloutType fallout);    
    inline FalloutType getFalloutType() const;
    
    int getDuration() const;
    void setDuration(int duration);
    
    void update(WeatherSystem* wSystem, FieldSet& processedFields);                
    void placeArea();
    void removeArea(FieldSet& processedFields);
    
    void write (tnstream& outputStream) const;
    void read (tnstream& inputStream);
    
    
    
};

typedef multimap<int, WeatherArea*> WeatherAreas;

enum WeatherSystemMode {
EVENTMODE,
RANDOMMODE
};

class WeatherField{
private:
tfield* mapField;
tmap* map;
Point2D posInArea;
int counter;

void setMapField(tfield* mapField);
public:
WeatherField(tmap* map);
WeatherField(Point2D mapPos, const WeatherArea* area);
~WeatherField();
void move(const Vector2D& vector);
bool isOnMap(const tmap* map) const;
void update(const WeatherArea*, FieldSet& processedFields);
void reset(FieldSet& processedFields);
void write (tnstream& outputStream) const;
void read (tnstream& inputStream);
};

class WeatherSystem{
private:

static const int weather_version = 1;
float seedValue;
int timeInterval;
int areaSpawnAmount;
WeatherSystemMode currentMode;

float windspeed2FieldRatio;
int windspeed;
Direction globalWindDirection;

float lowerRandomSize;
float upperRandomSize;


WeatherAreas weatherAreas;
WindChanges windTriggers;
FieldSet processedFields;

Percentages falloutPercentages;
Percentages windDirPercentages;
Percentages windSpeedPercentages;

tmap* gameMap;


WeatherSystem(const WeatherSystem&);
WeatherSystem& operator=(const WeatherSystem&);
void randomWeatherChange(int currentTurn, Direction windDirection);
Direction randomWindChange(int currentTurn);
unsigned short createRandomValue(int limit) const;
float createRandomValue(float lowerlimit, float upperlimit) const;

public:
  static const int FallOutNum = 6;
  static const int WindDirNum = 8;
  
  WeatherSystem(tmap* map);
  WeatherSystem(tmap* map, int areaSpawns, float windspeed2FieldRatio, unsigned int timeInterval = 1, WeatherSystemMode mode = EVENTMODE);
  ~WeatherSystem();
  inline void setSeedValue();
  void setLikelihoodFallOut(const Percentages& fol) throw (IllegalValueException);
  void setLikelihoodWindDirection(const Percentages&  wd) throw (IllegalValueException);
  void setLikelihoodWindSpeed(const Percentages&  wd) throw (IllegalValueException);
  void setRandomSizeBorders(float lower, float upper);
  void setGlobalWind(unsigned int speed, Direction direction) throw (IllegalValueException);
  void addWeatherArea(WeatherArea* area, FalloutType fallout, unsigned int turn, unsigned int duration);
  void addGlobalWindChange(int speed, Direction direction, unsigned int turn) throw (IllegalValueException);  
  void update(int currentTurn);
  void write (tnstream& outputStream) const;
  void read (tnstream& inputStream);
  
  
  inline int getSpawnsAmount(){
    return areaSpawnAmount;
  }
  
  inline void setEventMode(const WeatherSystemMode& mode){
    currentMode = mode;
  }
  
  inline int getTimeInterval(){
    return timeInterval;
  }
  
  inline const WeatherSystemMode getEventMode() const{
    return currentMode;
  }
  inline float getWindspeed2FieldRatio() const{
    return windspeed2FieldRatio;
  }
  inline const int getFalloutPercentage(int i) const{
    return falloutPercentages[i];
  }
  
  inline const int getWindSpeedPercentage(int i) const{
    return windSpeedPercentages[i];
  }
  
  inline const int getWindDirPercentage(int i) const{
    return windDirPercentages[i];
  }
};

#endif

