/*
 * PaletteInfo.java
 *
 * Created on 7. November 2000, 15:45
 */



/**
 *
 * @author  MS
 * @version 
 */
public class PaletteInfo extends Object {
    
    int bits;
    int size;
    byte r[],g[],b[],a[];

    /** Creates new PaletteInfo */
    public PaletteInfo() {
        size = 256;
        bits = 16;
        r = new byte[size];
        g = new byte[size];
        b = new byte[size];
        a = new byte[size];
    }

}