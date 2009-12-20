/*
     This file is part of Advanced Strategic Command; http://www.asc-hq.org
     Copyright (C) 1994-2009  Martin Bickel
 
     This program is free software; you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation; either version 2 of the License, or
     (at your option) any later version.
 
     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.
 
     You should have received a copy of the GNU General Public License
     along with this program; see the file COPYING. If not, write to the 
     Free Software Foundation, Inc., 59 Temple Place, Suite 330, 
     Boston, MA  02111-1307  USA
*/

#include <wx/intl.h>
#include <iostream>

#include "i18n.h"
#include "gameoptions.h"

class OpaqueLocaleData {
   public: 
      ASCString canonicalName;  
};

Locale::Locale()
{
   data = new OpaqueLocaleData();
   const wxLanguageInfo* lanInfo = wxLocale::GetLanguageInfo( wxLocale::GetSystemLanguage() );
   if ( lanInfo ) {
      data->canonicalName = ASCString( lanInfo->CanonicalName.mb_str() );
   }
   
}


ASCString Locale::getLang() 
{
   if ( CGameOptions::Instance()->languageOverride.length() >= 2 )
      return CGameOptions::Instance()->languageOverride;
   else {
      return data->canonicalName;
   }
}
      
ASCString Locale::getLocalizedFile( const ASCString& filename, const ASCString& nativeMessageLanguage )
{
   ASCString lang = getLang();
   
   if ( lang != nativeMessageLanguage && !lang.empty()) {
      ASCString newfilename = filename + "." + lang;
      if ( exist( newfilename )) 
         return newfilename; 
      else {
         newfilename.toLower();
         if ( exist( newfilename )) 
            return newfilename;
      }
      
      if ( lang.find( '_' ) != ASCString::npos ) {
         ASCString country = lang.substr( 0, lang.find( '_' ) );
         
         tfindfile ff( filename + "." + country + "*");
         if ( ff.getFoundFileNum() )
            return ff.getnextname();
         
         tfindfile ff2( copytoLower(filename) + "." + country + "*");
         if ( ff2.getFoundFileNum() )
            return ff2.getnextname();
         
      }
   }

   return "";
}
      
Locale::~Locale()
{
   delete data;  
}

