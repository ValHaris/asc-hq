//
// C++ Interface: guidimension
//
// Description: 
//
//
// Author: Martin Bickel <bickel@asc-hq.org>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef GUIDIMENSION_H
#define GUIDIMENSION_H


/**
@author Martin Bickel
*/
class GuiDimension{
public:
    GuiDimension();
    
    static int getLeftIndent();
    static int getTopOffSet();
    static int getLeftOffSet();
    
    static int getLineEditWidth();
    static int getLineOffset();
    static int getButtonHeight();
    static int getButtonWidth();
    
    static int getSliderWidth();


    ~GuiDimension();

};

#endif



