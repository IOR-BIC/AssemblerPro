/*=========================================================================
Program:   AssemblerPro
Module:    appGUIDialogProsthesis.h
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

#ifndef __appGUIDialogProsthesis_H__
#define __appGUIDialogProsthesis_H__

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

class ALBA_EXPORT appGUIDialogProsthesis : public albaGUIDialog
{
public:
	appGUIDialogProsthesis(const wxString& title, long style = albaCLOSEWINDOW);
	virtual ~appGUIDialogProsthesis();

	void OnEvent(albaEventBase *alba_event);

	void SetProsthesis(Prosthesis &model);;
	Prosthesis GetProsthesis() { return m_CurrentProsthesis; };

	void Show();

protected:

	void CreateDialog();
	void UpdateDialog();

	void EditComponentGroup();
	void AddComponentGroup();
	void RemoveComponentGroup();

	void SelectImage();

	Prosthesis m_CurrentProsthesis;

	albaGUI *m_Gui; ///< Gui variable used to plug custom widgets

	wxBoxSizer *m_MainBoxSizer;
	wxBoxSizer *m_ImageSizer;

	wxComboBox *m_ImageComboBox;
	wxImage	*m_Image;
	albaGUIPicButton *m_ImageButton;
	wxString m_ImagePath;

	wxTextCtrl *m_Name_textCtrl;

	std::vector<wxString> m_ProducerNameList;
	int m_SelectedProducer;
	int m_SelectedType;
	int m_SelectedSide;

	wxComboBox *m_ComponentGroupCombo;
	std::vector<wxString> m_ComponentNameList;
	int m_SelectedComponentGroup;

};
#endif
