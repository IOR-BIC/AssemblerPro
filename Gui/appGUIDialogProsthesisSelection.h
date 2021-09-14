/*=========================================================================
Program:   AssemblerPro
Module:    appGUIDialogProsthesisSelection.h
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

#ifndef __appGUIDialogProsthesisSelection_H__
#define __appGUIDialogProsthesisSelection_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaGUIDialog.h"
#include "appOpCreateProsthesis.h"
#include "albaProsthesesDBManager.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class albaGUIButton;
class albaGUILab;
class albaGUIPicButton;
class appGUIHyperLink;

class ALBA_EXPORT appGUIDialogProsthesisSelection : public albaGUIDialog
{
public:

	appGUIDialogProsthesisSelection(const wxString& title, long style = albaCLOSEWINDOW);
	virtual ~appGUIDialogProsthesisSelection();

	void OnEvent(albaEventBase *alba_event);

	void SelectImage();

	void Show();

protected:

	void CreateDialog();
	void UpdateDialog();

	albaProDBProducer m_CurrentProducer;
	albaProDBProsthesis m_CurrentProsthesis;

	albaGUI *m_Gui; ///< Gui variable used to plug custom widgets
	
	wxBoxSizer *m_MainBoxSizer;
	wxBoxSizer *m_ImageSizer;
	albaGUIPicButton *m_ProducerImageButton;

	wxTextCtrl *m_ProducerName_textCtrl;
};
#endif
