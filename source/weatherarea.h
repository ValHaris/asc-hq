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
// #include "graphics/ascttfont.h"

/**
@brief A Vector2D is a line (without a defined start point) in a 2 dimensional space and is deccribed by its
       x- and y component
@author Kevin Hirschmann

 */
class Vector2D{
   private:
      int xComponent;
      int yComponent;

   public:
 /**
      @brief Default constructor
  */
      Vector2D();
 /**
      @brief Constructor for creating a new vector
      @param x The xComponent
      @param y The yComponent
  */
      Vector2D(int x, int y);
 /**
      @brief Destructor
  */
      ~Vector2D();
 /**
      @brief Retrieves the xComponent of the Vector
      @return the xComponent
  */
      int getXComponent() const;
 /**
      @brief Retrieves the yComponent of the Vector
      @return the yComponent
  */
      int getYComponent() const;
 /**
      @brief Calculates the length of the vector (Pythagoras)
      @return The length of the Vector
  */
      double getLength() const;
 /**
      @brief Checks if the vector is the zero-vector
      @return true if x- and yComponent == 0; flase otherwise
  */
      bool isZeroVector() const;
};



class IllegalValueException: public ASCmsgException{
public:
IllegalValueException(const ASCString& msg);
~IllegalValueException();
};

extern const char*  cdirections[];
enum Direction {
N,
NE,
SE,
S,
SW,
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
GameMap* map;
MapCoordinate center;
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
bool clustered;
int seedValue;
WeatherArea &operator=(const WeatherArea&);
WeatherArea(const WeatherArea&);
MapCoordinate calculateFieldPosition(MapCoordinate center, Vector2D relPos);


short createAlgebraicSign();
void step(WeatherRect r);
int calculateCornerPoint(int a, int b, int c);
int calculateDiamondPoint(int a, int b, int c, int d);
int calculateCurrentOffset(int currentOffset);
int calculateCornerPointValue(int a, int b, int c);
int calculateDiamondPointValue(int a, int b, int c, int d);
void createWeatherFields();

public:
    WeatherArea(GameMap* map, int xCenter, int yCenter, int width, int height, int duration, FalloutType fType, unsigned int seedValue, bool clustered = true);
    WeatherArea(GameMap* map, int xCenter, int yCenter, int radius);
    WeatherArea(GameMap* map);
    
    ~WeatherArea();
    inline GameMap* getMap() const;
    
    Vector2D getWindVector() const;
   // void setWindVector(unsigned int speed, Direction windDirection);
    void updateMovementVector(unsigned int speed, Direction windDirection, double ratio);
    void setFalloutType(FalloutType fallout);    
    FalloutType getFalloutType() const;
    FalloutType getFalloutType(int value) const;
    int getWidth() const;
    int getHeight() const;
    float getHorizontalWindAccu() const;
    float getVerticalWindAccu() const;
    
    MapCoordinate getCenterPos() const{
      return center;
    }
    
    const WeatherField* getCenterField() const{
      return area[static_cast<int>(height/2 * ((area.size() % height)-1) +  (width/2 +0.5))];
    };
    
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
GameMap* map;
int counter;
int value;

void setMapField(tfield* mapField);

public:
WeatherField(GameMap* map);
WeatherField(MapCoordinate mapPos, const WeatherArea* area);
~WeatherField();
void move(const Vector2D& vector);
bool isOnMap(const GameMap* map) const;
void update(const WeatherArea*, FieldSet& processedFields);
void reset(GameMap* m, const WeatherArea*, FieldSet& processedFields);
void write (tnstream& outputStream) const;
void read (tnstream& inputStream);
void setValue(int v);
int getValue();

MapCoordinate posInArea;
};
//**************************************************************************************************************************************
class WeatherSystem : public SigC::Object{
private:

static const int WEATHERVERSION = 1;
unsigned int seedValue;
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

GameMap* gameMap;
WeatherSystemMode currentMode;

WeatherSystem(const WeatherSystem&);
WeatherSystem& operator=(const WeatherSystem&);
void randomWeatherChange(GameTime currentTime, Direction windDirection, int delay = 0);
Direction randomWindChange(int currentTurn, int delay = 0);

float createRandomValue(float lowerlimit, float upperlimit);
Direction getNthTurnWindDirection(int turn, GameTime currentTime);

public:
  static const int FallOutNum = 6;
  static const int WindDirNum = 6;
  static const int WINDSPEEDDETAILLEVEL = 8;
  
  //Used to support old wind data
  //Once the read order of GameMap is changed and fields + weather are read inside of gamemap::read
  //get rid of this
  
  static int legacyWindSpeed;
  static int legacyWindDirection;
  
  
  WeatherSystem(GameMap* map);
  WeatherSystem(GameMap* map, int areaSpawns, float windspeed2FieldRatio, unsigned int timeInterval = 6, WeatherSystemMode mode = EVENTMODE, FalloutType defaultFallout = DRY);
  ~WeatherSystem();
  inline void setSeedValue();
  void setLikelihoodFallOut(const Percentages& fol) throw (IllegalValueException);
  void setLikelihoodWindDirection(const Percentages&  wd) throw (IllegalValueException);
  void setLikelihoodWindSpeed(const Percentages&  wd) throw (IllegalValueException);
  void setRandomSizeBorders(float lower, float upper);
  void setGlobalWind(unsigned int speed, Direction direction) throw (IllegalValueException);
  void setDefaultFallout(FalloutType newFalloutType);
  void addWeatherArea(WeatherArea* area, GameTime time);
  void removeWeatherArea(GameTime time, WeatherArea* area);
  void removeWindChange(int time, WindData);
  void addGlobalWindChange(int speed, Direction direction, int time) throw (IllegalValueException);  
  void update();  
  void write (tnstream& outputStream) const;
  void read (tnstream& inputStream);
  
  pair<GameTime, WeatherArea*> getNthWeatherArea(int n) const;
  const WeatherArea* getNthActiveWeatherArea(int n) const;
  pair<int, WindData> getNthWindChange(int n) const;
  const int getQueuedWeatherAreasSize() const;
  const int getActiveWeatherAreasSize() const;
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
  
  void setLowerDurationLimit(int udl){
    lowerRandomDuration = udl;
  }
  
  void setUpperDurationLimit(int ldl){
    upperRandomDuration = ldl;
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
  
  const int getLowerDurationLimit() const{
    return lowerRandomDuration;
  }
  
  const int getUpperDurationLimit() const{
    return upperRandomDuration;
  }
  
  
  WindData getWindDataOfTurn(int turn) const;
};

#endif






