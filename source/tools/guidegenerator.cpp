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

typedef vector<InfoPage*> InfoPageVector;

const ASCString BuildingGuideGen::APPENDIX ="B";
const ASCString UnitGuideGen::APPENDIX ="U";

const ASCString UnitGuideGen::TROOPER = "Trooper";
const ASCString UnitGuideGen::GROUNDUNIT = "Ground unit";
const ASCString UnitGuideGen::AIRUNIT = "Aircraft";
const ASCString UnitGuideGen::SEAUNIT = "Marine";
const ASCString UnitGuideGen:: TURRETUNIT = "Turret";

bool InfoPageUtil::equalFiles(const ASCString src, const ASCString dst) {
  const int maxFileSize = 10000000;
  auto_ptr<char> s ( new char[maxFileSize]);
  auto_ptr<char> d ( new char[maxFileSize]);
  FILE* i = fopen ( src.c_str(), filereadmode );
  FILE* o = fopen ( dst.c_str(), filereadmode );
  bool result = true;
  if ( !i )
    fatalError ("equalFiles :: Could not open input file " + src);
  if ( !o ) {
    fclose ( i );
    result = false;
    return result;
  } else {
    int s1 = fread ( s.get(), 1, maxFileSize, i );
    int s2 = fread ( d.get(), 1, maxFileSize, o );
    if ( s1 != s2 )
      result = false;
    else {
      for ( int i = 0; i < s1; i++ )
        if ( s.get()[i] != d.get()[i] ) {
          result = false;
          break;
        }
    }
  }
  fclose( o );
  fclose( i );
  return result;
}

void InfoPageUtil::copyFile(const ASCString src, const ASCString dst) {
  const int maxFileSize = 10000000;

  auto_ptr<char> s ( new char[maxFileSize]);
  FILE* i = fopen ( src.c_str(), filereadmode );
  FILE* o = fopen ( dst.c_str(), filewritemode);
  if(!i){
    fclose( o );
    return;
  }
  if ( !o ){
   fclose( i );
   return;
  }
  int s1 = fread ( s.get(), 1, maxFileSize, i );
  fwrite ( s.get(), 1, s1, o );
  fclose( o );
  fclose( i );
  printf("*");
}

bool InfoPageUtil::diffMove(const ASCString src, const ASCString dst) {
  if(equalFiles(src, dst)) {
    remove(src.c_str());
    return false;
  } else {
    InfoPageUtil::copyFile(src, dst);
    remove(src.c_str());
    return true;
  }  
}

ASCString InfoPageUtil::getTmpPath() {
#ifdef _WIN32_
  ASCString tempPath = getenv("temp");
  appendbackslash(tempPath);
  return tempPath;
#else
  return ("/tmp/");
#endif

}

void InfoPageUtil::updateFile(ASCString fileName, ASCString exportPath) {  
  ASCString destilledFileName = fileName; 
#ifdef _WIN32_
  destilledFileName.erase(0, fileName.find_last_of("\\") + 1);  
#else
  destilledFileName.erase(0, fileName.find_last_of("/") + 1);  
#endif
  ASCString exportTarget = exportPath + destilledFileName ;
  ASCString tmpTarget = InfoPageUtil::getTmpPath() + destilledFileName;
  bool updatedFile = InfoPageUtil::diffMove(tmpTarget, exportTarget);    
}
//*************************************************************************************************************
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
  convert(constructImgFileName(bt), filePath, xsize, ysize);
  return  (constructImgPath(bt, RELATIVEIMGPATH)) ;
}

ASCString ImageConverter::createPic(const VehicleType&  vt, ASCString filePath) {
  tvirtualdisplay sb(100,100,255);
  putspriteimage ( 0, 0, vt.picture[0] );
  pal[255][0] = 254;
  pal[255][1] = 253;
  pal[255][2] = 252;
  convert(constructImgFileName(vt), filePath);
  return  (constructImgPath(vt, RELATIVEIMGPATH)) ;
}


void ImageConverter::convert(const ASCString&  fileName,  ASCString filePath, int xsize, int ysize) {
  ASCString command;  
  ASCString tempFileName = InfoPageUtil::getTmpPath() + "tempPic.pcx";  
  command = "convert \"" + tempFileName + "\" -transparent \"#f8f4f0\" " + "\"" + filePath + fileName + "\"";  
  writepcx ( tempFileName, 0, 0, xsize, ysize, pal );
  cout << "creating image..." << fileName << endl;
  system( command.c_str() );
  remove ( tempFileName.c_str() );
}




ASCString ImageConverter::constructImgPath(const BuildingType&  bt, const ASCString filePath) {
  return (filePath + constructImgFileName(bt));
}

ASCString ImageConverter::constructImgPath(const VehicleType&  vt, const ASCString filePath) {
  return (filePath + constructImgFileName(vt));
}

ASCString ImageConverter::constructImgFileName(const BuildingType&  bt) {
  return (strrr(bt.id) + ASCString("B.gif"));
}

ASCString ImageConverter::constructImgFileName(const VehicleType&  vt) {
  return (strrr(vt.id) + ASCString("U.gif"));
}


//**************************************************************************************************************

GuideGenerator::GuideGenerator(ASCString fp, ASCString css, int id, ASCString techIDs, bool imgCreate, bool upload,
                               int imageSize):filePath(fp), cssFile(css), setID(id), techTreeIDs(String2IntRangeVector(techIDs)), createImg(imgCreate), createUpload(upload), imageWidth(imageSize) {}

const ASCString& GuideGenerator::getImagePath(int id) {
  return graphicRefs[id];
}

const ASCString& GuideGenerator::getCSSPath() const {
  return cssFile;
}


//******************************************************************************************************
BuildingGuideGen::BuildingGuideGen(ASCString fp, ASCString css, int id, ASCString techIDs, bool imgCreate, bool bUnique,
                                   bool upload, int imageSize): GuideGenerator(fp, css, id, techIDs, upload, imgCreate), buildingsUnique(bUnique) {}


ASCString BuildingGuideGen::constructFileName(const BuildingType& bType) {
  return ASCString(strrr(bType.id)) + APPENDIX;

}

void BuildingGuideGen::processBuilding(const BuildingType&  bt) {
  cout << "Processing building:"  << bt.id <<", " << bt.name << endl;
  int buildingID = bt.id;
  ImageConverter ic;
  ASCString imgPath;
  ASCString target;  
  if(createUpload){
    target = InfoPageUtil::getTmpPath();
  }else{
    target = filePath;
  }
  if(createImg) {
    imgPath = ic.createPic(bt, target);
    if(createUpload){      
      InfoPageUtil::updateFile(ic.constructImgPath(bt, filePath), filePath);
    }    
  } else {
    imgPath = ic.constructImgPath(bt, filePath);
  }  
  graphicRefs.insert(Int2String::value_type(buildingID, imgPath));
  InfoPageVector ipv;
  BuildingMainPage mp(bt, target, this);
  ipv.push_back(&mp);
  BuildingTerrainPage tp(bt, target, this);
  ipv.push_back(&tp);
  BuildingCargoPage ap(bt, target,  this);
  ipv.push_back(&ap);
  BuildingResourcePage rp(bt, target, this);
  ipv.push_back(&rp);
  BuildingResearchPage rea(bt, target, this);
  ipv.push_back(&rea);
  for(int i = 0; i < ipv.size(); i++) {
    ipv[i]->buildPage();
    if(createUpload){          
      InfoPageUtil::updateFile(ipv[i]->getPageFileName(), filePath);
    }
  }
}



void BuildingGuideGen::generateCategories() const {
  try {
    for ( std::vector<SingleUnitSet*>::iterator i = unitSets.begin(); i != unitSets.end(); i++  ) {
      SingleUnitSet* s = (*i);
      if((s->ID == setID)||(setID<=0)) {
        ASCString fileName =  InfoPageUtil::getTmpPath() + s->name + APPENDIX + GROUPS;
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
          if(s->isMember(bt->id, SingleUnitSet::building)) {
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
            CategoryMember* idEntry = new CategoryMember(ASCString(strrr(bt->id)) + "(" +  bt->name +")", cssFile, fileLink, TARGET);
            idCat->addEntry(idEntry);
          }
        }
        set.sort();
        idCat->sort();
        set.addEntry(idCat);
        GroupFile gf (fileName.c_str(), set);
        gf.write();
        InfoPageUtil::updateFile(fileName, filePath);
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
        if((s->ID == setID) && (s->isMember(bt->id,SingleUnitSet::building))) {
          if(buildingNames.end() != buildingNames.find(strrr(s->ID) +bt->name)||(!buildingsUnique)) {            
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
UnitGuideGen::UnitGuideGen(ASCString fp, ASCString css, int id, ASCString techIDs, bool imgCreate, bool upload, int imageSize): GuideGenerator(fp, css, id, techIDs, imgCreate, upload, imageSize) {}



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
        if((s->ID == setID) && (s->isMember(vt->id,SingleUnitSet::unit))) {
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
  ImageConverter ic;
  ASCString imgPath;
  ASCString target;
  if(createUpload){
    target = InfoPageUtil::getTmpPath();
  }else{
    target = filePath;
  }
  if(createImg) {
    imgPath = ic.createPic(vt, target);
    if(createUpload){      
      InfoPageUtil::updateFile(ic.constructImgPath(vt, filePath), filePath);
    }
  } else {
    imgPath = ic.constructImgPath(vt, filePath);
  }
  graphicRefs.insert(Int2String::value_type(unitID, imgPath));
  InfoPageVector ipv;  
  UnitMainPage mp(vt, target, this);
  ipv.push_back(&mp);
  UnitTerrainPage tp(vt, target, this);
  ipv.push_back(&tp);
  UnitCargoPage ap(vt, target,  this);
  ipv.push_back(&ap);
  UnitWeaponPage wp(vt, target, this);
  ipv.push_back(&wp);
  UnitConstructionPage cp(vt, target, this);
  ipv.push_back(&cp);
  UnitResearchPage rp(vt, target, this);
  ipv.push_back(&rp);
  for(int i = 0; i < ipv.size(); i++) {
    ipv[i]->buildPage();
    InfoPageUtil::updateFile(ipv[i]->getPageFileName(), filePath);
  }
}

void UnitGuideGen::generateCategories() const {
  try {
    for ( std::vector<SingleUnitSet*>::iterator i = unitSets.begin(); i != unitSets.end(); i++  ) {
      SingleUnitSet* s = (*i);
      if((s->ID == setID)||(setID<=0)) {
        ASCString fileName =  InfoPageUtil::getTmpPath() + s->name + APPENDIX + GROUPS;
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
          if(s->isMember(vt->id,SingleUnitSet::unit)) {
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
            CategoryMember* idEntry = new CategoryMember(ASCString(strrr(vt->id)) + "(" + vt->name + ")", cssFile, fileLink, TARGET);
            idCat->addEntry(idEntry);
          }
        }
        set.sort();
        idCat->sort();
        set.addEntry(idCat);	
        GroupFile gf (fileName.c_str(), set);
        gf.write();
        InfoPageUtil::updateFile(fileName, filePath);
      }
    }
  } catch(ASCmsgException& e) {
    cout << e.getMessage() << endl;
  }
}




