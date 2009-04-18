//     $Id: MakeCheckIntRangeField.java,v 1.4 2009-04-18 13:48:40 mbickel Exp $
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
//     Revision 1.2  2000/10/13 13:15:46  schelli
//     Load&Save routines finished
//

/*
 * MakeCheckIntRangeField.java
 *
 * Created on 29. November 1999, 14:31
  
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


public class MakeCheckIntRangeField {
  
  public int convertError = 0;
  
  private javax.swing.JTextField txtField;
  private ErrorMessage errMsg;
  private int min;
  private int max;  
  
  private String lastValidValue;  

  /** Creates new MakeCheckIntRange */
  public MakeCheckIntRangeField(javax.swing.JTextField textField,ErrorMessage errMessage,int minInt,int maxInt) {
    txtField = textField;
    errMsg = errMessage;
    min = minInt;
    max = maxInt;
    
    lastValidValue = txtField.getText();
    
/*    txtField.addInputMethodListener (new java.awt.event.InputMethodListener () {
      public void caretPositionChanged (java.awt.event.InputMethodEvent evt) {

      }
      public void inputMethodTextChanged (java.awt.event.InputMethodEvent evt) {
        actionHandler (evt);
      }
    }
    );*/
    
    txtField.addActionListener (new java.awt.event.ActionListener () {
      public void actionPerformed (java.awt.event.ActionEvent evt) {
        actionHandler ();
      }
    }
    );
    txtField.addFocusListener (new java.awt.event.FocusAdapter () {
      public void focusLost (java.awt.event.FocusEvent evt) {
        actionHandler ();
      }
    }
    );
  }
  
  public int getInt() {
    int i;
    
    convertError = 0;
    try {
      i = new java.lang.Integer(lastValidValue).parseInt(lastValidValue);
      return i;
    } catch (java.lang.NumberFormatException e) {
      errMsg.msgString = e.getMessage();      
      errMsg.showMessage("Invalid numerical format or number greater than Integer ! (value could not be set)");     
      convertError = 1;
      return 0; // Fehler -> kein Integer
    }    
  }
  
  public void setMin(int val) {
     min = val;
  }
  
  public void setMax(int val) {
     max = val;
  }
    
  public void setInt(int val) {
     lastValidValue = new java.lang.Integer(val).toString();
     txtField.setText (lastValidValue);
  }
  
  private void actionHandler(/*java.awt.event.InputMethodEvent evt*/) {
    String id;
    
    id =  txtField.getText ();    
    /*( (javax.swing.JTextField) evt.getSource() ).*/
    if ( checkIntRange(id,min,max) != 0 ) txtField.setText (lastValidValue);      
    else lastValidValue = id;    
  }
  
  
  private int checkIntRange(String s, int min,int max) {
    int i;
    String err;
    
    try {
      i = new java.lang.Integer(s).parseInt(s);
    } catch (java.lang.NumberFormatException e) {
      errMsg.msgString = e.getMessage();      
      errMsg.showMessage("Invalid numerical format or number greater than Integer !");     
      return 1; // Fehler -> kein Integer
    }
    if ( (i >= min) && (i <= max) ) {      
      return 0; // Alles Ok
    } else {      
      err = "Number must be from ";
      err = err.concat(new java.lang.Integer(min).toString()) .concat(" to ")
            .concat(new java.lang.Integer(max).toString()) .concat(" !");      
      errMsg.showMessage(err);
      return 2; // Fehler -> aus dem Limit
    }  
  }
}