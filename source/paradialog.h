/***************************************************************************
                          paradialog.h  -  description
                             -------------------
    begin                : Thu Feb 21 2002
    copyright            : (C) 2002 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/*! \file paradialog.h
    \brief Dialog classes based on the Paragui library
*/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef paradialog_h_included
 #define paradialog_h_included

#include "global.h"

#include <paragui.h>
#include <pgapplication.h>
#include <pgmessagebox.h>
#include <pgdropdown.h>
#include "pgbutton.h"
#include "pgwidgetlist.h"
#include "pglabel.h"
#include "pgwindow.h"
#include "pgmaskedit.h"
#include "pgscrollbar.h"
#include "pgprogressbar.h"
#include "pgradiobutton.h"
#include "pgthemewidget.h"
#include "pgcheckbutton.h"
#include "pgslider.h"
#include "pglistbox.h"
#include "pgcolumnitem.h"
#include "pgeventobject.h"
#include "pgpopupmenu.h"
#include "pgspinnerbox.h"
#include "pglog.h"
#include "pgmenubar.h"

#include "sdl/graphics.h"

 class ASC_PG_App : public PG_Application {
    public:
       ASC_PG_App ();
      // PG_Theme* LoadTheme(const char* xmltheme, bool asDefault = true, const char* searchpath = NULL );
 };

 extern void soundSettings();

 extern void setupMainScreenWidget();
 
 extern ASC_PG_App* pgApp;

#endif