/***************************************************************************
                          Named.h  -  description
                             -------------------
    begin                : Thu Jun 29 2000
    copyright            : (C) 2000 by frank landgraf
    email                : 
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef NAMED_H
#define NAMED_H

#ifdef __WATCOM_CPLUSPLUS__
 namespace std {};
#endif

/** 
* A simple Interface-class for a named Thing 
* @author Frank Landgraf
* @version 1.0
*/
class Named	{
	public:

		/** 
		* default constructor
		* sets the Name to NULL
		*/
		Named();
		
		/** 
		* constructor
		* sets the Name to 
		* @param pszName 
		*/
		Named(const char* pszName);
		
		/** 
		* (virtual) destructor 
		* deletes the internal buffer
		*/
		virtual ~Named();

		/** 
		* gets the Name of the Object
		*/
		const char*	getName() const;
		
		/** 
		* set the Name of the Object to pszName
		*/
		virtual void	setName(const char* pszName);

	private:
		/** 
		* internal namebuffer
		*/
		char* _name;
};

#endif // #ifndef NAMED_H