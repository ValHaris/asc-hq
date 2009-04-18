//     $Id: SingleWeapon.java,v 1.4 2009-04-18 13:48:40 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.3  2000/10/17 17:28:26  schelli
//     minor bugs fixed in lots of sources
//     add & remove weapon works now
//     revert to save button removed
//     class-handling bugs fixed
//     load & save routines fully implemented
//     terrainacces added
//
//     Revision 1.2  2000/10/13 13:15:47  schelli
//     Load&Save routines finished
//

/*
 * SingleWeapon.java
 *
 * Created on 5. Juni 2000, 13:40
  
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
  int          targets_not_hittable;  

  /** Creates new SingleWeapon */
  public SingleWeapon() {
    efficiency = new int[13]; 
    for (int i=0;i<13; i++) efficiency[i] = 100;   
    sourceHeight = 1;
    targ = 1;
    typ = 1;
  }
  
}