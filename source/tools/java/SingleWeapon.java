/*
 * SingleWeapon.java
 *
 * Created on 5. Juni 2000, 13:40
 */
 


/** 
 *
 * @author  MS
 * @version 
 */
public class SingleWeapon extends Object {
  int          typ;            /*  BM      <= CWaffentypen  */
  int          targ;           /*  BM      <= CHoehenstufen  */
  int          sourceHeight;   /*  BM  "  */
  int          maxDistance;
  int          minDistance;
  int          count;
  int          maxStrength;    // Wenn der Waffentyp == Mine ist, dann ist hier die Minenst„rke als Produkt mit der Bassi 64 abgelegt.
  int          minStrength;
  int          efficiency[]; // 13 floating and driving are the same ; 0-5 is lower ; 6 is same height ; 7-12 is higher
  int          reserved[]; // 10

  /** Creates new SingleWeapon */
  public SingleWeapon() {
    efficiency = new int[13];
    reserved = new int[10];
  }
  
}