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

#include "dashboard.h"
#include "graphics/blitter.h"
#include "graphics/drawing.h"
#include "gamemap.h"
#include "iconrepository.h"
#include "spfst.h"



DashboardPanel::DashboardPanel ( PG_Widget *parent, const PG_Rect &r, const ASCString& panelName_, bool loadTheme = true )
               :Panel ( parent, r, panelName_, loadTheme )
{
   updateFieldInfo.connect ( SigC::slot( *this, &WindInfoPanel::eval ));
   registerSpecialDisplay( "windarrow" );

   registerSpecialDisplay( "unitexp" );
   registerSpecialDisplay( "unit_level" );
   registerSpecialDisplay( "unit_pic" );
   for ( int i = 0; i < 10; ++i)
      registerSpecialDisplay( "symbol_weapon" + ASCString::toString(i) );
   registerSpecialDisplay( "showplayercolor0" );
   registerSpecialDisplay( "showplayercolor1" );
   registerSpecialDisplay( "field_weather" );
};


void DashboardPanel::registerSpecialDisplay( const ASCString& name )
{
   SpecialDisplayWidget* sdw = dynamic_cast<SpecialDisplayWidget*>( FindChild( name, true ) );
   if ( sdw )
     sdw->display.connect( SigC::slot( *this, &UnitInfoPanel::painter ));
}


void DashboardPanel::painter ( const PG_Rect &src, const ASCString& name, const PG_Rect &dst)
{
   Surface screen = Surface::Wrap( PG_Application::GetScreen() );
   MapCoordinate mc = actmap->player[actmap->actplayer].cursorPos;

   if ( name == "windarrow" ) {
      if ( actmap && actmap->weatherSystem->getCurrentWindSpeed() > 0 ) {
         MegaBlitter<4,colorDepth,ColorTransform_None, ColorMerger_AlphaOverwrite, SourcePixelSelector_Rotation> blitter;
         blitter.setAngle( directionangle[actmap->weatherSystem->getCurrentWindDirection()] );
         blitter.blit ( IconRepository::getIcon("wind-arrow.png"), screen, SPoint(dst.x, dst.y) );
      }
      return;
   }

   if ( name == "field_weather" ) {
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



   if ( mc.valid() && fieldvisiblenow( actmap->getField(mc)) ) {
      Vehicle* veh = actmap->getField(mc)->vehicle;

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
               if ( name == "symbol_weapon" + ASCString::toString(pos))
                  screen.Blit( IconRepository::getIcon(SingleWeapon::getIconFileName( veh->typ->weapons.weapon[i].getScalarWeaponType()) + "-small.png"), SPoint(dst.x, dst.y));

               ++pos;
             }
          }
      }

   }
}


void DashboardPanel::eval()
{
   MapCoordinate mc = actmap->player[actmap->actplayer].cursorPos;
   pfield fld = actmap->getField(mc);

   Vehicle* veh = fld? fld->vehicle : NULL;

   PG_Application::SetBulkMode(true);

   int weaponsDisplayed = 0;

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

   if ( mc.valid() ) {
      if ( veh && fieldvisiblenow( fld ) ) {

         setLabelText( "unittypename", veh->typ->name );
         setLabelText( "unitname", veh->name );
         setBargraphValue( "unitdamage", float(100-veh->damage) / 100  );
         setLabelText( "unitstatus", 100-veh->damage );
         setBargraphValue( "unitfuel", veh->typ->tank.fuel ? float( veh->getTank().fuel) / veh->typ->tank.fuel : 0  );
         setLabelText( "unitfuelstatus", veh->getTank().fuel );
         setBargraphValue( "unitmaterial", veh->typ->tank.material ? float( veh->getTank().material) / veh->typ->tank.material : 0  );
         setLabelText( "unitmaterialstatus", veh->getTank().material );
         setBargraphValue( "unitenergy", veh->typ->tank.energy ? float( veh->getTank().energy) / veh->typ->tank.energy : 0  );
         setLabelText( "unitenergystatus", veh->getTank().energy );
         setLabelText( "movepoints", veh->getMovement() );

         int &pos = weaponsDisplayed;
         for ( int i = 0; i < veh->typ->weapons.count; ++i) {
            if ( !veh->typ->weapons.weapon[i].service() && pos < 10 ) {
                ASCString ps = ASCString::toString(pos);
                setLabelText( "punch_weapon" + ps, veh->typ->weapons.weapon[i].maxstrength );
                if ( (veh->typ->functions & cfno_reactionfire) || !veh->typ->weapons.weapon[i].shootable() || veh->typ->weapons.weapon[i].getScalarWeaponType() == cwminen )
                   setLabelText( "RF_weapon" + ps, "-" );
                else
                   setLabelText( "RF_weapon" + ps, veh->typ->weapons.weapon[i].reactionFireShots );
                setLabelText( "status_ammo" + ps, veh->ammo[i] );
                setBargraphValue( "bar_ammo" + ps, veh->typ->weapons.weapon[i].count ? float(veh->ammo[i]) / veh->typ->weapons.weapon[i].count : 0 );
                ++pos;
             }
          }


      } else {

         Building* bld = fld->building;
         if ( bld && fieldvisiblenow( fld ) ) {
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

   }

   PG_Application::SetBulkMode(false);
   Redraw(true);
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
   SpecialInputWidget* siw = dynamic_cast<SpecialInputWidget*>(obj);
   if ( siw ) {
      if ( event->button == SDL_BUTTON_RIGHT ) {
         if ( event->type == SDL_MOUSEBUTTONDOWN  ) {
            WeaponInfoPanel* wip = new WeaponInfoPanel( PG_Application::GetWidgetById( 1 ), PG_Rect( 200, 100, 200, 150 ));
            wip->Show();
            return true;
         }
         if ( event->type == SDL_MOUSEBUTTONUP ) {
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


WeaponInfoPanel::WeaponInfoPanel (PG_Widget *parent, const PG_Rect &r ) : Panel( parent, r, "WeaponInfo" )
{
   SetName(name);
/*
   updateFieldInfo.connect ( SigC::slot( *this, &UnitInfoPanel::eval ));

   SpecialInputWidget* siw = dynamic_cast<SpecialInputWidget*>( FindChild( "weapinfo", true ) );
   if ( siw ) {
      siw->sigMouseButtonDown.connect( SigC::slot( *this, &UnitInfoPanel::onClick ));
      siw->sigMouseButtonUp.connect( SigC::slot( *this, &UnitInfoPanel::onClick ));
   }
   */
}

ASCString WeaponInfoPanel::name = "WeaponInfoPanel";
const ASCString& WeaponInfoPanel::WIP_Name()
{
   return name;
}


