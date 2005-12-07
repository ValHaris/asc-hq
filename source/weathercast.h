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
#include "paradialog.h"
#include "dashboard.h"
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


class WeatherPanel: public Panel{
public:
WeatherPanel ( PG_Widget *parent, const PG_Rect &r, const ASCString& panelName_, bool loadTheme = true );
virtual ~WeatherPanel();
WindAccu getWindAccuData(const WeatherArea* wa);
int getCounter();
private:
  int counter;
  int windSpeed;
    
  PG_Button* back;
  PG_Button* forward;  
  
  PG_Image* windRoseImage;
  PG_Image* windRoseArrow;  
  PG_Image* windBar;  
  
  PG_Label* turnLabel;
  PG_Label* windspeedLabel; 
  
  BarGraphWidget* bgw;  
  
  const WeatherSystem* weatherSystem;  
  WindStack windStack;  
  WeatherMap warea2WindAccu;
  WindAccu wAccu;
    
  
  bool buttonForward( PG_Button* button );  
  bool buttonBack( PG_Button* button );  
  void painter (const PG_Rect &src, const ASCString& name, const PG_Rect &dst);
  void updateWeatherSpeed(int turn);
  
  void showTurn();  
  WindAccu updateWindAccu(const WindAccu&, unsigned int windspeed, Direction windDirection, float ratio);  
};


class Weathercast: public ASC_PG_Dialog{
public:        
    Weathercast(const WeatherSystem& ws);    

    virtual ~Weathercast();
     
    void painter (const PG_Rect &src, const ASCString& name, const PG_Rect &dst);
    bool mouseButtonDown ( const SDL_MouseButtonEvent *button);
    bool mouseMotion (  const SDL_MouseMotionEvent *motion);
    bool mouseClick ( SPoint pos );
    void paintWeatherArea(const WeatherArea* wa, int vMove, int hMove);
private:
  static const int xSize;
  static const int ySize;
  static const int MAPXSIZE;
  static const int MAPYSIZE;
  
  
  WeatherPanel* weatherPanel;
  
  int mapYPos;
  int mapXPos;
  
  
      
  const WeatherSystem& weatherSystem;    
    
    
  Surface s;  
  float currentZoomX;  
  float currentZoomY;
  SpecialDisplayWidget* sdw;
  MapDisplayPG* mapDisplayWidget;    
  //PG_Image* weatherMapImage;    
  PG_Button* okButton;      
  bool closeWindow();      
  void generateWeatherMap(int turn);
  void redraw() { Redraw(true); };      

};

extern void weathercast();

#endif

