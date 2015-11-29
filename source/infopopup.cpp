//
// C++ Implementation: infopopup
//
// Description: 
//
//
// Author: Kevin Hirschmann <hirsch@dhcppc0>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <iostream>
#include "infopopup.h"
#include "sg.h"

InfoPopUp::InfoPopUp(PG_Widget* parent): PG_PopupMenu(parent, 20, 20 ) {
  addMenuItem("Unit Type", 1, slot(showUnitInfo));
  addMenuItem("Unit Weight", 2, slot(showUnitWeight));
  addMenuItem("Weapon Range", 3, slot(showWeaponRange));
  addMenuItem("Movement Range", 4, slot(showMovementRange));
  addMenuItem("", 5,MenuItem::MIF_SEPARATOR );
  addMenuItem("Terrain", 6, slot(showTerrainInfo));
  //ut = new PG_PopupMenu::MenuItem(this, "Unit Type",  1, MenuItem::MIF_NONE);
  //ut->sigSelectMenuItem.connect(sigc::mem_fun(*this, &InfoPopUp::showUnitInfo));
  //actmap->getField(mc)->vehicle;
  Hide();
}


InfoPopUp::~InfoPopUp()
{

}

bool showUnitInfo(PG_PopupMenu::MenuItem* item, PG_Pointer clientdata){
  
  execUserAction_ev(ua_vehicleinfo);
return true;
}

bool showUnitWeight(PG_PopupMenu::MenuItem* item, PG_Pointer clientdata){
  
  execUserAction_ev(ua_unitweightinfo);
return true;
}

bool showWeaponRange(PG_PopupMenu::MenuItem* item, PG_Pointer clientdata){
   execUserAction_ev(ua_viewunitweaponrange);
return true;
}
bool showMovementRange(PG_PopupMenu::MenuItem* item, PG_Pointer clientdata){
   execUserAction_ev(ua_viewunitmovementrange);
return true;

}

bool showTerrainInfo(PG_PopupMenu::MenuItem* item, PG_Pointer clientdata){
   execUserAction_ev(ua_viewterraininfo);
return true;
}


