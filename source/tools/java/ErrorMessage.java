//     $Id: ErrorMessage.java,v 1.2 2000-10-13 13:15:46 schelli Exp $
//
//     $Log: not supported by cvs2svn $

/*
 * ErrorMessage.java
 *
 * Created on 28. November 1999, 21:35
 
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