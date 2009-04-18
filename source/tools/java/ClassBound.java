//     $Id: ClassBound.java,v 1.5 2009-04-18 13:48:40 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.4  2000/11/07 16:19:39  schelli
//     Minor Memory-Functions & Problems changed
//     Picture Support partly enabled
//     New Funktion partly implemented
//     Buildings partly implemented
//
//     Revision 1.3  2000/10/17 17:28:26  schelli
//     minor bugs fixed in lots of sources
//     add & remove weapon works now
//     revert to save button removed
//     class-handling bugs fixed
//     load & save routines fully implemented
//     terrainacces added
//
//     Revision 1.2  2000/10/13 13:15:46  schelli
//     Load&Save routines finished
//

/*
 * ClassBound.java
 *
 * Created on 22. November 1999, 14:09
 
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

public class ClassBound {
  
     static int weaponcount = 8;
         
     int          weapstrength[]; //w     
     int          armor;  //w
     int          techlevel; //w  Techlevel ist eine ALTERNATIVE zu ( techrequired und envetrequired )
     int          techrequired[]; //w
     int          eventrequired; //c
     int          vehiclefunctions;   
    
  /** Creates new tClassBound */
  public ClassBound() {
    weapstrength = new int[weaponcount];
    techrequired = new int[4];
    armor = 0;
    techlevel = 0;
    eventrequired = 0;
    vehiclefunctions = 0;
  }
  
}