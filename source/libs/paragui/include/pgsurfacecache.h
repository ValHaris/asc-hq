/*
    ParaGUI - crossplatform widgetset
    Copyright (C) 2000,2001,2002  Alexander Pipelka
 
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
 
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.
 
    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 
    Alexander Pipelka
    pipelka@teleweb.at
 
    Last Update:      $Author: mbickel $
    Update Date:      $Date: 2006-02-15 21:30:16 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pgsurfacecache.h,v $
    CVS/RCS Revision: $Revision: 1.1.2.1 $
    Status:           $State: Exp $
*/

/** \file pgsurfacecache.h
    Headerfile for for the PG_SurfaceCache class.
  */

#ifndef PG_SURFACECACHE_H
#define PG_SURFACECACHE_H

#include "paragui.h"
#include "pgcolor.h"
#include "pgdraw.h"
#include <string>

typedef struct {
	int refcount;
	SDL_Surface* surface;
	std::string key;
}
pg_surface_cache_t;


/** @short A class for caching SDL_Surface images.
    
    To reduce memory consumption, paragui classes use this class to cache
    their surfaces. The class stores an internal counter for each surface that
    is decreased every time the surface is deleted. It is proposed to use this
    interface if you add a new class to ParaGui.
  */

class DECLSPEC PG_SurfaceCache {
public:

	PG_SurfaceCache();

	virtual ~PG_SurfaceCache();

	/** Create a fairly unique key for a given surface.
	    
	  @param    key         the key will be returned here
	  @param    w           width of the surface
	  @param    h           height of the surface
	  @param    gradient    is added to the key if non-zero
	  @param    background  the surface for the key
	  @param    bkmode      see explanation
	  @param    blend       see explanation

	  This function has the task of creating a unique key to a given
	  surface. This is done by simply writing all parameters one after
	  the other, but the implementation may change in future versions.
	  Therefore, the parameters are not really neccessary. However, to ensure
	  that the created keys are unique, you should use the parameters.
	  */
	void CreateKey(std::string &key, Uint16 w, Uint16 h,
	               PG_Gradient* gradient, SDL_Surface* background,
	               PG_Draw::BkMode bkmode, Uint8 blend);

	/** Find the surface to a given key .

	    @param  key the key whose surface is searched
	    @return the surface or NULL if no surfac was found
	  */
	SDL_Surface* FindSurface(const std::string &key);

	/** Add a surface to the list.
	    
	    @param  key     the key associated with the surface
	    @param  surface the surface you want to add to the cache
	    @return the surface from the cache

	    \warning If you add a surface whose key already exists and which is
	    different from the surface in the cache, your surface will be quietly
	    deleted. So NEVER touch a surface again after you have added it to the
	    cache. ALWAYS use the returned pointer instead.

	    This behaviour has a slight drawback if you add a surface the
	    second time, because then the surface is not deleted...
	  */
	SDL_Surface* AddSurface(const std::string &key, SDL_Surface* surface);

	/** Deletes a surface from the list

	    @param  surface             the surface to be deleted
	    @param  bDeleteIfNotExists  if set to true, the surface is deleted
	                                when the reference counter reaches 0

	    This function reduces the reference counter of the surface. If the
	    counter reaches 0 and the bool parameter is set, the surface is freed.
	  */
	void DeleteSurface(SDL_Surface* surface, bool bDeleteIfNotExists = true);

	/** Increase the reference counter of a surface
	    
	    @param  key the key of the surface
	  */
	void IncRef(const std::string &key);

	/** Removes all surfaces from the cache */
	void Cleanup();

private:

	DLLLOCAL pg_surface_cache_t* FindByKey(const std::string &key);

	DLLLOCAL pg_surface_cache_t* FindBySurface(SDL_Surface* surface);

	void* my_surfacemap;
	void* my_surfacemap_index;
};

#endif	// SURFACECACHE_H
