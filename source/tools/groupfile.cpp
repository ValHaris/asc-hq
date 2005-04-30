/***************************************************************************
                          groupfile.cpp  -  description
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
#include <algorithm>

#include "groupfile.h"


bool lessCat(AbstractEntry const * a, AbstractEntry const * b) {
  return ((*a) < (*b));
}

//GroupFile*******************************************************************************************************
GroupFile::GroupFile(ASCString fileN, const Category& topCat):topCategory(topCat) {
  this->fileName = fileN;
}

GroupFile::GroupFile(const GroupFile& gf): topCategory(gf.topCategory) {
  fileName = gf.fileName;

}


GroupFile::~ GroupFile() {}


void GroupFile::write() {
  ASCString output;
  output  = topCategory.toString();
  std::ofstream ofs(fileName.c_str());
  ofs <<  output << std::endl;
  ofs.close();
}

//AbstractEntry******************************************************************************************************

bool AbstractEntry::operator<(const AbstractEntry& e) const {
  return ((this->lineData.compare_ci(e.lineData)<0));
};

AbstractEntry::~ AbstractEntry() {}




//******************************************************************************************************

CategoryMember::CategoryMember(ASCString label, ASCString css, ASCString file, ASCString t):AbstractEntry(label, css), target(t) {
  lineData = label + ";" + css + ";" +"\""+ file + "\"" + " target = \"" + t + "\";\n";  

}


ASCString CategoryMember::toString() const {
  ASCString result;
  for(int i = 0; i < depth; i++) {
    result += ".";
  }
  result += lineData;
  return result;
}

void CategoryMember::updateDepth() {}
;

void CategoryMember::sort() {}

AbstractEntry* CategoryMember::clone() {
  return new CategoryMember(lineData, cssFile, file, target);
}


//*********************************************************************************************

Category::Category(ASCString label, ASCString css):AbstractEntry(label, css) {}

Category::~ Category() {
  for(std::list<AbstractEntry*>::iterator i = subEntries.begin(); i != subEntries.end(); i++) {
    delete (*i);
  }

}

Category& Category::operator=(const Category& cat) {
  if(this == &cat) {
    return (*this);
  }
  for(std::list<AbstractEntry*>::iterator i = subEntries.begin(); i != subEntries.end(); i++) {
    delete (*i);
  }
  for(std::list<AbstractEntry*>::iterator i = subEntries.begin(); i != subEntries.end(); i++) {
    subEntries.push_back( (*i)->clone() );
  }
  return (*this);
}

AbstractEntry* Category::clone() {
  return new Category(lineData, cssFile);
}


void Category::addEntry(AbstractEntry* entry) throw (ASCmsgException) {
  AbstractEntry* current = this;
  while(current != 0) {
    if( entry == current) {
      throw ASCmsgException("Circle detected in Category::addEntry");
    }
    current = current->parent;
  }
  if(entry->parent != 0){
    entry->parent->removeEntry(entry);
  }
  entry->parent = this;
  entry->depth = depth + 1; 
  subEntries.push_back(entry);
  entry->updateDepth();

};

void Category::removeEntry(AbstractEntry* entry){
  std::list<AbstractEntry*>::iterator result = std::find(subEntries.begin(), subEntries.end(), entry);   
  if(result != subEntries.end()){
    delete *result;
  }

}
void Category::updateDepth() {
  for(std::list<AbstractEntry*>::iterator i = subEntries.begin(); i != subEntries.end(); i++) {
    (*i)->depth = depth + 1;
    (*i)->updateDepth();
  }
};

void Category::sort() { 
  subEntries.sort(lessCat);
  for(std::list<AbstractEntry*>::iterator i = subEntries.begin(); i != subEntries.end(); i++) {
    (*i)->sort();
  }
};


Category::Category(const Category& cat):AbstractEntry(cat) {
  for(std::list<AbstractEntry*>::iterator i = subEntries.begin(); i != subEntries.end(); i++) {
    delete (*i);
  }
  for(std::list<AbstractEntry*>::const_iterator i = cat.subEntries.begin(); i != cat.subEntries.end(); i++) {
    subEntries.push_back((*i)->clone());
  }
}



ASCString Category::toString() const  {
  ASCString result="";
  for(int j = 0; j < depth; j++) {
    result += ".";
  }
  result += (lineData + ";" + cssFile + ";-;\n");
  for(std::list<AbstractEntry*>::const_iterator i = subEntries.begin(); i != subEntries.end(); i++) {
    result += (*i)->toString();
  }

  return result;
}



