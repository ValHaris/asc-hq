//     $Id: ImageProcess.java,v 1.1 2000-11-07 16:19:39 schelli Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.8  2000/11/01 11:41:04  mbickel
//      Fixed: Selection in MainWindow-List not being evaluated, the first list
//       entry was always edited.
//
//

/*
 * ImageProcess.java
 *
 * Created on 5. November 2000, 13:03

This file is part of Advanced Strategic Command; http://www.asc-hq.de
Copyright (C) 1994-2000  Martin Bickel  and  Marc Schellenberger

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING. If not, write to the
Free Software Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
 */

import java.awt.*;
import java.awt.image.*;

public class ImageProcess extends Object {

    ImgCanvas canvas;
    RLEPackage rPack;
    byte pix[];
    int w,h;

    public byte[] uncompress() {       
        
        if (rPack.head.id == 16973) {
            
            byte pixels[];

            int indexPic = 0;
            int indexDest = 0;

            pixels = new byte[w * h];

            for ( int c = 0; c < rPack.head.size; c++ ) {
                if ( rPack.b[indexPic] == rPack.head.rle ) {
                    for ( int i = rPack.b[indexPic+1]; i > 0; i-- ) {
                        pixels[indexDest] = rPack.b[indexPic+2];
                        indexDest++;
                    }
                    indexPic += 3;
                    c += 2;
                } else {
                    pixels[indexDest] = rPack.b[indexPic];
                    indexPic++;
                    indexDest++;
                }
            }
            return pixels;
        }
        else {            
            /*for (int i=0;i<20;i++) {
                rPack.b[i]=(byte)i;
            }*/
            
            return rPack.b;
        }
    }
    
    public int showImage() {
        return canvas.showImage();
    }


    /** Creates new ImageProcess */
    public ImageProcess(RLEPackage rPackage,ImgCanvas cnvs) {
        rPack = rPackage;
        canvas = cnvs;
        
        w = rPack.head.x +1;
        h = rPack.head.y +1;
        pix = uncompress();
        
        canvas.setImage(w,h,pix);
    }    
}