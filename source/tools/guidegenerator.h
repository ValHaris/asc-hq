/***************************************************************************
                          guidegenerator.h  -  description
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


#ifndef guidegeneratorH
#define guidegeneratorH

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

#include "groupfile.h"


#define VehicleType Vehicletype
#define RELATIVEIMGPATH "./"
#define RELATIVEBUILDINGSPATH "./"

typedef map<int, ASCString> Int2String;
//typedef map<int, Category*> GroupFileEntriesMap;
typedef set<ASCString> StringSet;
typedef set<int> IntSet;


class InfoPageUtil{

public:

static bool diffMove( const ASCString src, const ASCString dst );

static ASCString getTmpPath();

static void copyFile(const ASCString src, const ASCString dst);

static bool equalFiles(const ASCString src, const ASCString dst);

static void updateFile(ASCString fileName, ASCString exportPath);
};


/**
*@brief Abstract base class for building and unit guide generator
*@author Kevin Hirschmann
*/
class GuideGenerator {
public:
  /**
  *@brief Constructor for creating a new GuideGenerator
  *@param filePath The path to which the generated files are stored
  *@param menuCSSPath  The full path to the used css-file for the menu
  *@param setID The set of which the guide is created, if set to 0 guides for all sets will be created
  *@param mainCSSPath  The full path to the used css-file for the pages
  *@param createImg Determines if images shall be created.
  */

  GuideGenerator(ASCString filePath, ASCString menuCSSPath, int setID, ASCString mainCSSPath, ASCString techIDs, bool createImg, ASCString relMenuPath, bool upload = false, int imageSize = 0);

  /**
  *@brief Destructor
  */
  virtual ~GuideGenerator() {};
  /**
  *@brief Starts the guide generation
  *       
  */
  virtual void processSubjects() = 0;
  /**
  *@brief Returns the path to the image of subject with the corresponding ID 
  *@param ID The id of the subject to which the image path is to retrieve
  *@return  The file path of the image of the subject with corresponding ID
  */
  const ASCString& getImagePath(int ID);
  /**
  *@brief Returns the path to the css-File used by generated pages
  *        void diffCopy( const ASCString& src, const ASCString& dst );
  */
  const ASCString& getMainCSSPath() const;  
  /*
  *@brief Gets the width for the unit image
  */
  int getImageWidth() const{
    return imageWidth;
  }
  /*
  *@brief Tests if the generator is running in upload mode
  */
  bool generatesUpload() const{
    return createUpload;
  }
  /*
  *@brief Gets the filePath of the output
  */
  const ASCString getFilePath() const{
    return filePath;
  }
  
  const vector<IntRange>& getTechTreeIDs() const{
    return techTreeIDs;
  }

protected:
  /**
  *@brief A map containing: subject ids -> path to the corresponding image
  */
  Int2String graphicRefs;
  /**
  *@brief The path to which the pages of the guide are generated
  */
  ASCString filePath;
  /**
  *@brief The full path to the used css-file for the menu
  */
  ASCString menuCSSFile;
  /**
  *@brief The full path to the used css-file for the pages
  */
  ASCString mainCSSFile;
   
  /**
  *@brief Determines if images should be created.
  */
  bool createImg;
  /**
  *@brief Specifies the id of the set for which a guide is generated
  */
  int setID;
  /**
  *@brief imageWidth Determines the width of the unit image in pix
  */
  int imageWidth;
  /**
  *@brief createUpload Defines if an upload specific output shall be created
  *       This upload is placed in a special directory which contains only
  *       data which differs from previous generated data
  */
  bool createUpload;
  /**
  *@brief techTreeID Selects which branch of a technology is shown.
  *Each unit/builing can have multiple ways to research them
  *E.g. there is one branch for pbp and another for the asc campain
  */
  vector<IntRange> techTreeIDs;
  /**
  *@brief Default-Constructor
  */    
  GuideGenerator() {};  
    
  /**
  *@brief Determines the relative path from the menu to the single pages.
  *       Is prefixed to filelink in GroupFile
  */
  ASCString relMenuPath;
private:

};

  /**
  *@brief Class for generating the building guide
  *@author Kevin Hirschmann
  */
class BuildingGuideGen: public GuideGenerator {
public:
  /**
  *@brief Constructor for creating a new BuildingGuideGen(erator)
  *@param filePath The path to which the generated files are stored
  *@param cssPath  The full path to the used css-file
  *@param setID The set of which the guide is created, if set to 0 guides for all sets will be created
  *@param createImg Determines if images shall be created.
  *@param buildingsUnique Determines if the pages for a building shall be created only once or for each
  *       of its occurence in the set (different directions, different weather) 
  */
  BuildingGuideGen(ASCString filePath, ASCString menuCSSPath, int setID, ASCString mainCSSPath, ASCString techIDs, bool createImg,  ASCString relMenuPath, bool buildingsUnique = false, bool upload = false, int imageSize = 0);
  /*
  *@brief Destructor
  */
  virtual ~BuildingGuideGen();
  /*
  *@brief Creates a guide (aggregated of several specialistaions of BuildingInfoPage)
  */
  virtual void processSubjects();
  /**
  *@brief Creates the file name for the pages of a buildingtype (no index, no extension)
  **A full fileName is: filePath + constructFileName(...) + [INDEX] + extension
  */
  static ASCString constructFileName(const BuildingType& bt);  

private:
  /**
  *@brief Default-Constructor
  */
  BuildingGuideGen() {};
  /*
  *@brief Creates the group files
  */
  virtual void generateCategories() const;
  /**
  *@brief Creates the guide pages for one BuildingType
  *@param bt The building type for which the guide pages are created
  */
  void processBuilding(const BuildingType&  bt);
  
  /**
  *@brief The APPENDIX is used to distinguish between Building, Unit etc. files
  *       Necessary, because the file names are created from the ids which are only unique in their domain (e.g. buildingTypes)
  */
  static const ASCString APPENDIX;
  /**
  *@brief Determines if the pages for a building shall be created only once or for each
  *       of its occurence in the set (different directions, different weather)   
  */
  bool buildingsUnique;
  
  /**
  *@brief 
  *       
  */
  StringSet buildingNames;
  
  IntSet processedBuildingIds;
};

  /**
  *@brief Class for generating the unit guide
  *@author Kevin Hirschmann
  */
class UnitGuideGen: public GuideGenerator {
public:
  /**
  *@brief Constructor for creating a new BuildingGuideGen(erator)
  *@param filePath The path to which the generated files are stored
  *@param cssPath  The full path to the used css-file
  *@param setID The set of which the guide is created, if set to 0 guides for all sets will be created
  *@param createImg Determines if images shall be created.
  *@param imageSize Determines the width of the unit image in pix
  */
  UnitGuideGen(ASCString filePath, ASCString menuCSSPath, int setID, ASCString mainCSSPath, ASCString techIDs, bool createImg,  ASCString relMenuPath, bool upload = false, int imageSize = 0);
  /**
  *@brief Destructor
  */
  virtual ~UnitGuideGen();
  /*
  *@brief Creates a guide (aggregated of several specialistaions of UnitInfoPage)
  */
  virtual void processSubjects();
  /**
  *@brief Creates the file name for the pages of a VehicleType (no index, no extension)
  *A full fileName is: filePath + constructFileName(...) + [INDEX] + extension
  */
  static ASCString constructFileName(const VehicleType& vt);
  
  private:  
  /**
  *The label for the category trooper
  */
  static const ASCString TROOPER;
  /**
  *The label for the category sea units
  */
  static const ASCString SEAUNIT;
  /**
  *The label for the category air units
  */
  static const ASCString AIRUNIT;
  /**
  *The label for the category turret units
  */
  static const ASCString TURRETUNIT;
  /**
  *The label for the category ground unit
  */
  static const ASCString GROUNDUNIT;
  

  /**
  *brief Default-Constructor
  */
  UnitGuideGen() {};
  /**
  *@brief Creates the guide pages for one VehicleType
  *@param vt The vehicle type for which the guide pages are created
  */  
  void processUnit(const VehicleType&  vt);
  /**
  *@brief Creates the group files
  */
  virtual void generateCategories() const;
  
  /**
  *@brief The APPENDIX is used to distinguish between Building, Unit etc. files
  *       Necessary, because the file names are created from the ids which are only unique in their domain (e.g. buildingTypes)
  */
  static const ASCString APPENDIX;
};


  /**
  *@brief Class for converting pcx-files
  */
class ImageConverter {
public:
  /**
  *@brief Standard-Constructor
  */
  ImageConverter();
  /**
  *@brief Creates a picture for a BuildingType
  *@param bt The building type for which the pic is created
  *@param filePath The path to the place where the pic will be stored
  */
  ASCString createPic(const BuildingType&  bt,  ASCString filePath);
  /**
  *@brief Creates a picture for a VehicleType
  *@param vt The vehicle type for which the pic is created
  *@param filePath The path to the place where the pic will be stored
  */
  ASCString createPic(const VehicleType&  vt,  ASCString filePath);

  /**
  *@brief Constructs the full path to the image of a BuildingType    
  *@param bt The building type for which an image is created
  *@param filePath The path where the images are stored
  *@return The full Path to the image: path + fileName + extension
  */
  ASCString constructImgPath(const BuildingType&  cbt, const ASCString filePath);
  
  /**
  *@brief Constructs the full path to the image of a VehicleType  
  *@param vt The building type for which an image is created
  *@param filePath The path where the images are stored
  *@return The full Path to the image: path + fileName + extension
  */
  ASCString constructImgPath(const VehicleType&  cbt, const ASCString filePath);
  
  ASCString constructImgFileName(const VehicleType&  vt);
  
  ASCString constructImgFileName(const BuildingType&  vt);
  
  
  
private:
  /**
  *@brief Converts a pcx-file of a building to a gif
  *@param fileName The file name for the created pic
  *@param filePath The place where the image will be stored
  *@param xsize The width of the created image
  *@param ysize The height of the created image
  */
  void convert(const ASCString&  fileName, ASCString filePath , int xsize = fieldsizex, int ysize = fieldsizey); 


};
#endif





