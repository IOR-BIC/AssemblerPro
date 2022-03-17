/*=========================================================================
Program:   AssemblerPro
Module:    appGUI.h
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

#ifndef __appGUI_h
#define __appGUI_h

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "appDecl.h"
#include "appGuiDefines.h"

#include "albaGUI.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class albaEventBase;
class albaGUIValidator;
class albaInteractorCompositorMouse;
class appGUIComponent;
class albaGUITransformMouseFloatVME;
class appInteractionVolume;
class albaInteractorGenericMouseFloatVME;

//----------------------------------------------------------------------------
// Class Name: appGUI
//----------------------------------------------------------------------------
class APP_GUI_EXPORT appGUI : public albaGUI
{
public:

	appGUI(albaObserver *listener);
	~appGUI();

	void OnDClickList(wxCommandEvent& event);

	/*Create HyperLink*/
	void HyperLink(int id, wxString label, wxString url);

	/* Create Separator. Style 0 = empty space, 1 = line, 2 = empty space + line */
	void Separator(long style, const wxSize& size = wxDefaultSize);

	/** Integer entry widget. */
	void TwoIntegers(int id, albaString label, int *var1, int *var2, int min = MININT, int max = MAXINT, albaString tooltip = "", bool labelAlwaysEnable = false, double customSizer = 1.0);

	/** Component widget (Show, Name, Delete). */
	void ComponentButton(int id1, int id2, int id3, int* boolVar, wxString* textVar);

private:
	DECLARE_EVENT_TABLE()
};

#endif
