/***************************************************************************
                          groupfile.h  -  description
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
#ifndef groupfileH
#define groupfileH


#include <list>
#include <fstream>
#include <iostream>
#include "../errors.h"

#include "../ascstring.h"



//using namespace std;


/**
*@brief Abstract representation of an entry in a group-file. Composite-Pattern
*@author Kevin Hirschmann
*/
class AbstractEntry {
  friend class Category;
public:
  /**
  *@brief Destructor
  */
  virtual ~AbstractEntry();
  /**
  *@brief Generates the string-representation of this entry (including all its subentries)
  */
  virtual ASCString toString() const = 0;
  /**
  *@brief Sorts the subentries of this entry in alphabetic order
  */
  virtual void sort()=0;
  /**
  *@brief Adds a new subpoint (with all its entries)
  *@param entry The new entry is appended behind all previous added entries
  *@attention Add only pointer to heap objects. Memory management is delgated to
  *superCategory
  *@code
  * {
  * Category set(s->name, cssFile);
  * Category* hqCat = new Category(HQ, cssFile);
  * Category* facCat = new Category(FACTORY, cssFile);
  * set.addEntry(hqCat);
  * set.addEntry(facCat);   
  * CategoryMember* dataEntry = new CategoryMember(bt->name.toUpper(), cssFile, fileLink, TARGET);
  * //At block end  set is deleted and automatically all added entries
  * //which are here hqCat, facCat, dataEntry
  * }
  *@endcode
  */
  virtual void addEntry(AbstractEntry* entry) throw (ASCmsgException) = 0;  
  /**
  *@brief Removes the AbstractEntry (if it is a sub-entry)
  */
  virtual void removeEntry(AbstractEntry* e) = 0;
  /**
  *@brief Updates the depth of all subentries 
  */
  virtual void updateDepth() = 0;
  /**
  *@brief Defines an order upon AbstractEntry. The order is equivalent to the alphabetic order of member lineData
  */
  bool operator<(const AbstractEntry& e) const;
  /**
  *@brief Retrieves the depth of this entry. The topmost entry has a depth of 0.
  *@return An int representing the depth of on which this entry resides.
  */
  int getDepth() const {
    return depth;
  }

protected:
  /**
  *@brief Full name of the cssFile
  */
  ASCString cssFile;
  /**
  *@brief The text line of this entry
  */
  ASCString lineData;

  /**
  *@brief A reference to the parent
  */
  AbstractEntry* parent;
  /**
  *@brief The depth of this entry
  */
  int depth;

  /**
  *@brief Returns a copy of this element with copies of all its subelements.
  *       The caller is responsible for deletion
  */
  virtual AbstractEntry* clone() = 0;

  /**
  *@brief Standard-Constructor
  */
  AbstractEntry(): depth(0), parent(0) {}
  AbstractEntry(ASCString data, ASCString css): lineData(data), cssFile(css), depth(0), parent(0) {};


};


typedef std::list<AbstractEntry*> AbstractEntryList;


/**
*@brief Represents a category entry in a GroupFile
*@author Kevin Hirschmann
*/
class Category: public AbstractEntry {
public:

  /**
  *@brief Constructor for creating a new Category in a GroupFile
  *@param label The label of the category
  *@param css The full path to the cssFile
  */
  Category(ASCString label, ASCString css);
  /**
  *@brief Copy-Constructor
  */
  Category(const Category& cat);
  /**
  *@brief Destructor
  */
  virtual ~Category();

  /**
  *@brief Adds a new subpoint (with all its entries)
  *@param entry The new entry is appended behind all previous added entries
  *@attention Add only pointer to heap objects. Memory management is delgated to
  *superCategory
  *@code
  * {
  * Category set(s->name, cssFile);
  * Category* hqCat = new Category(HQ, cssFile);
  * Category* facCat = new Category(FACTORY, cssFile);
  * set.addEntry(hqCat);
  * set.addEntry(facCat);   
  * CategoryMember* dataEntry = new CategoryMember(bt->name.toUpper(), cssFile, fileLink, TARGET);
  * //At block end  set is deleted and automatically all added entries
  * //which are here hqCat, facCat, dataEntry
  * }
  *@endcode
  */
  virtual void addEntry(AbstractEntry* entry) throw (ASCmsgException);

  virtual void removeEntry(AbstractEntry* entry);
  /**
  *@brief Generates the string-representation of this entry (including all its subentries)
  */
  virtual ASCString toString() const;
  /**
  *@brief Updates the depth of all subentries 
  */
  virtual void updateDepth();
  /**
  *@brief Sorts the subentries of this entry in alphabetic order
  */
  virtual void sort();

  /**
  *@brief Assignment operator. Creates a deep copy
  */
  Category& operator=(const Category&);
  /**
  *@brief Returns a copy of this element with copies of all its subelements.
  *       The caller is responsible for deletion
  */
  virtual AbstractEntry* clone();


private:
  /**
  *@brief A vector containing pointers to all subentries
  */
  AbstractEntryList subEntries;

};

/**
  *@brief A leaf in the category entries
  */
class CategoryMember: public AbstractEntry {
public:
  /**
  *@brief Constructor for creating a link in the menu to a HTML-page 
  *@param label The label for the menu entry
  *@param css Full path to a css file
  *@param file the full name to the HTML page
  *@param target The target 
  */
  CategoryMember(ASCString label, ASCString css, ASCString file, ASCString target);
  /**
  *@brief Generates the string-representation of this entry
  */
  virtual ASCString toString() const;
  /**
  *@brief Adds a new subpoint (with all its entries)
  *@param entry The new entry is appended behind all previous added entries
  *@attention Add only pointer to heap objects. Memory management is delgated to
  *superCategory
  *@code
  * {
  * Category set(s->name, cssFile);
  * Category* hqCat = new Category(HQ, cssFile);
  * Category* facCat = new Category(FACTORY, cssFile);
  * set.addEntry(hqCat);
  * set.addEntry(facCat);   
  * CategoryMember* dataEntry = new CategoryMember(bt->name.toUpper(), cssFile, fileLink, TARGET);
  * //At block end  set is deleted and automatically all added entries
  * //which are here hqCat, facCat, dataEntry
  * }
  *@endcode
  */
  virtual void addEntry(AbstractEntry* entry) throw (ASCmsgException) {
    throw ASCmsgException("Exception: Trying to add an entry to a CategoryMember");
  } //Exception
  
  virtual void removeEntry(AbstractEntry* entry){} //Exception
  /**
  *@brief Updates the depth of all subentries 
  */
  virtual void updateDepth();
  /**
  *@brief Does nothing
  */
  virtual void sort();

protected:
  /**
  *@brief Returns a copy of this element with copies of all its subelements.
  *       The caller is responsible for deletion
  */
  virtual AbstractEntry* clone();
private:
  ASCString target;
  /**
  *@brief The full name to the HTML file
  */
  ASCString file;

};

/**
*@brief Represents a group-file for menu generation by menugen.
*@author Kevin Hirschmann
*/
class GroupFile {
private:
  const Category& topCategory;
  ASCString fileName;
  static int count;
public:

  /**
  *@brief Constructor
  *@param fileName Defines the name (including path) to write to
  *@param topCat The top-category (topmost menu point)
  */
  GroupFile(ASCString fileName, const Category& topCat);
  /**
  *@brief Copy-Constructor
  */
  GroupFile(const GroupFile& gf);

  /**
  *@brief Destructor
  */
  ~GroupFile();

  /**
  *@brief Writes the output to the destination defined in the constructor parameter fileName  
  */
  void write();
};


#endif

