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
    Update Date:      $Date: 2007-12-08 13:19:02 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/src/core/pgfilearchive.cpp,v $
    CVS/RCS Revision: $Revision: 1.3 $
    Status:           $State: Exp $
*/

#include "physfs.h"
#include "pgfilearchive.h"
#include "pgapplication.h"
#include "pglog.h"
#include "pgfont.h"

///#include "SDL_loadso.h"
#include "physfsrwops.h"
#include <iostream>
#include <cstring>
#include "paragui.h"

Uint32 PG_FileArchive::my_instance_count = 0;
PG_SurfaceCache PG_FileArchive::my_cache;
#ifdef PG_LOAD_SDL_IMAGE_DYNAMICALLY
static void* SDL_image_obj = NULL;
#else
#include <SDL_image.h>
#endif

typedef SDL_Surface* (*IMG_Load_RW_FT)(SDL_RWops* src, int freesrc);
static IMG_Load_RW_FT IMG_Load_RW_FUNC = NULL;

PG_FileArchive::PG_FileArchive() {

	// increment instance count
	my_instance_count++;

	// First instance ? -> initialize PhysFS
	if(my_instance_count == 1) {
		if(PHYSFS_init("paragui") == 0) {
			std::cerr << "Unable to initialize PhysicsFS !" << std::endl;
			return;
		}

#ifdef PG_LOAD_SDL_IMAGE_DYNAMICALLY
		// try different names to find SDL_image
		SDL_image_obj = SDL_LoadObject(SDLIMAGE_LIB);
		if(SDL_image_obj == NULL) {
			PG_LogMSG("SDL_image not found! Only bmp images can be loaded!");
		} else {
			IMG_Load_RW_FUNC = (IMG_Load_RW_FT)SDL_LoadFunction(SDL_image_obj, "IMG_Load_RW");
			if(IMG_Load_RW_FUNC == NULL) {
				PG_LogERR("Unable to load IMG_Load_RW function. SDL_image disabled!");
				SDL_UnloadObject(SDL_image_obj);
				SDL_image_obj = NULL;
			}
		}
#else
         IMG_Load_RW_FUNC = IMG_Load_RW;
#endif
	}

}

PG_FileArchive::~PG_FileArchive() {

	if(my_instance_count == 0) {
		return;
	}

	// decrement instance count
	my_instance_count--;

	if(my_instance_count == 0) {
		Deinit();
#ifdef PG_LOAD_SDL_IMAGE_DYNAMICALLY
		if(SDL_image_obj != NULL) {
			SDL_UnloadObject(SDL_image_obj);
			SDL_image_obj = NULL;
		}
#endif
	}
}

void PG_FileArchive::Deinit() {
	PHYSFS_deinit();
}

std::string *PG_FileArchive::PathToPlatform(const std::string& path) {
	std::string *newpath;
	const char* sep = GetDirSeparator();
	std::string::size_type pos = 0, incr;
	newpath = new std::string(path);
	incr = std::strlen(sep);
	if(incr == 1 && sep[0] == '/')
		return newpath;

#ifdef __MACOS__

	while( (pos = newpath->find(":", pos)) != std::string::npos) {
#else
	while( (pos = newpath->find("/", pos)) != std::string::npos) {
#endif
		newpath->replace(pos, 1, sep);
		pos += incr;
	}
	return newpath;
}


bool PG_FileArchive::AddArchive(const std::string& arch, bool append) {
	std::string *newpath = PathToPlatform(arch);
	bool ret = (PHYSFS_addToSearchPath(newpath->c_str(),  append) != 0);
	delete newpath;
	return ret;
}

bool PG_FileArchive::RemoveArchive(const std::string& arch) {
	std::string *newpath = PathToPlatform(arch);
	bool ret = (PHYSFS_removeFromSearchPath(newpath->c_str()) != 0);
	delete newpath;
	return ret;
}

char **PG_FileArchive::EnumerateFiles(const std::string& dir) {
	return PHYSFS_enumerateFiles(dir.c_str());
}

PG_FileList* PG_FileArchive::GetFileList(const std::string& dir, const std::string& wildcard) {
	char **tempList = EnumerateFiles(dir);

	if( tempList == NULL ) {
		return NULL;
	}

	PG_FileList* retVal = new PG_FileList;

	for( char** i = tempList; *i != NULL; i++) {
		if(fnmatch(wildcard.c_str(), *i, FNM_PATHNAME) == 0) {
			retVal->push_back(std::string(*i));
		}
	}

	// Clean up.
	PHYSFS_freeList(tempList);

	return retVal;
}

bool PG_FileArchive::Exists(const std::string& filename) {
	return PHYSFS_exists(filename.c_str()) != 0;
}

bool PG_FileArchive::IsDirectory(const std::string& filename) {
	return PHYSFS_isDirectory(filename.c_str()) != 0;
}

const char* PG_FileArchive::GetDirSeparator() {
	return PHYSFS_getDirSeparator();
}

const char* PG_FileArchive::GetRealDir(const std::string& filename) {
	return PHYSFS_getRealDir(filename.c_str());
}

const char* PG_FileArchive::GetLastError() {
	return PHYSFS_getLastError();
}

const char* PG_FileArchive::GetBaseDir() {
	return PHYSFS_getBaseDir();
}

const char* PG_FileArchive::GetUserDir() {
	return PHYSFS_getUserDir();
}

const char* PG_FileArchive::GetWriteDir() {
	return PHYSFS_getWriteDir();
}

PG_File* PG_FileArchive::OpenFile(const std::string& filename, Mode mode) {
	PHYSFS_file* file = 0;
	switch(mode) {
		case READ:
			file = PHYSFS_openRead(filename.c_str());
			break;
		case WRITE:
			file = PHYSFS_openWrite(filename.c_str());
			break;
		case APPEND:
			file = PHYSFS_openAppend(filename.c_str());
			break;
	}
	if(file == NULL) {
		return NULL;
	}

	return new PG_File(file);
}

SDL_RWops* PG_FileArchive::OpenFileRWops(const std::string& filename, Mode mode) {
	SDL_RWops* file = NULL;
	switch(mode) {
		case READ:
			file = PHYSFSRWOPS_openRead(filename.c_str());
			break;
		case WRITE:
			file = PHYSFSRWOPS_openWrite(filename.c_str());
			break;
		case APPEND:
			file = PHYSFSRWOPS_openAppend(filename.c_str());
			break;
	}

	return file;
}

bool PG_FileArchive::MakeDir(const std::string& dir) {
	return PHYSFS_mkdir(dir.c_str()) == 1;
}
bool PG_FileArchive::SetWriteDir(const std::string& dir) {
	if(PHYSFS_setWriteDir(dir.c_str())) {
		return PHYSFS_addToSearchPath(dir.c_str(), 0) == 1;
	} else {
		return false;
	}
}

bool PG_FileArchive::SetSaneConfig(const std::string& organization,
                                   const std::string& appName,
                                   const std::string& archiveExt,
                                   bool includeCdRoms,
                                   bool archivesFirst) {
	return PHYSFS_setSaneConfig(organization.c_str(), appName.c_str(), archiveExt.c_str(),
	                            includeCdRoms, archivesFirst) == 1;
}


PG_DataContainer* PG_FileArchive::ReadFile(const std::string& filename) {
	PG_File *file = OpenFile(filename);

	if(!file) {
		return 0;
	}

	int size = file->fileLength();

	if(size <= 0) {
		delete file;
		return 0;
	}

	PG_DataContainer* data = new PG_DataContainer(size);

	if(file->read(data->data(), size, 1) != 1) {
		delete data;
		delete file;
		return NULL;
	}

	delete file;

	return data;
}

SDL_Surface* PG_FileArchive::LoadSurface(const std::string& filename, bool convert) {
	return LoadSurface(filename, false, 0, convert);
}

SDL_Surface* PG_FileArchive::LoadSurface(const std::string& filename, bool usekey, Uint32 colorkey, bool convert) {
	if(filename.empty() || filename == "none") {
		return NULL;
	}

	// take a look into the cache
	SDL_Surface* surface = my_cache.FindSurface(filename);

	// return the cache surface if it has been found
	if(surface != NULL) {
		my_cache.IncRef(filename);
		return surface;
	}

	surface = NULL;
	SDL_RWops *rw = OpenFileRWops(filename);

	if(rw == NULL) {
		PG_LogWRN("Unable to open '%s' !", filename.c_str());
		return NULL;
	}

	if(IMG_Load_RW_FUNC != NULL) {
		surface = IMG_Load_RW_FUNC(rw, 1);
	} else {
		surface = SDL_LoadBMP_RW(rw, 1);
	}

	if(surface == NULL) {
		PG_LogWRN("Failed to load imagedata from '%s' !", filename.c_str());
		return NULL;
	}

	if(surface == NULL) {
		PG_LogERR("Unable to load imagedata from '%s'", filename.c_str());
		PG_LogERR("PhysFS reported: '%s'", PG_FileArchive::GetLastError());
		PG_LogERR("SDL reported: '%s'", SDL_GetError());
	}

	if(usekey == true) {
		SDL_SetColorKey(surface, SDL_SRCCOLORKEY, colorkey);
	}

	if(convert) {
		SDL_Surface* tmpsrf = NULL;
		if (surface->flags & SDL_SRCALPHA)
			tmpsrf = SDL_DisplayFormatAlpha(surface);
		else
			tmpsrf = SDL_DisplayFormat(surface);

		if(tmpsrf) {
			SDL_FreeSurface(surface);
			surface = tmpsrf;
		}
	}

	// add the loaded surface to the cache and return result
	return my_cache.AddSurface(filename, surface);
}

bool PG_FileArchive::UnloadSurface(SDL_Surface* surface, bool bDeleteIfNotExists) {
	my_cache.DeleteSurface(surface, bDeleteIfNotExists);
	return true;
}

bool PG_FileArchive::RemoveAllArchives() {
	char** i = GetSearchPath();
	char** d;
	bool success = true;

	for(d = i; *d != NULL; d++) {
		if(!RemoveArchive(*d)) {
			PG_LogWRN("Unable to remove '%s' from searchpath!", *d);
			success = false;
		}
	}

	FreeList(i);
	return success;
}

char** PG_FileArchive::GetSearchPath() {
	return PHYSFS_getSearchPath();
}

PG_FileList* PG_FileArchive::GetSearchPathList() {
	char **tempList = PHYSFS_getSearchPath();

	if( tempList == NULL ) {
		return NULL;
	}

	PG_FileList* retVal = NULL;

	// Scan through to get the length of the listing to get the proper vector size.
	Uint32 size = 0;
	for(; tempList[ size ] != NULL; ++size) {}

	// Now we're ready to initialize everything.
	retVal = new std::vector< std::string >;
	retVal->reserve( size );
	for( Uint32 i = 0; i < size; ++i ) {
		retVal->push_back(std::string(tempList[ i ]));
	}

	// Clean up.
	PHYSFS_freeList(tempList);

	return retVal;
}

void PG_FileArchive::FreeList(void* list) {
	PHYSFS_freeList(list);
}

void PG_FileArchive::EnableSymlinks(bool followSymlinks) {
	PHYSFS_permitSymbolicLinks(followSymlinks);
}

/*
 * Local Variables:
 * c-basic-offset: 8
 * End:
 */
