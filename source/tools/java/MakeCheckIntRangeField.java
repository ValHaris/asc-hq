/*
 * MakeCheckIntRangeField.java
 *
 * Created on 29. November 1999, 14:31
 */
 


/** 
 *
 * @author  MS
 * @version 
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