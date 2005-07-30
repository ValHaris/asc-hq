/***************************************************************************
                          infopage.cpp  -  description
                             -------------------
    begin                : Tue Jan 23 2001
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

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

#include "infopage.h"
#include "guidegenerator.h"


#define USE_GRAPHVIZ
const ASCString InfoPage::ANCHORTOP = "pageTop";

const ASCString TechTreePage::TITLE = "Tech tree";

const ASCString BuildingMainPage::TITLE = "Main info";
const ASCString BuildingTerrainPage::TITLE = "Construction";
const ASCString BuildingCargoPage::TITLE = "Cargo";
const ASCString BuildingResourcePage::TITLE  = "Resource";
const ASCString BuildingResearchPage::TITLE  = "Research";



const ASCString UnitCargoPage::TITLE = "Cargo";
const ASCString UnitMainPage::TITLE = "Main Info";
const ASCString UnitTerrainPage::TITLE = "Movement";
const ASCString UnitWeaponPage::TITLE = "Weapon Systems";
const ASCString UnitConstructionPage::TITLE = "Construction";
const ASCString UnitResearchPage::TITLE = "Research";


InfoPage::~InfoPage() {}
;


InfoPage::InfoPage(const ContainerBaseType&  baseType, ASCString fn, ASCString fp, ASCString in, ASCString title, GuideGenerator* gen):filePath(fp), fileName(fn), cbt(baseType), index(in), generator(gen), pageTitle(title), fullFileName(filePath + fileName + index + HTML) {
  buildingInfStream = openOFileStream(fullFileName);

}

ASCString InfoPage::constructEMF(int energy, int material, int fuel) {
  stringstream ss;
  ss << energy << " Energy<br>"  << material << " Material<br>" << fuel << " Fuel";
  return  ss.str();
}

ASCString InfoPage::getHeightImgString( int height ) {
  ASCString s;
  for ( int j = 0; j < 8 ; j++ )
    if ( height & ( 1 << j)) {
      ASCString s2;
      s2 = constructImageLink(STDGFXPATH + ASCString("hoehe") + strrr(j) +ASCString(".gif"), choehenstufen[j]);
      s += s2;
    }
  return s;
}

void InfoPage::buildPage() {
  createHTMLHeader(generator->getMainCSSPath());
  startBody();
  *buildingInfStream << "<p>" << endl;
  *buildingInfStream << "<a name=\"" << ANCHORTOP << "\"></a>" << endl;
  *buildingInfStream << "</p>" << endl;
  IntVec iv;
  iv.push_back(45);
  iv.push_back(55);
  startTable(0, RELATIVE, 100, RELATIVE, 90, VOID, "bg", &iv);
  *buildingInfStream << "<tr class=\"bg\">" << endl;
  //Left part
  *buildingInfStream << "<td valign=\"top\">" << endl;
  buildLeftArea();
  *buildingInfStream << "</td>" << endl;

  //Right part
  *buildingInfStream << "<td>" << endl;
  startTable(0, RELATIVE, 100, RELATIVE, 100);
  *buildingInfStream << "<tr>" << endl;
  this->addSectionLinks();
  *buildingInfStream << "</tr>" << endl;
  *buildingInfStream << "<tr class=\"bg\">" << endl;
  *buildingInfStream << "<td class=\"bg\" valign=\"top\">" << endl;
  addHeadline(pageTitle, 2);
  buildContent();
  endTable();
  *buildingInfStream << "</td>" << endl;
  *buildingInfStream << "</tr>" << endl;
  *buildingInfStream << "<tr>" << endl;
  addEmptyTD(1);
  addTDEntry(constructLink("back to top", "#" + ASCString(ANCHORTOP), "top"));
  *buildingInfStream << "</tr>" << endl;
  endTable();
  endBody();
  endHTML();
  buildingInfStream->close();
}

void InfoPage::createHTMLHeader(ASCString cssPath) {
  *buildingInfStream << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">" << endl;
  *buildingInfStream << "<html>" << endl;
  *buildingInfStream << "<HEAD>" << endl;
  *buildingInfStream << "<meta http-equiv=\"content-type\" content=\"text/html; charset=ISO-8859-1\"><TITLE>ASC-Document</TITLE>" << endl;
  *buildingInfStream << "<LINK REL=\"stylesheet\" TYPE=\"text/css\"";
  (cssPath == "")? *buildingInfStream << "HREF=\"" << CSSPATH <<"\"": *buildingInfStream << "HREF=\"" << cssPath << "\"";
  *buildingInfStream << ">" << endl;
  *buildingInfStream << "</HEAD>" << endl;
}

void InfoPage::startBody() {
  *buildingInfStream << "<BODY  class=\"wgleft\">" << endl;
}

void InfoPage::endBody() {
  *buildingInfStream << "</body>" << endl;

}
void InfoPage::startTable(int border, TableWidth interpretWAs, int width, TableWidth interpretHAs, int height, TableFrame frame, ASCString cssClass, const IntVec* cols) {
  *buildingInfStream << "<table ";
  if(cssClass!="") {
    *buildingInfStream << "class=\"" + cssClass + "\" ";
  }
  if(interpretWAs != NONE) {
    *buildingInfStream << "width=\"" << width;
    switch(interpretWAs) {
    case ABSOLUTE: *buildingInfStream << "px\""; break;
    case RELATIVE: *buildingInfStream << "%%\"";
      break;
    }
  }
  *buildingInfStream <<  "border=\"" << border << "\" ";
  if(frame != VOID) {
    *buildingInfStream << "frame=\"";
    if(frame == BOX) {
      *buildingInfStream << "box";
    } else if(frame == RHS) {
      *buildingInfStream << "RHS";
    }
    *buildingInfStream << "\"";
  }
  *buildingInfStream << ">" << endl;
  if(cols) {
    *buildingInfStream << "<colgroup>" << endl;
    for(int i=0; i< cols->size(); i++) {
      *buildingInfStream << "<col width=\"" << (*cols)[i] <<"%%\">" << endl;
    }
    *buildingInfStream << "</colgroup>" << endl;
  }
}

void InfoPage::endTable() {
  *buildingInfStream << "</table>" << endl;

}
void InfoPage::endHTML() {
  *buildingInfStream << "</html>" << endl;
}

void InfoPage::addTREntry(ASCString title, const ASCString& data) {
  *buildingInfStream << "<tr class=\"wg\">" << "<th class=\"wg\">" << title << "</th>" << "<td class=\"wg\">" << data << "</td>" << "</tr>" << endl;
}

void InfoPage::addTREntry(ASCString title, const int data) {
  *buildingInfStream << "<tr class=\"wg\">" << "<th class=\"wg\">" << title << "</th>" << "<td class=\"wg\">" << data << "</td>" << "</tr>" << endl;
}

void InfoPage::addTREntry(const ASCString& text) {
  *buildingInfStream << "<tr class=\"wg\">" << "<td class=\"wg\">" << text << "</td>" <<  "</tr>"  << endl;
}

void InfoPage::addEmptyTD(int times) {
  for(int i = 0; i < times; i++) {
    *buildingInfStream << "<td class=\"wg\"></td>";
  }
}

void InfoPage::addTDEntry(const ASCString& data) {
  *buildingInfStream << "<td class=\"wg\">" << data << "</td>";
}

void InfoPage::addTDEntry(const int data) {
  *buildingInfStream << "<td class=\"wg\">" << strrr(data) << "</td>";
}
void InfoPage::startTD() {
  *buildingInfStream << "<td class=\"wg\">";
}
void InfoPage::endTD() {
  *buildingInfStream << "</td>";
}

ASCString InfoPage::addTREntryln(ASCString oldCap, ASCString toAddCap) {
  return (oldCap + toAddCap + "<br>") ;
}

void InfoPage::addTitle(const ASCString& title) {
  *buildingInfStream << "<th class=\"wg\">" << title << "</th>" << endl;

}

void InfoPage::addHeadline(ASCString headLine, int select) {
  if((1 > select)||(select > 6)) {
    select = 1;
  }
  *buildingInfStream << "<h" << select << ">" << headLine << "</h" << select << ">" << endl;
}

void InfoPage::addParagraph(ASCString text) {
  *buildingInfStream << "<p>" << text << "</p>" << endl;

}
void InfoPage::addLink(ASCString label, ASCString ref, ASCString cssClass, ASCString target) {
  *buildingInfStream << constructLink(label, ref, cssClass, target) << endl;

}

ASCString InfoPage::constructLink(ASCString label, ASCString ref, ASCString cssClass, ASCString target) {
  ASCString result = "<a ";
  if(!cssClass.empty()) {
    result += "class =\"" + cssClass + "\" ";
  }
  if(!target.empty()) {
    result += "target =\"" + target +"\" ";
  }
  return(result+= "href=\"" + ref + "\">" + label + "</a>&nbsp");
}


ASCString InfoPage::constructImageLink(ASCString ref, ASCString altText, ASCString cssClass, int width) {
  ASCString result = "<img src=\"" + ref + "\" alt=\"" + altText + "\" ";
  if(width > 0) {
    result += ASCString(" width =\"") + strrr(width) + "\"";
  }
  if(!cssClass.empty()) {
    result += " class =\"" + cssClass + "\" ";
  }
  result += + ">";
  return result;
}

void InfoPage::addImage(ASCString ref, ASCString altText, int imageWidth) {
  *buildingInfStream << constructImageLink(ref, altText, "", imageWidth);
}

void InfoPage::buildLeftArea() {
  startTable(0, RELATIVE, 100, RELATIVE, 100);
  *buildingInfStream << "<tr>" << endl;
  *buildingInfStream << "<td align=\"center\">" << endl;
  *buildingInfStream << "<p class=\"imgalign\">" << endl;    
  addImage(generator->getImagePath(cbt.id), "Container picture", generator->getImageWidth());
  *buildingInfStream << "</p>" << endl;
  addHeadline(cbt.name, 3);
  *buildingInfStream << "</td>" << endl;
  *buildingInfStream << "<tr>" << endl;
  *buildingInfStream << "<td>" << endl;
  *buildingInfStream << "<hr>" << endl;
  addMainText();
  *buildingInfStream << "</td>" << endl;
  *buildingInfStream << "</tr>" << endl;
  endTable();
}
void InfoPage::addMainText() {
  addHeadline("Description", 2);  
  addInfoPicture(generator->getFilePath(), strrr(cbt.id));
  if ( !cbt.infotext.empty() ) {
    ASCString text = cbt.infotext;
    while ( text.find ( "#crt#" ) != ASCString::npos )
      text.replace ( text.find  ("#crt#"), 5, "<p>");
    while ( text.find ( "#CRT#" ) != ASCString::npos )
      text.replace ( text.find  ("#CRT#"), 5, "<p>");
    while ( text.find ( "\n" ) != ASCString::npos )
      text.replace ( text.find  ("\n"), 1, "<p>");
    while ( text.find ( "\r" ) != ASCString::npos )
      text.replace ( text.find  ("\r"), 1, "");

    ASCString::size_type begin = 0;
    ASCString::size_type end = 0;
    do {
      begin = text.find ( "#", end );
      if ( begin != ASCString::npos && begin+1 < text.length() ) {
        end = text.find ( "#", begin+1 );
        if ( end != ASCString::npos ) {
          text.erase ( begin, end-begin+1 );
          begin = 0;
          end = 0;
        }
      } else
        end = ASCString::npos;
    } while ( end != ASCString::npos );

    addParagraph(text.c_str() );
  } else {
    addParagraph("No information available: Do you  want to write an info text? Send it to unitguide@asc-hq.org" );
  }
}
ofstream* InfoPage::openOFileStream(const ASCString& fileName) {
  ofstream* buildingInfStream = new ofstream(fileName.c_str());
  if(!buildingInfStream->is_open()) {
    cerr << "File: " << fileName.c_str() << " could not be opened" << endl;
  }
  return buildingInfStream;
}

ifstream* InfoPage::openIFileStream(const ASCString& fileName) {
  ifstream* inputStream = new ifstream(fileName.c_str());
  if(!inputStream->is_open()) {
    cerr << "File: " << fileName.c_str() << " could not be opened" << endl;
  }
  return inputStream;
}

bool InfoPage::fileExists(const ASCString& fileName) const{
  ifstream inputStream(fileName.c_str());  
  return(inputStream.good());
}
//***********************************************************************************************************

BuildingInfoPage::BuildingInfoPage(const BuildingType&  buildingType, ASCString filePath, ASCString index, ASCString title, BuildingGuideGen* generator):InfoPage(buildingType, BuildingGuideGen::constructFileName(buildingType), filePath,  index, title, generator), bt(buildingType) {}

BuildingInfoPage::~BuildingInfoPage() {}

void BuildingInfoPage::addSectionLinks() {
  *buildingInfStream << "<td class=\"linksection\" valign=\"top\">" << endl;
  *buildingInfStream << "<p class=\"bgmenu\">" << endl;
  addLink (BuildingMainPage::TITLE, fileName + MAINLINKSUFFIX + HTML);
  addLink (BuildingTerrainPage::TITLE, fileName + CONSTRUCTIONLINKSUFFIX + HTML);
  addLink (BuildingCargoPage::TITLE, fileName + CARGOLINKSUFFIX + HTML);
  addLink (BuildingResourcePage::TITLE, fileName + RESOURCELINKSUFFIX + HTML);
  addLink (BuildingResearchPage::TITLE, fileName + RESEARCHLINKSUFFIX + HTML);
  *buildingInfStream << "</p>" << endl;
  *buildingInfStream << "<hr>" << endl;
  *buildingInfStream << "</td>" << endl;
}

void BuildingInfoPage::addInfoPicture(ASCString path, ASCString fileName){

}


//***********************************************************************************************************


BuildingMainPage::BuildingMainPage(const BuildingType&  bt, ASCString filePath, BuildingGuideGen* gen):BuildingInfoPage(bt, filePath,  MAINLINKSUFFIX, TITLE, gen) {}

void BuildingMainPage::addCategory() {

  if ( bt.special & cghqb ) {
    addTREntry("Category", HQ);
    return;
  }
  if ( bt.special & cgvehicleproductionb ) {
    addTREntry("Category", FACTORY);
    return;
  }
  if ( bt.special & cgresearchb ) {
    addTREntry("Category", RESEARCHFAC);
    return;
  }

  if ( bt.special & cgwindkraftwerkb ) {
    addTREntry("Category", WINDPOWERPLANT);
    return;
  }
  if ( bt.special & cgsolarkraftwerkb ) {
    addTREntry("Category", SOLARPOWERPLANT);
    return;
  }
  if ( bt.special & cgconventionelpowerplantb ) {
    addTREntry("Category", MATTERCONVERTER);
    return;
  }
  if ( bt.special & cgminingstationb ) {
    addTREntry("Category", MININGSTATION);
    return;
  }
  if ( bt.special & cgtrainingb ) {
    addTREntry("Category", TRAININGCENTER);
    return;
  }
  addTREntry("Category", NOCATEGORY);
  return;

}


void BuildingMainPage::addCapabilities() {
  ASCString cap;
  for ( int i = 0; i < cbuildingfunctionnum; ++i)
  if ( bt.special & ( 1 << i )){
     cap = addTREntryln(cap, cbuildingfunctions[i] );
  }
  addTREntry("Capabilities", cap);
  return;


}

void BuildingMainPage::buildContent() {
  addHeadline("General", 4);
  startTable(1, RELATIVE, 100, RELATIVE, 100);
  addTREntry("Name", bt.name);
  addTREntry("ID", bt.id);
  addCategory();
  addCapabilities();
  addTREntry("Default Armor", bt._armor);
  addTREntry("View", bt.view);
  addTREntry("Jamming", bt.jamming);  
  endTable();

}
//*******************************************************************************************
BuildingTerrainPage::BuildingTerrainPage(const BuildingType&  bt, ASCString fp, BuildingGuideGen* gen)
    : BuildingInfoPage(bt, fp, CONSTRUCTIONLINKSUFFIX, TITLE, gen) {}


void BuildingTerrainPage::buildContent() {
  addHeadline("General", 4);
  startTable(1, RELATIVE, 100, RELATIVE, 100);
  addConstructionCosts();
  addTREntry("Construction Steps", bt.construction_steps);
  addTREntry("BuildingLevel", getHeightImgString(bt.buildingheight));
  endTable();

  addHeadline("Terrain access", 4);
  IntVec iv;
  iv.push_back(37);
  iv.push_back(21);
  iv.push_back(21);
  iv.push_back(21);
  startTable(1, RELATIVE, 100, NONE, 0, VOID, "", &iv);
  *buildingInfStream << "<tr>";
  addTitle("Terrain Type");
  addTitle("Constructable on");
  addTitle("Not Constructable on");
  addTitle("Fatal");
  *buildingInfStream << "</tr>" << endl;
  for(int i=0; i< cbodenartennum; i++) {
    ASCString terrain = cbodenarten[i];

    if (( bt.terrainaccess.terrain.test(i) )|| ( bt.terrainaccess.terrainnot.test(i) )
        || ( bt.terrainaccess.terrainkill.test(i) )) {


      if ( bt.terrainaccess.terrain.test(i) ) {
        addTREntry(terrain, constructImageLink(STDGFXPATH + ASCString("haken.gif"), "YES") + "<td class=\"wg\"></td><td class=\"wg\">" );
      } /*
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  if ( bt.terrainaccess.terrainreq.test(i) ){
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               addTREntry(terrain, constructImageLink("http://www.asc-hq.org/asc/unitguide/exkl.gif"));
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  }*/
      if ( bt.terrainaccess.terrainnot.test(i) ) {
        addTREntry(terrain, "</td> <td class=\"wg\">" + constructImageLink(STDGFXPATH + ASCString("hakenrot.gif"), "YES") + "<td class=\"wg\">");
      }
      if ( bt.terrainaccess.terrainkill.test(i) ) {
        addTREntry(terrain, "</td> <td class=\"wg\"></td> <td class=\"wg\">" + constructImageLink(STDGFXPATH + ASCString ("hakenrot.gif"), "YES"));
      }
    }
  }
  endTable();
}


void BuildingTerrainPage::addConstructionCosts() {
  Resources costs = bt.productionCost;
  int energyCost = costs.resource(0);
  int materialCost = costs.resource(1);
  int fuelCost = costs.resource(2);
  ASCString buildingCosts = constructEMF(energyCost, materialCost, fuelCost);
  addTREntry("Building Costs", buildingCosts);
}



//*********************************************************************************************
BuildingCargoPage::BuildingCargoPage(const BuildingType&  bt, ASCString filePath, BuildingGuideGen* generator)
    : BuildingInfoPage(bt, filePath, CARGOLINKSUFFIX, TITLE, generator) {}

void BuildingCargoPage::buildContent() {
  addHeadline("Capacaties", 4);
  startTable(1, RELATIVE, 100);

  addTREntry("Maximal loadable Units", cbt.maxLoadableUnits);
  if(cbt.maxLoadableUnits > 0 ) {
    addTREntry("Max. total Cargo Weight", cbt.maxLoadableWeight);
    addTREntry("Max. single Cargo Weight", cbt.maxLoadableUnitSize);
    ASCString type;
    for ( int h = 0; h < cmovemalitypenum; h++ ) {
      if ( cbt.vehicleCategoriesStorable & (1 << h)) {
        type = addTREntryln(type, cmovemalitypes[h]);
      }
    }
    addTREntry("Building accessable to", type);
    endTable();
    if ( !cbt.entranceSystems.empty() ) {
      addHeadline("Entrance System", 4);
      startTable(1, RELATIVE, 100);
      *buildingInfStream << "<tr class=\"wg\"><th class=\"wg\">Mode</th>";
      ASCString entry;
      for ( ContainerBaseType::EntranceSystems::const_iterator i = cbt.entranceSystems.begin();
            i !=cbt.entranceSystems.end(); i++ ) {
        if ( i->mode & ContainerBaseType::TransportationIO::In )
          entry = addTREntryln(entry,"In");
        if ( i->mode & ContainerBaseType::TransportationIO::Out )
          entry = addTREntryln(entry,"Out");
        if ( i->mode & ContainerBaseType::TransportationIO::Docking )
          entry = addTREntryln(entry,"Docking");
        addTDEntry(entry);
        entry = "";
      }
      *buildingInfStream << "</tr>" << endl;
      *buildingInfStream << "<tr class=\"wg\"><th class=\"wg\">Absolute Height</th>";
      for ( ContainerBaseType::EntranceSystems::const_iterator i = cbt.entranceSystems.begin(); i != cbt.entranceSystems.end(); i++ ) {
        addTDEntry(getHeightImgString(i->height_abs));
      }
      *buildingInfStream << "</tr>" << endl;
      //Relative Height
      *buildingInfStream << "<tr class=\"wg\"><th class=\"wg\">Relative Height</th>";
      for ( ContainerBaseType::EntranceSystems::const_iterator i = cbt.entranceSystems.begin(); i != cbt.entranceSystems.end(); i++ ) {
        if(i->dockingHeight_rel != -100) {
          addTDEntry(strrr(i->height_rel));
        } else {
          addEmptyTD(1);
        }
      }
      *buildingInfStream << "</tr>" << endl;
      //Absolute Docking Height
      entry="";
      *buildingInfStream << "<tr class=\"wg\"><th class=\"wg\">Absolute Docking Height</th>";
      for ( ContainerBaseType::EntranceSystems::const_iterator i = cbt.entranceSystems.begin(); i != cbt.entranceSystems.end(); i++ ) {
        if ( i->mode & ContainerBaseType::TransportationIO::Docking ) {
          addTDEntry(getHeightImgString( i->dockingHeight_abs));
        } else {
          addEmptyTD(1);
        }
      }
      *buildingInfStream << "</tr>" << endl;
      //Relative Docking Height
      entry = "";
      *buildingInfStream << "<tr class=\"wg\"><th class=\"wg\">Relative Docking Height</th>";
      for ( ContainerBaseType::EntranceSystems::const_iterator i = cbt.entranceSystems.begin();
            i != cbt.entranceSystems.end(); i++ ) {
        if ( i->mode & ContainerBaseType::TransportationIO::Docking &&  i->dockingHeight_rel != -100 ) {
          addTDEntry( strrr( i->dockingHeight_rel));
        } else {
          addEmptyTD(1);
        }
      }
      *buildingInfStream << "</tr>" << endl;
      *buildingInfStream << "<tr class=\"wg\"><th class=\"wg\">Excluded Unit Groups</th>";
      for ( ContainerBaseType::EntranceSystems::const_iterator i = cbt.entranceSystems.begin(); i !=     cbt.entranceSystems.end(); i++ ) {
        ASCString s;
        type ="";
        for ( int h = 0; h < cmovemalitypenum; h++ )
          if ( !(i->vehicleCategoriesLoadable & (1 << h))) {
            type = addTREntryln(type, cmovemalitypes[h]);
          }

        addTDEntry(type);
        type ="";
      }
      *buildingInfStream << "</tr>" << endl;
      *buildingInfStream << "<tr class=\"wg\"><th class=\"wg\">Building Height</th>";
      for ( ContainerBaseType::EntranceSystems::const_iterator i = cbt.entranceSystems.begin(); i != cbt.entranceSystems.end(); i++ ) {
        if ( i->mode & ContainerBaseType::TransportationIO::Docking ) {
          addTDEntry(getHeightImgString( i->container_height));
        } else {
          addEmptyTD(1);
        }
      }
      *buildingInfStream << "</tr>" << endl;
      *buildingInfStream << "<tr class=\"wg\"><th class=\"wg\">Unit can attack afterwards</th>";
      for ( ContainerBaseType::EntranceSystems::const_iterator i = cbt.entranceSystems.begin(); i != cbt.entranceSystems.end(); i++ ) {
        addTDEntry(i->disableAttack ? "No": "Yes");
      }
      *buildingInfStream << "</tr>" << endl;
      *buildingInfStream << "<tr class=\"wg\"><th class=\"wg\">Needs Vehicle Functions</th>";
      for ( ContainerBaseType::EntranceSystems::const_iterator i = cbt.entranceSystems.begin(); i != cbt.entranceSystems.end(); i++ ) {
        ASCString funcs;
        bool none = true;
        for ( int j = 0; j<cvehiclefunctionsnum; j++) {
          if ( i->requireUnitFunction & ( 1 << j )) {
            funcs = addTREntryln(funcs, cvehiclefunctions[j]);
            none = false;
          }
        }
        if(none) {
          addTDEntry("None");
        } else {
          addTDEntry(funcs);
        }
      }
      *buildingInfStream << "</tr>" << endl;


      *buildingInfStream << "<tr class=\"wg\"><th class=\"wg\">Movement Costs</th>";
      for ( ContainerBaseType::EntranceSystems::const_iterator i = cbt.entranceSystems.begin(); i != cbt.entranceSystems.end(); i++ ) {
        if ( i->movecost >= 0 )
          addTDEntry(strrr(i->movecost) );
        else
          addTDEntry(0);
      }
    }
    *buildingInfStream << "</tr>" << endl;
    endTable();
  }
}


//*********************************************************************************************
BuildingResourcePage::BuildingResourcePage(const BuildingType& bt, ASCString fp, BuildingGuideGen* gen)
  :BuildingInfoPage(bt, fp, RESOURCELINKSUFFIX, TITLE, gen) {}


void BuildingResourcePage::buildContent() {
  startTable(1, RELATIVE, 100);
  addHeadline("Mining & Storage", 4);
  addTREntry("Effeciency fuel", bt.efficiencyfuel);
  addTREntry("Effeciency material", bt.efficiencymaterial);

  {
    Resources stor = bt._tank;
    ASCString storage;
    int energyStorage = stor.resource(0);
    int materialStorage = stor.resource(1);
    int fuelStorage = stor.resource(2);
    storage = constructEMF(energyStorage, materialStorage, fuelStorage);
    addTREntry("Maximum storage (ASC mode)", storage);
  }
  {
    Resources biStorage = bt._bi_maxstorage;
    ASCString storage;
    int energyCost = biStorage.resource(0);
    int materialCost = biStorage.resource(1);
    int fuelCost = biStorage.resource(2);
    storage = constructEMF(energyCost, materialCost, fuelCost);
    addTREntry("Maximum Storage in BI-mode", storage);
  }
  {
    Resources plus = bt.maxplus;
    ASCString storage;
    int energyPlus = plus.resource(0);
    int materialPlus = plus.resource(1);
    int fuelPlus = plus.resource(2);
    storage = constructEMF(energyPlus, materialPlus, fuelPlus);
    addTREntry("Maximum plus", storage);
  }



  Resources defProduction = bt.defaultProduction;
  ASCString defProd;
  int energyProd = defProduction.resource(0);
  int materialProd = defProduction.resource(1);
  int fuelProd = defProduction.resource(2);
  defProd = constructEMF(energyProd, materialProd, fuelProd);
  addTREntry("Default Production", defProd);
  endTable();
  addHeadline("Research", 4);
  IntVec colVec;
  colVec.push_back(66);
  colVec.push_back(34);
  startTable(1, RELATIVE, 100, NONE, 0, VOID, "bg", &colVec);
  addTREntry("Max research points", bt.maxresearchpoints);
  addTREntry("Default research points", bt.defaultMaxResearchpoints);
  addTREntry("Nominal research points", bt.nominalresearchpoints);
  endTable();
}

//*********************************************************************************************
UnitInfoPage::UnitInfoPage(const VehicleType&  vType, ASCString filePath, ASCString index, ASCString title, UnitGuideGen* generator):InfoPage(vType, UnitGuideGen::constructFileName(vType), filePath, index, title, generator), vt(vType) {}

UnitInfoPage::~UnitInfoPage() {}

void UnitInfoPage::addSectionLinks() {
  *buildingInfStream << "<td class=\"linksection\" valign=\"top\">" << endl;
  *buildingInfStream << "<p class=\"bgmenu\">" << endl;
  addLink (BuildingMainPage::TITLE, fileName  + MAINLINKSUFFIX + HTML);
  addLink (UnitTerrainPage::TITLE, fileName + MOVEMENTLINKSUFFIX + HTML);
  addLink (UnitWeaponPage::TITLE, fileName + WEAPONLINKSUFFIX + HTML);
  addLink (UnitCargoPage::TITLE, fileName + CARGOLINKSUFFIX + HTML);
  addLink (UnitConstructionPage::TITLE, fileName + CONSTRUCTIONLINKSUFFIX + HTML);
  addLink (UnitResearchPage::TITLE, fileName + RESEARCHLINKSUFFIX + HTML);
  *buildingInfStream << "</p>" << endl;
  *buildingInfStream << "<hr>" << endl;
  *buildingInfStream << "</td>" << endl;
}


void UnitInfoPage::addInfoPicture(ASCString filePath, ASCString fileName){
  if(fileExists(filePath + fileName + UNITINFOPICSUFFIX)){
    addImage(ASCString(RELATIVEIMGPATH) + fileName + UNITINFOPICSUFFIX, "InfoPic");
  }
}

//*********************************************************************************************
UnitCargoPage::UnitCargoPage(const VehicleType&  vt, ASCString filePath, UnitGuideGen* generator):
UnitInfoPage(vt, filePath, CARGOLINKSUFFIX, TITLE, generator) {}


void UnitCargoPage::buildContent() {
  addHeadline("Capacaties: Resources", 4);
  startTable(1, RELATIVE, 100, RELATIVE, 100);
  addTREntry("Energy", vt.tank.energy);
  addTREntry("Material", vt.tank.material);
  addTREntry("Fuel", vt.tank.fuel);
  endTable();
  addHeadline("Capacaties: Units", 4);
  if(cbt.maxLoadableUnits) {
    startTable(1, RELATIVE, 100);
    addTREntry("Maximal loadable Units", cbt.maxLoadableUnits);
    addTREntry("Max. total Cargo Weight", cbt.maxLoadableWeight);
    addTREntry("Max. single Cargo Weight", cbt.maxLoadableUnitSize);
    ASCString type;
    for ( int h = 0; h < cmovemalitypenum; h++ ) {
      if ( cbt.vehicleCategoriesStorable & (1 << h)) {
        type = addTREntryln(type, cmovemalitypes[h]);
      }
    }
    addTREntry("Unit accessable to", type);
    endTable();
    if ( !cbt.entranceSystems.empty() ) {
      addHeadline("Entrance System", 4);
      startTable(1, RELATIVE, 100);
      *buildingInfStream << "<tr class=\"wg\"><th class=\"wg\">Mode</th>";
      ASCString entry;
      for ( ContainerBaseType::EntranceSystems::const_iterator i = cbt.entranceSystems.begin();
            i !=cbt.entranceSystems.end(); i++ ) {
        if ( i->mode & ContainerBaseType::TransportationIO::In )
          entry = addTREntryln(entry,"In");
        if ( i->mode & ContainerBaseType::TransportationIO::Out )
          entry = addTREntryln(entry,"Out");
        if ( i->mode & ContainerBaseType::TransportationIO::Docking )
          entry = addTREntryln(entry,"Docking");
        addTDEntry(entry);
        entry = "";
      }
      *buildingInfStream << "</tr>" << endl;
      *buildingInfStream << "<tr class=\"wg\"><th class=\"wg\">Absolute Height</th>";
      for ( ContainerBaseType::EntranceSystems::const_iterator i = cbt.entranceSystems.begin(); i != cbt.entranceSystems.end(); i++ ) {
        addTDEntry(getHeightImgString(i->height_abs));
      }
      *buildingInfStream << "</tr>" << endl;
      //Relative Height
      *buildingInfStream << "<tr class=\"wg\"><th class=\"wg\">Relative Height</th>";
      for ( ContainerBaseType::EntranceSystems::const_iterator i = cbt.entranceSystems.begin(); i != cbt.entranceSystems.end(); i++ ) {
        if(i->dockingHeight_rel != -100) {
          addTDEntry(strrr(i->height_rel));
        } else {
          addEmptyTD(1);
        }
      }
      *buildingInfStream << "</tr>" << endl;
      //Absolute Docking Height
      entry="";
      *buildingInfStream << "<tr class=\"wg\"><th class=\"wg\">Absolute Docking Height</th>";
      for ( ContainerBaseType::EntranceSystems::const_iterator i = cbt.entranceSystems.begin(); i != cbt.entranceSystems.end(); i++ ) {
        if ( i->mode & ContainerBaseType::TransportationIO::Docking ) {
          addTDEntry(getHeightImgString( i->dockingHeight_abs));
        } else {
          addEmptyTD(1);
        }
      }
      *buildingInfStream << "</tr>" << endl;
      //Relative Docking Height
      entry = "";
      *buildingInfStream << "<tr class=\"wg\"><th class=\"wg\">Relative Docking Height</th>";
      for ( ContainerBaseType::EntranceSystems::const_iterator i = cbt.entranceSystems.begin();
            i != cbt.entranceSystems.end(); i++ ) {
        if ( i->mode & ContainerBaseType::TransportationIO::Docking &&  i->dockingHeight_rel != -100 ) {
          addTDEntry(strrr( i->dockingHeight_rel));
        } else {
          addEmptyTD(1);
        }
      }
      *buildingInfStream << "</tr>" << endl;
      *buildingInfStream << "<tr class=\"wg\"><th class=\"wg\">Transporter Height</th>";
      for ( ContainerBaseType::EntranceSystems::const_iterator i = cbt.entranceSystems.begin(); i != cbt.entranceSystems.end(); i++ ) {
        if ( i->mode & ContainerBaseType::TransportationIO::Docking ) {
          addTDEntry(getHeightImgString( i->container_height));
        } else {
          addEmptyTD(1);
        }
      }      
      *buildingInfStream << "</tr>" << endl;
      *buildingInfStream << "<tr class=\"wg\"><th class=\"wg\">Excluded Unit Groups</th>";
      for ( ContainerBaseType::EntranceSystems::const_iterator i = cbt.entranceSystems.begin(); i !=     cbt.entranceSystems.end(); i++ ) {
        ASCString s;
        type ="";
        for ( int h = 0; h < cmovemalitypenum; h++ )
          if ( !(i->vehicleCategoriesLoadable & (1 << h))) {
            type = addTREntryln(type, cmovemalitypes[h]);
          }

        addTDEntry(type);
        type ="";
      }
      *buildingInfStream << "</tr>" << endl;
      *buildingInfStream << "<tr class=\"wg\"><th class=\"wg\">Unit can attack afterwards</th>";
      for ( ContainerBaseType::EntranceSystems::const_iterator i = cbt.entranceSystems.begin(); i != cbt.entranceSystems.end(); i++ ) {
        addTDEntry(i->disableAttack ? "No": "Yes");
      }
      *buildingInfStream << "</tr>" << endl;
      *buildingInfStream << "<tr class=\"wg\"><th class=\"wg\">Needs Vehicle Functions</th>";
      for ( ContainerBaseType::EntranceSystems::const_iterator i = cbt.entranceSystems.begin(); i != cbt.entranceSystems.end(); i++ ) {
        ASCString funcs;
        bool none = true;
        for ( int j = 0; j<cvehiclefunctionsnum; j++) {
          if ( i->requireUnitFunction & ( 1 << j )) {
            funcs = addTREntryln(funcs, cvehiclefunctions[j]);
            none = false;
          }
        }
        if(none) {
          addTDEntry("None");
        } else {
          addTDEntry(funcs);
        }
      }
      *buildingInfStream << "</tr>" << endl;


      *buildingInfStream << "<tr class=\"wg\"><th class=\"wg\">Movement Costs</th>";
      for ( ContainerBaseType::EntranceSystems::const_iterator i = cbt.entranceSystems.begin(); i != cbt.entranceSystems.end(); i++ ) {
        if ( i->movecost >= 0 )
          addTDEntry(strrr(i->movecost) );
        else
          addTDEntry(0);
      }
    }
    *buildingInfStream << "</tr>" << endl;
    endTable();
  }
}

//******************************************************************************************************
UnitMainPage::UnitMainPage(const VehicleType&  vtype, ASCString filePath, UnitGuideGen* generator): UnitInfoPage(vtype, filePath, MAINLINKSUFFIX, TITLE, generator) {}



void UnitMainPage::addCapabilities() {

 ASCString cap;

 for ( int i = 0; i < cvehiclefunctionsnum; ++i)
  if ( vt.functions & ( 1 << i )){       
   cap = addTREntryln(cap, cvehiclefunctions[i] );
 }
 addTREntry("Capabilities & Properties", cap);
 return;
}

void UnitMainPage::addConstructionCosts() {
  Resources costs = vt.productionCost;
  ASCString buildingCosts;
  int energyCost = costs.resource(0);
  int materialCost = costs.resource(1);
  int fuelCost = costs.resource(2);
  buildingCosts = constructEMF(energyCost, materialCost, fuelCost);
  addTREntry("Production Costs", buildingCosts);
}

void UnitMainPage::buildContent() {
  addHeadline("General", 4);
  startTable(1, RELATIVE, 100, RELATIVE, 100);
  addTREntry("Name", vt.name);
  addTREntry("ID", vt.id);
  addTREntry("Group", cmovemalitypes[vt.movemalustyp]);
  addTREntry("Task", vt.description);
  addTREntry("Weight", vt.weight);
  addCapabilities();
  addTREntry("Default Armor", vt.armor);
  addTREntry("View", vt.view);
  addTREntry("Jamming", vt.jamming);
  addTREntry("Auto-repair Rate", vt.autorepairrate);
  if(vt.maxwindspeedonwater >= maxwindspeed) {
    addTREntry("Resists wind force", "Unlimited");
  } else {
    addTREntry("Resists wind force", vt.maxwindspeedonwater);
  }

  endTable();
  addHeadline("Production", 3);
  startTable(1, RELATIVE, 100, RELATIVE, 100);
  addConstructionCosts();
  endTable();
}
//******************************************************************************************************

UnitTerrainPage::UnitTerrainPage(const VehicleType&  vt, ASCString filePath, UnitGuideGen* generator): UnitInfoPage(vt, filePath, MOVEMENTLINKSUFFIX, TITLE, generator) {}


void UnitTerrainPage::buildContent() {
  addHeadline("General", 4);
  startTable(1, RELATIVE, 100, RELATIVE, 100);
  addTREntry("Fuel tank", vt.tank.fuel );
  addTREntry("Fuel Consumption", vt.fuelConsumption);
  addTREntry("Range", vt.fuelConsumption ? strrr(vt.tank.fuel/vt.fuelConsumption) : "None");   
  endTable();
  addHeadline("Speed", 4);
  startTable(1, RELATIVE, 100, NONE, 0);
  *buildingInfStream << "<tr>";
  addTitle("Level");
  for(int i=0; i< choehenstufennum; i++) {
    addTDEntry(constructImageLink(STDGFXPATH + ASCString("hoehe") +strrr(i)+ ".gif", choehenstufen[i]))  ;
  }
  *buildingInfStream << "</tr>";
  *buildingInfStream << "<tr>";
  addTitle("max. Speed");
  for(int i =0; i < vt.movement.size(); i++) {
    if(vt.movement[i] > 0) {
      addTDEntry(vt.movement[i]);
    } else {
      addTDEntry("n.a.");
    }
  }
  *buildingInfStream << "</tr>";
  endTable();
  addHeadline("Terrain access", 4);
  IntVec iv;
  iv.push_back(36);
  iv.push_back(16);
  iv.push_back(16);
  iv.push_back(16);
  iv.push_back(16);
  startTable(1, RELATIVE, 100, NONE, 0, VOID, "", &iv);
  *buildingInfStream << "<tr>";
  addTitle("Terrain Type");
  addTitle("Accessible");
  addTitle("Requieres additionally");
  addTitle("Blocked by");
  addTitle("Fatal");
  *buildingInfStream << "</tr>" << endl;
  for(int i=0; i< cbodenartennum; i++) {
    ASCString terrain = cbodenarten[i];
    if(vt.terrainaccess.terrain.test(i) && vt.terrainaccess.terrainreq.test(i)) {
      addTREntry(terrain, constructImageLink(STDGFXPATH + ASCString("haken.gif"), "YES") +
                 "</td><td class=\"wg\">" + constructImageLink(STDGFXPATH + ASCString("exkl.gif"), "YES") +
                 "<td class=\"wg\"></td><td class=\"wg\"></td><td class=\"wg\"></td>" );
    } else {
      if ( vt.terrainaccess.terrain.test(i) ) {
        addTREntry(terrain, constructImageLink(STDGFXPATH + ASCString("haken.gif"), "YES") + "<td class=\"wg\"></td><td class=\"wg\"></td><td class=\"wg\"></td>" );
      }
      if (vt.terrainaccess.terrainreq.test(i) ) {
        addTREntry(terrain, "</td><td class=\"wg\">" + constructImageLink(STDGFXPATH + ASCString("exkl.gif"), "YES") + "<td class=\"wg\"></td><td class=\"wg\">");
      }
    }
    if ( vt.terrainaccess.terrainnot.test(i) ) {
      addTREntry(terrain, "</td> <td class=\"wg\"> </td><td class=\"wg\">" + constructImageLink(STDGFXPATH + ASCString("hakenrot.gif"), "YES") + "<td class=\"wg\">");
    }
    if ( vt.terrainaccess.terrainkill.test(i) ) {
      addTREntry(terrain, "</td> <td class=\"wg\"></td> <td class=\"wg\"></td> <td class=\"wg\">" + constructImageLink(STDGFXPATH + ASCString ("hakenrot.gif"), "YES"));
    }
  }
  endTable();
  addHeadline("Height Change", 4);
  {
    if ( vt.heightChangeMethodNum ) {
      startTable(1, RELATIVE, 100, NONE, 0, VOID, "", &iv);
      *buildingInfStream << "<tr>";
      addTitle("Current Height");
      addTitle("Height Change");
      addTitle("Requiered Movements Points");
      addTitle("Can still attack");
      addTitle("Distance");
      *buildingInfStream << "</tr>";
      for ( int w = 0; w < vt.heightChangeMethod.size() ; w++) {
        ASCString entry="";
        *buildingInfStream << "<tr>";
        for ( int h = 7; h >= 0; h-- ) {
          if ( vt.heightChangeMethod[w].startHeight & (1 << h ) & vt.height ) {
            entry = addTREntryln(entry, constructImageLink(STDGFXPATH + ASCString("hoehe") + strrr(h) +".gif",  choehenstufen[w]));
          }
        }
        addTDEntry(entry);
        if(vt.heightChangeMethod[w].heightDelta < 0) {
          addTDEntry(strrr(vt.heightChangeMethod[w].heightDelta));
        } else {
          addTDEntry(ASCString("+") + strrr(vt.heightChangeMethod[w].heightDelta));
        }
        addTDEntry(vt.heightChangeMethod[w].moveCost);
        addTDEntry(vt.heightChangeMethod[w].canAttack ? "yes" : "no");
        addTDEntry(vt.heightChangeMethod[w].dist);
        *buildingInfStream << "</tr>";
      }
    } else {
      addParagraph("n.a.");
    }
  }
}

//******************************************************************************************************
UnitWeaponPage::UnitWeaponPage(const VehicleType&  vt, ASCString filePath, UnitGuideGen* generator):UnitInfoPage(vt, filePath, WEAPONLINKSUFFIX, TITLE, generator) {}


void UnitWeaponPage::buildContent() {
  if(vt.weapons.count > 0) {
    startTable(1, RELATIVE, 100, NONE, 100);
    addHeadline("General", 3);
    *buildingInfStream << "<tr>" << endl;
    addTitle("Can shoot after movement");    
    if(!vt.wait)
      addTDEntry(constructImageLink(STDGFXPATH + ASCString("haken.gif"), "YES"));    
    else
      addTDEntry(constructImageLink(STDGFXPATH + ASCString("hakenrot.gif"), "NO"));    
    *buildingInfStream << "</tr>";
    endTable();
    IntVec iv;
    int k = div(100,  vt.weapons.count + 1).quot;
    for (int i = 0; i < vt.weapons.count + 1; i++) {
      iv.push_back(k);
    }
    addHeadline("Weapon Systems", 3);
    startTable(1, RELATIVE, 100, NONE, 100, VOID, "", &iv);
    *buildingInfStream << "<tr>" << endl;
    addTitle("Weapon System");
    for(int i = 0; i < vt.weapons.count; i++) {
      addTDEntry(i);
    }
    *buildingInfStream << "</tr>";
    *buildingInfStream << "<tr>" << endl;
    addTitle("Name");
    for(int i = 0; i < vt.weapons.count; i++) {
      addTDEntry(vt.weapons.weapon[i].getName());
    }
    *buildingInfStream << "</tr>" << endl;
    *buildingInfStream << "<tr>" << endl;
    addTitle("Min Range");
    for(int i = 0; i < vt.weapons.count; i++) {
      addTDEntry((vt.weapons.weapon[i].mindistance+9)/10);
    }
    *buildingInfStream << "</tr>" << endl;
    *buildingInfStream << "<tr>" << endl;
    addTitle("Max Range");
    for(int i = 0; i < vt.weapons.count; i++) {
      addTDEntry(vt.weapons.weapon[i].maxdistance/10);
    }
    *buildingInfStream << "</tr>" << endl;
    *buildingInfStream << "<tr>" << endl;
    addTitle("Punch @ Min Range");
    for(int i = 0; i < vt.weapons.count; i++) {
      addTDEntry(vt.weapons.weapon[i].maxstrength);
    }
    *buildingInfStream << "</tr>" << endl;
    *buildingInfStream << "<tr>" << endl;
    addTitle("Punch @ Max Range");
    for(int i = 0; i < vt.weapons.count; i++) {
      addTDEntry(vt.weapons.weapon[i].minstrength);
    }
    *buildingInfStream << "</tr>" << endl;
    *buildingInfStream << "<tr>" << endl;
    addTitle("Ammo");
    for(int i = 0; i < vt.weapons.count; i++) {
      addTDEntry(vt.weapons.weapon[i].count);
    }
    *buildingInfStream << "</tr>" << endl;
    bool laser = false;
    for ( int w = 0; w < vt.weapons.count ; w++) {
      if ( vt.weapons.weapon[w].gettype() & cwlaserb ) {
        laser = true;
      }
    }
    if ( laser ) {
      *buildingInfStream << "</tr>" << endl;
      addTitle("Laser recharge rate");
      for ( int w = 0; w < vt.weapons.count ; w++)
        addTDEntry( vt.weapons.weapon[w].laserRechargeRate );

      *buildingInfStream << "</tr>" << endl;
      *buildingInfStream << "<tr>" << endl;
      addTitle( "Resources to recharge laser");

      for ( int w = 0; w < vt.weapons.count ; w++) {
        ASCString s;
        for ( int r = 0; r < 3; r++ ) {
          if ( vt.weapons.weapon[w].laserRechargeCost.resource(r)) {
            if ( !s.empty() )
              s += "; ";
            s += strrr(vt.weapons.weapon[w].laserRechargeCost.resource(r));
            s += " " ;
            s += resourceNames[r] ;
          }
        }
        addTDEntry(s);
      }
    }
    *buildingInfStream << "</tr>" << endl;
    *buildingInfStream << "<tr>" << endl;
    addTitle("Reactionfire Shot Count");
    for(int i = 0; i < vt.weapons.count; i++) {
      addTDEntry(vt.weapons.weapon[i].reactionFireShots);
    }
    *buildingInfStream << "</tr>" << endl;
    *buildingInfStream << "<tr>" << endl;
    addTitle("Can be fired");
    for(int i = 0; i < vt.weapons.count; i++) {
      if(vt.weapons.weapon[i].shootable()) {
        addTDEntry(constructImageLink(STDGFXPATH + ASCString("haken.gif"), "YES"));
      } else {
        addEmptyTD(1);
      }
    }
    *buildingInfStream << "</tr>" << endl;
    *buildingInfStream << "<tr>" << endl;
    addTitle("Can transfer ammo");
    for(int i = 0; i < vt.weapons.count; i++) {
      if(vt.weapons.weapon[i].canRefuel()) {
        addTDEntry(constructImageLink(STDGFXPATH + ASCString("haken.gif"), "YES"));
      } else {
        addEmptyTD(1);
      }
    }
    *buildingInfStream << "</tr>" << endl;
    endTable();

    addHeadline("Operating Height", 3);
    startTable(1, RELATIVE, 100, NONE, 100, VOID, "", &iv);
    *buildingInfStream << "<tr>" << endl;
    addTitle("Targets");
    for ( int w = 0; w < vt.weapons.count ; w++) {
      ASCString hstring;
      for ( int h = 0; h < 8; h++ )
        if ( vt.weapons.weapon[w].targ & (1 << h )) {
          ASCString s;
          s = addTREntryln(s, (constructImageLink((STDGFXPATH + ASCString("hoehe") + strrr(h) +".gif"), choehenstufen[h] )));
          hstring += s;
        } /*else {
                                                                                                                                        ASCString s;
                                                                                                                                        s = addTREntryln(s, (constructImageLink((STDGFXPATH + ASCString("hoehetrans") + strrr(h) +".gif"), "None" )));
                                                                                                                                        hstring += s;
                                                                                                                                      }*/
      addTDEntry(hstring);
    }
    *buildingInfStream << "</tr>" << endl;
    *buildingInfStream << "<tr>" << endl;
    addTitle("Usable on");
    for ( int w = 0; w < vt.weapons.count ; w++) {
      ASCString hstring;
      for ( int h = 0; h < 8; h++ )
        if ( vt.weapons.weapon[w].sourceheight & (1 << h )) {
          ASCString s;
          s = addTREntryln(s, (constructImageLink((STDGFXPATH + ASCString("hoehe") + strrr(h) +".gif"), choehenstufen[h] )));
          hstring += s;
        } /*else {
                                                                                                                                        ASCString s;
                                                                                                                                        s = addTREntryln(s, (constructImageLink((STDGFXPATH + ASCString("hoehetrans") + strrr(h) +".gif"), "None" )));
                                                                                                                                        hstring += s;
                                                                                                                                      }*/
      addTDEntry(hstring);
    }
    *buildingInfStream << "</tr>" << endl;
    endTable();
    addHeadline("Target Accuracy: Unit Types", 3);
    startTable(1, RELATIVE, 100, NONE, 100, VOID, "", &iv);
    //addTitle("Targets");
    for ( int h = 0; h < cmovemalitypenum; h++ ) {
      *buildingInfStream << "<tr>" << endl;
      addTitle( cmovemalitypes[h] );
      for ( int w = 0; w < vt.weapons.count ; w++)
        addTDEntry(strrr(vt.weapons.weapon[w].targetingAccuracy[h]) + ASCString("%"));
      *buildingInfStream << "</tr>" << endl;
    }
    endTable();
    addHeadline("Target Accuracy: Altitude Difference", 3);
    startTable(1, RELATIVE, 100, NONE, 100, VOID, "", &iv);
    for ( int h = 0; h < (choehenstufennum - 2) * 2 + 1; h++ ) {
      *buildingInfStream << "<tr>" << endl;
      addTitle(strrr(-1* choehenstufennum + 2+ h));
      for ( int w = 0; w < vt.weapons.count ; w++) {
        if ( vt.weapons.weapon[w].efficiency[h] > 0 ) {
          addTDEntry( strrr(vt.weapons.weapon[w].efficiency[h]) + ASCString("%") );
        } else {
          addTDEntry("n.a.");
        }
      }
    }
    endTable();
  } else {
    addParagraph("No weapon systems available");
  }
}
//******************************************************************************************************
UnitConstructionPage::UnitConstructionPage(const VehicleType&  vt, ASCString filePath, UnitGuideGen* generator):UnitInfoPage(vt, filePath, CONSTRUCTIONLINKSUFFIX, TITLE, generator) {}

void UnitConstructionPage::buildContent() {
  if((vt.objectsBuildable.size()==0) && (vt.objectsRemovable.size()==0)&&
      (vt.objectGroupsRemovable.size()==0)&& vt.vehiclesBuildable.size()==0) {
    addParagraph("n.a.");
  } else {
    addHeadline("Objects", 4);
    ASCString names;
    startTable(1, RELATIVE, 100);
    for ( unsigned int i = 0; i < vt.objectsBuildable.size(); i++ ) {
      for ( int b = 0; b < objectTypeRepository.getNum(); b++ ) {
        Object*type obj = objectTypeRepository.getObject_byPos ( b );
        if (     obj->id >= vt.objectsBuildable[i].from
                 && obj->id <= vt.objectsBuildable[i].to ) {
          names = addTREntryln(names, obj->name.c_str() + ASCString("(") + strrr(obj->id )+ ")");
        }
      }
    }
    if(!names.empty()) {
      *buildingInfStream << "<tr>" << endl;
      addTitle("Constructable Objects");
      addTDEntry(names);
      *buildingInfStream << "</tr>" << endl;
    }


    names = "";
    for ( unsigned int i = 0; i < vt.objectsRemovable.size(); i++ ) {
      for ( int b = 0; b < objectTypeRepository.getNum(); b++ ) {
        Object*type obj = objectTypeRepository.getObject_byPos ( b );
        if (     obj->id >= vt.objectsRemovable[i].from
                 && obj->id <= vt.objectsRemovable[i].to ) {
          ASCString s;
          names = addTREntryln(names, obj->name.c_str() + ASCString("(") + strrr(obj->id )+ ")");
        }
      }
    }
    if ( !names.empty() ) {
      *buildingInfStream << "<tr>" << endl;
      addTitle("Destructable Objects");
      addTDEntry(names);
      *buildingInfStream << "</tr>" << endl;
    }
    names = "";
    for ( vector<IntRange>::const_iterator i = vt.objectGroupsRemovable.begin(); i != vt.objectGroupsRemovable.end(); ++i ) {
      if ( i->from != i->to ) {
        for(int k = i->from; k < i->to; k++) {
          names = addTREntryln(names, strrr(k));
        }
      } else {
        names = addTREntryln(names, strrr(i->from));
      }
    }
    if ( !names.empty() ) {
      *buildingInfStream << "<tr>" << endl;
      addTitle("Destructable Object-Groups");
      addTDEntry(names);
      *buildingInfStream << "</tr>" << endl;
    }

    names = "";
    for ( vector<IntRange>::const_iterator i = vt.objectGroupsBuildable.begin(); i != vt.objectGroupsBuildable.end(); ++i ) {
      if ( i->from != i->to ) {
        for(int k = i->from; k < i->to; k++) {
          names = addTREntryln(names, strrr(k));
        }
      } else {
        names = addTREntryln(names, strrr(i->from));
      }
    }
    if ( !names.empty() ) {
      *buildingInfStream << "<tr>" << endl;
      addTitle("Constructable Object-Groups");
      addTDEntry(names);
      *buildingInfStream << "</tr>" << endl;
    }

    names = "";
    for ( unsigned int i = 0; i < vt.buildingsBuildable.size(); i++ ) {
      for ( int b = 0; b < buildingTypeRepository.getNum(); b++ ) {
        BuildingType* bld = buildingTypeRepository.getObject_byPos ( b );
        if (     bld->id >= vt.buildingsBuildable[i].from
                 && bld->id <= vt.buildingsBuildable[i].to ) {

          names = addTREntryln(names, constructLink(bld->name.c_str() + ASCString("(") + strrr(bld->id )+ ")", RELATIVEBUILDINGSPATH + BuildingGuideGen::constructFileName(*bld) + MAINLINKSUFFIX + HTML));

        }
      }
    }
    if ( !names.empty() ) {
      *buildingInfStream << "<tr>" << endl;
      addTitle("Constructable Buildings");
      addTDEntry(names);
      *buildingInfStream << "</tr>" << endl;
    }

    names = "";
    for ( unsigned int i = 0; i < vt.vehiclesBuildable.size(); i++ ) {
      for ( int b = 0; b < vehicleTypeRepository.getNum(); b++ ) {
        Vehicletype* veh = vehicleTypeRepository.getObject_byPos ( b );
        if (     veh->id >= vt.vehiclesBuildable[i].from
                 && veh->id <= vt.vehiclesBuildable[i].to ) {
          names = addTREntryln(names, constructLink(veh->name.c_str() + ASCString("(") + strrr(veh->id )+ ")", "./" + UnitGuideGen::constructFileName(*veh) + MAINLINKSUFFIX + HTML));
        }
      }
    }
    if ( !names.empty() ) {
      *buildingInfStream << "<tr>" << endl;
      addTitle("Constructable Units");
      addTDEntry(names);
      *buildingInfStream << "</tr>" << endl;
    }
  }
  endTable();
}




//******************************************************************************************************
UnitResearchPage::UnitResearchPage(const VehicleType&  vt, ASCString filePath, UnitGuideGen* generator):UnitInfoPage(vt, filePath, RESEARCHLINKSUFFIX, TITLE, generator) {}


void UnitResearchPage::buildContent() {
#ifdef USE_GRAPHVIZ
  addHeadline("TechTree", 3);
  startTable(0, RELATIVE, 100, RELATIVE, 100);
  *buildingInfStream << "<tr>" << endl;
  startTD();
  ASCString techedFileName = UnitGuideGen::constructFileName(vt) + "_tech";
  ASCString tempTechFileName = filePath + techedFileName;
  {
    tn_file_buf_stream f ( tempTechFileName + ".dot", tnstream::writing );
    f.writeString ( "digraph  \"Tech Dependency\" { \nbgcolor=\"transparent\" \n", false );
    f.writeString("node [bgcolor=\"blue\"]\n", false);
    vt.techDependency.writeInvertTreeOutput( vt.getName() + " ", f, &generator->getTechTreeIDs() );
    f.writeString ( "\"" + vt.getName() + " \" [shape=diamond] \n", false );
    f.writeString ( "}\n", false );
  }
  ASCString techPicPath = RELATIVEIMGPATH + techedFileName + ".png";
  ASCString dotFile = tempTechFileName + ".dot";
  ASCString sysCommand = "dot " + dotFile + "  -Tpng -o" + tempTechFileName + ".png";
  int res = system ( sysCommand.c_str() );
  if ( res != 0 ) {
    cerr << "ERROR encountered at: " << sysCommand.c_str() << endl;
  }
  remove(dotFile.c_str());

  TechTreePage ttp(vt, techedFileName, filePath, generator, techPicPath);
  ttp.buildPage();
  if(generator->generatesUpload()) {
    InfoPageUtil::updateFile(ttp.getPageFileName(), generator->getFilePath());
    InfoPageUtil::updateFile(tempTechFileName + ".png", generator->getFilePath());
  }
  addLink(constructImageLink(RELATIVEIMGPATH + techedFileName + ".png", "TECH-TREE", "techtree"),  techedFileName + HTML, "", "External");
  endTD();
  addParagraph("(Click to enlarge)");
  *buildingInfStream << "</tr>" << endl;
  endTable();

#endif
}

//******************************************************************************************************
BuildingResearchPage::BuildingResearchPage(const BuildingType&  bt, ASCString filePath, BuildingGuideGen* generator):BuildingInfoPage(bt, filePath, RESEARCHLINKSUFFIX, TITLE, generator) {}



void BuildingResearchPage::buildContent() {
#ifdef USE_GRAPHVIZ
  addHeadline("TechTree", 3);
  startTable(0, RELATIVE, 100, RELATIVE, 100);
  *buildingInfStream << "<tr>" << endl;
  startTD();
  ASCString techedFileName = BuildingGuideGen::constructFileName(bt) + "_tech";
  ASCString tempTechFileName = filePath + techedFileName;
  {
    tn_file_buf_stream f ( tempTechFileName + ".dot", tnstream::writing );

    f.writeString ( "digraph  \"Tech Dependency\" { \nbgcolor=\"transparent\" \n", false );
    f.writeString("node [bgcolor=\"blue\"]\n", false);
    bt.techDependency.writeInvertTreeOutput( bt.name + " ", f );
    f.writeString ( "\"" + bt.name + " \" [shape=diamond] \n", false );

    f.writeString ( "}\n", false );

  }
  ASCString techPicPath = RELATIVEIMGPATH + techedFileName + ".png";
  ASCString dotFile = tempTechFileName + ".dot";
  ASCString sysCommand = "dot " + dotFile + "  -Tpng -o" + tempTechFileName + ".png";
  int res = system ( sysCommand.c_str() );
  if ( res != 0 ) {
    cerr << "ERROR encountered at: " << sysCommand.c_str() << endl;
  }

  remove(dotFile.c_str());

  TechTreePage ttp(bt, techedFileName, filePath, generator, techPicPath);
  ttp.buildPage();
  if(generator->generatesUpload()) {
    InfoPageUtil::updateFile(ttp.getPageFileName(), generator->getFilePath());
    InfoPageUtil::updateFile(tempTechFileName + ".png", generator->getFilePath());
  }
  addLink(constructImageLink(RELATIVEIMGPATH + techedFileName + ".png", "TECH-TREE", "techtree"),  techedFileName + HTML, "", "External");
  endTD();
  addParagraph("(Click to enlarge)");
  *buildingInfStream << "</tr>" << endl;
  endTable();

#endif
}


//***************************************************************************************************************
TechTreePage::TechTreePage(const ContainerBaseType& cbt, ASCString fn, ASCString filePath, GuideGenerator* generator, ASCString tPPath):
InfoPage(cbt, fn, filePath, "", TITLE, generator), techPicPath(tPPath) {}

void TechTreePage::buildContent() {}

void TechTreePage::addSectionLinks() {}

void TechTreePage::buildPage() {
  createHTMLHeader(generator->getMainCSSPath());
  startBody();
  addImage(techPicPath, "Tech Tree");
  endBody();
  endHTML();

}





