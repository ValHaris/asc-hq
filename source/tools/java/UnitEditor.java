/*
 * UnitEditor.java
 *
 * Created on 23. November 1999, 16:59
 */
 


/** 
 *
 * @author  MS
 * @version 
 */

public class UnitEditor extends javax.swing.JFrame {
  
  public static int DISPOSE = 1;
  public static int EXIT = 0;  
  
  public int initDone;
  
  public String errorMessage;  
    
  private Unit tUnit;  
  private ErrorMessage errMsg;
  private int exitAction;      
  
  
  //*Variable-Definitions*
  
  //Main-Table
  private MakeCheckIntRangeField jIntFieldID;
  private MakeCheckIntRangeField jIntFieldArmor;
  private MakeCheckIntRangeField jIntFieldWeight;
  private MakeCheckIntRangeField jIntFieldEnergyTank;
  private MakeCheckIntRangeField jIntFieldMaterialTank;
  private MakeCheckIntRangeField jIntFieldProductionEnergy;
  private MakeCheckIntRangeField jIntFieldProductionMaterial;
  private MakeCheckIntRangeField jIntFieldView;
  private MakeCheckIntRangeField jIntFieldJamming;    
  //Movement-Table
  private MakeCheckIntRangeField jIntFieldMovementTable[];
  private MakeCheckIntRangeField jIntFieldFuelconsumption;
  private MakeCheckIntRangeField jIntFieldTank;
  private MakeCheckIntRangeField jIntFieldDistanceHeightChange;
  private MakeCheckIntRangeField jIntFieldMaxWindspeed;
  private MakeCheckIntRangeField jIntFieldMaxLoad;
  private MakeCheckIntRangeField jIntFieldMaxUnitWeight;  
  private javax.swing.JCheckBox movementTableCheckBox[];
  private javax.swing.JTextField movementTableTextField[];  
  //Load-Table
  private javax.swing.JCheckBox loadOnHeightTableCheckBox[];
  private javax.swing.JCheckBox loadAbleHeightTableCheckBox[];
  private javax.swing.JCheckBox loadNotHeightTableCheckBox[];  
  //Functions-Table
  private MakeCheckIntRangeField jIntFieldResourceRadius;  
  private javax.swing.JCheckBox functionSelectCheckBox[];
  //Class-Table
  private String classNames[];
  private int classLastIndex = 0;
  private javax.swing.JCheckBox classFunctionSelectCheckBox[];
  private static int CLASS_STANDARD = 0;  
  private static int CLASS_ORIGINAL = 1;  
  private javax.swing.JTextField classWeaponImprovementTextField[];  
  private MakeCheckIntRangeField jIntFieldClassWeaponImprovement[];
  private javax.swing.JTextField classTechnologyIDTextField[];  
  private MakeCheckIntRangeField jIntFieldClassTechnologyID[];
  private MakeCheckIntRangeField jIntFieldClassTechLevel;
  //Weapon-Table
  private int weaponLastIndex = 0;
  private static int WEAPON_NOSAVE = 0;  
  private static int WEAPON_SAVE = 1;  
  private javax.swing.JCheckBox weaponTypeSelectCheckBox[];
  private javax.swing.JCheckBox weaponAimSelectCheckBox[];
  private javax.swing.JCheckBox weaponSourceSelectCheckBox[];  
  private MakeCheckIntRangeField jIntFieldMinDistance;
  private MakeCheckIntRangeField jIntFieldMaxDistance;
  private MakeCheckIntRangeField jIntFieldMinDistanceStrength;
  private MakeCheckIntRangeField jIntFieldMaxDistanceStrength;
  private MakeCheckIntRangeField jIntFieldAmmo;
  private javax.swing.JTextField jTxtFieldweaponEfficiencies[];  
  private MakeCheckIntRangeField jIntFieldEfficiencies[];
  
  
    
   
  /** Creates new form UnitWindow */
public UnitEditor(String unitPath, String unitFileName, int action,int exitAct){
  // action new / load;
  //exitAct exit / dispose
    initDone = 0;
  
    if (unitPath.length() == 0) return;
    exitAction = exitAct;
    initComponents ();
    setTitle(unitFileName);
    
    if ( unitPath.endsWith("\\") == false ) unitPath = unitPath.concat("\\");
    String unitAbsoluteFileName = unitPath.concat(unitFileName);
    tUnit = new Unit(unitAbsoluteFileName);
    if (action == 0 ) tUnit.makeNew();
    else tUnit.load();
    
    errMsg = new ErrorMessage(jLabelErrorMessage);          
    //*Int-Field-Settings*
    
    // Main-Panel
    jIntFieldID = new MakeCheckIntRangeField(jTextFieldID,errMsg,0,65534);       
    jIntFieldArmor = new MakeCheckIntRangeField(jTextFieldArmor,errMsg,0,65535);
    jIntFieldWeight = new MakeCheckIntRangeField(jTextFieldWeight,
      errMsg,0,32000); 
    jIntFieldEnergyTank = new MakeCheckIntRangeField(jTextFieldEnergyTank,
      errMsg,0,2147483646); 
    jIntFieldMaterialTank = new MakeCheckIntRangeField(jTextFieldMaterialTank,
      errMsg,0,2147483646); 
    jIntFieldProductionEnergy = new MakeCheckIntRangeField(jTextFieldProductionEnergy,
      errMsg,0,65535); 
    jIntFieldProductionMaterial = new MakeCheckIntRangeField(jTextFieldProductionMaterial,
      errMsg,0,65535); 
    jIntFieldView = new MakeCheckIntRangeField(jTextFieldView,errMsg,0,255); 
    jIntFieldJamming = new MakeCheckIntRangeField(jTextFieldJamming,
      errMsg,0,255); 
    // Movement-Panel
    jIntFieldFuelconsumption = new MakeCheckIntRangeField(jTextFieldFuelconsumption,
      errMsg,0,65534); 
    jIntFieldTank = new MakeCheckIntRangeField(jTextFieldTank,
      errMsg,0,2147483646); 
    jIntFieldDistanceHeightChange = new MakeCheckIntRangeField(jTextFieldDistanceHeightChange,
      errMsg,0,255); 
    jIntFieldMaxWindspeed = new MakeCheckIntRangeField(jTextFieldMaxWindspeed,
      errMsg,0,255); 
    // Load-Panel
    jIntFieldMaxLoad = new MakeCheckIntRangeField(jTextFieldMaxLoad,
      errMsg,0,32000);   
    jIntFieldMaxUnitWeight = new MakeCheckIntRangeField(jTextFieldMaxUnitWeight,
      errMsg,0,32000); 
    // Functions-Panel    
    jIntFieldResourceRadius = new MakeCheckIntRangeField(jTextFieldResourceRadius,
      errMsg,0,255); 
    // Class-Panel    
    jIntFieldClassTechLevel = new MakeCheckIntRangeField(jTextFieldTechLevel,
      errMsg,0,255); 
    // Weapon-Panel    
    jIntFieldMinDistance = new MakeCheckIntRangeField(jTextFieldWeaponMinDistance,
      errMsg,0,255); 
    jIntFieldMaxDistance = new MakeCheckIntRangeField(jTextFieldWeaponMaxDistance,
      errMsg,0,255); 
    jIntFieldMinDistanceStrength = new MakeCheckIntRangeField(jTextFieldWeaponMinStrength,
      errMsg,1,2147483646); 
    jIntFieldMaxDistanceStrength = new MakeCheckIntRangeField(jTextFieldWeaponMaxStrength,
      errMsg,1,2147483646); 
    jIntFieldAmmo = new MakeCheckIntRangeField(jTextFieldWeaponAmmo,
      errMsg,0,255);        
    
    //*Field-Value-Initialisations*
    
    // Main-Panel
    if (tUnit.name != null) jTxtFieldName.setText (tUnit.name);
    else jTxtFieldName.setText ("");
    if (tUnit.description != null) 
      jTxtFieldDescription.setText (tUnit.description);  
    else jTxtFieldDescription.setText ("");  
    jIntFieldID.setInt(tUnit.id);
    jIntFieldArmor.setInt(tUnit.armor);
    jIntFieldWeight.setInt(tUnit.weight);
    jIntFieldEnergyTank.setInt(tUnit.energy);
    jIntFieldMaterialTank.setInt(tUnit.material);
    jIntFieldProductionEnergy.setInt(tUnit.production.energy);
    jIntFieldProductionMaterial.setInt(tUnit.production.material);
    jIntFieldView.setInt(tUnit.view);
    jIntFieldJamming.setInt(tUnit.jamming);
    // InfoText-Panel
    if (tUnit.infotext != null) jTextPaneInfoText.setText (tUnit.infotext);
    else jTextPaneInfoText.setText ("");
    // Movement-Panel
    jIntFieldFuelconsumption.setInt(tUnit.fuelconsumption);
    jIntFieldTank.setInt(tUnit.tank);
    jIntFieldDistanceHeightChange.setInt(tUnit.steigung);
    jIntFieldMaxWindspeed.setInt(tUnit.maxwindspeedonwater);
    // Load-Panel
    jIntFieldMaxLoad.setInt(tUnit.loadcapacity); 
    if (tUnit.maxunitweight <=0 ) tUnit.maxunitweight = tUnit.loadcapacity;
    jIntFieldMaxUnitWeight.setInt(tUnit.maxunitweight); 
    // Functions-Panel
    jIntFieldResourceRadius.setInt(tUnit.digrange); 
     
    //*Variable-Layout-Design*
    
    // Movement-Panel
    jPanelMovementTable.setLayout 
      (new java.awt.GridLayout (cHeightLevel.length, 2));
    movementTableCheckBox = new javax.swing.JCheckBox[cHeightLevel.length];
    movementTableTextField = new javax.swing.JTextField[cHeightLevel.length];
    jIntFieldMovementTable = new MakeCheckIntRangeField[cHeightLevel.length];
    
    for (int i=0; i < cHeightLevel.length;i++) {
      movementTableCheckBox[i] = new javax.swing.JCheckBox();
      movementTableCheckBox[i].setText (cHeightLevel[i]);
      movementTableCheckBox[i].setName(new java.lang.Integer(i).toString());

      jPanelMovementTable.add (movementTableCheckBox[i]);
      
      movementTableTextField[i] = new javax.swing.JTextField();
      movementTableTextField[i].setBorder (new javax.swing.border
      .TitledBorder(new javax.swing.border.EtchedBorder(),cHeightLevel[i] 
        .concat(".Movement (0-255)"), 1, 2, new java.awt.Font ("Arial", 0, 10)));
      movementTableTextField[i].setText ("0");
      movementTableTextField[i].setEnabled (false);
      jIntFieldMovementTable[i] = new MakeCheckIntRangeField
        (movementTableTextField[i],errMsg,0,255); 
      
      if ( (tUnit.height & (1 << i)) > 0 ) {
        movementTableCheckBox[i].setSelected(true);
        movementTableTextField[i].setEnabled (true);
        jIntFieldMovementTable[i].setInt(tUnit.movement[i]);
      }
      
      jPanelMovementTable.add (movementTableTextField[i]);     
      
      movementTableCheckBox[i].addActionListener (new java.awt.event.ActionListener () {
        public void actionPerformed (java.awt.event.ActionEvent evt) {
          movementTableActionPerformed(evt);
        }
      }
      );
    }   
    
    for (int i = 0; i < cMovemaliType.length;i++) 
      jComboBoxCategory.addItem(cMovemaliType[i]);  
    jComboBoxCategory.setSelectedIndex(tUnit.movemalustyp);
    
    if (tUnit.wait > 0 ) jCheckBoxWaitAfterMove.setSelected(true);
    else jCheckBoxWaitAfterMove.setSelected(false);
    
    heightChangeCheck(); //check, if at least 2 heights r selected
    
    
    //Load-Table
    jPanelLoadTable.setLayout (new java.awt.GridLayout (cHeightLevel.length, 3));
    loadOnHeightTableCheckBox = new javax.swing.JCheckBox[cHeightLevel.length];
    loadAbleHeightTableCheckBox = new javax.swing.JCheckBox[cHeightLevel.length];
    loadNotHeightTableCheckBox = new javax.swing.JCheckBox[cHeightLevel.length];
    
    
    for (int i=0; i < cHeightLevel.length;i++) {
      loadOnHeightTableCheckBox[i] = new javax.swing.JCheckBox();
      loadOnHeightTableCheckBox[i].setText (cHeightLevel[i]);
      loadAbleHeightTableCheckBox[i] = new javax.swing.JCheckBox();
      loadAbleHeightTableCheckBox[i].setText (cHeightLevel[i]);
      loadNotHeightTableCheckBox[i] = new javax.swing.JCheckBox();
      loadNotHeightTableCheckBox[i].setText (cHeightLevel[i]);
            
      jPanelLoadTable.add (loadOnHeightTableCheckBox[i]);
      jPanelLoadTable.add (loadAbleHeightTableCheckBox[i]);
      jPanelLoadTable.add (loadNotHeightTableCheckBox[i]);
            
      if ( (tUnit.loadcapability & (1 << i)) > 0 ) 
        loadOnHeightTableCheckBox[i].setSelected(true);
      if ( (tUnit.loadcapabilityreq & (1 << i)) > 0 ) 
        loadAbleHeightTableCheckBox[i].setSelected(true);
      if ( (tUnit.loadcapabilitynot & (1 << i)) > 0 ) 
        loadNotHeightTableCheckBox[i].setSelected(true);
    }    
    
    jTextFieldMaxLoad.addActionListener (new java.awt.event.ActionListener () {
      public void actionPerformed (java.awt.event.ActionEvent evt) {        
        maxLoadCheck();
      }
    }
    );
    jTextFieldMaxLoad.addFocusListener (new java.awt.event.FocusAdapter () {
      public void focusLost (java.awt.event.FocusEvent evt) {        
        maxLoadCheck();
      }
    }
    );
    
    maxLoadCheck();
    
    //Functions-Table
    jPanelFunctionSelect.setLayout 
      (new java.awt.GridLayout (cVehicleFunctions.length, 3));
    functionSelectCheckBox = new javax.swing.JCheckBox[cVehicleFunctions.length];
    
    for (int i=0; i < cVehicleFunctions.length;i++) {
      functionSelectCheckBox[i] = new javax.swing.JCheckBox();
      functionSelectCheckBox[i].setText (cVehicleFunctions[i]);
          
      jPanelFunctionSelect.add (functionSelectCheckBox[i]);
      
      if ( (tUnit.functions & (1 << i)) > 0 ) 
        functionSelectCheckBox[i].setSelected(true);
      
      if (((1 << i) & (cfautodigger | cfmanualdigger)) > 0) {
        functionSelectCheckBox[i].addActionListener (new java.awt.event.ActionListener () {        
          public void actionPerformed (java.awt.event.ActionEvent evt) {
            digCheck();
          }
        }
        );
        functionSelectCheckBox[i].addFocusListener (new java.awt.event.FocusAdapter () {
          public void focusLost (java.awt.event.FocusEvent evt) {
            digCheck();
          }
        }
        );
      }
    }
    
    digCheck();
    
    //Class-Table
    
    classNames = new String[9];
    //Einer mehr als Classes, damit man auch hinter der letzten Class einfuegen kann
    for (int i=0; i<8;i++) {
      if (tUnit.classnames[i] != null) classNames[i] = tUnit.classnames[i];
    }      
    jListClasses.setListData(classNames);    
    jListClasses.setSelectionMode
      (new javax.swing.DefaultListSelectionModel().SINGLE_SELECTION);    
    
    jPanelClassFunctionSelect.setLayout 
      (new java.awt.GridLayout (cVehicleFunctions.length, 1));
    classFunctionSelectCheckBox = new javax.swing.JCheckBox[cVehicleFunctions.length];
    
    for (int i=0; i < cVehicleFunctions.length;i++) {
      classFunctionSelectCheckBox[i] = new javax.swing.JCheckBox();
      classFunctionSelectCheckBox[i].setText (cVehicleFunctions[i]);          
      jPanelClassFunctionSelect.add (classFunctionSelectCheckBox[i]);
    } 
    
    jPanelClassWeaponImprovement.setLayout 
      (new java.awt.GridLayout (tUnit.classbound[0].weaponcount + 1, 1));
    
    classWeaponImprovementTextField = 
      new javax.swing.JTextField[tUnit.classbound[0].weaponcount +1];
    jIntFieldClassWeaponImprovement = 
      new MakeCheckIntRangeField[tUnit.classbound[0].weaponcount +1];
    
    for (int i=0; i < tUnit.classbound[0].weaponcount;i++) {      
      classWeaponImprovementTextField[i] = new javax.swing.JTextField();
      classWeaponImprovementTextField[i].setBorder (new javax.swing.border
        .TitledBorder(new javax.swing.border.EtchedBorder(),cWeaponType[i] 
        .concat(".Improvement (1024-65530)"), 1, 2, new java.awt.Font ("Arial", 0, 10)));                  
      jIntFieldClassWeaponImprovement[i] = new MakeCheckIntRangeField
        (classWeaponImprovementTextField[i],errMsg,1024,65530);       
      jIntFieldClassWeaponImprovement[i].setInt(0);      
      
      jPanelClassWeaponImprovement.add (classWeaponImprovementTextField[i]);
    }
    
    classWeaponImprovementTextField[tUnit.classbound[0].weaponcount] = new javax.swing.JTextField();
    classWeaponImprovementTextField[tUnit.classbound[0].weaponcount].setBorder 
      (new javax.swing.border.TitledBorder(new javax.swing.border.EtchedBorder(),
      "armor.Improvement (1024-65530)", 1, 2, new java.awt.Font ("Arial", 0, 10)));
    classWeaponImprovementTextField[tUnit.classbound[0].weaponcount].setEnabled(true);        
    jIntFieldClassWeaponImprovement[tUnit.classbound[0].weaponcount] = new MakeCheckIntRangeField
        (classWeaponImprovementTextField[tUnit.classbound[0].weaponcount],errMsg,1024,65530);       
    jPanelClassWeaponImprovement.add (classWeaponImprovementTextField[tUnit.classbound[0].weaponcount]);
    // Armor improvement ist einfach an Weapon angehängt
    
    classTechnologyIDTextField = new javax.swing.JTextField[4];
    jIntFieldClassTechnologyID = new MakeCheckIntRangeField[4];
    
    for (int i=0; i < 4;i++) {      
      classTechnologyIDTextField[i] = new javax.swing.JTextField();
      classTechnologyIDTextField[i].setBorder (new javax.swing.border
        .TitledBorder(new javax.swing.border.EtchedBorder(),"Technology required ID "
        .concat(new java.lang.Integer(i+1).toString()).concat(" (0-65500)")
        , 1, 2, new java.awt.Font ("Arial", 0, 10)));
      classTechnologyIDTextField[i].setText ("0");
      classTechnologyIDTextField[i].setEnabled (false);
      classTechnologyIDTextField[i].setName(new java.lang.Integer(i).toString());
      jIntFieldClassTechnologyID[i] = new MakeCheckIntRangeField
        (classTechnologyIDTextField[i],errMsg,0,65500); 
      
      jPanelClassTechnologyIDs.add (classTechnologyIDTextField[i]);    
      
      classTechnologyIDTextField[i].addActionListener (new java.awt.event.ActionListener () {        
        public void actionPerformed (java.awt.event.ActionEvent evt) {
          technologyIDCheck();
        }
      }
      );
      classTechnologyIDTextField[i].addFocusListener (new java.awt.event.FocusAdapter () {
        public void focusLost (java.awt.event.FocusEvent evt) {
          technologyIDCheck();
        }
      }
      );
    }
    classTechnologyIDTextField[0].setEnabled (true);
    
    showClassValues(0);  
    
    //Weapon-Table
    
    for (int i = 0; i < tUnit.weapons.count;i++) 
      jComboBoxWeapons.addItem("Weapon ".concat(new java.lang.Integer(i+1).toString()));  
      jComboBoxWeapons.setSelectedIndex(0);    
      
    jPanelWeaponType.setLayout 
      (new java.awt.GridLayout (cWeaponType.length, 1));
    
    weaponTypeSelectCheckBox = new javax.swing.JCheckBox[cWeaponType.length];
    
    for (int i=0; i < cWeaponType.length;i++) {
      weaponTypeSelectCheckBox[i] = new javax.swing.JCheckBox();
      weaponTypeSelectCheckBox[i].setText (cWeaponType[i]);          
      jPanelWeaponType.add (weaponTypeSelectCheckBox[i]);
    }   
    
    jPanelWeaponAim.setLayout 
      (new java.awt.GridLayout (cHeightLevel.length, 1));
    
    weaponAimSelectCheckBox = new javax.swing.JCheckBox[cHeightLevel.length];
    
    for (int i=0; i < cHeightLevel.length;i++) {
      weaponAimSelectCheckBox[i] = new javax.swing.JCheckBox();
      weaponAimSelectCheckBox[i].setText (cHeightLevel[i]);          
      jPanelWeaponAim.add (weaponAimSelectCheckBox[i]);
    }       
    
    jPanelWeaponSource.setLayout 
      (new java.awt.GridLayout (cHeightLevel.length, 1));
    
    weaponSourceSelectCheckBox = new javax.swing.JCheckBox[cHeightLevel.length];
    
    for (int i=0; i < cHeightLevel.length;i++) {
      weaponSourceSelectCheckBox[i] = new javax.swing.JCheckBox();
      weaponSourceSelectCheckBox[i].setText (cHeightLevel[i]);          
      jPanelWeaponSource.add (weaponSourceSelectCheckBox[i]);
    }       
    
    jPanelEfficiencies.setLayout 
      (new java.awt.GridLayout (13, 1));
    
    jTxtFieldweaponEfficiencies = new javax.swing.JTextField[13];
    jIntFieldEfficiencies = new MakeCheckIntRangeField[13];
    
    for (int i=0; i < 13;i++) {      
      jTxtFieldweaponEfficiencies[i] = new javax.swing.JTextField();
      jTxtFieldweaponEfficiencies[i].setBorder (new javax.swing.border
        .TitledBorder(new javax.swing.border.EtchedBorder(),"height " 
        .concat(new java.lang.Integer(i-6).toString()).concat(" (0-100)"), 1, 2, 
        new java.awt.Font ("Arial", 0, 10)));      
      jIntFieldEfficiencies[i] = new MakeCheckIntRangeField
        (jTxtFieldweaponEfficiencies[i],errMsg,0,100); 
      jIntFieldEfficiencies[i].setInt(100);
      
      jPanelEfficiencies.add (jTxtFieldweaponEfficiencies[i]);
    }
    
    jTextFieldWeaponMaxDistance.addActionListener (new java.awt.event.ActionListener () {
      public void actionPerformed (java.awt.event.ActionEvent evt) {        
        weaponMaxDistanceCheck();
      }
    }
    );
    jTextFieldWeaponMaxDistance.addFocusListener (new java.awt.event.FocusAdapter () {
      public void focusLost (java.awt.event.FocusEvent evt) {        
        weaponMaxDistanceCheck();
      }
    }
    );
    
    showWeaponValues(0,WEAPON_NOSAVE);
    
    weaponMaxDistanceCheck();
    
    initDone = 1;
    
    pack ();
  }

  /** This method is called from within the constructor to
   * initialize the form.
   * WARNING: Do NOT modify this code. The content of this method is
   * always regenerated by the FormEditor.
   */
  private void initComponents () {//GEN-BEGIN:initComponents
    jTabbedPaneMain = new javax.swing.JTabbedPane ();
    jPanelClass = new javax.swing.JPanel ();
    jPanelClassFunctionSelect = new javax.swing.JPanel ();
    jListClasses = new javax.swing.JList ();
    jTextFieldClassName = new javax.swing.JTextField ();
    jButtonNewClass = new javax.swing.JButton ();
    jButtonSetClassName = new javax.swing.JButton ();
    jButtonDeleteClass = new javax.swing.JButton ();
    jButtonSetClassStandard = new javax.swing.JButton ();
    jPanelClassWeaponImprovement = new javax.swing.JPanel ();
    jPanelClassTechnologyIDs = new javax.swing.JPanel ();
    jTextFieldTechLevel = new javax.swing.JTextField ();
    jPanelFunctions = new javax.swing.JPanel ();
    jPanelFunctionSelect = new javax.swing.JPanel ();
    jTextFieldResourceRadius = new javax.swing.JTextField ();
    jPanelMovement = new javax.swing.JPanel ();
    jPanelMovementTable = new javax.swing.JPanel ();
    jComboBoxCategory = new javax.swing.JComboBox ();
    jCheckBoxWaitAfterMove = new javax.swing.JCheckBox ();
    jTextFieldFuelconsumption = new javax.swing.JTextField ();
    jTextFieldTank = new javax.swing.JTextField ();
    jLabelFuelconsumptionTankBorder = new javax.swing.JLabel ();
    jTextFieldDistanceHeightChange = new javax.swing.JTextField ();
    jTextFieldMaxWindspeed = new javax.swing.JTextField ();
    jLabelMaxWindspeed = new javax.swing.JLabel ();
    jLabelWindspeedBorder = new javax.swing.JLabel ();
    jPanelMain = new javax.swing.JPanel ();
    jTxtFieldName = new javax.swing.JTextField ();
    jTxtFieldDescription = new javax.swing.JTextField ();
    jTextFieldID = new javax.swing.JTextField ();
    jTextFieldArmor = new javax.swing.JTextField ();
    jTextFieldProductionMaterial = new javax.swing.JTextField ();
    jTextFieldProductionEnergy = new javax.swing.JTextField ();
    jTextFieldWeight = new javax.swing.JTextField ();
    jTextFieldView = new javax.swing.JTextField ();
    jTextFieldJamming = new javax.swing.JTextField ();
    jTextFieldEnergyTank = new javax.swing.JTextField ();
    jTextFieldMaterialTank = new javax.swing.JTextField ();
    jLabelProductionBorder = new javax.swing.JLabel ();
    jLabelNameDescriptionBorder = new javax.swing.JLabel ();
    jLabelIDArmorWeightBorder = new javax.swing.JLabel ();
    jLabelViewJammingBorder = new javax.swing.JLabel ();
    jLabelEnergyMaterialTankBorder = new javax.swing.JLabel ();
    jLabelEnergyTankWarning = new javax.swing.JLabel ();
    jPanelInfoText = new javax.swing.JPanel ();
    jScrollPaneInfoText = new javax.swing.JScrollPane ();
    jTextPaneInfoText = new javax.swing.JTextPane ();
    jPanelLoad = new javax.swing.JPanel ();
    jTextFieldMaxLoad = new javax.swing.JTextField ();
    jTextFieldMaxUnitWeight = new javax.swing.JTextField ();
    jPanelLoadTable = new javax.swing.JPanel ();
    jPanelLoadTabelTitle = new javax.swing.JPanel ();
    jLabelOnHeight = new javax.swing.JLabel ();
    jLabelAbleHeight = new javax.swing.JLabel ();
    jLabelNotHeight = new javax.swing.JLabel ();
    jPanelWeapons = new javax.swing.JPanel ();
    jComboBoxWeapons = new javax.swing.JComboBox ();
    jPanelEfficiencies = new javax.swing.JPanel ();
    jPanelWeaponAim = new javax.swing.JPanel ();
    jPanelWeaponSource = new javax.swing.JPanel ();
    jPanelWeaponType = new javax.swing.JPanel ();
    jTextFieldWeaponMinDistance = new javax.swing.JTextField ();
    jTextFieldWeaponMaxDistance = new javax.swing.JTextField ();
    jTextFieldWeaponMinStrength = new javax.swing.JTextField ();
    jTextFieldWeaponMaxStrength = new javax.swing.JTextField ();
    jTextFieldWeaponAmmo = new javax.swing.JTextField ();
    jButtonAddWeapon = new javax.swing.JButton ();
    jButtonRemoveWeapon = new javax.swing.JButton ();
    jLabelErrorMessage = new javax.swing.JLabel ();
    getContentPane ().setLayout (new AbsoluteLayout ());
    addWindowListener (new java.awt.event.WindowAdapter () {
      public void windowClosing (java.awt.event.WindowEvent evt) {
        closeDialog (evt);
      }
    }
    );

    jTabbedPaneMain.addChangeListener (new javax.swing.event.ChangeListener () {
      public void stateChanged (javax.swing.event.ChangeEvent evt) {
        jTabbedPaneMainStateChanged (evt);
      }
    }
    );

    jPanelClass.setLayout (new AbsoluteLayout ());

    jPanelClassFunctionSelect.setLayout (new java.awt.GridLayout (25, 1));

    jPanelClass.add (jPanelClassFunctionSelect, new AbsoluteConstraints (530, 10, 250, 420));

    jListClasses.setBorder (new javax.swing.border.TitledBorder(
    new javax.swing.border.EtchedBorder(), "Classes", 1, 2,
    new java.awt.Font ("Arial", 0, 10)));
    jListClasses.addListSelectionListener (new javax.swing.event.ListSelectionListener () {
      public void valueChanged (javax.swing.event.ListSelectionEvent evt) {
        jListClassesValueChanged (evt);
      }
    }
    );

    jPanelClass.add (jListClasses, new AbsoluteConstraints (0, 60, 220, 180));

    jTextFieldClassName.setBorder (new javax.swing.border.TitledBorder(
    new javax.swing.border.EtchedBorder(), "Classname", 1, 2,
    new java.awt.Font ("Arial", 0, 10)));
    jTextFieldClassName.setText ("Classname");

    jPanelClass.add (jTextFieldClassName, new AbsoluteConstraints (0, 0, 220, 50));

    jButtonNewClass.setText ("New class");
    jButtonNewClass.addActionListener (new java.awt.event.ActionListener () {
      public void actionPerformed (java.awt.event.ActionEvent evt) {
        jButtonNewClassActionPerformed (evt);
      }
    }
    );

    jPanelClass.add (jButtonNewClass, new AbsoluteConstraints (0, 250, 110, 30));

    jButtonSetClassName.setText ("Set name");
    jButtonSetClassName.addActionListener (new java.awt.event.ActionListener () {
      public void actionPerformed (java.awt.event.ActionEvent evt) {
        jButtonSetClassNameActionPerformed (evt);
      }
    }
    );

    jPanelClass.add (jButtonSetClassName, new AbsoluteConstraints (110, 250, 110, 30));

    jButtonDeleteClass.setText ("Delete class");
    jButtonDeleteClass.addActionListener (new java.awt.event.ActionListener () {
      public void actionPerformed (java.awt.event.ActionEvent evt) {
        jButtonDeleteClassActionPerformed (evt);
      }
    }
    );

    jPanelClass.add (jButtonDeleteClass, new AbsoluteConstraints (110, 280, 110, 30));

    jButtonSetClassStandard.setText ("Set standard");
    jButtonSetClassStandard.addActionListener (new java.awt.event.ActionListener () {
      public void actionPerformed (java.awt.event.ActionEvent evt) {
        jButtonSetClassStandardActionPerformed (evt);
      }
    }
    );

    jPanelClass.add (jButtonSetClassStandard, new AbsoluteConstraints (0, 280, 110, 30));

    jPanelClassWeaponImprovement.setLayout (new java.awt.GridLayout (9, 1));

    jPanelClass.add (jPanelClassWeaponImprovement, new AbsoluteConstraints (230, 10, 290, 470));

    jPanelClassTechnologyIDs.setLayout (new java.awt.GridLayout (4, 1));

    jPanelClass.add (jPanelClassTechnologyIDs, new AbsoluteConstraints (0, 320, 220, 160));

    jTextFieldTechLevel.setBorder (new javax.swing.border.TitledBorder(
    new javax.swing.border.EtchedBorder(), "Techlevel (0-255)", 1, 2,
    new java.awt.Font ("Arial", 0, 10)));
    jTextFieldTechLevel.setText ("0");

    jPanelClass.add (jTextFieldTechLevel, new AbsoluteConstraints (530, 440, 250, 40));

    jTabbedPaneMain.addTab ("Class", jPanelClass);

    jPanelFunctions.setLayout (new AbsoluteLayout ());

    jPanelFunctionSelect.setLayout (new java.awt.GridLayout (25, 1));

    jPanelFunctions.add (jPanelFunctionSelect, new AbsoluteConstraints (10, 10, 410, 460));

    jTextFieldResourceRadius.setBorder (new javax.swing.border.TitledBorder(
    new javax.swing.border.EtchedBorder(), "Radius to  check for resources (0-255)", 1, 2,
    new java.awt.Font ("Arial", 0, 10)));
    jTextFieldResourceRadius.setText ("0");

    jPanelFunctions.add (jTextFieldResourceRadius, new AbsoluteConstraints (440, 20, 330, 40));

    jTabbedPaneMain.addTab ("Functions", jPanelFunctions);

    jPanelMovement.setLayout (new AbsoluteLayout ());

    jPanelMovementTable.setLayout (new java.awt.GridLayout (8, 2));

    jPanelMovement.add (jPanelMovementTable, new AbsoluteConstraints (340, 10, 430, 360));

    jComboBoxCategory.setBorder (new javax.swing.border.TitledBorder(
    new javax.swing.border.EtchedBorder(), "Category", 1, 2,
    new java.awt.Font ("Arial", 0, 10)));

    jPanelMovement.add (jComboBoxCategory, new AbsoluteConstraints (20, 40, 290, 50));

    jCheckBoxWaitAfterMove.setText ("wait after movement (for attack)");

    jPanelMovement.add (jCheckBoxWaitAfterMove, new AbsoluteConstraints (20, 130, 290, 40));

    jTextFieldFuelconsumption.setBorder (new javax.swing.border.TitledBorder(
    new javax.swing.border.EtchedBorder(), "Fuelconsumption (0-65534)", 1, 2,
    new java.awt.Font ("Arial", 0, 10)));
    jTextFieldFuelconsumption.setText ("0");

    jPanelMovement.add (jTextFieldFuelconsumption, new AbsoluteConstraints (360, 410, 180, 40));

    jTextFieldTank.setBorder (new javax.swing.border.TitledBorder(
    new javax.swing.border.EtchedBorder(), "Tank (0-2147483646)", 1, 2,
    new java.awt.Font ("Arial", 0, 10)));
    jTextFieldTank.setText ("0");

    jPanelMovement.add (jTextFieldTank, new AbsoluteConstraints (580, 410, 170, 40));

    jLabelFuelconsumptionTankBorder.setBorder (new javax.swing.border.EtchedBorder());

    jPanelMovement.add (jLabelFuelconsumptionTankBorder, new AbsoluteConstraints (340, 390, 430, 80));

    jTextFieldDistanceHeightChange.setBorder (new javax.swing.border.TitledBorder(
    new javax.swing.border.EtchedBorder(), "Distance for height-change (0-255)", 1, 2,
    new java.awt.Font ("Arial", 0, 10)));
    jTextFieldDistanceHeightChange.setText ("0");

    jPanelMovement.add (jTextFieldDistanceHeightChange, new AbsoluteConstraints (20, 210, 290, 40));

    jTextFieldMaxWindspeed.setBorder (new javax.swing.border.TitledBorder(
    new javax.swing.border.EtchedBorder(), "Maximum windspeed (0-255)", 1, 2,
    new java.awt.Font ("Arial", 0, 10)));
    jTextFieldMaxWindspeed.setText ("0");

    jPanelMovement.add (jTextFieldMaxWindspeed, new AbsoluteConstraints (30, 310, 270, 40));

    jLabelMaxWindspeed.setBorder (new javax.swing.border.TitledBorder(
    new javax.swing.border.EtchedBorder(), "Maximum windspeed.Note", 1, 2,
    new java.awt.Font ("Arial", 0, 10)));
    jLabelMaxWindspeed.setText ("maximum windspeed that can be survived");
    jLabelMaxWindspeed.setFont (new java.awt.Font ("Arial", 0, 10));

    jPanelMovement.add (jLabelMaxWindspeed, new AbsoluteConstraints (30, 380, 270, 50));

    jLabelWindspeedBorder.setBorder (new javax.swing.border.EtchedBorder());

    jPanelMovement.add (jLabelWindspeedBorder, new AbsoluteConstraints (10, 290, 310, 180));

    jTabbedPaneMain.addTab ("Movement", jPanelMovement);

    jPanelMain.setLayout (new AbsoluteLayout ());
    jPanelMain.setBorder (new javax.swing.border.EmptyBorder(new java.awt.Insets(1, 1, 1, 1)));

    jTxtFieldName.setBorder (new javax.swing.border.TitledBorder(
    new javax.swing.border.EtchedBorder(), "Name", 1, 2, new java.awt.Font ("Arial", 0, 10)));
    jTxtFieldName.setText ("Name");

    jPanelMain.add (jTxtFieldName, new AbsoluteConstraints (20, 20, 750, 40));

    jTxtFieldDescription.setBorder (new javax.swing.border.TitledBorder(
    new javax.swing.border.EtchedBorder(), "Description", 1, 2,
    new java.awt.Font ("Arial", 0, 10)));
    jTxtFieldDescription.setText ("Description");

    jPanelMain.add (jTxtFieldDescription, new AbsoluteConstraints (20, 70, 750, 40));

    jTextFieldID.setBorder (new javax.swing.border.TitledBorder(
    new javax.swing.border.EtchedBorder(), "ID (0-65534)", 1, 2,
    new java.awt.Font ("Arial", 0, 10)));
    jTextFieldID.setText ("0");

    jPanelMain.add (jTextFieldID, new AbsoluteConstraints (30, 160, 380, 40));

    jTextFieldArmor.setBorder (new javax.swing.border.TitledBorder(
    new javax.swing.border.EtchedBorder(), "Armor (0-65535)", 1, 2,
    new java.awt.Font ("Arial", 0, 10)));
    jTextFieldArmor.setText ("0");

    jPanelMain.add (jTextFieldArmor, new AbsoluteConstraints (30, 210, 380, 40));

    jTextFieldProductionMaterial.setBorder (new javax.swing.border.TitledBorder(
    new javax.swing.border.EtchedBorder(), "Produktioncost.Material (0-65535)", 1, 2,
    new java.awt.Font ("Arial", 0, 10)));
    jTextFieldProductionMaterial.setText ("0");

    jPanelMain.add (jTextFieldProductionMaterial, new AbsoluteConstraints (30, 350, 380, 40));

    jTextFieldProductionEnergy.setBorder (new javax.swing.border.TitledBorder(
    new javax.swing.border.EtchedBorder(), "Produktion.Energy (0-65535)", 1, 2,
    new java.awt.Font ("Arial", 0, 10)));
    jTextFieldProductionEnergy.setText ("0");

    jPanelMain.add (jTextFieldProductionEnergy, new AbsoluteConstraints (30, 410, 380, 40));

    jTextFieldWeight.setBorder (new javax.swing.border.TitledBorder(
    new javax.swing.border.EtchedBorder(), "Weight (0-32000)", 1, 2,
    new java.awt.Font ("Arial", 0, 10)));
    jTextFieldWeight.setText ("0");

    jPanelMain.add (jTextFieldWeight, new AbsoluteConstraints (30, 260, 380, 40));

    jTextFieldView.setBorder (new javax.swing.border.TitledBorder(
    new javax.swing.border.EtchedBorder(), "View (0-255)", 1, 2,
    new java.awt.Font ("Arial", 0, 10)));
    jTextFieldView.setText ("0");

    jPanelMain.add (jTextFieldView, new AbsoluteConstraints (460, 150, 300, 40));

    jTextFieldJamming.setBorder (new javax.swing.border.TitledBorder(
    new javax.swing.border.EtchedBorder(), "Jamming (0-255)", 1, 2,
    new java.awt.Font ("Arial", 0, 10)));
    jTextFieldJamming.setText ("0");

    jPanelMain.add (jTextFieldJamming, new AbsoluteConstraints (460, 210, 300, 40));

    jTextFieldEnergyTank.setBorder (new javax.swing.border.TitledBorder(
    new javax.swing.border.EtchedBorder(), "Energy.Tank (0-2147483646)", 1, 2,
    new java.awt.Font ("Arial", 0, 10)));
    jTextFieldEnergyTank.setText ("0");

    jPanelMain.add (jTextFieldEnergyTank, new AbsoluteConstraints (460, 360, 300, 40));

    jTextFieldMaterialTank.setBorder (new javax.swing.border.TitledBorder(
    new javax.swing.border.EtchedBorder(), "Material.Tank (0-2147483646)", 1, 2,
    new java.awt.Font ("Arial", 0, 10)));
    jTextFieldMaterialTank.setText ("0");

    jPanelMain.add (jTextFieldMaterialTank, new AbsoluteConstraints (460, 300, 300, 40));

    jLabelProductionBorder.setBorder (new javax.swing.border.EtchedBorder());

    jPanelMain.add (jLabelProductionBorder, new AbsoluteConstraints (10, 330, 420, 140));

    jLabelNameDescriptionBorder.setBorder (new javax.swing.border.EtchedBorder());

    jPanelMain.add (jLabelNameDescriptionBorder, new AbsoluteConstraints (10, 10, 770, 110));

    jLabelIDArmorWeightBorder.setBorder (new javax.swing.border.EtchedBorder());

    jPanelMain.add (jLabelIDArmorWeightBorder, new AbsoluteConstraints (10, 130, 420, 190));

    jLabelViewJammingBorder.setBorder (new javax.swing.border.EtchedBorder());

    jPanelMain.add (jLabelViewJammingBorder, new AbsoluteConstraints (440, 130, 340, 140));

    jLabelEnergyMaterialTankBorder.setBorder (new javax.swing.border.EtchedBorder());

    jPanelMain.add (jLabelEnergyMaterialTankBorder, new AbsoluteConstraints (440, 280, 340, 190));

    jLabelEnergyTankWarning.setBorder (new javax.swing.border.TitledBorder(
    new javax.swing.border.EtchedBorder(), "Energy.Tank.Note", 1, 2,
    new java.awt.Font ("Arial", 0, 10)));
    jLabelEnergyTankWarning.setText ("Energy.Tank > 0 for generators ONLY !");

    jPanelMain.add (jLabelEnergyTankWarning, new AbsoluteConstraints (460, 410, 300, 40));

    jTabbedPaneMain.addTab ("Main", jPanelMain);

    jPanelInfoText.setLayout (new java.awt.GridLayout (1, 1));


    jTextPaneInfoText.setText ("InfoText");

    jScrollPaneInfoText.setViewportView (jTextPaneInfoText);

    jPanelInfoText.add (jScrollPaneInfoText);

    jTabbedPaneMain.addTab ("InfoText", jPanelInfoText);

    jPanelLoad.setLayout (new AbsoluteLayout ());

    jTextFieldMaxLoad.setBorder (new javax.swing.border.TitledBorder(
    new javax.swing.border.EtchedBorder(), "Maximum total load (0-32000)", 1, 2,
    new java.awt.Font ("Arial", 0, 10)));
    jTextFieldMaxLoad.setText ("0");

    jPanelLoad.add (jTextFieldMaxLoad, new AbsoluteConstraints (120, 10, 260, 40));

    jTextFieldMaxUnitWeight.setBorder (new javax.swing.border.TitledBorder(
    new javax.swing.border.EtchedBorder(), "Maximum weight of unit (0-32000)", 1, 2,
    new java.awt.Font ("Arial", 0, 10)));
    jTextFieldMaxUnitWeight.setText ("0");

    jPanelLoad.add (jTextFieldMaxUnitWeight, new AbsoluteConstraints (390, 10, 240, 40));

    jPanelLoadTable.setLayout (new java.awt.GridLayout (8, 3));

    jPanelLoad.add (jPanelLoadTable, new AbsoluteConstraints (10, 110, 760, 360));

    jPanelLoadTabelTitle.setLayout (new java.awt.GridLayout (1, 3));

    jLabelOnHeight.setText ("Unit must be on that height");
    jLabelOnHeight.setFont (new java.awt.Font ("Dialog", 0, 12));

    jPanelLoadTabelTitle.add (jLabelOnHeight);

    jLabelAbleHeight.setText ("Unit must be able to be on that height");
    jLabelAbleHeight.setFont (new java.awt.Font ("Dialog", 0, 12));

    jPanelLoadTabelTitle.add (jLabelAbleHeight);

    jLabelNotHeight.setText ("Unit mustn´t be able to be on that height");
    jLabelNotHeight.setFont (new java.awt.Font ("Dialog", 0, 12));

    jPanelLoadTabelTitle.add (jLabelNotHeight);

    jPanelLoad.add (jPanelLoadTabelTitle, new AbsoluteConstraints (10, 70, 760, 40));

    jTabbedPaneMain.addTab ("Load", jPanelLoad);

    jPanelWeapons.setLayout (new AbsoluteLayout ());

    jComboBoxWeapons.addItemListener (new java.awt.event.ItemListener () {
      public void itemStateChanged (java.awt.event.ItemEvent evt) {
        jComboBoxWeaponsItemStateChanged (evt);
      }
    }
    );

    jPanelWeapons.add (jComboBoxWeapons, new AbsoluteConstraints (10, 10, 250, 30));

    jPanelEfficiencies.setLayout (new java.awt.GridLayout (13, 1));
    jPanelEfficiencies.setBorder (new javax.swing.border.TitledBorder(
    new javax.swing.border.EtchedBorder(), "Efficiencies", 1, 2,
    new java.awt.Font ("Arial", 0, 10)));

    jPanelWeapons.add (jPanelEfficiencies, new AbsoluteConstraints (580, 0, 200, 470));

    jPanelWeaponAim.setLayout (new java.awt.GridLayout (8, 1));
    jPanelWeaponAim.setBorder (new javax.swing.border.TitledBorder(
    new javax.swing.border.EtchedBorder(), "Weapon.aim.heights", 1, 2,
    new java.awt.Font ("Arial", 0, 10)));

    jPanelWeapons.add (jPanelWeaponAim, new AbsoluteConstraints (270, 50, 290, 170));

    jPanelWeaponSource.setLayout (new java.awt.GridLayout (8, 1));
    jPanelWeaponSource.setBorder (new javax.swing.border.TitledBorder(
    new javax.swing.border.EtchedBorder(), "Weapon.source.heights", 1, 2,
    new java.awt.Font ("Arial", 0, 10)));

    jPanelWeapons.add (jPanelWeaponSource, new AbsoluteConstraints (270, 240, 290, 180));

    jPanelWeaponType.setLayout (new java.awt.GridLayout (12, 1));
    jPanelWeaponType.setBorder (new javax.swing.border.TitledBorder(
    new javax.swing.border.EtchedBorder(), "Weapon.type", 1, 2,
    new java.awt.Font ("Arial", 0, 10)));

    jPanelWeapons.add (jPanelWeaponType, new AbsoluteConstraints (10, 250, 250, 220));

    jTextFieldWeaponMinDistance.setBorder (new javax.swing.border.TitledBorder(
    new javax.swing.border.EtchedBorder(), "Distance.min (0-255)", 1, 2,
    new java.awt.Font ("Arial", 0, 10)));
    jTextFieldWeaponMinDistance.setText ("0");

    jPanelWeapons.add (jTextFieldWeaponMinDistance, new AbsoluteConstraints (10, 50, 250, 40));

    jTextFieldWeaponMaxDistance.setBorder (new javax.swing.border.TitledBorder(
    new javax.swing.border.EtchedBorder(), "Distance.max (0-255)", 1, 2,
    new java.awt.Font ("Arial", 0, 10)));
    jTextFieldWeaponMaxDistance.setText ("0");

    jPanelWeapons.add (jTextFieldWeaponMaxDistance, new AbsoluteConstraints (10, 100, 250, 40));

    jTextFieldWeaponMinStrength.setBorder (new javax.swing.border.TitledBorder(
    new javax.swing.border.EtchedBorder(), "Strength.at.min.distance (1-2147483646)", 1, 2,
    new java.awt.Font ("Arial", 0, 10)));
    jTextFieldWeaponMinStrength.setText ("0");

    jPanelWeapons.add (jTextFieldWeaponMinStrength, new AbsoluteConstraints (10, 150, 250, 40));

    jTextFieldWeaponMaxStrength.setBorder (new javax.swing.border.TitledBorder(
    new javax.swing.border.EtchedBorder(), "Strength.at.max.distance (1-2147483646)", 1, 2,
    new java.awt.Font ("Arial", 0, 10)));
    jTextFieldWeaponMaxStrength.setText ("0");

    jPanelWeapons.add (jTextFieldWeaponMaxStrength, new AbsoluteConstraints (10, 200, 250, 40));

    jTextFieldWeaponAmmo.setBorder (new javax.swing.border.TitledBorder(
    new javax.swing.border.EtchedBorder(), "Ammo (0-255)", 1, 2,
    new java.awt.Font ("Arial", 0, 10)));
    jTextFieldWeaponAmmo.setText ("0");

    jPanelWeapons.add (jTextFieldWeaponAmmo, new AbsoluteConstraints (270, 430, 290, 40));

    jButtonAddWeapon.setText ("Add");

    jPanelWeapons.add (jButtonAddWeapon, new AbsoluteConstraints (270, 10, 140, 30));

    jButtonRemoveWeapon.setText ("Remove");

    jPanelWeapons.add (jButtonRemoveWeapon, new AbsoluteConstraints (430, 10, 130, 30));

    jTabbedPaneMain.addTab ("Weapons", jPanelWeapons);


    getContentPane ().add (jTabbedPaneMain, new AbsoluteConstraints (0, 0, 790, 510));

    jLabelErrorMessage.setBorder (new javax.swing.border.EtchedBorder());


    getContentPane ().add (jLabelErrorMessage, new AbsoluteConstraints (0, 520, 790, 20));

  }//GEN-END:initComponents

private void jTabbedPaneMainStateChanged (javax.swing.event.ChangeEvent evt) {//GEN-FIRST:event_jTabbedPaneMainStateChanged
// Add your handling code here:  
  //if Pane is changed, check these things
  if (initDone == 1) {
    for (int i=0; i < cHeightLevel.length;i++) {
      if ( movementTableCheckBox[i].isSelected() == true ) {
        weaponSourceSelectCheckBox[i].setEnabled(true);     
      } else weaponSourceSelectCheckBox[i].setEnabled(false);      
      //Check if Unit can move on this height !!
      // if unit can move on this height it can fire from this height
      // movement -> weapon panel
    }      
        
    showWeaponValues(weaponLastIndex,WEAPON_SAVE); //save last Weapon vals before check
    
    for (int i=0; i < tUnit.classbound[0].weaponcount; i++) 
      classWeaponImprovementTextField[i].setEnabled (false);
    
    for (int i=0; i < tUnit.weapons.count;i++) {
      for (int j=0; j < tUnit.classbound[0].weaponcount;j++) {
        if ( (tUnit.weapons.weapon[i].typ & (1 << j)) > 0)                    
          classWeaponImprovementTextField[j].setEnabled (true);
      }      
    }
    // check if weapon is available, then make Class-improvement possible
    // weapon -> class panel
    
    for (int i=0; i < cVehicleFunctions.length;i++) 
      if (functionSelectCheckBox[i].isSelected() == true) 
        classFunctionSelectCheckBox[i].setEnabled(true);
      else classFunctionSelectCheckBox[i].setEnabled(false);
    // check if functions are available on this unit, then they can be available at this class
    // functions -> class panel
  }
  }//GEN-LAST:event_jTabbedPaneMainStateChanged

private void jComboBoxWeaponsItemStateChanged (java.awt.event.ItemEvent evt) {//GEN-FIRST:event_jComboBoxWeaponsItemStateChanged
// Add your handling code here: //Weapon-Panel  
  if (initDone == 1) {
    int pos = jComboBoxWeapons.getSelectedIndex();
    showWeaponValues(pos,WEAPON_SAVE);
  }
  }//GEN-LAST:event_jComboBoxWeaponsItemStateChanged

private void jButtonSetClassNameActionPerformed (java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButtonSetClassNameActionPerformed
// Add your handling code here: //Class-Panel
  if (jListClasses.isSelectionEmpty() == true) return;  
  int selection = jListClasses.getSelectedIndex();
  
  classNames[selection] = jTextFieldClassName.getText();
  tUnit.classnames[selection] = classNames[selection];
  jListClasses.setListData(classNames);
  showClassValues(selection);
  }//GEN-LAST:event_jButtonSetClassNameActionPerformed

private void jButtonSetClassStandardActionPerformed (java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButtonSetClassStandardActionPerformed
// Add your handling code here: //Class-Panel
  if (jListClasses.isSelectionEmpty() == true) return;
  int selection = jListClasses.getSelectedIndex();
  
  setClassValues(CLASS_STANDARD,selection);
  showClassValues(selection);
  }//GEN-LAST:event_jButtonSetClassStandardActionPerformed

private void jListClassesValueChanged (javax.swing.event.ListSelectionEvent evt) {//GEN-FIRST:event_jListClassesValueChanged
// Add your handling code here: //Class-Panel
  checkClassButtonsAndIndex();
  
  if (jListClasses.isSelectionEmpty() == true) return;
  int selection = jListClasses.getSelectedIndex();
  
  setClassValues(CLASS_ORIGINAL,classLastIndex);
  classLastIndex = selection;
  showClassValues(selection);
  }//GEN-LAST:event_jListClassesValueChanged

private void jButtonDeleteClassActionPerformed (java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButtonDeleteClassActionPerformed
// Add your handling code here: //Class-Panel
    if (jListClasses.isSelectionEmpty() == true) return;
    int selection = jListClasses.getSelectedIndex();
    
    tUnit.classnum--;
    for (int i = selection;i < tUnit.classnum;i++)  {
      classNames[i] = classNames[i+1];
      tUnit.classnames[i] = tUnit.classnames[i+1];
      tUnit.classbound[i] = tUnit.classbound[i+1];
    }      
    classNames[tUnit.classnum] = "";
    tUnit.classnames[tUnit.classnum] = "";    
    
    jListClasses.setListData(classNames);        
    
    showClassValues(selection);
    
    checkClassButtonsAndIndex();
  }//GEN-LAST:event_jButtonDeleteClassActionPerformed

private void jButtonNewClassActionPerformed (java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButtonNewClassActionPerformed
// Add your handling code here: //Class-Panel
    if (jListClasses.isSelectionEmpty() == true) return;
    if (jTextFieldClassName.getText().length() <= 0) {
      errMsg.showMessage("You must enter a classname !");
      return;
    }  
    int selection = jListClasses.getSelectedIndex();
    
    setClassValues(CLASS_ORIGINAL,selection);
    
    String selectedName = jTextFieldClassName.getText();
    for (int i=0; i < tUnit.classnum;i++) 
      if (classNames[i].compareTo(selectedName) == 0 ) {
        errMsg.showMessage("Name already exists !");
        return;
      }
      
    for (int i=tUnit.classnum; i > selection;i--){
      classNames[i] = classNames[i-1];
      tUnit.classnames[i] = tUnit.classnames[i-1];
      tUnit.classbound[i] = tUnit.classbound[i-1];      
    }        
    
    tUnit.classbound[selection] = new ClassBound();
    classNames[selection] = selectedName;
    tUnit.classnames[selection] = selectedName;       
    
    if (selection == tUnit.classnum) setClassValues(CLASS_STANDARD,selection);
    else setClassValues(CLASS_ORIGINAL,selection);    
    
    tUnit.classnum++;
    
    showClassValues(selection);      
    
    jListClasses.setListData(classNames);
    
    checkClassButtonsAndIndex();    
  }//GEN-LAST:event_jButtonNewClassActionPerformed

public void showWeaponValues(int index,int save) {  //Weapon-Panel
  // -1 for no save
  
    if (save == 1) {
      tUnit.weapons.weapon[weaponLastIndex].minDistance = jIntFieldMinDistance.getInt();
      tUnit.weapons.weapon[weaponLastIndex].maxDistance = jIntFieldMaxDistance.getInt();
      tUnit.weapons.weapon[weaponLastIndex].minStrength = jIntFieldMinDistanceStrength.getInt();
      tUnit.weapons.weapon[weaponLastIndex].maxStrength = jIntFieldMaxDistanceStrength.getInt();
      tUnit.weapons.weapon[weaponLastIndex].count = jIntFieldAmmo.getInt();
      
      for (int i=0; i < 13;i++) {      
        tUnit.weapons.weapon[weaponLastIndex].efficiency[i] = jIntFieldEfficiencies[i].getInt();
      }
      
      for (int i=0; i < cWeaponType.length;i++) {
        if (weaponTypeSelectCheckBox[i].isSelected() == true )  
         tUnit.weapons.weapon[weaponLastIndex].typ |= (1 << i);
        else tUnit.weapons.weapon[weaponLastIndex].typ &= ~(1 << i);     
      }
      for (int i=0; i < cHeightLevel.length;i++) {
        if ( weaponAimSelectCheckBox[i].isSelected() == true ) 
         tUnit.weapons.weapon[weaponLastIndex].targ |= (1 << i);
        else tUnit.weapons.weapon[weaponLastIndex].targ &= ~(1 << i);
       
        if ( weaponSourceSelectCheckBox[i].isSelected() == true ) 
         tUnit.weapons.weapon[weaponLastIndex].sourceHeight |= (1 << i);
        else tUnit.weapons.weapon[weaponLastIndex].sourceHeight &= ~(1 << i);
      }  
    }
    
   for (int i=0; i < cWeaponType.length;i++) {
      if ( (tUnit.weapons.weapon[index].typ & (1 << i)) > 0 ) 
       weaponTypeSelectCheckBox[i].setSelected(true);      
      else weaponTypeSelectCheckBox[i].setSelected(false);      
    }
  for (int i=0; i < cHeightLevel.length;i++) {
      if ( (tUnit.weapons.weapon[index].targ & (1 << i)) > 0 ) 
       weaponAimSelectCheckBox[i].setSelected(true);      
      else weaponAimSelectCheckBox[i].setSelected(false);      
      
      if ( (tUnit.weapons.weapon[index].sourceHeight & (1 << i)) > 0 ) 
       weaponSourceSelectCheckBox[i].setSelected(true);      
      else weaponSourceSelectCheckBox[i].setSelected(false);      
    }
    
    jIntFieldMinDistance.setInt(tUnit.weapons.weapon[index].minDistance);
    jIntFieldMaxDistance.setInt(tUnit.weapons.weapon[index].maxDistance);
    jIntFieldMinDistanceStrength.setInt(tUnit.weapons.weapon[index].minStrength);
    jIntFieldMaxDistanceStrength.setInt(tUnit.weapons.weapon[index].maxStrength);
    jIntFieldAmmo.setInt(tUnit.weapons.weapon[index].count);
    
    for (int i=0; i < 13;i++) {      
      jIntFieldEfficiencies[i].setInt(tUnit.weapons.weapon[index].efficiency[i]);
    }
    weaponLastIndex = index;
}

private void weaponMaxDistanceCheck() { //Load-Panel
  int maxDistance = jIntFieldMaxDistance.getInt();
  if (jIntFieldMaxDistance.convertError != 0) return;    
  jTextFieldWeaponMinDistance.setBorder (new javax.swing.border.TitledBorder(
    new javax.swing.border.EtchedBorder(), "Distance.min (0-".concat(new java.lang.Integer(maxDistance)
    .toString()).concat(")"), 1, 2,
    new java.awt.Font ("Arial", 0, 10)));  
  jIntFieldMinDistance.setMax(maxDistance);
}

  
private void checkClassButtonsAndIndex() { //Class-Panel
  if (tUnit.classnum == 0) {
    jButtonSetClassStandard.setEnabled(false);
    jButtonSetClassName.setEnabled(false);
    jButtonDeleteClass.setEnabled(false);
    jListClasses.setSelectedIndex(0);
  }
  else {
    jButtonSetClassStandard.setEnabled(true);
    jButtonSetClassName.setEnabled(true);
    jButtonDeleteClass.setEnabled(true);
  }    
  if (tUnit.classnum == 8) jButtonNewClass.setEnabled(false);
  else jButtonNewClass.setEnabled(true);
  
  if (jListClasses.getSelectedIndex() > tUnit.classnum) 
    jListClasses.setSelectedIndex(tUnit.classnum);  
  }
  
private void showClassValues(int selection) { //Class-Panel
 jTextFieldClassName.setText(tUnit.classnames[selection]);
 for(int i = 0;i < tUnit.classbound[selection].weaponcount;i++) 
   jIntFieldClassWeaponImprovement[i].setInt(tUnit.classbound[selection].weapstrength[i]);
  jIntFieldClassWeaponImprovement[tUnit.classbound[selection].weaponcount]
    .setInt(tUnit.classbound[selection].armor);
  
  for (int i=0; i < cVehicleFunctions.length;i++) {      
    if ( (tUnit.classbound[selection].vehiclefunctions & (1 << i)) > 0 ) 
      classFunctionSelectCheckBox[i].setSelected(true);
    else classFunctionSelectCheckBox[i].setSelected(false);
  }
  
  for(int i = 0;i < 4;i++) jIntFieldClassTechnologyID[i]
    .setInt(tUnit.classbound[selection].techrequired[i]);
  technologyIDCheck();  
  
  jIntFieldClassTechLevel.setInt(tUnit.classbound[selection].techlevel);
}
private void setClassValues(int val, int selection) { //Class-Panel
   // setzt die Classvalues von selection auf Standard oder akt. Boxen
   // ORIGINAL for original values, STANDARD for standard vals    
    if (val == CLASS_ORIGINAL) {
      tUnit.classbound[selection].vehiclefunctions = 0;
      for (int i=0; i < cVehicleFunctions.length;i++) {              
        if ( classFunctionSelectCheckBox[i].isSelected() == true )                    
          tUnit.classbound[selection].vehiclefunctions |= (1 << i);        
      }
      for(int i = 0;i < 4;i++) 
        tUnit.classbound[selection].techrequired[i] = jIntFieldClassTechnologyID[i].getInt();
      for (int i=0; i< tUnit.classbound[selection].weaponcount ;i++ ) 
        tUnit.classbound[selection].weapstrength[i] = jIntFieldClassWeaponImprovement[i].getInt();
      tUnit.classbound[selection].armor = 
        jIntFieldClassWeaponImprovement[tUnit.classbound[selection].weaponcount].getInt();
      tUnit.classbound[selection].techlevel =jIntFieldClassTechLevel.getInt();  
    } else {      
      for (int i=0; i< tUnit.classbound[selection].weaponcount ;i++ ) 
        tUnit.classbound[selection].weapstrength[i] = 1024;
      tUnit.classbound[selection].armor = 1024;
      for (int i=0; i< 4; i++) 
        tUnit.classbound[selection].techrequired[i] = 0;
      tUnit.classbound[selection].eventrequired = 0;
      tUnit.classbound[selection].techlevel = 0;
      tUnit.classbound[selection].vehiclefunctions = 0;
    }  
  }  
  
private int setVariablesForSave() {
  int i = 0;
  
  // Main-Panel
  tUnit.name = jTxtFieldName.getText();
  tUnit.description = jTxtFieldDescription.getText();
  tUnit.id = jIntFieldID.getInt();
    if (jIntFieldID.convertError != 0) i++;
  tUnit.armor = jIntFieldArmor.getInt();
    if (jIntFieldArmor.convertError != 0) i++;
  tUnit.weight = jIntFieldWeight.getInt();
    if (jIntFieldWeight.convertError != 0) i++;
  tUnit.energy = jIntFieldEnergyTank.getInt();
    if (jIntFieldEnergyTank.convertError != 0) i++;
  tUnit.material = jIntFieldMaterialTank.getInt();
    if (jIntFieldMaterialTank.convertError != 0) i++;
  tUnit.production.energy = jIntFieldProductionEnergy.getInt();
    if (jIntFieldProductionEnergy.convertError != 0) i++;
  tUnit.production.material = jIntFieldProductionMaterial.getInt();
    if (jIntFieldProductionMaterial.convertError != 0) i++;
  tUnit.view = jIntFieldView.getInt();
    if (jIntFieldView.convertError != 0) i++;
  tUnit.jamming = jIntFieldJamming.getInt();
    if (jIntFieldJamming.convertError != 0) i++;
    
  // Movement-Panel  
  tUnit.height = 0;  
  for (int j=0; j < cHeightLevel.length;j++) {    
    if ( movementTableCheckBox[j].isSelected() == true ) {
      tUnit.height |= (1 << j);
      tUnit.movement[j] = jIntFieldMovementTable[j].getInt();
    } else tUnit.movement[j] = 0;    
  }  
  
  tUnit.movemalustyp = jComboBoxCategory.getSelectedIndex();
  
  if (jCheckBoxWaitAfterMove.isSelected() == true ) tUnit.wait = 1;
  else tUnit.wait = 0;
  
  tUnit.fuelconsumption = jIntFieldFuelconsumption.getInt();
    if (jIntFieldFuelconsumption.convertError != 0) i++;    
  tUnit.tank = jIntFieldTank.getInt();
    if (jIntFieldTank.convertError != 0) i++;    
  tUnit.steigung = jIntFieldDistanceHeightChange.getInt();
    if (jIntFieldDistanceHeightChange.convertError != 0) i++;
  tUnit.maxwindspeedonwater = jIntFieldMaxWindspeed.getInt();  
    if (jIntFieldMaxWindspeed.convertError != 0) i++;
   
  //Load-Panel
        
  tUnit.loadcapacity = jIntFieldMaxLoad.getInt(); 
    if (jIntFieldMaxLoad.convertError != 0) i++;
  tUnit.maxunitweight = jIntFieldMaxUnitWeight.getInt(); 
    if (jIntFieldMaxUnitWeight.convertError != 0) i++;
 
  tUnit.loadcapability = 0;  
  tUnit.loadcapabilityreq = 0;  
  tUnit.loadcapabilitynot = 0;  
  for (int j=0; j < cHeightLevel.length;j++) {    
    if ( loadOnHeightTableCheckBox[j].isSelected() == true ) 
      tUnit.loadcapability |= (1 << j);
    if ( loadAbleHeightTableCheckBox[j].isSelected() == true ) 
      tUnit.loadcapabilityreq |= (1 << j);
    if ( loadNotHeightTableCheckBox[j].isSelected() == true ) 
      tUnit.loadcapabilitynot |= (1 << j);
  }  
  
  //Functions-Panel
  
  for (int j=0; j < cVehicleFunctions.length;j++) {
    if ( functionSelectCheckBox[i].isSelected() == true ) 
      tUnit.functions |= (1 << j);
  }  
  tUnit.digrange = jIntFieldResourceRadius.getInt(); 
  
  //Class-Panel
  //Values will be directly written into tUnit ! except last values with ->
  setClassValues(CLASS_ORIGINAL,classLastIndex);
  
  //Weapon-Panel
  //Values will be directly written into tUnit ! ; except last values with ->
  showWeaponValues(weaponLastIndex,WEAPON_SAVE);
  
  return i;
}

private void technologyIDCheck(){ //Class-Panel
  int enabled = 0;
  // errMsg.showMessage("Technology Handler Accessed !");
  for(int i=1; i < 4;i++) {
    if ( (jIntFieldClassTechnologyID[i-1].getInt() > 0) && (enabled == 0) )
      classTechnologyIDTextField[i].setEnabled(true);
    else { 
      classTechnologyIDTextField[i].setEnabled(false);
      enabled++;
    }  
  }    
}


private void digCheck() {//Functions-Panel
  int j = 0;
  for (int i=0; i < cVehicleFunctions.length;i++)
    if (((1 << i) & (cfautodigger | cfmanualdigger)) > 0) 
      if (functionSelectCheckBox[i].isSelected() == true) j++;
  if ( j > 0 ) jTextFieldResourceRadius.setEnabled(true);
  else  jTextFieldResourceRadius.setEnabled(false);
}  

private void maxLoadCheck() { //Load-Panel
  int maxLoad = jIntFieldMaxLoad.getInt();
  if (jIntFieldMaxLoad.convertError != 0) return;
  if (maxLoad > 0) {
    jTextFieldMaxUnitWeight.setEnabled(true);
    jTextFieldMaxUnitWeight.setBorder (new javax.swing.border
      .TitledBorder(new javax.swing.border.EtchedBorder(), 
      "Maximum weight of unit (0-".concat(new java.lang.Integer(maxLoad)
      .toString()).concat(")"), 1, 2, new java.awt.Font ("Arial", 0, 10)));
    jIntFieldMaxUnitWeight.setMax(maxLoad);
    for (int j=0; j < cHeightLevel.length;j++) {      
      loadOnHeightTableCheckBox[j].setEnabled(true);
      loadAbleHeightTableCheckBox[j].setEnabled(true);
      loadNotHeightTableCheckBox[j].setEnabled(true);
    }  
  } else {
    jTextFieldMaxUnitWeight.setEnabled(false);    
    for (int j=0; j < cHeightLevel.length;j++) {      
      loadOnHeightTableCheckBox[j].setEnabled(false);
      loadAbleHeightTableCheckBox[j].setEnabled(false);
      loadNotHeightTableCheckBox[j].setEnabled(false);
    }  
  }
}

private int borderCheck(int i) { //Movement-Panel
  int j = 0;
  
  if ( (i > 0) && ( movementTableCheckBox[i-1].isSelected() == true) ) j++;
  if ( (i < cHeightLevel.length-1) && 
    ( movementTableCheckBox[i+1].isSelected() == true) ) j++;
  
  int l = 0;
  for(int k = 0;k < cHeightLevel.length;k++) {
    if ( movementTableCheckBox[k].isSelected() == true ) l++;
  }
  if (l <= 1) return 1; 
    //Nothing selected (one was just selected) => 1 to select at least one
  
  if (j > 0) return j;
  else {
    errMsg.showMessage("No gaps allowed between height-levels !");
    return 0;      
  }  
}  

private boolean heightChangeCheck() { //Movement-Panel
  int l = 0;
  for(int k = 0;k < cHeightLevel.length;k++) {
    if ( movementTableCheckBox[k].isSelected() == true ) l++;
  }
  
  if ((movementTableCheckBox[2].isSelected() == true) || (movementTableCheckBox[3].isSelected() == true)) 
    jTextFieldMaxWindspeed.setEnabled(true);
  else jTextFieldMaxWindspeed.setEnabled(false);    
  //MaxwindspeedCheck : Filed only avail when driving or floating
  
  if (l > 1) {
    jTextFieldDistanceHeightChange.setEnabled(true);
    return true; //Minimum 2 selcted to change height
  } else {
    jTextFieldDistanceHeightChange.setEnabled(false);
    return false;
  }   
}  

private void movementTableActionPerformed(java.awt.event.ActionEvent evt) { //Movement-Panel
    javax.swing.JCheckBox CB = (javax.swing.JCheckBox) evt.getSource();
    String boxNumberString = CB.getName();
    int boxNumber = 0;
     try {
      boxNumber = new java.lang.Integer(boxNumberString).parseInt(boxNumberString);   
    } catch (java.lang.NumberFormatException e) {
      errorMessage = e.getMessage();      
      errMsg.showMessage("Internal Error No 1");
      return;
    }   
    
    int i = borderCheck(boxNumber);
    if ( movementTableCheckBox[boxNumber].isSelected() == true ) {
      if (i > 0) movementTableTextField[boxNumber].setEnabled(true);
      else movementTableCheckBox[boxNumber].setSelected(false);
    }  
    else {
      if (i == 2 ) {
        errMsg.showMessage("No gaps allowed between height-levels !");
        movementTableCheckBox[boxNumber].setSelected(true);
      }  
      else movementTableTextField[boxNumber].setEnabled(false);
    }  
    heightChangeCheck();
  }  
  
  /** Closes the dialog */
  private void closeDialog(java.awt.event.WindowEvent evt) {//GEN-FIRST:event_closeDialog
    if (exitAction == DISPOSE ) {
      setVisible (false);
      dispose ();
    } else System.exit (0);
  }//GEN-LAST:event_closeDialog

  /**
  * @param args the command line arguments
  */
  public static void main (String args[]) {    
    new UnitEditor ("L:\\Java\\MyJava\\Asc\\","S_submar.veh",
      new Unit("").LOAD,EXIT).show ();
  }


  // Variables declaration - do not modify//GEN-BEGIN:variables
  private javax.swing.JTabbedPane jTabbedPaneMain;
  private javax.swing.JPanel jPanelClass;
  private javax.swing.JPanel jPanelClassFunctionSelect;
  private javax.swing.JList jListClasses;
  private javax.swing.JTextField jTextFieldClassName;
  private javax.swing.JButton jButtonNewClass;
  private javax.swing.JButton jButtonSetClassName;
  private javax.swing.JButton jButtonDeleteClass;
  private javax.swing.JButton jButtonSetClassStandard;
  private javax.swing.JPanel jPanelClassWeaponImprovement;
  private javax.swing.JPanel jPanelClassTechnologyIDs;
  private javax.swing.JTextField jTextFieldTechLevel;
  private javax.swing.JPanel jPanelFunctions;
  private javax.swing.JPanel jPanelFunctionSelect;
  private javax.swing.JTextField jTextFieldResourceRadius;
  private javax.swing.JPanel jPanelMovement;
  private javax.swing.JPanel jPanelMovementTable;
  private javax.swing.JComboBox jComboBoxCategory;
  private javax.swing.JCheckBox jCheckBoxWaitAfterMove;
  private javax.swing.JTextField jTextFieldFuelconsumption;
  private javax.swing.JTextField jTextFieldTank;
  private javax.swing.JLabel jLabelFuelconsumptionTankBorder;
  private javax.swing.JTextField jTextFieldDistanceHeightChange;
  private javax.swing.JTextField jTextFieldMaxWindspeed;
  private javax.swing.JLabel jLabelMaxWindspeed;
  private javax.swing.JLabel jLabelWindspeedBorder;
  private javax.swing.JPanel jPanelMain;
  private javax.swing.JTextField jTxtFieldName;
  private javax.swing.JTextField jTxtFieldDescription;
  private javax.swing.JTextField jTextFieldID;
  private javax.swing.JTextField jTextFieldArmor;
  private javax.swing.JTextField jTextFieldProductionMaterial;
  private javax.swing.JTextField jTextFieldProductionEnergy;
  private javax.swing.JTextField jTextFieldWeight;
  private javax.swing.JTextField jTextFieldView;
  private javax.swing.JTextField jTextFieldJamming;
  private javax.swing.JTextField jTextFieldEnergyTank;
  private javax.swing.JTextField jTextFieldMaterialTank;
  private javax.swing.JLabel jLabelProductionBorder;
  private javax.swing.JLabel jLabelNameDescriptionBorder;
  private javax.swing.JLabel jLabelIDArmorWeightBorder;
  private javax.swing.JLabel jLabelViewJammingBorder;
  private javax.swing.JLabel jLabelEnergyMaterialTankBorder;
  private javax.swing.JLabel jLabelEnergyTankWarning;
  private javax.swing.JPanel jPanelInfoText;
  private javax.swing.JScrollPane jScrollPaneInfoText;
  private javax.swing.JTextPane jTextPaneInfoText;
  private javax.swing.JPanel jPanelLoad;
  private javax.swing.JTextField jTextFieldMaxLoad;
  private javax.swing.JTextField jTextFieldMaxUnitWeight;
  private javax.swing.JPanel jPanelLoadTable;
  private javax.swing.JPanel jPanelLoadTabelTitle;
  private javax.swing.JLabel jLabelOnHeight;
  private javax.swing.JLabel jLabelAbleHeight;
  private javax.swing.JLabel jLabelNotHeight;
  private javax.swing.JPanel jPanelWeapons;
  private javax.swing.JComboBox jComboBoxWeapons;
  private javax.swing.JPanel jPanelEfficiencies;
  private javax.swing.JPanel jPanelWeaponAim;
  private javax.swing.JPanel jPanelWeaponSource;
  private javax.swing.JPanel jPanelWeaponType;
  private javax.swing.JTextField jTextFieldWeaponMinDistance;
  private javax.swing.JTextField jTextFieldWeaponMaxDistance;
  private javax.swing.JTextField jTextFieldWeaponMinStrength;
  private javax.swing.JTextField jTextFieldWeaponMaxStrength;
  private javax.swing.JTextField jTextFieldWeaponAmmo;
  private javax.swing.JButton jButtonAddWeapon;
  private javax.swing.JButton jButtonRemoveWeapon;
  private javax.swing.JLabel jLabelErrorMessage;
  // End of variables declaration//GEN-END:variables

  static String cHeightLevel[] = {"deep submerged", "submerged", "floating", 
    "ground level", "low-level flight", "flight", "high-level flight", "orbit"}; 
  static String cMovemaliType[] = { "default", "light tracked vehicle",
    "medium tracked vehicle", "heavy tracked vehicle", "light wheeled vehicle", 
    "medium wheeled vehicle", "heavy wheeled vehicle", "trooper", "rail vehicle"};  
  static String cVehicleFunctions[] = {"sonar", "paratrooper", "mine-layer", "trooper",
    "repair vehicle", "conquer buildings", "move after attack","view satellites",
    "construct ALL buildings", "view mines", "construct vehicles","construct specific buildings",
    "refuel units", "icebreaker", "!", "refuels material", "!", "makes tracks",
    "drill for mineral resources manually", "sailing", "auto repair", "generator",
    "search for mineral resources automatically", "Kamikaze only"  };
  static String cWeaponType[] = {"cruise missile", "mine", "bomb", "air - missile", 
    "ground - missile", "torpedo", "machine gun", "cannon", "service", "ammunition refuel",
    "laser (not implemented yet!)", "shootable"};
  
   static int cfsonar = 1;
   static int cfparatrooper = 2;
   static int cfminenleger = 4;
   static int cf_trooper = 8;
   static int cfrepair = 16;
   static int cf_conquer = 32;
   static int cf_moveafterattack = 64;
   static int cfsatellitenview = 128;
   static int cfputbuilding = 256;
   static int cfmineview = 512;
   static int cfvehicleconstruction = 1024;
   static int cfspecificbuildingconstruction = 2048;
   static int cffuelref = 4096;
   static int cficebreaker = 8192;
   static int cfmaterialref = 32768;
   // static int cfenergyref 65536  
   static int cffahrspur = ( 1 << 17 );   /*  !!  */
   static int cfmanualdigger = ( 1 << 18 );
   static int cfwindantrieb = ( 1 << 19 );
   static int cfautorepair = ( 1 << 20 );
   static int cfgenerator = ( 1 << 21 );
   static int cfautodigger = ( 1 << 22 );
   static int cfkamikaze = ( 1 << 23 );  
  
   static int cwcruisemissilen = 0;
   static int cwcruisemissileb = ( 1 << cwcruisemissilen );
   static int cwminen = 1;
   static int cwmineb = ( 1 << cwminen   );
   static int cwbombn = 2;
   static int cwbombb = ( 1 << cwbombn  );
   static int cwairmissilen = 3;
   static int cwairmissileb = ( 1 << cwairmissilen  );
   static int cwgroundmissilen = 4;
   static int cwgroundmissileb = ( 1 << cwgroundmissilen  );
   static int cwtorpedon = 5;
   static int cwtorpedob = ( 1 << cwtorpedon  );
   static int cwmachinegunn = 6;
   static int cwmachinegunb = ( 1 << cwmachinegunn );
   static int cwcannonn = 7;
   static int cwcannonb = ( 1 << cwcannonn );
   static int cwweapon = ( cwcruisemissileb | cwbombb | cwairmissileb 
    | cwgroundmissileb | cwtorpedob | cwmachinegunb | cwcannonb );
   static int cwshootablen = 11;
   static int cwshootableb = ( 1 << cwshootablen  );
   static int cwammunitionn = 9;
   static int cwammunitionb = ( 1 << cwammunitionn );
   static int cwservicen = 8;
   static int cwserviceb = ( 1 << cwservicen );
  
   static int chtiefgetaucht = 1;
   static int chgetaucht = 2;
   static int chschwimmend = 4;
   static int chfahrend = 8;
   static int chtieffliegend = 16;
   static int chfliegend = 32;
   static int chhochfliegend = 64;
   static int chsatellit = 128;
  }  