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
#include "gamemap.h"
#include "iconrepository.h"
#include "spfst.h"


WindInfoPanel::WindInfoPanel (PG_Widget *parent, const PG_Rect &r ) : Panel( parent, r, "WindInfo" ), dir(-1)
{
   updateFieldInfo.connect ( SigC::slot( *this, &WindInfoPanel::eval ));
   
   SpecialDisplayWidget* sdw = dynamic_cast<SpecialDisplayWidget*>( FindChild( "winddir", true ) );
   if ( sdw )
     sdw->display.connect( SigC::slot( *this, &WindInfoPanel::painter ));
}

// void WindInfoPanel::painter ( Surface& surf, const PG_Rect &area, int id, const PG_Rect &size)
void WindInfoPanel::painter ( const PG_Rect &src, const ASCString& name, const PG_Rect &dst)
{
   if ( actmap && actmap->weather.windSpeed > 0 ) {
      Surface screen = Surface::Wrap( PG_Application::GetScreen() );

      MegaBlitter<4,colorDepth,ColorTransform_None, ColorMerger_AlphaOverwrite, SourcePixelSelector_Rotation> blitter;
      blitter.setAngle( directionangle[actmap->weather.windDirection] );
      blitter.blit ( IconRepository::getIcon("wind-arrow.png"), screen, SPoint(dst.x, dst.y) );
   }
}



void WindInfoPanel::eval()
{
   Redraw(true);
}

WindInfoPanel::~WindInfoPanel()
{

}

UnitInfoPanel::UnitInfoPanel (PG_Widget *parent, const PG_Rect &r ) : Panel( parent, r, "UnitInfo" )
{
   updateFieldInfo.connect ( SigC::slot( *this, &UnitInfoPanel::eval ));

   SpecialInputWidget* siw = dynamic_cast<SpecialInputWidget*>( FindChild( "weapinfo", true ) );
   if ( siw ) {
      siw->sigMouseButtonDown.connect( SigC::slot( *this, &UnitInfoPanel::onClick ));
      siw->sigMouseButtonUp.connect( SigC::slot( *this, &UnitInfoPanel::onClick ));
   }

   SpecialDisplayWidget* sdw = dynamic_cast<SpecialDisplayWidget*>( FindChild( "unitexp", true ) );
   if ( sdw )
     sdw->display.connect( SigC::slot( *this, &UnitInfoPanel::painter ));

   sdw = dynamic_cast<SpecialDisplayWidget*>( FindChild( "unit_level", true ) );
   if ( sdw )
     sdw->display.connect( SigC::slot( *this, &UnitInfoPanel::painter ));




}


void UnitInfoPanel::painter ( const PG_Rect &src, const ASCString& name, const PG_Rect &dst)
{
   MapCoordinate mc = actmap->player[actmap->actplayer].cursorPos;
   int weaponsDisplayed = 0;

   if ( mc.valid() ) {
      Vehicle* veh = actmap->getField(mc)->vehicle;
      Surface screen = Surface::Wrap( PG_Application::GetScreen() );

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
            if ( height1 & (1 << i ))
               screen.Blit( IconRepository::getIcon("height-b" + ASCString::toString(i) + ".png"), SPoint(dst.x, dst.y + (7-i) * 13) );
            else
               if ( height2 & (1 << i ))
                  screen.Blit( IconRepository::getIcon("height-a" + ASCString::toString(i) + ".png"), SPoint(dst.x, dst.y + (7-i) * 13 ) );
         }
      }

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

void UnitInfoPanel::eval()
{
   MapCoordinate mc = actmap->player[actmap->actplayer].cursorPos;
   int weaponsDisplayed = 0;

   if ( mc.valid() ) {
      Vehicle* veh = actmap->getField(mc)->vehicle;
      if ( veh ) {
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

         int &pos = weaponsDisplayed;
         for ( int i = 0; i < veh->typ->weapons.count; ++i) {
            if ( !veh->typ->weapons.weapon[i].service() && pos < 10 ) {
                ASCString ps = ASCString::toString(pos);
                setLabelText( "punch_weapon" + ps, veh->typ->weapons.weapon[i].maxstrength );
                if ( veh->typ->functions & cfno_reactionfire )
                   setLabelText( "RF_weapon" + ps, "-" );
                else
                   setLabelText( "RF_weapon" + ps, veh->typ->weapons.weapon[i].reactionFireShots );
                setLabelText( "status_ammo" + ps, veh->ammo[i] );
                setBargraphValue( "bar_ammo" + ps, veh->typ->weapons.weapon[i].count ? float(veh->ammo[i]) / veh->typ->weapons.weapon[i].count : 0 );
                ++pos;
             }
          }


      } else {
         setLabelText( "unittypename", "" );
         setLabelText( "unitname", "" );
         setBargraphValue( "unitdamage", 0  );
         setLabelText( "unitstatus", "" );
         setBargraphValue( "unitfuel", 0  );
         setLabelText( "unitfuelstatus", "" );
         setBargraphValue( "unitmaterial",  0  );
         setLabelText( "unitmaterialstatus", "" );
         setBargraphValue( "unitenergy",  0  );
         setLabelText( "unitenergystatus", "" );
      }
      for ( int i = weaponsDisplayed; i < 10; ++i ) {
          ASCString ps = ASCString::toString(i);
          setLabelText( "punch_weapon" + ps, "" );
          setLabelText( "RF_weapon" + ps, "" );
          setLabelText( "status_ammo" + ps, "" );
          setBargraphValue( "bar_ammo" + ps, 0 );
       }

   }
   Redraw(true);                  
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


#if 0
         tdashboard  dashboard;

tdashboard::tdashboard ( void ) : vehicletype(NULL)
{
   fuelbkgr  = NULL;
   imagebkgr = NULL;
   movedisp  = 0;
   windheightshown = 0;
   for ( int i = 0; i< 8; i++ )
      weaps[i].displayed = 0;
   repainthard = 1;
   materialdisplayed = 1;
}

void tdashboard::paint ( const pfield ffield, int playerview )
{
   if ( playerview >= 0 ) {
      if (fieldvisiblenow(ffield, playerview ))
         paintvehicleinfo ( ffield->vehicle, ffield->building, ffield, NULL );
      else
         paintvehicleinfo( NULL, NULL, NULL, NULL );
   }
}

void         tdashboard::putheight(integer      i, integer      sel)
                                      //          h�he           m�glichk.
{
   putrotspriteimage ( agmp->resolutionx - ( 640 - 589), 92  + i * 13, icons.height2[sel][i], actmap->actplayer * 8);
}






void         tdashboard::paintheight(void)
{
   if ( vehicle )
          for ( int i = 0; i <= 7; i++) {
             if (vehicle->typ->height & (1 << (7 - i)))
                if (vehicle->height & (1 << (7 - i)))
                  putheight(i,1);
                else
                  putheight(i,2);
             else
               putheight(i,0);
          }

    else
       if ( vehicletype ) {
          for ( int i = 0; i <= 7; i++) {
             if (vehicletype->height & (1 << (7 - i)))
               putheight(i,2);
             else
               putheight(i,0);
          }
       } else
          for ( int i = 0; i <= 7; i++)
              putheight(i,0);
}



void         tdashboard::painttank(void)
{
    int         w;
    int         c;

    int x1 = agmp->resolutionx - ( 640 - 520);
    int x2 = agmp->resolutionx - ( 640 - 573);
    int y1 = 59;
    int y2 = 67;

    if ( vehicle )
       if ( vehicle->typ->tank.fuel )
          w = ( x2 - x1 + 1) * vehicle->getTank().fuel / vehicle->typ->tank.fuel;
       else
          w = 0;
    else
      w = 0;

    if (w < ( x2 - x1 + 1) )
       bar( x1 + w , y1 , x2, y2 , 172);

    c = vgcol;
    if (w < 25)
       c = 14;
    if (w < 15)
       c = red;
    if ( w )
       bar(x1, y1, x1 + w - 1 , y2 ,c);

    putspriteimage ( x1, y1, fuelbkgr );
}


char*         tdashboard:: str_2 ( int num )
{
      char* tmp;

      if ( num >= 1000000 ) {
         tmp = strrr ( num / 1000000 );
         strcat ( tmp, "M");
         return tmp;
      } else
         if ( num >= 10000 ) {
            tmp = strrr ( num / 1000 );
            strcat ( tmp, "k");
            return tmp;
         } else
            return strrr ( num );
}


void         tdashboard::paintweaponammount(int h, int num, int max, bool dash )
{
      int         w;

      w = 20 * num / max;
      if (w > 0)
         bar( agmp->resolutionx - ( 640 - 552),     93 + h * 13, agmp->resolutionx - ( 640 - 551 ) + w, 101 + h * 13, 168 );
      if (w < 20)
         bar( agmp->resolutionx - ( 640 - 552) + w, 93 + h * 13, agmp->resolutionx - ( 640 - 571 ),     101 + h * 13, 172 );

      activefontsettings.justify = righttext;
      activefontsettings.font = schriften.guifont;
      activefontsettings.height = 9;
      activefontsettings.background = 255;
      activefontsettings.length = 19;

      if ( dash )
         showtext2c( "-", agmp->resolutionx - ( 640 - 552), 93 + h * 13);
      else
         showtext2c( str_2( num ), agmp->resolutionx - ( 640 - 552), 93 + h * 13);
}


void         tdashboard::paintweapon(int         h, int num, int strength,  const SingleWeapon  *weap )
{
      if ( weap->getScalarWeaponType() >= 0 ) {
         void* img;
         if ( weap->gettype() & cwlaserb )
            img = icons.unitinfoguiweapons[ 13 ] ;
         else
            img = icons.unitinfoguiweapons[ weap->getScalarWeaponType() ] ;

         if ( weap->canRefuel() )
            putimage ( agmp->resolutionx - ( 640 - 465), 93 + h * 13, xlatpict ( &xlattables.a.light, img));
         else
            putimage ( agmp->resolutionx - ( 640 - 465), 93 + h * 13, img );
      } else
         if ( weap->service() )
            putimage ( agmp->resolutionx - ( 640 - 465), 93 + h * 13, icons.unitinfoguiweapons[ cwservicen ] );


      paintweaponammount( h, num, weap->count );


      activefontsettings.background = 172;
      activefontsettings.justify = righttext;
      activefontsettings.font = schriften.guifont;
      activefontsettings.height = 9;
      activefontsettings.length = 27;
      if ( weap->shootable() ) {
         showtext2c( strrr(strength), agmp->resolutionx - ( 640 - 503), 93 + h * 13);

         weaps[h].displayed = 1;
         weaps[h].maxstrength = int(strength * weapDist.getWeapStrength(weap, weap->mindistance, -1, -1 ));
         weaps[h].minstrength = int(strength * weapDist.getWeapStrength(weap, weap->maxdistance, -1, -1 ));
         weaps[h].mindist = weap->mindistance;
         weaps[h].maxdist = weap->maxdistance;

      } else {
         bar( agmp->resolutionx - ( 640 - 503),  93 + h * 13 ,agmp->resolutionx - ( 640 - 530), 101 + h * 13, 172 );
         weaps[h].displayed = 0;
      }



}




void         tdashboard::paintweapons(void)
{
   memset ( weaps, 0, sizeof ( weaps ));

   int i, j;

   activefontsettings.color = black;
   activefontsettings.background = 255;
   activefontsettings.justify = lefttext;
   activefontsettings.font = font;

   int serv = 0;


   int xp = agmp->resolutionx - ( 640 - 465);

   activefontsettings.justify = righttext;
   i = 0;
   int k = 7;

   const Vehicletype* vt;
   if ( vehicle )
      vt = vehicle->typ;
   else
      vt = vehicletype;

    if ( vt ) {
       if ( vt->weapons.count )
          for (j = 0; j < vt->weapons.count && j < 8; j++) {
             if ( vt->weapons.weapon[j].count ) {
                paintweapon(i, ( vehicle ? vehicle->ammo[j] : vt->weapons.weapon[j].count ), ( vehicle ? vehicle-> weapstrength[j] : vt->weapons.weapon[j].maxstrength ), &vt->weapons.weapon[j] );
                i++;
             }
             else {
                if ( vt->weapons.weapon[j].service() ) {
                   serv = 1;
                   if ( materialdisplayed )
                      if ( vt->tank.fuel ) {
                         putimage ( xp, 93 + k * 13, xlatpict ( &xlattables.a.light, icons.unitinfoguiweapons[ 8 ] ));
                         paintweaponammount ( k, ( vehicle ? vehicle->getTank().fuel : vt->tank.fuel ), vt->tank.fuel );
                         k--;
                      }
                }
             }
          }

       if ( materialdisplayed ) {
          if ( vt->tank.material ) {
             if ( serv )
                putimage ( xp, 93 + k * 13, xlatpict ( &xlattables.a.light, icons.unitinfoguiweapons[ 11 ] ));
             else
                putimage ( xp, 93 + k * 13, icons.unitinfoguiweapons[ 11 ] );
              paintweaponammount ( k, ( vehicle ? vehicle->getTank().material : vt->tank.material ), vt->tank.material );
              k--;
          }
          if ( vt->tank.energy ) {
             if ( serv )
                putimage ( xp, 93 + k * 13, xlatpict ( &xlattables.a.light, icons.unitinfoguiweapons[ 9 ] ));
             else
                putimage ( xp, 93 + k * 13, icons.unitinfoguiweapons[ 9 ] );

              if ( vehicle && vehicle->getGeneratorStatus() )
                  paintweaponammount ( k, ( vehicle ? vehicle->getTank().energy : vt->tank.energy ), vt->tank.energy );
              else
                  paintweaponammount ( k, 0, vt->tank.energy, true );
              k--;
          }
       }
    }

    for (j = i; j <= k; j++) {
       putimage( xp, 93 + j * 13, icons.unitinfoguiweapons[12]);
       bar( agmp->resolutionx - ( 640 - 552),  93 + j * 13 ,agmp->resolutionx - ( 640 - 571), 101 + j * 13, 172 );
       bar( agmp->resolutionx - ( 640 - 503),  93 + j * 13 ,agmp->resolutionx - ( 640 - 530), 101 + j * 13, 172 );
    }
}

void         tdashboard :: paintlweaponinfo ( void )
{
   paintlargeweaponinfo();
}

void         tdashboard :: paintlargeweaponinfo ( void )
{
   int i = 0;
   for ( int lw = 0; lw < 16; lw++ )
      largeWeaponsDisplayPos[lw] = -1;

   int serv = -1;
   const Vehicletype* vt;
   if ( vehicle )
      vt = vehicle->typ;
   else
      vt = vehicletype;
   if ( vt ) {
       npush ( activefontsettings );

       int x1 = (agmp->resolutionx - 640) / 2;
       int y1 = 150;

       int count = 0;
       if ( vt->weapons.count )
          for ( int j = 0; j < vt->weapons.count ; j++)
             if ( vt->weapons.weapon[j].getScalarWeaponType() >= 0 )
                count++;
             else
                if (vt->weapons.weapon[j].service() )
                   serv = count;


       if ( serv >= 0 )
          count++;

       if ( vt->tank.energy )
          count++;

       int funcs;
       if ( vehicle )
          funcs = vehicle->typ->functions;
       else
          funcs  = vt->functions;


       if ( (serv>= 0 || (funcs & cfmaterialref)) && vt->tank.material )
          count++;

       if ( (serv>= 0 || (funcs & cffuelref)) && vt->tank.fuel )
          count++;

       count++;

       void* imgbuf = asc_malloc ( imagesize ( x1, y1, x1 + 640, y1 + count * 25 + 110 ));
       getimage ( x1, y1, x1 + 640, y1 + count * 25 + 110, imgbuf );

       putimage ( x1, y1, icons.weaponinfo[0] );

       if ( vt->weapons.count )
          for ( int j = 0; j < vt->weapons.count ; j++) {
             if ( vt->weapons.weapon[j].getScalarWeaponType() >= 0 ) {
                int maxstrength = vt->weapons.weapon[j].maxstrength;
                int minstrength = vt->weapons.weapon[j].minstrength;
                if ( vehicle && maxstrength ) {
                   minstrength = minstrength * vehicle->weapstrength[j] / maxstrength;
                   maxstrength = vehicle->weapstrength[j];
                }

                paintlargeweapon(i, cwaffentypen[ vt->weapons.weapon[j].getScalarWeaponType() ],
                               ( vehicle ? vehicle->ammo[j] : vt->weapons.weapon[j].count ) , vt->weapons.weapon[j].count,
                               vt->weapons.weapon[j].shootable(), vt->weapons.weapon[j].canRefuel(),
                               maxstrength,
                               minstrength,
                               vt->weapons.weapon[j].maxdistance,
                               vt->weapons.weapon[j].mindistance,
                               vt->weapons.weapon[j].sourceheight & vt->height,
                               vt->weapons.weapon[j].targ );
                largeWeaponsDisplayPos[i] = j;
                i++;
             }
          }

       if ( serv >= 0 ) {
          paintlargeweapon(i, cwaffentypen[ cwservicen ], -1, -1, -1, -1, -1, -1,
                         vt->weapons.weapon[serv].maxdistance, vt->weapons.weapon[serv].mindistance,
                         vt->weapons.weapon[serv].sourceheight, vt->weapons.weapon[serv].targ );
          largeWeaponsDisplayPos[i] = serv;
          i++;
       }
       if ( vt->tank.energy ) {
          paintlargeweapon(i, resourceNames[ 0 ], ( vehicle ? vehicle->getTank().energy : vt->tank.energy ), vt->tank.energy, -1, -1, -1, -1, -1, -1, -1, -1 );
          largeWeaponsDisplayPos[i] = -1;
          i++;
       }

       if ( (serv>= 0 || (funcs & cfmaterialref)) && vt->tank.material ) {
          paintlargeweapon(i, resourceNames[ 1 ], ( vehicle ? vehicle->getTank().material : vt->tank.material ), vt->tank.material, -1, -1, -1, -1, -1, -1, -1, -1 );
          largeWeaponsDisplayPos[i] = -1;
          i++;
       }
       if ( (serv>= 0 || (funcs & cffuelref)) && vt->tank.fuel ) {
          paintlargeweapon(i, resourceNames[ 2 ], ( vehicle ? vehicle->getTank().fuel : vt->tank.fuel ), vt->tank.fuel, -1, -1, -1, -1, -1, -1, -1, -1 );
          largeWeaponsDisplayPos[i] = -1;
          i++;
       }

      {
         int x = x1;
         int y = y1 + i * 14 + 28;

         int height, width;
         getpicsize ( icons.weaponinfo[4], width, height );

         putspriteimage ( x, y, icons.weaponinfo[4] );

         activefontsettings.justify = centertext;
         activefontsettings.font = schriften.guifont;
         activefontsettings.height = 11;
         activefontsettings.length = 80;
         activefontsettings.background = 255;
         if ( vt->wait )
            showtext2c ( "no", x + 140, y +  2 );
         else
            showtext2c ( "yes", x + 140, y +  2 );

         if ( funcs & cf_moveafterattack )
            showtext2c ( "yes", x + 364, y +  2 );
         else
            showtext2c ( "no", x + 364, y +  2 );

         i++;
      }



      int lastpainted = -1;
      int first = 1;
      while ( mouseparams.taste == 2) {
         int topaint  = -1;
         int serv = 0;
         for ( int j = 0; j < vt->weapons.count ; j++) {
            int x = (agmp->resolutionx - 640) / 2;
            int y = 150 + 28 + (j - serv) * 14;
            if ( mouseinrect ( x, y, x + 640, y+ 14 ))
               if ( largeWeaponsDisplayPos[j] != -1 )
                  topaint = largeWeaponsDisplayPos[j];

         }
         if ( topaint != lastpainted ) {
            if ( topaint == -1 )
               paintlargeweaponefficiency ( i, NULL, first, NULL );
            else {
               int effic[13];
               for ( int k = 0; k < 13; k++ )
                  effic[k] = vt->weapons.weapon[topaint].efficiency[k];
               int mindelta = 1000;
               int maxdelta = -1000;
               for ( int h1 = 0; h1 < 8; h1++ )
                  for ( int h2 = 0; h2 < 8; h2++ )
                     if ( vt->weapons.weapon[topaint].sourceheight & ( 1 << h1 ))
                        if ( vt->weapons.weapon[topaint].targ & ( 1 << h2 )) {
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

               paintlargeweaponefficiency ( i, effic, first, vt->weapons.weapon[topaint].targetingAccuracy );
            }
            lastpainted = topaint;
            first = 0;
         }
         releasetimeslice();
      }

      putimage ( x1, y1, imgbuf );

      asc_free  ( imgbuf );

      npop ( activefontsettings );
   }

}

void         tdashboard::paintlargeweaponefficiency ( int pos, int* e, int first, const int* hit )
{
   int x = (agmp->resolutionx - 640) / 2;
   int y = 150 + 28 + pos * 14;

   int height, width;
   getpicsize ( icons.weaponinfo[3], width, height );

   if ( first )
      putspriteimage ( x, y, icons.weaponinfo[3] );

   static int bk1 = -1;
   static int bk2 = -1;
   if ( bk1 == -1 )
      bk1 = getpixel ( x + 100, y + 5 );
   if ( bk2 == -1 )
      bk2 = getpixel ( x + 100, y + 19);

   activefontsettings.justify = centertext;
   activefontsettings.font = schriften.guifont;
   activefontsettings.height = 10;
   activefontsettings.length = 36;

/*   int maxunitheigt = 0;
   int minunitheight = 1000;
   int maxtargetheight = 0;
   int mintargetheight = 1000; */


   for ( int i = 0; i < 13; i++ )
      if ( e && e[i] != -1 ) {
         activefontsettings.background = bk1;
         showtext2c ( strrr ( i - 6 ), x + 88 + i * 42, y +  2 );
         activefontsettings.background = bk2;
         showtext4c ( "%s%%", x + 88 + i * 42, y + 15, strrr ( e[i] ) );
      } else {
         activefontsettings.background = bk1;
         showtext2c ( "", x + 88 + i * 42, y +  2 );
         activefontsettings.background = bk2;
         showtext2c ( "",  x + 88 + i * 42, y + 15 );
     }

   activefontsettings.length = 179;
   // activefontsettings.background = white;
   activefontsettings.color = 86;
   activefontsettings.justify = lefttext;
   // activefontsettings.color = black;
   for ( int j = 0; j < cmovemalitypenum; j++ ) {
      int xp = x + 88 + (j % 3) * 180;
      int yp = y + 15 + 16 + (j / 3) * 12;
      if ( hit ) {
         ASCString s = ASCString(cmovemalitypes[j]) + "=" + strrr ( hit[j] ) + "%";
         if ( hit[j] == 100 ) {
            activefontsettings.font = schriften.guifont;
            showtext2c (  s, xp, yp );
         } else
            if ( hit[j] > 0 && hit[j] < 100 ) {
               activefontsettings.font = schriften.guicolfont;
               showtext2c (  s, xp, yp );
            } else
               if ( hit[j] > 100 ) {
                  activefontsettings.color = 26;
                  activefontsettings.font = schriften.monogui;
                  showtext2 (  s, xp, yp );
               } else {
                  activefontsettings.color = 86;
                  activefontsettings.font = schriften.monogui;
                  showtext2 (  s, xp, yp );
               }
      } else {
         activefontsettings.font = schriften.monogui;
         showtext2 ( "-", xp, yp );
      }
   }
   activefontsettings.font = schriften.guifont;
}


void         tdashboard::paintlargeweapon ( int pos, const char* name, int ammoact, int ammomax, int shoot, int refuel, int strengthmax, int strengthmin, int distmax, int distmin, int from, int to )
{
   int height, width;
   getpicsize ( icons.weaponinfo[1], width, height );



   int x = (agmp->resolutionx - 640) / 2;
   int y = 150 + 28 + pos * 14;

   putspriteimage ( x, y, icons.weaponinfo[1] );
   y += 4;

   activefontsettings.background = 255;
   activefontsettings.font = schriften.guifont;
   activefontsettings.height = 11;
   activefontsettings.length = 75;

   if ( name ) {
      activefontsettings.justify = lefttext;
      activefontsettings.length = 75;
      showtext2c ( name, x + 2, y );
   }

   if ( ammoact >= 0 ) {
      activefontsettings.length = 20;
      char buf[100];
      char buf2[100];
      char buf3[100];
      sprintf(buf3, "%s / %s", int2string ( ammoact, buf ), int2string ( ammomax, buf2 ) );
      activefontsettings.length = 50;
      activefontsettings.justify = centertext;
      showtext2c ( buf3, x + 77, y );
   }

   if ( shoot >= 0 ) {
      activefontsettings.justify = centertext;
      activefontsettings.length = 25;
      if ( shoot )
         showtext2c ( "yes", x + 130, y );
      else
         showtext2c ( "no", x + 130, y );
   }

   if ( refuel >= 0 ) {
      activefontsettings.justify = centertext;
      activefontsettings.length = 25;
      if ( refuel )
         showtext2c ( "yes", x + 158, y );
      else
         showtext2c ( "no", x + 158, y );
   }

   if ( strengthmax >= 0 ) {
      activefontsettings.justify = lefttext;
      activefontsettings.length = 38;
      showtext2c ( strrr( strengthmax ), x + 190, y );
   }

   if ( strengthmin >= 0 ) {
      activefontsettings.length = 38;
      activefontsettings.justify = righttext;
      showtext2c ( strrr( strengthmin ), x + 190, y );
   }

   if ( distmin >= 0 ) {
      activefontsettings.length = 36;
      activefontsettings.justify = lefttext;
      showtext2c ( strrrd8u( distmin ), x + 237, y );
   }

   if ( distmax >= 0 ) {
      activefontsettings.length = 36;
      activefontsettings.justify = righttext;
      showtext2c ( strrrd8d( distmax ), x + 237, y );
   }


   if ( from > 0 )
      for ( int i = 0; i < 8; i++ )
         if ( from & ( 1 << i ))
            putimage ( x + 285 + i * 22, y-2, icons.weaponinfo[2] );

   if ( to > 0 )
      for ( int i = 0; i < 8; i++ )
         if ( to & ( 1 << i ))
            putimage ( x + 465 + i * 22, y-2, icons.weaponinfo[2] );

   activefontsettings.justify = lefttext;

}




void         tdashboard::allocmem ( void )
{
    int x1 = 520;
    int x2 = 573;
    int y1 = 71;
    int y2 = 79;

    fuelbkgrread = 0;
    fuelbkgr = new char[  imagesize ( x1, y1, x2, y2 ) ] ;

    x1 = 460;
    y1 = 31;

    imagebkgr = new char[  imagesize ( x1, y1, x1 + 30, y1 + 30 ) ];
    imageshown = 10;

}


void         tdashboard::paintdamage(void)
{
    int x1 = agmp->resolutionx - ( 640 - 520);
    int x2 = agmp->resolutionx - ( 640 - 573);
    int y1 = 71;
    int y2 = 79;

    if ( fuelbkgrread == 0 ) {
       getimage ( x1, y1, x2, y2, fuelbkgr );
       int sze = imagesize ( x1, y1, x2, y2 );
       char *pc = (char*) fuelbkgr;
       int cl  = getpixel ( agmp->resolutionx - 81, 63 );
       for ( int m = 4; m < sze; m++ )
          if ( pc[m] == cl )
             pc[m] = 255;
       fuelbkgrread = 1;
    }


    int w = 0;
    int         c;



    if ( vehicle ) {
       w = (x2 - x1 + 1) * ( 100 - vehicle->damage ) / 100;
       if ( w > 23 )       // container :: subwin :: buildinginfo :: damage verwendet die selben Farben
          c = vgcol;
       else
          if ( w > 15 )
             c = yellow;
          else
             if ( w > 7 )
                c = lightred;
             else
                c = red;
    } else
       if ( building ) {
          w = (x2 - x1 + 1) * ( 100 - building->damage ) / 100;
          if (building->damage >= mingebaeudeeroberungsbeschaedigung)
             c = red;
          else
             c = vgcol;

       }
       else
          if ( objfield ) {
             c = darkgray;
             for ( tfield::ObjectContainer::iterator i = objfield->objects.begin(); i != objfield->objects.end(); i++ )
               if ( i->typ->armor > 0 )
                  w = (x2 - x1 + 1) * ( 100 - i->damage ) / 100;

          } else
             w = 0;


    if (w < (x2 - x1 + 1) )
       bar( x1 + w , y1 , x2, y2 , 172);

    if ( w )
       bar(x1, y1, x1 + w - 1, y2 ,c);

    putspriteimage ( x1, y1, fuelbkgr );
}




void         tdashboard::paintexperience(void)
{
    if (vehicle)
       putimage( agmp->resolutionx - ( 640 - 587),  27, icons.experience[vehicle->experience]);
    else
       bar( agmp->resolutionx - ( 640 - 587), 27,agmp->resolutionx - ( 640 - 611), 50, 171);
}




void         tdashboard::paintmovement(void)
{
    if ( vehicle ) {
       activefontsettings.justify = centertext;
       activefontsettings.color = white;
       activefontsettings.background = 172;
       activefontsettings.font = schriften.guifont;
       activefontsettings.length = 17;
       activefontsettings.height = 9;
       if ( vehicle->typ->fuelConsumption ) {
          if ( movedisp  || (minmalq*vehicle->getTank().fuel / vehicle->typ->fuelConsumption  < vehicle->getMovement() ))
             showtext2c( strrrd8d( minmalq*vehicle->getTank().fuel / vehicle->typ->fuelConsumption ), agmp->resolutionx - ( 640 - 591), 59);
          else
             showtext2c( strrrd8d(vehicle->getMovement() ), agmp->resolutionx - ( 640 - 591), 59);
       } else
          if ( movedisp )
             showtext2c( "-", agmp->resolutionx - ( 640 - 591), 59);
          else
             showtext2c( strrrd8d( vehicle->getMovement() ), agmp->resolutionx - ( 640 - 591), 59);
    } else
       bar( agmp->resolutionx - ( 640 - 591), 59,agmp->resolutionx - ( 640 - 608), 67, 172);
}

void         tdashboard::paintarmor(void)
{
    if ( vehicle || vehicletype ) {
       activefontsettings.justify = centertext;
       activefontsettings.color = white;
       activefontsettings.background = 172;
       activefontsettings.font = schriften.guifont;
       activefontsettings.length = 18;
       activefontsettings.height = 9;
       int arm;
       if ( vehicle )
          arm = vehicle->armor;
       else
          arm = vehicletype->armor;

       showtext2c( strrr(arm),agmp->resolutionx - ( 640 - 591), 71);
    } else
       bar(agmp->resolutionx - ( 640 - 591), 71,agmp->resolutionx - ( 640 - 608), 79, 172);
}

void         tdashboard::paintwind( int repaint )
{
  int j, i;

/*   void *p;
   if (actmap->weather.wind.direction & 1)
      p = icons.wind.southwest[actmap->weather.wind.speed >> 6];
   else
      p = icons.wind.south[actmap->weather.wind.speed >> 6];

   switch (actmap->weather.wind.direction >> 1) {
      case 0: putimage(430,320,p);
         break;
      case 1: putrotspriteimage90(430,320,p,0);
         break;
      case 2: putrotspriteimage180(430,320,p,0);
         break;
      case 3: putrotspriteimage270(430,320,p,0);
         break;
   }

   activefontsettings.justify = centertext;
   activefontsettings.color = black;
   activefontsettings.background = white;
   activefontsettings.font = schriften.smallarial;
   activefontsettings.length = 30;
   showtext2( strrr(actmap->weather.wind.speed),430,354);

   */

   if ( !CGameOptions::Instance()->smallmapactive ) {
   #if 0
      static int lastdir = -1;

      if ( repaint ) {
         putimage ( agmp->resolutionx - ( 640 - 450), 211, icons.windbackground );
         lastdir = -1;
      }


      if ( !windheightshown ) {
         int x1 = agmp->resolutionx - ( 640 - 489 );
         int x2 = agmp->resolutionx - ( 640 - 509 );
         int y1 = 284;
         int y2 = 294;
         windheightbackground = new char [imagesize ( x1, y1, x2, y2 )];
         getimage ( x1, y1, x2, y2, windheightbackground );
         windheightshown = 1;
      }


      int unitspeed;
      if ( vehicle )
         unitspeed = getmaxwindspeedforunit ( vehicle );
      else
         unitspeed = maxint;

      if ( actmap->weather.windSpeed ) {
          if ( lastdir != actmap->weather.windDirection ) {
             putimage ( agmp->resolutionx - ( 640 - 506), 227, icons.wind[ 8 ] );
             char* pic = rotatepict_grw ( icons.windarrow, directionangle[ actmap->weather.windDirection ] );
             int h1,w1, h2, w2;
             getpicsize ( pic, w2, h2 );
             getpicsize ( icons.wind[ 8 ], w1, h1 );
             putspriteimage ( agmp->resolutionx - ( 640 - (506 + w1/2 - w2/2)), 227 + h1/2- h2/2, pic );
             delete[] pic;
             lastdir = actmap->weather.windDirection;
          }
      } else
         putimage ( agmp->resolutionx - ( 640 - 506), 227, icons.wind[ 8 ] );

      for (i = 0; i < (actmap->weather.windSpeed+31) / 32 ; i++ ) {
         int color = green;

         if ( vehicle == NULL ) {
            /*
            if ( i >= 6 )
               color = red;
            else
               if ( i >= 4 )
                  color = yellow;
             */
         } else {
             int windspeed = actmap->weather.windSpeed*maxwindspeed ;
             if ( unitspeed < 255*256 )
                if ( windspeed > unitspeed*9/10 )
                   color = red;
                else
                   if ( windspeed > unitspeed*66/100 )
                     color = yellow;
         }
         bar ( agmp->resolutionx - ( 640 - 597), 282-i*7, agmp->resolutionx - ( 640 - 601), 284-i*7, color );
      } /* endfor */
      for (j = i; j < 8; j++ )
         bar ( agmp->resolutionx - ( 640 - 597), 282-j*7, agmp->resolutionx - ( 640 - 601), 284-j*7, black );
#endif
   }
}




// template<int pixelSize> class BBind : public SourcePixelSelector_Rotation< pixelSize, SourcePixelSelector_Zoom<pixelSize> >  {};


void         tdashboard::paintimage(void)
 {

    int x1 = agmp->resolutionx - ( 640 - 460);
    int y1 = 31;
    if ( imageshown == 10 ) {
       getimage ( x1, y1, x1 + 30, y1 + 30 , imagebkgr );
       imageshown = 0;
    }

    if ( imageshown )
       putimage ( x1, y1, imagebkgr );

    if ( vehicle ) {
       // vehicle->paint( getActiveSurface(), SPoint( x1, y1) );

       MegaBlitter<1,1,ColorTransform_PlayerCol, ColorMerger_AlphaOverwrite, SourcePixelSelector_Zoom > blitter;
       blitter.setZoom( 0.8 );
       blitter.setPlayer( vehicle->getOwner() );
       blitter.blit ( vehicle->typ->getImage(), getActiveSurface(), SPoint(x1,y1) );

       imageshown = 1;
    } else
       imageshown = 0;
}


void         tdashboard::paintclasses ( void )
{
   if ( CGameOptions::Instance()->showUnitOwner ) {
      const char* owner = NULL;
      if ( vehicle )
         owner = actmap->getPlayerName(vehicle->color / 8).c_str();
      else
         if ( building )
            owner = actmap->getPlayerName(building->color / 8).c_str();

      if ( owner ) {
         activefontsettings.justify = lefttext;
         activefontsettings.color = white;
         activefontsettings.background = 171;
         activefontsettings.font = schriften.guifont;
         activefontsettings.length = 75;
         activefontsettings.height = 0;
         showtext2c( owner, agmp->resolutionx - ( 640 - 500), 42);
         activefontsettings.height = 9;
      } else
         bar ( agmp->resolutionx - ( 640 - 499), 42, agmp->resolutionx - ( 640 - 575), 50, 171 );
   } else {
      const Vehicletype* vt;
      if ( vehicle )
         vt = vehicle->typ;
      else
         vt = vehicletype;

      if ( vt && !vt->description.empty() ) {
         activefontsettings.justify = lefttext;
         activefontsettings.color = white;
         activefontsettings.background = 171;
         activefontsettings.font = schriften.guifont;
         activefontsettings.length = 75;
         activefontsettings.height = 0;
         showtext2c( vt->description ,agmp->resolutionx - ( 640 - 500 ), 42);
         activefontsettings.height = 9;
     } else
         bar ( agmp->resolutionx - ( 640 - 499), 42, agmp->resolutionx - ( 640 - 575), 50, 171 );
   }
}

void         tdashboard::paintname ( void )
{
   if ( vehicle || building || vehicletype) {
      activefontsettings.justify = lefttext;
      activefontsettings.color = white;
      activefontsettings.background = 171;
      activefontsettings.font = schriften.guifont;
      activefontsettings.length = 75;
      activefontsettings.height = 9;
      const Vehicletype* vt;
      if ( vehicle )
         vt = vehicle->typ;
      else
         vt = vehicletype;

      if ( vehicle || vt ) {
         if ( vehicle && !vehicle->name.empty() )
            showtext2c( vehicle->name.c_str() , agmp->resolutionx - ( 640 - 500 ), 27);
         else
            if ( !vt->name.empty() )
               showtext2c( vt->name , agmp->resolutionx - ( 640 - 500 ), 27);
            else
               /*
               if ( !vt->description.empty() )
                  showtext2c( vt->description ,agmp->resolutionx - ( 640 - 500 ), 27);
               else
               */
               bar ( agmp->resolutionx - ( 640 - 499 ), 27, agmp->resolutionx - ( 640 - 575 ), 35, 171 );

      } else
         showtext2c( building->getName(), agmp->resolutionx - ( 640 - 500), 27);

      activefontsettings.height = 0;
   } else
      bar ( agmp->resolutionx - ( 640 - 499), 27, agmp->resolutionx - ( 640 - 575), 35, 171 );
}

void         tdashboard::paintplayer( void )
{
   putspriteimage ( agmp->resolutionx - ( 640 - 540), 127, icons.player[actmap->actplayer] );
}


void         tdashboard::paintalliances ( void )
{
   int j = 0;
   for (int i = 0; i< 8 ; i++ ) {
      if ( i != actmap->actplayer ) {
         if ( actmap->player[i].exist() ) {
            if ( getdiplomaticstatus ( i*8 ) == capeace )
               putimage ( agmp->resolutionx - ( 640 - 476) + j * 19, agmp->resolutiony - ( 480 - 452), icons.allianz[i][0] );
            else
               putimage ( agmp->resolutionx - ( 640 - 476) + j * 19, agmp->resolutiony - ( 480 - 452), icons.allianz[i][1] );
         } else
             putimage ( agmp->resolutionx - ( 640 - 476) + j * 19, agmp->resolutiony - ( 480 - 452), icons.allianz[i][2] );
         j++;
      }
   } /* endfor */
}

void         tdashboard::paintzoom( void )
{
   int h;
   getpicsize ( zoom.pic, zoom.picwidth, h );
   zoom.x1 = agmp->resolutionx - ( 640 - 464);
   zoom.x2 = agmp->resolutionx - ( 640 - 609);
   zoom.y1 = agmp->resolutiony - ( 480 - 444);
   zoom.y2 = agmp->resolutiony - ( 480 - 464);

   collategraphicoperations cgo  ( zoom.x1, zoom.y1, zoom.x2, zoom.y2 );

   static void* background = NULL;
   if ( !background ) {
      background = asc_malloc ( imagesize ( zoom.x1, zoom.y1, zoom.x2, zoom.y2 ));
      getimage ( zoom.x1, zoom.y1, zoom.x2, zoom.y2, background );
   } else
      putimage ( zoom.x1, zoom.y1, background );

   int actzoom = zoomlevel.getzoomlevel() - zoomlevel.getminzoom();
   int maxzoom = zoomlevel.getmaxzoom() - zoomlevel.getminzoom();
   int dist = zoom.x2 - zoom.picwidth - zoom.x1;
   putimage ( zoom.x1 + dist - dist * actzoom / maxzoom, zoom.y1, zoom.pic );

}


class tmainshowmap : public tbasicshowmap {
          public:
            void checkformouse ( void );
       };

void tmainshowmap :: checkformouse ( void )
{
   if ( mouseparams.taste == 1 ) {
       int oldx = dispxpos;
       int oldy = dispypos;
       tbasicshowmap :: checkformouse();
       if ( oldx != dispxpos  ||  oldy != dispypos ) {
          setmapposition();
          displaymap();
       }
   }
}

tmainshowmap* smallmap = NULL;

void         tdashboard::paintsmallmap ( int repaint )
{
   if ( !smallmap ) {
      smallmap = new tmainshowmap;
      CGameOptions::Instance()->smallmapactive = 1;
      CGameOptions::Instance()->setChanged();
      repaint = 1;
   }

   if ( repaint )
      bar ( agmp->resolutionx - ( 800 - 612), 213, agmp->resolutionx - ( 800 - 781), 305, greenbackgroundcol );

   smallmap->init ( agmp->resolutionx - ( 800 - 612 ) , 213, 781-612, 305-213 );
   smallmap->generatemap ( 1 );
   smallmap->dispimage ( );

}

void         tdashboard::checkformouse ( int func )
{
    if ( vehicle ) {
       // tmouserect experience = { 0,0,0,0};
       tmouserect experience;
       experience.x1 = agmp->resolutionx - ( 640 - 587);
       experience.y1 = 27;
       experience.x2 = agmp->resolutionx - ( 640 - 587) + 25;
       experience.y2 = 27 + 24 ;

       if ( mouseinrect ( &experience ) && mouseparams.taste != 0 ) {
          int xnum = 4;
          int ynum = 6;
          int xwidth;
          int ywidth;
          getpicsize ( icons.experience[0], xwidth, ywidth );
          tmouserect r;
          r.x1 = experience.x2 - xnum*(xwidth+2)-2;
          r.y1 = experience.y1;
          r.x2 = experience.x2;
          r.y2 = experience.y1 + (ywidth+2)*ynum+2 ;
          void* p = asc_malloc ( imagesize ( r.x1, r.y1, r.x2, r.y2 ));
          getimage  ( r.x1, r.y1, r.x2, r.y2, p );
          rahmen ( true, r.x1, r.y1, r.x2, r.y2 );
          bar ( r.x1, r.y1, r.x2, r.y2, 171 );
          for ( int x = 0; x < xnum; x++ )
             for ( int y = 0; y < ynum; y++ )
                 if ( y*xnum+x <= maxunitexperience ) {
                    void* q;
                    if ( vehicle->experience != y*xnum+x )
                       q = xlatpict ( xlatpictgraytable, icons.experience[y*xnum+x] );
                    else
                       q = icons.experience[y*xnum+x];

                    putimage( r.x1 +1 + (2 + xwidth)*x, r.y1 + 1 + (2 + ywidth)*y, q );
                }

          while ( mouseinrect ( &r ) && mouseparams.taste != 0 )
             releasetimeslice();
          putimage ( r.x1, r.y1, p );
          asc_free ( p );
       }
    }

    if ( mouseinrect ( agmp->resolutionx - ( 800 - 612), 213, agmp->resolutionx - ( 800 - 781), 305 ) && (mouseparams.taste == 2)) {
       CGameOptions::Instance()->smallmapactive = !CGameOptions::Instance()->smallmapactive;
       CGameOptions::Instance()->setChanged();

       if ( CGameOptions::Instance()->smallmapactive )
          dashboard.paintsmallmap( 1 );
       else
          dashboard.paintwind( 1 );

       while ( mouseparams.taste == 2 )
          releasetimeslice();
    }
    /*
    if ( mouseinrect ( agmp->resolutionx - ( 800 - 620),  90, agmp->resolutionx - ( 800 - 735), 196 ) && (mouseparams.taste == 2)) {
       npush ( activefontsettings );
       materialdisplayed = !materialdisplayed;
       setinvisiblemouserectanglestk ( agmp->resolutionx - ( 800 - 620),  90, agmp->resolutionx - ( 800 - 735), 196 );
       paintweapons();
       while ( mouseparams.taste == 2 );
       getinvisiblemouserectanglestk ();
       npop ( activefontsettings );
    }
    */

    if ( (func & 1) == 0 ) {
       if ( smallmap  &&  CGameOptions::Instance()->smallmapactive )
          smallmap->checkformouse();

       if ( !CGameOptions::Instance()->smallmapactive ) {
          if ( mouseparams.x >= agmp->resolutionx - ( 640 - 588 )   &&   mouseparams.x <= agmp->resolutionx - ( 640 - 610 )  &&   mouseparams.y >= 227   &&   mouseparams.y <= 290  && (mouseparams.taste & 1) ) {
             displaywindspeed();
             while ( mouseparams.x >= agmp->resolutionx - ( 640 - 588 )  &&   mouseparams.x <= agmp->resolutionx - ( 640 - 610 )  &&   mouseparams.y >= 227   &&   mouseparams.y <= 290  && (mouseparams.taste & 1) )
                releasetimeslice();
          }
       }

       if ( mouseparams.taste == 1 )
          if ( mouseinrect ( zoom.x1, zoom.y1, zoom.x2, zoom.y2 )) {
             int pos = mouseparams.x - zoom.x1;
             pos -= zoom.picwidth / 2;
             int w = zoom.x2 - zoom.x1 - zoom.picwidth;
             int perc = 1000 * pos / w;
             if ( perc < 0 )
                perc = 0;
             if ( perc > 1000 )
                perc = 1000;
             int newzoom = zoomlevel.getminzoom() + (zoomlevel.getmaxzoom() - zoomlevel.getminzoom()) * ( 1000 - perc ) / 1000;
             if ( newzoom != zoomlevel.getzoomlevel() ) {
                cursor.hide();
                zoomlevel.setzoomlevel( newzoom );
                paintzoom();
                cursor.show();
                displaymap();
                displaymessage2("new zoom level %d%%", newzoom );
                dashboard.x = 0xffff;
             }
          }
    }

    if ( mouseparams.x >= agmp->resolutionx - ( 640 - 578 )   &&   mouseparams.x <= agmp->resolutionx - ( 640 - 609 )  &&   mouseparams.y >=  59   &&   mouseparams.y <=  67  && (mouseparams.taste & 1) ) {
       dashboard.movedisp = !dashboard.movedisp;
       dashboard.x = 0xffff;
       while ( mouseparams.x >= agmp->resolutionx - ( 640 - 578 )   &&   mouseparams.x <= agmp->resolutionx - ( 640 - 609 )  &&   mouseparams.y >=  59   &&   mouseparams.y <=  67  && (mouseparams.taste & 1) )
          releasetimeslice();
    }

    for ( int i = 0; i < 8; i++ ) {
       if ( dashboard.weaps[i].displayed )
          if ( mouseinrect ( agmp->resolutionx - ( 640 - 502 ), 92 + i * 13, agmp->resolutionx - ( 640 - 572 ), 102 + i * 13 ) && (mouseparams.taste == 1)) {
             char tmp1[100];
             char tmp2[100];
             strcpy ( tmp1, strrrd8d ( dashboard.weaps[i].maxdist ));
             strcpy ( tmp2, strrrd8u ( dashboard.weaps[i].mindist ));
             displaymessage2 ( "min strength is %d at %s fields, max strength is %d at %s fields", dashboard.weaps[i].minstrength, tmp1, dashboard.weaps[i].maxstrength, tmp2 );

             while ( mouseinrect ( agmp->resolutionx - ( 640 - 502 ), 92 + i * 13, agmp->resolutionx - ( 640 - 572 ), 102 + i * 13 ) && (mouseparams.taste == 1))
                releasetimeslice();
          }
   }

   if ( (vehicle || vehicletype  ) && mouseinrect ( agmp->resolutionx - ( 640 - 461 ), 89, agmp->resolutionx - ( 640 - 577 ), 196 ) && (mouseparams.taste == 2))
      paintlargeweaponinfo();


   if ( mouseparams.x >= agmp->resolutionx - ( 640 - 501 )   &&   mouseparams.x <= agmp->resolutionx - ( 640 - 573 )   &&   mouseparams.y >= 71    &&   mouseparams.y <= 79   && (mouseparams.taste & 1) ) {
       pfield fld = getactfield();
       if ( fieldvisiblenow ( fld ) ) {
          if ( fld->vehicle )
             displaymessage2("damage is %d", fld->vehicle->damage );
          else
          if ( fld->building )
             displaymessage2("damage is %d", fld->building->damage );
          else
          if ( !fld->objects.empty() ) {
             ASCString temp = "damage is ";
             for ( tfield::ObjectContainer::reverse_iterator i = fld->objects.rbegin(); i != fld->objects.rend(); i++ )
                if ( i->typ->armor >= 0 ) {
                   temp += strrr ( i->damage );
                   temp += " ";
                }

             displaymessage2( temp.c_str() );
          }

       }
       while ( mouseparams.x >= agmp->resolutionx - ( 640 - 501 )   &&   mouseparams.x <= agmp->resolutionx - ( 640 - 573 )  &&   mouseparams.y >= 71    &&   mouseparams.y <= 79   && (mouseparams.taste & 1) )
          releasetimeslice();
   }

   if ( mouseparams.x >= agmp->resolutionx - ( 640 - 501 )   &&   mouseparams.x <= agmp->resolutionx - ( 640 - 573 )   &&   mouseparams.y >= 59    &&   mouseparams.y <= 67   && (mouseparams.taste & 1) ) {
      /*
       pfield fld = getactfield();
       if ( fieldvisiblenow ( fld ) ) {
          if ( fld->vehicle )
             displaymessage2("unit has %d fuel", fld->vehicle->tank.fuel );
       }
       */
       if ( vehicle )
          displaymessage2("unit has %d fuel", vehicle->getTank().fuel );
       while ( mouseparams.x >= agmp->resolutionx - ( 640 - 501 )   &&   mouseparams.x <= agmp->resolutionx - ( 640 - 573 )   &&   mouseparams.y >= 59    &&   mouseparams.y <= 67   && (mouseparams.taste & 1) )
          releasetimeslice();
   }
}



void   tdashboard :: paintvehicleinfo(  Vehicle*     vehicle,
                                       const Building*    building,
                                       const pfield       _objfield,
                                        pvehicletype vt )
{
   {
      collategraphicoperations cgo ( agmp->resolutionx - 800 + 610, 15, agmp->resolutionx - 800 + 783, 307 );

      int         ms;

      npush( activefontsettings );
      ms = getmousestatus();
      if (ms == 2) mousevisible(false);
      dashboard.backgrndcol    = 24;
      dashboard.vgcol          = green;    /* 26 / 76  */
      dashboard.ymx            = 471;    /*  469 / 471  */
      dashboard.ymn            = 380;
      dashboard.ydl            = dashboard.ymx - dashboard.ymn;
      dashboard.munitnumberx   = 545;
      dashboard.vehicle        = vehicle;
      dashboard.building       = building;
      dashboard.objfield       = _objfield;
      dashboard.vehicletype    = vt;

      dashboard.paintheight();
      dashboard.paintweapons();
      dashboard.paintdamage();
      dashboard.painttank();
      dashboard.paintexperience();
      dashboard.paintmovement();
      dashboard.paintarmor();

      if ( CGameOptions::Instance()->smallmapactive )
         dashboard.paintsmallmap( dashboard.repainthard );
      else
         dashboard.paintwind( dashboard.repainthard );

      dashboard.paintname();
      dashboard.paintclasses ();
      dashboard.paintimage();
      dashboard.paintplayer();
      dashboard.x = getxpos();
      dashboard.y = getypos();
      if (ms == 2) mousevisible(true);
      npop( activefontsettings );

      dashboard.repainthard = 0;

      if ( actmap && actmap->ellipse )
         actmap->ellipse->paint();

   }
   dashboard.paintzoom();
}   /*  paintvehicleinfo  */


#endif


