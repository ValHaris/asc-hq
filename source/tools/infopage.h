/***************************************************************************
                          infopage.h  -  description
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

#ifndef infopageH
#define infopageH

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <map>
#include <set>


#include "../itemrepository.h"
#include "../vehicletype.h"
#include "../buildingtype.h"
#include "../ascstring.h"
#include "../typen.h"
#include "../sgstream.h"

#include "guidegenerator.h"

#define VehicleType Vehicletype

#define HTML ".html"
#define GROUPS ".groups"
#define CSSPATH "asc.css"
#define ALTIMAGETEXT "Building graphic"
#define STDGFXPATH "../"
#define TARGET "main"
#define UNITINFOPICSUFFIX "u-big.jpg"

#define MAINLINKSUFFIX "i0"
#define MOVEMENTLINKSUFFIX "i1"
#define CARGOLINKSUFFIX "i2"
#define RESOURCELINKSUFFIX "i3"
#define WEAPONLINKSUFFIX "i4"
#define CONSTRUCTIONLINKSUFFIX "i5"
#define RESEARCHLINKSUFFIX "i6"

#define WINDPOWERPLANT "Wind power plant"
#define SOLARPOWERPLANT "Solar power plant"
#define HQ "Headquarters"
#define MININGSTATION "Mining station"
#define MATTERCONVERTER "Matter converter"
#define FACTORY "Factory"
#define TRAININGCENTER "Training Center"
#define RESEARCHFAC "Research Facility"
#define NOCATEGORY "Other"


#define SORTID "Sorted by ID"




typedef map<int, ASCString> Int2String;
typedef vector<ASCString> ASCStringVec;
typedef vector<int> IntVec;



enum TableWidth{NONE, RELATIVE, ABSOLUTE};
enum TableFrame{BOX, VOID, RHS};



/**
*@brief Abstract representation of one information page of a guide
*@author Kevin Hirschmann
*/
class InfoPage {

public:
  /**
  *@brief Destructor
  */
  virtual ~InfoPage();

  /**
  *@brief Fills a page with content
  */
  void buildPage();
  /**
  *@brief Returns the name of the page file
  */
  const ASCString getPageFileName() {
    return fullFileName;
  }
  
 virtual void addInfoPicture(ASCString path, ASCString fileName) = 0; 
protected:
  /**
  *@brief The file name of the guiding page
  */
  ASCString fileName;
  /**
  *@brief Path to where the page will be stored
  */
  ASCString filePath;
  /**
  *@brief The index of a concrete page category (used to distinguish between files for the same subject)
  */
  ASCString index;
  /**
  *@brief The title for the current page. Used for a headline
  */
  ASCString pageTitle;
  /**
  *@brief The stream on which to which the page is send
  */
  ofstream* buildingInfStream;
  /**
  *@brief The ContainerBaseType for which the page is created
  */
  const ContainerBaseType& cbt;
  /**
  *@brief The GuideGenerator who started the page construction. Holds several necessary informations
  */
  GuideGenerator* generator;
  /**
  *@brief Creates a new info-page
  *@param contBaseType The ContainerBaseType for which the page is created
  *@param fileName The fileName for the page (without index and extension)
  *@param filePath The place where the page will be stored
  *@param index The index for the page. Distinguishes it from the other pages for that ContainerBaseType
  *@param title The title displayed on the page
  *@param generator The GuideGenerator which is constructing this page
  */
  InfoPage(const ContainerBaseType&  contBaseType, ASCString fileName, ASCString filePath, ASCString index, ASCString title, GuideGenerator* generator);


  /**
  *@brief Builds the REAL content with information on the right page side  
  */
  virtual void buildContent()=0;
  /**
  *@brief Builds the main text. This is the area beneath the picture. 
  */
  virtual void addMainText();
  /**
  *@brief Builds the left area consisting of picture and mainText
  */
  virtual void buildLeftArea();
  /**
  *@brief Adds the section containing the links to the the pages of the subject
  */
  virtual void addSectionLinks() = 0;
  /**
  *@brief Constructs a string listing EMF, each separated by <br>
  *@param energy The energy value
  *@param material The material value
  *@param fuel The fuel value
  *@return A string listing EMF, each separated by <br>
  */
  ASCString constructEMF(int energy, int material, int fuel);
  /**
  *@brief Opens a file stream for output
  *@param fileName The path+fileName to the file where the stream writes to
  *@return A pointer to the stream
  */
  ofstream* openOFileStream(const ASCString& fileName);  
  /**
  *@brief Opens a file stream for input
  *@param fileName The path+fileName to the file where the stream reads from
  *@return A pointer to the stream
  */
  ifstream* openIFileStream(const ASCString& fileName);      
  /**
  *@brief Tests if a file exists
  *@param fileName The path+fileName to the file tested
  *@return true if the file exists, false otherwise
  */
  bool fileExists(const ASCString& fileName) const;
  /**
  *@brief Creates a HTML header. 
  *@param cssPath The css-File used for the InfoPage. Default is empty.
  */
  void createHTMLHeader(ASCString cssPath = "");
  /**
  *@brief Starts the body. 
  */
  void startBody();
  /**
  *@brief Ends the body
  */
  void endBody();
  /**
  *@brief  Starts a table
  *@param border Defines the thickness of the border
  *@param interpretWAs Defines if the width is to be seen absolute, relative or none is specified.
  *       Possible Values are ABSOLUTE, RELATIVE and NONE. Default is NONE
  *@param width The Width of the table
  *@param interpretHAs Defines if the height is to be seen absolute, relative or none is specified.
  *       Possible Values are ABSOLUTE, RELATIVE and NONE. Default is NONE
  *@param height The height of the table
  *@param frame Defines which borders of the frame shall be displayed. Default is VOID.
  *       Did not work (at least in Mozilla). 
  *@param cssClass The css-class used by this table. Default is empty
  *@param cols A vector containing the predefined width for the columns.
  *       The size of vector is equal to the amount of columns in this table
  */
  void startTable(int border, TableWidth interpretWAs = NONE,  int width = 0, TableWidth interpretHAs = NONE, int height = 0, TableFrame frame = VOID, ASCString cssClass = "", const IntVec* cols=0);
  /**
  *@brief Ends the table
  */
  void endTable();
  /**
  *@brief Ends the HTML document
  */
  void endHTML();
  /**
  *@brief Adds a text link to the page
  *@param label The label of the link
  *@param ref The destination where the link points to
  *@param cssClass A cssClass defining the style of the link
  */
  void addLink(ASCString label, ASCString ref, ASCString cssClass = "", ASCString target= "");
  /**
  *@brief Constructs a text link and returns the result
  *@param label The label of the link
  *@param ref The destination where the link points to
  *@param cssClass A cssClass defining the style of the link
  *@return The new constructed link
  */
  ASCString constructLink(ASCString label, ASCString ref, ASCString cssClass = "", ASCString target= "");
  /**
  *@brief Adds an image to the page  
  *@param ref The destination of the image added
  *@param altText The alternative text displayed if the image is not found
  */
  void addImage(ASCString ref, ASCString altText, int imageWidth = 0);
  /**
  *@brief Constructs the link to an image and returns it
  *@param ref The destination of the image added
  *@param altText The alternative text displayed if the image is not found
  *@param width The width for the image
  */
  ASCString constructImageLink(ASCString ref, ASCString altText, ASCString cssClass ="",  int width = 0);
  /**
  *@brief Constructs a string containing the links to all height pictures
          identified by height.
  *@param height A bitmapped value, from which can be decided which heights have to be chosen
          for the resulting string
  *@return A string containing the links to all height pictures identified by height.
  */
  ASCString getHeightImgString( int height );
  /**
  *@brief Adds a headline to the page
  *@param headline The headline to be displayed
  *@param select The chosen size of the headline. Possible values are 1-6 (HTML 4.0)
  */
  void addHeadline(ASCString headline, int select);
  /**
  *@brief Adds a text paragraph to the page
  *@param text The text of the paragraph  
  */
  void addParagraph(ASCString text);
  /**
  *@brief Adds table header field 
          In the table header field a text is added
  *@param title The title to be added to the new added table header field  
  */
  void addTitle(const ASCString& title);
  /**
  *@brief Adds a table row with one table header field and one table data field. 
          In the table header field a text and in the table data field a string is added
  *@param title The title to be added to the new added table header field
  *@param data The string to be filled in the new added table data field
  */
  void addTREntry(ASCString title, const ASCString& data);
  /**
  *@brief Adds a table row with one table header field and one table data field. 
          In the table header field a text and in the table data field a number is added
  *@param title The title to be added to the new added table header field
  *@param data The number to be filled in the new added table data field
  */
  void addTREntry(ASCString title, const int data);
  /**
  *@brief Adds a new table row containing exactly one table data field. In this 
  *       table data field is data added
  *@param data The string to be filled in the new added table data field contained in a new table row
  */
  void addTREntry(const ASCString& data);
  /**
  *@brief Appends  <br> to a string and appends to this another one. The result is returned.
  *@param oldLine The string to which <br> and toAddText are appended
  *@param toAddText The text appended to oldLine. Stands in a new line on the HTML page
  */
  ASCString addTREntryln(ASCString oldLine, ASCString toAddText) ;
  /**
  *@brief Adds an empty table data field n-times
  *@param times Defines how many empty table data fields shall be added
  */
  void addEmptyTD(int times);
  /**
  *@brief Adds a table data field and enters the string in it
  *@param data The string to be filled in the new added table data field
  */
  void addTDEntry(const ASCString& data);
  /**
  *@brief Adds table data field and enters a number in it
  *@param data A number to be filled in the new added table data field
  */
  void addTDEntry(const int data);
  /**
  *@brief Starts a table data field
  */
  void startTD();
  /**
  *@brief Ends a table data field
  */
  void endTD();
private:
  /**
  *@brief name for the anchor at pageTop
  */
  static const ASCString ANCHORTOP;

  /**
   *@brief fullFileName Consists of: path + fileName + index + extension
   */
  ASCString fullFileName;


};

/**
*@brief  Abstract representation of one building information page of a guide           
*@author Kevin Hirschmann
*/
class BuildingInfoPage: public InfoPage {
public:
  /**
  *@brief Destructor
  */
  virtual ~BuildingInfoPage();

protected:
  /**
  *@brief The BuildingType for which an info page is created. Avoids downcasts from 
          member cbt in super class InfoPage
  */
  const BuildingType& bt;
  /**
  *@brief Creates a new building info-page
  *@param contBaseType The ContainerBaseType for which the page is created  
  *@param filePath The place where the page will be stored
  *@param index The index for the page. Distinguishes it from the other pages for that ContainerBaseType
  *@param title The title displayed on the page
  *@param generator The GuideGenerator which is constructing this page
  */
  BuildingInfoPage(const BuildingType&  buildingType, ASCString filePath, ASCString index, ASCString title, BuildingGuideGen* generator);
  /**
  *@brief Adds the links to all pages of this BuildingType to the info page
  */
  virtual void addSectionLinks();

private:
 virtual void addInfoPicture(ASCString path, ASCString fileName);
};


/**
@brief  Representation of one the building information page containing the general (and most important)
*        informations
*@author Kevin Hirschmann
*/
class BuildingMainPage: public BuildingInfoPage {
public:
  /**
  *@brief  The page title
  */
  static const ASCString TITLE;
  /**
  *@brief  Destructor
  */
  virtual ~BuildingMainPage() {};
  /**
  *@brief  Constructor for creating a new BuildingMainPage.
  *@brief bt The BuildingType for which the side is created
  *@brief filePath The path to where the page is stored
  *@brief generator The BuildingGuideGen constructing the page
  */
  BuildingMainPage(const BuildingType&  bt, ASCString filePath, BuildingGuideGen* generator); 

private:
  /**
  *@brief  Assigns the BuildingType to one category. To which category is
  defined by the BuildingTyps capabilities and the order upon them in this method  
  */
  void addCategory();
  /**
  *@brief  Adds the capabilities of the BuildingType to the page
  */
  void addCapabilities();
  /**
  *@brief  Adds the main content on the right side
  */
  virtual void buildContent();


};

/**
@brief  Representation of one the building information page containing information where the building
*       can be build        
*@author Kevin Hirschmann
*/
class BuildingTerrainPage: public BuildingInfoPage {
private:
  /**
  *@brief  Adds construction costs of the building
  */
  void addConstructionCosts();
  /**
  *@brief  Adds the main content on the right side
  */
  virtual void buildContent();

public:
  /**
  *@brief  The page title
  */
  static const ASCString TITLE;
  /**
  *@brief  Destructor
  */
  ~BuildingTerrainPage() {};
  /**
  *@brief  Constructor for creating a new BuildingTerrainPage.
  *@brief bt The BuildingType for which the side is created
  *@brief filePath The path to where the page is stored
  *@brief generator The BuildingGuideGen constructing the page
  */
  BuildingTerrainPage(const BuildingType&  bt, ASCString filePath, BuildingGuideGen* generator);
};

/**
@brief  Representation of one the building information page containing the information about which cargo
        the BuildingType can contain
*@author Kevin Hirschmann
*/
class BuildingCargoPage: public BuildingInfoPage {
public:
  /**
  *@brief  Constructor for creating a new BuildingTerrainPage.
  *@brief bt The BuildingType for which the side is created
  *@brief filePath The path to where the page is stored
  *@brief generator The BuildingGuideGen constructing the page
  */
  BuildingCargoPage(const BuildingType&  cbt, ASCString filePath, BuildingGuideGen* generator);
  /**
  *@brief  Adds the main content on the right side
  */
  void buildContent();
  /**
  *@brief  The page title
  */
  static const ASCString TITLE;


};

/**
@brief  Representation of one the building information page containing the information which ressources
        the building can provide. Research points are treated as one ressource a building can provide
*@author Kevin Hirschmann
*/
class BuildingResourcePage: public BuildingInfoPage {
private:
  /**
  *@brief  Adds the main content on the right side
  */
  virtual void buildContent();
public:
  /**
  *@brief  The page title
  */
  static const ASCString TITLE;
  /**
  *@brief  Destructor
  */
  virtual ~BuildingResourcePage() {};
  /**
  *@brief  Constructor for creating a new BuildingTerrainPage.
  *@brief bt The BuildingType for which the side is created
  *@brief filePath The path to where the page is stored
  *@brief generator The BuildingGuideGen constructing the page
  */
  BuildingResourcePage(const BuildingType&  bt, ASCString filePath, BuildingGuideGen* generator);

};

/**
*@brief  Represents the page containing information about a units research properties
*@author Kevin Hirschmann
*/
class BuildingResearchPage: public BuildingInfoPage {
public:
  /**
  *@brief  Constructor for creating a new BuildingResearchPage.
  *@brief bt The BuildingType for which the side is created
  *@brief filePath The path to where the page is stored
  *@brief generator The BuildingGuideGen constructing the page
  */
  BuildingResearchPage(const BuildingType&  bt, ASCString filePath, BuildingGuideGen* generator);
  /**
  *@brief  Adds the main content on the right side
  */
  virtual void buildContent();
  /**
  *@brief  The page title
  */
  static const ASCString TITLE;
};

/**
*@brief  Abstract representation of one the vehicle information page 
*@author Kevin Hirschmann
*/
class UnitInfoPage: public InfoPage {
public:
  /**
  *@brief Creates a new unit info-page
  *@param vehicleType The VehicleType for which the page is created  
  *@param filePath The place where the page will be stored
  *@param index The index for the page. Distinguishes it from the other pages for that VehicleType
  *@param title The title displayed on the page
  *@param generator The UnitGuideGen(erator) which is constructing this page
  */
  UnitInfoPage(const VehicleType&  vehicleType, ASCString filePath, ASCString index, ASCString title, UnitGuideGen* generator);
  /**
  *@brief  Destructor
  */
  virtual ~UnitInfoPage();

protected:
  /*
  *@brief The VehicleType for which the page is constructed. Avoids downcasting 
          from memeber cbt of super class
  */
  const VehicleType& vt;
  /**
   *@brief Adds the links to all pages of this VehicleType to the info page
   */
  virtual void addSectionLinks();

private:
  virtual void addInfoPicture(ASCString path, ASCString fileName);
};

/**
*@brief  Representation the page describing the units cargo capabilities
*@author Kevin Hirschmann
*/
class UnitCargoPage: public UnitInfoPage {
public:
  /**
  *@brief  Constructor for creating a new UnitCargoPage.
  *@brief bt The VehicleType for which the side is created
  *@brief filePath The path to where the page is stored
  *@brief generator The UnitGuideGen constructing the page
  */
  UnitCargoPage(const VehicleType&  vt, ASCString filePath, UnitGuideGen* generator);
  /**
  *@brief  The page title
  */
  static const ASCString TITLE;
protected:
  /**
  *@brief  Adds the main content on the right side
  */
  virtual void buildContent();

};

/**
*@brief  Represents the page containing general (and most important informations)
*@author Kevin Hirschmann
*/
class UnitMainPage: public UnitInfoPage {
public:
  /**
  *@brief  Constructor for creating a new UnitCargoPage.
  *@brief bt The VehicleType for which the side is created
  *@brief filePath The path to where the page is stored
  *@brief generator The UnitGuideGen constructing the page
  */
  UnitMainPage(const VehicleType&  vt, ASCString filePath, UnitGuideGen* generator);

  /**
  *@brief  The page title
  */
  static const ASCString TITLE;
protected:
  /**
  *@brief  Adds the main content on the right side
  */
  virtual void buildContent();
  /**
  *@brief  Adds capabilities of the unit
  */
  void addCapabilities();
  /**
  *@brief  Adds the construction costs of the unit
  */
  void addConstructionCosts();  

};

/**
*@brief  Represents the page containing information about a units weapon systems
*@author Kevin Hirschmann
*/
class UnitWeaponPage: public UnitInfoPage {
public:
  /**
  *@brief  Constructor for creating a new UnitWeaponPage.
  *@brief vt The VehicleType for which the side is created
  *@brief filePath The path to where the page is stored
  *@brief generator The UnitGuideGen constructing the page
  */
  UnitWeaponPage(const VehicleType&  vt, ASCString filePath, UnitGuideGen* generator);
  /**
  *@brief  Adds the main content on the right side
  */
  virtual void buildContent();
  /**
  *@brief  The page title
  */
  static const ASCString TITLE;
};

/**
*@brief  Represents the page containing information about a units movement properties
*@author Kevin Hirschmann
*/
class UnitTerrainPage: public UnitInfoPage {
public:
  /**
  *@brief  Constructor for creating a new UnitTerrainPage.
  *@brief vt The VehicleType for which the side is created
  *@brief filePath The path to where the page is stored
  *@brief generator The UnitGuideGen constructing the page
  */
  UnitTerrainPage(const VehicleType&  vt, ASCString filePath, UnitGuideGen* generator);
  /**
  *@brief  Adds the main content on the right side
  */
  virtual void buildContent();
  /**
  *@brief  The page title
  */
  static const ASCString TITLE;
};

/**
*@brief  Represents the page containing information about a units construction properties
*@author Kevin Hirschmann
*/
class UnitConstructionPage: public UnitInfoPage {
public:
  /**
  *@brief  Constructor for creating a new UnitConstructionPage.
  *@brief vt The VehicleType for which the side is created
  *@brief filePath The path to where the page is stored
  *@brief generator The UnitGuideGen constructing the page
  */
  UnitConstructionPage(const VehicleType&  vt, ASCString filePath, UnitGuideGen* generator);
  /**
  *@brief  Adds the main content on the right side
  */
  virtual void buildContent();
  /**
  *@brief  The page title
  */
  static const ASCString TITLE;
};

/**
*@brief  Represents the page containing information about a units research properties
*@author Kevin Hirschmann
*/
class UnitResearchPage: public UnitInfoPage {
public:
  /**
  *@brief  Constructor for creating a new UnitResearchPage.
  *@brief vt The VehicleType for which the side is created
  *@brief filePath The path to where the page is stored
  *@brief generator The UnitGuideGen constructing the page
  */
  UnitResearchPage(const VehicleType&  vt, ASCString filePath, UnitGuideGen* generator);
  /**
  *@brief  Adds the main content on the right side
  */
  virtual void buildContent();
  /**
  *@brief  The page title
  */
  static const ASCString TITLE;
};

class TechTreePage: public InfoPage {

public:
  TechTreePage(const ContainerBaseType& cbt, ASCString fileName, ASCString filePath, GuideGenerator* generator,
  ASCString techPicPath); 
  virtual ~TechTreePage(){};  //Fehlen noch
  virtual void buildPage();
  virtual void buildContent();
  virtual void addSectionLinks();

private:  
  static const ASCString TITLE;
  ASCString techPicPath;
  
   virtual void addInfoPicture(ASCString path, ASCString fileName){};

};

#endif






