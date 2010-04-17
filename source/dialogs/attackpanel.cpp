/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <pgimage.h>
#include <pgtooltiphelp.h>

#include "attackpanel.h"

#include "../paradialog.h"
#include "../attack.h"
#include "../explosivemines.h"
#include "../iconrepository.h"

#include "../soundList.h"
#include "../gamemap.h"
#include "../events.h"
#include "../gameoptions.h"
#include "../windowing.h"
#include "../sdl/graphicsqueue.h"

#define USE_COLOR_CONSTANTS
#include <pgcolors.h>


class GetAttackerImage : public FightVisitor {
   private:
      const SDL_Rect dst;
      Surface& surf;

      void paint( UnitAttacksSomething& battle )
      {
         battle.getAttackingUnit()->typ->paint( surf, SPoint( dst.x, dst.y ), battle.getAttackingUnit()->getOwningPlayer().getPlayerColor() );
      }
      
   public:
      GetAttackerImage( tfight& battle, Surface& surface, const SDL_Rect& dest ) : dst ( dest ), surf( surface )
      {
         battle.visit( *this );
      };
      
      void visit( tunitattacksobject& battle )  { paint(battle); };
      void visit( tunitattacksbuilding& battle )  { paint(battle); };
      void visit( tunitattacksunit& battle )  { paint(battle); };
      
      void visit( tmineattacksunit& battle )
      {
         Mine* m = battle.getFirstMine();
         if ( m )
            m->paint( surf, SPoint( dst.x, dst.y ) );
      }
};

class GetTargetImage : public FightVisitor {
   private:
      const SDL_Rect dst;
      Surface& surf;
   public:
      GetTargetImage( tfight& battle, Surface& surface, const SDL_Rect& dest ) : dst ( dest ), surf( surface )
      {
         battle.visit( *this );
      };

      
      void visit( tunitattacksobject& battle )
      {
         battle.getTarget()->typ->display( surf, SPoint( dst.x, dst.y ) );
      };
      
      void visit( tmineattacksunit& battle )
      {
         battle.getTarget()->typ->paint( surf, SPoint( dst.x, dst.y ), battle.getTarget()->getOwningPlayer().getPlayerColor() );
      }
      
      void visit( tunitattacksbuilding& battle )
      {
         battle.getTarget()->paintSingleField( surf, SPoint(dst.x, dst.y), battle.getTarget()->typ->entry );
      };
      
      void visit( tunitattacksunit& battle )
      {
         battle.getTarget()->typ->paint( surf, SPoint( dst.x, dst.y ), battle.getTarget()->getOwningPlayer().getPlayerColor() );
      };
};



class AttackPanel : public Panel {
       tfight& engine;
       int attacker_exp;
       int defender_exp;
   public:
      AttackPanel ( tfight& engine_ ) ;
      void setBarGraphValue( const ASCString& widgetName, float fraction ) { Panel::setBargraphValue( widgetName, fraction ); };
      void setLabelText ( const ASCString& widgetName, int i ) { Panel::setLabelText ( widgetName, i ); };
      void setLabelText ( const ASCString& widgetName, const ASCString& i ) { Panel::setLabelText ( widgetName, i ); };
      void dispValue ( const ASCString& name, float value, float maxvalue, PG_Color color );
      void setBarGraphColor( const ASCString& widgetName, PG_Color color ) { Panel::setBarGraphColor( widgetName, color ); };
      void painter ( const PG_Rect &src, const ASCString& name, const PG_Rect &dst);
   private:
      void registerSpecialDisplay( const ASCString& name );
};



AttackPanel::AttackPanel ( tfight& engine_ ) : Panel( PG_Application::GetWidgetById(ASC_PG_App::mainScreenID), PG_Rect(0,0,170,200), "Attack" ), engine( engine_ )
{
   registerSpecialDisplay( "attacker_unit_pic" );
   registerSpecialDisplay( "defender_unit_pic" );
   registerSpecialDisplay( "attacker_unitexp" );
   registerSpecialDisplay( "defender_unitexp" );
   registerSpecialDisplay( "attacker_level" );
   registerSpecialDisplay( "defender_level" );
   registerSpecialDisplay( "attacker_weaponsymbol" );
   registerSpecialDisplay( "defender_weaponsymbol" );
   attacker_exp = engine.av.experience;
   defender_exp = engine.dv.experience;
}


void AttackPanel::registerSpecialDisplay( const ASCString& name )
{
   SpecialDisplayWidget* sdw = dynamic_cast<SpecialDisplayWidget*>( FindChild( name, true ) );
   if ( sdw )
      sdw->display.connect( SigC::slot( *this, &AttackPanel::painter ));
}


void AttackPanel::painter ( const PG_Rect &src, const ASCString& name, const PG_Rect &dst)
{
   Surface s = Surface::Wrap( PG_Application::GetScreen() );

   if ( name  == "attacker_unit_pic" ) {
      GetAttackerImage( engine, s, dst );
      return;
   }
   if ( name  == "defender_unit_pic" ) {
      GetTargetImage( engine, s, dst );
      return;
   }
   if ( name  == "attacker_unitexp" ) {
      s.Blit( IconRepository::getIcon("experience" + ASCString::toString(attacker_exp) + ".png"), SPoint(dst.x, dst.y) );
      return;
   }
   if ( name  == "defender_unitexp" ) {
      s.Blit( IconRepository::getIcon("experience" + ASCString::toString(defender_exp) + ".png"), SPoint(dst.x, dst.y) );
      return;
   }
   if ( name  == "attacker_level" && engine.av.height ) {
      s.Blit( IconRepository::getIcon("height-a" + ASCString::toString(getFirstBit(engine.av.height)) + ".png"), SPoint(dst.x, dst.y) );
      return;
   }
   if ( name  == "defender_level" && engine.dv.height ) {
      s.Blit( IconRepository::getIcon("height-a" + ASCString::toString(getFirstBit(engine.dv.height)) + ".png"), SPoint(dst.x, dst.y) );
      return;
   }

   if ( name  == "attacker_weaponsymbol" && engine.av.weapontype >= 0  ) {
      s.Blit( IconRepository::getIcon(SingleWeapon::getIconFileName( engine.av.weapontype ) + "-small.png"), SPoint(dst.x, dst.y) );
      return;
   }
   if ( name  == "defender_weaponsymbol" && engine.dv.weapontype >= 0  ) {
      s.Blit( IconRepository::getIcon(SingleWeapon::getIconFileName( engine.dv.weapontype ) + "-small.png"), SPoint(dst.x, dst.y) );
      return;
   }

}


void AttackPanel::dispValue ( const ASCString& name, float value, float maxvalue, PG_Color color )
{
   if ( value > 0 ) {
      setBarGraphColor( name + "bar", color );
      if ( value > maxvalue )
         setBarGraphValue( name + "bar", 1 );
      else
         setBarGraphValue( name + "bar", value / maxvalue );
   } else {
      setBarGraphColor( name + "bar", PG_Colormap::yellow );
      if ( value < -maxvalue )
         setBarGraphValue( name + "bar", 1 );
      else
         setBarGraphValue( name + "bar", -value / maxvalue );
   }
   ASCString s;
   s.format ( "%d", int(value * 100) );
   setLabelText( name, s );
}



class BattleSoundPlayer : public FightVisitor {
   private:
      bool first;
      tfight& the_battle;
            
      void play( UnitAttacksSomething& battle )
      {
         SoundList::getInstance().playSound ( SoundList::shooting,
                                              battle.getAttackingUnit()->getWeapon(battle.av.weapnum)->getScalarWeaponType(),
                                              false,
                                              battle.getAttackingUnit()->getWeapon(battle.av.weapnum)->soundLabel );
      };
      
   public:
      BattleSoundPlayer( tfight& battle ) : first ( true ), the_battle(battle)
      {
         battle.visit( *this );
      };

      void playEnd()
      {
         first = false;
         the_battle.visit( *this );
      }
      
      void visit( tunitattacksobject& battle )
      {
         if ( first )
            play( battle );
      };
      
      void visit( tunitattacksbuilding& battle )
      {
         if ( first )
            play( battle );
         else
            if ( battle.dv.damage >= 100 )
               SoundList::getInstance().playSound( SoundList::buildingCollapses );
      };
      
      void visit( tunitattacksunit& battle )
      {
         if ( first )
            play( battle );
         else {
            if ( battle.av.damage >= 100  )
               SoundList::getInstance().playSound( SoundList::unitExplodes , 0, false, battle.getAttackingUnit()->typ->killSoundLabel );
            
            if ( battle.dv.damage >= 100 )
               SoundList::getInstance().playSound( SoundList::unitExplodes , 0, false, battle.getTarget()->typ->killSoundLabel );

         }
      };
      
      void visit( tmineattacksunit& battle )
      {
         if ( first )
            SoundList::getInstance().playSound( SoundList::shooting , 1 );
         else
            if ( battle.dv.damage >= 100 )
               SoundList::getInstance().playSound( SoundList::unitExplodes , 0, false, battle.getTarget()->typ->killSoundLabel );
      }
      
};

const int maxdefenseshown = 2;
const int maxattackshown = 2;

/**
  Waits the given time, but still updates the screen.
  These is of no use to the user, but are needed by the video recorder
*/
void waitWithUpdate( int millisecs ) 
{
   int frameDelay;
   if ( CGameOptions::Instance()->video.ascframeratelimit  > 0 )
      frameDelay = 100 / CGameOptions::Instance()->video.ascframeratelimit ;
   else
      frameDelay = 10;
   
   int t = ticker;
   do {
      int t2 = ticker;
      do {
         releasetimeslice();
      } while ( t2 + frameDelay > ticker );
      postScreenUpdate( PG_Application::GetScreen() );
   } while ( t + millisecs/20 > ticker ); 
}


void showAttackAnimation( tfight& battle, GameMap* actmap, int ad, int dd )
{

   BattleSoundPlayer bsp( battle );
   
   auto_ptr<AttackPanel> at ( new AttackPanel(battle));

   float avd = float( 100 - battle.av.damage )/100;
   float dvd = float( 100 - battle.dv.damage )/100;

   PG_Color attackingColor = lighten_Color( actmap->player[battle.getAttackingPlayer()].getColor(), 22 );
   PG_Color defendingColor = lighten_Color( actmap->player[battle.getDefendingPlayer()].getColor(), 22 );

   at->setBarGraphValue( "attacker_unitstatusbar", avd );
   at->setBarGraphColor( "attacker_unitstatusbar", attackingColor );

   at->setBarGraphValue( "defender_unitstatusbar", dvd );
   at->setBarGraphColor( "defender_unitstatusbar", defendingColor );

   at->setLabelText( "attacker_unitstatus", 100 - battle.av.damage );
   at->setLabelText( "defender_unitstatus", 100 - battle.dv.damage );



   at->setBarGraphValue( "attacker_hemmingbar", (battle.dv.hemming -1) / 1.4 );
   at->setBarGraphColor( "attacker_hemmingbar", attackingColor );
   at->setBarGraphValue( "defender_hemmingbar", (battle.av.hemming -1) / 1.4 );
   at->setBarGraphColor( "defender_hemmingbar", defendingColor );

   at->setLabelText( "defender_hemming", "-" );
   at->setLabelText( "attacker_hemming", int((battle.dv.hemming-1) * 100 ));

   at->dispValue( "attacker_attackbonus", battle.strength_attackbonus(battle.av.attackbonus), maxattackshown, attackingColor );
   at->dispValue( "defender_attackbonus", battle.strength_attackbonus(battle.dv.attackbonus), maxattackshown, defendingColor );


   at->dispValue( "attacker_defencebonus", battle.defense_defensebonus(battle.av.defensebonus), maxattackshown, attackingColor );
   at->dispValue( "defender_defencebonus", battle.defense_defensebonus(battle.dv.defensebonus), maxattackshown, defendingColor );


   battle.calc();
   at->Show();


   int time1 = CGameOptions::Instance()->attackspeed1;
   if ( time1 <= 0 )
      time1 = 30;

   int time2 = CGameOptions::Instance()->attackspeed2;
   if ( time2 <= 0 )
      time2 = 50;

   int time3 = CGameOptions::Instance()->attackspeed3;
   if ( time3 <= 0 )
      time3 = 30;

   waitWithUpdate( time1*10 );



   if ( ad != -1 )
      battle.av.damage = ad;

   if ( dd != -1 )
      battle.dv.damage = dd;


   float avd2 = float( 100 - battle.av.damage )/100;
   float dvd2 = float( 100 - battle.dv.damage )/100;

   int starttime = ticker;
   while ( ticker < starttime + time2 ) {
      float p = float(ticker - starttime ) / time2;

      at->setBarGraphValue( "attacker_unitstatusbar", avd + (avd2-avd) * p );
      at->setBarGraphValue( "defender_unitstatusbar", dvd + (dvd2-dvd) * p );

      at->setLabelText( "attacker_unitstatus", int( 100.0 * (avd + (avd2-avd) * p )) );
      at->setLabelText( "defender_unitstatus", int( 100.0 * (dvd + (dvd2-dvd) * p )) );
      at->Update();

      releasetimeslice();
   }

   at->setBarGraphValue( "attacker_unitstatusbar", avd2 );
   at->setBarGraphValue( "defender_unitstatusbar", dvd2 );
   at->Update();

   bsp.playEnd();
   
   waitWithUpdate( time3*10 );
}

