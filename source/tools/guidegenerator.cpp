//
// C++ Implementation: guidegenerator
//
// Description: 
//
//
// Author: Kevin Hirschmann <hirsch@dhcppc0>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
/*
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>

#include "../itemrepository.h"
#include "../buildingtype.h"
#include "../vehicletype.h"
#include "../ascstring.h"
#include "../typen.h"
#include "../basegfx.h"
#include "../loadpcx.h"
#include "../terraintype.h"
#include "../sgstream.h"

#include "guidegenerator.h"
#include "infopage.h"

const ASCString BuildingGuideGen::APPENDIX ="B";
const ASCString UnitGuideGen::APPENDIX ="U";

const ASCString UnitGuideGen::TROOPER = "Trooper";
const ASCString UnitGuideGen::GROUNDUNIT = "Ground unit";
const ASCString UnitGuideGen::AIRUNIT = "Aircraft";
const ASCString UnitGuideGen::SEAUNIT = "Marine";
const ASCString UnitGuideGen:: TURRETUNIT = "Turret";

ImageConverter::ImageConverter() {}


ASCString ImageConverter::createPic(const BuildingType&  bt, ASCString filePath) {
  tvirtualdisplay sb(600,600,255);
  int x=0; int y=0;
  for ( int xp = 0; xp < 4; xp++ ) {
    for ( int yp = 0; yp < 6; yp++ ) {
      if ( bt.getpicture(BuildingType::LocalCoordinate(xp,yp) )) {
        putspriteimage ( x + xp * fielddistx + ( yp & 1 ) *
                         fielddisthalfx, y + yp * fielddisty, bt.getpicture(
                           BuildingType::LocalCoordinate( xp, yp) ) );
      }
    }
  }
  pal[255][0] = 254;
  pal[255][1] = 253;
  pal[255][2] = 252;
  int xsize = 300;
  int ysize = 200;
  convert(bt, constructImgPath(bt, filePath), xsize, ysize);
  return  (constructImgPath(bt, RELATIVEIMGPATH)) ;
}

void ImageConverter::convert(const ContainerBaseType&  cbt,  ASCString file, int xsize, int ysize) {
  ASCString command;

#ifdef _WIN32_
  ASCString tempPath = getenv("temp");
  appendbackslash(tempPath);
#else
  ASCString tempPath = "/tmp/";
#endif

  ASCString tempFileName = tempPath + "tempPic.pcx";
  cout << "command is: " << command << endl;
  command = "convert \"" + tempFileName + "\" -transparent \"#f8f4f0\" " + "\"" + file + "\"";
  cout << "creating image..." << command << endl;
  writepcx ( tempFileName, 0, 0, xsize, ysize, pal );
  cout << "creating image..." << file << endl;
  system( command.c_str() );
  //copyFile( tempPath + fileName+ ".gif", prefixDir + fileName+ ".gif" );
  remove ( tempFileName.c_str() );
}


ASCString ImageConverter::createPic(const VehicleType&  vt, ASCString filePath) {
  tvirtualdisplay sb(100,100,255);
  putspriteimage ( 0, 0, vt.picture[0] );
  pal[255][0] = 254;
  pal[255][1] = 253;
  pal[255][2] = 252;
  convert(vt, constructImgPath(vt, filePath));
  return  (constructImgPath(vt, RELATIVEIMGPATH)) ;
}

ASCString ImageConverter::constructImgPath(const BuildingType&  bt, const ASCString filePath) {
 return (filePath + strrr(bt.id) + "B.gif");
}

ASCString ImageConverter::constructImgPath(const VehicleType&  vt, const ASCString filePath) {
 return (filePath + strrr(vt.id) + "U.gif");
}

//**************************************************************************************************************

GuideGenerator::GuideGenerator(ASCString fp, ASCString css, int id, bool imgCreate, int imageSize):filePath(fp), cssFile(css), setID(id), createImg(imgCreate), imageWidth(imageSize) {}

const ASCString& GuideGenerator::getImagePath(int id) {
  return graphicRefs[id];
}

const ASCString& GuideGenerator::getCSSPath() const {
  return cssFile;
}

//******************************************************************************************************
BuildingGuideGen::BuildingGuideGen(ASCString fp, ASCString css, int id, bool imgCreate, bool bUnique, 
int imageSize): GuideGenerator(fp, css, id, imgCreate), buildingsUnique(bUnique) {}


ASCString BuildingGuideGen::constructFileName(const BuildingType& bType) {
  return ASCString(strrr(bType.id)) + APPENDIX;

}

void BuildingGuideGen::processBuilding(const BuildingType&  bt) {
  cout << "Processing building:"  << bt.id <<", " << bt.name << endl;
  int buildingID = bt.id;
  ASCString imgPath;
  ImageConverter ic;
  if(createImg) {
    imgPath = ic.createPic(bt, filePath);
  } else {
    imgPath = ic.constructImgPath(bt, filePath);
  }
  graphicRefs.insert(Int2String::value_type(buildingID, imgPath));
  BuildingMainPage mp(bt, filePath, this);
  mp.buildPage();
  BuildingTerrainPage tp(bt, filePath, this);
  tp.buildPage();
  BuildingCargoPage ap(bt, filePath,  this);
  ap.buildPage();
  BuildingResourcePage rp(bt, filePath, this);
  rp.buildPage();
}



void BuildingGuideGen::generateCategories() const {
  try {
    for ( std::vector<SingleUnitSet*>::iterator i = unitSets.begin(); i != unitSets.end(); i++  ) {
      SingleUnitSet* s = (*i);
      if((s->ID == setID)||(setID<=0)) {
        ASCString fileName =  filePath + s->name + APPENDIX + GROUPS;
        Category set(s->name, cssFile);

        Category* hqCat = new Category(HQ, cssFile);
        Category* facCat = new Category(FACTORY, cssFile);
        Category* researchCat = new Category(RESEARCHFAC, cssFile);
        Category* windpowCat = new Category(WINDPOWERPLANT, cssFile);
        Category* solarpowCat = new Category(SOLARPOWERPLANT, cssFile);
        Category* mattconCat = new Category(MATTERCONVERTER, cssFile);
        Category* trainCat = new Category(TRAININGCENTER, cssFile);
        Category* noCat = new Category(NOCATEGORY, cssFile);
        Category* mineCat = new Category(MININGSTATION, cssFile);
        Category* idCat = new Category(SORTID, cssFile);

        set.addEntry(hqCat);
        set.addEntry(facCat);
        set.addEntry(researchCat);
        set.addEntry(windpowCat);
        set.addEntry(solarpowCat);
        set.addEntry(mattconCat);
        set.addEntry(trainCat);
        set.addEntry(noCat);
        set.addEntry(mineCat);

        for ( int building = 0; building < buildingTypeRepository.getNum(); building++ ) {
          BuildingType*  bt = buildingTypeRepository.getObject_byPos ( building );
          if(s->isMember(bt->id, SingleUnitSet::unit)) {
            ASCString fileLink = strrr(bt->id) + APPENDIX + ASCString(MAINLINKSUFFIX) + HTML;
            CategoryMember* dataEntry = new CategoryMember(bt->name.toUpper(), cssFile, fileLink, TARGET);
            if ( bt->special & cghqb ) {
              hqCat->addEntry(dataEntry);
            } else if ( bt->special & cgvehicleproductionb ) {
              facCat->addEntry(dataEntry);
            } else if ( bt->special & cgresearchb ) {
              researchCat->addEntry(dataEntry);
            } else if ( bt->special & cgwindkraftwerkb ) {
              windpowCat->addEntry(dataEntry);
            } else if(bt->special & cgsolarkraftwerkb) {
              solarpowCat->addEntry(dataEntry);
            } else if ( bt->special & cgconventionelpowerplantb ) {
              mattconCat->addEntry(dataEntry);
            } else if ( bt->special & cgminingstationb ) {
              mineCat->addEntry(dataEntry);
            } else if ( bt->special & cgtrainingb ) {
              trainCat->addEntry(dataEntry);
            } else {
              noCat->addEntry(dataEntry);
            }
            CategoryMember* idEntry = new CategoryMember(strrr(bt->id) + ASCString (MAINLINKSUFFIX) + HTML, cssFile, fileLink, TARGET);
            idCat->addEntry(idEntry);
          }
        }
        set.sort();
        idCat->sort();
        set.addEntry(idCat);
        GroupFile gf (fileName.c_str(), set);
        gf.write();
      }
    }
  } catch(ASCmsgException& e) {
    cout << e.getMessage() << endl;
  }
}

BuildingGuideGen::~BuildingGuideGen() {}



void BuildingGuideGen::processSubjects() {
  cout << "Selected set-ID is " << setID << endl;
  for ( int building = 0; building < buildingTypeRepository.getNum(); building++ ) {
    BuildingType*  bt = buildingTypeRepository.getObject_byPos ( building );
    if(setID > 0) {
      for ( std::vector<SingleUnitSet*>::iterator i = unitSets.begin(); i != unitSets.end(); i++  ) {
        SingleUnitSet* s = (*i);
        if((s->ID == setID) && (s->isMember(bt->id,SingleUnitSet::unit))) {
          if(buildingNames.end() != buildingNames.find(strrr(s->ID) +bt->name)||(!buildingsUnique)) {
	    cout << "Test 1 " << setID << endl;
            processBuilding(*bt);
            buildingNames.insert(strrr(s->ID) +bt->name);
          }
        }
      }
    } else {
      processBuilding(*bt);
    }
  }
  generateCategories();
}
//UnitGuideGen****************************************************************************************************
UnitGuideGen::UnitGuideGen(ASCString fp, ASCString css, int id, bool imgCreate, int imageSize): GuideGenerator(fp, css, id, imgCreate, imageSize) {}



UnitGuideGen::~UnitGuideGen() {}

ASCString UnitGuideGen::constructFileName(const VehicleType& vt) {
  return ASCString(strrr(vt.id)) + APPENDIX;

}

void UnitGuideGen::processSubjects() {
  cout << "Selected set-ID is " << setID << endl;
  for(int unit = 0; unit < vehicleTypeRepository.getNum(); unit++) {
    VehicleType* vt = vehicleTypeRepository.getObject_byPos(unit);
    if(setID > 0) {
      for ( std::vector<SingleUnitSet*>::iterator i = unitSets.begin(); i != unitSets.end(); i++  ) {
        SingleUnitSet* s = (*i);
        if((s->ID == setID) && (s->isMember(vt->id, SingleUnitSet::unit))) {
          processUnit(*vt);
        }
      }
    } else {
      processUnit(*vt);
    }
  }
  generateCategories();
}

void UnitGuideGen::processUnit(const VehicleType& vt) {
  cout << "Processing unit:"  << vt.id <<", " << vt.name << endl;
  int unitID = vt.id;
  ASCString imgPath;
  ImageConverter ic;
  if(createImg) {
    imgPath = ic.createPic(vt, filePath);
  } else {
    imgPath = ic.constructImgPath(vt, filePath);
  }
  graphicRefs.insert(Int2String::value_type(unitID, imgPath));
  UnitMainPage mp(vt, filePath, this);
  mp.buildPage();
  UnitTerrainPage tp(vt, filePath, this);
  tp.buildPage();
  UnitCargoPage ap(vt, filePath,  this);
  ap.buildPage();
  UnitWeaponPage wp(vt, filePath, this);
  wp.buildPage();
  UnitConstructionPage cp(vt,filePath, this);
  cp.buildPage();
  UnitResearchPage rp(vt, filePath, this);
  rp.buildPage();
}

void UnitGuideGen::generateCategories() const {
  try {
    for ( std::vector<SingleUnitSet*>::iterator i = unitSets.begin(); i != unitSets.end(); i++  ) {
      SingleUnitSet* s = (*i);
      if((s->ID == setID)||(setID<=0)) {
        ASCString fileName =  filePath + s->name + APPENDIX + GROUPS;
        Category set(s->name, cssFile);

        Category* trooperCat = new Category(TROOPER, cssFile);
        Category* groundCat = new Category(GROUNDUNIT, cssFile);
        Category* airCat = new Category(AIRUNIT, cssFile);
        Category* seaCat = new Category(SEAUNIT, cssFile);
        Category* turretCat = new Category(TURRETUNIT, cssFile);
        Category* miscCat = new Category(NOCATEGORY, cssFile);
        Category* idCat = new Category(SORTID, cssFile);



        set.addEntry(trooperCat);
        set.addEntry(groundCat);
        set.addEntry(airCat);
        set.addEntry(seaCat);
        set.addEntry(turretCat);
        set.addEntry(miscCat);

        for ( int unit = 0; unit < vehicleTypeRepository.getNum(); unit++ ) {
          VehicleType*  vt = vehicleTypeRepository.getObject_byPos ( unit );
          if(s->isMember(vt->id, SingleUnitSet::unit )) {
            ASCString fileLink = strrr(vt->id) + APPENDIX + ASCString(MAINLINKSUFFIX) + HTML;

            CategoryMember* dataEntry = new CategoryMember(vt->name.toUpper(), cssFile, fileLink, TARGET);
            switch ( vt->movemalustyp ) {
            case 7:  //   "trooper"
              trooperCat->addEntry(dataEntry);
              break;
            case 1:  // "light tracked vehicle"
            case 2:  // "medium tracked vehicle"
            case 3:  // "heavy tracked vehicle",
            case 4:  // "light wheeled vehicle",
            case 5:  //  "medium wheeled vehicle",
            case 6:  //  "heavy wheeled vehicle",
            case 8:   // "rail vehicle",
              groundCat->addEntry(dataEntry);
              break;
            case 9:   // "medium aircraft",
            case 12:  // "light aircraft",
            case 13:  // "heavy aircraft",
            case 16:  //  "helicopter",
              airCat->addEntry(dataEntry);
              break;
            case 10:  // "medium ship",
            case 14:  // "light ship",
            case 15:  // "heavy ship",
              seaCat->addEntry(dataEntry);
              break;

            case 11:  // "building / turret / object",
              turretCat->addEntry(dataEntry);
              break;

            case 0:  // default",
            case 17: // "hoovercraft"
            default:
              miscCat->addEntry(dataEntry);
              break;
            };
            CategoryMember* idEntry = new CategoryMember(strrr(vt->id) + ASCString (MAINLINKSUFFIX) + HTML, cssFile, fileLink, TARGET);
            idCat->addEntry(idEntry);
          }
        }
        set.sort();
        idCat->sort();
        set.addEntry(idCat);
        GroupFile gf (fileName.c_str(), set);
        gf.write();
      }
    }
  } catch(ASCmsgException& e) {
    cout << e.getMessage() << endl;
  }
}


