/***************************************************************************
                          base.cpp  -  description
                             -------------------
    begin                : Fri Mar 30 2001
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

#include "ai_common.h"


AI :: AI ( pmap _map, int _player ) : activemap ( _map ) , sections ( this )
{
   player = _player;

   _isRunning = false;
   fieldInformation = NULL;

   reset();
   ReplayMapDisplay* r = new ReplayMapDisplay ( &defaultMapDisplay );
   r->setCursorDelay (CGameOptions::Instance()->replayspeed + 30 );
   rmd = r;
   mapDisplay = rmd;
}

void AI :: reset ( void )
{
   maxTrooperMove = 0;
   maxTransportMove = 0;
   maxUnitMove = 0;

   for ( int i= 0; i < 8; i++ )
      maxWeapDist[i] = -1;
   baseThreatsCalculated = 0;

   if ( fieldInformation )
      delete[] fieldInformation;

   fieldInformation = NULL;
   fieldNum = 0;

   config.wholeMapVisible = 1;
   config.lookIntoTransports = 1;
   config.lookIntoBuildings = 1;
   config.aggressiveness  = 2;
   config.damageLimit = 70;
   config.resourceLimit = Resources ( 0, 5, 20 );
   config.ammoLimit= 10;
   config.maxCaptureTime = 15;
   config.maxTactTime = 10*100;
   config.waitForResourcePlus = 2;

   sections.reset();
}



void    AI :: setup (void)
{
   displaymessage2("calculating all threats ... ");
   calculateAllThreats ();

   displaymessage2("calculating field threats ... ");
   calculateFieldInformation();

   displaymessage2("calculating sections ... ");
   sections.calculate();

/*
   for ( i = 0; i <= 8; i++) {
      pbuilding building = actmap->player[i].firstbuilding;
      while (building != NULL) {
         generatethreatvaluebuilding(building);
         building = building->next;
      }
      if (i == actmap->actplayer) {
         building = actmap->player[i].firstbuilding;
         while (building != NULL) {
            tcmpcheckreconquerbuilding ccrcb;
            ccrcb.init(3);
            ccrcb.initsuche(building->xpos,building->ypos,(maxfusstruppenmove + maxtransportmove) / 8 + 1,0);
            ccrcb.startsuche();
            int j;
            ccrcb.returnresult( &j );
            ccrcb.done();
            building = building->next;
         }
      }
   }
   */

   /*
    punits units = new tunits;
    tjugdesituationspfd jugdesituationspfd;
    jugdesituationspfd.init(units,1);
    jugdesituationspfd.startsuche();
    jugdesituationspfd.done();
    delete units;
   */

   // showthreats("init: threatvals generated");
   displaymessage2("setup completed ... ");
}


void AI::checkKeys ( void )
{
   if ( keypress() ) {
      tkey k = r_key();
      if ( k == ct_esc ) {
         mapDisplay = NULL;
         tlockdispspfld a;
         repaintdisplay();
      }
   }
}


void AI:: run ( void )
{
   mapDisplay = rmd;
   int startTime = ticker;
   AiResult res;

   cursor.hide();

   unitCounter = 0;
   _isRunning = true;
   _vision = visible_ago;

   int setupTime = ticker;
   tempsvisible = false;
   setup();

   if ( !originalUnitDistribution.calculated )
      originalUnitDistribution = calcUnitDistribution();

   calcReconPositions();
   tempsvisible = true;
   setupTime = ticker-setupTime;

   int serviceTime = ticker;
   issueServices( );
   executeServices();
   serviceTime = ticker-serviceTime;

   int conquerTime = ticker;
   checkConquer();
   conquerTime = ticker - conquerTime;

   runReconUnits ( );

   int containerTime = ticker;
   buildings( 3 );
   transports ( 3 );
   containerTime = ticker-containerTime;

   int tacticsTime = ticker;
   do {
      res = tactics();
   } while ( res.unitsMoved );
   tacticsTime = ticker - tacticsTime;

   int strategyTime = ticker;
   strategy();
   strategyTime = ticker - strategyTime;

   buildings( 1 );
   transports ( 3 );

   production();

   _isRunning = false;
   if ( !mapDisplay )
      displaymap();
   int duration = ticker-startTime;

   if ( CGameOptions::Instance()->runAI == 2 )
      if ( duration > 100*60 )
         displaymessage ("The AI took %d seconds to run\n"
                         " setup: %d \n"
                         " service: %d \n"
                         " conquer: %d \n"
                         " container: %d \n"
                         " tactics: %d \n"
                         " strategy: %d \n",
           3, duration / 100,
              setupTime / 100,
              serviceTime/100,
              conquerTime /100,
              containerTime/100,
              tacticsTime/100,
              strategyTime/100 );

   displaymessage2("AI completed in %d second", duration/100);

   checkforvictory();   
}

const int currentAiStreamVersion = 102;

void AI :: read ( tnstream& stream )
{
   int version = stream.readInt ( );
   if ( version > currentServiceOrderVersion )
      throw tinvalidversion ( "AI :: read", currentServiceOrderVersion, version );
   _isRunning = stream.readInt ();
   _vision = stream.readInt ( );
   unitCounter = stream.readInt ( );

   int i = stream.readInt();
   while ( i ) {
      ServiceOrder so ( this, stream );
      serviceOrders.push_back ( so );
      i = stream.readInt();
   }

   for_each ( serviceOrders.begin(), serviceOrders.end(), ServiceOrder::activate );

   i = stream.readInt();
   while ( i ) {
      MapCoordinate mc;
      mc.read ( stream );

      AI::BuildingCapture bc;
      bc.read ( stream );

      buildingCapture[mc] = bc;

      i = stream.readInt();
   }

   config.lookIntoTransports = stream.readInt();
   config.lookIntoBuildings = stream.readInt( );
   config.wholeMapVisible = stream.readInt( );
   config.aggressiveness = stream.readFloat( );
   config.damageLimit = stream.readInt();
   config.resourceLimit.read( stream );
   config.ammoLimit = stream.readInt();
   config.maxCaptureTime = stream.readInt();
   if ( version >= 102 )
      config.waitForResourcePlus = stream.readInt();

   if ( version >= 101 )
      config.maxTactTime = stream.readInt();

   if ( version >= 102 )
      originalUnitDistribution.read ( stream );

   int version2 = stream.readInt();
   if ( version != version2 )
      throw tinvalidversion ( "AI :: read", version, version2 );


}

void AI :: write ( tnstream& stream ) const
{
   const int version = currentAiStreamVersion;
   stream.writeInt ( version );
   stream.writeInt ( _isRunning );
   stream.writeInt ( _vision );
   stream.writeInt ( unitCounter );

   for ( ServiceOrderContainer::const_iterator i = serviceOrders.begin(); i != serviceOrders.end(); i++) {
      stream.writeInt ( 1 );
      i->write ( stream );
   }

   stream.writeInt ( 0 );

   for ( map<MapCoordinate,BuildingCapture>::const_iterator i = buildingCapture.begin(); i != buildingCapture.end(); i++ ) {
      stream.writeInt ( 1 );
      i->first.write ( stream );
      i->second.write ( stream );
   }
   stream.writeInt ( 0 );

   stream.writeInt( config.lookIntoTransports );   /*  gegnerische transporter einsehen  */
   stream.writeInt( config.lookIntoBuildings );
   stream.writeInt( config.wholeMapVisible );
   stream.writeFloat( config.aggressiveness );   // 1: units are equally worth ; 2
   stream.writeInt( config.damageLimit );
   config.resourceLimit.write( stream );
   stream.writeInt( config.ammoLimit );
   stream.writeInt( config.maxCaptureTime );
   stream.writeInt( config.maxTactTime );
   stream.writeInt( config.waitForResourcePlus );

   originalUnitDistribution.write( stream );
   stream.writeInt ( version );
}


bool AI :: isRunning ( void )
{
   return _isRunning;
}


int AI :: getVision ( void )
{
   return _vision;
}

void AI :: showFieldInformation ( int x, int y )
{
   if ( !fieldInformation )
      calculateFieldInformation();

   const char* fieldinfo = "#font02#Field Information (%d,%d)#font01##aeinzug20##eeinzug10##crtp10#"
                           "threat orbit: %d\n"
                           "threat high-level flight: %d\n"
                           "threat flight: %d\n"
                           "threat low-level flight: %d\n"
                           "threat ground level: %d\n"
                           "threat floating: %d\n"
                           "threat submerged: %d\n"
                           "threat deep submerged: %d\n"
                           "controlled by %d\n";

   char text[10000];
   AiThreat& threat = getFieldThreat ( x, y );
   sprintf(text, fieldinfo, x,y,threat.threat[7], threat.threat[6], threat.threat[5],
                                threat.threat[4], threat.threat[3], threat.threat[2],
                                threat.threat[1], threat.threat[0], getFieldInformation(x,y).control );

   pfield fld = getfield (x, y );
   if ( fld->vehicle && fieldvisiblenow ( fld ) && fld->vehicle->aiparam[getPlayerNum()] ) {
      char text2[1000];
      sprintf(text2, "\nunit nwid: %d ; typeid: %d", fld->vehicle->networkid, fld->vehicle->typ->id );
      strcat ( text, text2 );
      AiParameter& aip = *fld->vehicle->aiparam[getPlayerNum()];

      if ( fld->vehicle->aiparam ) {
         static char* tasks[] = { "nothing",
                                  "tactics",
                                  "tactwait",
                                  "stratwait",
                                  "wait",
                                  "strategy",
                                  "serviceRetreat",
                                  "move" };
        static char* jobs[] = { "undefined",
                                "fight",
                                "supply",
                                "conquer",
                                "build",
                                "recon",
                                "guard" };

         sprintf(text2, "\nunit value: %d; xtogo: %d, ytogo: %d; ztogo: %d;\njob %s ; task %s \n", aip.getValue(), aip.dest.x, aip.dest.y, aip.dest.z, jobs[aip.job], tasks[aip.task] );
         strcat ( text, text2 );
      }

      if ( aip.dest.x >= 0 && aip.dest.y >= 0 ) {
         getMap()->cleartemps ( 1 );
         getfield ( aip.dest.x, aip.dest.y )->a.temp = 1;
      }


   }

   for ( ReconPositions::iterator i = reconPositions.begin(); i != reconPositions.end(); i++ )
      getMap()->getField( i->first )->a.temp2 = 1;

   displaymap();


   strcat ( text, "\n#font02#Section Information#font01##aeinzug20##eeinzug10##crtp10#");
   string s;

   Section& sec = sections.getForCoordinate ( x, y );

   s += "xp = ";
   s += strrr ( sec.xp );
   s += " ; yp = ";
   s += strrr ( sec.yp );
   s += "\n";
   const char* threattypes[4] = { "absUnitThreat", "avgUnitThreat", "absFieldThreat", "avgFieldThreat" };

   for ( int i = 0; i < 4; i++ ) {
      for ( int h = 0; h < 8; h++ ) {
         s += threattypes[i];
         s += " ";
         s += choehenstufen [h];
         s += " ";
         switch ( i ) {
            case 0: s += strrr ( sec.absUnitThreat.threat[h] );
                    break;
            case 1: s += strrr ( sec.avgUnitThreat.threat[h] );
                    break;
            case 2: s += strrr ( sec.absFieldThreat.threat[h] );
                    break;
            case 3: s += strrr ( sec.avgFieldThreat.threat[h] );
                    break;
         }
         s += "\n";
      }
      s += "\n";
   }

   for ( int j = 0; j < aiValueTypeNum; j++ ) {
      s+= "\nvalue ";
      s+= strrr ( j );
      s+= " = ";
      s+= strrr ( sec.value[j] );
   }

   strcat ( text, s.c_str() );
   tviewanytext vat;
   vat.init ( "AI information", text );
   vat.run();
   vat.done();
}


AI :: ~AI ( )
{
   if ( fieldInformation ) {
      delete[] fieldInformation;
      fieldInformation = NULL;
      fieldNum = 0;
   }
}


