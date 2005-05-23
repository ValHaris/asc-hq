//
// C++ Interface: weathercast
//
// Description: 
//
//
// Author: Kevin Hirschmann <hirsch@dhcppc0>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef WEATHERCAST_H
#define WEATHERCAST_H

#include <list>
#include <map>
#include "pgimage.h"
#include "paradialog.h"
#include "weatherarea.h"
/**
@author Kevin Hirschmann
*/


//SDLmm::Color  LRCOLOR = 700;

struct WindAccu{
  int horizontalValue;
  int verticalValue;
};

typedef list<WindData> WindStack;
typedef map<const WeatherArea*, WindAccu> WeatherMap;

class Weathercast: public ASC_PG_Dialog{
public:        
    Weathercast(const WeatherSystem& ws);    

    virtual ~Weathercast();
     
    void painter (const PG_Rect &src, const ASCString& name, const PG_Rect &dst);
    void paintWeatherArea(const WeatherArea* wa);
private:
  static const int xSize;
  static const int ySize;
  static const int MAPXSIZE;
  static const int MAPYSIZE;
  int counter;
  WindStack windStack;
  const WeatherSystem& weatherSystem;
  PG_Label* turnLabel;
  int turnLabelWidth;
  
  WeatherMap warea2WindAccu;
    
  Surface s;
  int windSpeed;
  SpecialDisplayWidget* sdw;
  PG_Image* windRoseImage;
  PG_Image* windRoseArrow;
  PG_Image* weatherMapImage;
  PG_Label* windspeedLabel;
  PG_Button* back;
  bool buttonBack( PG_Button* button );
  
  PG_Button* forward;  
  bool buttonForward( PG_Button* button );
  
  PG_Button* okButton;    
  
  bool closeWindow();
  
  void updateWeatherSpeed(int turn);
  void generateWeatherMap(int turn);
  WindAccu updateWindAccu(const WindAccu&, unsigned int windspeed, Direction windDirection, float ratio);
  
  void showTurn();
  void showWindSpeed();
  
  

};

extern void weathercast();

#endif

