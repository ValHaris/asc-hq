/*
 * tWeapon.java
 *
 * Created on 22. November 1999, 14:35
 */
 


/** 
 *
 * @author  MS
 * @version 
 */
public class Weapon extends Object {
       int          typ;            /*w  BM      <= CWaffentypen  */
       int          targ;           /*c  BM      <= CHoehenstufen  */
       int          sourceheight;   /*c  BM  "  */
       int          maxdistance; //w
       int          mindistance;//w
       int          count; //c
       int          maxstrength;    //c Wenn der Waffentyp == Mine ist, dann ist hier die Minenst„rke als Produkt mit der Bassi 64 abgelegt.
       int          minstrength; //c

  /** Creates new tWeapon */
  public Weapon() {
  }
  
}