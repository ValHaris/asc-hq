/*
 * SgStream.java
 *
 * Created on 21. November 1999, 15:03
 */
 


/** 
 *
 * @author  MS
 * @version 
 */

public class SgStream {

  public static byte STREAM_READ = 0;
  public static byte STREAM_WRITE = 1;    
  public int error;
  public String errorMessage;
  
  private byte mode;
  private java.io.FileInputStream istream;
  private java.io.FileOutputStream ostream;
  private byte[] b;
  
  public int readByte(){
    
    if (mode == STREAM_WRITE) return 0;
    
    int i = 0;
    byte b;
    
    try {
      i = istream.read();
    } catch (java.io.IOException e ) {
      errorMessage=e.getMessage();
      error = 2;
    }
    
    return i;
  }
  
  public int readWord(){
    
    if (mode == STREAM_WRITE) return 0;
    
    int i = 0;
    int bb[];
    
    b = new byte[2];
    
    try {
      istream.read(b);
    } catch (java.io.IOException e ) {
      errorMessage=e.getMessage();
      error = 2;
    }
    
    bb = new int[2];
    
    for (int j = 0; j < 2 ;j++) {
      if (b[j] < 0) {
       bb[j] = 256 + b[j];
      }  
      else bb[j] = b[j];
    }  
    
     i = (bb[1] << 8) + bb[0];
    
    return i;
  }
  
  public int readInt(){
    
    if (mode == STREAM_WRITE) return 0;
    
    int i = 0;
    int bb[];
    
    b = new byte[4];
    
    try {
      istream.read(b);
    } catch (java.io.IOException e ) {
      errorMessage=e.getMessage();
      error = 2;
    }
    
    bb = new int[4];
    
    for (int j = 0; j < 4 ;j++) {
      if (b[j] < 0) {
       bb[j] = 256 + b[j];
      }  
      else bb[j] = b[j];
    }  
    
    i = (bb[3] << 24) + (bb[2] << 16) + (bb[1] << 8) +bb[0];
    
    return i;
  }
  
    public String readPChar(){
    
    if (mode == STREAM_WRITE) return "";    
    char i = 0;
    String s ="";
    
    do {
       try {
         i = (char) istream.read();
       } catch (java.io.IOException e ) {
         errorMessage=e.getMessage();
         error = 2;
       }
      if ( i != 0 ) s += i;
    } while ( i != 0 );
        
    if (s == null) return " ";
    else return s;
  }  
  
  public RLEPackage readRlePicture() {
    
    RLEPackage pack;
    pack = new RLEPackage();
    
    if (mode == STREAM_WRITE) return pack;
    
    RLEHeader head;    
    int calculatedSize;
        
    pack.head.read(this);
    if (pack.head.id == 16973) {       
      pack.b = new byte[pack.head.size];    
      try {
        istream.read(pack.b);
      } catch (java.io.IOException e ) {
        errorMessage=e.getMessage();
        error = 2;
      }      
   } 
   else { 
      calculatedSize = (pack.head.id + 1) * (pack.head.size + 1) + 4; 
      pack.b = new byte[calculatedSize - 9 ];    // 9 = sizeOf(Head)
      try {
        istream.read(pack.b);
      } catch (java.io.IOException e ) {
        errorMessage=e.getMessage();
        error = 2;
      }                  
    } 
    return pack;
  }

  
  public void writeByte(int x){
    
    if (mode == STREAM_READ) return;
        
    byte b;
    
    try {
      ostream.write((byte) x);
    } catch (java.io.IOException e ) {
      errorMessage=e.getMessage();
      error = 2;
    }
  }
  
  public void writeWord(int x){
    
    if (mode == STREAM_READ) return;
        
    int bb[];
    
    b = new byte[2];
    bb = new int[2];
    
    for (int j = 0; j < 2 ;j++) {
      bb[j] = (x >> 8*j) & 255;
      if (bb[j] >= 128) {
       b[j] = (byte) (bb[j] - 256);
      }  
      else b[j] = (byte) bb[j];
    }     
    
    try {
      ostream.write(b);
    } catch (java.io.IOException e ) {
      errorMessage=e.getMessage();
      error = 2;
    }
  } 
  
   public void writeInt(int x){    
    
    if (mode == STREAM_READ) return;
    
    int bb[];    
    
    b = new byte[4];
    bb = new int[4];
    
    for (int j = 0; j < 4 ;j++) {
      bb[j] = (x >> 8*j) & 255;
      if (bb[j] >= 128) {
       b[j] = (byte) (bb[j] - 256);
      }  
      else b[j] = (byte) bb[j];
    }     
    
    try {
      ostream.write(b);
    } catch (java.io.IOException e ) {
      errorMessage=e.getMessage();
      error = 2;
    }
  }
  
    public void writePChar(String x){
    
    if (mode == STREAM_READ) return;       
    
    for (int j=0;j < x.length();j++) {
       try {
         ostream.write((byte) x.charAt(j));
       } catch (java.io.IOException e ) {
         errorMessage=e.getMessage();
         error = 2;
       }      
    } 
    
    byte i = 0;
    try {
      ostream.write(i);
    } catch (java.io.IOException e ) {
      errorMessage=e.getMessage();
      error = 2;
    }      
    
  }  
  
  public void writeRlePicture(RLEPackage pack) {
        
    if (mode == STREAM_READ) return;    
       
    pack.head.write(this);
    
    try {
      ostream.write(pack.b);
    } catch (java.io.IOException e ) {
      errorMessage=e.getMessage();
      error = 2;
    }      
  }  
  
  /** Creates new tSgStream */
  public SgStream(String s,byte readWrite) {
    error = 0;
    mode = readWrite;
    if (readWrite == STREAM_READ) {
      try {
         istream = new java.io.FileInputStream(s);
      } catch (java.io.FileNotFoundException e ) {
        errorMessage=e.getMessage();
        error = 1;
      }  
    } else {      
      try {
         ostream = new java.io.FileOutputStream(s);
      } catch (java.io.FileNotFoundException e ) {
        errorMessage=e.getMessage();
        error = 1;
      }  
    }
  }  
}