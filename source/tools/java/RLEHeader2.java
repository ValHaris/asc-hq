/*
 * RLEHeader.java
 *
 * Created on 1. Juni 2000, 18:51
 */
 


/** 
 *
 * @author  MS
 * @version 
 */
public class RLEHeader2 {
  
  int id; //w
  int size; //w
  int rle; //b
  int x; //w
  int y; //w

  /** Creates new RLEHeader */
  public RLEHeader2() {
  }
  
  public void write( SgStream stream ) {
    stream.writeWord(id);
    stream.writeWord(size);
    stream.writeByte(rle);
    stream.writeWord(x);
    stream.writeWord(y);
  }  
  
  public void read( SgStream stream ) {
    id = stream.readWord();
    size = stream.readWord();
    rle = stream.readByte();
    x = stream.readWord();
    y = stream.readWord();
  }  
  
}