/***************************************************************************
 *   Copyright (C) 2004 by Kevin Hirschmann                                *
 *   hirsch@dhcppc0                                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef ascttfontH
#define ascttfontH

#define ASCRect SDL_Rect
#include <SDL_ttf.h>
#include <map>
#include <iostream>
#include "../ascstring.h"



/**
@author Kevin Hirschmann
*/
class ASCTTFont;

typedef std::map<ASCString, ASCTTFont*> FontMap;

enum ASCFontStyle{
  NORMAL,
  BOLD,
  ITALIC,
  UNDERLINE,
};


class ASCRGBColor {
private:
  int r;
  int g;
  int b;

public:
  static const ASCRGBColor BLACKCOLOR;
  ASCRGBColor():r(0), g(0), b(0) {}

  ASCRGBColor(int red, int green, int blue) {
    if(((0 <= red) || (red <= 255)) && ((0 <= green) || (green <= 255)) &&
        ((0 <= blue) || (blue <= 255))) {
      r = red;
      g = green;
      b = blue;
    } else { //Invalid value(s) used
      r = g = b = 0; 
    }
  }

  ~ASCRGBColor() {}
  
  int getRedValue() const{
    return r;
  };
  int getGreenValue() const{
    return g;
  };
  int getBlueValue() const{
    return b;
  };
};
//***************************************************************************************************************************
class ASCTTFontRenderAlgorithm{
public:
ASCTTFontRenderAlgorithm(){};
virtual ~ASCTTFontRenderAlgorithm(){};
virtual SDL_Surface* render(const ASCTTFont* font, ASCString text, ASCRGBColor, ASCRGBColor = ASCRGBColor()) const = 0;

};




//***************************************************************************************************************************
class ASCTTFontRenderSolid : public ASCTTFontRenderAlgorithm{
public:
ASCTTFontRenderSolid(): ASCTTFontRenderAlgorithm(){};
~ASCTTFontRenderSolid(){};
virtual SDL_Surface* render(const ASCTTFont* font, ASCString text, ASCRGBColor, ASCRGBColor bgColor) const;
};

//***************************************************************************************************************************
class ASCTTFontRenderShaded : public ASCTTFontRenderAlgorithm{
public:
ASCTTFontRenderShaded(): ASCTTFontRenderAlgorithm(){};
~ASCTTFontRenderShaded(){};
virtual SDL_Surface* render(const ASCTTFont* font, ASCString text, ASCRGBColor fgCol, ASCRGBColor bgColor) const;
};

//***************************************************************************************************************************
class ASCTTFontRenderBlended : public ASCTTFontRenderAlgorithm{
public:
ASCTTFontRenderBlended(): ASCTTFontRenderAlgorithm(){};
~ASCTTFontRenderBlended(){};
virtual SDL_Surface* render(const ASCTTFont* font, ASCString text, ASCRGBColor fgCol, ASCRGBColor bgColor) const;
};


//***************************************************************************************************************************
class ASCTTFont {
friend class ASCTTFontFactory;
friend class ASCTTFontRenderSolid;
friend class ASCTTFontRenderShaded;
friend class ASCTTFontRenderBlended;
private:
  TTF_Font* myFont;
  const ASCRGBColor& defaultColor;  
  ASCTTFont(const ASCString& file, int size, const ASCRGBColor& defaultColor = ASCRGBColor::BLACKCOLOR);  
  
public:    
  ASCTTFont();    
  ~ASCTTFont();
  SDL_Surface* output(const ASCTTFontRenderAlgorithm& alg, const ASCString& text, const ASCRGBColor& color, ASCFontStyle style = NORMAL) const;
};
//***************************************************************************************************************************
class ASCTTFontFactory {
private:
FontMap fonts;
public:
  ASCTTFontFactory();
  ~ASCTTFontFactory();  
  const ASCTTFont& newFont(ASCString file, int size, const ASCRGBColor& defaultColor = ASCRGBColor::BLACKCOLOR);

};

//***************************************************************************************************************************
enum ASCRenderStyle{
  SOLID,
  BLENDED  
};

//***************************************************************************************************************************
class ASCLabel{
public:
  ASCLabel(const ASCTTFont& font, const ASCString& text, const ASCRGBColor& fColor, ASCRenderStyle rStyle = BLENDED, ASCFontStyle fStyle = NORMAL);
  ~ASCLabel();
inline  int getHeight();
inline  int getWidth();

void output(SDL_Surface* sDest, ASCRect rect);
  
private:
SDL_Surface* labelSurface;

};
#endif

