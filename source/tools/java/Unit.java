/*
 * Unit.java
 *
 * Created on 21. November 1999, 15:03
 */
 


/** 
 *
 * @author  MS
 * @version 
 */

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
                    int       _terrainreq; /*  BM     diese Bits MšSSEN in ( field->typ->art & terrain ) gesetzt sein */
                    int       _terrainkill;  /* falls das aktuelle field nicht befahrbar ist, und bei field->typ->art eine dieser Bits gesetzt ist, verschwindet die vehicle */
                    int       steigung;      /*  max. befahrbare H”hendifferenz zwischen 2 fieldern  */
                    int       jamming;      /*  St„rke der St”rstrahlen  */
                    int       view;         /*w  viewweite  */
                    int       wait;        /*c  Kann vehicle nach movement sofort schieáen ?  */
                    int       dummy2; //c
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
                    int       pterrainaccess; //pterrainaccess
                    int       bipicture;
                    int       vehiclesbuildablenum;
                    int       vehiclesbuildableid[]; //int*
                    int       buildicon; //void*
                    int       buildingsbuildablenum;
                    int       buildingsbuildable;//pbuildrange
                    UnitWeapon weapons; //UnitWeapon*
                    int       dummy[];//2

                    String fileLocation;
                    private SgStream tSgStream;
                    int intLoadDummy;
                  
                                      
                  public Unit(String s) {
                    fileLocation = s;
                  }  
                  
                  public void makeNew() {
                  }
                  
                  
                  
                  public int load () {

                    tSgStream = new SgStream(fileLocation,SgStream.STREAM_READ);
                    
                    if (tSgStream.error != 0) return tSgStream.error;
                    
                    version = tSgStream.readInt();
                    
                    if (tSgStream.readInt() != 0) name = "Load";
                    if (tSgStream.readInt() != 0) description = "Load";
                    if (tSgStream.readInt() != 0) infotext = "Load";
                    
                    
                    attack = new Weapons();
                    attack.weaponcount = tSgStream.readByte();
                    for (int j = 0;j < 8;j++) {
                      attack.weapon[j] = new Weapon();
                      attack.weapon[j].typ = tSgStream.readWord();
                      attack.weapon[j].targ = tSgStream.readByte();
                      attack.weapon[j].sourceheight = tSgStream.readByte();
                      attack.weapon[j].maxdistance = tSgStream.readWord();
                      attack.weapon[j].mindistance = tSgStream.readWord();
                      attack.weapon[j].count = tSgStream.readByte();
                      attack.weapon[j].maxstrength = tSgStream.readByte();
                      attack.weapon[j].minstrength = tSgStream.readByte();
                    }
                    
                    production = new PK();
                    production.energy = tSgStream.readWord();
                    production.material = tSgStream.readWord();
                                                           
                    armor = tSgStream.readWord();
                    
                    picture = new int[8];
                    for (int j=0;j < 8;j++) picture[j] = tSgStream.readInt();
                    
                    height = tSgStream.readByte();
                    researchid = tSgStream.readWord();
                    _terrain = tSgStream.readInt();
                    _terrainreq = tSgStream.readInt();
                    _terrainkill = tSgStream.readInt();
                    steigung = tSgStream.readByte();
                    jamming = tSgStream.readByte();
                    view = tSgStream.readWord();
                    wait = tSgStream.readByte();
                    dummy2 = tSgStream.readByte();
                    loadcapacity = tSgStream.readWord();
                    maxunitweight = tSgStream.readWord();
                    loadcapability = tSgStream.readByte();
                    loadcapabilityreq = tSgStream.readByte();
                    loadcapabilitynot = tSgStream.readByte();
                    id = tSgStream.readWord();
                    tank = tSgStream.readInt();
                    fuelconsumption = tSgStream.readWord();
                    energy = tSgStream.readInt();
                    material = tSgStream.readInt();
                    functions = tSgStream.readInt();
                    movement = new int[8];
                    for (int j=0;j < 8;j++) movement[j] = tSgStream.readByte();
                    movemalustyp = tSgStream.readByte();
                    generalthreatvalue = tSgStream.readWord();
                    threatvalue = new int[8];
                    for (int j=0;j < 8;j++) threatvalue[j] = tSgStream.readWord();
                    classnum = tSgStream.readByte();
                    classnames = new String[8];
                    for (int j=0;j < 8;j++) if (tSgStream.readInt() != 0) classnames[j] = "Load";
                    classbound = new ClassBound[8];
                    for (int j=0;j < 8;j++) {
                      classbound[j] = new ClassBound();
                      for (int k=0;k < classbound[j].weaponcount;k++)  
                         classbound[j].weapstrength[k] = tSgStream.readWord();
                      classbound[j].dummy2 = tSgStream.readWord();
                      classbound[j].armor = tSgStream.readWord();
                      classbound[j].techlevel = tSgStream.readWord();
                      for (int k=0;k < 4;k++) classbound[j].techrequired[k] = tSgStream.readWord();
                      classbound[j].eventrequired = tSgStream.readByte();
                      classbound[j].vehiclefunctions = tSgStream.readInt();
                      classbound[j].dummy = tSgStream.readByte();
                    }
                    
                    maxwindspeedonwater = tSgStream.readByte();
                    digrange = tSgStream.readByte();
                    initiative = tSgStream.readInt();
                    _terrainnot = tSgStream.readInt();
                    _terrainreq1 = tSgStream.readInt();
                    objectsbuildablenum = tSgStream.readInt();                    
                    intLoadDummy /*objectsbuildableid*/ = tSgStream.readInt(); 
                    // fuer objectsbuildableid = tSgStream.readInt(); , weil geht nicht wegen int[]
                    // und objectsbuildableid wird nicht gebraucht
                    weight = tSgStream.readInt();
                    pterrainaccess = tSgStream.readInt();
                    bipicture = tSgStream.readInt();
                    vehiclesbuildablenum = tSgStream.readInt();                    
                    intLoadDummy /*vaehiclesbuildableid*/ = tSgStream.readInt(); 
                    // fuer vaehiclesbuildableid = tSgStream.readInt(); , weil geht nicht wegen int[]
                    // und vaehiclesbuildableid wird nicht gebraucht
                    buildicon = tSgStream.readInt();
                    buildingsbuildablenum = tSgStream.readInt();
                    buildingsbuildable = tSgStream.readInt();                    
                    intLoadDummy /*weapons*/ = tSgStream.readInt(); 
                    // fuer weapons = tSgStream.readInt(); , weil geht nicht wegen UnitWeapon
                    // und weapons wird nicht gebraucht
                    dummy = new int[2];
                    for (int j=0;j < 2;j++) dummy[j] = tSgStream.readInt();
                    
                    if (name != null)
                       if (name.compareTo("Load") == 0) 
                          name = tSgStream.readPChar();
                    if (description != null)
                       if (description.compareTo("Load") == 0) 
                          description = tSgStream.readPChar();
                    if (infotext != null)
                       if (infotext.compareTo("Load") == 0)
                          infotext = tSgStream.readPChar();
                    for (int j=0; j<8;j++) 
                       if (classnames[j] != null)
                          if (classnames[j].compareTo("Load") == 0)
                             classnames[j] = tSgStream.readPChar();
                    for (int j=0; j<8;j++) 
                      if ( picture[j] != 0) 
                        if (bipicture <= 0) tSgStream.readRlePicture();                        
                       //else tSgStream.readBi3Picture();
                    if ( objectsbuildablenum != 0) {
                       objectsbuildableid = new int [ objectsbuildablenum ];
                       for ( int j = 0; j < objectsbuildablenum; j++ ) 
                           objectsbuildableid[j] = tSgStream.readInt();
                    }
                    if ( vehiclesbuildablenum != 0) {
                       vehiclesbuildableid = new int [ vehiclesbuildablenum ];
                       for ( int j = 0; j < vehiclesbuildablenum; j++ ) 
                           vehiclesbuildableid[j] = tSgStream.readInt();                      
                    }                      
                    weapons = new UnitWeapon();
                    if ( (weapons != null)&& (version > 1) ) { 
                      weapons.count = tSgStream.readInt();
                      weapons.weapon = new SingleWeapon[16];
                      for (int j = 0; j < 16; j++ ) {                        
                        weapons.weapon[j] = new SingleWeapon();
                        weapons.weapon[j].typ = tSgStream.readInt();        
                        weapons.weapon[j].targ = tSgStream.readInt();         
                        weapons.weapon[j].sourceHeight = tSgStream.readInt();
                        weapons.weapon[j].maxDistance = tSgStream.readInt();               
                        weapons.weapon[j].minDistance = tSgStream.readInt();               
                        weapons.weapon[j].count = tSgStream.readInt();               
                        weapons.weapon[j].maxStrength = tSgStream.readInt();                  
                        weapons.weapon[j].minStrength = tSgStream.readInt();
                        weapons.weapon[j].efficiency = new int[13];
                        for (int k = 0; k < 13; k++ ) 
                          weapons.weapon[j].efficiency[k] = tSgStream.readInt();               
                        weapons.weapon[j].reserved = new int[10];
                        for (int k = 0; k < 10; k++ ) 
                          weapons.weapon[j].reserved[k] = tSgStream.readInt();
                      }
                      weapons.reserved = new int[10];
                      for (int k = 0; k < 10; k++ ) 
                          weapons.reserved[k] = tSgStream.readInt();
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
                    return 0;                    
                  }                     
                  
                  public int write () {

                    tSgStream = new SgStream(fileLocation,SgStream.STREAM_WRITE);
                    
                    return 0;
                    
                  }    
                  
                 };                           
              