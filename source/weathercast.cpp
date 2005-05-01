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
#include "iconrepository.h"
#include "spfst.h"

const int Weathercast::xSize = 400;
const int Weathercast::ySize = 400;

Weathercast::Weathercast(const WeatherSystem& ws):  ASC_PG_Dialog(NULL, PG_Rect( 100, 100, xSize, ySize ), "Weathercast", MODAL ), weatherSystem(ws), windSpeed(0), counter(0) {

    turnLabel = new PG_Label(this, PG_Rect(xSize /2, GuiDimension::getTopOffSet(), 10, GetTextHeight() * 2), "Turn: +0");
    turnLabel->SetSizeByText();
    turnLabelWidth = turnLabel->Width();
    back = new PG_Button(this, PG_Rect(10, GuiDimension::getTopOffSet()*2, 55, 35), "back", 90);
    back->sigClick.connect(SigC::slot( *this, &Weathercast::buttonBack));
    forward = new PG_Button(this, PG_Rect(Width() -(back->Width() + 10) , GuiDimension::getTopOffSet()*2, 55, 35), "forward", 90);
    forward->sigClick.connect(SigC::slot( *this, &Weathercast::buttonForward));




    windRoseImage = new PG_Image(this, PG_Point(300, 150), IconRepository::getIcon("windrose.png").GetSurface(), false, PG_Draw::BkMode(PG_Draw::TILE), "g");
    sdw  = new SpecialDisplayWidget(this, PG_Rect(100, 100, 100, 100));
    sdw->display.connect( SigC::slot( *this, &Weathercast::painter ));

    WindData wData;
    wData.speed = weatherSystem.getCurrentWindSpeed();
    wData.direction = weatherSystem.getCurrentWindDirection();
    windStack.push_front(wData);
    windspeedLabel = new PG_Label(this, PG_Rect(280, 170 + windRoseImage->Height() , 10, GetTextHeight() * 2), "Windspeed: ");
    updateWeatherSpeed(actmap->time.turn());

    okButton = new PG_Button(this, PG_Rect((xSize - GuiDimension::getButtonWidth()) / 2, ySize - (GuiDimension::getButtonHeight() + GuiDimension::getTopOffSet()), GuiDimension::getButtonWidth(), GuiDimension::getButtonHeight()), "OK", 90);
    okButton->sigClick.connect(SigC::slot( *this, &Weathercast::closeWindow ));

    sigClose.connect( SigC::slot( *this, &Weathercast::closeWindow ));
}

Weathercast::~Weathercast() {}


void Weathercast::painter (const PG_Rect &src, const ASCString& name, const PG_Rect &dst) {
    cout << "painter" << endl;
    if(windStack.front().speed > 0) {
        Surface screen = Surface::Wrap( PG_Application::GetScreen() );

        MegaBlitter<4,colorDepth,ColorTransform_None, ColorMerger_AlphaOverwrite, SourcePixelSelector_DirectRotation> blitter;
        blitter.setAngle( directionangle[windStack.front().direction] );
        PG_Point p(windRoseImage->x, windRoseImage->y);
        blitter.blit ( IconRepository::getIcon("wind-arrow.png"), screen, SPoint(p.x, p.y) );
    }
}



bool Weathercast::buttonForward( PG_Button* button ) {
    if(counter < weatherSystem.getMaxForecast()) {
        ++counter;
        showTurn();
        updateWeatherSpeed(actmap->time.turn() + counter);
	Update();
    }    
    return true;
}

bool Weathercast::buttonBack( PG_Button* button ) {
    if(counter>0) {
        --counter;
        showTurn();
        if(weatherSystem.getWindDataOfTurn(actmap->time.turn() + counter + 1).speed >=0) {
            windStack.pop_front();
            windSpeed = windStack.front().speed;
            showWindSpeed();
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

void Weathercast::showWindSpeed() {
    std::string wind = "WindSpeed: ";
    wind.append(strrr(windSpeed));
    windspeedLabel->SetText(wind);
    windspeedLabel->SetSizeByText();
    windspeedLabel->Redraw();

}


void Weathercast::updateWeatherSpeed(int turn) {
    int newWindSpeed = weatherSystem.getWindDataOfTurn(turn).speed;
    WindData wd = weatherSystem.getWindDataOfTurn(turn);
    windSpeed = newWindSpeed;
    windStack.push_front(wd);
    windRoseImage->Update();

    /*if(windSpeed > 0){
    Surface screen = Surface::Wrap( PG_Application::GetScreen() );

    MegaBlitter<4,colorDepth,ColorTransform_None, ColorMerger_AlphaOverwrite, SourcePixelSelector_Rotation> blitter;
    blitter.setAngle( directionangle[actmap->weather.windDirection] );
    PG_Point p = ClientToScreen(300, 170);
    blitter.blit ( IconRepository::getIcon("wind-arrow.png"), screen, SPoint(p.x, p.y) );
    }*/
    showWindSpeed();
}


extern void weathercast() {
    Weathercast wc(*(actmap->weatherSystem));
    wc.Show();
    wc.Run();
}


