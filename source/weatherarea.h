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
//#include "error.h"
#include "ascstring.h"
#include "geometry.h"
#include "typen.h"
#include "basestreaminterface.h"
#include "graphics/ascttfont.h"

class IllegalValueException: public ASCmsgException{
public:
IllegalValueException(const ASCString& msg);
~IllegalValueException();
};

extern const char*  cdirections[];
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

struct WeatherRect {
  int a, b, c, d;

};

typedef map<int, WindData> WindChanges;
typedef multiset<tfield*> FieldSet;
class WeatherSystem;
class WeatherField;


typedef list<tfield*> MapFields;
typedef vector<WeatherField*> WeatherFields;
typedef vector<int> Percentages;

struct ltGTime{
bool operator()(const GameTime& gt1, const GameTime& gt2) const{
  return gt1.abstime < gt2.abstime;
}

};
/**
@author Kevin Hirschmann
*/
class WeatherArea{
private:
WeatherFields area;
tmap* map;
Point2D center;
int duration;
int width;
int height;
int radius;
FalloutType ft;
Vector2D currentMovement;
int stepCount;
static const int MAXVALUE;
static const int MAXOFFSET;
float horizontalWindAccu;
float verticalWindAccu;

int seedValue;
WeatherArea &operator=(const WeatherArea&);
WeatherArea(const WeatherArea&);
Point2D calculateFieldPosition(Point2D center, Vector2D relPos);


short createAlgebraicSign();
void step(WeatherRect r);
int calculateCornerPoint(int a, int b, int c);
int calculateDiamondPoint(int a, int b, int c, int d);
int calculateCurrentOffset(int currentOffset);
int calculateCornerPointValue(int a, int b, int c);
int calculateDiamondPointValue(int a, int b, int c, int d);
void createWeatherFields();

public:
    WeatherArea(tmap* map, int xCenter, int yCenter, int width, int height, int duration, FalloutType fType, unsigned int seedValue);
    WeatherArea(tmap* map, int xCenter, int yCenter, int radius);
    WeatherArea(tmap* map);
    
    ~WeatherArea();
    inline tmap* getMap() const;
    
    Vector2D getWindVector() const;
   // void setWindVector(unsigned int speed, Direction windDirection);
    void updateMovementVector(unsigned int speed, Direction windDirection, double ratio);
    void setFalloutType(FalloutType fallout);    
    FalloutType getFalloutType() const;
    FalloutType getFalloutType(int value) const;
    
    Point2D getCenterPos(){
      return center;
    }
    
    int getDuration() const;
    void setDuration(int duration);
    
    void update(WeatherSystem* wSystem, FieldSet& processedFields);                
    void placeArea();
    void removeArea(FieldSet& processedFields);
    unsigned int createRandomValue(int limit); 
    
    void write (tnstream& outputStream) const;
    void read (tnstream& inputStream);
    
    
    
};

typedef multimap<GameTime, WeatherArea*, ltGTime> WeatherAreas;
typedef list<WeatherArea*> WeatherAreaList;

enum WeatherSystemMode {
EVENTMODE,
RANDOMMODE
};

class WeatherField{
private:
tfield* mapField;
tmap* map;
int counter;
int value;

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
void setValue(int v);
int getValue();

Point2D posInArea;
};

class WeatherSystem{
private:

static const int WEATHERVERSION = 1;
float seedValue;
int timeInterval;
int areaSpawnAmount;
int access2RandCount;
int maxForecast;


float windspeed2FieldRatio;
int windspeed;
Direction globalWindDirection;

float lowerRandomSize;
float upperRandomSize;

int lowerRandomDuration;
int upperRandomDuration;

bool seedValueIsSet;
FalloutType defaultFallout;

WeatherAreas weatherAreas;
WeatherAreaList activeWeatherAreas;
WindChanges windTriggers;
FieldSet processedFields;

Percentages falloutPercentages;
Percentages windDirPercentages;
Percentages windSpeedPercentages;

tmap* gameMap;
WeatherSystemMode currentMode;

WeatherSystem(const WeatherSystem&);
WeatherSystem& operator=(const WeatherSystem&);
void randomWeatherChange(GameTime currentTime, Direction windDirection);
Direction randomWindChange(int currentTurn);

float createRandomValue(float lowerlimit, float upperlimit);

public:
  static const int FallOutNum = 6;
  static const int WindDirNum = 8;
  static const int WINDSPEEDDETAILLEVEL = 8;
  WeatherSystem(tmap* map);
  WeatherSystem(tmap* map, int areaSpawns, float windspeed2FieldRatio, unsigned int timeInterval = 6, WeatherSystemMode mode = EVENTMODE, FalloutType defaultFallout = DRY);
  ~WeatherSystem();
  inline void setSeedValue();
  void setLikelihoodFallOut(const Percentages& fol) throw (IllegalValueException);
  void setLikelihoodWindDirection(const Percentages&  wd) throw (IllegalValueException);
  void setLikelihoodWindSpeed(const Percentages&  wd) throw (IllegalValueException);
  void setRandomSizeBorders(float lower, float upper);
  void setGlobalWind(unsigned int speed, Direction direction) throw (IllegalValueException);
  void addWeatherArea(WeatherArea* area, GameTime time);
  void removeWeatherArea(GameTime time, WeatherArea* area);
  void removeWindChange(int time, WindData);
  void addGlobalWindChange(int speed, Direction direction, int time) throw (IllegalValueException);  
  void update(GameTime currentTime);
  void write (tnstream& outputStream) const;
  void read (tnstream& inputStream);
  
  pair<GameTime, WeatherArea*> getNthWeatherArea(int n) const;
  pair<int, WindData> getNthWindChange(int n) const;
  const int getQueuedWeatherAreasSize() const;
  const int getQueuedWindChangesSize() const;
  void setSeedValueGeneration(bool setNew);

  unsigned int createRandomValue(unsigned int limit = 1000);  
  void skipRandomValue() const;  
  
  FalloutType getDefaultFalloutType() const{
    return defaultFallout;  
  };
  
  inline int getCurrentWindSpeed() const{
    return windspeed;
  }
  
  inline Direction getCurrentWindDirection() const{
    return globalWindDirection;
  }
  
  int getMaxForecast() const;
  
  bool isSeedValueSet(){
    return seedValueIsSet;
  }  
  inline int getSpawnsAmount(){
    return areaSpawnAmount;
  }
  
  inline void setEventMode(const WeatherSystemMode& mode){
    currentMode = mode;
  }
  
  inline int getTimeInterval(){
    return timeInterval;
  }
  
  inline void setTimeInterval(int i){
    timeInterval = i;
  }
  
  inline void setWindSpeed2FieldRatio(float ratio){
    windspeed2FieldRatio = ratio;
  }
  
  inline void setSpawnAmount(int a){
    areaSpawnAmount = a;
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
  
  inline const float getLowerSizeLimit() const{
    return lowerRandomSize;
  }
  
  inline const float getUpperSizeLimit() const{
    return upperRandomSize;
  }
  
  WindData getWindDataOfTurn(int turn) const;
};

#endif





