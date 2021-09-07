/*=========================================================================
Program:   AssemblerPro
Module:    appGUIDialogMatrix.h
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

#ifndef __appGUIDialogMatrix_H__
#define __appGUIDialogMatrix_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaGUIDialog.h"
#include "albaMatrix.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class albaMatrix;
class albaGUIButton;
class albaGUILab;
class albaProDBComponent;

class ALBA_EXPORT appGUIDialogMatrix : public albaGUIDialog
{
public:
	appGUIDialogMatrix(const wxString& title, long style = albaCLOSEWINDOW);
	virtual ~appGUIDialogMatrix();

	void OnEvent(albaEventBase *alba_event);

	void SetComponent(albaProDBComponent *component);;
	albaProDBComponent* GetComponent() { return m_CurrentComponent; };

	void Show();

	bool OkClosed() { return m_IsChanged; };

protected:

	void CreateMatrixDialog();
	void UpdateMatrixDialog();

	albaProDBComponent *m_CurrentComponent;
	albaMatrix m_ComponentMatrix;
	bool m_IsChanged;
	
	wxString m_TextValues[4][4];

	albaGUI *m_Gui; ///< Gui variable used to plug custom widgets
	albaGUIButton *m_OkBtn;
};
#endif
