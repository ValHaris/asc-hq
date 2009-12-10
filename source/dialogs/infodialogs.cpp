/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <iostream>

#ifdef WIN32
# include<direct.h> 
#endif

#include "../loadbi3.h"
#include "../unitset.h"


#include "unitcounting.h"
#include "../widgets/textrenderer.h"
#include "../containerbase-functions.h"
#include "../memory-measurement.h"

#include "../spfst-legacy.h"
#include "../i18n.h"

void showSearchPath()
{

   ASCString s = "#fontsize=17#ASC search path#fontsize=13#\n";
   for ( int i = 0; i < getSearchPathNum(); ++i )
      s += getSearchPath ( i ) + "\n"; 

   s += "\n";
   s += "Configuration file used: \n";
   s += getConfigFileName();

#ifdef WIN32
   char buffer[_MAX_PATH];

   if( _getcwd( buffer, _MAX_PATH ) ) {
      s += "\n#fontsize=17#Current working directory#fontsize=13#\n";
      s += buffer;
   }
#endif

   s += "\n\n#fontsize=17#Mounted archive files#fontsize=13#\n";
   s += listContainer();


   ViewFormattedText vft("ASC directories", s, PG_Rect( -1, -1, 400, 400 ));
   vft.Show();
   vft.RunModal();
}


void showSDLInfo()
{
   ASCString s;
   s += "#fontsize=18#SDL versions#fontsize=14#\n";
   char buf[1000];
   SDL_version compiled;
   SDL_VERSION(&compiled);
   sprintf(buf, "\nCompiled with SDL version: %d.%d.%d\n", compiled.major, compiled.minor, compiled.patch);
   s += buf;

   sprintf(buf, "Linked with SDL version: %d.%d.%d\n", SDL_Linked_Version()->major, SDL_Linked_Version()->minor, SDL_Linked_Version()->patch);
   s += buf;

   s += "Byte order is ";
#if SDL_BYTEORDER==SDL_LIL_ENDIAN
   s += "little endian\n";
#else
#if SDL_BYTEORDER==SDL_BIG_ENDIAN
   s += "big endian\n";
#else
   s += "undefined\n";
#endif
#endif
  
   s += "Graphics backend: ";
   s += SDL_VideoDriverName( buf, 1000 );
   s += "\n";


   const SDL_VideoInfo* videoInfo = SDL_GetVideoInfo();
   s += "VideoInfo: \n";

   s += "Hardware surfaces available: ";
   s += videoInfo->hw_available ? "yes" : "no";


   s += "\nScreen uses hardware surface: ";
   s += PG_Application::GetScreen()->flags & SDL_HWSURFACE ? "yes" : "no";

   s += "\nWindow manager available: ";
   s += videoInfo->wm_available ? "yes" : "no";

   s += "\nhardware to hardware blits accelerated: ";
   s += videoInfo->blit_hw ? "yes" : "no";

   s += "\nhardware to hardware colorkey blits accelerated: ";
   s += videoInfo->blit_hw_CC ? "yes" : "no";

   s += "\nhardware to hardware alpha blits accelerated: ";
   s += videoInfo->blit_hw_A ? "yes" : "no";

   s += "\nsoftware to hardware blits accelerated: ";
   s += videoInfo->blit_sw ? "yes" : "no";

   s += "\nsoftware to hardware colorkey blits accelerated: ";
   s += videoInfo->blit_sw_CC ? "yes" : "no";

   s += "\nsoftware to hardware alpha blits accelerated: ";
   s += videoInfo->blit_sw_A ? "yes" : "no";

   s += "\ncolor fills accelerated: ";
   s += videoInfo->blit_fill ? "yes" : "no";

   s += "\nVideo memory: ";
   s += ASCString::toString( int(videoInfo->video_mem ));

   s += "\n\nLanguage: ";
   Locale locale;
   s += locale.getLang();
   
   ViewFormattedText vft( "SDL Settings", s, PG_Rect(-1,-1,450,550));
   vft.Show();
   vft.RunModal();
}




void ammoCounter( const ContainerBase* c, map<int,int>& amount )
{
   for ( int i = 0; i < weaponTypeNum; ++i )
      if ( weaponAmmo[i] )
         amount[i] += c->getAmmo(i,maxint );

   for ( int i = 1000; i < 1003; ++i )
      amount[i] += c->getAvailableResource( maxint, i-1000, 0 );

   for ( ContainerBase::Cargo::const_iterator i = c->getCargo().begin(); i != c->getCargo().end(); ++i )
      if ( *i )
         ammoCounter( *i, amount );
}


void showCargoSummary( tfield* fld )
{
   if ( !fld->vehicle ) {
      infoMessage( "Please select a unit");
      return;
   }

   if ( actmap->getCurrentPlayer().diplomacy.isAllied( fld->vehicle  )) {
      showUnitCargoSummary( fld->vehicle );
      map<int,int> ammo;
      ammoCounter( fld->vehicle, ammo );
      ASCString s;
      for ( int i = 0; i < weaponTypeNum; ++i )
         if ( weaponAmmo[i] )
            s += ASCString(cwaffentypen[i]) + ": " + ASCString::toString( ammo[i] ) + "\n";

      s += "\n";
      for ( int i = 1000; i < 1003; ++i )
         s += ASCString(Resources::name(i-1000)) + ": " + ASCString::toString( ammo[i] ) + "\n";

      ViewFormattedText vft("Ammo summary", s, PG_Rect( -1, -1, 300, 300 ));
      vft.Show();
      vft.RunModal();
   } else
      infoMessage( "The unit is not yours");
   
}


class FontViewer : public ASC_PG_Dialog {
   static const int spacing = 30;
   public:
      FontViewer() : ASC_PG_Dialog( NULL, PG_Rect( -1, -1, 18 * spacing, 19 * spacing ), "view character set" ) 
      {
         for ( int i = 32; i < 255; ++i ) {
            ASCString s;
            s += char(i);
            new PG_Label ( this, PG_Rect( (i % 16 + 1) * spacing, (i / 16 + 2) * spacing, spacing, spacing ), s );
         }
      };
};

void viewFont()
{
   FontViewer fv;
   fv.Show();
   fv.RunModal();
}



void resourceAnalysis()
{
   ASCString s;
   Resources total;
   for ( Player::BuildingList::iterator j = actmap->player[actmap->actplayer].buildingList.begin(); j != actmap->player[actmap->actplayer].buildingList.end() ; j++ ) {
      Resources res = (*j)->getResource( Resources(maxint,maxint,maxint), true, 0 );
      MapCoordinate pos = (*j)->getPosition();
      s += (*j)->getName() + " #pos150#(" + ASCString::toString( pos.x ) + "/" + ASCString::toString( pos.y) + pos.toString() + ") : #pos300#" + ASCString::toString(res.energy) + "#pos400#" + ASCString::toString(res.material) + "#pos500#" + ASCString::toString(res.fuel) + "\n";
      total += res;
   }
   s += "\nTotal:\n";

   for ( int r = 0; r < 3; ++r)
      if ( actmap->isResourceGlobal(r))
         total.resource(r) = actmap->bi_resource[actmap->actplayer].resource(r);
   s += total.toString();

   s += "\nIncluding units:\n";
   for ( Player::VehicleList::iterator j = actmap->player[actmap->actplayer].vehicleList.begin(); j != actmap->player[actmap->actplayer].vehicleList.end() ; j++ ) 
      total += (*j)->getResource( Resources(maxint,maxint,maxint), true, 0 );

   s += total.toString();

   ViewFormattedText vft("Resource Analysis", s, PG_Rect( -1, -1, 600, 550 ));
   vft.Show();
   vft.RunModal();
}

void showUnitEndurance()
{
   
   vector<Vehicletype*> units;
   for ( int i = 0; i < vehicleTypeRepository.getNum(); ++i ) {
      Vehicletype* vt = vehicleTypeRepository.getObject_byPos(i);
      if ( vt && (vt->movemalustyp == MoveMalusType::medium_aircraft || 
           vt->movemalustyp == MoveMalusType::light_aircraft  ||
           vt->movemalustyp == MoveMalusType::heavy_aircraft  ||
           vt->movemalustyp == MoveMalusType::helicopter))
         units.push_back( vt );
   }
   sort( units.begin(), units.end(), vehicleComp );
   
   ASCString s;
   for ( vector<Vehicletype*>::iterator i = units.begin(); i != units.end(); ++i )
   {
      ASCString u;
      ASCString range;
      if ( (*i)->fuelConsumption )
         range = ASCString::toString( (*i)->getStorageCapacity(0).fuel / (*i)->fuelConsumption);
      else
         range = "-";
      
      u.format( "#vehicletype=%d# %s : %d fuel ; %s fields range ; %d turns endurance \n", (*i)->id, (*i)->getName().c_str(), (*i)->getStorageCapacity(0).fuel, range.c_str(), UnitHooveringLogic::getEndurance(*i) );
      s += u;
   }
   
   ViewFormattedText vft("Unit Endurance", s, PG_Rect( -1, -1, 650, 550 ));
   vft.Show();
   vft.RunModal();
}


void showMemoryFootprint()
{
   MemoryMeasurement mm;
   mm.measureTypes();
   mm.measure( actmap );
   mm.measureIcons();
   
   ViewFormattedText vft("Memory Usage", mm.getResult(), PG_Rect( -1, -1, 750, 550 ));
   vft.Show();
   vft.RunModal();
}


void viewMiningPower()
{
   typedef map<const ContainerBaseType*,ASCString> InfoMap;
   InfoMap info;

   for ( Player::BuildingList::iterator i = actmap->getCurrentPlayer().buildingList.begin(); i != actmap->getCurrentPlayer().buildingList.end(); ++i )
      if ( (*i)->baseType->hasFunction( ContainerBaseType::MiningStation )) {
         int power = 0;
         int output = 0;
         MiningStation miningStation ( *i, true );
         for ( int r = 0; r < 3; ++r )
            if ( (*i)->maxplus.resource(r) ) {
               power = 100 * (*i)->plus.resource(r) / (*i)->maxplus.resource(r) ;
               break;
            }
               
         for ( int r = 0; r < 3; ++r )
            if ( miningStation.getPlus().resource(r) ) {
               output = miningStation.getPlus().resource(r);
               break;
            }
            
         ASCString txt = ASCString::toString(output) + " (" + ASCString::toString(power) + "%) " + (*i)->getPosition().toString() + " " + (*i)->baseType->name + "\n";
         info[(*i)->baseType] += txt;
      }

      ASCString fullText = "Mining Station Statistics\n\n";
      for ( InfoMap::iterator i = info.begin(); i != info.end(); ++i )
         fullText += i->second;


      ViewFormattedText vft("Mining Stations", fullText, PG_Rect( -1, -1, 750, 550 ));
      vft.Show();
      vft.RunModal();

}


void createUnitCostList()
{
   cout << "name;id;unitset id;new cost E;new cost M;old cost E;old cost M\n";
   for ( int i = 0; i < vehicleTypeRepository.getNum(); ++i ) {
      Vehicletype* v = vehicleTypeRepository.getObject_byPos(i);
      Resources oldc = v->calcProductionsCost();
      Resources newc = v->calcProductionsCost();
      
      cout <<  v->getName() << ";" << v->id << ";" << getUnitSetID( v) << ";" << newc.energy << ";" << newc.material << ";" << oldc.energy << ";" << oldc.material << "\n";
   }
}
