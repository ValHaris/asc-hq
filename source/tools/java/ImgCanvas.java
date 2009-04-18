//     $Id: ImgCanvas.java,v 1.2 2009-04-18 13:48:40 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.1  2000/11/07 16:19:39  schelli
//     Minor Memory-Functions & Problems changed
//     Picture Support partly enabled
//     New Funktion partly implemented
//     Buildings partly implemented
//
//     Revision 1.8  2000/11/01 11:41:04  mbickel
//      Fixed: Selection in MainWindow-List not being evaluated, the first list
//       entry was always edited.
//
//

/*
 * ImgCanvas.java
 *
 * Created on 6. November 2000, 19:32

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

public class ImgCanvas extends java.awt.Canvas {
    Image img;
    public PaletteInfo palette;
    ColorModel cModel;

    public void paint(Graphics g) {
        int w = getSize().width;
        int h = getSize().height;
        if (img == null) {
            super.paint(g);
            g.setColor(Color.black);
        } else {
            g.drawImage(img, 0, 0, w, h, this);
        }
    }

    public void update(Graphics g) {
        paint(g);
    }

    public Dimension getMinimumSize() {
        return new Dimension(20, 20);
    }

    public Dimension getPreferredSize() {
        return new Dimension(100, 100);
    }

    public Image getImage() {
        return img;
    }
    
    Image calculateImage() {

        int w = 100;
        int h = 100;
        int pix[] = new int[w * h];
        int index = 0;
        for (int y = 0; y < h; y++) {
            int red = (y * 255) / (h - 1);
            for (int x = 0; x < w; x++) {
                int blue = (x * 255) / (w - 1);
                pix[index++] = (255 << 24) | (red << 16) | blue;
            }
        }

        return createImage(new MemoryImageSource(w, h, pix, 0, w));
    }

    public void setImage(int w,int h,byte[] pix) {        
        
        int[] pixtst;        
        pixtst = new int[pix.length];        
        for (int i=0;i<pix.length;i++) pixtst[i] = (int) pix[i];                
        this.img = createImage(new MemoryImageSource(w, h, pixtst, 0, w));
        
        cModel = new IndexColorModel(palette.bits,palette.size,palette.r,palette.g,palette.b);        
        //this.img = createImage(new MemoryImageSource(w, h, cModel, pix, 0, w));       
        
        
    }

    public int showImage() {
        Graphics g = getGraphics();
        try {
            paint(g);
        } finally {
            if (g != null) {
                g.dispose();
                return 0;
            }
        }
        return 1;
    }
}