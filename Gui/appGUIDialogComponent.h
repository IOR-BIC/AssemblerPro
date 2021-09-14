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

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class albaGUIButton;
class albaGUILab;
class albaProDBComponent;
class albaVMEStorage;

class ALBA_EXPORT appGUIDialogComponent : public albaGUIDialog
{
public:
	appGUIDialogComponent(const wxString& title, long style = albaCLOSEWINDOW);
	virtual ~appGUIDialogComponent();

	void OnEvent(albaEventBase *alba_event);

	void SetComponent(albaProDBComponent *component);;
	albaProDBComponent* GetComponent() { return m_CurrentComponent; };

	void Show();

	bool OkClosed() { return m_IsChanged; };

protected:

	void CreateComponentDialog();
	void UpdateComponentDialog();

	void AddVTKFromFile();
	void AddVTKFromTree(albaVME *node);

	static bool VTKDataAccept(albaVME* node) { return(node != NULL /*&& node->IsALBAType(albaVMELandmarkCloud)*/); };

	albaProDBComponent *m_CurrentComponent;
	wxString m_ComponentName;
	bool m_HasVtkData;
	bool m_IsChanged;

	albaGUI *m_Gui; ///< Gui variable used to plug custom widgets
	wxTextCtrl *m_ComponentName_textCtrl;
	wxTextCtrl *m_VtkDataTextCtrl;
	albaGUIButton *m_OkBtn;
};
#endif
