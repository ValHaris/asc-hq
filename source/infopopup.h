//
// C++ Interface: infopopup
//
// Description: 
//
//
// Author: Kevin Hirschmann <hirsch@dhcppc0>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef INFOPOPUP_H
#define INFOPOPUP_H

#include "pgpopupmenu.h"
/**
@author Kevin Hirschmann
*/
class InfoPopUp: public PG_PopupMenu{
public:
    InfoPopUp(PG_Widget* parent);

    ~InfoPopUp();

    private:
    PG_PopupMenu::MenuItem* ut;
    
    
};
extern bool showUnitInfo(PG_PopupMenu::MenuItem* item, PG_Pointer clientdata);
extern bool showUnitWeight(PG_PopupMenu::MenuItem* item, PG_Pointer clientdata);
extern bool showWeaponRange(PG_PopupMenu::MenuItem* item, PG_Pointer clientdata);
extern bool showMovementRange(PG_PopupMenu::MenuItem* item, PG_Pointer clientdata);
extern bool showTerrainInfo(PG_PopupMenu::MenuItem* item, PG_Pointer clientdata);
#endif

