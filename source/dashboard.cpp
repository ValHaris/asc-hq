/*! \file dashboard.cpp
    \brief The box displaying unit information
*/


/***************************************************************************
                          dashboard.cpp  -  description
                             -------------------
    begin                : Sat Jan 27 2001
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

 #include "sigc++/retype.h"

#include "dashboard.h"
#include "graphics/blitter.h"
#include "graphics/drawing.h"
#include "gamemap.h"
#include "iconrepository.h"
#include "spfst.h"
#include "pgimage.h"
#include "textfiletags.h"
#include "mapdisplay.h"
#include "dialogs/unitinfodialog.h"
#include "gameoptions.h"

#include "sg.h"



DashboardPanel::DashboardPanel ( PG_Widget *parent, const PG_Rect &r, const ASCString& panelName_, bool loadTheme = true )
   :LayoutablePanel ( parent, r, panelName_, loadTheme ), veh(NULL), bld(NULL)
{
   updateFieldInfo.connect ( SigC::slot( *this, &DashboardPanel::eval ));
   registerSpecialDisplay( "windarrow" );

   registerSpecialDisplay( "unitexp" );
   registerSpecialDisplay( "unit_level" );
   registerSpecialDisplay( "unit_pic" );
   for ( int i = 0; i < 10; ++i)
      registerSpecialDisplay( "symbol_weapon" + ASCString::toString(i) );
   registerSpecialDisplay( "showplayercolor0" );
   registerSpecialDisplay( "showplayercolor1" );
   registerSpecialDisplay( "field_weather" );

   GameMap::sigMapDeletion.connect( SigC::slot( *this, &DashboardPanel::reset ));

   PG_LineEdit* l = dynamic_cast<PG_LineEdit*>( parent->FindChild( "unitname", true ) );
   if ( l ) {
      l->sigEditEnd.connect( SigC::slot( *this, &DashboardPanel::containerRenamed ));
   }
};


bool DashboardPanel::containerRenamed( PG_LineEdit* lineEdit )
{
   if ( veh )
      veh->name = lineEdit->GetText();
   
   if ( bld )
      bld->name = lineEdit->GetText();

   return true;
}

void DashboardPanel::reset(GameMap& map)
{
   if ( veh && veh->getMap() == &map )
      veh = NULL;
   
   if ( bld && bld->getMap() == &map )
      bld = NULL;
}



void DashboardPanel::registerSpecialDisplay( const ASCString& name )
{
   SpecialDisplayWidget* sdw = dynamic_cast<SpecialDisplayWidget*>( FindChild( name, true ) );
   if ( sdw )
     sdw->display.connect( SigC::slot( *this, &DashboardPanel::painter ));
}


void DashboardPanel::painter ( const PG_Rect &src, const ASCString& name, const PG_Rect &dst)
{
   if ( !actmap )
      return;

   Surface screen = Surface::Wrap( PG_Application::GetScreen() );

   if ( name == "windarrow" ) {
      if ( actmap && actmap->weatherSystem->getCurrentWindSpeed() > 0 ) {
         MegaBlitter<4,colorDepth,ColorTransform_None, ColorMerger_AlphaOverwrite, SourcePixelSelector_DirectRotation> blitter;
         blitter.setAngle( (WeatherSystem::WindDirNum - actmap->weatherSystem->getCurrentWindDirection()) * (360 /WeatherSystem::WindDirNum));
         blitter.blit ( IconRepository::getIcon("wind-arrow.png"), screen, SPoint(dst.x, dst.y) );
      }
      return;
   }

   if ( name == "field_weather" ) {
      MapCoordinate mc = actmap->getCursor();
      if ( actmap && mc.valid() && fieldvisiblenow( actmap->getField(mc)) ) {
         MegaBlitter<4,colorDepth,ColorTransform_None, ColorMerger_AlphaOverwrite> blitter;

         static const char* weathernames[] = {"terrain_weather_dry.png",
                                              "terrain_weather_lightrain.png",
                                              "terrain_weather_heavyrain.png",
                                              "terrain_weather_lightsnow.png",
                                              "terrain_weather_heavysnow.png",
                                              "terrain_weather_ice.png" };

         blitter.blit ( IconRepository::getIcon(weathernames[actmap->getField(mc)->getweather()]), screen, SPoint(dst.x, dst.y) );
      }
      return;
   }



   if( name == "showplayercolor0" || name == "showplayercolor1" ) {
      MegaBlitter<4,4,ColorTransform_PlayerTrueCol,ColorMerger_PlainOverwrite> blitter;
      blitter.setColor( actmap->player[actmap->actplayer].getColor() );
      blitter.blit( IconRepository::getIcon("show_playercolor.png"), screen, SPoint(dst.x, dst.y));
      return;
   }



      if ( name == "unitexp" ) {
         int experience = 0;
         if ( veh )
            experience = veh->experience;

         screen.Blit( IconRepository::getIcon("experience" + ASCString::toString(experience) + ".png"), SPoint(dst.x, dst.y) );
      }

      if ( name == "unit_level" ) {
         int height1 = 0;
         int height2 = 0;
         if ( veh ) {
            height1 = veh->height;
            height2 = veh->typ->height;
         }

         for ( int i = 0; i < 8; ++i ) {
            if ( height1 & (1 << i )) {
               MegaBlitter<4,4,ColorTransform_PlayerTrueCol,ColorMerger_PlainOverwrite> blitter;
               blitter.setColor( actmap->player[actmap->actplayer].getColor() );
               blitter.blit( IconRepository::getIcon("height-b" + ASCString::toString(i) + ".png"), screen, SPoint(dst.x, dst.y + (7-i) * 13));
            } else
               if ( height2 & (1 << i ))
                  screen.Blit( IconRepository::getIcon("height-a" + ASCString::toString(i) + ".png"), SPoint(dst.x, dst.y + (7-i) * 13 ) );

         }
      }

      if ( name == "unit_pic" ) {
         if ( veh )
           veh->typ->paint( screen, SPoint( dst.x, dst.y ), veh->getOwner() );
      }

      if ( veh ) {
         int pos = 0;
         for ( int i = 0; i < veh->typ->weapons.count; ++i) {
            if ( !veh->typ->weapons.weapon[i].service() && pos < 10 ) {
               if ( name == "symbol_weapon" + ASCString::toString(pos) )
                  screen.Blit( IconRepository::getIcon(SingleWeapon::getIconFileName( veh->typ->weapons.weapon[i].getScalarWeaponType()) + "-small.png"), SPoint(dst.x, dst.y));

               ++pos;
             }
          }
      }

}


void DashboardPanel::eval()
{
   if ( !actmap || actmap->actplayer < 0 )
      return;


   MapCoordinate mc = actmap->player[actmap->actplayer].cursorPos;
   tfield* fld = actmap->getField(mc);

   Vehicle* veh = fld? fld->vehicle : NULL;

   PG_Application::SetBulkMode(true);

   setBargraphValue( "winddisplay", float(actmap->weatherSystem->getCurrentWindSpeed()) / 255  );

   setLabelText( "windspeed", actmap->weatherSystem->getCurrentWindSpeed() );

   if ( mc.valid() && fieldvisiblenow( fld )) {
      setLabelText( "terrain_harbour", fld->bdt.test(cbharbour) ? "YES" : "NO" );
      setLabelText( "terrain_pipe", fld->bdt.test(cbpipeline) || fld->building ? "YES" : "NO" );

      setLabelText( "terrain_defencebonus", fld->getdefensebonus() );
      setLabelText( "terrain_attackbonus", fld->getattackbonus() );
      setLabelText( "terrain_jam", fld->getjamming() );
      setLabelText( "terrain_name", fld->typ->terraintype->name );

      int unitspeed;
      if ( veh )
         unitspeed = getmaxwindspeedforunit ( veh );
      else
         unitspeed = maxint;

       int windspeed = actmap->weatherSystem->getCurrentWindSpeed()*maxwindspeed ;
       if ( unitspeed < 255*256 ) {
          if ( windspeed > unitspeed*9/10 )
             setBarGraphColor( "winddisplay", 0xff0000  );
          else
             if ( windspeed > unitspeed*66/100 )
                setBarGraphColor( "winddisplay", 0xffff00  );
             else
                setBarGraphColor( "winddisplay", 0x00ff00  );
       } else
          setBarGraphColor( "winddisplay", 0x00ff00  );

   } else {
      setLabelText( "terrain_harbour", "" );
      setLabelText( "terrain_pipe", "" );

      setLabelText( "terrain_defencebonus", "" );
      setLabelText( "terrain_attackbonus", "" );
      setLabelText( "terrain_jam", "" );
      setLabelText( "terrain_name", "" );
      setBarGraphColor( "winddisplay", 0x00ff00  );
   }

   if ( mc.valid() && fld ) {
      if ( veh && fieldvisiblenow( fld ) ) {
         showUnitData( veh, NULL );
      } else {

         Building* bld = fld->building;
         if ( bld && fieldvisiblenow( fld ) ) 
            showUnitData( NULL, bld );
         else
            showUnitData( NULL, NULL );
      }
   }

   PG_Application::SetBulkMode(false);
   Redraw(true);
}
void DashboardPanel::showUnitData( Vehicle* veh, Building* bld, bool redraw )
{
   int weaponsDisplayed = 0;
   this->veh = veh;
   this->bld = bld;
   
   bool bulk = PG_Application::GetBulkMode();
   if ( redraw )
      PG_Application::SetBulkMode(true);
      
   if ( veh ) {
      setLabelText( "unittypename", veh->typ->name );
      setLabelText( "unitname", veh->name );
      setBargraphValue( "unitdamage", float(100-veh->damage) / 100  );
      setLabelText( "unitstatus", 100-veh->damage );
      setBargraphValue( "unitfuel", veh->getStorageCapacity().fuel ? float( veh->getTank().fuel) / veh->getStorageCapacity().fuel : 0  );
      setLabelText( "unitfuelstatus", veh->getTank().fuel );
      setBargraphValue( "unitmaterial", veh->getStorageCapacity().material ? float( veh->getTank().material) / veh->getStorageCapacity().material : 0  );
      setLabelText( "unitmaterialstatus", veh->getTank().material );
      setBargraphValue( "unitenergy", veh->getStorageCapacity().energy ? float( veh->getTank().energy) / veh->getStorageCapacity().energy : 0  );
      setLabelText( "unitenergystatus", veh->getTank().energy );
      setLabelText( "movepoints", veh->getMovement() );
   
      int &pos = weaponsDisplayed;
      for ( int i = 0; i < veh->typ->weapons.count; ++i) {
         if ( !veh->typ->weapons.weapon[i].service() && pos < 10 ) {
            ASCString ps = ASCString::toString(pos);
            setLabelText( "punch_weapon" + ps, veh->typ->weapons.weapon[i].maxstrength );
            if ( veh->typ->hasFunction( ContainerBaseType::NoReactionfire  ) || !veh->typ->weapons.weapon[i].shootable() || veh->typ->weapons.weapon[i].getScalarWeaponType() == cwminen )
               setLabelText( "RF_weapon" + ps, "-" );
            else
               setLabelText( "RF_weapon" + ps, veh->typ->weapons.weapon[i].reactionFireShots );
            setLabelText( "status_ammo" + ps, veh->ammo[i] );
            setBargraphValue( "bar_ammo" + ps, veh->typ->weapons.weapon[i].count ? float(veh->ammo[i]) / veh->typ->weapons.weapon[i].count : 0 );
            ++pos;
         }
      }
   } else {
      if ( bld ) {
         setLabelText( "unittypename", bld->typ->name );
         setLabelText( "unitname", bld->name );
         setBargraphValue( "unitdamage", float(100-bld->damage) / 100  );
         setLabelText( "unitstatus", 100-bld->damage );
      } else {
         setLabelText( "unittypename", "" );
         setLabelText( "unitname", "" );
         setBargraphValue( "unitdamage", 0  );
         setLabelText( "unitstatus", "" );
      }
   
      setBargraphValue( "unitfuel", 0  );
      setLabelText( "unitfuelstatus", "" );
      setBargraphValue( "unitmaterial",  0  );
      setLabelText( "unitmaterialstatus", "" );
      setBargraphValue( "unitenergy",  0  );
      setLabelText( "unitenergystatus", "" );
      setLabelText( "movepoints", "" );
   }
   for ( int i = weaponsDisplayed; i < 10; ++i ) {
      ASCString ps = ASCString::toString(i);
      setLabelText( "punch_weapon" + ps, "" );
      setLabelText( "RF_weapon" + ps, "" );
      setLabelText( "status_ammo" + ps, "" );
      setBargraphValue( "bar_ammo" + ps, 0 );
   }
   
   if ( redraw ) {
      if ( !bulk )
         PG_Application::SetBulkMode(false);
      Redraw(true);
   }
}



WindInfoPanel::WindInfoPanel (PG_Widget *parent, const PG_Rect &r ) : DashboardPanel( parent, r, "WindInfo" )
{
}




UnitInfoPanel::UnitInfoPanel (PG_Widget *parent, const PG_Rect &r ) : DashboardPanel( parent, r, "UnitInfo" )
{
   SpecialInputWidget* siw = dynamic_cast<SpecialInputWidget*>( FindChild( "weapinfo", true ) );
   if ( siw ) {
      siw->sigMouseButtonDown.connect( SigC::slot( *this, &UnitInfoPanel::onClick ));
      siw->sigMouseButtonUp.connect( SigC::slot( *this, &UnitInfoPanel::onClick ));
   }
}


bool UnitInfoPanel::onClick ( PG_MessageObject* obj, const SDL_MouseButtonEvent* event )
{

   static const bool modalWeaponInfo = true;
   
   SpecialInputWidget* siw = dynamic_cast<SpecialInputWidget*>(obj);
   if ( siw ) {
      if ( event->button == SDL_BUTTON_RIGHT ) {
         if ( event->type == SDL_MOUSEBUTTONDOWN  ) {

            tfield* fld = actmap->getField( actmap->player[actmap->actplayer].cursorPos );
            const Vehicletype* vt = NULL;
            const Vehicle* veh = NULL;
            if ( fld && fld->vehicle ) {
               vt = fld->vehicle->typ;
               veh = fld->vehicle;
            }
            if ( vt || veh ) {
               WeaponInfoPanel* wip = new WeaponInfoPanel( PG_Application::GetWidgetById( ASC_PG_App::mainScreenID ), veh, vt );
               wip->Show();
               if ( modalWeaponInfo ) {
                  wip->SetCapture();
                  wip->RunModal();
                  delete wip;
               } // else
              //     PG_Application::
            }
            return true;
         }
         if ( event->type == SDL_MOUSEBUTTONUP && !modalWeaponInfo ) {
            bool result = false;
            PG_Widget* wip;
            do  {
               wip = PG_Application::GetWidgetByName( WeaponInfoPanel::WIP_Name() );
               if ( wip ) {
                  delete wip;
                  result = true;
               }
            } while ( wip );
            return result;
         }
      }
   }
   return false;
}

class WeaponInfoLine: public PG_Image {
      const SingleWeapon* weapon;
      const Vehicletype* veh;
      WeaponInfoPanel* wip;
   public:
      WeaponInfoLine( WeaponInfoPanel* parent, const PG_Point& p, SDL_Surface* image, const SingleWeapon* weap, const Vehicletype* vehicle )
           : PG_Image( parent, p, image, false ), weapon(weap), veh ( vehicle ), wip(parent)
      {
      };

      void painter ( const PG_Rect &src, const ASCString& name, const PG_Rect &dst)
      {
         Surface screen = Surface::Wrap( PG_Application::GetScreen() );
         if ( name == "weapon_symbol1" )
            screen.Blit( IconRepository::getIcon(SingleWeapon::getIconFileName( weapon->getScalarWeaponType()) + "-small.png"), SPoint(dst.x, dst.y));

         if ( name == "weapon_targets" || name == "weapon_shootfrom" ) {
            int height;
            if (name == "weapon_targets")
               height = weapon->targ;
            else {
               height = weapon->sourceheight;
               if ( veh )
                  height &= veh->height;
            }

            for ( int i = 0; i < 8; ++i )
               if ( height & (1 << i )) {
                  Surface& tick = IconRepository::getIcon("weapon_ok.png");
                  screen.Blit( tick, SPoint(dst.x + i * tick.w(), dst.y  ) );
               }
         }

      };

      void registerSpecialDisplay( const ASCString& name )
      {
         SpecialDisplayWidget* sdw = dynamic_cast<SpecialDisplayWidget*>( FindChild( name, true ) );
         if ( sdw )
            sdw->display.connect( SigC::slot( *this, &WeaponInfoLine::painter ));
      };

	   void eventMouseEnter()
      {
         wip->showWeapon( weapon );
      };

      void eventMouseLeave()
      {
         wip->showWeapon();
      };

};



WeaponInfoPanel::WeaponInfoPanel (PG_Widget *parent, const Vehicle* veh, const Vehicletype* vt ) : Panel( parent, PG_Rect::null, "WeaponInfo" ), weaponCount(0)
{
   SetName(name);

   vector<const SingleWeapon*> displayedWeapons;
   vector<int> displayedWeaponNum;

   for ( int j = 0; j < vt->weapons.count ; j++)
      if ( vt->weapons.weapon[j].getScalarWeaponType() >= 0 ) {
         ++weaponCount;
         displayedWeapons.push_back( &vt->weapons.weapon[j] );
         displayedWeaponNum.push_back(j);
      }


   Surface& head = IconRepository::getIcon("weapon_large_top.png");
   Surface& line = IconRepository::getIcon("weapon_large_line.png");
   Surface& foot = IconRepository::getIcon("weapon_large_bottom.png");
   int height = head.h() + foot.h() + weaponCount * line.h() + GetTitlebarHeight();

   SizeWidget( head.w(), height, false );

   int lineStartY = GetTitlebarHeight() + head.h()  - 1;

   PG_Widget* footWidget = FindChild( "bottom", true );
   assert( footWidget != NULL );
   footWidget->MoveWidget(0,  lineStartY + line.h() * weaponCount, false );

   for ( int i = 0; i < weaponCount; ++i ) {
      WidgetParameters widgetParams = getDefaultWidgetParams();
      WeaponInfoLine* lineWidget = new WeaponInfoLine ( this, PG_Point( 0,  lineStartY + i * line.h() ), line.getBaseSurface(), displayedWeapons[i], vt );

      PropertyReadingContainer pc ( "panel", textPropertyGroup );

      pc.openBracket("LineWidget");
      parsePanelASCTXT( pc, lineWidget, widgetParams );
      pc.closeBracket();


      assignWeaponInfo( this, lineWidget, *displayedWeapons[i] );
      lineWidget->registerSpecialDisplay( "weapon_shootfrom" );
      lineWidget->registerSpecialDisplay( "weapon_targets" );
      if ( veh )
         setLabelText( "weapon_currentammo", veh->ammo[displayedWeaponNum[i]], lineWidget );

   }
   setLabelText( "weapon_shootaftermove", vt->wait ? "no" : "yes" );
   setLabelText( "weapon_moveaftershoot", vt->hasFunction( ContainerBaseType::MoveAfterAttack  ) ? "yes" : "no" );

   /*
   for ( int i = 0; i < cmovemalitypenum; ++i ) 
      setLabelText( ASCString("weapon_efficiency_") + unitCategoryTags[i], cmovemalitypes[i] );
      */
}


void WeaponInfoPanel::showWeapon( const SingleWeapon* weap )
{
   int effic[13];
   for ( int k = 0; k < 13; k++ )
      if ( weap )
         effic[k] = weap->efficiency[k];
      else
         effic[k] = -1;

   if ( weap ) {
      int mindelta = 1000;
      int maxdelta = -1000;
      for ( int h1 = 0; h1 < 8; h1++ )
         for ( int h2 = 0; h2 < 8; h2++ )
            if ( weap->sourceheight & ( 1 << h1 ) )
               if ( weap->targ & ( 1 << h2 )) {
                  int delta = getheightdelta ( h1, h2);
                  if ( delta > maxdelta )
                     maxdelta = delta;
                  if ( delta < mindelta )
                     mindelta = delta;
               }
      for ( int a = -6; a < mindelta; a++ )
         effic[6+a] = -1;
      for ( int b = maxdelta+1; b < 7; b++ )
         effic[6+b] = -1;
   }

   for ( int i = -6; i <= 6; ++i ) {
      if ( effic[6+i] >= 0 )
         setLabelText( "weapon_distance_" + ASCString::toString(i), i );
      else
         setLabelText( "weapon_distance_" + ASCString::toString(i), "" );

      if ( weap && effic[6+i] >= 0 )
         setLabelText( "weapon_efficiency_" + ASCString::toString(i), weap->efficiency[6+i]  );
      else
         setLabelText( "weapon_efficiency_" + ASCString::toString(i), "" );
   }

                                   // grey light grey  yellow,    blue      red        green
   static const int colors[6] = { 0x969595, 0xdfdfdf, 0xfac914,  0x5383e6,   0xff5e5e, 0x08ce37 };

   for ( int i = 0; i< cmovemalitypenum; ++i)
      if ( weap ) {
         int col;
         if ( weap->targetingAccuracy[i] < 10 )
            col = colors[0] ;
         else
            if ( weap->targetingAccuracy[i] < 30 )
               col = colors[1] ;
            else
               if ( weap->targetingAccuracy[i] < 80 )
                  col = colors[3] ;
               else
                  if ( weap->targetingAccuracy[i] < 120 )
                     col = colors[2];
                  else
                     col = colors[4];
         setLabelColor( ASCString("weapon_efficiency_") + unitCategoryTags[i], col );
         setLabelText( ASCString("weapon_efficiency_") + unitCategoryTags[i], weap->targetingAccuracy[i]  );
      } else
         setLabelText( ASCString("weapon_efficiency_") + unitCategoryTags[i], "" );

   if ( weap )
      setLabelText( "weapon_text2",  weap->getName() );
   else
      setLabelText( "weapon_text2",  "" );

   if ( weap )  {
      setImage( "weapon_symbol2", IconRepository::getIcon(SingleWeapon::getIconFileName( weap->getScalarWeaponType()) + "-small.png") );
      show( "weapon_symbol2" );
   } else
      hide( "weapon_symbol2" );
}

bool   WeaponInfoPanel::eventMouseButtonUp (const SDL_MouseButtonEvent *button)
{
   if ( Panel::eventMouseButtonUp( button ))
      return true;
   
   if ( button->button == SDL_BUTTON_RIGHT ) {
      QuitModal();
      return true;
   } else
      return false;
}




ASCString WeaponInfoPanel::name = "WeaponInfoPanel";
const ASCString& WeaponInfoPanel::WIP_Name()
{
   return name;
}


MapInfoPanel::MapInfoPanel (PG_Widget *parent, const PG_Rect &r, MapDisplayPG* mapDisplay ) : DashboardPanel( parent, r, "MapInfo" ), zoomSlider(NULL), changeActive(false)
{
   assert( mapDisplay );
   this->mapDisplay = mapDisplay;
   
   zoomSlider = dynamic_cast<PG_Slider*>( FindChild( "zoomscroller", true ) );
   if ( zoomSlider ) {
      zoomSlider->SetRange(0,75); // results in zoomlevels from 100 - 25
      zoomSlider->sigSlide.connect( SigC::slot( *this, &MapInfoPanel::scrollTrack ));
      mapDisplay->newZoom.connect( SigC::slot( *this, &MapInfoPanel::zoomChanged ));
      zoomSlider->SetPosition( 100 - mapDisplay->getZoom() );
   }   

   const int labelnum = 3;
   char* label[labelnum] = { "pipes", "container", "resources"};
   for ( int i = 0; i < labelnum; ++i ) {
      PG_CheckButton* cb = dynamic_cast<PG_CheckButton*>( FindChild( label[i], true ) );
      if ( cb ) 
         cb->sigClick.connect( SigC::bind( SigC::slot( *this, &MapInfoPanel::checkBox ), label[i] ));
   }      
   
   mapDisplay->layerChanged.connect( SigC::slot( *this, &MapInfoPanel::layerChanged ));
      
   PG_Button* b = dynamic_cast<PG_Button*>( FindChild( "weaprange", true ) );
   if ( b )
      b->sigClick.connect( SigC::slot( *this, &MapInfoPanel::showWeaponRange ));
   
   PG_Button* b2 = dynamic_cast<PG_Button*>( FindChild( "moverange", true ) );
   if ( b2 )
      b2->sigClick.connect( SigC::slot( *this, &MapInfoPanel::showMovementRange ));
   
}

void MapInfoPanel::layerChanged( bool state, const ASCString& label )
{
   PG_CheckButton* cb = dynamic_cast<PG_CheckButton*>( FindChild( label, true ) );
   if ( cb && ! changeActive ) 
      if ( state )
         cb->SetPressed();
      else
         cb->SetUnpressed();
}

bool MapInfoPanel::showWeaponRange()
{
   execuseraction( ua_viewunitweaponrange );
   return true;
}

bool MapInfoPanel::showMovementRange()
{
   execuseraction( ua_viewunitmovementrange );
   return true;
}


void MapInfoPanel::zoomChanged( int zoom )
{
   if ( !changeActive )
      if ( zoomSlider )
         zoomSlider->SetPosition( 100 - zoom );
}

bool MapInfoPanel::scrollTrack( long pos )
{
   changeActive = true;
   mapDisplay->setNewZoom( 100 - pos );
   repaintMap();
   changeActive = false;
   return true;
}

bool MapInfoPanel::checkBox( bool state, const char* name )
{
   changeActive = true;
   mapDisplay->activateMapLayer( name, state );
   repaintMap();
   changeActive = false;
   return true;
}


