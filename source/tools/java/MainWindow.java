//     $Id: MainWindow.java,v 1.4 2000-10-17 20:21:03 schelli Exp $
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
 * MainWindow.java
 *
 * Created on 7. November 1999, 01:36

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


public class MainWindow extends javax.swing.JFrame {

    String tUnitPath = ".";
    public javax.swing.JList unitFileList;
    String unitFile[];
    ErrorMessage errMsg;
    String CfgFile = "MainWindow.cfg";

    SgStream input;
    SgStream output;

    /** Creates new form JFrame */
    public MainWindow() {
        initComponents ();
        setTitle("ASC File Editor");

        input = new SgStream(CfgFile,input.STREAM_READ);
        if (input.error != 1) {
            tUnitPath = input.readPChar();
        }

        jTxtFieldUnitPath.setText (tUnitPath);

        unitFileList = new javax.swing.JList();
        unitFileList.setSelectionMode(new javax.swing.DefaultListSelectionModel().SINGLE_SELECTION);
        jScrollPaneUnitFiles.getViewport().setView(unitFileList);
        selUnitFiles();

        errMsg = new ErrorMessage(jLabelErrorMessage);

        pack ();
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the FormEditor.
     */
private void initComponents () {//GEN-BEGIN:initComponents
    jTabbedPaneMain = new javax.swing.JTabbedPane ();
    jPanelUnits = new javax.swing.JPanel ();
    jTxtFieldUnitPath = new javax.swing.JTextField ();
    jButtonEditUnit = new javax.swing.JButton ();
    jButtonBrowsePath = new javax.swing.JButton ();
    jScrollPaneUnitFiles = new javax.swing.JScrollPane ();
    jButtonNewUnit = new javax.swing.JButton ();
    jPanelTechnologies = new javax.swing.JPanel ();
    jLabelErrorMessage = new javax.swing.JLabel ();
    getContentPane ().setLayout (new org.netbeans.lib.awtextra.AbsoluteLayout ());
    addWindowListener (new java.awt.event.WindowAdapter () {
        public void windowClosing (java.awt.event.WindowEvent evt) {
            exitForm (evt);
        }
    }
    );

    jTabbedPaneMain.setPreferredSize (new java.awt.Dimension(520, 382));

      jPanelUnits.setLayout (new org.netbeans.lib.awtextra.AbsoluteLayout ());
      jPanelUnits.setPreferredSize (new java.awt.Dimension(370, 310));
  
        jTxtFieldUnitPath.setBorder (new javax.swing.border.TitledBorder(
        new javax.swing.border.EtchedBorder(java.awt.Color.white, new java.awt.Color (134, 134, 134)),
        "UnitPath", 4, 2, new java.awt.Font ("Arial", 0, 10), java.awt.Color.black));
        jTxtFieldUnitPath.setText ("c:\\");
        jTxtFieldUnitPath.addActionListener (new java.awt.event.ActionListener () {
            public void actionPerformed (java.awt.event.ActionEvent evt) {
                jTxtFieldUnitPathActionPerformed (evt);
            }
        }
        );
        jTxtFieldUnitPath.addFocusListener (new java.awt.event.FocusAdapter () {
            public void focusLost (java.awt.event.FocusEvent evt) {
                jTxtFieldUnitPathFocusLost (evt);
            }
        }
        );
    
        jPanelUnits.add (jTxtFieldUnitPath, new org.netbeans.lib.awtextra.AbsoluteConstraints (10, 250, 350, 50));
    
        jButtonEditUnit.setText ("Edit Unit");
        jButtonEditUnit.addActionListener (new java.awt.event.ActionListener () {
            public void actionPerformed (java.awt.event.ActionEvent evt) {
                jBtnUnitEditorActionPerformed (evt);
            }
        }
        );
    
        jPanelUnits.add (jButtonEditUnit, new org.netbeans.lib.awtextra.AbsoluteConstraints (380, 150, 130, 60));
    
        jButtonBrowsePath.setText ("Browse");
        jButtonBrowsePath.addActionListener (new java.awt.event.ActionListener () {
            public void actionPerformed (java.awt.event.ActionEvent evt) {
                jButtonBrowsePathActionPerformed (evt);
            }
        }
        );
    
        jPanelUnits.add (jButtonBrowsePath, new org.netbeans.lib.awtextra.AbsoluteConstraints (380, 260, 130, 30));
    
    
        jPanelUnits.add (jScrollPaneUnitFiles, new org.netbeans.lib.awtextra.AbsoluteConstraints (10, 10, 350, 230));
    
        jButtonNewUnit.setText ("New Unit");
        jButtonNewUnit.addActionListener (new java.awt.event.ActionListener () {
            public void actionPerformed (java.awt.event.ActionEvent evt) {
                jButtonNewUnitActionPerformed (evt);
            }
        }
        );
    
        jPanelUnits.add (jButtonNewUnit, new org.netbeans.lib.awtextra.AbsoluteConstraints (380, 50, 130, 60));
    
      jTabbedPaneMain.addTab ("Units", jPanelUnits);
  
      jPanelTechnologies.setLayout (new org.netbeans.lib.awtextra.AbsoluteLayout ());
  
      jTabbedPaneMain.addTab ("Technologies", jPanelTechnologies);
  

    getContentPane ().add (jTabbedPaneMain, new org.netbeans.lib.awtextra.AbsoluteConstraints (0, 0, 620, 360));

    jLabelErrorMessage.setBorder (new javax.swing.border.EtchedBorder(java.awt.Color.white, new java.awt.Color (134, 134, 134)));


    getContentPane ().add (jLabelErrorMessage, new org.netbeans.lib.awtextra.AbsoluteConstraints (0, 370, 620, 40));

}//GEN-END:initComponents

private void jButtonNewUnitActionPerformed (java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButtonNewUnitActionPerformed
    // Add your handling code here:
  }//GEN-LAST:event_jButtonNewUnitActionPerformed

private void jTxtFieldUnitPathFocusLost (java.awt.event.FocusEvent evt) {//GEN-FIRST:event_jTxtFieldUnitPathFocusLost
    // Add your handling code here:
    String path;

    path = jTxtFieldUnitPath.getText ();
    if ( checkPath(path) != 0 ) {
        jTxtFieldUnitPath.setText (tUnitPath);
        errMsg.showMessage("Invalid Path !");
    } else {
        tUnitPath = path;
        selUnitFiles();
    }
  }//GEN-LAST:event_jTxtFieldUnitPathFocusLost

private void jTxtFieldUnitPathActionPerformed (java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jTxtFieldUnitPathActionPerformed
    // Add your handling code here:
    String path;

    path = jTxtFieldUnitPath.getText ();
    if ( checkPath(path) != 0 ) {
        jTxtFieldUnitPath.setText (tUnitPath);
        errMsg.showMessage("Invalid Path !");
    } else {
        tUnitPath = path;
        selUnitFiles();
    }
  }//GEN-LAST:event_jTxtFieldUnitPathActionPerformed

private void jButtonBrowsePathActionPerformed (java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButtonBrowsePathActionPerformed
    // Add your handling code here:
    String dir;

    javax.swing.JFileChooser jFileCh;

    jFileCh = new javax.swing.JFileChooser(tUnitPath);

    jFileCh.setFileSelectionMode(jFileCh.DIRECTORIES_ONLY);
    if ( jFileCh.showOpenDialog(jButtonBrowsePath) ==  jFileCh.APPROVE_OPTION ) {
        dir = jFileCh.getSelectedFile().getAbsolutePath();
        jTxtFieldUnitPath.setText (dir);
        tUnitPath = dir;
        selUnitFiles();
    }
  }//GEN-LAST:event_jButtonBrowsePathActionPerformed

private void jBtnUnitEditorActionPerformed (java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jBtnUnitEditorActionPerformed
    // Add your handling code here:
    int selection = unitFileList.getSelectedIndex();
    if (selection >= 0) {
        String selName = unitFile[selection];
        new UnitEditor (tUnitPath,selName,tUnit.LOAD,new UnitEditor("","",0,0).DISPOSE).show ();
    }
  }//GEN-LAST:event_jBtnUnitEditorActionPerformed

/** Exit the Application */
  private void exitForm(java.awt.event.WindowEvent evt) {//GEN-FIRST:event_exitForm
    output = new SgStream(CfgFile,output.STREAM_WRITE);
    if (output.error != 1) {
        output.writePChar(tUnitPath);
    }
    System.exit (0);
  }//GEN-LAST:event_exitForm

private void selUnitFiles() {
    String AllFiles[];
    String ext;
    java.io.File f;
    int i = 0;

    f = new java.io.File(tUnitPath);
    AllFiles = f.list();

    for (int j = 0; j < AllFiles.length; j ++) {
        ext = getExtension(AllFiles[j]);
        if (ext != null)
        if (ext.compareTo("veh") == 0) {
            i++;
        }
    }

    unitFile = new String[i];

    i=0;

    for (int j = 0; j < AllFiles.length; j ++) {
        ext = getExtension(AllFiles[j]);
        if (ext != null)
        if (ext.compareTo("veh") == 0) {
            unitFile[i] = AllFiles[j];
            i++;
        }
    }
    unitFileList.setListData(unitFile);
}

public String getExtension(String filename) {
    int i = filename.lastIndexOf('.');
    if(i>0 && i<filename.length()-1) {
        return filename.substring(i+1).toLowerCase();
    };
    return null;
}

private int checkPath(String path) {
    java.io.File cFile = new java.io.File(path);

    if (cFile.isDirectory() == true) return 0;
    else return 1;
}

/**
 * @param args the command line arguments
 */
public static void main (String args[]) {
    new MainWindow ().show ();
}


// Variables declaration - do not modify//GEN-BEGIN:variables
private javax.swing.JTabbedPane jTabbedPaneMain;
private javax.swing.JPanel jPanelUnits;
private javax.swing.JTextField jTxtFieldUnitPath;
private javax.swing.JButton jButtonEditUnit;
private javax.swing.JButton jButtonBrowsePath;
private javax.swing.JScrollPane jScrollPaneUnitFiles;
private javax.swing.JButton jButtonNewUnit;
private javax.swing.JPanel jPanelTechnologies;
private javax.swing.JLabel jLabelErrorMessage;
// End of variables declaration//GEN-END:variables
public Unit tUnit;
}