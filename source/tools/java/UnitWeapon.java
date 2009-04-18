//     $Id: UnitWeapon.java,v 1.4 2009-04-18 13:48:40 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.3  2000/10/17 17:28:28  schelli
//     minor bugs fixed in lots of sources
//     add & remove weapon works now
//     revert to save button removed
//     class-handling bugs fixed
//     load & save routines fully implemented
//     terrainacces added
//
//     Revision 1.2  2000/10/13 13:15:48  schelli
//     Load&Save routines finished
//

/*
 * UnitWeapon.java
 *
 * Created on 5. Juni 2000, 13:37
  
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

public class UnitWeapon extends Object {
  int count;
  SingleWeapon weapon[]; //16  

  /** Creates new UnitWeapon */
  public UnitWeapon() {
    weapon = new SingleWeapon[16];    
  }
  
}