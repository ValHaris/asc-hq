//     $Id: Unit.java,v 1.2 2000-10-13 13:15:47 schelli Exp $
//
//     $Log: not supported by cvs2svn $

/*
 * Unit.java
 *
 * Created on 21. November 1999, 15:03

This file is part of Advanced Strategic Command; http://www.asc-hq.de
Copyright (C) 1994-2000  Martin Bickel  and  Marc Schellenberger

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


import UnitEditor.*;

public class Unit {   /*  vehicleart: z.B. Schwere Fuátruppe  */

    public static int NEW = 0;
    public static int LOAD = 1;

    int       version;
    String    name;          /* z.B. Exterminator  */
    String    description;   /* z.B. Jagdbomber    */
    String    infotext;      /* optional, kann sehr ausf?hrlich sein. Empfehlenswert ?ber eine Textdatei einzulesen */
    Weapons   attack;        /*  Angriffsst„rke der einzelnen Waffen  */
    PK        production;    /*  Produktionskosten der vehicle  */
    int       armor;    //w
    int       picture[];    /*8  0ø  ,  45ø   */
    int       height;        /*  BM  Besteht die M”glichkeit zum H”henwechseln  */
    int       researchid;    //w inzwischen ?berfl?ssig, oder ?
    int       _terrain;    /*  BM     befahrbare terrain: z.B. Schiene, Wasser, Wald, ...  */
    int       _terrainreq; /*  BM     diese Bits MšSSEN in ( field.typ.art & terrain ) gesetzt sein */
    int       _terrainkill;  /* falls das aktuelle field nicht befahrbar ist, und bei field.typ.art eine dieser Bits gesetzt ist, verschwindet die vehicle */
    int       steigung;      /*  max. befahrbare H”hendifferenz zwischen 2 fieldern  */
    int       jamming;      /*  St„rke der St”rstrahlen  */
    int       view;         /*w  viewweite  */
    int       wait;        /*c  Kann vehicle nach movement sofort schieáen ?  */
    int       loadcapacity;      /*w  Transportm”glichkeiten  */
    int       maxunitweight; /*w  maximales Gewicht einer zu ladenden vehicle */
    int       loadcapability;     /*c  BM     CHoehenStufen   die zu ladende vehicle muá sich auf einer dieser H”henstufen befinden */
    int       loadcapabilityreq;  /*c  eine vehicle, die geladen werden soll, muá auf eine diese H”henstufen kommen k”nnen */
    int       loadcapabilitynot;  /*c  eine vehicle, die auf eine dieser H”henstufen kann, darf NICHT geladen werden. Beispiel: Flugzeuge in Transportflieger */
    int       id;     //w
    int       tank;
    int       fuelconsumption; //w
    int       energy;
    int       material;
    int       functions;
    int       movement[];      /*8c  max. movementsstrecke  */
    int       movemalustyp;     /*c  wenn ein Bodentyp mehrere Movemali f?r unterschiedliche vehiclearten, wird dieser genommen.  <= cmovemalitypes */
    int       generalthreatvalue;   /*w  Wird von ArtInt ben”tigt, auáerhalb keine Bedeutung  */
    int       threatvalue[];       /*8w  dito                                                 */
    int       classnum;         /*c Anzahl der Klassen, max 8, min 0 ;  Der EINZIGE Unterschied zwischen 0 und 1 ist der NAME ! */
    String    classnames[];    /*8 Name der einzelnen Klassen */
    ClassBound  classbound[];    /* untergrenze (minimum), die zum erreichen dieser Klasse notwendig ist, classbound[0] gilt f?r vehicletype allgemein*/
    int       maxwindspeedonwater;//c
    int       digrange;        //c Radius, um den nach bodensch„tzen gesucht wird.
    int       initiative;      // 0 ist ausgeglichen // 256 ist verdoppelung
    int       _terrainnot;    /*  BM     sobald eines dieser Bits gesetzt ist, kann die vehicle NICHT auf das field fahren  */
    int       _terrainreq1;  // wie terrainreq, es braucht aber nur 1 bit gesetzt zu sein
    int       objectsbuildablenum;
    int       objectsbuildableid[]; //int*
    int       weight;           // basic weight, without fuel etc.
    int       terrainaccess; //pterrainaccess
    int       bipicture;
    int       vehiclesbuildablenum;
    int       vehiclesbuildableid[]; //int*
    int       buildicon; //void*
    int       buildingsbuildablenum;
    int       buildingsbuildable;//pbuildrange
    UnitWeapon weapons; //UnitWeapon*
    int       autorepairrate;

    int       vehicleCategoriesLoadable;

    byte      terrainAccessDummy[];
    byte      buildingBuildableDummy[];

    private String fileLocation;
    private SgStream stream;
    private RLEPackage picPackage[];


    public Unit(String s) {
        fileLocation = s;
    }

    public void makeNew() {
    }

    public int load () {

        stream = new SgStream(fileLocation,SgStream.STREAM_READ);

        if (stream.error != 0) return stream.error;

        version = stream.readInt();

        if ( version <= vehicle_version && version >= 1) {

            if (stream.readInt() != 0) name = "Load";
            if (stream.readInt() != 0) description = "Load";
            if (stream.readInt() != 0) infotext = "Load";

            if ( version <= 2 ) {
                attack = new Weapons();
                attack.weaponcount = stream.readChar();
                for (int j = 0;j < 8;j++) {
                    attack.weapon[j] = new Weapon();
                    attack.weapon[j].typ = stream.readWord();
                    attack.weapon[j].targ = stream.readChar();
                    attack.weapon[j].sourceheight = stream.readChar();
                    attack.weapon[j].maxdistance = stream.readWord();
                    attack.weapon[j].mindistance = stream.readWord();
                    attack.weapon[j].count = stream.readChar();
                    attack.weapon[j].maxstrength = stream.readChar();
                    attack.weapon[j].minstrength = stream.readChar();
                }
            }

            production = new PK();
            production.energy = stream.readWord();
            production.material = stream.readWord();

            armor = stream.readWord();

            picture = new int[8];
            for (int j=0;j < 8;j++) picture[j] = stream.readInt();

            height = stream.readChar();
            researchid = stream.readWord();
            if ( version <= 2 ) {
                _terrain = stream.readInt();
                _terrainreq = stream.readInt();
                _terrainkill = stream.readInt();
            }
            steigung = stream.readChar();
            jamming = stream.readChar();
            view = stream.readWord();
            wait = stream.readChar();

            if ( version <= 2 ) stream.readChar(); //dummy

            loadcapacity = stream.readWord();
            maxunitweight = stream.readWord();
            loadcapability = stream.readChar();
            loadcapabilityreq = stream.readChar();
            loadcapabilitynot = stream.readChar();
            id = stream.readWord();
            tank = stream.readInt();
            fuelconsumption = stream.readWord();
            energy = stream.readInt();
            material = stream.readInt();
            functions = stream.readInt();

            movement = new int[8];
            for (int j=0;j < 8;j++) movement[j] = stream.readChar();

            movemalustyp = stream.readChar();

            if ( version <= 2 ) {
                generalthreatvalue = stream.readWord();
                threatvalue = new int[8];
                for (int j=0;j < 8;j++) threatvalue[j] = stream.readWord();
            }

            classnum = stream.readChar();
            classnames = new String[8];
            for (int j=0;j < 8;j++) if (stream.readInt() != 0) classnames[j] = "Load";
            classbound = new ClassBound[8];
            for (int j=0;j < 8;j++) {
                classbound[j] = new ClassBound();
                for (int k=0;k < 8;k++) {
                    classbound[j].weapstrength[k] = stream.readWord();
                }

                if ( version <= 2 )  stream.readWord(); //dummy

                classbound[j].armor = stream.readWord();
                classbound[j].techlevel = stream.readWord();
                for (int k=0;k < 4;k++) classbound[j].techrequired[k] = stream.readWord();
                classbound[j].eventrequired = stream.readChar();
                classbound[j].vehiclefunctions = stream.readInt();
                if ( version <= 2 ) stream.readChar(); //dummy
            }

            maxwindspeedonwater = stream.readChar();
            digrange = stream.readChar();
            initiative = stream.readInt();
            _terrainnot = stream.readInt();
            _terrainreq1 = stream.readInt();
            objectsbuildablenum = stream.readInt();

            /*objectsbuildableid*/ stream.readInt();
            // fuer objectsbuildableid = stream.readInt(); , weil geht nicht wegen int[]
            // und objectsbuildableid wird nicht gebraucht

            weight = stream.readInt();
            terrainaccess = stream.readInt();
            bipicture = stream.readInt();
            vehiclesbuildablenum = stream.readInt();

            /*vaehiclesbuildableid*/ stream.readInt();
            // fuer vaehiclesbuildableid = stream.readInt(); , weil geht nicht wegen int[]
            // und vaehiclesbuildableid wird nicht gebraucht

            buildicon = stream.readInt();
            buildingsbuildablenum = stream.readInt();
            buildingsbuildable = stream.readInt();

            /*weapons*/ stream.readInt();
            // fuer weapons = stream.readInt(); , weil geht nicht wegen UnitWeapon
            // und weapons wird nicht gebraucht
            autorepairrate = stream.readInt();

            if ( version <= 2 ) stream.readInt(); //dummy

            if ( version >= 4 ) {
                vehicleCategoriesLoadable = stream.readInt();
            } else {
                vehicleCategoriesLoadable = -1;
            }

            if (name != null)
            if (name.compareTo("Load") == 0)
            name = stream.readPChar();
            if (description != null)
            if (description.compareTo("Load") == 0)
            description = stream.readPChar();
            if (infotext != null)
            if (infotext.compareTo("Load") == 0)
            infotext = stream.readPChar();
            for (int j=0; j<8;j++)
            if (classnames[j] != null)
            if (classnames[j].compareTo("Load") == 0)
            classnames[j] = stream.readPChar();

            if ( (functions & UnitEditor.cfautorepair) != 0 )
            if ( autorepairrate == 0 )
            autorepairrate = UnitEditor.autorepairdamagedecrease; // which is 10

            picPackage = new RLEPackage[8];
            for (int j=0; j<8;j++)
            if ( picture[j] != 0)
            if (bipicture <= 0) picPackage[j] = stream.readRlePicture();
            //else stream.readBi3Picture();
            if ( objectsbuildablenum != 0) {
                objectsbuildableid = new int [ objectsbuildablenum ];
                for ( int j = 0; j < objectsbuildablenum; j++ )
                objectsbuildableid[j] = stream.readInt();
            }
            if ( vehiclesbuildablenum != 0) {
                vehiclesbuildableid = new int [ vehiclesbuildablenum ];
                for ( int j = 0; j < vehiclesbuildablenum; j++ )
                vehiclesbuildableid[j] = stream.readInt();
            }
            weapons = new UnitWeapon();
            if ( (weapons != null)&& (version > 1) ) {
                weapons.count = stream.readInt();
                weapons.weapon = new SingleWeapon[16];
                for (int j = 0; j < 16; j++ ) {
                    weapons.weapon[j] = new SingleWeapon();
                    weapons.weapon[j].typ = stream.readInt();
                    weapons.weapon[j].targ = stream.readInt();
                    weapons.weapon[j].sourceHeight = stream.readInt();
                    weapons.weapon[j].maxDistance = stream.readInt();
                    weapons.weapon[j].minDistance = stream.readInt();
                    weapons.weapon[j].count = stream.readInt();
                    weapons.weapon[j].maxStrength = stream.readInt();
                    weapons.weapon[j].minStrength = stream.readInt();

                    weapons.weapon[j].efficiency = new int[13];
                    for (int k = 0; k < 13; k++ )
                    weapons.weapon[j].efficiency[k] = stream.readInt();

                    weapons.weapon[j].targets_not_hittable = stream.readInt();

                    if ( version <= 2 )
                    for (int k = 0; k < 9; k++ )
                    stream.readInt(); //dummy
                }
                if ( version <= 2 )
                for (int k = 0; k < 10; k++ )
                stream.readInt();
            }
            else {
                weapons.count = attack.weaponcount;
                for (int j = 0; j < attack.weaponcount; j++ ) {
                    weapons.weapon[j].typ  = attack.weapon[j].typ;
                    weapons.weapon[j].targ  = attack.weapon[j].targ;
                    weapons.weapon[j].sourceHeight  = attack.weapon[j].sourceheight;
                    weapons.weapon[j].maxDistance  = attack.weapon[j].maxdistance;
                    weapons.weapon[j].minDistance  = attack.weapon[j].mindistance;
                    weapons.weapon[j].count  = attack.weapon[j].count;
                    weapons.weapon[j].maxStrength  = attack.weapon[j].maxstrength;
                    weapons.weapon[j].minStrength  = attack.weapon[j].minstrength;
                    for ( int k = 0; k < 13; k++ )
                    weapons.weapon[j].efficiency[k] = 100;
                }
            }

            if ( terrainaccess != 0) {
                terrainAccessDummy = stream.readByteArray(72);
            }
            /*else {
            fztn->terrainaccess = new tterrainaccess;
            fztn->terrainaccess->terrain.set ( fztn->_terrain, 0 );
            fztn->terrainaccess->terrainreq.set ( fztn->_terrainreq, 0 );
            fztn->terrainaccess->terrainnot.set ( fztn->_terrainnot, 0 );
            fztn->terrainaccess->terrainkill.set ( fztn->_terrainkill, 0 );
            }*/
            /*
            if ( terrainaccess ) {
            terrainaccess = new tterrainaccess;
            stream.readdata2 ( *(terrainaccess) );
            } else {
            terrainaccess = new tterrainaccess;
            terrainaccess.terrain.set ( _terrain, 0 );
            terrainaccess.terrainreq.set ( _terrainreq, 0 );
            terrainaccess.terrainnot.set ( _terrainnot, 0 );
            terrainaccess.terrainkill.set ( _terrainkill, 0 );
            }

            if ( buildingsbuildablenum ) {
            buildingsbuildable = new tbuildrange[buildingsbuildablenum];
            for ( i = 0; i < buildingsbuildablenum; i++ )
            stream.readdata2( buildingsbuildable[i] );
            }*/

            if ( buildingsbuildablenum != 0 ) {
                buildingBuildableDummy = stream.readByteArray(buildingsbuildablenum*8);
            }

            return 0;
        } else return 1;

    }

    public int write () {

        int  one  = 1;
        int  zero = 0;

        stream = new SgStream(fileLocation,stream.STREAM_WRITE);

        if (stream.error != 0) return stream.error;

        stream.writeInt(vehicle_version);

        if ( name != null ) stream.writeInt( one );
        else stream.writeInt( zero );

        if ( description != null ) stream.writeInt( one );
        else stream.writeInt( zero );

        if ( infotext != null ) stream.writeInt( one );
        else stream.writeInt( zero );

        stream.writeWord( production.energy );
        stream.writeWord( production.material );
        stream.writeWord( armor );

        for ( int j = 0; j < 8; j++ )
        if ( picture[j] != 0) stream.writeInt( one );
        else stream.writeInt( zero );

        stream.writeChar( height );
        stream.writeWord(researchid);

        stream.writeChar(steigung);
        stream.writeChar(jamming);
        stream.writeWord(view);
        stream.writeChar(wait);

        stream.writeWord(loadcapacity);
        stream.writeWord(maxunitweight);
        stream.writeChar(loadcapability);
        stream.writeChar(loadcapabilityreq);
        stream.writeChar(loadcapabilitynot);

        stream.writeWord(id );
        stream.writeInt(tank );
        stream.writeWord(fuelconsumption );
        stream.writeInt(energy );
        stream.writeInt(material );
        stream.writeInt(functions );
        for ( int j = 0; j < 8; j++ ) stream.writeChar( movement[j] );

        stream.writeChar(movemalustyp );
        stream.writeChar(classnum );
        for ( int j = 0; j < 8; j++ )
        if ( classnames[j] != null ) stream.writeInt( one );
        else stream.writeInt( zero );

        for ( int j = 0; j < 8; j++ ) {
            for ( int k = 0; k < 8; k++ ) {
                stream.writeWord(classbound[j].weapstrength[k]);
            }
            stream.writeWord(classbound[j].armor);
            stream.writeWord(classbound[j].techlevel );
            for ( int k = 0; k < 4; k++ )
            stream.writeWord(classbound[j].techrequired[k]);

            stream.writeChar(classbound[j].eventrequired );
            stream.writeInt(classbound[j].vehiclefunctions );
        }

        stream.writeChar(maxwindspeedonwater );
        stream.writeChar(digrange );
        stream.writeInt(initiative );
        stream.writeInt(_terrainnot );
        stream.writeInt(_terrainreq1 );
        stream.writeInt(objectsbuildablenum );

        if ( objectsbuildableid != null) stream.writeInt( one );
        else stream.writeInt( zero );

        stream.writeInt(weight );

        if ( terrainaccess != 0) stream.writeInt( one );
        else stream.writeInt( zero );

        stream.writeInt(bipicture);
        stream.writeInt(vehiclesbuildablenum );

        if ( vehiclesbuildableid != null ) stream.writeInt( one );
        else stream.writeInt( zero );

        if ( buildicon != 0) stream.writeInt( one );
        else stream.writeInt( zero );

        stream.writeInt(buildingsbuildablenum );

        if ( buildingsbuildable != 0) stream.writeInt( one );
        else stream.writeInt( zero );

        if ( weapons != null) stream.writeInt( one );
        else stream.writeInt( zero );

        stream.writeInt( autorepairrate );
        stream.writeInt( vehicleCategoriesLoadable );

        if (name != null) stream.writePChar( name );

        if (description != null) stream.writePChar( description );

        if (infotext != null) stream.writePChar( infotext );

        for (int i=0; i<8; i++)
        if ( classnames[i] != null)
        stream.writePChar( classnames[i] );

        if ( bipicture <= 0 )
        for (int i=0;i<8  ;i++ )
        if ( picture[i] != 0)
        stream.writeRlePicture(picPackage[i]);

        for ( int i = 0; i < objectsbuildablenum; i++ )
        stream.writeInt ( objectsbuildableid[i] );

        for ( int i = 0; i < vehiclesbuildablenum; i++ )
        stream.writeInt ( vehiclesbuildableid[i] );

        stream.writeInt(weapons.count );
        for ( int j = 0; j< 16; j++ ) {
            stream.writeInt(weapons.weapon[j].typ);
            stream.writeInt(weapons.weapon[j].targ);
            stream.writeInt(weapons.weapon[j].sourceHeight );
            stream.writeInt(weapons.weapon[j].maxDistance );
            stream.writeInt(weapons.weapon[j].minDistance );
            stream.writeInt(weapons.weapon[j].count );
            stream.writeInt(weapons.weapon[j].maxStrength );
            stream.writeInt(weapons.weapon[j].minStrength );

            for ( int k = 0; k < 13; k++ )
            stream.writeInt(weapons.weapon[j].efficiency[k] );

            stream.writeInt(weapons.weapon[j].targets_not_hittable );
        }

        /*if ( terrainaccess )
        stream.writeInt ( *(terrainaccess) );

        for ( i = 0; i < buildingsbuildablenum; i++ ) {
        stream.writeInt( buildingsbuildable[i].from );
        stream.writeInt( buildingsbuildable[i].to );
        }*/
        if ( terrainaccess != 0) {
            stream.writeByteArray(terrainAccessDummy);
        }

        if ( buildingsbuildablenum != 0 ) {
            stream.writeByteArray(buildingBuildableDummy);
        }

        return 0;
    }

    static int vehicle_version = 4;
};
