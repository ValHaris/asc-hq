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
    Update Date:      $Date: 2009-04-18 13:48:40 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/src/themes/themeloader.cpp,v $
    CVS/RCS Revision: $Revision: 1.3 $
    Status:           $State: Exp $
*/

#include "paragui.h"
#include "pgcolor.h"

#include "theme_priv.h"
#include "pglog.h"
#include "pgfilearchive.h"
#include "pgdraw.h"

#include <string>
#include <expat.h>
#include <iostream>

// Expat UNICODE workaround - copied from xmltchar.h (Expat dist) - Inserted by Ales Teska
#ifdef XML_UNICODE
#ifndef XML_UNICODE_WCHAR_T
#error UNICODE version requires a 16-bit Unicode-compatible wchar_t=20
#endif
#define T(x) L ## x
#define tcscmp wcscmp
#else /* not XML_UNICODE */
#define T(x) x
#define tcscmp strcmp
#endif

typedef struct _PARSE_INFO {
	int depth;
	int mode;
	THEME_THEME* theme;
	std::string str_currentWidget;
	std::string str_currentObject;
	THEME_WIDGET* p_currentWidget;
	THEME_OBJECT* p_currentObject;
	std::string themename;
}
PARSE_INFO;


#define THEMEMODE_NONE			0
#define THEMEMODE_THEME			1
#define THEMEMODE_WIDGET		2
#define THEMEMODE_OBJECT		3

#define THEME_SUFFIX ".theme"

#define BUFFSIZE	8192

static char* buff;

void parseGlobProps(PARSE_INFO* info, const XML_Char* name, const XML_Char** atts) {

	if(strcmp(name, "theme") == 0) {
		// create new theme template
		info->theme = new THEME_THEME;
	} else {
		std::cerr << "UNKNOWN PROP: " << name << std::endl;
	}
}

void parseThemeProps(PARSE_INFO* info, const XML_Char* prop, const XML_Char** atts) {
	const XML_Char* val = atts[1];
	int i=0;

	if(tcscmp(T(prop), T("title")) == 0) {
		info->theme->title = val;
	} else if (tcscmp(T(prop), T("description")) == 0) {
		info->theme->description = val;
	} else if (tcscmp(T(prop), T("author")) == 0) {
		info->theme->author = val;
	} else if (tcscmp(T(prop), T("email")) == 0) {
		info->theme->email = val;
	} else if (tcscmp(T(prop), T("widget")) == 0) {
		THEME_WIDGET* widget = new THEME_WIDGET;
		widget->type = "";
		info->p_currentWidget = widget;
		info->mode = THEMEMODE_WIDGET;
	} else if(tcscmp(T(prop), T("font")) == 0) {
		THEME_FONT* font = new THEME_FONT;
		font->size = 14;

		for(i=0; atts[i]; i += 2) {
			if(tcscmp(T(atts[i]), T("name")) == 0) {
				font->name = atts[i+1];
			} else if(tcscmp(T(atts[i]), T("value")) == 0) {
				font->value = atts[i+1];
			} else if(tcscmp(T(atts[i]), T("size")) == 0) {
				font->size = atoi(atts[i+1]);
			} else {
				std::cerr << "UNKNOWN FONT ATTRIBUTE: " << atts[i] << std::endl;
			}
		}

		info->theme->defaultfont = font;
	} else {
		std::cerr << "UNKNOWN THEME ATTRIBUTE: " << prop << std::endl;
	}

}

void parseWidgetProps(PARSE_INFO* info, const XML_Char* prop, const XML_Char** atts) {
	std::string val=atts[1];

	if(tcscmp(T(prop), T("type")) == 0) {
		info->p_currentWidget->type = val;
		info->theme->widget[val] = info->p_currentWidget;
	} else if(tcscmp(T(prop), T("object")) == 0) {
		THEME_OBJECT* object = new THEME_OBJECT;
		object->type = "";
		object->name = "";
		info->p_currentObject = object;
		info->mode = THEMEMODE_OBJECT;
	} else {
		std::cerr << "UNKNOWN WIDGET ATTRIBUTE: " << prop << std::endl;
	}
}

void parseObjectProps(PARSE_INFO* info, const XML_Char* prop, const XML_Char** atts) {
	std::string val=atts[1];
	int i;
	THEME_OBJECT* object = info->p_currentObject;

	//
	//	TYPE
	//

	if(tcscmp(T(prop), T("type")) == 0) {
		object->type = val;
	}

	//
	//	NAME
	//

	else if(tcscmp(T(prop), T("name")) == 0) {
		object->name = val;
		info->p_currentWidget->object[object->name] = object;
	}

	//
	//	FILENAME
	//

	else if(tcscmp(T(prop), T("filename")) == 0) {
		THEME_FILENAME* filename = new THEME_FILENAME;
		filename->hasColorKey = false;

		for(i=0; atts[i]; i += 2) {
			if(tcscmp(T(atts[i]), T("name")) == 0) {
				filename->name = atts[i+1];
			} else if(tcscmp(T(atts[i]), T("value")) == 0) {
				filename->value = atts[i+1];
			} else if(tcscmp(T(atts[i]), T("colorkey")) == 0) {
				sscanf(atts[i+1], "0x%08x", (unsigned int*)(&filename->colorkey));
				filename->hasColorKey = true;
			} else {
				std::cerr << "UNKNOWN FILENAME ATTRIBUTE: " << atts[i] << std::endl;
			}
		}

		// load the image file
		filename->surface = PG_FileArchive::LoadSurface(filename->value, true);

		if(filename->surface == NULL) {
			delete filename;
			return;
		}

		// set the colorkey (if there is any)
		if(filename->hasColorKey && filename->surface) {
			PG_Color c = filename->colorkey;
			Uint32 key = c.MapRGB(filename->surface->format);
			SDL_SetColorKey(filename->surface, SDL_TRUE, key);
		}
		object->filename[filename->name] = filename;
	}

	//
	//	FONT
	//

	else if(tcscmp(T(prop), T("font")) == 0) {
		THEME_FONT* font = new THEME_FONT;
		font->size = 14;

		for(i=0; atts[i]; i += 2) {
			if(tcscmp(T(atts[i]), T("name")) == 0) {
				font->name = atts[i+1];
			} else if(tcscmp(T(atts[i]), T("value")) == 0) {
				font->name = atts[i+1];
				font->value = atts[i+1];
			} else if(tcscmp(T(atts[i]), T("size")) == 0) {
				font->size = atoi(atts[i+1]);
			} else {
				std::cerr << "UNKNOWN FONT ATTRIBUTE: " << atts[i] << std::endl;
			}
		}

		object->font = font;
	}

	//
	//	PROPERTY
	//

	else if(tcscmp(T(prop), T("property")) == 0) {
		THEME_PROPERTY* property = new THEME_PROPERTY;

		for(i=0; atts[i]; i += 2) {
			if(tcscmp(T(atts[i]), T("name")) == 0) {
				property->name = atts[i+1];
			} else if(tcscmp(T(atts[i]), T("value")) == 0) {
				if(tcscmp(T(atts[i+1]), T("TILE")) == 0) {
					property->value = PG_Draw::TILE;
				} else if(tcscmp(T(atts[i+1]), T("STRETCH")) == 0) {
					property->value = PG_Draw::STRETCH;
				} else if(tcscmp(T(atts[i+1]), T("3TILEH")) == 0) {
					property->value = PG_Draw::TILE3H;
				} else if(tcscmp(T(atts[i+1]), T("3TILEV")) == 0) {
					property->value = PG_Draw::TILE3V;
				} else if(tcscmp(T(atts[i+1]), T("9TILE")) == 0) {
					property->value = PG_Draw::TILE9;
				} else {
					property->value = atoi(atts[i+1]);
				}
			} else {
				std::cerr << "UNKNOWN PROPERTY ATTRIBUTE: " << atts[i] << std::endl;
			}
		}
		object->property[property->name] = property;
	}

	//
	//	COLOR
	//

	else if(tcscmp(T(prop), T("color")) == 0) {
		THEME_PROPERTY* property = new THEME_PROPERTY;

		for(i=0; atts[i]; i += 2) {
			if(tcscmp(T(atts[i]), T("name")) == 0) {
				property->name = atts[i+1];
			} else if(tcscmp(T(atts[i]), T("value")) == 0) {
				unsigned int value;
				sscanf(atts[i+1], "0x%08x", &value);
				property->value = value;
			} else {
				std::cerr << "UNKNOWN COLOR ATTRIBUTE: " << atts[i] << std::endl;
			}
		}
		object->property[property->name] = property;
	}

	//
	//	GRADIENT
	//

	else if(tcscmp(T(prop), T("gradient")) == 0) {
		THEME_GRADIENT* gradient = new THEME_GRADIENT;
		Uint32 c;
		std::string val;

		for(i=0; atts[i]; i += 2) {

			if(tcscmp(T(atts[i]), T("name")) == 0) {
				gradient->name = atts[i+1];
			} else if(tcscmp(T(atts[i]), T("color0")) == 0) {
				val = atts[i+1];
				sscanf(val.c_str(), "0x%08x", &c);
				gradient->colors[0] = c;
			} else if(tcscmp(T(atts[i]), T("color1")) == 0) {
				val = atts[i+1];
				sscanf(val.c_str(), "0x%08x", &c);
				gradient->colors[1] = c;
			} else if(tcscmp(T(atts[i]), T("color2")) == 0) {
				val = atts[i+1];
				sscanf(val.c_str(), "0x%08x", &c);
				gradient->colors[2] = c;
			} else if(tcscmp(T(atts[i]), T("color3")) == 0) {
				val = atts[i+1];
				sscanf(val.c_str(), "0x%08x", &c);
				gradient->colors[3] = c;
			} else {
				std::cerr << "UNKNOWN PROPERTY ATTRIBUTE: " << atts[i] << std::endl;
			}
		}

		/*for(i=0; i<4; i++) {
			gradient->gradient.colors[i] = gradient->color[i];
		}*/

		object->gradient[gradient->name] = gradient;
	}

	//
	//	STRING
	//
	else if(tcscmp(T(prop), T("string")) == 0) {
		THEME_STRING* str = new THEME_STRING;

		for(i=0; atts[i]; i += 2) {

			if(tcscmp(T(atts[i]), T("name")) == 0) {
				str->name = atts[i+1];
			} else if(tcscmp(T(atts[i]), T("value")) == 0) {
				str->value = atts[i+1];
			} else {
				std::cerr << "UNKNOWN STRING ATTRIBUTE: " << atts[i] << std::endl;
			}
		}

		object->strings.insert(info->p_currentObject->strings.end(), str);
	}

	//
	//	UNKNOWN OBJECT ATTRIBUTE
	//

	else {
		std::cerr << "UNKNOWN OBJECT ATTRIBUTE: " << prop << std::endl;
	}
}


void handlerStart(void* userData, const XML_Char *name, const XML_Char** atts) {
	PARSE_INFO* info = (PARSE_INFO*)userData;

	info->depth++;

	switch(info->mode) {
		case THEMEMODE_NONE:
			parseGlobProps(info, name, atts);
			info->mode = THEMEMODE_THEME;
			break;

		case THEMEMODE_THEME:
			parseThemeProps(info, name, atts);
			break;

		case THEMEMODE_WIDGET:
			parseWidgetProps(info, name, atts);
			break;

		case THEMEMODE_OBJECT:
			parseObjectProps(info, name, atts);
			break;
	}
}

void handlerEnd(void* userData, const XML_Char* name) {
	PARSE_INFO* info = (PARSE_INFO*)userData;

	if(tcscmp(T(name), T("object")) == 0) {
		info->mode = THEMEMODE_WIDGET;
	} else if(tcscmp(T(name), T("widget")) == 0) {
		info->mode = THEMEMODE_THEME;
	} else if(tcscmp(T(name), T("theme")) == 0) {
		info->mode = THEMEMODE_NONE;
	}

	info->depth--;
}

PG_Theme* PG_Theme::Load(const std::string& xmltheme) {
	std::string filename;
	buff = new char[BUFFSIZE];

	// create new parse info
	PARSE_INFO info;

	// init parseinfo
	info.theme = NULL;
	info.p_currentWidget = NULL;
	info.p_currentObject = NULL;
	info.depth = 0;
	info.mode = THEMEMODE_NONE;

	filename = xmltheme;

	// check if we have a compressed themefile somewhere
	filename = xmltheme + ".zip";

	// and add it to the searchpath
	if(PG_FileArchive::Exists(filename)) {
		const char* path = PG_FileArchive::GetRealDir(filename);
		char sep = PG_FileArchive::GetDirSeparator()[0];

		std::string fullpath = (std::string)path;
		if(fullpath[fullpath.size()-1] != sep) {
			fullpath += sep;
		}
		fullpath += filename;

		bool rc = PG_FileArchive::AddArchive(fullpath);
		if(rc) {
			PG_LogMSG("added '%s' to the searchpath", fullpath.c_str());
		} else {
			PG_LogWRN("failed to add '%s'", fullpath.c_str());
			PG_LogWRN("%s", PG_FileArchive::GetLastError());
		}
	}

	// try to open the theme

	filename = xmltheme + THEME_SUFFIX;
	if(!PG_FileArchive::Exists(filename)) {
		PG_LogERR("theme '%s' not found !", filename.c_str());
		return NULL;
	}

	info.themename = xmltheme;

	// create a parser
	XML_Parser p = XML_ParserCreate(NULL);

	// set userdata (parseinfo)
	XML_SetUserData(p, (void*)&info);
	XML_SetElementHandler(p, handlerStart, handlerEnd);

	// create an input-stream

	PG_File* file = PG_FileArchive::OpenFile(filename);

	if(!file) {
		XML_ParserFree(p);
		delete[] buff;
		return NULL;
	}

	// do the parsing
	while (!file->eof()) {
		int done;

		int n = file->read(buff, BUFFSIZE);
		done = file->eof();

		if (! XML_Parse(p, buff, n, done)) {
			PG_LogERR("Parse error at line %i:", XML_GetCurrentLineNumber(p));
			PG_LogERR("%s", XML_ErrorString(XML_GetErrorCode(p)));
			XML_ParserFree(p);
			delete[] buff;
			return NULL;
		}

		if(done) {
			break;
		}
	}

	// free the parser
	XML_ParserFree(p);

	// close the file
	delete file;
	delete[] buff;

	PG_LogMSG("theme '%s' loaded successfully", filename.c_str());

	return info.theme;
}

void PG_Theme::Unload(PG_Theme* theme) {
	delete theme;
}
