/*
 * UnitWeapon.java
 *
 * Created on 5. Juni 2000, 13:37
 */
 


/** 
 *
 * @author  MS
 * @version 
 */
public class UnitWeapon extends Object {
  int count;
  SingleWeapon weapon[]; //16
  int reserved[]; //10

  /** Creates new UnitWeapon */
  public UnitWeapon() {
    weapon = new SingleWeapon[16];  
    reserved = new int[10];
  }
  
}