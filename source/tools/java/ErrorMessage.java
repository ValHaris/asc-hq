/*
 * ErrorMessage.java
 *
 * Created on 28. November 1999, 21:35
 */
 


/** 
 *
 * @author  MS
 * @version 
 */
public class ErrorMessage extends Object {
  
  private javax.swing.JLabel msgWindow;
  private javax.swing.Timer ErrorMsgTimer;
  private int defaultDelay;
  
  public String msgString;

  /** Creates new ErrorMessage */
  public ErrorMessage(javax.swing.JLabel errorMsgWindow, int delay) {
    msgWindow = errorMsgWindow;
    defaultDelay = delay;
    ErrorMsgTimer = new javax.swing.Timer(delay, new java.awt.event.ActionListener () {
      public void actionPerformed (java.awt.event.ActionEvent evt) {
        ErrorMsgTimerActionPerformed (evt);
      }
    }
    );
    ErrorMsgTimer.setRepeats(false);    
  }
  
  /** Creates new ErrorMessage */
  public ErrorMessage(javax.swing.JLabel errorMsgWindow) {
    msgWindow = errorMsgWindow;
    ErrorMsgTimer = new javax.swing.Timer(3000, new java.awt.event.ActionListener () {
      public void actionPerformed (java.awt.event.ActionEvent evt) {
        ErrorMsgTimerActionPerformed (evt);
      }
    }
    );
    ErrorMsgTimer.setRepeats(false);    
  }
  
  public void setDelay(int tdelay) {
    ErrorMsgTimer.setDelay(tdelay);    
  }
  
  public void showMessage(String msg, int tdelay) {
    msgWindow.setText("  ".concat(msg));
    ErrorMsgTimer.setDelay(tdelay);    
    ErrorMsgTimer.restart();
    ErrorMsgTimer.setDelay(defaultDelay);    
  }
  
  public void showMessage(String msg) {
    msgWindow.setText("  ".concat(msg));
    ErrorMsgTimer.restart();
  }
  
  private void ErrorMsgTimerActionPerformed (java.awt.event.ActionEvent evt) {
      showMessage("");
  }  
}