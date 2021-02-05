/*=========================================================================
Program:   Assembler
Module:    appDecl.h
Language:  C++
Date:      $Date: 2021-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2021 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __appDecl_H__
#define __appDecl_H__

#include "appDefines.h"
#include "albaDecl.h"

//----------------------------------------------------------------------------
// Class Name: appDecl
//----------------------------------------------------------------------------

enum APP_MAIN_EVENT_ID
{
	ID_SELECTED_PROSTHESIS = MED_EVT_USER_START,
	MENU_VIEW_TASKBAR,
};

enum APP_MENU_ID
{
  OP_IMPORTER_DICOM = MENU_VIEW_USER_FIRST,
	OP_LAST,

	ID_SHOW_IMAGE_VIEW,
	ID_SHOW_SURFACE_VIEW,

	ID_VIEW_TOOLBAR,
	ID_OPERATIONS_TOOLBAR,
	ID_EXTRA_TOOLBAR,
};

enum APP_VIEW_ID
{
	VIEW_IMAGE = VIEW_START,
	VIEW_SURFACE,
};

#endif