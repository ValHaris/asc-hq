/*
 * RLEPackage.java
 *
 * Created on 25. Juli 2000, 18:06
 */
 


/** 
 *
 * @author  MS
 * @version 
 */
public class RLEPackage {

  public RLEHeader2 head;
  public byte b[];
  
  /** Creates new RLEPackage */
  public RLEPackage() {
    head = new RLEHeader2();
  }
  
}