/*
 * ClassBound.java
 *
 * Created on 22. November 1999, 14:09
 */
 


/** 
 *
 * @author  MS
 * @version 
 */
public class ClassBound {
  
     static int weaponcount = 8;
         
     int          weapstrength[]; //w
     int          dummy2; //w
     int          armor;  //w
     int          techlevel; //w  Techlevel ist eine ALTERNATIVE zu ( techrequired und envetrequired )
     int          techrequired[]; //w
     int          eventrequired; //c
     int          vehiclefunctions;
     int          dummy;//c
    
  /** Creates new tClassBound */
  public ClassBound() {
    weapstrength = new int[weaponcount];
    techrequired = new int[4];
  }
  
}