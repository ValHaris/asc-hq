
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <pgimage.h>
#include "paradialog.h"
#include "typen.h"
#include "vehicletype.h"
#include "vehicle.h"
#include "iconrepository.h"
#include "spfst.h"
#include "dialog.h"

#include "textfiletags.h"

void assignWeaponInfo ( Panel* panel, PG_Widget* widget, const SingleWeapon& weapon )
{
   panel->setImage( "weapon_symbol1", IconRepository::getIcon(SingleWeapon::getIconFileName( weapon.getScalarWeaponType()) + "-small.png"), widget );

   panel->setLabelText( "weapon_text1", weapon.getName(), widget );
   panel->setLabelText( "weapon_reactionfire", weapon.reactionFireShots, widget );
   panel->setLabelText( "weapon_maxammo", weapon.count, widget );
   panel->setLabelText( "weapon_canshoot", weapon.offensive()? "yes" : "no", widget );
   panel->setLabelText( "weapon_canrefuel", weapon.canRefuel()? "yes" : "no", widget );
   panel->setLabelText( "weapon_strenghtmax", weapon.maxstrength, widget );
   panel->setLabelText( "weapon_strenghtmin", weapon.minstrength, widget );
   panel->setLabelText( "weapon_distancemin", weapon.mindistance, widget );
   panel->setLabelText( "weapon_distancemax", weapon.maxdistance, widget );

   for ( int i = 0; i < 8; ++i ) {
      if ( weapon.targ & (1<< i)) {
         panel->setWidgetTransparency( ASCString("weapon_notarget_") + heightTags[i] , 255 );
         panel->setWidgetTransparency( ASCString("weapon_target_") + heightTags[i] , 0 );
      } else {
         panel->setWidgetTransparency( ASCString("weapon_notarget_") + heightTags[i] , 0 );
         panel->setWidgetTransparency( ASCString("weapon_target_") + heightTags[i] , 255 );
      }

      if ( weapon.sourceheight & (1<< i)) {
         panel->setWidgetTransparency( ASCString("weapon_nosource_") + heightTags[i] , 255 );
         panel->setWidgetTransparency( ASCString("weapon_source_") + heightTags[i] , 0 );
      } else {
         panel->setWidgetTransparency( ASCString("weapon_nosource_") + heightTags[i] , 0 );
         panel->setWidgetTransparency( ASCString("weapon_source_") + heightTags[i] , 255 );
      }
   }
}


static const char* paneName[5]  = { "information", "movement", "weapons", "transport", "description" };

class UnitInfoDialog : public Panel {
        const Vehicle* veh;
        const Vehicletype* vt;
        PG_Widget* weaponGraph;
        int currentWeapon;
        typedef vector< pair<int,int> > EntranceHeights;
        EntranceHeights entranceHeights;
        Surface infoImage;

  	     bool eventKeyDown(const SDL_KeyboardEvent* key)
        {
           if ( key->keysym.sym == SDLK_ESCAPE ) {
              QuitModal();
              return true;
           }
           return false;
        };



         void registerSpecialDisplay( const ASCString& name )
         {
            SpecialDisplayWidget* sdw = dynamic_cast<SpecialDisplayWidget*>( FindChild( name, true ) );
            if ( sdw )
               sdw->display.connect( SigC::slot( *this, &UnitInfoDialog::painter ));
         };

         void registerSpecialInput( const ASCString& name )
         {
            SpecialInputWidget* siw = dynamic_cast<SpecialInputWidget*>( FindChild( name, true ) );
            if ( siw )
               siw->sigMouseButtonDown.connect( SigC::slot( *this, &UnitInfoDialog::onClick ));
         };

         bool onClick ( PG_MessageObject* obj, const SDL_MouseButtonEvent* event ) {
            PG_Widget* w = dynamic_cast<PG_Widget*>(obj);
            if ( w ) {
               click( w->GetName() );
               return true;
            } 
            return false;
         };

         bool onEntranceClick ( PG_MessageObject* obj, const SDL_MouseButtonEvent* event, int entranceNum ) {
            if ( vt ) {
               PG_Widget* swi = FindChild( "single_weapon_info", true );
               if ( swi ) {
                  ContainerBaseType::TransportationIO tio = vt->entranceSystems[entranceNum];
                  setWidgetTransparency( "pad_transport_in",      tio.mode & ContainerBaseType::TransportationIO::In      ? 0 : 255 );
                  setWidgetTransparency( "pad_transport_out",     tio.mode & ContainerBaseType::TransportationIO::Out     ? 0 : 255 );
                  setWidgetTransparency( "pad_transport_docking", tio.mode & ContainerBaseType::TransportationIO::Docking ? 0 : 255 );

                  for ( int i = 0; i < cmovemalitypenum; ++i )
                     if ( (vt->vehicleCategoriesStorable & (1<<i)) && (tio.vehicleCategoriesLoadable & (1<<i)))
                        setImage( ASCString("unitpad_transport_") + unitCategoryTags[i], IconRepository::getIcon("pad_symbol_ok.png") );
                     else
                        setImage( ASCString("unitpad_transport_") + unitCategoryTags[i], IconRepository::getIcon("pad_symbol_no.png") );

                  ASCString s;
                  for ( int i = 0; i < cvehiclefunctionsnum; ++i )
                     if ( tio.requireUnitFunction & (1 << i)) {
                        if ( s.length() )
                           s += ", ";
                        s += cvehiclefunctions[i];
                     }
                  setLabelText( "unitpad_transport_specialfunctions", s );

                  setLabelText( "unitpad_transport_attackafterwards", tio.disableAttack ? "no" : "yes" );

                  entranceHeights.clear();
                  for ( int i = 0; i < 8; ++i )
                     if ( vt->height & (1 << i) & tio.container_height )
                        for ( int j = 0; j < 8; ++j )
                           if ( !tio.height_abs || tio.height_abs & (1 <<j ))
                              if ( tio.height_rel == -100 || getheightdelta( i, j ) == tio.height_rel  )
                                 entranceHeights.push_back( make_pair(i,j)); 

                  PG_Widget* w = FindChild( "unitpad_transport_leveldisplay", true );
                  if ( w )
                     w->Update();
               }

               for ( int i = 0; i < vt->entranceSystems.size(); ++i ) {
                  ASCString n = "pad_transport_square" + ASCString::toString(i);
                  PG_Widget* w = FindChild( n, true );
                  if ( w ) {
                     if ( i == entranceNum ) 
                        w->SetTransparency( 0 );
                        // w->SetVisible(true );
                     else
                        w->SetTransparency( 255 );
                        // w->SetVisible(false  );
                     // w->Update();
                  }
               }
               Update();
            }
            return true;
         };

         bool onWeaponClick ( PG_MessageObject* obj, const SDL_MouseButtonEvent* event, int weaponNum ) {
            if ( vt ) {
               PG_Widget* swi = FindChild( "single_weapon_info", true );
               if ( swi )
                  assignWeaponInfo( this, swi, vt->weapons.weapon[weaponNum] );

               for ( int i = 0; i < vt->weapons.count; ++i ) {
                  ASCString n = "pad_weaponbar" + ASCString::toString(i);
                  PG_Widget* w = FindChild( n, true );
                  if ( w ) {
                     if ( i == weaponNum )
                        w->SetTransparency( 0 );
                        // w->SetVisible(true );
                     else
                        // w->SetVisible(false );
                        w->SetTransparency( 255 );
                     // w->Update();
                  }
               }
               Update();
               currentWeapon = weaponNum;
               if ( weaponGraph )
                  weaponGraph->Update();
            }
            return true;
         };

         SPoint getWeaponGraphCoords( int maxdist, int maxstrength, int dist, int strength )
         {
            const int border  = 0;
            SPoint p;
            if( maxdist < 2 )
               maxdist = 2;

            p.x = (weaponGraph->Width() - 2 * border ) * dist / maxdist + border;
            p.y = weaponGraph->Height() - border - (weaponGraph->Height() - 2 * border ) * strength / maxstrength ;
            p.x += weaponGraph->my_xpos;
            p.y += weaponGraph->my_ypos;
            return p;
         }

         void painter ( const PG_Rect &src, const ASCString& name, const PG_Rect &dst)
         {
            Surface screen = Surface::Wrap( PG_Application::GetScreen() );

            if ( name == "unitpad_unitsymbol" ) 
               if ( vt )
                  vt->paint( screen, SPoint( dst.x, dst.y ), veh ? veh->getOwner() : 0 );

            if ( name == "unitpad_weapon_diagram" ) {
               if ( vt && weaponGraph ) {
                  int maxdist  = 0;
                  int maxstrength = 0;
                  for ( int i = 0; i < vt->weapons.count; ++i )
                     if ( vt->weapons.weapon[i].shootable() ) {
                        maxdist = max ( maxdist, vt->weapons.weapon[i].maxdistance );
                        maxstrength = max ( maxstrength, vt->weapons.weapon[i].maxstrength );
                     }

                  setLabelText( "unitpad_weapon_diagram_maxdist", maxdist / maxmalq );
                  setLabelText( "unitpad_weapon_diagram_maxstrength", maxstrength );

                  if( maxdist > 0 && maxstrength > 0 )
                     for ( int i = 0; i < vt->weapons.count; ++i ) {
                        int mind = (vt->weapons.weapon[i].mindistance+maxmalq-1) / maxmalq;
                        int maxd = vt->weapons.weapon[i].maxdistance / maxmalq;
                        int linewidth = currentWeapon == i ? 2 : 1 ;
                        if ( mind == maxd ) {
                           SPoint p = getWeaponGraphCoords( maxdist/maxmalq, maxstrength, mind, vt->weapons.weapon[i].maxstrength );
                           PG_Draw::DrawLine( PG_Application::GetScreen(), p.x , p.y - 2, p.x , p.y + 2, 0xffffff, linewidth );
                           PG_Draw::DrawLine( PG_Application::GetScreen(), p.x - 2, p.y , p.x + 2, p.y , 0xffffff, linewidth );
                        } else {
                           SPoint p = getWeaponGraphCoords( maxdist/maxmalq, maxstrength, mind, vt->weapons.weapon[i].maxstrength );
                           SPoint p2 = getWeaponGraphCoords( maxdist/maxmalq, maxstrength, maxd, vt->weapons.weapon[i].minstrength );
                           PG_Draw::DrawLine( PG_Application::GetScreen(), p.x , p.y , p2.x, p2.y , 0xffffff, linewidth );
                        }
                     }
               }
            }
            if ( name == "unitpad_transport_transporterlevel" || name == "unitpad_transport_unitlevel" ) {
               int xoffs = 0;
               for ( EntranceHeights::iterator i = entranceHeights.begin(); i != entranceHeights.end(); ++i ) {
                  Surface& icon = IconRepository::getIcon( ASCString("height-a") + ASCString::toString( name == "unitpad_transport_unitlevel" ? i->second : i->first) + ".png");
                  screen.Blit( icon, SPoint( dst.x + xoffs, dst.y ));
                  xoffs += icon.w() + 5;
               }
            }
         };

         void activate( const ASCString& pane ) {
            PG_Application::SetBulkMode();
            for ( int i = 0; i < 5; ++i )
                if ( ASCString( paneName[i]) != pane )
                   hide( paneName[i] );
                   
            for ( int i = 0; i < 5; ++i )
                if ( ASCString( paneName[i]) == pane )
                   show( paneName[i] );
            PG_Application::SetBulkMode(false);
            Update();
         };

         void click( const ASCString& name ) {
            for ( int i = 0; i < 5; ++i)
               if ( name == ASCString("padclick_") + paneName[i] ) 
                  activate(paneName[i]);
            if ( name == "padclick_exit" )
               QuitModal();
         };

     public:
        
        
        UnitInfoDialog (PG_Widget *parent, const Vehicle* vehicle, const Vehicletype* vehicleType ) 
           : Panel( parent, PG_Rect::null, "UnitInfoDialog", false ), veh(vehicle), vt( vehicleType ), weaponGraph(NULL), currentWeapon(-1) {
               sigClose.connect( SigC::slot( *this, &UnitInfoDialog::QuitModal ));

               if( veh )
                  vt = veh->typ;

               try {
                  setup();
               }
               catch ( ParsingError err ) {
                  displaymessage( err.getMessage(), 1 );
                  return;
               }
               catch ( ... ) {
                  displaymessage( "unknown exception", 1 );
                  return;
               }

               if ( !vt->infoImageFilename.empty() && exist( vt->infoImageFilename )) {
                  PG_Image* img = dynamic_cast<PG_Image*>(FindChild( "unitpad_3dpic", true ));
                  if ( img ) {
                     tnfilestream stream ( vt->infoImageFilename, tnstream::reading );
                     infoImage.readImageFile( stream );
                     img->SetDrawMode( PG_Draw::STRETCH );
                     img->SetImage( infoImage.getBaseSurface(), false );
                     img->SizeWidget( img->GetParent()->w, img->GetParent()->h );
                  }
               }

               if ( veh )
                  setLabelText( "unitpad_unitname", veh->getName() );
               else
                  if ( vt )
                     setLabelText( "unitpad_unitname", vt->getName() );

               setLabelText( "unitpad_unitcategory", cmovemalitypes[ vt->movemalustyp ] );
               registerSpecialDisplay( "unitpad_unitsymbol");
               registerSpecialDisplay( "unitpad_weapon_diagram");
               registerSpecialDisplay( "unitpad_transport_transporterlevel");
               registerSpecialDisplay( "unitpad_transport_unitlevel");
               weaponGraph = FindChild( "unitpad_weapon_diagram", true );

               if ( vt ) {
                  setLabelText( "unitpad_unitarmor", vt->armor );
                  setLabelText( "unitpad_unitweight", vt->weight );
                  setLabelText( "unitpad_unitview", vt->view );
                  setLabelText( "unitpad_unitjamming", vt->jamming );
                  setLabelText( "unitpad_unitcostenergy", vt->productionCost.energy );
                  setLabelText( "unitpad_unitcostmaterial", vt->productionCost.material );
                  setLabelText( "unitpad_unitcostfuel", vt->productionCost.fuel );
                  setLabelText( "unitpad_unittankfuel", vt->tank.fuel );
                  setLabelText( "unitpad_unittankenergy", vt->tank.energy );
                  setLabelText( "unitpad_unittankmaterial", vt->tank.material );

                  ASCString abilities;
                  for ( int i = 0; i < cvehiclefunctionsnum; ++i )
                     if ( vt->functions & (1 << i)) 
                        abilities += cvehiclefunctions[i] + ASCString("\n");
                  setLabelText( "unitpad_unitabilities", abilities );


                  setLabelText( "unitpad_unitmove_unitfuelconsumption", vt->fuelConsumption );
                  for ( int i = 0; i< 8; ++i )
                     setLabelText( ASCString("unitpad_unitmove_") + heightTags[i], vt->movement[i] );

                  if ( vt->maxwindspeedonwater < 255 && vt->maxwindspeedonwater > 0 )
                     setLabelText( "unitpad_unitmove_windresistance", vt->maxwindspeedonwater );
                  else
                     setLabelText( "unitpad_unitmove_windresistance", "-" );

                  setLabelText( "unitpad_transport_maxtotalweight", vt->maxLoadableWeight );
                  setLabelText( "unitpad_transport_maxsingleweight", vt->maxLoadableUnitSize );
                  setLabelText( "unitpad_transport_loadableunits", vt->maxLoadableUnits );


               }




               for ( int i = 0; i < 5; ++i )
                  registerSpecialInput( ASCString("padclick_") + paneName[i] );
               registerSpecialInput( "padclick_exit" );

               setLabelText( "unitpad_description_text", vt->infotext );
               
              activate(paneName[0]);
              Show();
               
           };

      void userHandler( const ASCString& label, PropertyReadingContainer& pc, PG_Widget* parent, WidgetParameters widgetParams ) 
      {
         if ( label == "unitpad_heightchange" && vt ) {
            int yoffset = 0;
            for ( int i = 0; i < vt->heightChangeMethodNum; ++i ) {
               int srcLevelCount = 0;
               for ( int j = 0; j < 8; ++j )
                  if ( vt->height & vt->heightChangeMethod[i].startHeight & (1 << j)) 
                     ++srcLevelCount;

               pc.openBracket( "LineWidget" );
               PG_Rect r = parseRect( pc, parent);
               r.y += yoffset;
               r.my_height *= (srcLevelCount-1) / 3 + 1;

               SpecialInputWidget* sw = new SpecialInputWidget ( parent, r );
               parsePanelASCTXT( pc, sw, widgetParams );
               pc.closeBracket();
               yoffset += sw->Height();



               int xoffs = 0;
               int yoffs = 0;
               int counter = 0;
               for ( int j = 0; j < 8; ++j )
                  if ( vt->height & vt->heightChangeMethod[i].startHeight & (1 << j))  {
                     ASCString filename = "height-a" + ASCString::toString(j) + ".png";
                     xoffs = IconRepository::getIcon(filename).w() * (counter % 3 );
                     yoffs = IconRepository::getIcon(filename).h() * (counter / 3 );
                     new PG_Image( sw, PG_Point( xoffs, yoffs ), IconRepository::getIcon(filename).getBaseSurface(), false );
                     ++counter;
                  }

               ASCString delta = ASCString::toString( vt->heightChangeMethod[i].heightDelta );
               if ( vt->heightChangeMethod[i].heightDelta > 0 )
                  delta = "+" + delta;
               setLabelText( "unitpad_move_changeheight_change", delta, sw );
               
               setLabelText( "unitpad_move_changeheight_movepoints", vt->heightChangeMethod[i].moveCost, sw );
               setLabelText( "unitpad_move_changeheight_distance", vt->heightChangeMethod[i].dist, sw );
            }
         }
         if ( label == "unitpad_terrainaccess" && vt ) {
            int yoffset = 0;
            for ( int i = 0; i < cbodenartennum ; ++i ) {
               if ( vt->terrainaccess.terrain.test(i) || vt->terrainaccess.terrainkill.test(i) || vt->terrainaccess.terrainnot.test(i) || vt->terrainaccess.terrainreq.test(i) ) {
                  pc.openBracket( "LineWidget" );
                  PG_Rect r = parseRect( pc, parent);
                  r.y += yoffset;

                  
                  SpecialInputWidget* sw = new SpecialInputWidget ( parent, r );
                  parsePanelASCTXT( pc, sw, widgetParams );
                  pc.closeBracket();
                  yoffset += sw->Height();

                  setLabelText( "unitpad_unitmove_terraintype", cbodenarten[i], sw);

                  PG_Widget* w = sw->FindChild("unitpad_unitmove_terrainaccess", true);
                  if ( w ) {
                     int xoffs = 0;
                     static const char* iconName[] = {"pad_symbol_ok.png", "pad_symbol_warn.png", "pad_symbol_no.png", "pad_symbol_kill.png" };  
                     const TerrainBits* bits[] = { &vt->terrainaccess.terrain, &vt->terrainaccess.terrainreq, &vt->terrainaccess.terrainnot, &vt->terrainaccess.terrainkill };
                     for ( int icon = 0; icon < 4; ++icon ) {
                        bool set = bits[icon]->test(i);

                        if ( set ) {
                           PG_Image* img = new PG_Image( w, PG_Point( xoffs, 0 ), IconRepository::getIcon( iconName[icon] ).getBaseSurface(), false );
                           xoffs += img->Width();
                        }
                     }
                  }
               }
            }
         }
         if ( label == "unitpad_weaponlist" && vt ) {
            int yoffset = 0;
            for ( int i = 0; i < vt->weapons.count ; ++i ) {
               pc.openBracket( "LineWidget" );
               PG_Rect r = parseRect( pc, parent);
               r.y += yoffset;

               
               SpecialInputWidget* sw = new SpecialInputWidget ( parent, r );
               parsePanelASCTXT( pc, sw, widgetParams );
               rename( "pad_weaponbar", "pad_weaponbar" + ASCString::toString( i ));
               pc.closeBracket();
               yoffset += sw->Height();

               assignWeaponInfo( this, sw, vt->weapons.weapon[i] );
               sw->sigMouseButtonDown.connect( SigC::bind( SigC::slot( *this, &UnitInfoDialog::onWeaponClick ), i));
            }
         }
         if ( label == "entrancesystems" && vt ) {
            int xoffset = 0;
            for ( int i = 0; i < vt->entranceSystems.size() ; ++i ) {
               pc.openBracket( "LineWidget" );
               PG_Rect r = parseRect( pc, parent);
               r.x += xoffset;

               SpecialInputWidget* sw = new SpecialInputWidget ( parent, r );
               parsePanelASCTXT( pc, sw, widgetParams );
               rename( "pad_transport_square", "pad_transport_square" + ASCString::toString( i ));
               pc.closeBracket();

               setLabelText( "unitpad_transport_entrancenumber", i+1, sw );

               xoffset += sw->Width();

               // assignWeaponInfo( this, sw, vt->weapons.weapon[i] );
               sw->sigMouseButtonDown.connect( SigC::bind( SigC::slot( *this, &UnitInfoDialog::onEntranceClick ), i));
            }
         }
      }; 
        
};

void unitInfoDialog()
{

   pfield fld = actmap->getField( actmap->getCursor() );
   if ( fld && fld->vehicle ) {
      UnitInfoDialog* uid = new UnitInfoDialog( NULL, fld->vehicle, NULL );
      uid->Show();
      uid->RunModal();
      delete uid;
   } else
      displaymessage2("please select a unit" );
}

