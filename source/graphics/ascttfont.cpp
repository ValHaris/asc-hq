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
#include "ascttfont.h"
#include <map>
//#include "misc.h"

const ASCRGBColor ASCRGBColor::BLACKCOLOR(0,0,0);


ASCTTFont::ASCTTFont(): defaultColor(ASCRGBColor(0,0,0))
{
}

ASCTTFont::ASCTTFont(const ASCString& file, int size, const ASCRGBColor& defColor): defaultColor(defColor){
  myFont = TTF_OpenFont(file.c_str(), size);
}

ASCTTFont::~ASCTTFont()
{
  TTF_CloseFont(myFont); 
}


SDL_Surface* ASCTTFont::output(const ASCTTFontRenderAlgorithm& alg, const ASCString& text, const ASCRGBColor& fgColor, ASCFontStyle style) const{ 
  TTF_SetFontStyle(myFont, style);
  SDL_Surface* sdlText = alg.render(this, text, fgColor);
  TTF_SetFontStyle(myFont, NORMAL);  
  return sdlText;
}
//*****************************************************************************************************************************************
ASCTTFontFactory::ASCTTFontFactory(){
  TTF_Init();
  atexit(TTF_Quit);
}

ASCTTFontFactory::~ASCTTFontFactory(){
  for(std::map<ASCString, ASCTTFont*>::iterator it = fonts.begin(); it != fonts.end(); it++){
    delete (it->second);
  }
}

const ASCTTFont& ASCTTFontFactory::newFont(ASCString file, int size, const ASCRGBColor& defaultColor){
  ASCString key(file /*+ strrr(size)*/);
  FontMap::iterator it = fonts.find(key);  
  ASCTTFont* font;
  if(it != fonts.end()){
   font = it->second;
  }else{
  font = new ASCTTFont(file, size, defaultColor);
  fonts.insert( make_pair( key, font ) );
  fonts.insert(FontMap::value_type(key, font));  
  }
  return *font;
}

//*****************************************************************************************************************************************
ASCLabel::ASCLabel(const ASCTTFont& font, const ASCString& text, const ASCRGBColor& fColor, ASCRenderStyle rStyle, ASCFontStyle fStyle){
ASCTTFontRenderAlgorithm* algo;
if(rStyle == SOLID)
  algo = new ASCTTFontRenderSolid();
else
  algo = new ASCTTFontRenderBlended();  
  
  labelSurface = font.output(*algo, text, fColor, fStyle);
};
  
int ASCLabel::getHeight(){
  return labelSurface->h;
}

int ASCLabel::getWidth(){
  return labelSurface->w;
}


void ASCLabel::output(SDL_Surface* sDest, ASCRect rect){
  SDL_BlitSurface(labelSurface, NULL, sDest, &rect);
  SDL_UpdateRect(sDest, rect.x, rect.y, rect.w, rect.h);    
}
  
ASCLabel::~ASCLabel(){
  SDL_FreeSurface( labelSurface );
}

//*****************************************************************************************************************************************
SDL_Surface* ASCTTFontRenderSolid::render(const ASCTTFont* font, ASCString text, ASCRGBColor col, ASCRGBColor bgCol)const {
  SDL_Color color ={col.getRedValue(), col.getGreenValue(), col.getBlueValue()};
  return TTF_RenderText_Solid(font->myFont, text.c_str(), color);
}

//*****************************************************************************************************************************************
SDL_Surface* ASCTTFontRenderShaded::render(const ASCTTFont* font, ASCString text, ASCRGBColor fgCol, ASCRGBColor bgCol) const{
  SDL_Color fgColor ={fgCol.getRedValue(), fgCol.getGreenValue(), fgCol.getBlueValue()};
  SDL_Color bgColor ={bgCol.getRedValue(), bgCol.getGreenValue(), bgCol.getBlueValue()};  
  return TTF_RenderText_Shaded(font->myFont, text.c_str(), fgColor, bgColor);
}

//*****************************************************************************************************************************************
SDL_Surface* ASCTTFontRenderBlended::render(const ASCTTFont* font, ASCString text, ASCRGBColor col, ASCRGBColor bgCol) const{
  SDL_Color color ={col.getRedValue(), col.getGreenValue(), col.getBlueValue()};
  return TTF_RenderText_Blended(font->myFont, text.c_str(), color);
}

