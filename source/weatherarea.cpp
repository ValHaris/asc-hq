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

const char*  cdirections[WeatherSystem::WindDirNum] = {"North", "NorthEast", "East", "SouthEast", "South", "SouthWest", "West", "NorthWest"
                                                      };


//****************************************************************************************************************************************
const int WeatherArea::MAXVALUE = 200;
const int WeatherArea::MAXOFFSET = 100;
WeatherArea::WeatherArea(tmap* m, int xCenter, int yCenter, int w, int h, int d, FalloutType fallout, unsigned int sV): map(m), center(xCenter, yCenter), width(w), height(h), duration(d), ft(fallout), stepCount(0), seedValue(sV) {
    int x =  0;
    int y =  0;
    WeatherField* wf;
    for(int i = 0; i < width * height; i++) {
        x =  i % width;
        y =  i / width;
        wf = new WeatherField(Point2D(x + (center.getX() - width/2), y + (center.getY() - height/2)), this);
        area.push_back(wf);
    }
    createWeatherFields();
}

WeatherArea::WeatherArea(tmap* m): map(m) {
}


WeatherArea::WeatherArea(tmap* m, int xCenter, int yCenter, int r):map(m), center(xCenter, yCenter), width(r), height(r), radius(r) {
}

WeatherArea::~WeatherArea() {
    for(int i = 0; i < area.size(); i++) {
        delete area[i];
    }
}

void WeatherArea::createWeatherFields() {
    if(area.size()>0) {
        counter = 0;
        WeatherField* wf;
        srand(seedValue);

        wf = area[(width*height)/2];
        wf->setValue(MAXVALUE);
        wf = area[0];
        wf->setValue(createRandomValue(MAXVALUE));
        wf = area[width - 1];
        wf->setValue(createRandomValue(MAXVALUE));
        wf = area[width * height - 1];
        wf->setValue(createRandomValue(MAXVALUE));
        wf = area[(width * height - 1) - width];
        wf->setValue(createRandomValue(MAXVALUE));
        WeatherRect r = {0, width -1, width * height -1, (width * height) - width };
        step(r);
    }
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

FalloutType WeatherArea::getFalloutType(int value) const {
    if(ft > 0) {
    if(value < (MAXVALUE * 0.25)) {
            return DRY;
        }
    }

    return ft;
}
void WeatherArea::updateMovementVector(unsigned int windspeed, Direction windDirection, double ratio) {

    if(windDirection == N) {
        currentMovement = Vector2D(0,static_cast<int>(-1 * ratio * windspeed));
    } else if(windDirection == NE) {
        currentMovement = Vector2D(static_cast<int>(ratio * windspeed), static_cast<int>(-1 * ratio * windspeed));
    } else if(windDirection == SE) {
        currentMovement = Vector2D(static_cast<int>(ratio * windspeed), static_cast<int>(ratio * windspeed));
    } else if(windDirection == S) {
        currentMovement = Vector2D(0,  static_cast<int>(ratio * windspeed));
    } else if(windDirection == SW) {
        currentMovement = Vector2D(static_cast<int>(-1 * ratio * windspeed), static_cast<int>( ratio * windspeed));
    } else if(windDirection == NW) {
        currentMovement = Vector2D(static_cast<int>(-1 * ratio * windspeed), static_cast<int>(-1 * ratio * windspeed));
    } else if(windDirection == W) {
        currentMovement = Vector2D(static_cast<int>(-1 * ratio * windspeed), 0);
    } else if(windDirection == E) {
        currentMovement = Vector2D(static_cast<int>(ratio * windspeed), 0);
    }

}


void WeatherArea::update(WeatherSystem* wSystem, FieldSet& processedFields) {
    duration--;
    if(!currentMovement.isZeroVector()) {

        for(int i = 0; i < area.size(); i++) {
            WeatherField* wf = area[i];
            if(wf->isOnMap(map)) {
                wf->reset(processedFields);
            }
        }
    }
    center.move(getWindVector());
    for(int i = 0; i < area.size(); i++) {
        WeatherField* wf = area[i];
        wf->move(currentMovement);
        if(wf->isOnMap(map)) {
            wf->update(this, processedFields);
        }
    }
}

void WeatherArea::write (tnstream& outputStream) const {
    outputStream.writeInt(seedValue);
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
    seedValue = inputStream.readInt();
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
    createWeatherFields();
}

Vector2D WeatherArea::getWindVector() const {
    return currentMovement;
}

FalloutType WeatherArea::getFalloutType() const {
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
        }
    }
}

int WeatherArea::getDuration() const {
    return duration;
}

void WeatherArea::setDuration(int d) {
    duration = d;
}


unsigned int WeatherArea::createRandomValue(int limit) {
    if(limit == 0) {
        return 1;
    }
    int random_integer = rand();
    return (random_integer % limit);
}

short WeatherArea::createAlgebraicSign() {
    int random_integer = rand();
    random_integer = random_integer % 2;
    if(random_integer == 0)
        return -1;
    else
        return 1;
}

int WeatherArea::calculateCurrentOffset(int currentOffset) {
    return  currentOffset / stepCount;
}

void WeatherArea::step(WeatherRect r) {
    //cout << "step" << endl;
    ++stepCount;
    int diamondPoint = calculateDiamondPoint(r.a, r.b, r.c, r.d);
    int f = calculateCornerPoint(r.a, r.b, diamondPoint);
    int g = calculateCornerPoint(r.b, r.c, diamondPoint);
    int h = calculateCornerPoint(r.d, r.c, diamondPoint);
    int i = calculateCornerPoint(r.a, r.d, diamondPoint);


    WeatherRect r1 ={r.a, f, diamondPoint, i};
    WeatherRect r2 ={f, r.b, g, diamondPoint};
    WeatherRect r3 ={diamondPoint, g, r.c, h};
    WeatherRect r4 ={i, diamondPoint, h, r.d};

    if((f - r.a > 1) || (diamondPoint - (f + width) > 1)) {
        step(r1);
    }
    if((r.b - f > 1) || (diamondPoint - (r.b + width -1) >1)) {
        step(r2);
    }
    if((g - diamondPoint > 1) || (r.c - (g + width ) > 1)) {
        step(r3);
    }
    if((diamondPoint -i > 1) || (r.d - (diamondPoint + width) > 1)) {
        step(r4);
    }
    --stepCount;
}

int WeatherArea::calculateDiamondPoint(int a, int b, int c, int d) {
    int xd = (b - a)/2;
    int diva = static_cast<int>(a / width);
    int divd = static_cast<int>(d / width);
    int diamondPoint = static_cast<int>((divd - diva)/2);
    diamondPoint *= width;
    diamondPoint = diamondPoint + xd + a;

    area[diamondPoint]->setValue(calculateDiamondPointValue(a, b, c, d));
    return diamondPoint;
}

int WeatherArea::calculateCornerPoint(int a, int b, int diamondPoint) {
    int x = 0;
    int y = 0;
    double xd = (b - a)/2;
    int cornerPoint = 0;
    int diva = static_cast<int>(a / width);
    int divb = static_cast<int>(b / width);
    if(diva == divb) {
        cornerPoint = a + (b - a)/2;

    } else {
        int diva = static_cast<int>(a / width);
        int divb = static_cast<int>(b / width);
        int yPos = (divb - diva)/2;
        cornerPoint = yPos * width + a;

    }
    area[cornerPoint]->setValue(calculateCornerPointValue(a, b, diamondPoint));
    return cornerPoint;
}

int WeatherArea::calculateCornerPointValue(int a, int b, int c) {
    int cornerValue = (area[a]->getValue() + area[b]->getValue() + area[c]->getValue()) / 3;
    cornerValue = cornerValue + static_cast<int>((createRandomValue(MAXOFFSET) * 1.3 * createAlgebraicSign()));
    if(cornerValue < 0) {
        cornerValue = 0;
    } else if(cornerValue>MAXVALUE) {
        cornerValue = MAXVALUE;
    }
    return cornerValue;
}

int WeatherArea::calculateDiamondPointValue(int a, int b, int c, int d) {
    int diamondValue = (area[a]->getValue() + area[b]->getValue() + area[c]->getValue() + area[c]->getValue()) / 4;
    diamondValue = diamondValue + static_cast<int>((createRandomValue(MAXOFFSET) * 1.3 * createAlgebraicSign()));
    if(diamondValue < 0) {
        diamondValue = 0;
    } else if(diamondValue > MAXVALUE) {
        diamondValue = MAXVALUE;
    }
    return diamondValue;

}

//******************************************************************************************************************************************

WeatherField::WeatherField(tmap* m):mapField(0), map(m) {}

WeatherField::WeatherField(Point2D mapPos, const WeatherArea* area):posInArea(mapPos), mapField(0) {
    if(isOnMap(area->getMap())) {
        mapField = area->getMap()->getField(posInArea.getX(), posInArea.getY());
    }
    counter = 0;
}

WeatherField::~WeatherField() {
}

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
    mapField->setweather(area->getFalloutType(value));
    mapField->setparams();
    processedFields.insert(mapField);
}

void WeatherField::write (tnstream& outputStream) const {
    outputStream.writeInt(posInArea.getX());
    outputStream.writeInt(posInArea.getY());
    //outputStream.writeInt(value);

}

void  WeatherField::read(tnstream& inputStream) {
    int x = inputStream.readInt();
    int y = inputStream.readInt();
    //value = inputStream.readInt();
    posInArea.set(x,y);
    if(isOnMap(map))
        mapField = map->getField(x,y);
}

void WeatherField::setValue(int v) {
    value = v;
}

int WeatherField::getValue() {
    return value;
}
//******************************************************************************************************************************************

WeatherSystem::WeatherSystem(tmap* map, int spawn, float ws2fr, unsigned int tInterval, WeatherSystemMode mode):timeInterval(tInterval), windspeed2FieldRatio(ws2fr), areaSpawnAmount(spawn), gameMap(map), currentMode(mode), windspeed(2), globalWindDirection(S), lowerRandomSize(0.25), upperRandomSize(0.8), access2RandCount(0) {
    seedValue = time(0);
    srand(static_cast<unsigned int>(seedValue));

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
    for(int i = 0; i < WeatherSystem::WINDSPEEDDETAILLEVEL - 1 ; i++) {
        int weight = (100 / WINDSPEEDDETAILLEVEL);
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

pair<GameTime, WeatherArea*> WeatherSystem::getNthWeatherArea(int n) const {
    int k = 0;
    WeatherAreas::const_iterator it;
    it = weatherAreas.begin();
    while(k<n) {
    ++it;
    ++k;
    }
    return *it;

}
const int WeatherSystem::getQueuedWeatherAreasSize() const {
    return weatherAreas.size();
}

void WeatherSystem::setSeedValueGeneration(bool setNew) {
    seedValueIsSet = !setNew;
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

void WeatherSystem::addWeatherArea(WeatherArea* area, GameTime time) {
    pair<GameTime, WeatherArea*>p(time, area);
    weatherAreas.insert(p);
}

void WeatherSystem::removeWeatherArea(GameTime time, WeatherArea* area) {
    WeatherAreas::iterator lower = weatherAreas.lower_bound(time);
    WeatherAreas::iterator upper = weatherAreas.upper_bound(time);
    while(lower != upper) {
        if(lower->second == area) {
            weatherAreas.erase(lower);
            delete area;
            return;
        }
        ++lower;
    }
}


Direction WeatherSystem::randomWindChange(int currentTurn) {
    int newWindDirection = createRandomValue(100);
    int newWindSpeed = createRandomValue(100);
    cout << "new random windspeed 1: " << newWindSpeed << endl;
    Direction newDirection;
    int sum = 0;
    for(int i = 0; i < windDirPercentages.size(); i++) {
        sum += windDirPercentages[i];
        if(newWindDirection <= sum) {
            newDirection = static_cast<Direction>(i);
            break;
        }
    }
    sum = 0;
    for(int i = 0; i < windSpeedPercentages.size(); i++) {
        cout << "sum: " << sum << endl;
        sum += windSpeedPercentages[i];
        if(newWindSpeed <= sum) {
            cout << "sum 2: " << sum << endl;
            newWindSpeed = i * (100 /WINDSPEEDDETAILLEVEL);
            break;
        }
    }
    cout << "new random windspeed: " << newWindSpeed << endl;
    addGlobalWindChange(newWindSpeed, newDirection, currentTurn);
    return newDirection;
}

void WeatherSystem::randomWeatherChange(GameTime currentTime, Direction windDirection) {
    WeatherArea* newArea;
    int xpos;
    int ypos;
    int width = static_cast<int>(gameMap->xsize * createRandomValue(lowerRandomSize, upperRandomSize));
    int height = static_cast<int>(gameMap->ysize * createRandomValue(lowerRandomSize, upperRandomSize));
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
                ypos = createRandomValue(gameMap->ysize);
                ;
                xpos = 0;
            }
        } else if(windDirection == SW) {
            if(sideDecision == 0) {
                ypos = 0;
                xpos = createRandomValue(gameMap->xsize);
            } else {
                ypos = createRandomValue(gameMap->ysize);
                ;
                xpos = gameMap->xsize;
            }
        } else if(windDirection == NW) {
            if(sideDecision == 0) {
                ypos = gameMap->ysize;
                xpos = createRandomValue(gameMap->xsize);
            } else {
                ypos = createRandomValue(gameMap->ysize);
                ;
                xpos = gameMap->xsize;
            }
        }
    }
    FalloutType ft;
    int sum = 0;
    int newFallout = createRandomValue(100);
    for(int i = 0; i < falloutPercentages.size(); i++) {
        sum += falloutPercentages[i];
        if(newFallout <= sum) {
            ft = static_cast<FalloutType>(i);
            break;
        }
    }
    newArea = new WeatherArea(gameMap, xpos, ypos, width, height, 6, ft, createRandomValue());
    this->addWeatherArea(newArea, currentTime);
}


void WeatherSystem::update(GameTime currentTime) {
    GameTime startTime;
    startTime.set(0,0);
   
    if((currentMode == RANDOMMODE)  && (currentTime.turn() % timeInterval == 0)) {
            if(currentTime.abstime == startTime.abstime) {
                if(!isSeedValueSet()) {
                    setSeedValue();
                } else {
                    srand(seedValue);
                }
            }
            Direction newDirection = randomWindChange(currentTime.turn());
            for(int i=0; i < areaSpawnAmount; i++) {
                randomWeatherChange(currentTime, newDirection);
            }
        }

    WeatherAreas::iterator lower = weatherAreas.lower_bound(startTime);
    WeatherAreas::iterator medium = weatherAreas.upper_bound(currentTime); //currentTurn-1
    WeatherAreas::iterator upper = weatherAreas.upper_bound(currentTime);
    WeatherAreas::iterator it;
    WindChanges::iterator itTr = windTriggers.find(currentTime.turn());
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
            it->second->updateMovementVector(windspeed, globalWindDirection, getWindspeed2FieldRatio());
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

void WeatherSystem::skipRandomValue() const {
    unsigned int random_integer = rand();
}

unsigned int WeatherSystem::createRandomValue(unsigned int limit) {
    ++access2RandCount;
    unsigned int random_integer = rand();
    if(limit == 0) {
        limit = 1;
    }
    return (random_integer % limit);
}

float WeatherSystem::createRandomValue(float lowerLimit, float upperLimit) {
    ++access2RandCount;
    return (lowerLimit + (float)rand()/(float)RAND_MAX * (upperLimit - lowerLimit));
}

void WeatherSystem::addGlobalWindChange(int speed, Direction direction,  int turn) throw (IllegalValueException) {
    WindData data ={speed, direction};
    pair<int, WindData>p(turn, data);
    windTriggers.insert(p);
}

void WeatherSystem::removeWindChange(int turn, WindData data) {
    WindChanges::iterator windChange = windTriggers.find(turn);
    windTriggers.erase(windChange);
}

const int WeatherSystem::getQueuedWindChangesSize() const {
    return windTriggers.size();
}


pair<int, WindData> WeatherSystem::getNthWindChange(int n) const {
    int k = 0;
    WindChanges::const_iterator it;
    it = windTriggers.begin();
    while(k<n) {
    ++it;
    ++k;
    }
    return *it;
}


void WeatherSystem::write(tnstream& outputStream) const {
    outputStream.writeInt(WEATHERVERSION);
    outputStream.writeFloat( seedValue );
    outputStream.writeInt(seedValueIsSet);
    outputStream.writeInt(access2RandCount);
    outputStream.writeInt( timeInterval );
    outputStream.writeInt(areaSpawnAmount);
    outputStream.writeInt(currentMode);


    outputStream.writeInt(windspeed );
    outputStream.writeFloat( windspeed2FieldRatio );
    outputStream.writeInt(globalWindDirection );

    outputStream.writeFloat(lowerRandomSize);
    outputStream.writeFloat(upperRandomSize);

    outputStream.writeInt(falloutPercentages.size());
    for (int i = 0; i < falloutPercentages.size(); i++ ) {
    outputStream.writeInt(falloutPercentages[i]);
    }

    outputStream.writeInt(windDirPercentages.size());
    for (int i  = 0; i < windDirPercentages.size(); i++ ) {
    outputStream.writeInt(windDirPercentages[i]);
    }

    outputStream.writeInt(windSpeedPercentages.size());
    for (int i = 0; i < windSpeedPercentages.size(); i++ ) {
    outputStream.writeInt(windSpeedPercentages[i]);
    }

    outputStream.writeInt(weatherAreas.size());
    for ( WeatherAreas::const_iterator i = weatherAreas.begin(); i != weatherAreas.end(); ++i ) {
    outputStream.writeInt(i->first.turn());
        outputStream.writeInt(i->first.move());
        i->second->write ( outputStream );
    }

    outputStream.writeInt(windTriggers.size());
    for ( WindChanges::const_iterator i = windTriggers.begin(); i != windTriggers.end(); ++i ) {
    outputStream.writeInt(i->first);
        outputStream.writeInt(i->second.speed);
        outputStream.writeInt(i->second.direction);
    }
}

void WeatherSystem::read (tnstream& inputStream) {

    int version = inputStream.readInt();
    seedValue = inputStream.readFloat();
    seedValueIsSet = inputStream.readInt();
    access2RandCount = inputStream.readInt();
    timeInterval = inputStream.readInt();
    areaSpawnAmount = inputStream.readInt();
    currentMode  = static_cast<WeatherSystemMode>(inputStream.readInt());

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
        int move = inputStream.readInt();
        GameTime time;
        time.set(turn, move);
        WeatherArea* newArea = new WeatherArea(gameMap);
        newArea->read(inputStream);
        pair<GameTime, WeatherArea*>p(time, newArea);
        weatherAreas.insert(p);
    }
    size = inputStream.readInt();
    for(int i = 0; i < size; i++) {
        int turn = inputStream.readInt();
        WindData data;
        data.speed = inputStream.readInt();
        data.direction = static_cast<Direction>(inputStream.readInt());
        pair<int, WindData>p(turn, data);
        windTriggers.insert(p);
    }
    srand(seedValue);
    for(int i = 0; i < access2RandCount; i++) {
        skipRandomValue();

    }
}





