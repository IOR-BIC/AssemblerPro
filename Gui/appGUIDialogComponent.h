/*=========================================================================
Program:   AssemblerPro
Module:    appGUIDialogComponent.h
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

#ifndef __appGUIDialogComponent_H__
#define __appGUIDialogComponent_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaGUIDialog.h"
#include "appOpCreateProsthesis.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class albaGUIButton;
class albaGUILab;
class albaGUIPicButton;

class ALBA_EXPORT appGUIDialogComponent : public albaGUIDialog
{
public:
	appGUIDialogComponent(const wxString& title, long style = albaCLOSEWINDOW);
	virtual ~appGUIDialogComponent();

  void OnEvent(albaEventBase *alba_event);
	
	void SetComponent(Component *component) { m_CurrentComponent = *component; };
	Component GetComponent() { return m_CurrentComponent; };

	void Show();

protected:

	void CreateComponentDialog();
	void UpdateComponentDialog();

	Component m_CurrentComponent;

  albaGUI *m_Gui; ///< Gui variable used to plug custom widgets

	wxTextCtrl *m_ComponentName_textCtrl;
};
#endif
