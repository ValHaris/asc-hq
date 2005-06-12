//
// C++ Implementation: weathercast
//
// Description:
//
//
// Author: Kevin Hirschmann <hirsch@dhcppc0>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "weathercast.h"
#include "guidimension.h"
#include "graphics/blitter.h"
#include "graphics/drawing.h"
#include "iconrepository.h"
#include "mapdisplay2.h"
#include "spfst.h"
#include "paradialog.h"


const int Weathercast::xSize = 500;
const int Weathercast::ySize = 400;

const int Weathercast::MAPXSIZE = 200;
const int Weathercast::MAPYSIZE = 200;
Weathercast::Weathercast(const WeatherSystem& ws):  ASC_PG_Dialog(NULL, PG_Rect( 100, 100, xSize, ySize ), "Weathercast", MODAL ), weatherSystem(ws), windSpeed(0), counter(0), currentZoom(0.5), mapYPos(80) {
    
    mapDisplayWidget = (MapDisplayPG*)(getPGApplication().GetWidgetById(ASC_PG_App::mapDisplayID));

    //Forward, Backward Button
    forward = new PG_Button(this, PG_Rect(Width() -(50 + 10) , Height() - GuiDimension::getTopOffSet()*6, 55, 35), "forward", 90);
    forward->sigClick.connect(SigC::slot( *this, &Weathercast::buttonForward));
    back = new PG_Button(this, PG_Rect(Width() -(50 + 10) - 60, Height() - GuiDimension::getTopOffSet()*6, 55, 35), "back", 90);
    back->sigClick.connect(SigC::slot( *this, &Weathercast::buttonBack));
    

    //Display wind elements
    int windRoseYPos = mapYPos + 30 + GuiDimension::getTopOffSet();
    windRoseImage = new PG_Image(this, PG_Point(400, windRoseYPos), IconRepository::getIcon("windrose.png").GetSurface(), false, PG_Draw::BkMode(PG_Draw::TILE), "g");
    sdw  = new SpecialDisplayWidget(this, PG_Rect(100, 100, xSize, ySize));
    sdw->display.connect( SigC::slot( *this, &Weathercast::painter ));
    sdw->sigMouseMotion.connect( SigC::slot( *this, &Weathercast::mouseMotion ));
    sdw->sigMouseButtonDown.connect( SigC::slot( *this, &Weathercast::mouseButtonDown ));
    viewChanged.connect ( SigC::slot( *this, &Weathercast::redraw ));
    
    int windBarYPos = windRoseYPos + windRoseImage->Height();
    windBar = new PG_Image(this, PG_Point(400, windBarYPos ), IconRepository::getIcon("wind_bar.png").GetSurface(), false, PG_Draw::BkMode(PG_Draw::TILE), "g");
    bgw = new BarGraphWidget ( this, PG_Rect(400, windBarYPos, 90, 20), BarGraphWidget::Direction(0) );

    WindData wData;
    wData.speed = weatherSystem.getCurrentWindSpeed();
    wData.direction = weatherSystem.getCurrentWindDirection();
    windStack.push_front(wData);
    windspeedLabel = new PG_Label(this, PG_Rect(427, windBarYPos + windBar->Height() -16 , 10, GetTextHeight() * 2));    
    windspeedLabel->SetFontSize(windspeedLabel->GetFontSize() -5);        
    updateWeatherSpeed(actmap->time.turn());

    //Display turn
    turnLabel = new PG_Label(this, PG_Rect(Width() -(48) - 60, (Height() - GuiDimension::getTopOffSet()*8) + 5, 10, GetTextHeight() * 2), "Turn: +0");
    turnLabel->SetSizeByText();
    turnLabelWidth = turnLabel->Width();
    
    
    okButton = new PG_Button(this, PG_Rect((xSize - GuiDimension::getButtonWidth()) / 2, ySize - (GuiDimension::getButtonHeight() + GuiDimension::getTopOffSet()), GuiDimension::getButtonWidth(), GuiDimension::getButtonHeight()), "OK", 90);
    okButton->sigClick.connect(SigC::slot( *this, &Weathercast::closeWindow ));

    sigClose.connect( SigC::slot( *this, &Weathercast::closeWindow ));
}

Weathercast::~Weathercast() {}

void Weathercast::paintWeatherArea(const WeatherArea* wa) {
    MegaBlitter<4,colorDepth,ColorTransform_None, ColorMerger_AlphaOverwrite,SourcePixelSelector_DirectRectangle> blitter;
    static const char* weathernames[] = {"terrain_weather_dry.png",
                                         "terrain_weather_lightrain.png",
                                         "terrain_weather_heavyrain.png",
                                         "terrain_weather_lightsnow.png",
                                         "terrain_weather_heavysnow.png",
                                         "terrain_weather_ice.png"
                                        };

    WindAccu wAccu = warea2WindAccu[wa];
    int vMove = static_cast<int>(wAccu.verticalValue);
    int hMove = static_cast<int>(wAccu.horizontalValue);
    if((wa->getCenterPos().x + hMove >0) && (wa->getCenterPos().y + vMove >0)) {
        SPoint pixCenter = OverviewMapImage::map2surface(MapCoordinate(wa->getCenterPos().x + hMove, wa->getCenterPos().y + vMove));
        Surface wSurface = IconRepository::getIcon(weathernames[wa->getFalloutType()]);
        wSurface.strech(20,12);
        if(pixCenter.x + wSurface.w() > s.w()) {
            blitter.setRectangle( SPoint(0,0), s.w() - (pixCenter.x), wSurface.h());
        } else {
            blitter.setRectangle( SPoint(0,0), wSurface.w(), wSurface.h());
        }
        blitter.blit( wSurface, s, pixCenter );
    }

    /*for(int i = (wa->getCenterPos().x - wa->getWidth()/2); i < wa->getCenterPos().x + wa->getWidth(); i++){
      for(int j = (wa->getCenterPos().y - wa->getHeight()/2); j < wa->getCenterPos().y + wa->getHeight()/2; j++){   
        SPoint pixCenter = OverviewMapImage::map2surface(MapCoordinate(i,j));
        OverviewMapImage::fill(s, pixCenter, LRAINOR);
      }
    }*/
}

void Weathercast::generateWeatherMap(int turn) {
    for(int i =0; i < weatherSystem.getActiveWeatherAreasSize(); i++) {
        const WeatherArea* wa = weatherSystem.getNthActiveWeatherArea(i);
        if((wa->getDuration() > (turn - (actmap->time.turn() + 1))) && (wa->getCenterField()->isOnMap(actmap))) {
            paintWeatherArea(wa);
        }

    }
    for(int i =0; i < weatherSystem.getQueuedWeatherAreasSize(); i++) {
        pair<GameTime, WeatherArea*> p = weatherSystem.getNthWeatherArea(i);
        if(p.first.turn() <= turn) {
            paintWeatherArea(p.second);
        }
    }
}


void Weathercast::painter (const PG_Rect &src, const ASCString& name, const PG_Rect &dst) {
    Surface screen = Surface::Wrap( PG_Application::GetScreen() );
    {
        MegaBlitter<4,colorDepth,ColorTransform_None, ColorMerger_AlphaOverwrite, SourcePixelSelector_DirectZoom> blitter;
        static const char* weathernames[] = {"terrain_weather_dry.png",
                                             "terrain_weather_lightrain.png",
                                             "terrain_weather_heavyrain.png",
                                             "terrain_weather_lightsnow.png",
                                             "terrain_weather_heavysnow.png",
                                             "terrain_weather_ice.png"

                                            }
                                            ;
        PG_Point defaultWeatherPoint(ClientToScreen(403,mapYPos));
        Surface ws = IconRepository::getIcon(weathernames[actmap->weatherSystem->getDefaultFalloutType()]);
        ws.strech(60, 25);
        blitter.blit( ws, screen, SPoint(defaultWeatherPoint.x, defaultWeatherPoint.y ));
    }
    if(windStack.front().speed > 0) {
        MegaBlitter<4,colorDepth,ColorTransform_None, ColorMerger_AlphaOverwrite, SourcePixelSelector_DirectRotation> blitter;
        blitter.setAngle( directionangle[windStack.front().direction] );
        PG_Point p(windRoseImage->x, windRoseImage->y);
        blitter.blit ( IconRepository::getIcon("wind-arrow.png"), screen, SPoint(p.x, p.y) );
    }
    if ( actmap ) {
        {
            actmap->overviewMapHolder.clear();
            s = actmap->overviewMapHolder.getOverviewMap( true );
            generateWeatherMap(actmap->time.turn() + counter);
            MegaBlitter< gamemapPixelSize, gamemapPixelSize,ColorTransform_None,ColorMerger_AlphaOverwrite,SourcePixelSelector_DirectZoom> blitter;
            blitter.setSize( s.w(), s.h(), dst.w - 100, dst.h -100 );

            //blitter.setZoom(4.0);
            currentZoom  = blitter.getZoomX();
            PG_Point dstPoint(ClientToScreen(30,mapYPos));
            blitter.blit( s, screen, SPoint(dstPoint.x, dstPoint.y) );

            SPoint ul = OverviewMapImage::map2surface( mapDisplayWidget->upperLeftCorner());
            SPoint lr = OverviewMapImage::map2surface( mapDisplayWidget->lowerRightCorner());


            ul.x *= currentZoom;
            ul.y *= currentZoom;
            lr.x *= currentZoom;
            lr.y *= currentZoom;

            if ( ul.x < 0 )
                ul.x = 0;
            if ( ul.y < 0 )
                ul.y = 0;
            if ( lr.x >= dst.w - 100 )
                lr.x = dst.w - 100 -1;
            if ( lr.y >= dst.h -100 )
                lr.y = dst.h -100 -1;

            rectangle<4>(screen, SPoint(dstPoint.x + ul.x, dstPoint.y + ul.y), lr.x - ul.x, lr.y- ul.y, ColorMerger_Set<4>(0xff), ColorMerger_Set<4>(0xff) );
        }
    }
    {
        bgw->setColor(0x00ff00);
        bgw->setFraction(windSpeed/255.0);
        bgw->SendToBack();
        bgw->Show();
        windBar->Show();
        std::string wind;
        wind.append(strrr(windSpeed));
        windspeedLabel->SetText(wind);
        windspeedLabel->SetSizeByText();
        windspeedLabel->Show();

    }
}

bool Weathercast::mouseButtonDown ( const SDL_MouseButtonEvent *button) {
    if ( sdw->IsMouseInside() )
        if ( button->type == SDL_MOUSEBUTTONDOWN && button->button == 1 ) {
            PG_Point p = sdw->ScreenToClient( button->x, button->y );
            return mouseClick( SPoint( p.x, p.y ));
        }

    return false;
}

bool Weathercast::mouseMotion  (  const SDL_MouseMotionEvent *motion) {
    if ( sdw->IsMouseInside() )
        if ( motion->type == SDL_MOUSEMOTION && (motion->state & 1 ) ) {
            PG_Point p = sdw->ScreenToClient( motion->x, motion->y );
            return mouseClick( SPoint( p.x, p.y ));
        }

    return false;
}

bool Weathercast::mouseClick ( SPoint pos ) {
    MapCoordinate mc = OverviewMapImage::surface2map( SPoint(float(pos.x) / currentZoom, float(pos.y) / currentZoom ));
    if ( !(mc.valid() && mc.x < actmap->xsize && mc.y < actmap->ysize ))
        return false;

    mapDisplayWidget->centerOnField( mc );
    return true;
}



WindAccu Weathercast::updateWindAccu(const WindAccu& ac, unsigned int windspeed, Direction windDirection, float ratio) {
    WindAccu wAccu = {ac.horizontalValue, ac.verticalValue};
    if(windDirection == N) {
        wAccu.verticalValue += -1 * static_cast<int>(ratio * windspeed) *2;
    } else if(windDirection == NE) {
        wAccu.verticalValue += -1 * static_cast<int>(ratio * windspeed) *2;
        wAccu.horizontalValue += static_cast<int>(ratio * windspeed);
    } else if(windDirection == SE) {
        wAccu.verticalValue +=  static_cast<int>(ratio * windspeed) *2;
        wAccu.horizontalValue += static_cast<int>(ratio * windspeed);
    } else if(windDirection == S) {
        wAccu.verticalValue += static_cast<int>(ratio * windspeed) *2;
    } else if(windDirection == SW) {
        wAccu.verticalValue += static_cast<int>(ratio * windspeed) *2;
        wAccu.horizontalValue += -1 * static_cast<int>(ratio * windspeed);
    } else if(windDirection == NW) {
        wAccu.verticalValue += -1 * static_cast<int>(ratio * windspeed) *2;
        wAccu.horizontalValue += -1 * static_cast<int>(ratio * windspeed);
    }
    return wAccu;
}

bool Weathercast::buttonForward( PG_Button* button ) {
    if(counter < weatherSystem.getMaxForecast()) {
        //update turn information
        ++counter;
        showTurn();
        //update wind data
        updateWeatherSpeed(actmap->time.turn() + counter);
        //update display data of active weatherareas (active at least since actmap->time)
        for(int i =0; i < weatherSystem.getActiveWeatherAreasSize(); i++) {
            const WeatherArea* wa = weatherSystem.getNthActiveWeatherArea(i);
            if(wa->getDuration() - counter > 0) {
                WindAccu wAccu;
                if(counter == 1) {
                    wAccu.horizontalValue = wa->getHorizontalWindAccu();
                    wAccu.verticalValue = wa->getVerticalWindAccu();
                } else {
                    wAccu = warea2WindAccu[wa];
                }
                wAccu = updateWindAccu(wAccu, windSpeed,  windStack.front().direction, weatherSystem.getWindspeed2FieldRatio());
                warea2WindAccu[wa] = wAccu;
            }
        }
        //update display data for queued weatherareas
        for(int i =0; i < weatherSystem.getQueuedWeatherAreasSize(); i++) {
            pair<GameTime, WeatherArea*> p = weatherSystem.getNthWeatherArea(i);
            const WeatherArea* wa = p.second;
            WindAccu wAccu;
            if(p.first.turn() == actmap->time.turn() + counter) {
                wAccu.horizontalValue = 0;
                wAccu.verticalValue = 0;
            } else {
                wAccu.horizontalValue = warea2WindAccu[p.second].horizontalValue;
                wAccu.verticalValue = warea2WindAccu[p.second].verticalValue;
            }
            wAccu = updateWindAccu(wAccu, windSpeed,  windStack.front().direction, weatherSystem.getWindspeed2FieldRatio());
            warea2WindAccu[p.second] = wAccu;
        }
        Update();
        this->redraw();
    }
    return true;
}

bool Weathercast::buttonBack( PG_Button* button ) {
    if(counter>0) {
        //update turn information
        --counter;
        showTurn();
        //update display data of active weatherareas (active at least since actmap->time)
        for(int i =0; i < weatherSystem.getActiveWeatherAreasSize(); i++) {
            const WeatherArea* wa = weatherSystem.getNthActiveWeatherArea(i);
            if(wa->getDuration() - counter > 0) {
                WindAccu wAccu = warea2WindAccu[wa];
                wAccu = updateWindAccu(wAccu, windSpeed,  static_cast<Direction>((windStack.front().direction +3) % 6), weatherSystem.getWindspeed2FieldRatio());
                warea2WindAccu[wa] = wAccu;
            }
        }
        //update display data for queued weatherareas
        for(int i =0; i < weatherSystem.getQueuedWeatherAreasSize(); i++) {
            pair<GameTime, WeatherArea*> p = weatherSystem.getNthWeatherArea(i);
            const WeatherArea* wa = p.second;
            WindAccu wAccu;
            if(p.first.turn() <= actmap->time.turn() + counter) {
                WindAccu wAccu = warea2WindAccu[wa];
                wAccu = updateWindAccu(wAccu, windSpeed,  static_cast<Direction>((windStack.front().direction +3) % 6), weatherSystem.getWindspeed2FieldRatio());
                warea2WindAccu[wa] = wAccu;
            }
        }
        //update wind data
        if(weatherSystem.getWindDataOfTurn(actmap->time.turn() + counter + 1).speed >=0) {
            windStack.pop_front();
            windSpeed = windStack.front().speed;            
        }
        Update();
    }
    return true;
}

bool Weathercast::closeWindow() {
    quitModalLoop(1);
    return true;

}

void Weathercast::showTurn() {
    std::string turn = "Turn: +";
    turn.append(strrr(counter));
    turnLabel->SetText(turn);
    turnLabel->SetSizeByText();
    turnLabel->Redraw();

}


void Weathercast::updateWeatherSpeed(int turn) {
    int newWindSpeed = weatherSystem.getWindDataOfTurn(turn).speed;
    WindData wd = weatherSystem.getWindDataOfTurn(turn);
    windSpeed = newWindSpeed;
    windStack.push_front(wd);    
}


extern void weathercast() {
    Weathercast wc(*(actmap->weatherSystem));
    wc.Show();
    wc.RunModal();
}


