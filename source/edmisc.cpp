/** \file edmisc.cpp
    \brief various functions for the mapeditor
*/

/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2010  Martin Bickel  and  Marc Schellenberger

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; see the file COPYING. If not, write to the
    Free Software Foundation, Inc., 59 Temple Place, Suite 330,
    Boston, MA  02111-1307  USA
*/

// #include <QClipbrd.hpp>

#include <string>
#include <iostream>
#include <math.h>


#include "sdl/graphics.h"
#include "vehicletype.h"
#include "buildingtype.h"
#include "edmisc.h"
#include "loadbi3.h"
#include "edgen.h"
#include "edselfnt.h"
#include "edglobal.h"
#include "dialogs/pwd_dlg.h"
#include "mapdisplay.h"
#include "graphicset.h"
#include "itemrepository.h"
#include "textfileparser.h"
#include "textfile_evaluation.h"
#include "textfiletags.h"
#include "clipboard.h"
#include "dialogs/cargowidget.h"
#include "dialogs/fieldmarker.h"
#include "dialogs/newmap.h"
#include "stack.h"

#include "unitset.h"
#include "maped-mainscreen.h"
#include "gameevents.h"
#include "gameevent_dialogs.h"
#include "dialogs/fileselector.h"
#include "pgeventsupplier.h"
#include "dialogs/edittechadapter.h"
#include "spfst-legacy.h"
#include "textfile_evaluation.h"
#include "lua/luarunner.h"
#include "lua/luastate.h"
#include "widgets/multilistbox.h"
#include "pgpropertyfield_integer.h"
#include "dialogs/messagedialog.h"
#include "widgets/textrenderer.h"
#include "widgets/playerselector.h"

bool       mapsaved;



void placeCurrentItem()
{
    if ( selection.getSelection() && actmap->getCursor().valid() ) {
        if ( selection.getSelection()->supportMultiFieldPlacement()  && selection.brushSize > 1 ) {
            circularFieldIterator( actmap, actmap->getCursor(), 0, selection.brushSize-1, FieldIterationFunctor( selection.getSelection(), &MapComponent::vPlace ));
        } else {
            selection.getSelection()->place( actmap->getCursor() );
        }
        mapChanged( actmap );
    }
}

bool removeCurrentItem()
{
    if ( selection.getSelection() && actmap->getCursor().valid() )
        return selection.getSelection()->remove( actmap->getCursor() );
    else
        return false;
}


bool mousePressedOnField( const MapCoordinate& pos, const SPoint& mousePos, bool cursorChanged, int button, int prio )
{
    if ( prio > 1 )
        return false;

    if ( button == 1 ) {
        execaction_ev( act_primaryAction );
        return true;
    } else
        return false;
}

bool mouseDraggedToField( const MapCoordinate& pos, const SPoint& mousePos, bool cursorChanged, int prio )
{
    if ( prio > 1 )
        return false;
    execaction_ev( act_primaryAction );
    return true;
}


// � Checkobject

Uint8 checkobject(MapField* pf)
{
    return !pf->objects.empty();
}



// � PutResource

void tputresources :: init ( int sx, int sy, int dst, int restype, int resmax, int resmin )
{
    centerPos = MapCoordinate(sx,sy);
    initsearch( centerPos, dst, 0);
    resourcetype = restype;
    maxresource = resmax;
    minresource = resmin;
    maxdst = dst;
    startsearch();
}

void tputresources :: testfield ( const MapCoordinate& mc )
{
    int dist = beeline ( mc, centerPos ) / 10;
    int m = maxresource - dist * ( maxresource - minresource ) / maxdst;

    MapField* fld = gamemap->getField ( mc );
    if ( resourcetype == 1 )
        fld->material = min( 255, fld->material + m);
    else
        fld->fuel = min( 255, fld->fuel + m );
}

void tputresourcesdlg :: init ( void )
{
    resourcetype = 1;
    restp2 = 0;
    status = 0;
    tdialogbox::init();
    title = "set resources";
    xsize = 400;
    ysize = 300;
    maxresource = 200;
    minresource = 0;
    dist = 10;

    addbutton ( "~O~k", 10, ysize - 30, xsize/2 - 5, ysize - 10 , 0, 1, 1, true );
    addkey ( 1, ct_enter );
    addbutton ( "~C~ancel", xsize/2 + 5, ysize - 30, xsize-10 - 5, ysize - 10 , 0, 1, 2, true );
    addkey ( 2, ct_esc );

    addbutton ( "ma~x~ resources ( at center )", xsize/2 + 5, 80, xsize-20 - 5, 100 , 2, 1, 3, true );
    addeingabe ( 3, &maxresource, 0, 255 );

    addbutton ( "mi~n~ resources ", xsize/2 + 5, 120, xsize-20 - 5, 140 , 2, 1, 4, true );
    addeingabe ( 4, &minresource, 0, 255 );

    addbutton ( "max ~d~istance", 20, 120, xsize/2-5, 140 , 2, 1, 5, true );
    addeingabe ( 5, &dist, 1, 20 );

    addbutton ( "~m~aterial", 20, 160, xsize/2-5, 180, 3, 10, 6, true );
    addeingabe ( 6, &resourcetype, black, dblue );

    addbutton ( "~f~uel", xsize/2 + 5, 160, xsize - 20, 180, 3, 10, 7, true );
    addeingabe ( 7, &restp2, black, dblue );

    buildgraphics();

}

void tputresourcesdlg :: buttonpressed ( int id )
{
    tdialogbox :: buttonpressed ( id );

    switch ( id ) {
    case 1:
        status = 11;
        break;
    case 2:
        status = 10;
        break;
    case 6:
        restp2 = 0;
        enablebutton ( 7 );
        break;
    case 7:
        resourcetype = 0;
        enablebutton ( 6 );
        break;
    } /* endswitch */

}

void tputresourcesdlg :: run ( void )
{
    mousevisible ( true );
    do {
        tdialogbox :: run ( );
    } while ( status < 10 ); /* enddo */
    if ( status == 11 ) {
        tputresources pr ( actmap );
        pr.init ( actmap->getCursor().x, actmap->getCursor().y, dist, resourcetype ? 1 : 2, maxresource, minresource );
    }
}




// � Lines

void lines(int x1,int y1,int x2,int y2)
{
    line(x1,y1,x2,y1,white);
    line(x2,y1,x2,y2,darkgray);
    line(x1,y2,x2,y2,darkgray);
    line(x1,y1,x1,y2,white);
}



class PlayerChange : public ASC_PG_Dialog {
    GameMap* gameMap;
    PlayerSelector* left;
    PlayerSelector* right;
    PG_DropDown* action;

    bool apply() {
        if ( left->getFirstSelectedPlayer() == right->getFirstSelectedPlayer())
            return false;

        if (     left->getFirstSelectedPlayer() < 0 ||  left->getFirstSelectedPlayer() >= gameMap->getPlayerCount()
             || right->getFirstSelectedPlayer() < 0 || right->getFirstSelectedPlayer() >= gameMap->getPlayerCount()
             || action->GetSelectedItemIndex() < 0 || action->GetSelectedItemIndex() > 1 )
        return false;

        if ( action->GetSelectedItemIndex() == 0) {
            gameMap->getPlayer(left->getFirstSelectedPlayer()).swap ( gameMap->getPlayer(right->getFirstSelectedPlayer()) );
        } else {
            gameMap->getPlayer(right->getFirstSelectedPlayer()).mergeFrom( gameMap->getPlayer(left->getFirstSelectedPlayer()) );
        }
        mapChanged(gameMap);
        displaymap();
        return true;
    }

public:
    PlayerChange(GameMap* gameMap) : ASC_PG_Dialog(NULL, PG_Rect(-1, -1, 600, 300), "Exchange Players"), gameMap(gameMap) {

        left = new PlayerSelector(this, PG_Rect(10, 30, 150, 200), gameMap, false, 0, 2);
        action = new PG_DropDown(this, PG_Rect(170, 120, 130, 20));
        action->AddItem("<- exchange ->", NULL);
        action->AddItem("merge into ->", NULL);

        right = new PlayerSelector(this, PG_Rect(310, 30, 150, 200), gameMap, false, 0, 2);
        (new PG_Button(this, PG_Rect(470, 120, 60, 20), "Apply"))->sigClick.connect(sigc::hide( sigc::mem_fun( *this, &PlayerChange::apply )));

        AddStandardButton("OK")->sigClick.connect(sigc::hide( sigc::mem_fun( *this, &PlayerChange::QuitModal)));
    }
};

void playerchange(void)
{
    PlayerChange playerChange(actmap);
    playerChange.Show();
    playerChange.RunModal();
}





#ifndef pbpeditor

void         k_savemap(bool saveas)
{
    ASCString filename;

    int nameavail = 0;
    if ( !actmap->preferredFileNames.mapname[0].empty() ) {
        nameavail = 1;
        filename = actmap->preferredFileNames.mapname[0];;
    }

    if ( saveas || !nameavail ) {
        filename = selectFile(mapextension, false);
    }
    if ( !filename.empty() ) {
        mapsaved = true;
        actmap->preferredFileNames.mapname[0] = filename;
        savemap( filename, actmap );
        displaymap();
    }
}


void         k_loadmap(void)
{
    ASCString s1 = selectFile( mapextension, true );
    if ( !s1.empty() ) {
        StatusMessageWindowHolder smw = MessagingHub::Instance().infoMessageWindow( "loading map " + s1 );
        GameMap* mp = mapLoadingExceptionChecker( s1, MapLoadingFunction( tmaploaders::loadmap ));
        if ( !mp )
            return;

        /*
        if ( mp->campaign.avail && !mp->campaign.directaccess && !mp->codeWord.empty() ) {
            tlockdispspfld ldsf;
            removemessage();
            Password pwd;
            pwd.setUnencoded ( mp->codeWord );
            if ( enterpassword ( pwd ) == ;
        } else
            removemessage();
*/
        
         delete actmap;
         actmap =  mp;

        displaymap();
        mapsaved = true;
    }
}

#else

#include "pbpeditor.cpp"

#endif


void selectUnitFromMap ( GameMap* gamemap, MapCoordinate& pos )
{
    SelectFromMap::CoordinateList list;
    list.push_back( pos );

    SelectFromMap sfm( list, gamemap );
    sfm.Show();
    sfm.RunModal();

    if ( list.empty() )
        pos = MapCoordinate( -1, -1 );
    else
        pos = *list.begin();
}



void         setstartvariables(void)
{
    activefontsettings.font = schriften.smallarial;
    activefontsettings.color = 1;
    activefontsettings. background = 0;

    mapsaved = true;

}

//* � FillPolygonevent



//* � FillPolygonbdt
/*
void tfillpolygonbodentyp::setpointabs    ( int x,  int y  )
{
       tfield* ffield = getfield ( x , y );
       if (ffield) {
           ffield->a.temp = tempvalue;
           if ( auswahl->weather[auswahlw] )
              ffield->typ = auswahl->weather[auswahlw];
           else
              ffield->typ = auswahl->weather[0];
           ffield->direction = auswahld;
           ffield->setparams();
           if (ffield->vehicle != NULL)
              if ( terrainaccessible(ffield,ffield->vehicle) == false ) {
                 delete ffield->vehicle;
                 ffield->vehicle = NULL;
              }
       }
}

void tfillpolygonbodentyp::initevent ( void )
{
}

// � FillPolygonunit

void tfillpolygonunit::setpointabs    ( int x,  int y  )
{
       tfield* ffield = getfield ( x , y );
       if (ffield) {
          if ( terrainaccessible(ffield,ffield->vehicle) )
               {
                  if (ffield->vehicle != NULL) {
                     delete ffield->vehicle;
                     ffield->vehicle = NULL;
                  }
                  if (auswahlf != NULL) {
                     ffield->vehicle = new Vehicle ( auswahlf,actmap, farbwahl );
                     ffield->vehicle->fillMagically();
                     ffield->vehicle->height=1;
                     while ( ! ( ( ( ( ffield->vehicle->height & ffield->vehicle->typ->height ) > 0) && (terrainaccessible(ffield,ffield->vehicle) == 2) ) ) && (ffield->vehicle->height != 0) )
                        ffield->vehicle->height = ffield->vehicle->height * 2;
                     for (i = 0; i <= 31; i++) ffield->vehicle->loading[i] = NULL;
                     if (ffield->vehicle->height == 0 ) {
                        delete ffield->vehicle;
                        ffield->vehicle = NULL;
                     }
                     else ffield->vehicle->setMovement ( ffield->vehicle->typ->movement[getFirstBit(ffield->vehicle->height)] );
                     ffield->vehicle->direction = auswahld;
                  }
            }
            else
               if (auswahlf == NULL)
                  if (ffield->vehicle != NULL) {
                     delete ffield->vehicle;
                     ffield->vehicle=NULL;
                  }
            ffield->a.temp = tempvalue;
       }
}

void tfillpolygonunit::initevent ( void )
{
}


// � ChangePoly
*/


class  ShowPolygonUsingTemps : public PolygonPainterSquareCoordinate {
protected:
    virtual void setpointabs ( int x,  int y  ) {
        MapField* ffield = getfield ( x , y );
        if (ffield)
            ffield->setaTemp2(1);
    };
public:
    bool paintPolygon   (  const Poly_gon& poly ) {
        bool res = PolygonPainterSquareCoordinate::paintPolygon ( poly );
        for ( int i = 0; i < poly.vertex.size(); ++i ) {
            MapField* ffield = actmap->getField ( poly.vertex[i] );
            if (ffield)
                ffield->setaTemp(1);
        }
        return res;
    };
};


class PolygonEditor : public SelectFromMap {
protected:
    void showFieldMarking( const CoordinateList& coordinateList )
    {
        Poly_gon poly;
        for ( CoordinateList::const_iterator i = coordinateList.begin(); i != coordinateList.end(); ++i )
            poly.vertex.push_back( MapCoordinate(*i) );

        ShowPolygonUsingTemps sput;
        if ( !sput.paintPolygon ( poly ) )
            displaymessage("Invalid Polygon !",1 );

        repaintMap();
    }

public:
    PolygonEditor( CoordinateList& list, GameMap* map ) : SelectFromMap( list, map ) {};
};



void editpolygon(Poly_gon& poly)
{
    savemap ( "_backup_polygoneditor.map", actmap );

    PolygonEditor::CoordinateList list;

    for ( Poly_gon::VertexIterator  i = poly.vertex.begin(); i != poly.vertex.end(); ++i )
        list.push_back( *i );

    PolygonEditor cp ( list, actmap );
    cp.Show();
    cp.RunModal();

    poly.vertex.clear();
    for ( PolygonEditor::CoordinateList::iterator i = list.begin(); i != list.end(); ++i )
        poly.vertex.push_back( *i );
}



void         newmap(void)
{
    GameMap* map = createNewMap();
    if ( map ) {
        delete actmap;
        actmap = map;
        // displaymap();
        mapChanged( actmap );
        // tspfldloaders::mapLoaded( actmap );
    }
}


//* � MapVals



class BuildingValues : public tdialogbox {
    int action;
    Building& gbde;
    int rs,mrs;
    Resources plus,mplus, biplus,storage;
    int col;
    int damage;
    Uint8 tvisible;
    char name[260];
    int ammo[waffenanzahl];
    virtual void buttonpressed(int id);
    int lockmaxproduction;
    TemporaryContainerStorage buildingBackup; // if the editing is cancelled

public :
    BuildingValues ( Building& building ) : gbde ( building ), buildingBackup ( &building ) {};
    void init(void);
    virtual void run(void);
};


void         BuildingValues::init(void)
{
	Uint8      b;

    for ( int i = 0; i< waffenanzahl; i++ )
        ammo[i] = gbde.ammo[i];

    lockmaxproduction = 1;

    tdialogbox::init();
    action = 0;
    title = "Building Values";
    x1 = 0;
    xsize = 640;
    y1 = 10;
    ysize = 480;
    // int w = (xsize - 60) / 2;
    action = 0;

    windowstyle = windowstyle ^ dlg_in3d;

    storage = gbde.actstorage;
    plus = gbde.plus;
    mplus = gbde.maxplus;
    rs = gbde.researchpoints;
    mrs = gbde.maxresearchpoints;
    tvisible = gbde.visible;
    biplus = gbde.bi_resourceplus;
    col = gbde.color / 8;
    damage = gbde.damage;

    strcpy( name, gbde.name.c_str() );

    addbutton("~N~ame",15,50,215,70,1,1,10,true);
    addeingabe(10,&name[0],0,25);

    addbutton("~E~nergy-Storage",15,90,215,110,2,1,1,true);
    addeingabe(1,&storage.energy,0,gbde.getStorageCapacity().resource(0));

    addbutton("~M~aterial-Storage",15,130,215,150,2,1,2,true);
    addeingabe(2,&storage.material,0,gbde.getStorageCapacity().resource(1));

    addbutton("~F~uel-Storage",15,170,215,190,2,1,3,true);
    addeingabe(3,&storage.fuel,0,gbde.getStorageCapacity().resource(2));

    if ( gbde.typ->hasFunction( ContainerBaseType::MatterConverter  ) ||
            gbde.typ->hasFunction( ContainerBaseType::SolarPowerPlant  ) ||
            gbde.typ->hasFunction( ContainerBaseType::WindPowerPlant  ) ||
            gbde.typ->hasFunction( ContainerBaseType::MiningStation  ) ||
            gbde.typ->hasFunction( ContainerBaseType::ResourceSink  ))
        b = true;
    else
        b = false;

    addbutton("Energy-Max-Plus",230,50,430,70,2,1,13,b);
    addeingabe(13,&mplus.energy,0,gbde.typ->maxplus.energy);

    if ( gbde.typ->hasFunction( ContainerBaseType::MatterConverter  ) ||
            gbde.typ->hasFunction( ContainerBaseType::MiningStation  ) ||
            gbde.typ->hasFunction( ContainerBaseType::ResourceSink  ))
        b = true;
    else
        b = false;


    addbutton("Energ~y~-Plus",230,90,430,110,2,1,4,b);
    addeingabe(4,&plus.energy,0,mplus.energy);

    if ( ((gbde.typ->hasFunction( ContainerBaseType::MatterConverter) || gbde.typ->hasFunction( ContainerBaseType::MiningStation)) && gbde.typ->efficiencymaterial )
            || gbde.typ->hasFunction( ContainerBaseType::ResourceSink  ) )
        b = true;
    else
        b = false;

    addbutton("Material-Max-Plus",230,130,430,150,2,1,14,b);
    addeingabe(14,&mplus.material,0,gbde.typ->maxplus.material);

    addbutton("M~a~terial-Plus",230,170,430,190,2,1,5,b);
    addeingabe(5,&plus.material,0,mplus.material);

    if ( ((gbde.typ->hasFunction( ContainerBaseType::MatterConverter) || gbde.typ->hasFunction( ContainerBaseType::MiningStation)) && gbde.typ->efficiencyfuel )
            || gbde.typ->hasFunction( ContainerBaseType::ResourceSink  ) )
        b = true;
    else
        b = false;

    addbutton("Fuel-Max-Plus",230,210,430,230,2,1,15,b);
    addeingabe(15,&mplus.fuel,0,gbde.typ->maxplus.fuel);

    addbutton("F~u~el-Plus",230,250,430,270,2,1,6,b);
    addeingabe(6,&plus.fuel, 0, mplus.fuel);

    if ( gbde.typ->hasFunction( ContainerBaseType::Research) || ( gbde.typ->maxresearchpoints > 0))
        b = true;
    else
        b = false;

    addbutton("~R~esearch-Points",15,210,215,230,2,1,9,b);
    if ( gbde.typ->hasFunction(ContainerBaseType::Research))
        addeingabe(9,&rs,0,gbde.typ->maxresearchpoints);
    else
        addeingabe(9,&rs,gbde.typ->maxresearchpoints,gbde.typ->maxresearchpoints);

    addbutton("Ma~x~research-Points",15,250,215,270,2,1,11,b);
    if ( gbde.typ->hasFunction(ContainerBaseType::Research))
        addeingabe(11,&mrs,0,gbde.typ->maxresearchpoints);
    else
        addeingabe(11,&mrs,gbde.typ->maxresearchpoints,gbde.typ->maxresearchpoints);


    addbutton("~V~isible",15,290,215,300,3,1,12,true);
    addeingabe(12,&tvisible,0,lightgray);

    addbutton("~L~ock MaxPlus ratio",230,290,430,300,3,1,120,true);
    addeingabe(120,&lockmaxproduction,0,lightgray);


    addbutton("~C~olor",230,370,430,390,2,1,104,true);
    addeingabe(104,&col,0,8);

    addbutton("~D~amage",230,410,430,430,2,1,105,true);
    addeingabe(105,&damage,0,99);


    addbutton("BI energy plus",15,330,215,350,2,1,101,1);
    addeingabe(101,&biplus.energy,0,maxint);

    addbutton("BI material plus",230,330,430,350,2,1,102,1);
    addeingabe(102,&biplus.material,0,maxint);

    addbutton("BI fuel plus",15,370,215,390,2,1,103,1);
    addeingabe(103,&biplus.fuel,0,maxint);


    addbutton("~S~et Values",10,ysize - 40, xsize/3-5,ysize - 10,0,1,7,true);
    addkey(7,ct_enter);

    /*



    addbutton("Help (~F1~)", xsize/3+5, ysize - 40, xsize/3*2-5, ysize-10, 0, 1, 110, true );
    addkey(110, ct_f1 );
    */

    addbutton("~C~ancel",xsize/3*2+5,ysize - 40,xsize-10,ysize - 10,0,1,8,true);
    addkey(8, ct_esc );


    for ( int i = 0; i < waffenanzahl; i++ ) {
        addbutton ( cwaffentypen[i], 460, 90+i*40, 620, 110+i*40, 2, 1, 200+i, 1 );
        addeingabe(200+i, &ammo[i], 0, maxint );
    }

    buildgraphics();
    line(x1+450, y1+45, x1+450, y1+55+9*40, darkgray );
    activefontsettings.length =0;
    activefontsettings.font = schriften.smallarial;
    showtext2("ammunition:", x1 + 460, y1 + 50 );

    mousevisible(true);
}


void         BuildingValues::run(void)
{
    do {
        tdialogbox::run();
    }  while (!((taste == ct_esc) || (action == 1)));
}


void         BuildingValues::buttonpressed(int         id)
{
    switch (id) {
    case 4:            // energy, material & fuel plus
    case 5:
    case 6: {
        int changed_resource = id - 4;
        for ( int r = 0; r < 3; r++ )
            if ( r != changed_resource )
                if ( mplus.resource(changed_resource) ) {
                    int a = mplus.resource(r) * plus.resource(changed_resource) / mplus.resource(changed_resource);
                    if ( a != plus.resource(r) ) {
                        plus.resource(r) = a;
                        showbutton ( 4 + r );
                    }
                }
    }
    break;
    case 13:        // energy, material and fuel maxplus
    case 14:
    case 15: {
        int changed_resource = id - 13;
        if ( lockmaxproduction )
            for ( int r = 0; r < 3; r++ )
                if ( r != changed_resource )
                    if ( gbde.typ->maxplus.resource(changed_resource) ) {
                        int a = gbde.typ->maxplus.resource(r) * mplus.resource(changed_resource) / gbde.typ->maxplus.resource(changed_resource);
                        if ( a != mplus.resource(r) ) {
                            mplus.resource(r) = a;
                            showbutton ( 13 + r );
                        }
                    }

        for ( int r = 0; r < 3; r++ ) {
            if ( (mplus.resource(r) >= 0 && plus.resource(r) > mplus.resource(r) ) ||
                    (mplus.resource(r) <  0 && plus.resource(r) < mplus.resource(r) )) {
                plus.resource(r) = mplus.resource(r);
                showbutton ( 4 + r );
            }
            addeingabe(4+r, &plus.resource(r), 0, mplus.resource(r) );
        }
    }
    break;
    case 7: {
        mapsaved = false;
        action = 1;
        gbde.actstorage = storage;

        gbde.plus = plus;
        gbde.maxplus = mplus;
        if ( col != gbde.color/8 )
            gbde.convert ( col );

        gbde.researchpoints = rs;
        gbde.maxresearchpoints = mrs;
        gbde.visible = tvisible;
        gbde.bi_resourceplus = biplus;
        gbde.name = name;
        gbde.damage = damage;
        for ( int i = 0; i< waffenanzahl; i++ )
            gbde.ammo[i] = ammo[i];

    }
    break;
    case 8:
        action = 1;
        break;

        /*

        case 13: {
                        addeingabe(4,&plus.energy,0,mplus.energy);
                        if (mplus.energy < plus.energy ) {
                           plus.energy = mplus.energy;
                           enablebutton(4);
                        }
                     }
           break;
        case 14: {
                      addeingabe(5,&plus.material,0,mplus.material);
                         if (mplus.material < plus.material) {
                            plus.material =mplus.material;
                            enablebutton(5);
                         }
                      }

           break;
        case 15: {
                      addeingabe(6,&plus.fuel,0,mplus.fuel);
                         if (plus.fuel > mplus.fuel) {
                            plus.fuel = mplus.fuel;
                            enablebutton(6);
                         }
                      }
           break;
        */
    case 110:
        help ( 41 );
        break;

    }
}


void         changebuildingvalues( Building& b )
{
    BuildingValues bval ( b );
    bval.init();
    bval.run();
    bval.done();

    displaymap();
}


int selectString( int lc, const char* title, const char** text, int itemNum )
{
    vector<ASCString> entries;
    for ( int i = 0; i< itemNum; ++i )
        entries.push_back(text[i]);

    vector<ASCString> buttons;
    buttons.push_back("OK");
    buttons.push_back("Cancel");

    pair<int,int> result = new_chooseString(title,entries,buttons,lc);

    if ( result.first == 0 )
        return result.second;
    else
        return lc;
}


class EditAiParam : public tdialogbox {
    Vehicle* unit;
    TemporaryContainerStorage tus;
    int action;
    AiParameter& aiv;
    int z;
public:
    EditAiParam ( Vehicle* veh, int player ) : unit ( veh ), tus ( veh ), aiv ( *veh->aiparam[player] ) {};
    void init ( );
    void run ( );
    void buttonpressed ( int id );
    int  getcapabilities ( void ) {
        return 1;
    };
};


void         EditAiParam::init(  )
{
    tdialogbox::init();
    action = 0;
    title = "Unit AI Values";

    windowstyle = windowstyle ^ dlg_in3d;

    x1 = 20;
    xsize = 600;
    y1 = 40;
    ysize = 400;
    int w = (xsize - 60) / 2;
    action = 0;

    addbutton("dest ~X~",50,80,250,100,2,1,1,true);
    addeingabe(1, &aiv.dest.x, minint, maxint);

    addbutton("dest ~Y~",50,120,250,140,2,1,2,true);
    addeingabe(2, &aiv.dest.y, minint, maxint);

    z = aiv.dest.getNumericalHeight();
    addbutton("dest ~Z~",50,160,250,180,2,1,3,true);
    addeingabe(3, &z, minint, maxint);

    addbutton("dest ~N~WID",50,200,250,220,2,1,4,true);
    addeingabe(4, &aiv.dest_nwid, minint, maxint);

    addbutton("~V~alue",410,80,570,100,2,1,10,true);
    addeingabe( 10, &aiv.value, minint, maxint );

    addbutton("~A~dded value",410,120,570,140,2,1,11,true);
    addeingabe( 11, &aiv.addedValue, minint, maxint );

    addbutton("~R~eset task after completion",410,160,570,180,3,1,12,true);
    addeingabe( 12, &aiv.resetAfterJobCompletion, black, dblue );


    addbutton("~S~elect Dest",50,240,250,260,0,1,22,true);

    addbutton("~T~ask",20,ysize - 70,20 + w,ysize - 50,0,1,20,true);
    addbutton("~J~ob",40 + w,ysize - 70,40 + 2 * w,ysize - 50,0,1,21,true);

    addbutton("~S~et Values",20,ysize - 40,20 + w,ysize - 10,0,1,30,true);
    addkey(30,ct_enter );
    addbutton("~C~ancel",40 + w,ysize - 40,40 + 2 * w,ysize - 10,0,1,31,true);
    addkey(31,ct_esc );

    buildgraphics();
    mousevisible(true);
}


void         EditAiParam::run(void)
{
    do {
        tdialogbox::run();
    }  while ( !action );
}


void         EditAiParam::buttonpressed(int         id)
{
    switch (id) {
    case 20 :
        aiv.setTask( AiParameter::Task( selectString ( aiv.getTask(), "Select Task", AItasks , AiParameter::taskNum) ));
        break;
    case 21 : {
        AiParameter::JobList j;
        j.push_back ( AiParameter::Job( selectString ( aiv.getJob(), "Select Job", AIjobs , AiParameter::jobNum) ) );
        aiv.setJob ( j );
    }
    break;
    /*
    case 22 : getxy ( &aiv.dest.x, &aiv.dest.y );
    aiv.dest.setnum ( aiv.dest.x, aiv.dest.y, -2 );
    z = -2;
    redraw();
    break;
    */
    case 30 :
        action = 1;
        aiv.dest.setnum ( aiv.dest.x, aiv.dest.y, z );
        break;
    case 31 :
        action = 1;
        tus.restore();
        break;
    } /* endswitch */
}


void unitAiProperties( Vehicle* veh )
{
    if ( !veh )
        return;

    static int player = 0;
    player = getid("player",player,0,7);
    if ( !veh->aiparam[player] )
        veh->aiparam[player] = new AiParameter ( veh );

    EditAiParam eap ( veh, player );
    eap.init();
    eap.run();
}



//* � Resource

class tres: public tdialogbox {
    MapField* pf2;
public :
    int action;
    int fuel,material;
    void init(void);
    virtual void run(void);
    virtual void buttonpressed(int id);
};



void         tres::init(void)
{
    int w;

    tdialogbox::init();
    action = 0;
    title = "Resources";
    x1 = 170;
    xsize = 200;
    y1 = 70;
    ysize = 200;
    w = (xsize - 60) / 2;
    action = 0;
    pf2 = getactfield();
    fuel = pf2->fuel;
    material = pf2->material;

    windowstyle = windowstyle ^ dlg_in3d;

    activefontsettings.length = 200;

    addbutton("~F~uel (0-255)",50,70,150,90,2,1,1,true);
    addeingabe(1,&fuel,0,255);
    addbutton("~M~aterial (0-255)",50,110,150,130,2,1,2,true);
    addeingabe(2,&material,0,255);

    addbutton("~S~et Vals",20,ysize - 40,20 + w,ysize - 10,0,1,7,true);
    addkey(7,ct_enter);
    addbutton("~C~ancel",40 + w,ysize - 40,40 + 2 * w,ysize - 10,0,1,8,true);

    buildgraphics();

    mousevisible(true);
}


void         tres::run(void)
{
    do {
        tdialogbox::run();
    }  while (!((taste == ct_esc) || (action == 1)));
}


void         tres::buttonpressed(int         id)
{
    if (id == 7) {
        mapsaved = false;
        action = 1;
        pf2->fuel = fuel;
        pf2->material = material;
    }
    if (id == 8) action = 1;
}


class FieldResourceEditor : public ASC_PG_Dialog {
   private:
      PG_PropertyEditor* editor;
      MapField* field;
      int material;
      int fuel;

      bool ok() {
          editor->Apply();
          field->material = material;
          field->fuel = fuel;
          QuitModal();
          return true;
      }

   public:
      FieldResourceEditor( MapField* mapfield) : ASC_PG_Dialog(NULL, PG_Rect(-1, -1, 300,200), "Edit Mineral Resources" ), field(mapfield), material(mapfield->material), fuel(mapfield->fuel)
      {
          StandardButtonDirection(Horizontal);
          editor = new PG_PropertyEditor(this, PG_Rect(20, 40, Width()-40, Height()-60));

          // PG_PropertyField_Integer doesn't work on bytes, hence the copy
          new PG_PropertyField_Integer(editor, "Material", &material);
          new PG_PropertyField_Integer(editor, "Fuel", &fuel);

          AddStandardButton("Cancel")->sigClick.connect(sigc::hide( sigc::mem_fun( *this, &FieldResourceEditor::QuitModal )));
          AddStandardButton("OK")->sigClick.connect(sigc::hide( sigc::mem_fun( *this, &FieldResourceEditor::ok )));
      }
};

void         changeresource(void)
{
    FieldResourceEditor fre( getactfield());
    fre.Show();
    fre.RunModal();
}

class MineStrenghEditor : public ASC_PG_Dialog {
   private:
      PG_PropertyEditor* editor;

      bool ok() {
          editor->Apply();
          QuitModal();
          return true;
      }

   public:
      MineStrenghEditor( MapField* mapfield) : ASC_PG_Dialog(NULL, PG_Rect(-1, -1, 300,200), "Edit Mine Punch" )
      {
          StandardButtonDirection(Horizontal);
          editor = new PG_PropertyEditor(this, PG_Rect(20, 40, Width()-40, Height()-60));

          for ( MapField::MineContainer::iterator i = mapfield->mines.begin(); i != mapfield->mines.end(); ++i )
              new PG_PropertyField_Integer(editor, MineType(i->type).getName(), &i->strength);

          AddStandardButton("Cancel")->sigClick.connect(sigc::hide( sigc::mem_fun( *this, &MineStrenghEditor::QuitModal )));
          AddStandardButton("OK")->sigClick.connect(sigc::hide( sigc::mem_fun( *this, &MineStrenghEditor::ok )));
      }
};

void         changeminestrength(void)
{
    if ( getactfield()->mines.empty() )
        return;

    MineStrenghEditor ms(getactfield());
    ms.Show();
    ms.RunModal();
}



void movebuilding ( void )
{
    warningMessage("sorry, not implemented yet in ASC2!");
#if 0
    mapsaved = false;
    MapField* fld = getactfield();
    if ( fld->vehicle ) {
        Vehicle* v = fld->vehicle;
        fld->vehicle = NULL;

        int x = v->xpos;
        int y = v->ypos;
        int res2 = -1;
        do {
            res2 = selectfield ( &x, &y );
            if ( res2 > 0 ) {
                if ( getfield(x,y)->vehicle || getfield(x,y)->vehicle  || !terrainaccessible(getfield(x,y),v)) {
                    displaymessage ( "you cannot place the vehicle here", 1 );
                    res2 = -1;
                } else {
                    getfield(x,y)->vehicle = v;
                    v->setnewposition( x, y );
                }
            }
        } while ( res2 < 0  );
        displaymap();

    }
    if ( fld->building ) {
        Building* bld = fld->building;

        bld->unchainbuildingfromfield ();

        MapCoordinate mc = bld->getEntry();
        MapCoordinate oldPosition = mc;
        int res2 = -1;
        do {
            res2 = selectfield ( &mc.x, &mc.y );
            if ( res2 > 0 ) {
                int res = bld->chainbuildingtofield ( mc );
                if ( res ) {
                    displaymessage ( "you cannot place the building here", 1 );
                    res2 = -1;
                }
            }
        } while ( res2 < 0  );
        if ( res2 == 0 ) {   // operation canceled
            if ( bld->chainbuildingtofield ( oldPosition ))
                displaymessage ( "severe inconsistency in movebuilding !", 1 );
        }
        displaymap();
    }
#endif
}


class SelectUnitSetFilter : public ASC_PG_Dialog {

    PG_ListBox* listbox;

    PG_Button* okButton;
    vector<PG_ListBoxItem*> items;

public:
    SelectUnitSetFilter() : ASC_PG_Dialog( NULL, PG_Rect( -1, -1, 300, 500 ), "Visible UnitSet Filter" ) {

        listbox = (new MultiListBox( this, PG_Rect( 10, 30, Width()-20, Height()-80)))->getListBox();


        for ( int i = 0; i < ItemFiltrationSystem::itemFilters.size(); i++ ) {
            PG_ListBoxItem* item = new PG_ListBoxItem( listbox, 20, ItemFiltrationSystem::itemFilters[i]->name );
            items.push_back( item );
            item->Select( !ItemFiltrationSystem::itemFilters[i]->isActive() );
        }

        okButton = AddStandardButton( "OK" );
        okButton->sigClick.connect( sigc::hide( sigc::mem_fun( *this, &SelectUnitSetFilter::ok )));
    };

    bool ok() {
        for ( int i = 0; i < items.size(); ++i )
            ItemFiltrationSystem::itemFilters[i]->setActive( !items[i]->IsSelected() );

        QuitModal();
        return true;
    }

    bool eventKeyDown(const SDL_KeyboardEvent* key) {

        if ( key->keysym.sym == SDLK_RETURN   )
            return ok();

        if ( key->keysym.sym == SDLK_ESCAPE   ) {
            QuitModal();
            return true;
        }

        if ( key->keysym.sym == 'a' ) {
            for ( vector<PG_ListBoxItem*>::const_iterator i = items.begin(); i != items.end(); ++i )
                (*i)->Select( true );
            listbox->Update();
            return true;
        }

        if ( key->keysym.sym == 'n' ) {
            for ( vector<PG_ListBoxItem*>::const_iterator i = items.begin(); i != items.end(); ++i )
                (*i)->Select( false );

            listbox->Update();
            return true;
        }

        return false;
    }
};


void selectunitsetfilter ( void )
{
    if ( ItemFiltrationSystem::itemFilters.size() > 0 ) {
        SelectUnitSetFilter susf;
        susf.Show();
        susf.RunModal();

        filtersChangedSignal();
    } else
        displaymessage ( " no Filters defined !", 1 );
}



class UnitTypeTransformation {

    int unitstransformed;
    int unitsnottransformed;

    VehicleType* transformvehicletype ( const VehicleType* type, int unitsetnum, int translationnum );
    void transformvehicle ( Vehicle* veh, int unitsetnum, int translationnum );
    set<int> vehicleTypesNotTransformed;
public:
    void run ( void );
} ;


VehicleType* UnitTypeTransformation :: transformvehicletype ( const VehicleType* type, int unitsetnum, int translationnum )
{
    for ( int i = 0; i < unitSets[unitsetnum]->transtab[translationnum]->translation.size(); i++ )
        if ( unitSets[unitsetnum]->transtab[translationnum]->translation[i].from == type->id ) {
            VehicleType* tp = vehicleTypeRepository.getObject_byID ( unitSets[unitsetnum]->transtab[translationnum]->translation[i].to );
            if ( tp )
                return tp;
        }

    vehicleTypesNotTransformed.insert( type->id );

    return NULL;
}

void  UnitTypeTransformation ::transformvehicle ( Vehicle* veh, int unitsetnum, int translationnum )
{
    for ( ContainerBase::Cargo::const_iterator i = veh->getCargo().begin(); i != veh->getCargo().end(); ++i )
        if ( *i )
            transformvehicle ( *i, unitsetnum, translationnum );

    VehicleType* nvt = transformvehicletype ( veh->typ, unitsetnum, translationnum );
    if ( !nvt ) {
        unitsnottransformed++;
        return;
    }
    veh->transform ( nvt );

    unitstransformed++;
}


void UnitTypeTransformation :: run ( void )
{
    vector<ASCString> unitSetNames;
    for ( UnitSets::iterator i = unitSets.begin(); i != unitSets.end(); ++i)
        unitSetNames.push_back((*i)->name);

    vector<ASCString> buttons;
    buttons.push_back("OK");
    buttons.push_back("Cancel");

    pair<int,int> unitSetSelection = new_chooseString("Select Unitset", unitSetNames, buttons, 0);
    if ( unitSetSelection.first == 1)
        return;

    if ( unitSetSelection.second < 0 || unitSetSelection.second >= unitSets.size() )
        return;

    const SingleUnitSet* unitSet = unitSets[unitSetSelection.second];

    vector<ASCString> translationTableNames;
    for (std::vector<SingleUnitSet::TranslationTable*>::const_iterator i = unitSet->transtab.begin(); i != unitSet->transtab.end(); ++i)
        translationTableNames.push_back((*i)->name);

    pair<int,int> translationSelection = new_chooseString("Select Translation", translationTableNames, buttons, 0);
    if ( translationSelection.first == 1)
        return;

    if ( translationSelection.second < 0 || translationSelection.second >= unitSet->transtab.size() )
        return;

    int unitsetnum = unitSetSelection.second;
    int translationsetnum = translationSelection.second;

    unitstransformed = 0;
    unitsnottransformed = 0;


    for ( int y = 0; y < actmap->ysize; y++ )
        for ( int x = 0; x < actmap->xsize; x++ ) {
            MapField* fld = getfield ( x, y );
            if ( fld->vehicle )
                transformvehicle ( fld->vehicle, unitsetnum, translationsetnum );
            if ( fld->building && (fld->bdt & getTerrainBitType(cbbuildingentry) ).any() ) {
                for ( ContainerBase::Cargo::const_iterator i = fld->building->getCargo().begin(); i != fld->building->getCargo().end(); ++i )
                    if ( *i )
                        transformvehicle ( *i, unitsetnum, translationsetnum );

                ContainerBase::Production prod = fld->building->getProduction();
                for ( ContainerBase::Production::iterator j = prod.begin(); j != prod.end(); ++j )
                    if ( *j && transformvehicletype ( *j, unitsetnum, translationsetnum ) ) {
                        *j = transformvehicletype ( *j, unitsetnum, translationsetnum );
                        ++unitstransformed;
                    }

                fld->building->setProductionLines( prod );
            }
        }

    if ( vehicleTypesNotTransformed.size() ) {
        ASCString s = "The following vehicles could not be transformed: ";
        for ( set<int>::const_iterator i = vehicleTypesNotTransformed.begin(); i != vehicleTypesNotTransformed.end(); ++i ) {
            s += "\n ID ";
            s += ASCString::toString( *i );
            s += " : ";
            VehicleType* vt = vehicleTypeRepository.getObject_byID ( *i );
            if ( !vt-> name.empty() )
                s += vt->name;
            else
                s += vt->description;
        }

        s += "\n";
        s += strrr ( unitstransformed );
        s += " units were transformed\n";
        s += strrr ( unitsnottransformed );
        s += " units were NOT transformed\n (production included)";


        ViewFormattedText vft ( "Warnings", s, PG_Rect(-1, -1, 500, 400) );
        vft.Show();
        vft.RunModal();;
    } else
        displaymessage ( "All units were transformed !\ntotal number: %d", 3, unitstransformed );


}

void unitsettransformation( void )
{
    UnitTypeTransformation utt;
    utt.run();
}


void MapSwitcher::deleteMaps()
{
    delete actmap;
    actmap = NULL;
    toggle();
    delete actmap;
    actmap = NULL;
}

void MapSwitcher :: toggle ( )
{
    maps[active].map = actmap;
    maps[active].changed = !mapsaved;

    if ( getMainScreenWidget() ) {
        MapDisplayPG* md = getMainScreenWidget()->getMapDisplay();
        if ( md )
            maps[active].windowpos = md->upperLeftCorner();
    }

    active = !active;



    actmap = maps[active].map;
    mapsaved = !maps[active].changed;


    if ( getMainScreenWidget() ) {
        MapDisplayPG* md = getMainScreenWidget()->getMapDisplay();
        if ( md )
            md->cursor.goTo( actmap->getCursor(), maps[active].windowpos );
    }

    displaymap();
    viewChanged();

    int x,y;
    SDL_Event ev;
    while ( PG_Application::GetEventSupplier()->GetMouseState(x,y))
        PG_Application::GetEventSupplier()->WaitEvent (&ev);
}

string MapSwitcher :: getName ()
{
    const char* name[2] = { "primary", "secondary" };
    string s;
    s = name[active];

    if ( actmap )
        if ( !actmap->preferredFileNames.mapname[0].empty() ) {
            s += " ( ";
            s += actmap->preferredFileNames.mapname[0];
            s += " ) ";
        }

    return s;
}

MapSwitcher::Action MapSwitcher :: getDefaultAction ( )
{
    if ( active )
        return select;
    else
        return set;
}


MapSwitcher mapSwitcher;


VehicleType* transform( int id, const vector<int>& translation )
{
    for ( int i = 0; i < translation.size()/2; i++ )
        if ( id == translation[i*2] )
            return vehicleTypeRepository.getObject_byID ( translation[i*2+1] );
    return NULL;
}

void transformMap ( )
{

    ASCString filename = selectFile( "*.ascmapxlat", true );

    if ( filename.empty() )
        return;


    vector<int> terraintranslation;
    vector<int> objecttranslation;
    vector<int> terrainobjtranslation;
    vector<int> buildingtranslation;
    vector<int> vehicletranslation;
    vector<int> o2ot_translation;
    vector<ASCString> techAdapterTranslation;
    
    try {
        tnfilestream s ( filename, tnstream::reading );

        TextFormatParser tfp ( &s );
        TextPropertyGroup* tpg = tfp.run();

        PropertyReadingContainer pc ( "maptranslation", tpg );

        pc.addIntegerArray ( "TerrainTranslation", terraintranslation );
        pc.addIntegerArray ( "TerrainObjTranslation", terrainobjtranslation );
        pc.addIntegerArray ( "ObjectTranslation", objecttranslation );
        pc.addIntegerArray ( "BuildingTranslation", buildingtranslation );
        if ( pc.find( "VehicleTranslation" ))
            pc.addIntegerArray ( "VehicleTranslation", vehicletranslation );

        if ( pc.find( "Object2ObjTerrainTranslation" ))
            pc.addIntegerArray( "Object2ObjTerrainTranslation", o2ot_translation );

        if ( pc.find( "TechAdapterTranslation" ))
           pc.addStringArray( "TechAdapterTranslation", techAdapterTranslation );
        
        delete tpg;
    }
    catch ( ParsingError err ) {
        displaymessage ( "Error parsing file : " + err.getMessage() , 1 );
        return;
    }
    catch ( ASCexception ) {
        displaymessage ( "Error reading file " + filename , 1 );
        return;
    }

    if ( terraintranslation.size() % 2 ) {
        displaymessage ( "Map Translation : terraintranslation - Invalid number of entries ", 1);
        return;
    }

    if ( terrainobjtranslation.size() % 3 ) {
        displaymessage ( "Map Translation : terrainobjtranslation - Invalid number of entries ", 1);
        return;
    }

    if ( objecttranslation.size() % 2 ) {
        displaymessage ( "Map Translation : objecttranslation - Invalid number of entries ", 1);
        return;
    }

    if ( buildingtranslation.size() % 2 ) {
        displaymessage ( "Map Translation : buildingtranslation - Invalid number of entries ", 1);
        return;
    }

    if ( vehicletranslation.size() % 2 ) {
        displaymessage ( "Map Translation : VehicleTranslation - Invalid number of entries ", 1);
        return;
    }

    if ( o2ot_translation.size() % 3 ) {
        displaymessage ( "Map Translation : Object2ObjTerrainTranslation - Invalid number of entries ", 1);
        return;
    }

    if ( techAdapterTranslation.size() % 2 ) {
       displaymessage ( "Map Translation : TechAdapterTranslation - Invalid number of entries ", 1);
       return;
    }


    for ( int y = 0; y < actmap->ysize; y++ )
        for ( int x = 0; x < actmap->xsize; x++ ) {
            MapField* fld = actmap->getField ( x, y );
            for ( int i = 0; i < terraintranslation.size()/2; i++ )
                if ( fld->typ->terraintype->id == terraintranslation[i*2] ) {
                    TerrainType* tt = terrainTypeRepository.getObject_byID ( terraintranslation[i*2+1] );
                    if ( tt ) {
                        TerrainType::Weather* tw = tt->weather[fld->getWeather()];
                        if ( !tw )
                            tw = tt->weather[0];
                        fld->typ = tw;
                        fld->setparams();
                    }
                }

            for ( int i = 0; i < terrainobjtranslation.size()/3; i++ )
                if ( fld->typ->terraintype->id == terrainobjtranslation[i*3] ) {
                    TerrainType* tt = terrainTypeRepository.getObject_byID ( terrainobjtranslation[i*3+1] );
                    if ( tt ) {
                        TerrainType::Weather* tw = tt->weather[fld->getWeather()];
                        if ( !tw )
                            tw = tt->weather[0];
                        fld->typ = tw;
                        fld->addobject ( objectTypeRepository.getObject_byID ( terrainobjtranslation[i*3+2] ), -1, true );
                        fld->setparams();
                    }
                }


            for ( int j = 0; j < fld->objects.size(); ++j )
                for ( int i = 0; i < objecttranslation.size() / 2; i++ )
                    if ( fld->objects[j].typ->id == objecttranslation[i*2] ) {
                        ObjectType* ot = objectTypeRepository.getObject_byID ( objecttranslation[i*2+1] );
                        if ( ot ) {
                            fld->objects[j].typ = ot;
                            fld->sortobjects();
                            fld->setparams();
                            j = -1; // restarting the outer loop
                            break;
                        } else {
                            fld->removeObject( fld->objects[j].typ, true );
                            j = -1;
                            break;
                        }
                    }
            for ( int b = 0; b < buildingtranslation.size()/2; ++b )
                if ( (fld->bdt & getTerrainBitType(cbbuildingentry)).any() && fld->building && fld->building->typ->id == buildingtranslation[b*2] ) {
                    BuildingType* newtype = buildingTypeRepository.getObject_byID ( buildingtranslation[b*2+1] );
                    if ( newtype ) {
                        Building* bld  = fld->building;
                        const BuildingType* orgtype = fld->building->typ;
                        MapCoordinate orgpos = bld->getEntry();
                        MapCoordinate pos = orgpos;
                        bld->unchainbuildingfromfield();
                        bld->typ = newtype;
                        int iteration = 0;
                        enum { trying, success, failed } state = trying;
                        do {
                            if ( bld->chainbuildingtofield( pos )) {
                                pos = getNeighbouringFieldCoordinate( orgpos, iteration );
                            } else
                                state = success;

                            ++iteration;

                            if ( iteration > 6 && state != success ) {
                                state = failed;
                                bld->typ = orgtype;
                                bld->chainbuildingtofield( orgpos );
                            }
                        } while ( state == trying );

                    } else {
                        delete fld->building;
                    }
                }

            for ( int j = 0; j < fld->objects.size(); ++j )
                for ( int i = 0; i < o2ot_translation.size() / 3; i++ )
                    if ( fld->objects[j].typ->id == o2ot_translation[i*3] ) {
                        fld->removeObject( fld->objects[j].typ, true );
                        TerrainType* tt = terrainTypeRepository.getObject_byID ( o2ot_translation[i*3+2] );
                        if ( tt ) {
                            TerrainType::Weather* tw = tt->weather[fld->getWeather()];
                            if ( !tw )
                                tw = tt->weather[0];
                            fld->typ = tw;
                        }
                        ObjectType* ot = objectTypeRepository.getObject_byID ( o2ot_translation[i*3+1] );
                        if ( ot )
                            fld->addobject ( ot, -1, true );


                        fld->setparams();

                        j = -1; // restarting the outer loop
                        break;
                    }
        }



    for ( int p = 0; p < actmap->getPlayerCount(); ++p ) {
        Player::VehicleList::iterator di = actmap->getPlayer(p).vehicleList.begin();
        while ( di != actmap->getPlayer(p).vehicleList.end() ) {
            bool found = false;
            for ( int i = 0; i < vehicletranslation.size()/2; ++i )
                if ( (*di)->typ->id == vehicletranslation[i*2] && vehicletranslation[i*2+1] <= 0 ) {
                    delete *di;
                    found = true;
                    break;
                }
            if ( found )
                di = actmap->getPlayer(p).vehicleList.begin();
            else
                ++di;
        }



        for ( Player::VehicleList::iterator i = actmap->getPlayer(p).vehicleList.begin(); i != actmap->getPlayer(p).vehicleList.end(); ++i) {
            (*i)->transform( transform( (*i)->typ->id, vehicletranslation));

            ContainerBase::Production prod = (*i)->getProduction();
            for ( ContainerBase::Production::iterator j = prod.begin(); j != prod.end(); ++j )
                if ( *j && transform((*j)->id, vehicletranslation) )
                    *j = transform((*j)->id, vehicletranslation);

            (*i)->setProductionLines( prod );
        }
        for ( Player::BuildingList::iterator i = actmap->getPlayer(p).buildingList.begin(); i != actmap->getPlayer(p).buildingList.end(); ++i) {
            ContainerBase::Production prod = (*i)->getProduction();
            for ( ContainerBase::Production::iterator j = prod.begin(); j != prod.end(); ++j )
                if ( *j && transform((*j)->id, vehicletranslation) )
                    *j = transform((*j)->id, vehicletranslation);

            (*i)->setProductionLines( prod );
        }
        
        Research& r = actmap->getPlayer(p).research;
        for ( vector<ASCString>::iterator tat = techAdapterTranslation.begin(); tat != techAdapterTranslation.end();  ) {
           ASCString from = *(tat++);
           ASCString to = *(tat++);
           
           vector<ASCString> triggeredAdapters( r.triggeredTechAdapter.begin(),  r.triggeredTechAdapter.end() );
           replace( triggeredAdapters.begin(), triggeredAdapters.end(), from, to );
           r.triggeredTechAdapter.clear();
           r.triggeredTechAdapter.insert( triggeredAdapters.begin(), triggeredAdapters.end() );
                                         
           replace( r.predefinedTechAdapter.begin(), r.predefinedTechAdapter.end(), from, to );
        }

    }

// for ( int i = 0; i < vehicleTypeRepository.getNum(); ++i )

    /*
    tn_file_buf_stream s ( "out.txt", tnstream::writing );
    for ( int j = 0; j < vehicletranslation.size()/2; j++ ) {
    Vehicletype* vt1 = vehicleTypeRepository.getObject_byID ( vehicletranslation[j*2] );
    Vehicletype* vt2 = vehicleTypeRepository.getObject_byID ( vehicletranslation[j*2+1] );
    ASCString st = ASCString::toString( vehicletranslation[j*2] ) + " " +
                   ASCString::toString( vehicletranslation[j*2+1] );


    if ( vt1 ) {
       st += " ; " + vt1->name;
       if ( vt2 )
          st += " -> " + vt2->name;
    }

    s.writeString( st + "\n" );
    }
    */

    calculateallobjects( actmap );
    displaymap();
}


class EditArchivalInformation : public tdialogbox {
    GameMap* gamemap;
    char maptitle[10000];
    char author[10000];
    ASCString description;
    char tags[10000];
    char requirements[10000];
    int action;
public:
    EditArchivalInformation ( GameMap* map );
    void init();
    void run();
    void buttonpressed ( int id );
};


EditArchivalInformation :: EditArchivalInformation ( GameMap* map ) : gamemap ( map )
{
    strcpy ( maptitle, map->maptitle.c_str() );
    strcpy ( author, map->archivalInformation.author.c_str() );
    description = map->archivalInformation.description;
    strcpy ( tags, map->archivalInformation.tags.c_str() );
    strcpy ( requirements, map->archivalInformation.requirements.c_str() );
}

void EditArchivalInformation::init()
{
    tdialogbox::init();
    title = "Edit Archival Information";
    xsize = 600;
    ysize = 410;
    action = 0;


    addbutton("~T~itle",20,70,xsize - 20,90,1,1,10,true);
    addeingabe(10,maptitle,0,10000);

    addbutton("~A~uthor",20,120,xsize - 20,140,1,1,11,true);
    addeingabe(11,author,0,10000);

    addbutton("Ta~g~s",20,170,xsize - 20,190,1,1,12,true);
    addeingabe(12,tags,0,10000);

    addbutton("~R~equirements ( see http://asc-hq.sf.net/req.php for possible values)",20,220,xsize - 20,240,1,1,13,true);
    addeingabe(13,requirements,0,10000);


    addbutton("Edit ~D~escription",20,ysize - 70,170,ysize - 50,0,1,14,true);

    addbutton("~O~k",20,ysize - 40,xsize/2-10,ysize - 20,0,1,1,true);
    addkey(1,ct_enter);
    addbutton("~C~ancel",xsize/2+10,ysize - 40,xsize-20,ysize - 20,0,1,2,true);
    addkey(2,ct_esc);

    buildgraphics();
}

void EditArchivalInformation :: buttonpressed ( int id )
{
    switch ( id ) {
    case 14: {
        MultilineEdit mle ( description, "Map Description" );
        mle.init();
        mle.run();
        mle.done();
        break;
    }
    case 1:
        gamemap->archivalInformation.author = author;
        gamemap->archivalInformation.tags   = tags;
        gamemap->archivalInformation.requirements   = requirements;
        gamemap->archivalInformation.description   = description;
        gamemap->maptitle = maptitle;
        mapsaved = false;
    case 2:
        action = id;
    }
}

void EditArchivalInformation :: run ()
{
    do {
        tdialogbox::run();
    } while ( action == 0);
}


void editArchivalInformation()
{
    EditArchivalInformation eai ( actmap );
    eai.init();
    eai.run();
    eai.done();
}

void resourceComparison ( )
{
    ASCString s;
    for ( int i = 0; i < 9; i++ ) {
        s += ASCString("player ") + strrr ( i ) + " : ";
        Resources plus;
        Resources have;
        for ( Player::BuildingList::iterator b = actmap->player[i].buildingList.begin(); b != actmap->player[i].buildingList.end(); ++b ) {
            if ( actmap->_resourcemode == 0 )
                plus += (*b)->plus;
            else
                plus += (*b)->bi_resourceplus;

            have += (*b)->actstorage;
        }
        for ( int r = 0; r < resourceTypeNum; r++ ) {
            s += ASCString(resourceNames[r]) + " " + strrr ( have.resource(r)) + " +";
            s += strrr(plus.resource(r));
            s += "; ";
        }

        s += "\n\n";
    }

    Resources res;
    for ( int x = 0; x < actmap->xsize; x++)
        for ( int y = 0; y < actmap->ysize; y++ ) {
            res.material += actmap->getField(x,y)->material;
            res.fuel     += actmap->getField(x,y)->fuel;
        }

    s += "Mineral Resources: ";
    s += strrr(res.material );
    s += " Material, ";
    s += strrr(res.material );
    s += " Fuel";

    displaymessage ( s, 3 );
}




void readClipboard()
{
    ASCString filename = selectFile( ASCString(clipboardFileExtension) + ";" + oldClipboardFileExtension, true);
    if ( !filename.empty() ) {
        try {
            if ( patimat( oldClipboardFileExtension, filename )) {
                tnfilestream stream ( filename, tnstream::reading );
                ClipBoard::Instance().read( stream );
            } else {
                tnfilestream stream ( filename, tnstream::reading );
                TextFormatParser parser(&stream);
                PropertyReadingContainer pc ( "ClipboardInfo", parser.run() );
                ClipBoard::Instance().readProperties( pc );
            }
        } catch ( ASCmsgException me ) {
            errorMessage( "Error reading clipboard file:\n" + me.getMessage());
        } catch ( ... ) {
            errorMessage( "Error reading clipboard file" );
        }

    }
}

void saveClipboard()
{
    ASCString filename = selectFile( clipboardFileExtension, false);
    if ( !filename.empty() ) {
        tn_file_buf_stream txtstream ( filename, tnstream::writing );
        PropertyWritingContainer pc ( "ClipboardInfo", txtstream );
        ClipBoard::Instance().writeProperties( pc );
    }
}

void setweatherall ( int weather  )
{
    for (int y=0; y < actmap->ysize; y++)
        for (int x=0; x < actmap->xsize; x++)
            actmap->getField(x,y)->setWeather( weather );
}


ASCString printTech( int id )
{
    ASCString s;
    s.format ( "%7d ", id );
    const Technology* t = technologyRepository.getObject_byID ( id );
    if ( t )
        s += t->name;

    return s;
}

void editResearch()
{
    vector<ASCString> buttons;
    buttons.push_back ( "~A~dd" );
    buttons.push_back ( "~R~emove" );
    buttons.push_back ( "~C~lose" );

    vector<ASCString> buttons2;
    buttons2.push_back ( "~A~dd" );
    buttons2.push_back ( "~C~ancel" );

    vector<ASCString> buttonsP;
    buttonsP.push_back ( "~E~dit" );
    buttonsP.push_back ( "~C~lose" );

    pair<int,int> playerRes;
    do {
        vector<ASCString> player;
        for ( int i = 0; i < 8; ++i )
            player.push_back ( ASCString ( strrr(i)) + " " + actmap->player[i].getName());

        playerRes = chooseString ( "Choose Player", player, buttonsP );
        if ( playerRes.first == 0 && playerRes.second >= 0 ) {
            int player = playerRes.second;

            pair<int,int> res;
            do {
                vector<int>& devTech = actmap->player[player].research.developedTechnologies;
                vector<ASCString> techs;
                map<ASCString,int> techIds;
                for ( int i = 0; i < devTech.size(); ++i ) {
                    int id = devTech[i];
                    ASCString s = printTech(id );
                    techs.push_back ( s );
                    techIds[s] = id;
                }
                sort (techs.begin(), techs.end() );
                res = chooseString ( "Available Technologies", techs, buttons );
                if ( res.first == 0 ) {
                    vector<ASCString> techs;
                    vector<int> techIds;
                    for ( int i = 0; i < technologyRepository.getNum(); ++i ) {
                        const Technology* t = technologyRepository.getObject_byPos(i);
                        if ( find ( devTech.begin(), devTech.end(), t->id ) == devTech.end() )
                            if ( !ItemFiltrationSystem::isFiltered( ItemFiltrationSystem::Technology, t->id )) {
                                techs.push_back ( printTech ( t->id ));
                                techIds.push_back ( t->id );
                            }
                    }

                    if ( techIds.empty() ) {
                        infoMessage("No technologies found. Please review Item Filters");
                    } else {
                        // sort (techs.begin(), techs.end() );
                        pair<int,int> r = chooseString ( "Unresearched Technologies", techs, buttons2 );
                        if ( r.first == 0 && r.second >= 0 ) {
                            devTech.push_back ( techIds[r.second] );
                        }
                    }
                } else if ( res.first == 1 && res.second >= 0 ) {
                    vector<int>::iterator p = find ( devTech.begin(), devTech.end(), techIds[techs[res.second]]);
                    devTech.erase ( p );
                }
            } while ( res.first != 2 );
        }
    } while ( playerRes.first != 1 );
}

void editResearchPoints()
{
    vector<ASCString> buttonsP;
    buttonsP.push_back ( "~E~dit" );
    buttonsP.push_back ( "~C~lose" );

    pair<int,int> playerRes;
    do {
        vector<ASCString> player;
        for ( int i = 0; i < 8; ++i ) {
            ASCString s = strrr(i);
            player.push_back ( s + " " + actmap->player[i].getName() + " (" + strrr(actmap->player[i].research.progress) + " Points)" );
        }

        playerRes = chooseString ( "Choose Player", player, buttonsP );
        if ( playerRes.first == 0 && playerRes.second >= 0 )
            actmap->player[playerRes.second].research.progress = editInt ( "Points", actmap->player[playerRes.second].research.progress );

    } while ( playerRes.first != 1 );
}


void generateTechTree()
{
    ASCString filename = selectFile("*.dot", false);
    if ( !filename.empty() ) {

        map<ASCString,int> temptechs;
        for ( int i = 0; i < technologyRepository.getNum(); ++i ) {
            const Technology* t = technologyRepository.getObject_byPos(i);
            if ( !ItemFiltrationSystem::isFiltered( ItemFiltrationSystem::Technology, t->id )) {
                temptechs[t->name] = t->id;
            }
        }

        vector<ASCString> techs;
        vector<int> techIds;
        for ( map<ASCString,int>::iterator i = temptechs.begin(); i != temptechs.end(); ++i ) {
            techs.push_back ( printTech ( i->second ));
            techIds.push_back ( i->second );
        }



        /*
        for ( int i = 1; i < techIds.size();  )
           if ( technologyRepository.getObject_byID( techIds[i-1] )->name > technologyRepository.getObject_byID( techIds[i] )->name ) {
              int a = techIds[i];
              int b = techIds[i-1];
              techs[i] = printTech ( b );
              techs[i-1] = printTech ( a );
              techIds[i] = b;
              techIds[i-1] = a;
              if ( i > 1 )
                 --i;
           } else
              ++i;
           */


        vector<ASCString> buttons2;
        buttons2.push_back ( "~O~k" );
        buttons2.push_back ( "~C~ancel" );

        bool leavesUp = choice_dlg ( "tree direction", "leaves up", "root up" ) == 1;

        if ( leavesUp )  {

            pair<int,int> r = chooseString ( "Choose Base Technology", techs, buttons2 );
            if ( r.first == 0 ) {


                bool reduce = choice_dlg ( "generate sparse tree ?", "~y~es", "~n~o" ) == 1;

                tn_file_buf_stream f ( filename, tnstream::writing );

                f.writeString ( "digraph \"ASC Technology Tree\" { \nnode [color=gray]\n", false );

                for ( int i = 0; i < technologyRepository.getNum(); ++i ) {
                    const Technology* t  = technologyRepository.getObject_byPos(i);
                    if ( !ItemFiltrationSystem::isFiltered( ItemFiltrationSystem::Technology, t->id )) {
                        vector<int> stack;
                        if ( t->techDependency.findInheritanceLevel( techIds[r.second], stack, techs[r.second] ) > 0 )
                            t->techDependency.writeTreeOutput( t->name, f, reduce );
                    }

                }

                ASCString stn2 = technologyRepository.getObject_byID(techIds[r.second])->name;
                while ( stn2.find ( "\"" ) != ASCString::npos )
                    stn2.erase ( stn2.find ( "\"" ),1 );

                f.writeString ( "\"" + stn2 + "\" [shape=doublecircle] \n", false );
                f.writeString ( "}\n", false );
            }
        } else {
            pair<int,int> r = chooseString ( "Choose Tip Technology", techs, buttons2 );
            if ( r.first == 0 ) {

                tn_file_buf_stream f ( filename, tnstream::writing );

                f.writeString ( "digraph \"ASC Technology Tree\" { \nnode [color=gray]\n", false );

                const Technology* t  = technologyRepository.getObject_byID(techIds[r.second]);
                vector<int> history;
                vector<pair<int,int> > blockedPrintList;
                if ( t )
                    t->techDependency.writeInvertTreeOutput( t->name, f, history, blockedPrintList );

                ASCString stn2 = technologyRepository.getObject_byID(techIds[r.second])->name;
                while ( stn2.find ( "\"" ) != ASCString::npos )
                    stn2.erase ( stn2.find ( "\"" ),1 );

                f.writeString ( "\"" + stn2 + "\" [shape=doublecircle] \n", false );
                f.writeString ( "}\n", false );
            }
        }

    }
}



void editTechAdapter()
{
    vector<ASCString> buttons;
    buttons.push_back ( "~A~dd" );
    buttons.push_back ( "~R~emove" );
    buttons.push_back ( "~C~lose" );

    vector<ASCString> buttons2;
    buttons2.push_back ( "~A~dd" );
    buttons2.push_back ( "~C~ancel" );

    vector<ASCString> buttonsP;
    buttonsP.push_back ( "~E~dit" );
    buttonsP.push_back ( "~C~lose" );

    pair<int,int> playerRes;
    do {
        vector<ASCString> player;
        for ( int i = 0; i < 8; ++i )
            player.push_back ( ASCString ( strrr(i)) + " " + actmap->player[i].getName());

        playerRes = chooseString ( "Choose Player", player, buttonsP );
        if ( playerRes.first == 0 && playerRes.second >= 0) {
            int player = playerRes.second;
            editTechAdapter(actmap->player[player] );
        }
    } while ( playerRes.first != 1 );
}



MapField*        getactfield(void)
{
    return actmap->getField( actmap->getCursor() );;
}


class ItemLocator : public ASC_PG_Dialog {

    PG_LineEdit* idField;
    DropDownSelector* typeSelector;


    bool findUnit( ContainerBase* c, int id )
    {
        if ( !c )
            return false;

        for ( ContainerBase::Cargo::const_iterator i = c->getCargo().begin(); i != c->getCargo().end(); ++i ) {
            if ( (*i) ) {
                if ( (*i)->typ->id == id )
                    return true;
                if ( findUnit( *i, id ))
                    return true;
            }
        }
        return false;
    }

    bool ok()
    {
        int id = atoi ( idField->GetText().c_str() );
        if ( id != 0 ) {
            for ( int y = 0; y < actmap->ysize; ++y )
                for ( int x = 0; x < actmap->xsize; ++x ) {
                    MapField* fld = actmap->getField(x,y);
                    if ( fld ) {
                        bool found = false;
                        switch ( typeSelector->GetSelectedItemIndex () ) {
                        case 0:
                            if ( fld->typ->terraintype->id == id )
                                found = true;
                            break;
                        case 1:
                            if ( fld->vehicle && fld->vehicle->typ->id == id )
                                found = true;
                            if ( findUnit( fld->vehicle, id ))
                                found = true;
                            break;
                        case 2:
                            if ( fld->building && fld->building->typ->id == id )
                                found = true;
                            if ( findUnit( fld->building, id ))
                                found = true;
                            break;
                        case 3:
                            if ( fld->checkForObject( objectTypeRepository.getObject_byID( id )) )
                                found = true;
                            break;
                        }


                        if ( found ) {
                            getMainScreenWidget()->getMapDisplay()->cursor.goTo(MapCoordinate(x,y));
                            QuitModal();
                            return true;
                        }
                    }
                }

        }
        return false;
    }

    bool cancel()
    {
        QuitModal();
        return true;
    }

public:
    ItemLocator() : ASC_PG_Dialog(NULL, PG_Rect(-1,-1,400,200 ), "Item Locator")
    {
        AddStandardButton("OK")->sigClick.connect( sigc::hide( sigc::mem_fun(*this, &ItemLocator::ok )));
        AddStandardButton("cancel")->sigClick.connect( sigc::hide( sigc::mem_fun(*this, &ItemLocator::cancel )));

        typeSelector = new DropDownSelector(this, PG_Rect(20, 60, 150, 25 ));
        typeSelector->AddItem("Terrain");
        typeSelector->AddItem("Vehicle");
        typeSelector->AddItem("Building");
        typeSelector->AddItem("Object");

        new PG_Label ( this, PG_Rect(20, 100, 50, 25), "ID: " );
        idField = new PG_LineEdit( this, PG_Rect( 70, 100, 150, 25 ));
    }

};


void locateItemByID()
{
    ItemLocator il;
    il.Show();
    il.RunModal();
}


/********************************************************************/
/********************************************************************/
/****** mirrorMap and utitity methods                          ******/
/********************************************************************/
/********************************************************************/


void copyVehicleData( Vehicle* source, Vehicle* target, GameMap* targetMap, int* playerTranslation )
{

    target->height = source->height;
    target->tank = source->getTank();
    target->name = source->name;
    target->setExperience_offensive( source->getExperience_offensive() );
    target->setExperience_defensive ( source->getExperience_defensive() );
    target->damage = source->damage;


    if ( source->reactionfire.getStatus() == Vehicle::ReactionFire::off )
        target->reactionfire.disable();
    else
        target->reactionfire.enable();

    // for( int i=0; i<8; i++ ) target->aiparam[ i ] = source->aiparam[ i ];
    for ( int i=0; i<16; i++ ) target->ammo[ i ] = source->ammo[ i ];

    // containing units
    ContainerBase::Cargo sourceCargo = source->getCargo();
    for ( int i=0; i<sourceCargo.size(); i++ )
    {
        int playerID = playerTranslation[ sourceCargo[ i ]->getOwner() ];
        Vehicle* cargo = new Vehicle( sourceCargo[ i ]->typ, targetMap, playerID );
        copyVehicleData( sourceCargo[ i ], cargo, targetMap, playerTranslation );
        target->addToCargo( cargo );
    }

    target->deleteAllProductionLines();
    for ( ContainerBase::Production::const_iterator i = source->getProduction().begin(); i != source->getProduction().end(); ++i )
        target->addProductionLine( *i );

}


void copyBuildingData( Building* source, Building* target, GameMap* targetMap, int *playerTranslation, bool mirrorUnits )
{
    for ( int i=0; i<waffenanzahl; i++ )
        target->ammo[ i ] = source->ammo[ i ];
    target->name = source->name;
    target->netcontrol = source->netcontrol;
    target->visible = source->visible;
    /*
    for( int i=0; i<8; i++ )
       target->aiparam[ i ] = source->aiparam[ i ];
    */
    target->lastmineddist = source->lastmineddist;
    target->actstorage = source->actstorage;
    target->damage = source->damage;

    if ( mirrorUnits )
    {
        // containing units
        ContainerBase::Cargo sourceCargo = source->getCargo();
        for ( int i=0; i<sourceCargo.size(); i++ )
        {
            int playerID = playerTranslation[ sourceCargo[ i ]->getOwner() ];
            Vehicle* cargo = new Vehicle( sourceCargo[ i ]->typ, targetMap, playerID );
            copyVehicleData( sourceCargo[ i ], cargo, targetMap, playerTranslation );
            target->addToCargo( cargo );
        }
    }

    target->deleteAllProductionLines();
    for ( ContainerBase::Production::const_iterator i = source->getProduction().begin(); i != source->getProduction().end(); ++i )
        target->addProductionLine( *i );

}

void copyFieldStep1( MapField* sourceField, MapField* targetField, bool mirrorTerrain, bool mirrorResources, bool mirrorWeather )
{
    targetField->deleteeverything();
    while ( targetField->objects.size() > 0 )
        targetField->removeObject( targetField->objects[ 0 ].typ, true );

    if ( mirrorTerrain )
    {
        targetField->typ = sourceField->typ;
        targetField->bdt = sourceField->bdt;
    }

    if ( mirrorResources )
    {
        targetField->fuel = sourceField->fuel;
        targetField->material = sourceField->material;
    }
    if ( mirrorWeather ) targetField->setWeather( sourceField->getWeather() );
}

void copyFieldStep2( MapField* sourceField, MapField* targetField, GameMap* targetMap, int *directionTranslation, int *playerTranslation, bool mirrorObjects, bool mirrorBuildings, bool mirrorUnits, bool mirrorMines )
{
    if ( mirrorObjects )
    {
        for ( int i=0; i<sourceField->objects.size(); i++ )
            targetField->addobject( sourceField->objects[ i ].typ, -1, true );
    }

    if ( mirrorBuildings )
    {
        if ( sourceField->building != NULL && sourceField->building->getEntryField() == sourceField )
        {
            int playerID = playerTranslation[ sourceField->building->getOwner() ];
            Building *newBuilding = new Building( targetMap, MapCoordinate( targetField->getx(), targetField->gety() ), sourceField->building->typ, playerID );
            copyBuildingData( sourceField->building, newBuilding, targetMap, playerTranslation, mirrorUnits );
        }
    }

    if ( mirrorUnits )
    {
        if ( sourceField->vehicle != NULL )
        {
            int playerID = playerTranslation[ sourceField->vehicle->getOwner() ];
            targetField->vehicle = new Vehicle( sourceField->vehicle->typ, targetMap, playerID );
            copyVehicleData( sourceField->vehicle, targetField->vehicle, targetMap, playerTranslation );
            targetField->vehicle->setnewposition( targetField->getx(), targetField->gety() );
            targetField->vehicle->direction = directionTranslation[ int(sourceField->vehicle->direction) ];
        }
    }

    if ( mirrorMines )
    {
        for ( int i=0; i<sourceField->mines.size(); i++ )
        {
            Mine sourceMine = sourceField->getMine( i );
            targetField->putmine( playerTranslation[ sourceMine.player ], sourceMine.type, sourceMine.strength );
        }
    }
}

class MirrorMap : public tdialogbox
{
    int mirrorUnits;
    int mirrorBuildings;
    int mirrorObjects;
    int mirrorMines;
    int mirrorResources;
    int mirrorWeather;

    int playerTranslation[ 9 ];

    bool doneMirrorMap;

public:
    void init( void );
    void buttonpressed ( int id );
    void run ( void );
    Uint8 checkvalue( int id, void* p );

protected:


    void mirrorX();
    void mirrorY();
};

Uint8 MirrorMap::checkvalue( int id, void* p )
{
    return 1;
}

void MirrorMap::buttonpressed( int id )
{
    tdialogbox::buttonpressed( id );

    if ( id == 1 )
        doneMirrorMap = true;

    if ( id == 2 )
    {
        mirrorX();
        doneMirrorMap = true;
    }

    if ( id == 3 )
    {
        mirrorY();
        doneMirrorMap = true;
    }

}

void MirrorMap::init( void )
{
    doneMirrorMap = false;

    mirrorUnits = true;
    mirrorBuildings = true;
    mirrorObjects = true;
    mirrorMines = true;
    mirrorResources = true;
    mirrorWeather = true;
    for ( int i=0; i<8; i++ )
        playerTranslation[ i ] = 7-i;
    playerTranslation[ 8 ] = 8;

    tdialogbox::init();
    title = "Mirror Map";

    xsize = 400;
    ysize = 450;

    x1 = -1;
    y1 = -1;

    addbutton( "~C~ancel", 10, ysize - 35, xsize / 3 - 5, ysize - 10, 0, 1, 1, true );
    addkey ( 1, ct_esc );

    addbutton( "Mirror ~X~", xsize / 3 + 5, ysize - 35, xsize / 3 * 2 - 5, ysize - 10, 0, 1, 2, true );
    addkey ( 2, ct_x );

    addbutton ( "Mirror ~Y~", xsize / 3 * 2 + 5, ysize - 35, xsize - 10, ysize - 10, 0, 1, 3, true );
    addkey ( 3, ct_y );

    addbutton( "~O~bjects", xsize - 100, 50, xsize - 10, 65, 3, 0, 4, true );
    addeingabe( 4, &mirrorObjects, 0, lightgray );
    addkey ( 4, ct_o );

    addbutton( "~U~nits", xsize - 100, 70, xsize - 10, 85, 3, 0, 5, true );
    addeingabe( 5, &mirrorUnits, 0, lightgray );
    addkey ( 5, ct_u );

    addbutton( "~B~uildings", xsize - 100, 90, xsize - 10, 105, 3, 0, 6, true );
    addeingabe( 6, &mirrorBuildings, 0, lightgray );
    addkey ( 6, ct_b );

    addbutton( "~M~ines", xsize - 100, 110, xsize - 10, 125, 3, 0, 7, true );
    addeingabe( 7, &mirrorMines, 0, lightgray );
    addkey ( 7, ct_m );

    addbutton( "~R~esources", xsize - 100, 130, xsize - 10, 145, 3, 0, 8, true );
    addeingabe( 8, &mirrorResources, 0, lightgray );
    addkey ( 8, ct_r );

    addbutton( "~W~eather", xsize - 100, 150, xsize - 10, 165, 3, 0, 9, true );
    addeingabe( 9, &mirrorWeather, 0, lightgray );
    addkey ( 9, ct_w );

    addbutton( "Player Conversions:", 30, 50, xsize - 120, 70, 0, 0, 10, true );
    addbutton( "Player 0:", 30, 80, 130, 95, 2, 0, 12, true );
    addeingabe( 12, &playerTranslation[ 0 ], 0, 8 );
    addbutton( "Player 1:", 30, 110, 130, 125, 2, 0, 13, true );
    addeingabe( 13, &playerTranslation[ 1 ], 0, 8 );
    addbutton( "Player 2:", 30, 140, 130, 155, 2, 0, 14, true );
    addeingabe( 14, &playerTranslation[ 2 ], 0, 8 );
    addbutton( "Player 3:", 30, 170, 130, 185, 2, 0, 15, true );
    addeingabe( 15, &playerTranslation[ 3 ], 0, 8 );
    addbutton( "Player 4:", 30, 200, 130, 215, 2, 0, 16, true );
    addeingabe( 16, &playerTranslation[ 4 ], 0, 8 );
    addbutton( "Player 5:", 150, 80, xsize - 120, 95, 2, 0, 17, true );
    addeingabe( 17, &playerTranslation[ 5 ], 0, 8 );
    addbutton( "Player 6:", 150, 110, xsize - 120, 125, 2, 0, 18, true );
    addeingabe( 18, &playerTranslation[ 6 ], 0, 8 );
    addbutton( "Player 7:", 150, 140, xsize - 120, 155, 2, 0, 19, true );
    addeingabe( 19, &playerTranslation[ 7 ], 0, 8 );
    addbutton( "Player 8:", 150, 170, xsize - 120, 185, 2, 0, 20, true );
    addeingabe( 20, &playerTranslation[ 8 ], 0, 8 );

    buildgraphics();

    activefontsettings.font = schriften.smallarial;
    activefontsettings.justify = lefttext;
    activefontsettings.length = 0;
    activefontsettings.background = 255;

    showtext2( "Warnings:",   x1 + 25, y1 + 220 );
    showtext2( "right&lower border might have to be manually fixed", x1 + 25, y1 + 260 );
    showtext2( "mountains, battle isle graphics coasts and similar", x1 + 25, y1 + 280 );
    showtext2( "terrain needs manual adaption", x1 + 25, y1 + 300 );
    showtext2( "building directions might be weird, resulting in:", x1 + 25, y1 + 330 );
    showtext2( "- pipeline net might be broken due to building directions", x1 + 25, y1 + 350 );
    showtext2( "- buildings might be missing due to terrain", x1 + 25, y1 + 370 );
    showtext2( "- units might be missing due to building locations", x1 + 25, y1 + 390 );

}


void MirrorMap::run ( void )
{
    mousevisible ( true );
    do {
        tdialogbox::run();
    } while ( !doneMirrorMap ); /* enddo */
}


void MirrorMap::mirrorX()
{
    if ( actmap->xsize % 2 == 1 ) actmap->resize ( 0, 0, 0, 1 );
    int maxX = actmap->xsize/2;
    int xOffset = 2;
    int directionTranslation[ 6 ] = { 0, 5, 4, 3, 2, 1 };

    // run 1: object removal and terrain copy
    for ( int x=0; x<maxX; x++ )
    {
        for ( int y=0; y<actmap->ysize; y++ )
        {
            int targetX = actmap->xsize - x - xOffset;
            if ( y%2 == 0 ) targetX++;
            if ( targetX >= actmap->xsize || targetX == x ) continue;

            MapField *targetField = actmap->getField( targetX, y );
            MapField *sourceField = actmap->getField( x, y );

            copyFieldStep1( sourceField, targetField, true, mirrorResources, mirrorWeather );
        }
    }

    // run 2: object copy
    for ( int x=0; x<maxX; x++ )
    {
        for ( int y=0; y<actmap->ysize; y++ )
        {
            int targetX = actmap->xsize - x - xOffset;
            if ( y%2 == 0 ) targetX++;
            if ( targetX >= actmap->xsize || targetX == x ) continue;

            MapField *targetField = actmap->getField( targetX, y );
            MapField *sourceField = actmap->getField( x, y );

            copyFieldStep2( sourceField, targetField, actmap, directionTranslation, playerTranslation, mirrorObjects, mirrorBuildings, mirrorUnits, mirrorMines );
        }
    }
    doneMirrorMap = true;
}

void MirrorMap::mirrorY()
{
    int yOffset = 2;
    int maxY = actmap->ysize/2;
    int directionTranslation[ 6 ] = { 3, 2, 1, 0, 5, 4 };

    // run 1: object removal and terrain copy
    for ( int x=0; x<actmap->xsize; x++ )
    {
        for ( int y=0; y<maxY; y++ )
        {
            int targetY = actmap->ysize - y - yOffset;

            MapField *targetField = actmap->getField( x, targetY );
            MapField *sourceField = actmap->getField( x, y );

            copyFieldStep1( sourceField, targetField, true, mirrorResources, mirrorWeather );
        }
    }

    // run 2: object copy
    for ( int x=0; x<actmap->xsize; x++ )
    {
        for ( int y=0; y<maxY; y++ )
        {
            int targetY = actmap->ysize - y - yOffset;

            MapField *targetField = actmap->getField( x, targetY );
            MapField *sourceField = actmap->getField( x, y );

            copyFieldStep2( sourceField, targetField, actmap, directionTranslation, playerTranslation, mirrorObjects, mirrorBuildings, mirrorUnits, mirrorMines );
        }
    }
    doneMirrorMap = true;
}

void mirrorMap()
{
    MirrorMap mm;
    mm.init();
    mm.run();
    mm.done();
    mapChanged( actmap );
    displaymap();
}

/********************************************************************/
/********************************************************************/
/****** mirrorMap and utitity methods END                      ******/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/********************************************************************/
/****** copy area and utitity methods START                    ******/
/********************************************************************/
/********************************************************************/

class CopyMap : public FieldAddressing, public ASC_PG_Dialog
{
private:
    GameMap *map;
    bool* fieldCopied;

    int mapStartX, mapStartY, mapEndX, mapEndY, sizeX, sizeY;
    int copyStep;

    int *directionTranslation;
    int *playerTranslation;

    PG_CheckButton* mirrorTerrain;
    PG_CheckButton* mirrorResources;
    PG_CheckButton* mirrorWeather;
    PG_CheckButton* mirrorObjects;
    PG_CheckButton* mirrorBuildings;
    PG_CheckButton* mirrorUnits;
    PG_CheckButton* mirrorMines;

    PG_CheckButton* mirrorX;
    PG_CheckButton* mirrorY;

    PG_CheckButton* autoIncreaseMapSize;

    PG_LineEdit* playerTranslation0;
    PG_LineEdit* playerTranslation1;
    PG_LineEdit* playerTranslation2;
    PG_LineEdit* playerTranslation3;
    PG_LineEdit* playerTranslation4;
    PG_LineEdit* playerTranslation5;
    PG_LineEdit* playerTranslation6;
    PG_LineEdit* playerTranslation7;
    PG_LineEdit* playerTranslation8;


public:
    CopyMap();
    ~CopyMap();
    bool paste();
    void copy();
    void selectArea();

protected:
    virtual void fieldOperator( const MapCoordinate& point );
};

CopyMap::CopyMap() : FieldAddressing( actmap ), ASC_PG_Dialog( NULL, PG_Rect( 30, 30, 550, 400 ), "Paste Options" )
{
    addressingMode = poly;
    mapStartX = -1;
    mapStartY = -1;
    mapEndX = -1;
    mapEndY = -1;
    map = NULL;
    fieldCopied = NULL;

    directionTranslation = new int[ 6 ];
    for ( int i=0; i<6; i++ ) directionTranslation[ i ] = i;
    playerTranslation = new int[ 9 ];
    for ( int i=0; i<9; i++ ) playerTranslation[ i ] = i;

    int dialogLine = 0;
    int lineHeight = 20;
    int lineSpacing = 10;

    int startX = 20;
    int startY = 20;
    int startX2 = 200;
    int startX3 = 250;
    int startX4 = 400;
    int xSpacer = 20;

    PG_Label* label = new PG_Label ( this, PG_Rect( startX, startY + dialogLine*(lineHeight+lineSpacing) , startX2 - xSpacer - startX, lineHeight ) );
    label->SetAlignment( PG_Label::LEFT );
    label->SetText( "Terrain" );
    mirrorTerrain = new PG_CheckButton( this, PG_Rect( startX2, startY + dialogLine*(lineHeight+lineSpacing) , startX3 - xSpacer - startX2, lineHeight ) );
    mirrorTerrain->SetPressed();
    dialogLine++;

    label = new PG_Label ( this, PG_Rect( startX, startY + dialogLine*(lineHeight+lineSpacing) , startX2 - xSpacer - startX, lineHeight ) );
    label->SetAlignment( PG_Label::LEFT );
    label->SetText( "Resources" );
    mirrorResources = new PG_CheckButton( this, PG_Rect( startX2, startY + dialogLine*(lineHeight+lineSpacing) , startX3 - xSpacer - startX2, lineHeight ) );
    mirrorResources->SetPressed();
    dialogLine++;

    label = new PG_Label ( this, PG_Rect( startX, startY + dialogLine*(lineHeight+lineSpacing) , startX2 - xSpacer - startX, lineHeight ) );
    label->SetAlignment( PG_Label::LEFT );
    label->SetText( "Weather" );
    mirrorWeather = new PG_CheckButton( this, PG_Rect( startX2, startY + dialogLine*(lineHeight+lineSpacing) , startX3 - xSpacer - startX2, lineHeight ) );
    mirrorWeather->SetPressed();
    dialogLine++;

    label = new PG_Label ( this, PG_Rect( startX, startY + dialogLine*(lineHeight+lineSpacing) , startX2 - xSpacer - startX, lineHeight ) );
    label->SetAlignment( PG_Label::LEFT );
    label->SetText( "Objects" );
    mirrorObjects = new PG_CheckButton( this, PG_Rect( startX2, startY + dialogLine*(lineHeight+lineSpacing) , startX3 - xSpacer - startX2, lineHeight ) );
    mirrorObjects->SetPressed();
    dialogLine++;

    label = new PG_Label ( this, PG_Rect( startX, startY + dialogLine*(lineHeight+lineSpacing) , startX2 - xSpacer - startX, lineHeight ) );
    label->SetAlignment( PG_Label::LEFT );
    label->SetText( "Buildings" );
    mirrorBuildings = new PG_CheckButton( this, PG_Rect( startX2, startY + dialogLine*(lineHeight+lineSpacing) , startX3 - xSpacer - startX2, lineHeight ) );
    mirrorBuildings->SetPressed();
    dialogLine++;

    label = new PG_Label ( this, PG_Rect( startX, startY + dialogLine*(lineHeight+lineSpacing) , startX2 - xSpacer - startX, lineHeight ) );
    label->SetAlignment( PG_Label::LEFT );
    label->SetText( "Units" );
    mirrorUnits = new PG_CheckButton( this, PG_Rect( startX2, startY + dialogLine*(lineHeight+lineSpacing) , startX3 - xSpacer - startX2, lineHeight ) );
    mirrorUnits->SetPressed();
    dialogLine++;

    label = new PG_Label ( this, PG_Rect( startX, startY + dialogLine*(lineHeight+lineSpacing) , startX2 - xSpacer - startX, lineHeight ) );
    label->SetAlignment( PG_Label::LEFT );
    label->SetText( "Mines" );
    mirrorMines = new PG_CheckButton( this, PG_Rect( startX2, startY + dialogLine*(lineHeight+lineSpacing) , startX3 - xSpacer - startX2, lineHeight ) );
    mirrorMines->SetPressed();
    dialogLine++;

    label = new PG_Label ( this, PG_Rect( startX, startY + dialogLine*(lineHeight+lineSpacing) , startX2 - xSpacer - startX, lineHeight ) );
    label->SetAlignment( PG_Label::LEFT );
    label->SetText( "X" );
    mirrorX = new PG_CheckButton( this, PG_Rect( startX2, startY + dialogLine*(lineHeight+lineSpacing) , startX3 - xSpacer - startX2, lineHeight ) );
    dialogLine++;

    label = new PG_Label ( this, PG_Rect( startX, startY + dialogLine*(lineHeight+lineSpacing) , startX2 - xSpacer - startX, lineHeight ) );
    label->SetAlignment( PG_Label::LEFT );
    label->SetText( "Y" );
    mirrorY = new PG_CheckButton( this, PG_Rect( startX2, startY + dialogLine*(lineHeight+lineSpacing) ,startX3 - xSpacer - startX2, lineHeight ) );
    dialogLine++;

    label = new PG_Label ( this, PG_Rect( startX, startY + dialogLine*(lineHeight+lineSpacing) , startX2 - xSpacer - startX, lineHeight ) );
    label->SetAlignment( PG_Label::LEFT );
    label->SetText( "autoIncreaseMapSize" );
    autoIncreaseMapSize = new PG_CheckButton( this, PG_Rect( startX2, startY + dialogLine*(lineHeight+lineSpacing) , startX3 - xSpacer - startX2, lineHeight ) );
    autoIncreaseMapSize->SetPressed();
    dialogLine++;


    PG_Button* ok = new PG_Button( this, PG_Rect( startX, startY + dialogLine*(lineHeight+lineSpacing) , startX2 - xSpacer - startX, lineHeight ), "Paste" );
    ok->sigClick.connect( sigc::hide( sigc::mem_fun( *this, &CopyMap::paste )));

    dialogLine = 0;

    label = new PG_Label ( this, PG_Rect( startX3, startY + dialogLine*(lineHeight+lineSpacing) , startX4 - xSpacer - startX, lineHeight ) );
    label->SetAlignment( PG_Label::LEFT );
    label->SetText( "playerTranslation0" );
    playerTranslation0 = new PG_LineEdit( this, PG_Rect( startX4, startY + dialogLine*(lineHeight+lineSpacing) , this->Width() - xSpacer - startX4, lineHeight ) );
    playerTranslation0->SetText( "0" );
    dialogLine++;

    label = new PG_Label ( this, PG_Rect( startX3, startY + dialogLine*(lineHeight+lineSpacing) , startX4 - xSpacer - startX, lineHeight ) );
    label->SetAlignment( PG_Label::LEFT );
    label->SetText( "playerTranslation1" );
    playerTranslation1 = new PG_LineEdit( this, PG_Rect( startX4, startY + dialogLine*(lineHeight+lineSpacing) , this->Width() - xSpacer - startX4, lineHeight ) );
    playerTranslation1->SetText( "1" );
    dialogLine++;

    label = new PG_Label ( this, PG_Rect( startX3, startY + dialogLine*(lineHeight+lineSpacing) , startX4 - xSpacer - startX, lineHeight ) );
    label->SetAlignment( PG_Label::LEFT );
    label->SetText( "playerTranslation2" );
    playerTranslation2 = new PG_LineEdit( this, PG_Rect( startX4, startY + dialogLine*(lineHeight+lineSpacing) , this->Width() - xSpacer - startX4, lineHeight ) );
    playerTranslation2->SetText( "2" );
    dialogLine++;

    label = new PG_Label ( this, PG_Rect( startX3, startY + dialogLine*(lineHeight+lineSpacing) , startX4 - xSpacer - startX, lineHeight ) );
    label->SetAlignment( PG_Label::LEFT );
    label->SetText( "playerTranslation3" );
    playerTranslation3 = new PG_LineEdit( this, PG_Rect( startX4, startY + dialogLine*(lineHeight+lineSpacing) , this->Width() - xSpacer - startX4, lineHeight ) );
    playerTranslation3->SetText( "3" );
    dialogLine++;

    label = new PG_Label ( this, PG_Rect( startX3, startY + dialogLine*(lineHeight+lineSpacing) , startX4 - xSpacer - startX, lineHeight ) );
    label->SetAlignment( PG_Label::LEFT );
    label->SetText( "playerTranslation4" );
    playerTranslation4 = new PG_LineEdit( this, PG_Rect( startX4, startY + dialogLine*(lineHeight+lineSpacing) , this->Width() - xSpacer - startX4, lineHeight ) );
    playerTranslation4->SetText( "4" );
    dialogLine++;

    label = new PG_Label ( this, PG_Rect( startX3, startY + dialogLine*(lineHeight+lineSpacing) , startX4 - xSpacer - startX, lineHeight ) );
    label->SetAlignment( PG_Label::LEFT );
    label->SetText( "playerTranslation5" );
    playerTranslation5 = new PG_LineEdit( this, PG_Rect( startX4, startY + dialogLine*(lineHeight+lineSpacing) , this->Width() - xSpacer - startX4, lineHeight ) );
    playerTranslation5->SetText( "5" );
    dialogLine++;

    label = new PG_Label ( this, PG_Rect( startX3, startY + dialogLine*(lineHeight+lineSpacing) , startX4 - xSpacer - startX, lineHeight ) );
    label->SetAlignment( PG_Label::LEFT );
    label->SetText( "playerTranslation6" );
    playerTranslation6 = new PG_LineEdit( this, PG_Rect( startX4, startY + dialogLine*(lineHeight+lineSpacing) , this->Width() - xSpacer - startX4, lineHeight ) );
    playerTranslation6->SetText( "6" );
    dialogLine++;

    label = new PG_Label ( this, PG_Rect( startX3, startY + dialogLine*(lineHeight+lineSpacing) , startX4 - xSpacer - startX, lineHeight ) );
    label->SetAlignment( PG_Label::LEFT );
    label->SetText( "playerTranslation7" );
    playerTranslation7 = new PG_LineEdit( this, PG_Rect( startX4, startY + dialogLine*(lineHeight+lineSpacing) , this->Width() - xSpacer - startX4, lineHeight ) );
    playerTranslation7->SetText( "7" );
    dialogLine++;

    label = new PG_Label ( this, PG_Rect( startX3, startY + dialogLine*(lineHeight+lineSpacing) , startX4 - xSpacer - startX, lineHeight ) );
    label->SetAlignment( PG_Label::LEFT );
    label->SetText( "playerTranslation8" );
    playerTranslation8 = new PG_LineEdit( this, PG_Rect( startX4, startY + dialogLine*(lineHeight+lineSpacing) , this->Width() - xSpacer - startX4, lineHeight ) );
    playerTranslation8->SetText( "8" );
    dialogLine++;

}

CopyMap::~CopyMap()
{
    if ( map != NULL )
    {
        delete map;
        delete[] fieldCopied;
    }
    delete[] directionTranslation;
    delete[] playerTranslation;
}

void CopyMap::selectArea()
{
    polygons.clear();

    Poly_gon area;
    editpolygon( area );
    setPolygon( area );
}

void CopyMap::copy()
{
    if ( map != NULL )
    {
        delete map;
        delete[] fieldCopied;
    }

    mapStartX = -1;
    mapStartY = -1;
    mapEndX = -1;
    mapEndY = -1;
    copyStep = 0;
    operate();

    sizeX = mapEndX-mapStartX + 1;
    sizeY = mapEndY-mapStartY + 1;

    map = new GameMap;
    map->allocateFields( sizeX, sizeY, terrainTypeRepository.getObject_byID(30)->weather[0] );

    int* oldDirectoyTranslation = directionTranslation;
    int* oldPlayerTranslation = playerTranslation;

    directionTranslation = new int[ 6 ];
    for ( int i=0; i<6; i++ ) directionTranslation[ i ] = i;
    playerTranslation = new int[ 9 ];
    for ( int i=0; i<9; i++ ) playerTranslation[ i ] = i;

    fieldCopied = new bool[ sizeY * sizeX ];
    for ( int i=0; i<sizeY * sizeX; i++ )
        fieldCopied[ i ] = false;

    copyStep = 1;
    operate();
    copyStep = 2;
    operate();

    delete[] directionTranslation;
    delete[] playerTranslation;
    directionTranslation = oldDirectoyTranslation;
    playerTranslation = oldPlayerTranslation;
}

bool CopyMap::paste()
{
    QuitModal();
    Hide();
    if ( map == NULL ) return false;

    MapField *field = getactfield();
    if ( field == NULL ) return false;

    int pasteStartX = field->getx();
    int pasteStartY = field->gety();

    if ( autoIncreaseMapSize->GetPressed() )
    {
        int targetSizeX = pasteStartX + map->xsize;
        int targetSizeY = pasteStartY + map->ysize;

        if ( targetSizeY%2 == 1 )
            targetSizeY++;

        if ( targetSizeX > actmap->xsize )
        {
            actmap->resize( 0, 0, 0, targetSizeX - actmap->xsize );
        }

        if ( targetSizeY > actmap->ysize )
        {
            actmap->resize( 0, targetSizeY - actmap->ysize, 0, 0 );
        }
    }

    playerTranslation[ 0 ] = atoi( playerTranslation0->GetText() );
    playerTranslation[ 1 ] = atoi( playerTranslation1->GetText() );
    playerTranslation[ 2 ] = atoi( playerTranslation2->GetText() );
    playerTranslation[ 3 ] = atoi( playerTranslation3->GetText() );
    playerTranslation[ 4 ] = atoi( playerTranslation4->GetText() );
    playerTranslation[ 5 ] = atoi( playerTranslation5->GetText() );
    playerTranslation[ 6 ] = atoi( playerTranslation6->GetText() );
    playerTranslation[ 7 ] = atoi( playerTranslation7->GetText() );
    playerTranslation[ 8 ] = atoi( playerTranslation8->GetText() );

    int* oldDirectoyTranslation = directionTranslation;

    directionTranslation = new int[ 6 ];
    for ( int i=0; i<6; i++ ) directionTranslation[ i ] = i;

    if ( mirrorY->GetPressed() )
    {
        int directionTranslationNew[ 6 ] =
        {
            directionTranslation[ 3 ],
            directionTranslation[ 2 ],
            directionTranslation[ 1 ],
            directionTranslation[ 0 ],
            directionTranslation[ 5 ],
            directionTranslation[ 4 ]
        };

        for ( int i=0; i<6; i++ ) directionTranslation[ i ] = directionTranslationNew[ i ];
    }

    if ( mirrorX->GetPressed() )
    {
        int directionTranslationNew[ 6 ] =
        {
            directionTranslation[ 0 ],
            directionTranslation[ 5 ],
            directionTranslation[ 4 ],
            directionTranslation[ 3 ],
            directionTranslation[ 2 ],
            directionTranslation[ 1 ]
        };

        for ( int i=0; i<6; i++ ) directionTranslation[ i ] = directionTranslationNew[ i ];
    }




    for ( int x=0; x<map->xsize; x++ )
    {
        for ( int y=0; y<map->ysize; y++ )
        {
            if ( fieldCopied[ x + y * sizeX ] )
            {
                int fieldX = pasteStartX + x;
                int fieldY = pasteStartY + y;

                if ( pasteStartY%2 == 1 && fieldY%2 == 0 )
                    fieldX++;

                if ( mirrorX->GetPressed() )
                {
                    fieldX = pasteStartX + sizeX - x - 1;

                    if ( pasteStartY%2 == 0 && fieldY%2 == 1 )
                        fieldX--;
                }

                if ( mirrorY->GetPressed() )
                {
                    fieldY = pasteStartY + sizeY - y - 1;
                }

                if ( fieldX >= actmap->xsize ) continue;
                if ( fieldY >= actmap->ysize ) continue;

                MapField* target = actmap->getField( fieldX, fieldY );
                MapField* source = map->getField( x, y );

                copyFieldStep1( source, target, mirrorTerrain->GetPressed(), mirrorResources->GetPressed(), mirrorWeather->GetPressed() );
            }
        }
    }

    for ( int x=0; x<map->xsize; x++ )
    {
        for ( int y=0; y<map->ysize; y++ )
        {
            if ( fieldCopied[ x + y * sizeX ] )
            {
                int fieldX = pasteStartX + x;
                int fieldY = pasteStartY + y;

                if ( pasteStartY%2 == 1 && fieldY%2 == 0 )
                    fieldX++;

                if ( mirrorX->GetPressed() )
                {
                    fieldX = pasteStartX + sizeX - x - 1;

                    if ( pasteStartY%2 == 0 && fieldY%2 == 1 )
                        fieldX--;
                }

                if ( mirrorY->GetPressed() )
                {
                    fieldY = pasteStartY + sizeY - y - 1;
                }

                if ( fieldX >= actmap->xsize ) continue;
                if ( fieldY >= actmap->ysize ) continue;

                MapField* target = actmap->getField( fieldX, fieldY );
                MapField* source = map->getField( x, y );

                copyFieldStep2( source, target, actmap, directionTranslation, playerTranslation, mirrorObjects->GetPressed(), mirrorBuildings->GetPressed(), mirrorUnits->GetPressed(), mirrorMines->GetPressed() );
            }
        }
    }

    delete[] directionTranslation;
    directionTranslation = oldDirectoyTranslation;

    return true;

}

void CopyMap::fieldOperator( const MapCoordinate& point )
{
    int mapX = point.x - mapStartX;
    int mapY = point.y - mapStartY;

    if ( mapStartY%2 == 1 && mapY%2 == 1 )
        mapX--;

    if ( copyStep == 0 )
    {
        if ( mapStartX == -1 || point.x < mapStartX )
            mapStartX = point.x;

        if ( mapStartY == -1 || point.y < mapStartY )
            mapStartY = point.y;

        if ( mapEndX == -1 || point.x > mapEndX )
            mapEndX = point.x;

        if ( mapEndY == -1 || point.y > mapEndY )
            mapEndY = point.y;
    } else if ( copyStep == 1 )
    {
        fieldCopied[ mapX + mapY * sizeX ] = true;

        MapField* source = actmap->getField( point );
        MapField* target = map->getField( mapX, mapY );
        if (!source || !target)
            return;


        copyFieldStep1( source, target, true, true, true );

    } else if ( copyStep == 2 )
    {
        MapField* source = actmap->getField( point );
        MapField* target = map->getField( mapX, mapY );
        if (!source || !target)
            return;

        copyFieldStep2( source, target, map, directionTranslation, playerTranslation, true, true, true, true );

    }
}

CopyMap *copyMap = NULL;

void copyArea()
{
    if ( copyMap == NULL )
    {
        copyMap = new CopyMap();
    }
    copyMap -> selectArea();
    copyMap -> copy();
}

void pasteArea()
{
    if ( copyMap != NULL )
    {
        copyMap -> Show();
        copyMap -> RunModal();
        mapChanged( actmap );
        displaymap();
    }
}

void selectAndRunLuaScript( const ASCString& filenamewildcard  )
{
    ASCString file = selectFile( filenamewildcard, true );
    if ( file.size() ) {
        LuaState state;
        LuaRunner runner( state );
        runner.runFile( file );
        if ( !runner.getErrors().empty() )
            errorMessage( runner.getErrors() );
        updateFieldInfo();
    }
}


/********************************************************************/
/********************************************************************/
/****** copy area and utitity methods END                      ******/
/********************************************************************/
/********************************************************************/




void testDebugFunction()
{
    // tempsvisible = true;
    actmap->cleartemps(7);
    for ( int x = 0; x < actmap->xsize; ++x )
        for ( int y = 0; y < actmap->ysize; ++y ) {
            MapField* fld = actmap->getField(x,y);
            if ( fld->getVisibility(5) >= visible_now )
                fld->setaTemp(1);
            else
                fld->setaTemp(0);
        }

    repaintMap();
}
