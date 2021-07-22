/*=========================================================================
Program:   AssemblerPro
Module:    appGUIDialogModel.h
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

#ifndef __appGUIDialogModel_H__
#define __appGUIDialogModel_H__

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

class ALBA_EXPORT appGUIDialogModel : public albaGUIDialog
{
public:
	appGUIDialogModel(const wxString& title, long style = albaCLOSEWINDOW);
	virtual ~appGUIDialogModel();

  void OnEvent(albaEventBase *alba_event);
	
	void SetModel(Model *model) { m_CurrentModel = *model; };
	Model GetModel() { return m_CurrentModel; };

	void Show();

protected:

	void CreateModelDialog();
	void UpdateModelDialog();

	Model m_CurrentModel;

  albaGUI *m_Gui; ///< Gui variable used to plug custom widgets

	wxComboBox *m_ModelImageComboBox;
	wxImage	*m_ModelImage;
	albaGUIPicButton *m_ModelImageButton;

	wxTextCtrl *m_ModelName_textCtrl;
};
#endif
