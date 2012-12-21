/*! \file guifunctions.cpp
    \brief All the buttons of the user interface with which the unit actions are controlled.
*/

/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2010  Martin Bickel  and  Marc Schellenberger

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

#include <stdio.h>
#include <cstring>
#include <stdlib.h>

#include "guifunctions.h"
#include "guifunctions-interface.h"
#include "controls.h"
#include "dialog.h"
#include "gameoptions.h"
#include "iconrepository.h"
#include "attack.h"
#include "graphics/blitter.h"
#include "viewcalculation.h"
#include "spfst.h"
#include "dialogs/cargodialog.h"
#include "dialogs/ammotransferdialog.h"
#include "mapdisplay.h"
#include "sg.h"
#include "loaders.h"
#include "itemrepository.h"
#include "turncontrol.h"
#include "spfst-legacy.h"
#include "dialogs/buildingtypeselector.h"
#include "dialogs/internalAmmoTransferDialog.h"
#include "dialogs/vehicleproductionselection.h"
#include "actions/jumpdrivecommand.h"
#include "actions/destructunitcommand.h"
#include "actions/attackcommand.h"
#include "actions/moveunitcommand.h"
#include "actions/putobjectcommand.h"
#include "actions/putminecommand.h"
#include "actions/constructunitcommand.h"
#include "actions/servicecommand.h"
#include "actions/reactionfireswitchcommand.h"
#include "actions/repairunitcommand.h"
#include "actions/constructbuildingcommand.h"
#include "actions/destructbuildingcommand.h"
#include "actions/powergenerationswitchcommand.h"
#include "actions/internalammotransfercommand.h"

bool commandPending()
{
    return NewGuiHost::pendingCommand;
}

namespace GuiFunctions
{



class AttackGui : public GuiIconHandler, public GuiFunction, public SigC::Object
{
    pair<const AttackWeap*, int> getEntry( const MapCoordinate& pos, int num );

    void mapDeleted( GameMap& map ) {
        if ( NewGuiHost::getIconHandler() == this )
            NewGuiHost::popIconHandler();
    }

protected:
    bool available( const MapCoordinate& pos, ContainerBase* subject, int num );
    void execute( const MapCoordinate& pos, ContainerBase* subject, int num );
    Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num );
    ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num );
    bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num );

public:
    AttackGui() {
        GameMap::sigMapDeletion.connect( SigC::slot( *this, &AttackGui::mapDeleted ));
    };
    void setupWeapons( AttackCommand* va ) {
        delete NewGuiHost::pendingCommand;
        NewGuiHost::pendingCommand = va;
    };
    void eval( const MapCoordinate& mc, ContainerBase* subject );

};

bool AttackGui :: checkForKey( const SDL_KeyboardEvent* key, int modifier, int num )
{
    if ( key->keysym.sym == SDLK_ESCAPE || key->keysym.unicode == 'c' ) {
        // execute( actmap->getCursor(), actmap->getField( actmap->getCursor())->getContainer() , -1 );
        return true;
    }
    return false;
}


pair<const AttackWeap*, int> AttackGui::getEntry( const MapCoordinate& pos, int num )
{
    int counter = 0;

    AttackCommand* attackEngine = dynamic_cast<AttackCommand*>(NewGuiHost::pendingCommand );
    if ( attackEngine ) {
        const AttackCommand::FieldList* afl = NULL;
        for ( int i = 0; i < 3; ++i ) {
            switch ( i ) {
            case 0:
                afl = &attackEngine->getAttackableUnits();
                break;
            case 1:
                afl = &attackEngine->getAttackableBuildings();
                break;
            case 2:
                afl = &attackEngine->getAttackableObjects();
                break;
            }

            AttackCommand::FieldList::const_iterator it = afl->find( pos );
            if ( it != afl->end() ) {
                const AttackWeap* aw = &(it->second);
                for ( int a = 0; a < aw->count; ++a ) {
                    if ( counter == num )
                        return make_pair( aw, a );

                    ++counter;
                }
            }
        }
    }
    return make_pair( (AttackWeap*)(NULL), 0 );
}


bool AttackGui::available( const MapCoordinate& pos, ContainerBase* subject, int num )
{
    if ( num == -1 )
        return true;

    if ( getEntry(pos,num).first )
        return true;
    else
        return false;
}

void AttackGui::execute( const MapCoordinate& pos, ContainerBase* subject, int num )
{
    if ( num != -1 ) {

        pair<const AttackWeap*, int> p = getEntry(pos,num);
        if ( p.first ) {
            AttackCommand* attack = dynamic_cast<AttackCommand*>(NewGuiHost::pendingCommand);
            if ( attack ) {
                attack->setTarget ( pos, p.first->num[p.second] );
                ActionResult res = attack->execute( createContext( actmap ));
                if ( !res.successful()  ) {
                    dispmessage2 ( res );
                    delete NewGuiHost::pendingCommand;
                }
            }

        }
    }
    NewGuiHost::pendingCommand = NULL;

    actmap->cleartemps();
    NewGuiHost::popIconHandler();
    repaintMap();
    updateFieldInfo();
}

Surface& AttackGui::getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
{
    if ( num == -1 )
        return IconRepository::getIcon("cancel.png");

    pair<const AttackWeap*, int> p = getEntry(pos,num);
    switch ( p.first->typ[p.second] ) {
    case cwcruisemissileb:
        return IconRepository::getIcon("weap-cruisemissile.png");
    case cwbombb:
        return IconRepository::getIcon("weap-bomb.png");
    case cwlargemissileb:
        return IconRepository::getIcon("weap-bigmissile.png");
    case cwsmallmissileb:
        return IconRepository::getIcon("weap-smallmissile.png");
    case cwtorpedob:
        return IconRepository::getIcon("weap-torpedo.png");
    case cwmachinegunb:
        return IconRepository::getIcon("weap-machinegun.png");
    case cwcannonb:
        return IconRepository::getIcon("weap-cannon.png");
    default:
        return IconRepository::getIcon("weap-laser.png");
    };
}

ASCString AttackGui::getName( const MapCoordinate& pos, ContainerBase* subject, int num )
{

    AttackCommand* attackEngine = dynamic_cast<AttackCommand*>(NewGuiHost::pendingCommand);

    if ( num == -1 || attackEngine == NULL )
        return "cancel";

    Vehicle* attacker = attackEngine->getAttacker();

    tfight* battle = NULL;

    pair<const AttackWeap*, int> p = getEntry(pos,num);


    if ( p.first->target == AttackWeap::vehicle )
        battle = new tunitattacksunit ( attacker, actmap->getField(pos)->vehicle, true, p.first->num[p.second] );
    else if ( p.first->target == AttackWeap::building )
        battle = new tunitattacksbuilding ( attacker, pos.x, pos.y, p.first->num[p.second] );
    else if ( p.first->target == AttackWeap::object )
        battle = new tunitattacksobject ( attacker, pos.x, pos.y, p.first->num[p.second] );


    int dd = battle->dv.damage;
    int ad = battle->av.damage;
    battle->calc ( );

    ASCString result;
    result.format( "%s; eff strength: %d; damage inflicted to enemy: %d, making a total of ~%d~; own damage will be +%d = %d", cwaffentypen[getFirstBit(p.first->typ[p.second])], battle->av.strength, battle->dv.damage-dd, battle->dv.damage, battle->av.damage-ad, battle->av.damage );
    return result;
}


void AttackGui::eval( const MapCoordinate& mc, ContainerBase* subject )
{

    int num = 0;
    while ( getEntry( mc, num).first ) {
        GuiButton* b = host->getButton(num);
        b->registerFunc( this, mc, subject, num );
        b->Show();
        ++num;
    }

    GuiButton* b = host->getButton(num);
    b->registerFunc( this, mc, subject, -1 );
    b->Show();
    ++num;

    host->disableButtons(num);
}



AttackGui attackGui;



bool Cancel::available( const MapCoordinate& pos, ContainerBase* subject, int num )
{
    return commandPending();
};

void Cancel::execute( const MapCoordinate& pos, ContainerBase* subject, int num )
{
    if ( NewGuiHost::pendingCommand ) {
        delete NewGuiHost::pendingCommand;
        NewGuiHost::pendingCommand = NULL;
    }

    actmap->cleartemps(7);
    updateFieldInfo();
    displaymap();
};

bool Cancel::checkForKey( const SDL_KeyboardEvent* key, int modifier, int num )
{
    return ( key->keysym.sym == SDLK_ESCAPE || key->keysym.unicode == 'c' );
};


Surface& Cancel::getImage( const MapCoordinate& po, ContainerBase* subject, int nums )
{
    return IconRepository::getIcon("cancel.png");
};

ASCString Cancel::getName( const MapCoordinate& pos, ContainerBase* subject, int num )
{
    return "~c~ancel";
};



bool Movement::checkForKey( const SDL_KeyboardEvent* key, int modifier, int num )
{
    return ( key->keysym.sym == SDLK_SPACE );
};

Surface& Movement::getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
{
    return IconRepository::getIcon("movement.png");
};

ASCString Movement::getName( const MapCoordinate& pos, ContainerBase* subject, int num )
{
    return "move unit (~space~)";
};


bool MovementBase::available( const MapCoordinate& pos, ContainerBase* subject, int num )
{
    if ( !commandPending() ) {
        Vehicle* eht = actmap->getField(pos)->vehicle;
        if ( eht )
            if ( eht->getOwner() == actmap->actplayer)
                return MoveUnitCommand::avail ( eht );
    } else {
        if ( !NewGuiHost::pendingCommand )
            return false;

        MoveUnitCommand* moveCommand = dynamic_cast<MoveUnitCommand*>(NewGuiHost::pendingCommand);
        if ( moveCommand && moveCommand->getVerticalDirection() == getVerticalDirection() ) {
            const AStar3D::Path& path = moveCommand->getPath();
            if ( path.size() ==  0 )
                return moveCommand->isFieldReachable(pos, true );
            else
                return path.rbegin()->x == pos.x && path.rbegin()->y == pos.y;
        }
    }
    return false;
}

void Movement::parametrizePathFinder( AStar3D& pathFinder )
{
}

bool Movement::available( const MapCoordinate& pos, ContainerBase* subject, int num )
{
    if ( !commandPending() ) {
        return MovementBase::available( pos, subject, num );
    } else {
        MoveUnitCommand* moveCommand = dynamic_cast<MoveUnitCommand*>(NewGuiHost::pendingCommand);
        if ( moveCommand && moveCommand->getVerticalDirection() == getVerticalDirection() ) {
            bool avail = MovementBase::available( pos, subject, num );
            if ( avail )
                return true;
            else {
                return moveCommand->longDistAvailable( pos );
            }
        }
    }
    return false;
}




void MovementBase::execute( const MapCoordinate& pos, ContainerBase* subject, int num )
{
    if ( !commandPending() ) {
        // This situation should have already been covered by MovementBase::available(), but there were crash reports with vehicle being NULL
        if ( !actmap->getField(pos)->vehicle )
           return;

       
        MoveUnitCommand* move = new MoveUnitCommand( actmap->getField(pos)->vehicle );

        move->setVerticalDirection( getVerticalDirection() );

        int mode = 0;
        if (  isKeyPressed( SDLK_LSHIFT ) ||  isKeyPressed( SDLK_RSHIFT )) {
            if ( getVerticalDirection() == 0 )
                mode |= MoveUnitCommand::DisableHeightChange;
            else
                mode |= MoveUnitCommand::ShortestHeightChange;
        }

        ActionResult res = move->searchFields ( -1, mode );
        if ( !res.successful() ) {
            dispmessage2 ( res.getCode(), NULL );
            delete move;
            return;
        }

        for ( set<MapCoordinate3D>::const_iterator i = move->getReachableFields().begin(); i != move->getReachableFields().end(); ++i )
            actmap->getField( *i)->setaTemp(1);

        // if ( !CGameOptions::Instance()->dontMarkFieldsNotAccessible_movement )
        for ( set<MapCoordinate3D>::const_iterator i = move->getReachableFieldsIndirect().begin(); i != move->getReachableFieldsIndirect().end(); ++i )
            actmap->getField( *i)->setaTemp2(2);
        displaymap();
        NewGuiHost::pendingCommand = move;
        updateFieldInfo();


    } else {
        MoveUnitCommand* move = dynamic_cast<MoveUnitCommand*>(NewGuiHost::pendingCommand);
        if ( !move || move->getVerticalDirection() != getVerticalDirection() )
            return;

        move->setDestination( pos );

        if ( CGameOptions::Instance()->fastmove || move->getPath().size() ) {
            move->calcPath();
            actmap->cleartemps(7);
            displaymap();
            MapDisplayPG::CursorHiding ch;
            ActionResult res = move->execute( createContext(actmap));
            NewGuiHost::pendingCommand = NULL;
            if ( !res.successful() ) {
                delete move;
                dispmessage2(res);
                updateFieldInfo();
                return;
            }
        } else {
            move->calcPath();
            actmap->cleartemps(7);
            for ( AStar3D::Path::const_iterator i = move->getPath().begin(); i != move->getPath().end(); ++i )
                actmap->getField( *i ) ->setaTemp(1);
            displaymap();
        }
        updateFieldInfo();
    }
}





class Ascend : public MovementBase
{
protected:
    virtual void parametrizePathFinder( AStar3D& pathFinder ) {};
public:
    // void execute( const MapCoordinate& pos, ContainerBase* subject, int num );
    bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num ) {
        return ( key->keysym.unicode == 's' );
    };
    Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return IconRepository::getIcon("ascend-airplane.png");
    };
    ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return "a~s~cend";
    };
    int getVerticalDirection() {
        return 1;
    };

    bool available( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        if ( !commandPending() ) {
            Vehicle* eht = actmap->getField(pos)->vehicle;
            if ( eht )
                if ( eht->getOwner() == actmap->actplayer)
                    return MoveUnitCommand::ascendAvail ( eht );
            return false;
        } else
            return MovementBase::available( pos, subject, num );
    }

};






class Descend : public MovementBase
{
protected:
    int getVerticalDirection() {
        return -1;
    };
public:
    bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num ) {
        return ( key->keysym.unicode == 'd' );
    };
    Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return IconRepository::getIcon("descent-airplane.png");
    };
    ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return "~d~escend";
    };

    bool available( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        if ( !commandPending() ) {
            Vehicle* eht = actmap->getField(pos)->vehicle;
            if ( eht )
                if ( eht->getOwner() == actmap->actplayer)
                    return MoveUnitCommand::descendAvail ( eht );
            return false;
        } else
            return MovementBase::available( pos, subject, num );
    }

};





class EndTurn : public GuiFunction
{
public:
    bool available( const MapCoordinate& pos, ContainerBase* subject, int num ) ;
    void execute( const MapCoordinate& pos, ContainerBase* subject, int num );
    bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num ) {
        return ( key->keysym.unicode == 'e' );
    };
    Surface& getImage( const MapCoordinate& po, ContainerBase* subject, int nums ) {
        return IconRepository::getIcon("endturn.png");
    };
    ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return "~e~nd turn";
    };
};

bool EndTurn::available( const MapCoordinate& pos, ContainerBase* subject, int num )
{
    if (!commandPending() )
        if (actmap->levelfinished == false)
            return true;
    return false;
}


void EndTurn::execute( const MapCoordinate& pos, ContainerBase* subject, int num )
{
    if ( !CGameOptions::Instance()->endturnquestion || (choice_dlg("do you really want to end your turn ?","~y~es","~n~o") == 1)) {

        Player& player = actmap->player[actmap->actplayer];
        ASCString message = "The following units are about to crash\n\n";
        bool crashWarning = checkUnitsForCrash( player, message );
        if ( crashWarning ) {
            if ( choiceDialog( message, "continue", "cancel", "AircraftCrashWarning" ) == 2 )
                return;

        }


        static int autosave = 0;
        ASCString name = ASCString("autosave") + strrr( autosave ) + &savegameextension[1];

        savegame ( name, actmap );



        autosave = !autosave;

        actmap->sigPlayerUserInteractionEnds( player );

        next_turn( actmap, NextTurnStrategy_AskUser(), &getDefaultMapDisplay() );

        displaymap();
    }
}


class Attack : public GuiFunction
{
public:
    bool available( const MapCoordinate& pos, ContainerBase* subject, int num ) ;
    void execute( const MapCoordinate& pos, ContainerBase* subject, int num );
    bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num ) {
        return ( key->keysym.unicode == 'a' );
    };
    Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return IconRepository::getIcon("attack.png");
    };
    ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return "~a~ttack";
    };
};


bool Attack::available( const MapCoordinate& pos, ContainerBase* subject, int num )
{
    if ( !commandPending() ) {
        Vehicle* eht = actmap->getField(pos)->vehicle;
        if ( eht )
            if ( eht->color == actmap->actplayer * 8)
                return AttackCommand::avail ( eht );
    }
    return false;
}

void Attack::execute(  const MapCoordinate& pos, ContainerBase* subject, int num )
{
    AttackCommand* attack = new AttackCommand( actmap->getField(pos)->vehicle );

    ActionResult result = attack->searchTargets();
    if ( !result.successful() ) {
        dispmessage2 ( result );
        return;
    }

    AttackCommand::FieldList::const_iterator i;
    for ( i = attack->getAttackableUnits().begin(); i != attack->getAttackableUnits().end(); i++ )
        actmap->getField( i->first )->setaTemp(1);
    for ( i = attack->getAttackableBuildings().begin(); i != attack->getAttackableBuildings().end(); i++ )
        actmap->getField( i->first )->setaTemp(1);
    for ( i = attack->getAttackableObjects().begin(); i != attack->getAttackableObjects().end(); i++ )
        actmap->getField( i->first )->setaTemp(1);

    displaymap();
    attackGui.setupWeapons( attack );
    NewGuiHost::pushIconHandler( &attackGui );
}




class PowerSwitch : public GuiFunction
{
private:
    bool newState;
public:

    PowerSwitch( bool state ) : newState( state ) {};

    bool available( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        if (!commandPending() )  {
            MapField* fld = actmap->getField ( pos );
            if ( fld->vehicle )
                return PowerGenerationSwitchCommand::avail( fld->vehicle, newState );

        }
        return false;
    };

    bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num ) {
        return ( key->keysym.unicode == 'p' );
    };
    void execute( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        auto_ptr<PowerGenerationSwitchCommand> pgsc ( new PowerGenerationSwitchCommand( actmap->getField(pos)->vehicle));
        pgsc->setNewState( newState );
        ActionResult res = pgsc->execute( createContext( actmap ));
        if ( !res.successful() )
            displayActionError( res );
        else
            pgsc.release();

        updateFieldInfo();
    }
};



class PowerOn : public PowerSwitch
{
public:
    PowerOn() : PowerSwitch( true ) {};

    Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return IconRepository::getIcon("poweron.png");
    };

    ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return "enable ~p~ower generation";
    };
};


class PowerOff : public PowerSwitch
{
public:
    PowerOff() : PowerSwitch( false ) {};

    bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num ) {
        return ( key->keysym.unicode == 'p' );
    };

    Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return IconRepository::getIcon("poweroff.png");
    };

    ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return "disable ~p~ower generation";
    };
};



class UnitInfo : public GuiFunction
{
public:
    bool available( const MapCoordinate& pos, ContainerBase* subject, int num ) ;
    void execute( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        executeUserAction(ua_vehicleinfo);
    };
    Surface& getImage( const MapCoordinate& po, ContainerBase* subject, int nums ) {
        return IconRepository::getIcon("unitinfo.png");
    };
    ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return "unit ~i~nfo";
    };
    bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num ) {
        return ( key->keysym.unicode == 'i' );
    };
};

bool UnitInfo::available( const MapCoordinate& pos, ContainerBase* subject, int num )
{
    MapField* fld = actmap->getField(pos);
    if ( fld && fld->vehicle )
        if ( !commandPending() )
            if ( fld->vehicle != NULL)
                if (fieldvisiblenow( fld ))
                    return true;
    return false;
}






class DestructBuilding : public GuiFunction
{
public:
    bool available( const MapCoordinate& pos, ContainerBase* subject, int num ) ;
    void execute( const MapCoordinate& pos, ContainerBase* subject, int num );
    Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return IconRepository::getIcon("destructbuilding.png");
    };
    ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return "destruct building";
    };
};


bool DestructBuilding::available( const MapCoordinate& pos, ContainerBase* subject, int num )
{
    MapField* fld = actmap->getField(pos);
    if (!commandPending()) {
        if ( fld->vehicle )
            return DestructBuildingCommand::avail( fld->vehicle );
    } else {
        DestructBuildingCommand* dbc = dynamic_cast<DestructBuildingCommand*>( NewGuiHost::pendingCommand );
        if ( dbc ) {
            return dbc->isFieldUsable(pos);
        }
    }
    return false;
}

void DestructBuilding::execute(  const MapCoordinate& pos, ContainerBase* subject, int num )
{
    DestructBuildingCommand* dbc = dynamic_cast<DestructBuildingCommand*>( NewGuiHost::pendingCommand );
    if ( !dbc ) {
		Vehicle* unit = actmap->getField(pos)->vehicle;
		if ( !unit )
			return;

        auto_ptr<DestructBuildingCommand> db ( new DestructBuildingCommand( unit ));

        vector<MapCoordinate> fields = db->getFields();

        if ( fields.empty() ) {
            dispmessage2( 306, NULL );
            return;
        }
        for ( vector<MapCoordinate>::iterator i = fields.begin(); i != fields.end(); ++i )
            actmap->getField(*i)->setaTemp(1);

        repaintMap();

        NewGuiHost::pendingCommand = db.release();

        updateFieldInfo();

    } else {
        dbc->setTargetPosition( pos );
        actmap->cleartemps();

        ActionResult res = dbc->execute( createContext( actmap ));
        if ( !res.successful() ) {
            displayActionError(res);
            delete NewGuiHost::pendingCommand;
        }
        NewGuiHost::pendingCommand = NULL;

        repaintMap();
        updateFieldInfo();
    }
}


class OpenContainer : public GuiFunction
{
    static int containeractive;
public:
    bool available( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        MapField* fld = actmap->getField(pos);
        if ( fieldvisiblenow ( fld ) && fld->getContainer() ) {
            if ( !containeractive && !commandPending() ) {
                Player& player = fld->getContainer()->getMap()->player[fld->getContainer()->getOwner()];
                if ( fld->building && ( player.diplomacy.isAllied( actmap->actplayer) || actmap->getNeutralPlayerNum() == fld->building->getOwner() || actmap->getCurrentPlayer().stat==Player::supervisor )) {
                    if ( fld->building->getCompletion() == fld->building->typ->construction_steps-1 )
                        return true;
                }  else {
                    if ( fld->vehicle && fld->vehicle->typ->maxLoadableUnits  &&  (player.diplomacy.isAllied( actmap->actplayer)  || actmap->getCurrentPlayer().stat==Player::supervisor) )
                        return true;
                }
            }
        }
        return false;
    };

    void execute( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        MapField* fld = actmap->getField(pos);

        cargoDialog( fld->getContainer() );

        updateFieldInfo();
        repaintMap();
    }

    Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return IconRepository::getIcon("container.png");
    };

    bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num ) {
        return ( key->keysym.unicode == 'l' );
    };

    ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return "view ~l~oaded units";
    };
};

int OpenContainer::containeractive = 0;




class EnableReactionfire : public GuiFunction
{
public:
    bool available( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        Vehicle* eht = actmap->getField(pos)->vehicle;
        if ( !commandPending() )
            if ( eht )
                if ( eht->getOwner() == actmap->actplayer )
                    return ReactionFireSwitchCommand::avail( eht, true );

        return false;
    };
    bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num ) {
        return ( key->keysym.unicode == 'x' );
    };

    void execute( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        Vehicle* eht = actmap->getField(pos)->vehicle;
        auto_ptr<ReactionFireSwitchCommand> rf ( new ReactionFireSwitchCommand( eht ));
        rf->setNewState( true );
        ActionResult res = rf->execute( createContext( actmap ));
        if ( res.successful() )
            rf.release();
        else
            displayActionError( res );

        updateFieldInfo();
    }

    Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return IconRepository::getIcon("enable-reactionfire.png");
    };

    ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return "enable reaction fire (~x~)";
    };
};

class DisableReactionfire : public GuiFunction
{
public:
    bool available( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        Vehicle* eht = actmap->getField(pos)->vehicle;
        if ( !commandPending() )
            if ( eht )
                if ( eht->getOwner() == actmap->actplayer )
                    return ReactionFireSwitchCommand::avail( eht, false );

        return false;
    };

    bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num ) {
        return ( key->keysym.unicode == 'x' );
    };
    void execute( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        Vehicle* eht = actmap->getField(pos)->vehicle;
        auto_ptr<ReactionFireSwitchCommand> rf ( new ReactionFireSwitchCommand( eht ));
        rf->setNewState( false );
        ActionResult res = rf->execute( createContext( actmap ));
        if ( res.successful() )
            rf.release();
        else
            displayActionError( res );

        repaintMap();
        updateFieldInfo();
    }

    Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return IconRepository::getIcon("disable-reactionfire.png");
    };

    ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return "disable reaction fire (~x~)";
    };
};


class JumpDriveIcon : public GuiFunction, public SigC::Object
{
public:
    bool available( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        if ( !commandPending() ) {

            Vehicle* eht = actmap->getField(pos)->vehicle;
            if ( eht )
                if ( eht->getOwner() == actmap->actplayer )
                    return JumpDriveCommand::available(eht).getAvailability() >= ActionAvailability::partially;
        } else {
            JumpDriveCommand* jdc = dynamic_cast<JumpDriveCommand*>(NewGuiHost::pendingCommand);
            if ( jdc ) {
                if ( jdc->fieldReachable( pos ))
                    return true;
            }
        }

        return false;
    };

    bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num ) {
        return ( key->keysym.unicode == 'j' );
    };

    void execute( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        if ( !commandPending() ) {
            Vehicle* eht = actmap->getField(pos)->vehicle;
            if ( !eht )
                return;

            ActionAvailability aa = JumpDriveCommand::available(eht);
            if ( !aa.ready() ) {
               warningMessage( aa.getMessage() );
               return;
            }
            
            auto_ptr<JumpDriveCommand> jdc ( new JumpDriveCommand( eht ));

            vector<MapCoordinate> fields = jdc->getDestinations();
            for ( vector<MapCoordinate>::const_iterator i = fields.begin(); i != fields.end(); ++i )
                actmap->getField(*i)->setaTemp(1);

            if ( fields.size() ) {
                repaintMap();
                NewGuiHost::pendingCommand = jdc.release();
                updateFieldInfo();
            } else
                dispmessage2( 22602 );
        } else {
            JumpDriveCommand* jdc = dynamic_cast<JumpDriveCommand*>(NewGuiHost::pendingCommand);
            if ( jdc ) {
               NewGuiHost::pendingCommand = NULL;
               jdc->setDestination( pos );
                actmap->cleartemps(7);
                ActionResult res = jdc->execute( createContext( actmap ));;
                mapChanged( actmap );

                if ( !res.successful() ) {
                    delete NewGuiHost::pendingCommand;
                    displayActionError( res );
                }
                updateFieldInfo();
            }
        }
    }

    Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return IconRepository::getIcon("jumpdrive.png");
    };

    ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return "activate jump drive (~j~)";
    };
};



class RepairUnit : public GuiFunction
{
public:
    bool available( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        MapField* fld = actmap->getField(pos);
        if (!commandPending()) {
            if ( fld && fld->vehicle )
                if (fld->vehicle->getOwner() == actmap->actplayer )
                    if ( RepairUnitCommand::availExternally ( fld->vehicle ) )
                        return true;
        } else {
            if ( NewGuiHost::pendingCommand ) {
                RepairUnitCommand* service = dynamic_cast<RepairUnitCommand*>(NewGuiHost::pendingCommand);
                if ( service && fld->vehicle )
                    return service->validTarget( fld->vehicle );
            }
        }
        return false;
    };

    bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num ) {
        return ( key->keysym.unicode == 'r' );
    };
    void execute( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        if ( !commandPending()  ) {
            auto_ptr<RepairUnitCommand> rp ( new RepairUnitCommand( actmap->getField(pos)->vehicle ));

            vector<Vehicle*> targets = rp->getExternalTargets();

            int fieldCount = 0;
            for ( vector<Vehicle*>::iterator i = targets.begin(); i != targets.end(); ++i  ) {
                actmap->getField ( (*i)->getPosition() )->setaTemp(1);
                fieldCount++;
            }
            if ( !fieldCount ) {
                dispmessage2 ( 211 );
            } else {
                NewGuiHost::pendingCommand = rp.release();
                repaintMap();
                updateFieldInfo();
            }
        } else {
            if ( NewGuiHost::pendingCommand ) {
                RepairUnitCommand* service = dynamic_cast<RepairUnitCommand*>(NewGuiHost::pendingCommand);
                if ( service ) {
                    MapField* fld = actmap->getField(pos);
                    if ( fld->vehicle ) {
                        service->setTarget( fld->vehicle );
                        ActionResult res = service->execute( createContext ( actmap ));
                        if ( !res.successful() ) {
                            displayActionError(res);
                            delete NewGuiHost::pendingCommand;
                        }
                        NewGuiHost::pendingCommand = NULL;

                        actmap->cleartemps(7);
                        repaintMap();
                        updateFieldInfo();
                    }
                }
            }
        }
    }

    Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return IconRepository::getIcon("repair.png");
    };

    ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        MapField* fld = actmap->getField(pos);
        if ( fld && fld->vehicle && NewGuiHost::pendingCommand  ) {
            RepairUnitCommand* service = dynamic_cast<RepairUnitCommand*>(NewGuiHost::pendingCommand);
            if ( service ) {
                Resources r;
                service->getRepairingUnit( )->getMaxRepair ( fld->vehicle, 0, r);
                return "~r~epair unit (cost: " + r.toString() + ")";
            }
        }
        return "~r~epair a unit";
    };
};




class RefuelUnitCommand : public GuiFunction
{
public:
    bool available( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        MapField* fld = actmap->getField(pos);
        if (!commandPending()) {
            if ( fld && fld->getContainer() )
                if (fld->getContainer()->getOwner() == actmap->actplayer )
                    if ( ServiceCommand::availExternally ( fld->getContainer() ) )
                        return true;
        } else {
            if ( NewGuiHost::pendingCommand ) {
                ServiceCommand* service = dynamic_cast<ServiceCommand*>(NewGuiHost::pendingCommand);
                if ( service && fld->getContainer() ) {
                    const ServiceTargetSearcher::Targets& destinations = service->getDestinations();
                    return find( destinations.begin(), destinations.end(), fld->getContainer() ) != destinations.end();
                }
            }
        }
        return false;

    };
    bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num ) {
        return ( key->keysym.unicode == 'f' );
    };

    void execute( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        if ( !NewGuiHost::pendingCommand ) {
            auto_ptr<ServiceCommand> service ( new ServiceCommand( subject ));

            MapCoordinate srcPos = subject->getPosition();
            int fieldCount = 0;

            const ServiceTargetSearcher::Targets& destinations = service->getDestinations();
            for ( ServiceTargetSearcher::Targets::const_iterator i = destinations.begin(); i != destinations.end(); ++i ) {
                MapCoordinate targetPos = (*i)->getPosition();
                if ( targetPos != srcPos ) {
                    MapField* fld = subject->getMap()->getField ( targetPos );
                    fieldCount++;
                    fld->setaTemp(1);
                }
            }
            if ( !fieldCount ) {
                dispmessage2 ( 211 );
                return;
            }

            NewGuiHost::pendingCommand = service.release();
            displaymap();
            updateFieldInfo();

        } else {
            ServiceCommand* service = dynamic_cast<ServiceCommand*>(NewGuiHost::pendingCommand);

            service->setDestination( subject );
            service->getTransferHandler().fillDest();
            service->saveTransfers();
            ActionResult res = service->execute( createContext( actmap ));
            if ( !res.successful() ) {
                dispmessage2( res );
                delete NewGuiHost::pendingCommand;
            }

            NewGuiHost::pendingCommand = NULL;
            actmap->cleartemps ( 7 );
            displaymap();
            updateFieldInfo();
        }
    }

    Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return IconRepository::getIcon("refuel.png");
    };

    ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return "re~f~uel a unit";
    };
};


class RefuelUnitDialogCommand : public GuiFunction
{
public:
    bool available( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        if ( NewGuiHost::pendingCommand ) {
            ServiceCommand* service = dynamic_cast<ServiceCommand*>(NewGuiHost::pendingCommand);
            MapField* fld = actmap->getField(pos);
            if ( service && fld->getContainer() ) {
                const ServiceTargetSearcher::Targets& destinations = service->getDestinations();
                return find( destinations.begin(), destinations.end(), fld->getContainer() ) != destinations.end();
            }
        }
        return false;
    };
    bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num ) {
        return ( key->keysym.unicode == 'f' && (modifier & KMOD_SHIFT)  );
    };

    void execute( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        ServiceCommand* service = dynamic_cast<ServiceCommand*>(NewGuiHost::pendingCommand);
        MapField* fld = actmap->getField(pos);
        service->setDestination( fld->getContainer() );
        ammoTransferWindow( service->getRefueller(), actmap->getField(pos)->getContainer(), service );

        if ( service->getState() != Command::Finished )
            delete NewGuiHost::pendingCommand;

        NewGuiHost::pendingCommand = NULL;
        actmap->cleartemps ( 7 );
        displaymap();
        updateFieldInfo();
    }

    Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return IconRepository::getIcon("refuel-dialog.png");
    };

    ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return "re~F~uel dialog ";
    };
};



class PutMine : public GuiFunction
{
public:
    bool available( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        MapField* fld = actmap->getField(pos);
        if ( !commandPending())
            if ( fld->vehicle )
                return PutMineCommand::avail(fld->vehicle);
        return false;
    };

    bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num ) {
        return ( key->keysym.unicode == 'm' );
    };

    void execute( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        MapField* fld = actmap->getField(pos);
        if ( fld->vehicle ) {
            auto_ptr<PutMineCommand> poc ( new PutMineCommand( actmap->getField(pos)->vehicle ));
            ActionResult res = poc->searchFields();
            if ( res.successful() ) {
                vector<MapCoordinate> fields = poc->getFields();
                for ( vector<MapCoordinate>::iterator i = fields.begin(); i != fields.end(); ++i )
                    actmap->getField(*i)->setaTemp(1);

                NewGuiHost::pendingCommand = poc.get();
                poc.release();

                repaintMap();
                updateFieldInfo();
            } else
                MessagingHub::Instance().statusInformation("can't build or remove any objects here");
        }
    }

    Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return IconRepository::getIcon("putmine.png");
    };

    ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return "put / remove ~m~ines";
    };
};


class PutMineStage2 : public GuiFunction
{
private:
    MineTypes type;
protected:
    PutMineStage2( MineTypes type ) {
        this->type = type;
    };

public:
    bool available( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        if ( !NewGuiHost::pendingCommand )
            return false;

        PutMineCommand* pmc = dynamic_cast<PutMineCommand*>( NewGuiHost::pendingCommand );
        if ( pmc ) {
            const vector<MineTypes>& mines = pmc->getCreatableMines( pos );
            for ( vector<MineTypes>::const_iterator i = mines.begin(); i != mines.end(); ++i )
                if ( *i == type )
                    return true;
        }
        return false;
    };

    void execute( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        PutMineCommand* pmc = dynamic_cast<PutMineCommand*>( NewGuiHost::pendingCommand );
        if ( pmc ) {
            actmap->cleartemps();
            pmc->setCreationTarget( pos, type );
            ActionResult res = pmc->execute( createContext( actmap ));
            if ( !res.successful() ) {
                displayActionError(res);
                delete NewGuiHost::pendingCommand;
            }
            NewGuiHost::pendingCommand = NULL;
            updateFieldInfo();
        }
    }
};

class PutAntiTankMine : public PutMineStage2
{
public:

    PutAntiTankMine() : PutMineStage2( cmantitankmine ) {};

    Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return IconRepository::getIcon("putantitankmine.png");
    };

    ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return "put anti-tank mine";
    };
};

class PutAntiPersonalMine : public PutMineStage2
{
public:

    PutAntiPersonalMine() : PutMineStage2( cmantipersonnelmine ) {};

    Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return IconRepository::getIcon("putantipersonalmine.png");
    };

    ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return "put anti-personnel mine";
    };
};




class PutAntiShipMine : public PutMineStage2
{
public:
    PutAntiShipMine() : PutMineStage2( cmfloatmine ) {};

    Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return IconRepository::getIcon("putantishipmine.png");
    };

    ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return "put anti-ship mine";
    };
};

class PutAntiSubMine : public PutMineStage2
{
public:
    PutAntiSubMine() : PutMineStage2( cmmooredmine ) {};

    Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return IconRepository::getIcon("putantisubmine.png");
    };

    ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return "put anti-submarine mine";
    };
};


class RemoveMine : public GuiFunction
{
public:
    bool available( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        if ( !NewGuiHost::pendingCommand )
            return false;

        PutMineCommand* pmc = dynamic_cast<PutMineCommand*>( NewGuiHost::pendingCommand );
        if ( pmc )
            return pmc->getRemovableMines(pos);

        return false;
    };

    void execute( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        PutMineCommand* pmc = dynamic_cast<PutMineCommand*>( NewGuiHost::pendingCommand );
        if ( pmc ) {
            pmc->setRemovalTarget( pos );
            actmap->cleartemps();
            ActionResult res = pmc->execute( createContext( actmap ));
            if ( !res.successful() ) {
                displayActionError(res);
                delete NewGuiHost::pendingCommand;
            }
            NewGuiHost::pendingCommand = NULL;
            updateFieldInfo();
        }
    }

    Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return IconRepository::getIcon("removemine.png");
    };

    ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return "remove mine";
    };
};






class ObjectBuildingGui : public GuiIconHandler, public GuiFunction, public SigC::Object
{
    Vehicle* veh;

    void mapDeleted( GameMap& map ) {
        if ( NewGuiHost::getIconHandler() == this )
            NewGuiHost::popIconHandler();
    }

protected:
    enum Mode { Build, Remove };

    bool available( const MapCoordinate& pos, ContainerBase* subject, int num );
    void execute( const MapCoordinate& pos, ContainerBase* subject, int num );
    Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num );
    ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num );

    bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num );

    void addButton( int &num, const MapCoordinate& mc, ContainerBase* subject, int id );

public:
    ObjectBuildingGui() : veh( NULL ) {
        GameMap::sigMapDeletion.connect( SigC::slot( *this, &ObjectBuildingGui::mapDeleted ));
    };

    bool init( Vehicle* vehicle );
    void eval( const MapCoordinate& mc, ContainerBase* subject );

};


bool ObjectBuildingGui::init( Vehicle* vehicle )
{
    veh = vehicle;
    return true;
}


bool ObjectBuildingGui::available( const MapCoordinate& pos, ContainerBase* subject, int num )
{
    return true;
    /*
    if ( num == 0 )
       return true;

    checkObject

    if ( getEntry(pos,num).first )
       return true;
    else
       return false;
       */
}

void ObjectBuildingGui::execute( const MapCoordinate& pos, ContainerBase* subject, int num )
{
    PutObjectCommand* poc = dynamic_cast<PutObjectCommand*>(NewGuiHost::pendingCommand);
    actmap->cleartemps();
    if ( num && poc ) {
        poc->setTarget( pos, abs(num) );
        ActionResult res = poc->execute( createContext( actmap ));
        if ( !res.successful()) {
            displayActionError(res);
            delete NewGuiHost::pendingCommand;
        }
    } else {
        delete NewGuiHost::pendingCommand;
    }
    repaintMap();

    NewGuiHost::pendingCommand = NULL;

    NewGuiHost::popIconHandler();
    updateFieldInfo();
}


Surface buildGuiIcon( const Surface& image, bool remove = false )
{
    const Surface& cancelIcon = IconRepository::getIcon("empty-pressed.png" ); //  "cancel.png"
    Surface s = Surface::createSurface( cancelIcon.w(), cancelIcon.h(), 32, 0 );

    const Surface& o = image;
    if ( o.GetPixelFormat().BitsPerPixel() == 32 ) {
        MegaBlitter<4,4,ColorTransform_None, ColorMerger_PlainOverwrite, SourcePixelSelector_DirectZoom> blitter;
        blitter.setSize( o.w(), o.h(), s.w(), s.h() );
        blitter.blit( o, s, SPoint(int((s.w() - blitter.getZoomX() * o.w())/2), int((s.h() - blitter.getZoomY() * o.h())/2)));
    } else {
        MegaBlitter<1,4,ColorTransform_None, ColorMerger_AlphaOverwrite, SourcePixelSelector_DirectZoom> blitter;
        blitter.setSize( o.w(), o.h(), s.w(), s.h() );
        blitter.blit( o, s, SPoint(int((s.w() - blitter.getZoomX() * o.w())/2), int((s.h() - blitter.getZoomY() * o.h())/2)));
    }

    if ( remove ) {
        MegaBlitter<4,4,ColorTransform_None, ColorMerger_AlphaOverwrite> blitter;
        Surface& removegui = IconRepository::getIcon( "cancel-addon.png" );
        blitter.blit( removegui, s,  SPoint((s.w() - removegui.w())/2, (s.h() - removegui.h())/2));
    }
    return s;
}


bool ObjectBuildingGui::checkForKey( const SDL_KeyboardEvent* key, int modifier, int num )
{
    if ( num > 0 )
        return false;
    else
        return ( key->keysym.sym == SDLK_ESCAPE || key->keysym.unicode == 'c' );
}

Surface& ObjectBuildingGui::getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
{
    if ( num == 0 )
        return IconRepository::getIcon("cancel.png");

    ObjectType* objtype = objectTypeRepository.getObject_byID( abs(num) );

    if ( !objtype )
        return IconRepository::getIcon("cancel.png");

    static map<int,Surface> removeIconRepository;

    if ( removeIconRepository.find( num ) != removeIconRepository.end() )
        return removeIconRepository[num];

    removeIconRepository[num] = buildGuiIcon( objtype->getPicture(), num < 0 );
    return removeIconRepository[num];
}


ASCString ObjectBuildingGui::getName( const MapCoordinate& pos, ContainerBase* subject, int num )
{
    if ( num == 0 )
        return "~c~ancel";

    ObjectType* objtype = objectTypeRepository.getObject_byID( abs(num) );
    if ( !objtype )
        return "";

    ASCString result;
    if ( num >= 0 ) {
        result.format( "Build %s (%d Material, %d Fuel, %d Movepoints)", objtype->name.c_str(), objtype->buildcost.material, objtype->buildcost.fuel, objtype->build_movecost );
        // num = -num;
    } else
        result.format( "Remove %s (%d Material, %d Fuel, %d Movepoints)", objtype->name.c_str(), objtype->removecost.material, objtype->removecost.fuel, objtype->remove_movecost );

    return result;
}



void ObjectBuildingGui::addButton( int &num, const MapCoordinate& mc, ContainerBase* subject, int id )
{
    GuiButton* b = host->getButton(num);
    b->registerFunc( this, mc, subject, id );
    b->Show();
    ++num;
}



void ObjectBuildingGui::eval( const MapCoordinate& mc, ContainerBase* subject )
{
    PutObjectCommand* poc = dynamic_cast<PutObjectCommand*>(NewGuiHost::pendingCommand);

    if ( poc ) {
        int num = 0;
        const vector<int>& creatable = poc->getCreatableObjects( mc );
        for ( vector<int>::const_iterator i = creatable.begin(); i != creatable.end(); ++i ) {
            addButton(num, mc, veh, *i);
        }


        const vector<int>& removable = poc->getRemovableObjects( mc );
        for ( vector<int>::const_iterator i = removable.begin(); i != removable.end(); ++i ) {
            addButton(num, mc, veh, -(*i));
        }

        GuiButton* b = host->getButton(num);
        b->registerFunc( this, mc, subject, 0 );
        b->Show();
        ++num;

        host->disableButtons(num);
    }

}



ObjectBuildingGui objectBuildingGui;


class BuildObject : public GuiFunction
{
public:
    bool available( const MapCoordinate& pos, ContainerBase* subject, int num ) ;
    void execute( const MapCoordinate& pos, ContainerBase* subject, int num );
    Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return IconRepository::getIcon("buildobjects.png");
    };
    ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return "~c~onstruct / remove objects";
    };
    bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num ) {
        return ( key->keysym.unicode == 'c' );
    };
};


bool BuildObject::available( const MapCoordinate& pos, ContainerBase* subject, int num )
{
    MapField* fld = actmap->getField(pos);
    if (!commandPending())
        if ( fld && fld->vehicle )
            return PutObjectCommand::avail(fld->vehicle );
    return false;
}

void BuildObject::execute(  const MapCoordinate& pos, ContainerBase* subject, int num )
{
    MapField* fld = actmap->getField(pos);
    if ( fld->vehicle ) {
        auto_ptr<PutObjectCommand> poc ( new PutObjectCommand( actmap->getField(pos)->vehicle ));
        ActionResult res =poc->searchFields();
        if ( res.successful() ) {
            vector<MapCoordinate> fields = poc->getFields();
            for ( vector<MapCoordinate>::iterator i = fields.begin(); i != fields.end(); ++i )
                actmap->getField(*i)->setaTemp(1);

            NewGuiHost::pendingCommand = poc.get();
            poc.release();
            NewGuiHost::pushIconHandler( &objectBuildingGui );

            repaintMap();
            updateFieldInfo();
        } else
            MessagingHub::Instance().statusInformation("can't build or remove any objects here");
    }
}





class BuildVehicleCommand : public GuiFunction
{
public:
    bool available( const MapCoordinate& pos, ContainerBase* subject, int num ) ;
    void execute( const MapCoordinate& pos, ContainerBase* subject, int num );
    Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return IconRepository::getIcon("constructunit.png");
    };
    ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return "Unit construction Command";
    };
};


bool BuildVehicleCommand::available( const MapCoordinate& pos, ContainerBase* subject, int num )
{
    MapField* fld = actmap->getField(pos);
    if (!commandPending())
        if ( fld && fld->vehicle )
            if (fld->vehicle->getOwner() == actmap->actplayer )
                return ConstructUnitCommand::externalConstructionAvail( fld->vehicle );

    if ( NewGuiHost::pendingCommand ) {
        ConstructUnitCommand* construct = dynamic_cast<ConstructUnitCommand*>(NewGuiHost::pendingCommand);
        if ( construct )
            return construct->isFieldUsable(pos);

    }
    return false;
}

void BuildVehicleCommand::execute(  const MapCoordinate& pos, ContainerBase* subject, int num )
{

    ConstructUnitCommand* construct = dynamic_cast<ConstructUnitCommand*>(NewGuiHost::pendingCommand);
    if ( !construct  ) {
        delete construct;
        auto_ptr<ConstructUnitCommand> constructCommand ( new ConstructUnitCommand( subject ));
        constructCommand->setMode( ConstructUnitCommand::external);
        ConstructUnitCommand::Producables buildables = constructCommand->getProduceableVehicles();

        VehicleProduction_SelectionWindow fsw( NULL, PG_Rect( 10, 10, 450, 550 ), subject, buildables, false );
        fsw.Show();
        fsw.RunModal();
        const VehicleType* v = fsw.getVehicletype();
        if ( v ) {

            for ( ConstructUnitCommand::Producables::const_iterator i = buildables.begin(); i != buildables.end(); ++i )
                if ( i->type == v ) {
                    if ( i->prerequisites.getValue() & ( ConstructUnitCommand::Lack::Energy  | ConstructUnitCommand::Lack::Material | ConstructUnitCommand::Lack::Fuel )) {
                        warningMessage("Not enough resources to build unit");
                        return;
                    }

                    if ( i->prerequisites.getValue() & ( ConstructUnitCommand::Lack::Movement )) {
                        ASCString message = "Not enough movement to build unit";
                        Vehicle* constructor = dynamic_cast<Vehicle*>(subject);
                        if ( constructor )
                            message += "\nRequired: " + ASCString::toString(constructor->typ->unitConstructionMoveCostPercentage * constructor->maxMovement() / 100 ) + " points";
                        warningMessage(message);
                        return;
                    }

                    if ( i->prerequisites.getValue() & ( ConstructUnitCommand::Lack::Research )) {
                        warningMessage("This unit has not been researched yet");
                        return;
                    }
                }


            constructCommand->setVehicleType( v );
            vector<MapCoordinate> fields = constructCommand->getFields();

            if ( fields.size() ) {
                for ( vector<MapCoordinate>::const_iterator i = fields.begin(); i != fields.end(); ++i )
                    subject->getMap()->getField( *i )->setaTemp(1);

                repaintMap();

                NewGuiHost::pendingCommand = constructCommand.release();

                updateFieldInfo();

            } else {
                warningMessage("no fields to construct unit.\n" + v->terrainaccess.toString());
            }
        }
    } else {
        actmap->cleartemps();
        construct->setTargetPosition( pos );
        ActionResult res = construct->execute ( createContext( actmap ) );
        if ( !res.successful() ) {
            delete NewGuiHost::pendingCommand;
            repaintMap();
            dispmessage2( res );
        }
        NewGuiHost::pendingCommand = NULL;
        updateFieldInfo();
    }

}



Surface generate_gui_build_icon ( BuildingType* bld )
{
    Surface s = Surface::createSurface(500,500,32, 0);

    int minx = 1000;
    int miny = 1000;
    int maxx = 0;
    int maxy = 0;

    for (int y = 0; y <= 5; y++)
        for (int x = 0; x <= 3; x++)
            if (bld->fieldExists( BuildingType::LocalCoordinate(x,y) ) ) {
                int xp = fielddistx * x  + fielddisthalfx * ( y & 1);
                int yp = fielddisty * y ;
                if ( xp < minx )
                    minx = xp;
                if ( yp < miny )
                    miny = yp;
                if ( xp > maxx )
                    maxx = xp;
                if ( yp > maxy )
                    maxy = yp;

                // bld->paintSingleField( s, SPoint(xp,yp), BuildingType::LocalCoordinate(x,y) );
                bld->paintSingleField( s, SPoint(xp,yp),BuildingType::LocalCoordinate(x,y) );
            }
    maxx += fieldxsize;
    maxy += fieldysize;

    /*
    Surface sc = Surface::Wrap( PG_Application::GetScreen() );
    sc.Blit( s, SPoint(0,0 ));
    PG_Application::UpdateRect( PG_Application::GetScreen(), 0,0,0,0);
    */

    Surface s2 = Surface::createSurface(maxx-minx+1,maxy-miny+1,32,0);
    // s2.Blit( s, SDLmm::SRect(SPoint(minx,miny), SPoint(maxx,maxy) ), SPoint(0,0));

    MegaBlitter<4,4,ColorTransform_None,ColorMerger_AlphaOverwrite,SourcePixelSelector_Rectangle> blitter;
    blitter.setSrcRectangle( SPoint( minx, miny ), maxx-minx, maxy-miny );
    blitter.blit ( s, s2, SPoint(0,0) );

    /*

    MegaBlitter<4,4,ColorTransform_None,ColorMerger_AlphaOverwrite,SourcePixelSelector_Zoom> blitter;

    blitter.setSize( s2.w(), s2.h(), s3.w(), s3.h() );
    blitter.initSource ( s2 );
    blitter.blit ( s2, s3, SPoint((s3.w() - blitter.getWidth())/2, (s3.h() - blitter.getHeight())/2) );
    */

    return s2;
}


class ConstructBuilding : public GuiFunction
{
public:
    bool available( const MapCoordinate& pos, ContainerBase* subject, int num ) ;
    void execute( const MapCoordinate& pos, ContainerBase* subject, int num );
    Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return IconRepository::getIcon("constructbuilding.png");
    };
    ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        ConstructBuildingCommand* build = dynamic_cast<ConstructBuildingCommand*>(NewGuiHost::pendingCommand );
        if ( !build )
            return "Building construction";
        else {
            return "Select building entrance position";
        }
    };
};


bool ConstructBuilding::available( const MapCoordinate& pos, ContainerBase* subject, int num )
{
    MapField* fld = actmap->getField(pos);
    if (!commandPending()) {
        if ( fld->vehicle )
            if ( ConstructBuildingCommand::avail( fld->vehicle ))
                if (fld->vehicle->getOwner() == actmap->actplayer )
                    return true;
    }

    ConstructBuildingCommand* build = dynamic_cast<ConstructBuildingCommand*>(NewGuiHost::pendingCommand );
    if ( build )
        return build->isFieldUsable( pos );

    return false;
}


class BuildingConstructionSelection : public BuildingTypeSelectionItemFactory
{
private:
    const BuildingType* selectedType;
protected:
    void BuildingTypeSelected( const BuildingType* type ) {
        selectedType = type;
    };
public:
    BuildingConstructionSelection ( Resources plantResources, const Container& types, const Player& player ) : BuildingTypeSelectionItemFactory(plantResources, types, player ), selectedType(NULL) {};
    const BuildingType* getSelectedType() {
        return selectedType;
    };
};

void ConstructBuilding::execute(  const MapCoordinate& pos, ContainerBase* subject, int num )
{

    ConstructBuildingCommand* build = dynamic_cast<ConstructBuildingCommand*>(NewGuiHost::pendingCommand );

    if ( !build  ) {
        MapField* fld = actmap->getField(pos);


        auto_ptr<ConstructBuildingCommand> cbc ( new ConstructBuildingCommand( fld->vehicle ));

        ConstructBuildingCommand::Producables producables = cbc->getProduceableBuildings();

        BuildingConstructionSelection::Container buildings;
        for ( ConstructBuildingCommand::Producables::const_iterator i = producables.begin(); i != producables.end(); ++i )
            if ( !( i->prerequisites.getValue() & (ConstructBuildingCommand::Lack::Research | ConstructBuildingCommand::Lack::Level )))
                buildings.push_back( i->type );

        if ( buildings.empty() ) {
            dispmessage2( 303, NULL );
            return;
        }


        BuildingConstructionSelection* bcs = new BuildingConstructionSelection ( fld->vehicle->getResource( Resources(maxint, maxint, maxint), true ), buildings, fld->vehicle->getOwningPlayer() );
        ItemSelectorWindow isw ( NULL, PG_Rect( -1, -1, 550, 650), "Select Building", bcs);
        isw.Show();
        isw.RunModal();
        isw.Hide();

        if ( bcs->getSelectedType() && cbc->buildingProductionPrerequisites( bcs->getSelectedType()).ok() ) {

            cbc->setBuildingType( bcs->getSelectedType() );

            vector<MapCoordinate> fields = cbc->getFields();
            if ( fields.size() == 0 ) {
                dispmessage2(301, NULL );
                return;
            }

            for ( vector<MapCoordinate>::iterator i = fields.begin(); i != fields.end(); ++i )
                actmap->getField(*i)->setaTemp(1);

            repaintMap();

            NewGuiHost::pendingCommand = cbc.release();

            updateFieldInfo();
        }
    } else {
        actmap->cleartemps();
        build->setTargetPosition( pos );
        ActionResult res = build->execute( createContext( actmap ));

        if ( !res.successful() ) {
            displayActionError(res);
            delete NewGuiHost::pendingCommand;
        }

        NewGuiHost::pendingCommand = NULL;


        updateFieldInfo();
    }
}






class InternalAmmoTransferDialog : public GuiFunction
{
public:
    bool available( const MapCoordinate& pos, ContainerBase* subject, int num ) {

        // deactivated, because there shouldn't be any units left which need this function
        return false;

        if ( subject && subject->getMap()->getPlayer(subject).diplomacy.isAllied( subject->getMap()->actplayer ))
            if (!commandPending()) {
                Vehicle* v = dynamic_cast<Vehicle*>(subject);
                if ( v )
                    return InternalAmmoTransferCommand::avail( v );
            }

        return false;
    };

    bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num ) {
        return ( key->keysym.unicode == 't' && (modifier & KMOD_SHIFT)  );
    };

    void execute( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        internalAmmoTransferWindow( (Vehicle*) subject );
        displaymap();
        updateFieldInfo();
    }

    Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return IconRepository::getIcon("internalAmmoTransfer-dialog.png");
    };

    ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return "internal Ammo ~T~ransfer dialog ";
    };
};

class SelfDestructIcon : public GuiFunction
{
    static int containeractive;
public:
    bool available( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        MapField* fld = actmap->getField(pos);
        ContainerBase* c = fld->getContainer();
        if ( fieldvisiblenow ( fld ) && c && !commandPending() && actmap->getPlayer(c).diplomacy.isAllied(actmap->actplayer))
            return DestructUnitCommand::avail( c );
        else
            return false;
    };

    void execute( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        MapField* fld = actmap->getField(pos);
        if (choice_dlg("do you really want to destruct this unit?","~y~es","~n~o") == 1) {
            auto_ptr<DestructUnitCommand> destructor ( new  DestructUnitCommand( fld->getContainer() ));
            ActionResult res = destructor->execute( createContext( actmap ) );
            if ( res.successful() )
                destructor.release();
            else
                displayActionError( res );
            updateFieldInfo();
            repaintMap();
        }
    }

    Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return IconRepository::getIcon("self-destruct.png");
    };

    bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num ) {
        return false;
    };

    ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num ) {
        return "self destruct";
    };
};



GuiIconHandler primaryGuiIcons;






} // namespace GuiFunctions



void registerGuiFunctions( GuiIconHandler& handler )
{
    handler.registerUserFunction( new GuiFunctions::Movement() );
    handler.registerUserFunction( new GuiFunctions::UnitInfo() );
    handler.registerUserFunction( new GuiFunctions::Attack() );
    handler.registerUserFunction( new GuiFunctions::JumpDriveIcon() );
    handler.registerUserFunction( new GuiFunctions::PowerOn() );
    handler.registerUserFunction( new GuiFunctions::PowerOff() );
    handler.registerUserFunction( new GuiFunctions::BuildObject() );
    handler.registerUserFunction( new GuiFunctions::BuildVehicleCommand() );
    handler.registerUserFunction( new GuiFunctions::ConstructBuilding() );
    handler.registerUserFunction( new GuiFunctions::DestructBuilding() );
//   handler.registerUserFunction( new GuiFunctions::SearchForMineralResources() );
    handler.registerUserFunction( new GuiFunctions::OpenContainer() );
    handler.registerUserFunction( new GuiFunctions::EnableReactionfire() );
    handler.registerUserFunction( new GuiFunctions::DisableReactionfire() );
    handler.registerUserFunction( new GuiFunctions::Ascend );
    handler.registerUserFunction( new GuiFunctions::Descend );
    handler.registerUserFunction( new GuiFunctions::RepairUnit );
    handler.registerUserFunction( new GuiFunctions::RefuelUnitCommand );
    handler.registerUserFunction( new GuiFunctions::RefuelUnitDialogCommand );
    // handler.registerUserFunction( new GuiFunctions::ViewMap );
    handler.registerUserFunction( new GuiFunctions::PutMine );
    handler.registerUserFunction( new GuiFunctions::PutAntiTankMine );
    handler.registerUserFunction( new GuiFunctions::PutAntiPersonalMine );
    handler.registerUserFunction( new GuiFunctions::PutAntiShipMine );
    handler.registerUserFunction( new GuiFunctions::PutAntiSubMine );
    handler.registerUserFunction( new GuiFunctions::RemoveMine );
    handler.registerUserFunction( new GuiFunctions::InternalAmmoTransferDialog );

    handler.registerUserFunction( new GuiFunctions::EndTurn() );
    handler.registerUserFunction( new GuiFunctions::SelfDestructIcon() );
    handler.registerUserFunction( new GuiFunctions::Cancel() );

}

